#include <userver/storages/clickhouse/io/columns/map_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/array.h>
#include <clickhouse/columns/tuple.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using ArrayNativeType = clickhouse::impl::clickhouse_cpp::ColumnArray;
using TupleNativeType = clickhouse::impl::clickhouse_cpp::ColumnTuple;
}  // namespace

template <typename K, typename V>
MapColumn<K, V>::MapColumn(ColumnRef column)
    : ClickhouseColumn<MapColumn<K, V>>{impl::GetTypedColumn<MapColumn<K, V>, ArrayNativeType>(column)} {}

template <typename K, typename V>
typename MapColumn<K, V>::MapDataHolder MapColumn<K, V>::MapDataHolder::operator++(int) {
    MapDataHolder old{};
    old.inner_ = inner_;
    old.ind_ = ind_++;
    old.current_value_ = std::move_if_noexcept(current_value_);
    current_value_.reset();
    return old;
}

template <typename K, typename V>
typename MapColumn<K, V>::MapDataHolder& MapColumn<K, V>::MapDataHolder::operator++() {
    ++ind_;
    current_value_.reset();
    return *this;
}

template <typename K, typename V>
typename MapColumn<K, V>::cpp_type& MapColumn<K, V>::MapDataHolder::UpdateValue() {
    UASSERT(ind_ < GetColumnSize(inner_));
    if (!current_value_.has_value()) {
        cpp_type item = RetrieveElement(inner_, ind_);
        current_value_.emplace(std::move(item));
    }
    return *current_value_;
}

template <typename K, typename V>
bool MapColumn<K, V>::MapDataHolder::operator==(const MapDataHolder& other) const {
    return inner_.get() == other.inner_.get() && ind_ == other.ind_;
}

template <typename K, typename V>
MapColumn<K, V>::MapDataHolder::MapDataHolder(
    typename ColumnIterator<MapColumn<K, V>>::IteratorPosition iter_position,
    ColumnRef&& column
)
    : inner_{std::move(column)}, ind_(iter_position == decltype(iter_position)::kEnd ? GetColumnSize(inner_) : 0) {}

// Retrieve a single row as vector<pair<K,V>> from Array(Tuple(K,V)) native column
template <typename K, typename V>
typename MapColumn<K, V>::cpp_type MapColumn<K, V>::RetrieveElement(const ColumnRef& ref, std::size_t ind) {
    auto array_native = ref->As<ArrayNativeType>();
    auto row_tuple_col = array_native->GetAsColumn(ind)->As<TupleNativeType>();

    K key_col(row_tuple_col->At(0));
    V val_col(row_tuple_col->At(1));

    const auto row_size = GetColumnSize(row_tuple_col->At(0));
    cpp_type result;
    result.reserve(row_size);
    auto key_it = key_col.begin();
    auto val_it = val_col.begin();
    for (std::size_t i = 0; i < row_size; ++i, ++key_it, ++val_it) {
        result.emplace_back(std::move(*key_it), std::move(*val_it));
    }
    return result;
}

// Serialize vector<vector<pair<K,V>>> to Array(Tuple(K,V))
template <typename K, typename V>
ColumnRef MapColumn<K, V>::Serialize(const container_type& from) {
    using UInt64NativeType = clickhouse::impl::clickhouse_cpp::ColumnUInt64;

    uint64_t cumulative_offset = 0;
    std::vector<uint64_t> offsets;
    offsets.reserve(from.size());

    typename K::container_type key_values;
    typename V::container_type val_values;

    for (const auto& row : from) {
        cumulative_offset += row.size();
        offsets.push_back(cumulative_offset);
        for (const auto& kv : row) {
            key_values.push_back(kv.first);
            val_values.push_back(kv.second);
        }
    }

    auto key_col = K::Serialize(key_values);
    auto val_col = V::Serialize(val_values);
    auto tuple_col = std::make_shared<TupleNativeType>(std::vector<ColumnRef>{key_col, val_col});
    auto offsets_native = std::make_shared<UInt64NativeType>(offsets);
    return std::make_shared<ArrayNativeType>(tuple_col, offsets_native);
}

// Explicit instantiations are not provided; users will instantiate with desired K,V wrappers

// Force template emission for common combinations if needed in the future.

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

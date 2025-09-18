#include <userver/storages/clickhouse/io/columns/fixed_string_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/string.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnFixedString;
}

template <std::size_t N>
FixedStringColumn<N>::FixedStringColumn(ColumnRef column)
    : ClickhouseColumn<FixedStringColumn<N>>{impl::GetTypedColumn<FixedStringColumn<N>, NativeType>(column)} {}

template <std::size_t N>
typename ColumnIterator<FixedStringColumn<N>>::value_type
ColumnIterator<FixedStringColumn<N>>::DataHolder::Get() const {
    return std::string{impl::NativeGetAt<NativeType>(column_, ind_)};
}

template <std::size_t N>
ColumnRef FixedStringColumn<N>::Serialize(const container_type& from) {
    auto col = std::make_shared<NativeType>(N);
    col->Reserve(from.size());
    for (const auto& v : from) col->Append(v);
    return col;
}

// Explicit instantiations may be added as needed

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

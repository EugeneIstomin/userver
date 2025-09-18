#include <userver/storages/clickhouse/io/columns/enum_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/enum.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
template <typename Int>
struct NativeEnum;

template <>
struct NativeEnum<std::int8_t> {
    using type = clickhouse::impl::clickhouse_cpp::ColumnEnum8;
};
template <>
struct NativeEnum<std::int16_t> {
    using type = clickhouse::impl::clickhouse_cpp::ColumnEnum16;
};
}  // namespace

template <typename Int>
EnumColumn<Int>::EnumColumn(ColumnRef column)
    : ClickhouseColumn<EnumColumn<Int>>{impl::GetTypedColumn<EnumColumn<Int>, typename NativeEnum<Int>::type>(column)} {
}

template <typename Int>
typename ColumnIterator<EnumColumn<Int>>::value_type ColumnIterator<EnumColumn<Int>>::DataHolder::Get() const {
    return impl::NativeGetAt<typename NativeEnum<Int>::type>(column_, ind_);
}

template <typename Int>
ColumnRef EnumColumn<Int>::Serialize(const container_type& from) {
    return std::make_shared<typename NativeEnum<Int>::type>(from);
}

// Explicit instantiations
template class EnumColumn<std::int8_t>;
template class EnumColumn<std::int16_t>;

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

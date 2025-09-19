#include <userver/storages/clickhouse/io/columns/int128_column.hpp>

#include <storages/clickhouse/io/columns/impl/numeric_column.hpp>

#include <clickhouse/columns/numeric.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnInt128;
}

Int128Column::Int128Column(ColumnRef column)
    : ClickhouseColumn<Int128Column>{impl::GetTypedColumn<Int128Column, NativeType>(column)} {}

template <>
auto ColumnIterator<Int128Column>::DataHolder::Get() const -> Int128Column::cpp_type {
    return impl::NativeGetAt<NativeType>(column_, ind_);
}

ColumnRef Int128Column::Serialize(const container_type& from) { return std::make_shared<NativeType>(from); }

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

#include <userver/storages/clickhouse/io/columns/uint128_column.hpp>

#include <storages/clickhouse/io/columns/impl/numeric_column.hpp>

#include <clickhouse/columns/numeric.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnUInt128;
}

UInt128Column::UInt128Column(ColumnRef column)
    : ClickhouseColumn<UInt128Column>{impl::GetTypedColumn<UInt128Column, NativeType>(column)} {}

template <>
auto ColumnIterator<UInt128Column>::DataHolder::Get() const -> UInt128Column::cpp_type {
    return impl::NativeGetAt<NativeType>(column_, ind_);
}

ColumnRef UInt128Column::Serialize(const container_type& from) { return std::make_shared<NativeType>(from); }

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

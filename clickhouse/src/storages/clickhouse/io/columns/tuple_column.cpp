#include <userver/storages/clickhouse/io/columns/tuple_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/tuple.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnTuple;
}

TupleColumn::TupleColumn(ColumnRef column) : ClickhouseColumn{impl::GetTypedColumn<TupleColumn, NativeType>(column)} {}

template <>
TupleColumn::cpp_type ColumnIterator<TupleColumn>::DataHolder::Get() const {
    // There is no direct vector-of-ColumnRef representation in user API; return empty for now
    return {};
}

ColumnRef TupleColumn::Serialize(const container_type& from) {
    // Build tuple from provided columns per row is non-trivial; not supported in high-level API
    // Expect callers to prebuild a ColumnTuple in native form and pass through; keep empty to avoid misuse
    return std::make_shared<NativeType>(std::vector<ColumnRef>{});
}

ColumnRef TupleGetSubcolumn(const ColumnRef& tuple_column, std::size_t index) {
    auto native = tuple_column->As<NativeType>();
    if (!native) return {};
    return (*native)[index];
}

ColumnRef MakeTupleColumn(const std::vector<ColumnRef>& columns) { return std::make_shared<NativeType>(columns); }

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

#include <userver/storages/clickhouse/io/columns/map_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/map.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnMap;
}

MapColumn::MapColumn(ColumnRef column) : ClickhouseColumn{impl::GetTypedColumn<MapColumn, NativeType>(column)} {}

namespace {
using ArrayNative = clickhouse::impl::clickhouse_cpp::ColumnArray;
using TupleNative = clickhouse::impl::clickhouse_cpp::ColumnTuple;
}  // namespace

// Extract (keys, values) subcolumns from ColumnMap's underlying array of tuple
static std::pair<ColumnRef, ColumnRef> ExtractKeyValueFromMapArray(const ColumnRef& map_array) {
    auto array_native = map_array->As<ArrayNative>();
    auto tuple_col = array_native->GetData()->As<TupleNative>();
    auto keys = (*tuple_col)[0];
    auto values = (*tuple_col)[1];
    return {keys, values};
}

static std::pair<ColumnRef, ColumnRef> ExtractKeyValueFromTupleArray(const ColumnRef& tuple_array) {
    auto array_native = tuple_array->As<ArrayNative>();
    auto tuple_col = array_native->GetData()->As<TupleNative>();
    auto keys = (*tuple_col)[0];
    auto values = (*tuple_col)[1];
    return {keys, values};
}

// Bridge for header declaration of MapColumnT::MapValueView::ExtractKeyValueColumns
std::pair<ColumnRef, ColumnRef> MapExtractKeyValueColumnsImpl(const ColumnRef& tuple_array) {
    return ExtractKeyValueFromTupleArray(tuple_array);
}

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

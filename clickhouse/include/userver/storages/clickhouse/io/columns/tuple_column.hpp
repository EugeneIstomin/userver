#pragma once

/// @file userver/storages/clickhouse/io/columns/tuple_column.hpp
/// @brief Tuple column support
/// @ingroup userver_clickhouse_types

#include <tuple>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

class TupleColumn final : public ClickhouseColumn<TupleColumn> {
public:
    using cpp_type = std::vector<ColumnRef>;
    using container_type = std::vector<cpp_type>;

    TupleColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

// Helper functions implemented in .cpp to avoid exposing native headers
ColumnRef TupleGetSubcolumn(const ColumnRef& tuple_column, std::size_t index);
ColumnRef MakeTupleColumn(const std::vector<ColumnRef>& columns);

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

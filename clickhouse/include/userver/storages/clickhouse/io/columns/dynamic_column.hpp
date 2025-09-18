#pragma once

/// @file userver/storages/clickhouse/io/columns/dynamic_column.hpp
/// @brief Dynamic column support
/// @ingroup userver_clickhouse_types

#include <string>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse Dynamic column
class DynamicColumn final : public ClickhouseColumn<DynamicColumn> {
public:
    using cpp_type = std::string;
    using container_type = std::vector<cpp_type>;

    DynamicColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

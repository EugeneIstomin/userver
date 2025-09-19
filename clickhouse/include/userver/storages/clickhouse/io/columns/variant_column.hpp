#pragma once

/// @file userver/storages/clickhouse/io/columns/variant_column.hpp
/// @brief Variant column support
/// @ingroup userver_clickhouse_types

#include <string>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse Variant column
class VariantColumn final : public ClickhouseColumn<VariantColumn> {
public:
    using cpp_type = std::string;
    using container_type = std::vector<cpp_type>;

    VariantColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

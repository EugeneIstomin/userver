#pragma once

/// @file userver/storages/clickhouse/io/columns/enum_column.hpp
/// @brief Enum8/Enum16 column support
/// @ingroup userver_clickhouse_types

#include <cstdint>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse Enum8 column
class Enum8Column final : public ClickhouseColumn<Enum8Column> {
public:
    using cpp_type = std::int8_t;
    using container_type = std::vector<cpp_type>;

    explicit Enum8Column(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

/// @brief Represents ClickHouse Enum16 column
class Enum16Column final : public ClickhouseColumn<Enum16Column> {
public:
    using cpp_type = std::int16_t;
    using container_type = std::vector<cpp_type>;

    explicit Enum16Column(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

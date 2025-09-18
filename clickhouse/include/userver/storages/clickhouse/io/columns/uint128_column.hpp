#pragma once

/// @file userver/storages/clickhouse/io/columns/uint128_column.hpp
/// @brief UInt128 column support
/// @ingroup userver_clickhouse_types

#include <absl/numeric/int128.h>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse UInt128 Column
class UInt128Column final : public ClickhouseColumn<UInt128Column> {
public:
    using cpp_type = absl::uint128;
    using container_type = std::vector<cpp_type>;

    UInt128Column(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

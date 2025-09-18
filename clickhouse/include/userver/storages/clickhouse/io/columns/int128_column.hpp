#pragma once

/// @file userver/storages/clickhouse/io/columns/int128_column.hpp
/// @brief Int128 column support
/// @ingroup userver_clickhouse_types

#include <absl/numeric/int128.h>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse Int128 Column
class Int128Column final : public ClickhouseColumn<Int128Column> {
public:
    using cpp_type = absl::int128;
    using container_type = std::vector<cpp_type>;

    Int128Column(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END



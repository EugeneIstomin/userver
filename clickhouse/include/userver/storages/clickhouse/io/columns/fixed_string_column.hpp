#pragma once

/// @file userver/storages/clickhouse/io/columns/fixed_string_column.hpp
/// @brief FixedString(N) column support
/// @ingroup userver_clickhouse_types

#include <cstddef>
#include <string>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse FixedString(N) column
template <std::size_t N>
class FixedStringColumn final : public ClickhouseColumn<FixedStringColumn<N>> {
public:
    using cpp_type = std::string;
    using container_type = std::vector<cpp_type>;

    explicit FixedStringColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

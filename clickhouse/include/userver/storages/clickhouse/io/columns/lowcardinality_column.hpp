#pragma once

/// @file userver/storages/clickhouse/io/columns/lowcardinality_column.hpp
/// @brief LowCardinality column helpers
/// @ingroup userver_clickhouse_types

#include <string>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

class LowCardinalityStringColumn final : public ClickhouseColumn<LowCardinalityStringColumn> {
public:
    using cpp_type = std::string;
    using container_type = std::vector<cpp_type>;

    LowCardinalityStringColumn(ColumnRef column);
    static ColumnRef Serialize(const container_type& from);
};

template <std::size_t N>
class LowCardinalityFixedStringColumn final : public ClickhouseColumn<LowCardinalityFixedStringColumn<N>> {
public:
    using cpp_type = std::string;
    using container_type = std::vector<cpp_type>;

    LowCardinalityFixedStringColumn(ColumnRef column);
    static ColumnRef Serialize(const container_type& from);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

#pragma once

/// @file userver/storages/clickhouse/io/columns/enum_column.hpp
/// @brief Enum8/Enum16 column support
/// @ingroup userver_clickhouse_types

#include <cstdint>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

template <typename Int>
class EnumColumn final : public ClickhouseColumn<EnumColumn<Int>> {
public:
    using cpp_type = Int;
    using container_type = std::vector<cpp_type>;

    EnumColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

using Enum8Column = EnumColumn<std::int8_t>;
using Enum16Column = EnumColumn<std::int16_t>;

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

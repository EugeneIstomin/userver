#pragma once

/// @file userver/storages/clickhouse/io/columns/decimal_column.hpp
/// @brief Decimal32/64/128 column support
/// @ingroup userver_clickhouse_types

#include <absl/numeric/int128.h>
#include <cstddef>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

template <std::size_t Precision, std::size_t Scale>
class DecimalColumn final : public ClickhouseColumn<DecimalColumn<Precision, Scale>> {
public:
    using cpp_type = absl::int128;
    using container_type = std::vector<cpp_type>;

    DecimalColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
};

template <std::size_t S>
using Decimal32Column = DecimalColumn<9, S>;
template <std::size_t S>
using Decimal64Column = DecimalColumn<18, S>;
template <std::size_t S>
using Decimal128Column = DecimalColumn<38, S>;

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

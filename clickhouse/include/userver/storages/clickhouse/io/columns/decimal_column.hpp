#pragma once

/// @file userver/storages/clickhouse/io/columns/decimal_column.hpp
/// @brief Decimal32/64/128 column support
/// @ingroup userver_clickhouse_types

#include <absl/numeric/int128.h>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse Decimal32(S) column
class Decimal32Column final : public ClickhouseColumn<Decimal32Column> {
public:
    using cpp_type = absl::int128;  // scaled integer value
    using container_type = std::vector<cpp_type>;

    explicit Decimal32Column(ColumnRef column);

    // Serialize with scale=0 by default; prefer the overload with explicit scale.
    static ColumnRef Serialize(const container_type& from);
    static ColumnRef Serialize(const container_type& from, std::size_t scale);

    // Optional range-checked serialization: throws on overflow when narrowing to int32.
    static ColumnRef SerializeChecked(const container_type& from, std::size_t scale);

    // Access current column scale if constructed from a native column.
    std::size_t GetScale() const;
};

/// @brief Represents ClickHouse Decimal64(S) column
class Decimal64Column final : public ClickhouseColumn<Decimal64Column> {
public:
    using cpp_type = absl::int128;  // scaled integer value
    using container_type = std::vector<cpp_type>;

    explicit Decimal64Column(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
    static ColumnRef Serialize(const container_type& from, std::size_t scale);

    // Optional range-checked serialization: throws on overflow when narrowing to int64.
    static ColumnRef SerializeChecked(const container_type& from, std::size_t scale);

    // Access current column scale if constructed from a native column.
    std::size_t GetScale() const;
};

/// @brief Represents ClickHouse Decimal128(S) column
class Decimal128Column final : public ClickhouseColumn<Decimal128Column> {
public:
    using cpp_type = absl::int128;  // scaled integer value
    using container_type = std::vector<cpp_type>;

    explicit Decimal128Column(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);
    static ColumnRef Serialize(const container_type& from, std::size_t scale);

    // For Decimal128, no narrowing occurs; kept for API parity.
    static ColumnRef SerializeChecked(const container_type& from, std::size_t scale);

    // Access current column scale if constructed from a native column.
    std::size_t GetScale() const;
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

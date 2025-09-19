#pragma once

/// @file userver/storages/clickhouse/io/columns/common_columns.hpp
/// Helper file to include every implemented column (except for Nullable)

#include <userver/storages/clickhouse/io/columns/datetime64_column.hpp>
#include <userver/storages/clickhouse/io/columns/datetime_column.hpp>
#include <userver/storages/clickhouse/io/columns/float32_column.hpp>
#include <userver/storages/clickhouse/io/columns/float64_column.hpp>
#include <userver/storages/clickhouse/io/columns/int32_column.hpp>
#include <userver/storages/clickhouse/io/columns/int64_column.hpp>
#include <userver/storages/clickhouse/io/columns/int8_column.hpp>
#include <userver/storages/clickhouse/io/columns/string_column.hpp>
#include <userver/storages/clickhouse/io/columns/uint16_column.hpp>
#include <userver/storages/clickhouse/io/columns/uint32_column.hpp>
#include <userver/storages/clickhouse/io/columns/uint64_column.hpp>
#include <userver/storages/clickhouse/io/columns/uint8_column.hpp>
#include <userver/storages/clickhouse/io/columns/uuid_column.hpp>
// NEW
#include <userver/storages/clickhouse/io/columns/dynamic_column.hpp>
#include <userver/storages/clickhouse/io/columns/enum_column.hpp>
#include <userver/storages/clickhouse/io/columns/fixed_string_column.hpp>
#include <userver/storages/clickhouse/io/columns/int128_column.hpp>
#include <userver/storages/clickhouse/io/columns/json_column.hpp>
#include <userver/storages/clickhouse/io/columns/lowcardinality_column.hpp>
#include <userver/storages/clickhouse/io/columns/map_column.hpp>
#include <userver/storages/clickhouse/io/columns/tuple_column.hpp>
#include <userver/storages/clickhouse/io/columns/uint128_column.hpp>
#include <userver/storages/clickhouse/io/columns/variant_column.hpp>

// #include <userver/storages/clickhouse/io/columns/decimal_column.hpp>

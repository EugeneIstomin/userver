#include <userver/storages/clickhouse/io/columns/decimal_column.hpp>

#include <limits>
#include <stdexcept>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <storages/clickhouse/impl/decimal_native.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using Native32 = storages::clickhouse::impl::decimal_native::ColumnDecimal32;
using Native64 = storages::clickhouse::impl::decimal_native::ColumnDecimal64;
using Native128 = storages::clickhouse::impl::decimal_native::ColumnDecimal128;
}  // namespace

Decimal32Column::Decimal32Column(ColumnRef column)
    : ClickhouseColumn<Decimal32Column>{impl::GetTypedColumn<Decimal32Column, Native32>(column)} {}

template <>
auto ColumnIterator<Decimal32Column>::DataHolder::Get() const -> Decimal32Column::cpp_type {
    return static_cast<absl::int128>(impl::NativeGetAt<Native32>(column_, ind_));
}

ColumnRef Decimal32Column::Serialize(const container_type& from) { return Serialize(from, 0); }

ColumnRef Decimal32Column::Serialize(const container_type& from, std::size_t scale) {
    auto col = std::make_shared<Native32>(scale);
    col->Reserve(from.size());
    for (const auto& v : from) col->AppendRaw(static_cast<int32_t>(v));
    return col;
}

ColumnRef Decimal32Column::SerializeChecked(const container_type& from, std::size_t scale) {
    auto col = std::make_shared<Native32>(scale);
    col->Reserve(from.size());
    constexpr absl::int128 min32 = static_cast<absl::int128>(std::numeric_limits<int32_t>::min());
    constexpr absl::int128 max32 = static_cast<absl::int128>(std::numeric_limits<int32_t>::max());
    for (const auto& v : from) {
        if (v < min32 || v > max32) {
            throw std::overflow_error{"Decimal32 serialize overflow: value out of int32 range"};
        }
        col->AppendRaw(static_cast<int32_t>(v));
    }
    return col;
}

std::size_t Decimal32Column::GetScale() const {
    auto native = GetColumnRef()->As<Native32>();
    return native ? native->GetScale() : 0;
}

Decimal64Column::Decimal64Column(ColumnRef column)
    : ClickhouseColumn<Decimal64Column>{impl::GetTypedColumn<Decimal64Column, Native64>(column)} {}

template <>
auto ColumnIterator<Decimal64Column>::DataHolder::Get() const -> Decimal64Column::cpp_type {
    return static_cast<absl::int128>(impl::NativeGetAt<Native64>(column_, ind_));
}

ColumnRef Decimal64Column::Serialize(const container_type& from) { return Serialize(from, 0); }

ColumnRef Decimal64Column::Serialize(const container_type& from, std::size_t scale) {
    auto col = std::make_shared<Native64>(scale);
    col->Reserve(from.size());
    for (const auto& v : from) col->AppendRaw(static_cast<int64_t>(v));
    return col;
}

ColumnRef Decimal64Column::SerializeChecked(const container_type& from, std::size_t scale) {
    auto col = std::make_shared<Native64>(scale);
    col->Reserve(from.size());
    constexpr absl::int128 min64 = static_cast<absl::int128>(std::numeric_limits<int64_t>::min());
    constexpr absl::int128 max64 = static_cast<absl::int128>(std::numeric_limits<int64_t>::max());
    for (const auto& v : from) {
        if (v < min64 || v > max64) {
            throw std::overflow_error{"Decimal64 serialize overflow: value out of int64 range"};
        }
        col->AppendRaw(static_cast<int64_t>(v));
    }
    return col;
}

std::size_t Decimal64Column::GetScale() const {
    auto native = GetColumnRef()->As<Native64>();
    return native ? native->GetScale() : 0;
}

Decimal128Column::Decimal128Column(ColumnRef column)
    : ClickhouseColumn<Decimal128Column>{impl::GetTypedColumn<Decimal128Column, Native128>(column)} {}

template <>
auto ColumnIterator<Decimal128Column>::DataHolder::Get() const -> Decimal128Column::cpp_type {
    return impl::NativeGetAt<Native128>(column_, ind_);
}

ColumnRef Decimal128Column::Serialize(const container_type& from) { return Serialize(from, 0); }

ColumnRef Decimal128Column::Serialize(const container_type& from, std::size_t scale) {
    auto col = std::make_shared<Native128>(scale);
    col->Reserve(from.size());
    for (const auto& v : from) col->AppendRaw(v);
    return col;
}

ColumnRef Decimal128Column::SerializeChecked(const container_type& from, std::size_t scale) {
    // No narrowing, but keep same API for uniformity.
    return Serialize(from, scale);
}

std::size_t Decimal128Column::GetScale() const {
    auto native = GetColumnRef()->As<Native128>();
    return native ? native->GetScale() : 0;
}

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

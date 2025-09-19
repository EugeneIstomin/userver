#include <userver/storages/clickhouse/io/columns/enum_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/numeric.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeEnum8 = clickhouse::impl::clickhouse_cpp::ColumnEnum8;
using NativeEnum16 = clickhouse::impl::clickhouse_cpp::ColumnEnum16;

// Some servers may expose enums as numeric; provide numeric fallbacks
using NativeInt8 = clickhouse::impl::clickhouse_cpp::ColumnInt8;
using NativeInt16 = clickhouse::impl::clickhouse_cpp::ColumnInt16;
}  // namespace

Enum8Column::Enum8Column(ColumnRef column) : ClickhouseColumn<Enum8Column>{std::move(column)} {}

template <>
auto ColumnIterator<Enum8Column>::DataHolder::Get() const -> Enum8Column::cpp_type {
    if (column_->As<NativeEnum8>() != nullptr) {
        return impl::NativeGetAt<NativeEnum8>(column_, ind_);
    }
    // Fallback for older servers/wrappers: treat as Int8
    return impl::NativeGetAt<NativeInt8>(column_, ind_);
}

ColumnRef Enum8Column::Serialize(const container_type& from) {
    // Serialize as numeric payload; server validates values against enum domain
    return std::make_shared<NativeInt8>(from);
}

Enum16Column::Enum16Column(ColumnRef column) : ClickhouseColumn<Enum16Column>{std::move(column)} {}

template <>
auto ColumnIterator<Enum16Column>::DataHolder::Get() const -> Enum16Column::cpp_type {
    if (column_->As<NativeEnum16>() != nullptr) {
        return impl::NativeGetAt<NativeEnum16>(column_, ind_);
    }
    // Fallback for older servers/wrappers: treat as Int16
    return impl::NativeGetAt<NativeInt16>(column_, ind_);
}

ColumnRef Enum16Column::Serialize(const container_type& from) { return std::make_shared<NativeInt16>(from); }

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

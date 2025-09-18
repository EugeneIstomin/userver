#include <userver/storages/clickhouse/io/columns/json_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <storages/clickhouse/impl/json_like_columns.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = storages::clickhouse::impl::json_like_native::ColumnJSON;
}

JSONColumn::JSONColumn(ColumnRef column) : ClickhouseColumn{impl::GetTypedColumn<JSONColumn, NativeType>(column)} {}

template <>
JSONColumn::cpp_type ColumnIterator<JSONColumn>::DataHolder::Get() const {
    return std::string{impl::NativeGetAt<NativeType>(column_, ind_)};
}

ColumnRef JSONColumn::Serialize(const container_type& from) {
    auto col = std::make_shared<NativeType>();
    col->Reserve(from.size());
    for (const auto& v : from) col->Append(v);
    return col;
}

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

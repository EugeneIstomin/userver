#include <userver/storages/clickhouse/io/columns/fixed_string_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/string.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnFixedString;
}

template <std::size_t N>
FixedStringColumn<N>::FixedStringColumn(ColumnRef column)
    : ClickhouseColumn<FixedStringColumn<N>>{impl::GetTypedColumn<FixedStringColumn<N>, NativeType>(column)} {}

template <std::size_t N>
ColumnRef FixedStringColumn<N>::Serialize(const container_type& from) {
    auto col = std::make_shared<NativeType>(N);
    col->Reserve(from.size());
    for (const auto& v : from) col->Append(v);
    return col;
}

// No explicit instantiations; users can instantiate needed N

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

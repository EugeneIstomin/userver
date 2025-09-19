#include <userver/storages/clickhouse/io/columns/lowcardinality_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/itemview.h>
#include <clickhouse/columns/lowcardinality.h>
#include <clickhouse/columns/string.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeLc = clickhouse::impl::clickhouse_cpp::ColumnLowCardinality;
using NativeStr = clickhouse::impl::clickhouse_cpp::ColumnString;
using NativeFixedStr = clickhouse::impl::clickhouse_cpp::ColumnFixedString;
using NativeLcString = clickhouse::impl::clickhouse_cpp::ColumnLowCardinalityT<NativeStr>;
using NativeLcFixedString = clickhouse::impl::clickhouse_cpp::ColumnLowCardinalityT<NativeFixedStr>;
}  // namespace

LowCardinalityStringColumn::LowCardinalityStringColumn(ColumnRef column)
    : ClickhouseColumn<LowCardinalityStringColumn>{impl::GetTypedColumn<LowCardinalityStringColumn, NativeLc>(column)} {
}

ColumnRef LowCardinalityStringColumn::Serialize(const container_type& from) {
    auto dict = std::make_shared<NativeStr>();
    auto lc = std::make_shared<NativeLcString>(dict);
    lc->Reserve(from.size());
    for (const auto& v : from) lc->Append(v);
    return std::static_pointer_cast<NativeLc>(lc);
}

template <std::size_t N>
LowCardinalityFixedStringColumn<N>::LowCardinalityFixedStringColumn(ColumnRef column)
    : ClickhouseColumn<LowCardinalityFixedStringColumn<N>>{
          impl::GetTypedColumn<LowCardinalityFixedStringColumn<N>, NativeLc>(column)
      } {}

template <std::size_t N>
ColumnRef LowCardinalityFixedStringColumn<N>::Serialize(const container_type& from) {
    auto dict = std::make_shared<NativeFixedStr>(N);
    auto lc = std::make_shared<NativeLcFixedString>(dict);
    lc->Reserve(from.size());
    for (const auto& v : from) lc->Append(v);
    return std::static_pointer_cast<NativeLc>(lc);
}

// No explicit instantiations; users can instantiate needed N

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

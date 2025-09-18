#include <userver/storages/clickhouse/io/columns/decimal_column.hpp>

#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/decimal.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnDecimal;
}

template <std::size_t P, std::size_t S>
DecimalColumn<P, S>::DecimalColumn(ColumnRef column)
    : ClickhouseColumn<DecimalColumn<P, S>>{impl::GetTypedColumn<DecimalColumn<P, S>, NativeType>(column)} {}

template <std::size_t P, std::size_t S>
ColumnRef DecimalColumn<P, S>::Serialize(const container_type& from) {
    auto col = std::make_shared<NativeType>(P, S);
    col->Reserve(from.size());
    for (const auto& v : from) col->Append(v);
    return col;
}

template <std::size_t P, std::size_t S>
typename ColumnIterator<DecimalColumn<P, S>>::value_type ColumnIterator<DecimalColumn<P, S>>::DataHolder::Get() const {
    return impl::NativeGetAt<NativeType>(column_, ind_);
}

// Explicit instantiations for some common scales (0..9) can be added as needed by users

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

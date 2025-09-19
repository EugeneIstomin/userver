#include <userver/storages/clickhouse/io/columns/tuple_column.hpp>

#include <stdexcept>
#include <storages/clickhouse/io/columns/impl/column_includes.hpp>

#include <clickhouse/columns/tuple.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

namespace {
using NativeType = clickhouse::impl::clickhouse_cpp::ColumnTuple;
}

TupleColumn::TupleColumn(ColumnRef column)
    : ClickhouseColumn<TupleColumn>{impl::GetTypedColumn<TupleColumn, NativeType>(column)} {}

template <>
TupleColumn::cpp_type ColumnIterator<TupleColumn>::DataHolder::Get() const {
    // Expose tuple subcolumns for external typed access without leaking native headers
    TupleColumn::cpp_type result;
    auto native = column_->As<NativeType>();
    if (!native) return result;
    const auto tuple_size = native->TupleSize();
    result.reserve(tuple_size);
    for (std::size_t i = 0; i < tuple_size; ++i) {
        result.push_back((*native)[i]);
    }
    return result;
}

ColumnRef TupleColumn::Serialize(const container_type& from) {
    // Two supported modes:
    // 1) from.size() == 0 -> empty tuple
    // 2) from.size() == 1 -> treat from[0] as vector of full subcolumns
    // 3) from.size()  > 1 -> treat as row-wise one-element subcolumns, combine into full subcolumns

    if (from.empty()) {
        return std::make_shared<NativeType>(std::vector<ColumnRef>{});
    }

    const auto num_fields = from.front().size();
    // Validate widths
    for (const auto& row : from) {
        if (row.size() != num_fields) {
            throw std::runtime_error{"TupleColumn::Serialize: rows have different field counts"};
        }
    }

    // Mode 2: prebuilt subcolumns
    if (from.size() == 1) {
        return std::make_shared<NativeType>(from.front());
    }

    // Mode 3: build each field by appending one-row columns
    std::vector<ColumnRef> subcolumns;
    subcolumns.reserve(num_fields);

    // Initialize target subcolumns with empty clones of first row's columns
    for (std::size_t j = 0; j < num_fields; ++j) {
        auto base = from[0][j];
        if (!base) {
            throw std::runtime_error{"TupleColumn::Serialize: null subcolumn in input"};
        }
        auto col = base->CloneEmpty();
        // Reserve approximately number of rows
        col->Reserve(from.size());
        subcolumns.push_back(std::move(col));
    }

    // Append one element per row into each target subcolumn
    for (const auto& row : from) {
        for (std::size_t j = 0; j < num_fields; ++j) {
            // All inputs in this mode must be one-element columns to keep tuple rows aligned
            if (row[j]->Size() != 1) {
                throw std::runtime_error{"TupleColumn::Serialize: expected one-element subcolumns per row"};
            }
            // Basic runtime type check via names
            if (row[j]->Type()->GetName() != subcolumns[j]->Type()->GetName()) {
                throw std::runtime_error{"TupleColumn::Serialize: subcolumn types mismatch across rows"};
            }
            subcolumns[j]->Append(row[j]);
        }
    }

    return std::make_shared<NativeType>(subcolumns);
}

ColumnRef TupleGetSubcolumn(const ColumnRef& tuple_column, std::size_t index) {
    auto native = tuple_column->As<NativeType>();
    if (!native) return {};
    return (*native)[index];
}

ColumnRef MakeTupleColumn(const std::vector<ColumnRef>& columns) { return std::make_shared<NativeType>(columns); }

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

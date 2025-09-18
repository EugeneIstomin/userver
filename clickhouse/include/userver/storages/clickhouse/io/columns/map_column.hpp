#pragma once

/// @file userver/storages/clickhouse/io/columns/map_column.hpp
/// @brief Map column support
/// @ingroup userver_clickhouse_types

#include <userver/storages/clickhouse/io/columns/array_column.hpp>
#include <userver/storages/clickhouse/io/columns/column_includes.hpp>
#include <userver/storages/clickhouse/io/columns/tuple_column.hpp>

#include <utility>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

class MapColumn final : public ClickhouseColumn<MapColumn> {
public:
    using cpp_type = void;
    using container_type = void;

    MapColumn(ColumnRef column);

    // Serialization is not provided at this abstraction; use native builders
    static ColumnRef Serialize(const std::vector<int>&) = delete;
};

/// Typed Map wrapper similar to _dev ColumnMapT
template <typename KeyColumn, typename ValueColumn>
class MapColumnT final : public ClickhouseColumn<MapColumnT<KeyColumn, ValueColumn>> {
public:
    using Key = typename KeyColumn::cpp_type;
    using Value = typename ValueColumn::cpp_type;
    using Row = std::vector<std::pair<Key, Value>>;
    using container_type = std::vector<Row>;

    MapColumnT(ColumnRef column) : ClickhouseColumn<MapColumnT<KeyColumn, ValueColumn>>{std::move(column)} {}

    class MapValueView {
    public:
        using ValueType = std::pair<Key, Value>;

        MapValueView(ColumnRef tuple_array) : tuple_array_{std::move(tuple_array)} {}

        class Iterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = ValueType;
            using reference = value_type;
            using iterator_category = std::forward_iterator_tag;

            Iterator() = default;
            Iterator(ColumnRef keys, ColumnRef values, std::size_t index)
                : keys_{std::move(keys)}, values_{std::move(values)}, index_{index} {}

            reference operator*() const {
                KeyColumn kc{keys_};
                ValueColumn vc{values_};
                auto it_k = kc.begin();
                auto it_v = vc.begin();
                for (std::size_t i = 0; i < index_; ++i) {
                    ++it_k;
                    ++it_v;
                }
                return {(*it_k), (*it_v)};
            }

            Iterator& operator++() {
                ++index_;
                return *this;
            }

            bool operator==(const Iterator& other) const {
                return keys_.get() == other.keys_.get() && values_.get() == other.values_.get() &&
                       index_ == other.index_;
            }
            bool operator!=(const Iterator& other) const { return !(*this == other); }

        private:
            ColumnRef keys_{};
            ColumnRef values_{};
            std::size_t index_{0};
        };

        Iterator begin() const { return MakeIterator(0); }
        Iterator end() const { return MakeIterator(Size()); }
        std::size_t Size() const { return GetColumnSize(tuple_array_); }

    private:
        Iterator MakeIterator(std::size_t index) const {
            auto key_values = ExtractKeyValueColumns(tuple_array_);
            return Iterator{key_values.first, key_values.second, index};
        }

        static std::pair<ColumnRef, ColumnRef> ExtractKeyValueColumns(const ColumnRef& tuple_array);

        ColumnRef tuple_array_{};
    };

    using iterator_data = typename ColumnIterator<MapColumnT<KeyColumn, ValueColumn>>::DataHolder;

    static ColumnRef Serialize(const container_type& from) {
        // Build array of tuples from vector of maps
        using TupleCol = TupleColumnT<KeyColumn, ValueColumn>;
        using ArrayOfTuple = ArrayColumn<TupleCol>;

        typename TupleCol::container_type tuples;
        tuples.reserve(TotalPairs(from));
        std::vector<typename ArrayOfTuple::cpp_type> rows;
        rows.reserve(from.size());
        for (const auto& row : from) {
            typename ArrayOfTuple::cpp_type this_row;
            this_row.reserve(row.size());
            for (const auto& kv : row) {
                tuples.emplace_back(kv.first, kv.second);
                this_row.push_back(std::make_tuple(kv.first, kv.second));
            }
            rows.push_back(std::move(this_row));
        }

        // Serialize per-row tuple arrays directly
        return ArrayOfTuple::Serialize(rows);
    }

private:
    static std::size_t TotalPairs(const container_type& from) {
        std::size_t total = 0;
        for (const auto& r : from) total += r.size();
        return total;
    }
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

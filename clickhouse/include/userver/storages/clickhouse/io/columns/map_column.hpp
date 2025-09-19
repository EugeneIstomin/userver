#pragma once

/// @file userver/storages/clickhouse/io/columns/map_column.hpp
/// @brief Map column support
/// @ingroup userver_clickhouse_types

#include <utility>
#include <vector>

#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse Map(K, V) column as a sequence of key/value pairs per row
///
/// Internally backed by Array(Tuple(K, V)).
template <typename K, typename V>
class MapColumn final : public ClickhouseColumn<MapColumn<K, V>> {
public:
    using key_type = typename K::cpp_type;
    using mapped_type = typename V::cpp_type;
    using pair_type = std::pair<key_type, mapped_type>;
    using cpp_type = std::vector<pair_type>;
    using container_type = std::vector<cpp_type>;

    MapColumn(ColumnRef column);

    // Iterator customization: reuse Array-like iterator to materialize row values on demand
    class MapDataHolder final {
    public:
        MapDataHolder() = default;
        MapDataHolder(typename ColumnIterator<MapColumn<K, V>>::IteratorPosition iter_position, ColumnRef&& column);

        MapDataHolder operator++(int);
        MapDataHolder& operator++();
        cpp_type& UpdateValue();

        bool operator==(const MapDataHolder& other) const;

    private:
        ColumnRef inner_{};
        std::size_t ind_{0};
        std::optional<cpp_type> current_value_ = std::nullopt;
    };

    using iterator_data = MapDataHolder;

    static ColumnRef Serialize(const container_type& from);
    static cpp_type RetrieveElement(const ColumnRef& ref, std::size_t ind);
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

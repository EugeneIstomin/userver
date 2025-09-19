#pragma once

/// @file userver/storages/clickhouse/io/columns/fixed_string_column.hpp
/// @brief FixedString(N) column support
/// @ingroup userver_clickhouse_types

#include <cstddef>
#include <string>

#include <clickhouse/columns/string.h>
#include <optional>
#include <storages/clickhouse/impl/wrap_clickhouse_cpp.hpp>
#include <userver/storages/clickhouse/io/columns/column_includes.hpp>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::io::columns {

/// @brief Represents ClickHouse FixedString(N) column
template <std::size_t N>
class FixedStringColumn final : public ClickhouseColumn<FixedStringColumn<N>> {
public:
    using cpp_type = std::string;
    using container_type = std::vector<cpp_type>;

    explicit FixedStringColumn(ColumnRef column);

    static ColumnRef Serialize(const container_type& from);

    // Custom iterator data to avoid dependent nested-type specialization
    class IteratorDataHolder final {
    public:
        using IteratorPosition = ColumnIterator<FixedStringColumn<N>>::IteratorPosition;

        IteratorDataHolder() = default;
        IteratorDataHolder(IteratorPosition iter_position, ColumnRef&& column) : column_{std::move(column)} {
            switch (iter_position) {
                case IteratorPosition::kBegin:
                    ind_ = 0;
                    break;
                case IteratorPosition::kEnd:
                    ind_ = GetColumnSize(column_);
                    break;
            }
        }

        IteratorDataHolder operator++(int) {
            IteratorDataHolder old{};
            old.column_ = column_;
            old.ind_ = ind_++;
            old.current_value_ = std::move_if_noexcept(current_value_);
            current_value_.reset();
            return old;
        }

        IteratorDataHolder& operator++() {
            ++ind_;
            current_value_.reset();
            return *this;
        }

        cpp_type& UpdateValue() {
            UASSERT(ind_ < GetColumnSize(column_));
            if (!current_value_.has_value()) {
                using NativeFixedType = clickhouse::impl::clickhouse_cpp::ColumnFixedString;
                current_value_.emplace(std::string{static_cast<NativeFixedType*>(column_.get())->At(ind_)});
            }
            return *current_value_;
        }

        bool operator==(const IteratorDataHolder& other) const {
            return ind_ == other.ind_ && column_.get() == other.column_.get();
        }

    private:
        ColumnRef column_{};
        size_t ind_{0};
        std::optional<cpp_type> current_value_{};
    };

    using iterator_data = IteratorDataHolder;
};

}  // namespace storages::clickhouse::io::columns

USERVER_NAMESPACE_END

#pragma once

#include <memory>
#include <string_view>

#include <absl/numeric/int128.h>
#include <clickhouse/base/input.h>
#include <clickhouse/base/output.h>
#include <clickhouse/base/wire_format.h>
#include <clickhouse/columns/column.h>
#include <clickhouse/columns/numeric.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::impl::decimal_native {

namespace detail {
inline ::clickhouse::TypeRef MakeType(size_t precision, size_t scale) {
    return ::clickhouse::Type::CreateDecimal(precision, scale);
}
}  // namespace detail

class ColumnDecimal32 : public ::clickhouse::Column {
public:
    using ValueType = long long;  // transport type; client wrappers expose absl::int128

    explicit ColumnDecimal32(size_t scale)
        : ::clickhouse::Column(detail::MakeType(9, scale)),
          data_(std::make_shared<::clickhouse::ColumnInt32>()),
          scale_(scale) {}

    void Reserve(size_t new_cap) override { data_->Reserve(new_cap); }

    void Append(::clickhouse::ColumnRef column) override {
        if (auto col = column->As<ColumnDecimal32>()) {
            data_->Append(col->data_);
            return;
        }
        if (auto col_i32 = column->As<::clickhouse::ColumnInt32>()) {
            data_->Append(col_i32);
            return;
        }
    }

    bool LoadPrefix(::clickhouse::InputStream* /*input*/, size_t /*rows*/) override { return true; }
    bool LoadBody(::clickhouse::InputStream* input, size_t rows) override { return data_->LoadBody(input, rows); }
    void SaveBody(::clickhouse::OutputStream* output) override { data_->SaveBody(output); }
    void Clear() override { data_->Clear(); }
    size_t Size() const override { return data_->Size(); }
    ::clickhouse::ColumnRef Slice(size_t begin, size_t len) const override {
        auto sliced = std::make_shared<ColumnDecimal32>(scale_);
        sliced->data_ = data_->Slice(begin, len)->AsStrict<::clickhouse::ColumnInt32>();
        return sliced;
    }
    ::clickhouse::ColumnRef CloneEmpty() const override { return std::make_shared<ColumnDecimal32>(scale_); }
    void Swap(::clickhouse::Column& other) override {
        auto& col = dynamic_cast<ColumnDecimal32&>(other);
        data_.swap(col.data_);
        std::swap(scale_, col.scale_);
    }
    ::clickhouse::ItemView GetItem(size_t index) const override { return data_->GetItem(index); }

    inline void AppendRaw(int32_t v) { data_->Append(v); }
    inline int32_t AtRaw(size_t i) const { return data_->At(i); }
    inline size_t GetScale() const { return scale_; }

    // Provide At() for helper access
    inline int32_t At(size_t index) const { return data_->At(index); }

private:
    std::shared_ptr<::clickhouse::ColumnInt32> data_;
    size_t scale_;
};

class ColumnDecimal64 : public ::clickhouse::Column {
public:
    using ValueType = long long;

    explicit ColumnDecimal64(size_t scale)
        : ::clickhouse::Column(detail::MakeType(18, scale)),
          data_(std::make_shared<::clickhouse::ColumnInt64>()),
          scale_(scale) {}

    void Reserve(size_t new_cap) override { data_->Reserve(new_cap); }
    void Append(::clickhouse::ColumnRef column) override {
        if (auto col = column->As<ColumnDecimal64>()) {
            data_->Append(col->data_);
            return;
        }
        if (auto col_i64 = column->As<::clickhouse::ColumnInt64>()) {
            data_->Append(col_i64);
            return;
        }
    }
    bool LoadPrefix(::clickhouse::InputStream* /*input*/, size_t /*rows*/) override { return true; }
    bool LoadBody(::clickhouse::InputStream* input, size_t rows) override { return data_->LoadBody(input, rows); }
    void SaveBody(::clickhouse::OutputStream* output) override { data_->SaveBody(output); }
    void Clear() override { data_->Clear(); }
    size_t Size() const override { return data_->Size(); }
    ::clickhouse::ColumnRef Slice(size_t begin, size_t len) const override {
        auto sliced = std::make_shared<ColumnDecimal64>(scale_);
        sliced->data_ = data_->Slice(begin, len)->AsStrict<::clickhouse::ColumnInt64>();
        return sliced;
    }
    ::clickhouse::ColumnRef CloneEmpty() const override { return std::make_shared<ColumnDecimal64>(scale_); }
    void Swap(::clickhouse::Column& other) override {
        auto& col = dynamic_cast<ColumnDecimal64&>(other);
        data_.swap(col.data_);
        std::swap(scale_, col.scale_);
    }
    ::clickhouse::ItemView GetItem(size_t index) const override { return data_->GetItem(index); }

    inline void AppendRaw(int64_t v) { data_->Append(v); }
    inline int64_t AtRaw(size_t i) const { return data_->At(i); }
    inline size_t GetScale() const { return scale_; }

    inline int64_t At(size_t index) const { return data_->At(index); }

private:
    std::shared_ptr<::clickhouse::ColumnInt64> data_;
    size_t scale_;
};

class ColumnDecimal128 : public ::clickhouse::Column {
public:
    using ValueType = long long;

    explicit ColumnDecimal128(size_t scale)
        : ::clickhouse::Column(detail::MakeType(38, scale)),
          data_(std::make_shared<::clickhouse::ColumnInt128>()),
          scale_(scale) {}

    void Reserve(size_t new_cap) override { data_->Reserve(new_cap); }
    void Append(::clickhouse::ColumnRef column) override {
        if (auto col = column->As<ColumnDecimal128>()) {
            data_->Append(col->data_);
            return;
        }
        if (auto col_i128 = column->As<::clickhouse::ColumnInt128>()) {
            data_->Append(col_i128);
            return;
        }
    }
    bool LoadPrefix(::clickhouse::InputStream* /*input*/, size_t /*rows*/) override { return true; }
    bool LoadBody(::clickhouse::InputStream* input, size_t rows) override { return data_->LoadBody(input, rows); }
    void SaveBody(::clickhouse::OutputStream* output) override { data_->SaveBody(output); }
    void Clear() override { data_->Clear(); }
    size_t Size() const override { return data_->Size(); }
    ::clickhouse::ColumnRef Slice(size_t begin, size_t len) const override {
        auto sliced = std::make_shared<ColumnDecimal128>(scale_);
        sliced->data_ = data_->Slice(begin, len)->AsStrict<::clickhouse::ColumnInt128>();
        return sliced;
    }
    ::clickhouse::ColumnRef CloneEmpty() const override { return std::make_shared<ColumnDecimal128>(scale_); }
    void Swap(::clickhouse::Column& other) override {
        auto& col = dynamic_cast<ColumnDecimal128&>(other);
        data_.swap(col.data_);
        std::swap(scale_, col.scale_);
    }
    ::clickhouse::ItemView GetItem(size_t index) const override { return data_->GetItem(index); }

    inline void AppendRaw(absl::int128 v) { data_->Append(static_cast<::clickhouse::ColumnInt128::DataType>(v)); }
    inline absl::int128 AtRaw(size_t i) const { return data_->At(i); }
    inline size_t GetScale() const { return scale_; }

    inline absl::int128 At(size_t index) const { return data_->At(index); }

private:
    std::shared_ptr<::clickhouse::ColumnInt128> data_;
    size_t scale_;
};

}  // namespace storages::clickhouse::impl::decimal_native

USERVER_NAMESPACE_END

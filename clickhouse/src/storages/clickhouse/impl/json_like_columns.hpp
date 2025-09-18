#pragma once

// Local native columns for JSON-like ClickHouse types: JSON, Variant, Dynamic.
// These wrap values as strings and follow ColumnString serialization for body.

#include <memory>
#include <string>
#include <string_view>

#include <clickhouse/base/input.h>
#include <clickhouse/base/output.h>
#include <clickhouse/base/wire_format.h>
#include <clickhouse/columns/column.h>
#include <clickhouse/columns/numeric.h>
#include <clickhouse/columns/string.h>
#include <clickhouse/columns/tuple.h>

USERVER_NAMESPACE_BEGIN

namespace storages::clickhouse::impl::json_like_native {

// Write-only helpers for JSON, Variant, and Dynamic types.
// Implementation stores values as strings and writes them using
// the same body encoding as ColumnString. Type codes are preserved
// to match server-side schema during INSERT with Native format.
// Reading is not implemented beyond minimal compatibility.

class ColumnJSON : public ::clickhouse::Column {
public:
    using ValueType = std::string_view;

    ColumnJSON()
        : ::clickhouse::Column(::clickhouse::Type::CreateJSON()),
          data_(std::make_shared<::clickhouse::ColumnString>()) {}

    // Convenience append for string-like values
    inline void Append(std::string_view str) { data_->Append(str); }
    inline void Append(const char* str) { data_->Append(str); }
    inline void Append(std::string&& str) { data_->Append(std::move(str)); }
    inline void AppendNoManagedLifetime(std::string_view str) { data_->AppendNoManagedLifetime(str); }

    // Provide At() to interop with existing helpers
    inline std::string_view At(size_t index) const { return data_->At(index); }

public:  // Column interface
    void Reserve(size_t new_cap) override { data_->Reserve(new_cap); }

    void Append(::clickhouse::ColumnRef column) override {
        if (auto col = column->As<ColumnJSON>()) {
            data_->Append(col->data_);
            return;
        }
        if (auto col_str = column->As<::clickhouse::ColumnString>()) {
            data_->Append(col_str);
            return;
        }
    }

    bool LoadPrefix(::clickhouse::InputStream* input, size_t /*rows*/) override {
        // JSON v1: write/read a 64-bit serialize version prefix.
        // For write-only use cases reading may not be required,
        // but accept and ignore the prefix if present.
        uint64_t serialize_version = 0;
        (void)::clickhouse::WireFormat::ReadFixed(*input, &serialize_version);
        return true;
    }

    bool LoadBody(::clickhouse::InputStream* input, size_t rows) override { return data_->LoadBody(input, rows); }

    void SavePrefix(::clickhouse::OutputStream* output) override {
        // Match clickhouse-connect behavior: write json_serialization_format (default 1)
        const uint64_t serialize_version = 1;
        ::clickhouse::WireFormat::WriteFixed(*output, serialize_version);
    }

    void SaveBody(::clickhouse::OutputStream* output) override { data_->SaveBody(output); }

    void Clear() override { data_->Clear(); }

    size_t Size() const override { return data_->Size(); }

    ::clickhouse::ColumnRef Slice(size_t begin, size_t len) const override {
        auto sliced = std::make_shared<ColumnJSON>();
        sliced->data_ = data_->Slice(begin, len)->AsStrict<::clickhouse::ColumnString>();
        return sliced;
    }

    ::clickhouse::ColumnRef CloneEmpty() const override { return std::make_shared<ColumnJSON>(); }

    void Swap(::clickhouse::Column& other) override {
        auto& col = dynamic_cast<ColumnJSON&>(other);
        data_.swap(col.data_);
    }

    ::clickhouse::ItemView GetItem(size_t index) const override { return data_->GetItem(index); }

private:
    std::shared_ptr<::clickhouse::ColumnString> data_;
};

class ColumnVariant : public ::clickhouse::Column {
public:
    using ValueType = std::string_view;

    ColumnVariant()
        : ::clickhouse::Column(::clickhouse::Type::CreateVariant()),
          data_(std::make_shared<::clickhouse::ColumnString>()) {}

    inline void Append(std::string_view str) { data_->Append(str); }
    inline void Append(const char* str) { data_->Append(str); }
    inline void Append(std::string&& str) { data_->Append(std::move(str)); }
    inline void AppendNoManagedLifetime(std::string_view str) { data_->AppendNoManagedLifetime(str); }

    inline std::string_view At(size_t index) const { return data_->At(index); }

public:
    void Reserve(size_t new_cap) override { data_->Reserve(new_cap); }

    void Append(::clickhouse::ColumnRef column) override {
        if (auto col = column->As<ColumnVariant>()) {
            data_->Append(col->data_);
            return;
        }
        if (auto col_str = column->As<::clickhouse::ColumnString>()) {
            data_->Append(col_str);
            return;
        }
    }

    bool LoadPrefix(::clickhouse::InputStream* /*input*/, size_t /*rows*/) override { return true; }
    bool LoadBody(::clickhouse::InputStream* input, size_t rows) override { return data_->LoadBody(input, rows); }
    void SaveBody(::clickhouse::OutputStream* output) override { data_->SaveBody(output); }
    void Clear() override { data_->Clear(); }
    size_t Size() const override { return data_->Size(); }
    ::clickhouse::ColumnRef Slice(size_t begin, size_t len) const override {
        auto sliced = std::make_shared<ColumnVariant>();
        sliced->data_ = data_->Slice(begin, len)->AsStrict<::clickhouse::ColumnString>();
        return sliced;
    }
    ::clickhouse::ColumnRef CloneEmpty() const override { return std::make_shared<ColumnVariant>(); }
    void Swap(::clickhouse::Column& other) override {
        auto& col = dynamic_cast<ColumnVariant&>(other);
        data_.swap(col.data_);
    }

    ::clickhouse::ItemView GetItem(size_t index) const override { return data_->GetItem(index); }

private:
    std::shared_ptr<::clickhouse::ColumnString> data_;
};

class ColumnDynamic : public ::clickhouse::Column {
public:
    using ValueType = std::string_view;

    ColumnDynamic()
        : ::clickhouse::Column(::clickhouse::Type::CreateDynamic()),
          data_(std::make_shared<::clickhouse::ColumnString>()) {}

    inline void Append(std::string_view str) { data_->Append(str); }
    inline void Append(const char* str) { data_->Append(str); }
    inline void Append(std::string&& str) { data_->Append(std::move(str)); }
    inline void AppendNoManagedLifetime(std::string_view str) { data_->AppendNoManagedLifetime(str); }

    inline std::string_view At(size_t index) const { return data_->At(index); }

public:
    void Reserve(size_t new_cap) override { data_->Reserve(new_cap); }
    void Append(::clickhouse::ColumnRef column) override {
        if (auto col = column->As<ColumnDynamic>()) {
            data_->Append(col->data_);
            return;
        }
        if (auto col_str = column->As<::clickhouse::ColumnString>()) {
            data_->Append(col_str);
            return;
        }
    }
    bool LoadPrefix(::clickhouse::InputStream* /*input*/, size_t /*rows*/) override { return true; }
    bool LoadBody(::clickhouse::InputStream* input, size_t rows) override { return data_->LoadBody(input, rows); }
    void SaveBody(::clickhouse::OutputStream* output) override { data_->SaveBody(output); }
    void Clear() override { data_->Clear(); }
    size_t Size() const override { return data_->Size(); }
    ::clickhouse::ColumnRef Slice(size_t begin, size_t len) const override {
        auto sliced = std::make_shared<ColumnDynamic>();
        sliced->data_ = data_->Slice(begin, len)->AsStrict<::clickhouse::ColumnString>();
        return sliced;
    }
    ::clickhouse::ColumnRef CloneEmpty() const override { return std::make_shared<ColumnDynamic>(); }
    void Swap(::clickhouse::Column& other) override {
        auto& col = dynamic_cast<ColumnDynamic&>(other);
        data_.swap(col.data_);
    }

    ::clickhouse::ItemView GetItem(size_t index) const override { return data_->GetItem(index); }

private:
    std::shared_ptr<::clickhouse::ColumnString> data_;
};

}  // namespace storages::clickhouse::impl::json_like_native

USERVER_NAMESPACE_END

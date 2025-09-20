#include <string>
#include <vector>

#include <userver/utest/using_namespace_userver.hpp>

#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include <userver/storages/clickhouse/cluster.hpp>
#include <userver/storages/clickhouse/component.hpp>

#include <boost/uuid/string_generator.hpp>
#include <chrono>
#include <optional>
#include <userver/storages/clickhouse/io/columns/array_column.hpp>
#include <userver/storages/clickhouse/io/columns/common_columns.hpp>
#include <userver/storages/clickhouse/io/columns/nullable_column.hpp>
#include <userver/storages/clickhouse/query.hpp>

#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

#include <userver/utils/daemon_run.hpp>
#include "userver/clients/http/component.hpp"
#include "userver/server/handlers/server_monitor.hpp"
#include "userver/server/handlers/tests_control.hpp"
#include "userver/testsuite/testsuite_support.hpp"

namespace clickhouse::datatypes {

// Narrow C++ structures used for Insert/Select mapping in tests
struct RowUInt128Int128 {
    absl::uint128 u128;
    absl::int128 i128;
};

struct RowDecimals {
    absl::int128 d32;
    absl::int128 d64;
    absl::int128 d128;
};

struct RowEnums {
    std::int8_t e8;
    std::int16_t e16;
};

struct RowFixedStrings {
    std::string fs4;
    std::string lcs;
    std::string lcfs4;
};

struct RowTupleMap {
    // Tuple(a Int32, b String)
    std::vector<storages::clickhouse::io::columns::ColumnRef> tuple_cols;
    // Map(String, Int32)
    std::vector<std::pair<std::string, std::int32_t>> map_values;
};

struct RowJsonDynamicVariant {
    std::string json;
    std::string dynamic_v;
    std::string variant_v;
};

struct RowPrimitives {
    std::int8_t i8;
    std::int32_t i32;
    std::int64_t i64;
    std::uint8_t u8;
    std::uint16_t u16;
    std::uint32_t u32;
    std::uint64_t u64;
    float f32;
    double f64;
    std::string s;
};

struct RowDatetimes {
    std::chrono::system_clock::time_point dt;
    std::chrono::system_clock::time_point dt64m;
    std::chrono::system_clock::time_point dt64u;
    std::chrono::system_clock::time_point dt64n;
};

struct RowUuid {
    boost::uuids::uuid u;
};

struct RowArrays {
    std::vector<std::int32_t> ai32;
    std::vector<std::string> as;
};

struct RowNullable {
    std::optional<std::int32_t> ni32;
    std::optional<std::string> ns;
};

class HandlerTypesClickhouse final : public server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-datatypes-clickhouse";

    HandlerTypesClickhouse(const components::ComponentConfig& config, const components::ComponentContext& context);

    std::string HandleRequestThrow(
        const server::http::HttpRequest& request,
        server::request::RequestContext& context
    ) const override;

private:
    storages::clickhouse::ClusterPtr clickhouse_;
};

HandlerTypesClickhouse::HandlerTypesClickhouse(
    const components::ComponentConfig& config,
    const components::ComponentContext& context
)
    : server::handlers::HttpHandlerBase{config, context},
      clickhouse_{context.FindComponent<components::ClickHouse>("clickhouse-database").GetCluster()} {}

std::string HandlerTypesClickhouse::HandleRequestThrow(
    const server::http::HttpRequest& request,
    server::request::RequestContext&
) const {
    const auto action = request.GetArg("action");

    if (action == "insert") {
        // Prepare and insert one row for every table to exercise serializers
        // UInt128/Int128
        {
            RowUInt128Int128 data{absl::MakeUint128(1, 2), absl::MakeInt128(3, 4)};
            clickhouse_->Insert("t_int128", {"u128", "i128"}, data);
        }
        // Decimals
        {
            RowDecimals data{12345, 67890123, 999999999};
            clickhouse_->Insert("t_decimals", {"d32", "d64", "d128"}, data);
        }
        // Enums
        {
            RowEnums data{1, 2};
            clickhouse_->Insert("t_enums", {"e8", "e16"}, data);
        }
        // FixedString/LC(String)/LC(FixedString)
        {
            RowFixedStrings data{"abcd", "foo", "zzzz"};
            clickhouse_->Insert("t_fixed_lc", {"fs4", "lcs", "lcfs4"}, data);
        }
        // Tuple and Map
        {
            using columns::Int32Column;
            using columns::StringColumn;
            RowTupleMap data;
            {
                // Build tuple subcolumns with one element each
                std::vector<int32_t> a{42};
                std::vector<std::string> b{"ok"};
                auto a_col = Int32Column::Serialize(a);
                auto b_col = StringColumn::Serialize(b);
                data.tuple_cols.push_back(columns::MakeTupleColumn({a_col, b_col}));
            }
            data.map_values = {std::make_pair(std::string{"k"}, 1), std::make_pair(std::string{"x"}, 2)};
            clickhouse_->Insert("t_tuple_map", {"t", "m"}, data);
        }
        // JSON/Dynamic/Variant
        {
            RowJsonDynamicVariant data{"{\"a\":1}", "{\"b\":2}", "{\"c\":3}"};
            clickhouse_->Insert("t_json_like", {"j", "d", "v"}, data);
        }
        // Primitives
        {
            RowPrimitives data{-1, 123456, 1234567890123LL, 255, 65535, 1234567890u, 9876543210ULL, 1.5f, 2.5, "hello"};
            clickhouse_->Insert(
                "t_primitives", {"i8", "i32", "i64", "u8", "u16", "u32", "u64", "f32", "f64", "s"}, data
            );
        }
        // Datetimes
        {
            using clock = std::chrono::system_clock;
            RowDatetimes data{
                clock::time_point{std::chrono::seconds{1}},
                clock::time_point{std::chrono::seconds{2}},
                clock::time_point{std::chrono::seconds{3}},
                clock::time_point{std::chrono::seconds{4}}
            };
            clickhouse_->Insert("t_datetimes", {"dt", "dt64m", "dt64u", "dt64n"}, data);
        }
        // UUID
        {
            boost::uuids::string_generator gen;
            RowUuid data{gen("00112233-4455-6677-8899-aabbccddeeff")};
            clickhouse_->Insert("t_uuid", {"u"}, data);
        }
        // Arrays
        {
            RowArrays data{{1, 2, 3}, {"a", "b"}};
            clickhouse_->Insert("t_arrays", {"ai32", "as"}, data);
        }
        // Nullable
        {
            RowNullable data{42, std::nullopt};
            clickhouse_->Insert("t_nullable", {"ni32", "ns"}, data);
        }
        return "inserted";
    }

    if (action == "select") {
        // Simple selects to ensure deserializers and iterators work
        {
            const storages::clickhouse::Query q{"SELECT u128, i128 FROM t_int128"};
            (void)clickhouse_->Execute(q).As<RowUInt128Int128>();
        }
        {
            const storages::clickhouse::Query q{"SELECT d32, d64, d128 FROM t_decimals"};
            (void)clickhouse_->Execute(q).As<RowDecimals>();
        }
        {
            const storages::clickhouse::Query q{"SELECT e8, e16 FROM t_enums"};
            (void)clickhouse_->Execute(q).As<RowEnums>();
        }
        {
            const storages::clickhouse::Query q{"SELECT fs4, lcs, lcfs4 FROM t_fixed_lc"};
            (void)clickhouse_->Execute(q).As<RowFixedStrings>();
        }
        {
            const storages::clickhouse::Query q{"SELECT t, m FROM t_tuple_map"};
            (void)clickhouse_->Execute(q).As<RowTupleMap>();
        }
        {
            const storages::clickhouse::Query q{"SELECT j, d, v FROM t_json_like"};
            (void)clickhouse_->Execute(q).As<RowJsonDynamicVariant>();
        }
        {
            const storages::clickhouse::Query q{"SELECT i8,i32,i64,u8,u16,u32,u64,f32,f64,s FROM t_primitives"};
            (void)clickhouse_->Execute(q).As<RowPrimitives>();
        }
        {
            const storages::clickhouse::Query q{"SELECT dt,dt64m,dt64u,dt64n FROM t_datetimes"};
            (void)clickhouse_->Execute(q).As<RowDatetimes>();
        }
        {
            const storages::clickhouse::Query q{"SELECT u FROM t_uuid"};
            (void)clickhouse_->Execute(q).As<RowUuid>();
        }
        {
            const storages::clickhouse::Query q{"SELECT ai32, as FROM t_arrays"};
            (void)clickhouse_->Execute(q).As<RowArrays>();
        }
        {
            const storages::clickhouse::Query q{"SELECT ni32, ns FROM t_nullable"};
            (void)clickhouse_->Execute(q).As<RowNullable>();
        }
        return "selected";
    }

    request.GetHttpResponse().SetStatus(server::http::HttpStatus::kBadRequest);
    return "bad";
}

}  // namespace clickhouse::datatypes

int main(int argc, char* argv[]) {
    const auto components_list = components::MinimalServerComponentList()
                                     .Append<server::handlers::ServerMonitor>()
                                     .Append<clickhouse::datatypes::HandlerTypesClickhouse>()
                                     .Append<components::ClickHouse>("clickhouse-database")
                                     .Append<components::HttpClient>()
                                     .Append<components::TestsuiteSupport>()
                                     .Append<server::handlers::TestsControl>()
                                     .Append<clients::dns::Component>()
                                     .Append<components::Secdist>()
                                     .Append<components::DefaultSecdistProvider>();

    return utils::DaemonMain(argc, argv, components_list);
}

USERVER_NAMESPACE_BEGIN
namespace storages::clickhouse::io {

// Column mappings
template <>
struct CppToClickhouse<::clickhouse::datatypes::RowUInt128Int128> final {
    using mapped_type = std::tuple<columns::UInt128Column, columns::Int128Column>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowDecimals> final {
    using mapped_type = std::tuple<columns::Decimal32Column, columns::Decimal64Column, columns::Decimal128Column>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowEnums> final {
    using mapped_type = std::tuple<columns::Enum8Column, columns::Enum16Column>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowFixedStrings> final {
    using mapped_type = std::tuple<
        columns::FixedStringColumn<4>,
        columns::LowCardinalityStringColumn,
        columns::LowCardinalityFixedStringColumn<4>>;
};

// Tuple(Int32,String) plus Map(String,Int32)
template <>
struct CppToClickhouse<::clickhouse::datatypes::RowTupleMap> final {
    using mapped_type =
        std::tuple<columns::TupleColumn, columns::MapColumn<columns::StringColumn, columns::Int32Column>>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowJsonDynamicVariant> final {
    using mapped_type = std::tuple<columns::JSONColumn, columns::DynamicColumn, columns::VariantColumn>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowPrimitives> final {
    using mapped_type = std::tuple<
        columns::Int8Column,
        columns::Int32Column,
        columns::Int64Column,
        columns::UInt8Column,
        columns::UInt16Column,
        columns::UInt32Column,
        columns::UInt64Column,
        columns::Float32Column,
        columns::Float64Column,
        columns::StringColumn>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowDatetimes> final {
    using mapped_type = std::tuple<
        columns::DateTimeColumn,
        columns::DateTime64ColumnMilli,
        columns::DateTime64ColumnMicro,
        columns::DateTime64ColumnNano>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowUuid> final {
    using mapped_type = std::tuple<columns::UuidRfc4122Column>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowArrays> final {
    using mapped_type =
        std::tuple<columns::ArrayColumn<columns::Int32Column>, columns::ArrayColumn<columns::StringColumn>>;
};

template <>
struct CppToClickhouse<::clickhouse::datatypes::RowNullable> final {
    using mapped_type =
        std::tuple<columns::NullableColumn<columns::Int32Column>, columns::NullableColumn<columns::StringColumn>>;
};

USERVER_NAMESPACE_END

}  // namespace storages::clickhouse::io

CREATE TABLE IF NOT EXISTS t_int128 (
  u128 UInt128,
  i128 Int128
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_decimals (
  d32 Decimal32(3),
  d64 Decimal64(3),
  d128 Decimal128(3)
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_enums (
  e8 Enum8('Unknown' = 0, 'One' = 1, 'Two' = 2),
  e16 Enum16('Zero' = 0, 'Two' = 2)
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_fixed_lc (
  fs4 FixedString(4),
  lcs LowCardinality(String),
  lcfs4 LowCardinality(FixedString(4))
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_tuple_map (
  t Tuple(Int32, String),
  m Map(String, Int32)
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_json_like (
  j JSON,
  d Dynamic,
  v Variant
) ENGINE = MergeTree ORDER BY tuple();


-- Additional tables to cover all supported datatypes
CREATE TABLE IF NOT EXISTS t_primitives (
  i8 Int8,
  i32 Int32,
  i64 Int64,
  u8 UInt8,
  u16 UInt16,
  u32 UInt32,
  u64 UInt64,
  f32 Float32,
  f64 Float64,
  s String
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_datetimes (
  dt DateTime,
  dt64m DateTime64(3),
  dt64u DateTime64(6),
  dt64n DateTime64(9)
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_uuid (
  u UUID
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_arrays (
  ai32 Array(Int32),
  as Array(String)
) ENGINE = MergeTree ORDER BY tuple();

CREATE TABLE IF NOT EXISTS t_nullable (
  ni32 Nullable(Int32),
  ns Nullable(String)
) ENGINE = MergeTree ORDER BY tuple();



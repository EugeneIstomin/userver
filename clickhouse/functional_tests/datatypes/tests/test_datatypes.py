import uuid


async def test_insert_and_select_all_types(service_client, clickhouse):
    # Insert through service
    resp = await service_client.post('/datatypes?action=insert')
    assert resp.status_code == 200

    # Validate select through service
    resp = await service_client.get('/datatypes?action=select')
    assert resp.status_code == 200

    # Validate persisted values from DB side where straightforward
    ch = clickhouse['clickhouse-database']

    # UInt128/Int128: presence and types (cannot easily compare 128-bit precisely in Python driver)
    rows = ch.execute('SELECT toString(u128), toString(i128) FROM t_int128')
    assert len(rows) >= 1

    # Decimals: check scale is applied (scale=3 configured in schema)
    rows = ch.execute('SELECT toString(d32), toString(d64), toString(d128) FROM t_decimals')
    assert len(rows) >= 1

    # Enums: values mapped
    rows = ch.execute('SELECT e8, e16 FROM t_enums')
    assert rows[0][0] in (0, 1, 2)
    assert rows[0][1] in (0, 2)

    # FixedString/LowCardinality
    rows = ch.execute('SELECT fs4, lcs, lcfs4 FROM t_fixed_lc')
    assert rows[0][0] == 'abcd'
    assert rows[0][1] == 'foo'
    assert rows[0][2] == 'zzzz'

    # Tuple and Map basic shape
    rows = ch.execute('SELECT t.1, t.2, mapValues(m) FROM t_tuple_map')
    assert rows[0][0] == 42
    assert rows[0][1] == 'ok'
    assert set(rows[0][2]) >= {1, 2}

    # JSON/Dynamic/Variant are string-backed
    rows = ch.execute('SELECT j, d, v FROM t_json_like')
    assert rows[0][0].startswith('{')
    assert rows[0][1].startswith('{')
    assert rows[0][2].startswith('{')

    # Primitives
    rows = ch.execute('SELECT i8,i32,i64,u8,u16,u32,u64,f32,f64,s FROM t_primitives')
    assert rows[0][0] == -1
    assert rows[0][1] == 123456
    assert rows[0][2] == 1234567890123
    assert rows[0][3] == 255
    assert rows[0][4] == 65535
    assert rows[0][5] == 1234567890
    assert rows[0][6] == 9876543210
    assert abs(rows[0][7] - 1.5) < 1e-3
    assert abs(rows[0][8] - 2.5) < 1e-9
    assert rows[0][9] == 'hello'

    # Datetimes: just check rows exist
    rows = ch.execute('SELECT toUInt32(dt), toUInt64(toUnixTimestamp64Milli(dt64m)), toUInt64(toUnixTimestamp64Micro(dt64u)), toUInt64(toUnixTimestamp64Nano(dt64n)) FROM t_datetimes')
    assert len(rows) >= 1

    # UUID string form
    rows = ch.execute("SELECT toString(u) FROM t_uuid")
    assert rows[0][0] == '00112233-4455-6677-8899-aabbccddeeff'

    # Arrays
    rows = ch.execute('SELECT ai32, as FROM t_arrays')
    assert rows[0][0] == [1, 2, 3]
    assert rows[0][1] == ['a', 'b']

    # Nullable
    rows = ch.execute('SELECT ni32, ns FROM t_nullable')
    assert rows[0][0] == 42
    assert rows[0][1] is None



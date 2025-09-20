import json
import typing

import pytest
from testsuite.databases.clickhouse import discover

pytest_plugins = ['pytest_userver.plugins.clickhouse']


@pytest.fixture(name='gate_settings', scope='session')
def _gate_settings() -> typing.Tuple[str, int]:
    return ('localhost', 19355)


@pytest.fixture(scope='session')
def service_env(gate_settings) -> dict:
    secdist_config = {
        'clickhouse_settings': {
            'clickhouse-database-alias': {
                'hosts': [gate_settings[0]],
                'port': gate_settings[1],
                'password': '',
                'user': 'default',
                'dbname': 'clickhouse-database',
            },
        },
    }

    return {'SECDIST_CONFIG': json.dumps(secdist_config)}


@pytest.fixture(scope='session')
def clickhouse_local(service_source_dir):
    return discover.find_schemas(
        schema_dirs=[service_source_dir.joinpath('schemas', 'clickhouse')],
        dbprefix='',
    )



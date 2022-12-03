import pathlib
import json
import pytest

pytest_plugins = ['testsuite.pytest_plugin']


def pytest_addoption(parser):
    group = parser.getgroup('dist-configuration-replication')
    group.addoption(
        '--port',
        help='bind echo server to this port (default is %(default)s)',
        default=8080,
        type=int,
    )


@pytest.fixture
async def registry(
        create_service_client,
        ensure_daemon_started,
        _registry_service_baseurl,
        _registry_service_scope,
):
    await ensure_daemon_started(_registry_service_scope)
    return create_service_client(_registry_service_baseurl)


@pytest.fixture(scope='session')
def _registry_service_baseurl(pytestconfig):
    return f'http://localhost:{pytestconfig.option.port}/'


@pytest.fixture(scope='session')
def _registry_server_dir(pytestconfig):
    return pathlib.Path(pytestconfig.rootpath).parent


@pytest.fixture(scope='session')
def _registry_server_binary(_registry_server_dir):
    config_path = _registry_server_dir.joinpath('static/config.json')
    config = json.load(open(config_path))
    return config['executable']


@pytest.fixture(scope='session')
async def _registry_service_scope(
        pytestconfig,
        create_daemon_scope,
        mockserver_info,
        _registry_service_baseurl,
        _registry_server_binary,
):
    async with create_daemon_scope(
            args=[
                _registry_server_binary,
                '--port', str(pytestconfig.option.port),
            ],
            ping_url=_registry_service_baseurl + 'ping',
    ) as scope:
        yield scope

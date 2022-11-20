import pathlib
import json
import pytest

pytest_plugins = ['testsuite.pytest_plugin']


def pytest_addoption(parser):
    group = parser.getgroup('echo server')
    group.addoption(
        '--echo-server-port',
        help='bind echo server to this port (default is %(default)s)',
        default=8080,
        type=int,
    )


@pytest.fixture
async def echo_server(
        create_service_client,
        echo_server_service_baseurl,
        ensure_daemon_started,
        echo_service_scope,
):
    await ensure_daemon_started(echo_service_scope)
    return create_service_client(echo_server_service_baseurl)


@pytest.fixture(scope='session')
def echo_server_service_baseurl(pytestconfig):
    return f'http://localhost:{pytestconfig.option.echo_server_port}/'


@pytest.fixture(scope='session')
def echo_server_dir(pytestconfig):
    return pathlib.Path(pytestconfig.rootpath).parent


@pytest.fixture(scope='session')
def echo_server_binary(echo_server_dir):
    config_path = echo_server_dir.joinpath('static/config.json')
    config = json.load(open(config_path))
    return config['echo_server_binary']


@pytest.fixture(scope='session')
async def echo_service_scope(
        pytestconfig,
        create_daemon_scope,
        mockserver_info,
        echo_server_service_baseurl,
        echo_server_binary,
):
    async with create_daemon_scope(
            args=[
                echo_server_binary,
            ],
            ping_url=echo_server_service_baseurl + 'ping',
    ) as scope:
        yield scope

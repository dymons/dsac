import pathlib
import json
import pytest

pytest_plugins = ['testsuite.pytest_plugin']


@pytest.fixture
async def registry(
        create_service_client,
        ensure_daemon_started,
        _registry_service_scope,
        _reset_service_cluster,
):
    await ensure_daemon_started(_registry_service_scope)
    await _reset_service_cluster()

    class Context:
        def __getitem__(self, port):
            return create_service_client(f'http://localhost:{port}/')

    return Context()


@pytest.fixture
async def snapshot(
        registry,
):
    async def __wrapper():
        snapshot = []
        for port in range(8080, 8083):
            response = await registry[port].post('v1/replica/get', json={})
            snapshot.append({
                'port': port,
                'snapshot': response.json() if response.status == 200 else None
            })
        return snapshot

    return __wrapper


@pytest.fixture(scope='session')
def _registry_server_dir(pytestconfig):
    return pathlib.Path(pytestconfig.rootpath).parent


@pytest.fixture(scope='session')
def _registry_server_binary(_registry_server_dir):
    config_path = _registry_server_dir.joinpath('static/config.json')
    config = json.load(open(config_path))
    return config['executable']


@pytest.fixture(scope='function')
async def _registry_service_scope(
        create_daemon_scope,
        _registry_server_binary,
):
    async with create_daemon_scope(
            args=[
                _registry_server_binary,
                '--port', '8080',
            ],
            ping_url=f'http://localhost:8080/ping',
    ) as scope:
        yield scope


@pytest.fixture(scope='function')
async def _reset_service_cluster(
        create_service_client,
):
    async def __wrapper():
        for port in range(8080, 8083):
            registry = create_service_client(f'http://localhost:{port}/')
            response = await registry.post('v1/replica/reset', json={})
            assert response.status == 200

    return __wrapper

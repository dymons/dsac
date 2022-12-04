async def test_happy_path(registry, snapshot):
    response = await registry[8080].post('v1/coordinator/set', json={'value': 10, 'timestamp': 0})
    assert response.status == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]


async def test_asynchronous_network(registry, snapshot):
    assert (await registry[8080].post('v1/coordinator/set', json={'value': 10, 'timestamp': 0})).status == 200
    assert (await registry[8080].post('v1/coordinator/set', json={'value': 11, 'timestamp': 1})).status == 200
    assert (await registry[8080].post('v1/coordinator/set', json={'value': 12, 'timestamp': 2})).status == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]

    assert (await registry[8080].post('v1/coordinator/set', json={'value': 10, 'timestamp': 0})).status == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]
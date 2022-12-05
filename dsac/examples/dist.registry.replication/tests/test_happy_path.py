async def test_happy_path(registry, snapshot):
    response = await registry[8080].post('v1/coordinator/set', json={'value': 10, 'timestamp': 0})
    assert response.status == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]

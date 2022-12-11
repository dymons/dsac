async def test_replica_set(registry, snapshot):
    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 0})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8081].post('v1/replica/write', json={'value': 20, 'timestamp': 1})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8082].post('v1/replica/write', json={'value': 30, 'timestamp': 2})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 30}},
    ]

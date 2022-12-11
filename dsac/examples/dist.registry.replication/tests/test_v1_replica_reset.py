async def test_reset_register_replica_with_unspecified_state(registry, snapshot):
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8080].post('v1/replica/reset')).status == 200
    assert (await registry[8081].post('v1/replica/reset')).status == 200
    assert (await registry[8082].post('v1/replica/reset')).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]


async def test_reset_register_replica_with_specified_state(registry, snapshot):
    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 10})).status == 200
    assert (await registry[8081].post('v1/replica/write', json={'value': 10, 'timestamp': 10})).status == 200
    assert (await registry[8082].post('v1/replica/write', json={'value': 10, 'timestamp': 10})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 10, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 10, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 10, 'value': 10}},
    ]

    assert (await registry[8080].post('v1/replica/reset')).status == 200
    assert (await registry[8081].post('v1/replica/reset')).status == 200
    assert (await registry[8082].post('v1/replica/reset')).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

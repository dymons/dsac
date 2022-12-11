async def test_read_from_register_coordinator_with_unspecified_state(
        registry,
        snapshot,
):
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}

    response = await registry[8081].post('v1/coordinator/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}

    response = await registry[8082].post('v1/coordinator/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}


async def test_read_from_register_coordinator_with_specified_state(
        registry,
        snapshot,
):
    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 0,
    }

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]


async def test_read_from_register_coordinator_with_updated_register_value(
        registry,
        snapshot,
):
    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 0,
    }

    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 20,
        'timestamp': 1,
    })
    assert response.status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 20,
        'timestamp': 1,
    }

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': {'timestamp': 1, 'value': 20}},
    ]


async def test_read_from_register_coordinator_after_reset_register_state(
        registry,
        snapshot,
):
    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 0,
    }

    assert (await registry[8080].post('v1/replica/reset')).status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 0,
    }

    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]

    assert (await registry[8081].post('v1/replica/reset')).status_code == 200
    assert (await registry[8082].post('v1/replica/reset')).status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}

    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

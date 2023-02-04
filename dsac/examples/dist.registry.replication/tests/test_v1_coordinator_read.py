async def test_given_empty_register_when_read_register_from_coordinators_then_register_not_found(
        registry,
        snapshot,
):
    # Arrange
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    for port in [8080, 8081, 8082]:
        # Act
        response = await registry[port].post('v1/coordinator/read')

        # Assert
        assert response.status_code == 404
        assert response.json() == {'error_message': 'The register is not initialized'}


async def test_given_empty_register_when_write_register_to_coordinator_then_register_is_specified(
        registry,
        snapshot,
):
    # Arrange
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    # Act
    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status_code == 200, response.text

    # Assert
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


async def test_given_specified_register_when_update_register_to_coordinator_then_register_is_updated(
        registry,
        snapshot,
):
    # Arrange
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

    # Act
    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 20,
        'timestamp': 1,
    })
    assert response.status_code == 200

    # Assert
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


async def test_given_specified_register_when_reset_register_on_some_replica_then_register_on_replica_unspecified(
        registry,
        snapshot,
):
    # Arrange
    response = await registry[8080].post('v1/coordinator/write', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status_code == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]

    # Act
    assert (await registry[8080].post('v1/replica/reset')).status_code == 200

    # Assert
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]

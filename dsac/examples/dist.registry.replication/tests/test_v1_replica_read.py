async def test_read_from_register_replica_with_unspecified_state(
        registry,
        snapshot,
):
    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    response = await registry[8080].post('v1/replica/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}

    response = await registry[8081].post('v1/replica/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}

    response = await registry[8082].post('v1/replica/read')
    assert response.status_code == 404
    assert response.json() == {'error_message': 'The register is not initialized'}

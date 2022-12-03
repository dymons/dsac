async def test_replica_get_unspecified_value(registry):
    response = await registry.post('v1/replica/get', json={
        'key': '__entity_0',
    })
    assert response.status == 400
    assert response.json() == {
        'error_message': '__unspecified_value',
    }

async def test_replica_get_unspecified_value(registry0):
    response = await registry0.post('v1/replica/get', json={})
    assert response.status == 404
    assert response.json() == {
        'error_message': '__unspecified_value',
    }

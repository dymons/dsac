async def test_coordinator_set_happy_path(registry):
    response = await registry.post('v1/coordinator/set', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status == 200

    response = await registry.post('v1/coordinator/get', json={})
    assert response.status == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 0,
    }

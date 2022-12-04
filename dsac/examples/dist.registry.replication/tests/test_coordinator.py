async def test_coordinator_set_happy_path(registry):
    response = await registry[8080].post('v1/coordinator/set', json={
        'value': 10,
        'timestamp': 0,
    })
    assert response.status == 200

    for port in range(8080, 8082):
        response = await registry[port].post('v1/coordinator/get', json={})
        assert response.status == 200
        assert response.json() == {
            'value': 10,
            'timestamp': 0,
        }

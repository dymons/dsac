async def test_coordinator_set(registry):
    response = await registry.post('v1/coordinator/set', json={'key': 10}, )
    assert response.status == 200

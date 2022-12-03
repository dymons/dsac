async def test_ping(registry):
    response = await registry.get('ping', json={}, )
    assert response.status == 200

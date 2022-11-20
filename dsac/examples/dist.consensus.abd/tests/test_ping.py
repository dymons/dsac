async def test_ping(consensus):
    response = await consensus.get('ping', json={}, )
    assert response.status == 200

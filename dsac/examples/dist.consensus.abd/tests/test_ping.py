async def test_ping(consensus):
    response = await consensus.post('echo', json={}, )
    assert response.status == 200

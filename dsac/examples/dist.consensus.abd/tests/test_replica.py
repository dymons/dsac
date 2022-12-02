async def test_replica_get(consensus):
    response = await consensus.post('v1/replica/get')
    assert response.status == 200
    assert response.text == '0'

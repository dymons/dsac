async def test_replica_get(consensus):
    response = await consensus.post('v1/replica/get', json={
        'key': 'entity_0',
    })
    assert response.status == 200
    assert response.json() == {'value': '__unspecified_value'}

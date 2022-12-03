async def test_replica_get_unspecified_value(consensus):
    response = await consensus.post('v1/replica/get', json={
        'key': '__entity_0',
    })
    assert response.status == 400
    assert response.json() == {
        'error_message': '__unspecified_value',
    }

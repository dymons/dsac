async def test_coordinator_set(consensus):
    response = await consensus.post('v1/coordinator/set', json={'key': 10}, )
    assert response.status == 200

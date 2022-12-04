async def test_ping(registry):
    response = await registry[8080].get('ping')
    assert response.status == 200

    response = await registry[8081].get('ping')
    assert response.status == 200

    response = await registry[8082].get('ping')
    assert response.status == 200

async def test_ping(registry0, registry1, registry2):
    response = await registry0.get('ping')
    assert response.status == 200

    response = await registry1.get('ping')
    assert response.status == 200

    response = await registry2.get('ping')
    assert response.status == 200

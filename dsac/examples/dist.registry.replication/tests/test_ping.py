async def test_ping(
        registry,
):
    # Arrange & Act & Assert
    assert (await registry[8080].get('ping')).status == 200
    assert (await registry[8081].get('ping')).status == 200
    assert (await registry[8082].get('ping')).status == 200

async def test_echo_server(echo_server):
    response = await echo_server.post('echo', data='content', )
    assert response.status == 200
    assert response.text == 'content'

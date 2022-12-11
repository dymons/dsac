async def test_spec_read_to_write_phase(registry, snapshot):
    """
                   |-W(10, 1)-------------------------------------------------------
                    \        /
     R[8080]  -------\------/-------------------/\----------------------/\----------
                      \    /                   /  \                    /  \
                       \  /                   /    \                  /    \
     R[8081]  ----------\/---------/\--------/--/\--\----------------/------\-------
                                  /  \      /  /  \  \              /        \
                                 /    \    /  /    \  \            /          \
     R[8082]  ------------------/-/\---\--/--/------\--\----------/-/\---------\----
                               / /  \   \/  /        \  \        / /  \         \
                              |-R(10, 1)-----W(10, 1)----|      |----R(10, 1)----|
                                  read        write
    """

    assert (await registry[8081].post('v1/replica/write', json={'value': 10, 'timestamp': 1})).status_code == 200

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 1,
    }

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 1, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 1, 'value': 10}},
    ]

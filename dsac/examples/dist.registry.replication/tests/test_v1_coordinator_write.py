async def test_happy_path(registry, snapshot):
    response = await registry[8080].post('v1/coordinator/write', json={'value': 10, 'timestamp': 0})
    assert response.status == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 0, 'value': 10}},
    ]


async def test_reject_olden_record(registry, snapshot):
    """
           |----W(12, 2)----|          |-W(10, 0)-|
            \ \        /   /            \ \  /   /
        -----\-\------/---/------/\------\-\/---/------/\-------------
              \ \    /   /      /  \      \    /      /  \
               \ \  /   /      /    \      \  /      /    \
        --------\-\/---/------/------\------\/------/--/\--\----------
                 \    /      /        \            /  /  \  \
                  \  /      /          \          /  /    \  \
        -----------\/------/--/\--------\--------/--/------\--\-------
                          /  /  \        \      /  /        \  \
                         |----R(12, 2)----|    |----R(12, 2)----|

          Record has olden timestamp, so the value will not be updated.
          Replicas in the cluster contain a value=12 with a timestamp=2
    """

    assert (await registry[8080].post('v1/coordinator/write', json={'value': 12, 'timestamp': 2})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]

    assert (await registry[8080].post('v1/coordinator/write', json={'value': 10, 'timestamp': 0})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]
async def test_spec_read_to_write_phase(
        registry,
        snapshot
):
    """
        This specification illustrates a scenario when the write operation
        has  not  yet  completed and  subsequent  reads can return various
        answer  options  and  violate  the  guarantee  of  Linearizability.
        To  do  this,  an  additional  write  is  added  to  the  replicas
        of the last actual value in the reading phase.

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
                                   ^     ^       ^       ^
                                   |     |       |        \
                             read phase  |  write phase    \
                                         |                  \
                         replicas are not consistent        replicas are consistent
    """

    # Arrange
    assert (await registry[8081].post('v1/replica/write', json={'value': 10, 'timestamp': 1})).status_code == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 10}},
        {'port': 8082, 'snapshot': None},
    ]

    # Act
    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 10,
        'timestamp': 1,
    }

    # Assert
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 1, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 1, 'value': 10}},
    ]

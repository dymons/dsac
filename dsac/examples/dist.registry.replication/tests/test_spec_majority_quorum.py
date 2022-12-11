async def test_spec_majority_quorum(registry, snapshot):
    """
        This specification illustrates an example of the quorum system.
        To  get  the  most  up-to-date  register  value  from replicas,
        we  need  to  collect  a  quorum from the majority of replicas
        and take the maximum value from it.

             |-W(10, 1)-|   |----W(20, 2)----|
              \  /     /     \ \  /         /
     R1 -------\/-----/-------\-\/---------/------------
                \    /         \          /
                 \  /           \        /
     R2 ----------\/-------------\------/-/\------------
                                  \    / /  \
                                   \  / /    \
     R3 ----------------------------\/-/-/\---\---------
                                      / /  \   \
                                     |-R(20, 2)-|
    """

    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 1})).status == 200
    assert (await registry[8081].post('v1/replica/write', json={'value': 10, 'timestamp': 1})).status == 200

    assert (await registry[8080].post('v1/replica/write', json={'value': 20, 'timestamp': 2})).status == 200
    assert (await registry[8082].post('v1/replica/write', json={'value': 20, 'timestamp': 2})).status == 200

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 20}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 10}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 20}},
    ]

    response = await registry[8080].post('v1/coordinator/read')
    assert response.status_code == 200
    assert response.json() == {
        'value': 20,
        'timestamp': 2,
    }

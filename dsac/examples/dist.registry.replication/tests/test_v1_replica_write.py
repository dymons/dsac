import pytest


@pytest.mark.parametrize(
    "request_json,expected_response_json",
    [
        (
                {},
                {'error_message': 'Input data is incorrect or the required field \'value\' is missing'},
        ),
        (
                {'value': 10},
                {'error_message': 'Input data is incorrect or the required field \'timestamp\' is missing'},
        ),
        (
                {'timestamp': 0},
                {'error_message': 'Input data is incorrect or the required field \'value\' is missing'},
        ),
        (
                {'value': '10', 'timestamp': 0},
                {'error_message': 'Input data is incorrect or the required field \'value\' is missing'},
        ),
        (
                {'value': 10, 'timestamp': '0'},
                {'error_message': 'Input data is incorrect or the required field \'timestamp\' is missing'},
        ),
        (
                {'value': 10, 'timestamp': -1},
                {'error_message': 'Input data is incorrect or the required field \'timestamp\' is missing'},
        ),
    ]
)
async def test_write_to_register_replica_with_invalid_value(
        registry,
        snapshot,
        request_json,
        expected_response_json,
        expected_status_code=400,
):
    response = await registry[8080].post('v1/replica/write', json=request_json)
    assert response.status_code == expected_status_code
    assert response.json() == expected_response_json

    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]


async def test_write_to_register_replica_with_valid_value(
        registry,
        snapshot,
):
    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 0})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8081].post('v1/replica/write', json={'value': 20, 'timestamp': 1})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8082].post('v1/replica/write', json={'value': 30, 'timestamp': 2})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 30}},
    ]


async def test_rewrite_register_replica_with_invalid_input_data(
        registry,
        snapshot,
):
    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 0})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': '0'})).status_code == 400
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]


async def test_rewrite_register_replica_with_valid_input_data(
        registry,
        snapshot,
):
    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 0})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8080].post('v1/replica/write', json={'value': 20, 'timestamp': 1})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]


async def test_rewrite_register_replica_with_olden_timestamp(
        registry,
        snapshot,
):
    """
        The replica can get the old value by the timestamp when writing to the register.
        This  behavior  can  happen  with  an  asynchronous  network  when a TCP packet
        is delivered longer than expected. The ABD algorithm ignores the old value  and
        responds with the status 200 that the record was completed successfully.
    """

    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 10})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 10, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8080].post('v1/replica/write', json={'value': 20, 'timestamp': 1})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 10, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

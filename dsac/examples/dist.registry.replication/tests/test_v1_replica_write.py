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
async def test_invalid_arguments(
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


async def test_replica_set(registry, snapshot):
    assert (await registry[8080].post('v1/replica/write', json={'value': 10, 'timestamp': 0})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8081].post('v1/replica/write', json={'value': 20, 'timestamp': 1})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': None},
    ]

    assert (await registry[8082].post('v1/replica/write', json={'value': 30, 'timestamp': 2})).status == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 0, 'value': 10}},
        {'port': 8081, 'snapshot': {'timestamp': 1, 'value': 20}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 30}},
    ]

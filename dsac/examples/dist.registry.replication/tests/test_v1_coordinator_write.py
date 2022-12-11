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
async def test_write_to_register_coordinator_with_invalid_value(
        registry,
        snapshot,
        request_json,
        expected_response_json,
        expected_status_code=400,
):
    response = await registry[8080].post('v1/coordinator/write', json=request_json)
    assert response.status_code == expected_status_code
    assert response.json() == expected_response_json

    assert await snapshot() == [
        {'port': 8080, 'snapshot': None},
        {'port': 8081, 'snapshot': None},
        {'port': 8082, 'snapshot': None},
    ]


async def test_write_to_register_coordinator_with_valid_value(
        registry,
        snapshot,
):
    assert (await registry[8080].post('v1/coordinator/write', json={'value': 12, 'timestamp': 2})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]


async def test_rewrite_register_coordinator_with_invalid_input_data(
        registry,
        snapshot,
):
    assert (await registry[8080].post('v1/coordinator/write', json={'value': 12, 'timestamp': 2})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]

    response = await registry[8080].post('v1/coordinator/write', json={'value': 10, 'timestamp': '0'})
    assert response.status_code == 400
    assert response.json() == {
        'error_message': 'Input data is incorrect or the required field \'timestamp\' is missing'
    }

    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]


async def test_rewrite_register_coordinator_with_valid_input_data(
        registry,
        snapshot,
):
    assert (await registry[8080].post('v1/coordinator/write', json={'value': 12, 'timestamp': 2})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8081, 'snapshot': {'timestamp': 2, 'value': 12}},
        {'port': 8082, 'snapshot': {'timestamp': 2, 'value': 12}},
    ]

    assert (await registry[8080].post('v1/coordinator/write', json={'value': 24, 'timestamp': 3})).status_code == 200
    assert await snapshot() == [
        {'port': 8080, 'snapshot': {'timestamp': 3, 'value': 24}},
        {'port': 8081, 'snapshot': {'timestamp': 3, 'value': 24}},
        {'port': 8082, 'snapshot': {'timestamp': 3, 'value': 24}},
    ]


async def test_rewrite_register_coordinator_with_olden_timestamp(registry, snapshot):
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

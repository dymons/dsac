#! /bin/bash

pushd tests || exit
pytest
popd || exit

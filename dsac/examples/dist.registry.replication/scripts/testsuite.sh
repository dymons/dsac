#! /bin/bash

if [ ! -d "${PWD}"/static ]; then
  echo "${PWD}/static1 directory does not exist. Please build the project dsac"
  exit 1
fi

REGISTRY_REPLICATION_EXECUTABLE=$(jq '.executable' "${PWD}"/static/config.json)

pushd tests || exit

kill_descendant_processes() {
  local pid="$1"
  local and_self="${2:-false}"
  if children="$(pgrep -P "$pid")"; then
    for child in $children; do
      kill_descendant_processes "$child" true
    done
  fi
  if [[ "$and_self" == true ]]; then
    kill -9 "$pid"
  fi
}

eval "${REGISTRY_REPLICATION_EXECUTABLE}" --port 8081 &
eval "${REGISTRY_REPLICATION_EXECUTABLE}" --port 8082 &
pytest -vvs

kill_descendant_processes $$

popd || exit

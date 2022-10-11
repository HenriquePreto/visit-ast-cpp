#!/bin/bash

BENCHMARKS_LIST=(
    https://github.com/Z3Prover/z3.git
    https://github.com/redis/redis.git
)

BENCHMARKS_PATH="$(pwd)/benchmarks"

mkdir -p ${BENCHMARKS_PATH}

for git_path in ${BENCHMARKS_LIST[@]}; do
    repo_name="$(basename ${git_path})"
    benchmark_name="${repo_name%.*}"
    echo ${benchmark_name}
done

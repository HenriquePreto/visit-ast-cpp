#!/bin/bash

SCRIPT=$(realpath "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

BAZEL_DIR="${SCRIPT_PATH}/.."

# All available tools
CC_TOOLS=("cast" "goto" "nobreak")

# This compiler call clang's tool and the compiler
export MY_CC="${SCRIPT_PATH}/my_cc.sh"

# Z3: https://github.com/Z3Prover/z3.git
Z3_GIT="https://github.com/Z3Prover/z3.git"
read -r -d '' Z3_BUILD <<- EOM
  mkdir build;
  cd build;
  CC=${MY_CC} CXX=${MY_CC} cmake -G "Unix Makefiles" ../;
  make -j8;
EOM

# Benchmark info stored in a list of tuples
export BENCHMARKS_LIST=(
  "${Z3_GIT}","${Z3_BUILD}"
)

# PATH to store all git repositories 
export BENCHMARKS_PATH="${SCRIPT_PATH}/benchmarks"

# PATH to store all output files
export OUTPUT_PATH=${SCRIPT_PATH}/out

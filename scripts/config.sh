#!/bin/bash

SCRIPT=$(realpath "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

export BAZEL_DIR="${SCRIPT_PATH}/.."

# All available tools
export CC_TOOLS=("cast" "goto" "nobreak")

# This compiler call clang's tool and the compiler
export MY_CC="${SCRIPT_PATH}/my_cc.sh"
export MY_CXX="${SCRIPT_PATH}/my_cxx.sh"

# # z3: https://github.com/Z3Prover/z3.git
GIT_Z3="https://github.com/Z3Prover/z3.git"
read -r -d '' BUILD_Z3 <<- EOM
  mkdir -p build;
  cd build;
  make -j8 clean;
  CC=${MY_CC} CXX=${MY_CXX} cmake -G "Unix Makefiles" ../;
  make -j8;
EOM

# sqlite3: https://github.com/sqlite/sqlite
GIT_SQLITE3="https://github.com/sqlite/sqlite.git"
read -r -d '' BUILD_SQLITE3 <<- EOM
  mkdir -p build;
  cd build;
  ../configure;
  make -j8 clean;
  make -j8 CC=${MY_CC} CXX=${MY_CXX};
EOM

# libxml2: https://github.com/GNOME/libxml2
PYTHON_CFLAGS="$(python3-config --cflags)"
PYTHON_CLIBS="$(python3-config --libs)"
GIT_LIBXML2="https://github.com/GNOME/libxml2.git"
read -r -d '' BUILD_LIBXML2 <<- EOM
  ./autogen.sh PYTHON_CFLAGS="${PYTHON_CFLAGS}" PYTHON_LIBS="${PYTHON_CLIBS}";
  make -j8 clean;
  make -j8 CC=${MY_CC} CXX=${MY_CXX};
EOM

# redis: https://github.com/redis/redis
GIT_REDIS="https://github.com/redis/redis.git"
read -r -d '' BUILD_REDIS <<- EOM
  make -j8 clean;
  make -j8 CC=${MY_CC} CXX=${MY_CXX};
EOM

# bzip2
# php
# llvm
# pytorch
# unreal
# blender

# Benchmark info stored in a list of tuples
export BENCHMARKS_LIST=(
  # "${GIT_Z3}","${BUILD_Z3}"
  # "${GIT_SQLITE3}","${BUILD_SQLITE3}"
  # "${GIT_LIBXML2}","${BUILD_LIBXML2}"
  "${GIT_REDIS}","${BUILD_REDIS}"
)

# PATH to store all git repositories 
export BENCHMARKS_PATH="${SCRIPT_PATH}/benchmarks"

# PATH to store all output files
export OUTPUT_PATH=${SCRIPT_PATH}/out

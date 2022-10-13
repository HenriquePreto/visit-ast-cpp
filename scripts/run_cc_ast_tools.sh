#!/bin/bash

function run_tool_on_file () {
  local cc_in="$1" 
  local cc_tool="$2"

  local basename_var="$(basename ${cc_in})"
  local name_var="${basename_var%.*}"

  local cc_out="$3"/${name_var}.out
  echo ${cc_in} ${cc_tool} ${cc_out}
  (cd ..; 
   bazel-bin/src/cc_ast_tool --cc_in ${cc_in} --cc_tool ${cc_tool} > ${cc_out})
}

function run_tools_on_benchmark () {
  local git_path="$1"
  local build_command="$2"
  local repo_name="$(basename ${git_path})"
  local benchmark_name="${repo_name%.*}"
  local benchmark_path="${BENCHMARKS_PATH}/${benchmark_name}"
  
  if [ ! -d ${BENCHMARKS_PATH} ]; then
    echo "Cloning ${git_path}"
    (cd ${BENCHMARKS_PATH}; git clone ${git_path})
  else
    echo "${git_path} cloned"
  fi

  if [ "$(which ${benchmark_name})" = "" ]; then
    echo "Building ${benchmark_name} : ${build_command}"
    (cd ${benchmark_path}; eval ${build_command})
  else
    echo "${benchmark_name} built"
  fi

  local cc_files_out=$(find ${benchmark_path} \
    -name "*.cpp" -o \
    -name "*.c" -o \
    -name "*.cc" -o \
    -name "*.h")
  IFS=$'\n' read -rd '' -a cc_files <<<"${cc_files_out}"
  local cc_tools=("cast" "goto" "nobreak")

  local output_dirname=$(pwd)/"out"
  mkdir -p ${output_dirname}

  # i=0
  for cc_tool in ${cc_tools[@]}; do
    local output_dirname_tool="${output_dirname}/${cc_tool}/${benchmark_name}"
    mkdir -p ${output_dirname_tool}
    for cc_in in ${cc_files[@]}; do
      echo "${cc_in}" "${cc_tool}"
      run_tool_on_file ${cc_in} ${cc_tool} ${output_dirname_tool}
      # i=$((i+1))
      # if [ $i -gt 10 ]; then break; fi
    done
  done
  
}

# list of tuples: (git repository, build commands)
declare -a BENCHMARKS_LIST=(
  "https://github.com/Z3Prover/z3.git","python3 scripts/mk_make.py; cd build; make; sudo make install"
  #"https://github.com/redis/redis.git","make"
)

BENCHMARKS_PATH="$(pwd)/benchmarks"

mkdir -p ${BENCHMARKS_PATH}

for i in "${BENCHMARKS_LIST[@]}"; do
  IFS=","; set -- $i
  echo $1 : $2
  run_tools_on_benchmark $1 $2
done

#!/bin/bash

function clone_repository () {
  if [ ! -d ${2} ]; then
    (cd ${BENCHMARKS_PATH}; git clone ${1})
  fi
  echo "Git repository location of '${3}': ${2}"
}

function build_repository () {
  # if [ "$(which ${1})" = "" ]; then
  #   echo "Building ${1} : ${2}"
    # (cd ${3}; eval ${2})
  (cd ${3}; eval ${2})
  # else
  #   echo "${1} built"
  # fi
}

function run_tools_on_benchmark () {
  local git_path="$1"
  local build_command="$2"
  local repository_name="$(basename ${git_path})"
  local project_name="${repository_name%.*}"
  local repository_path="${BENCHMARKS_PATH}/${project_name}"
  
  clone_repository "${git_path}" "${repository_path}" "${project_name}"
  build_repository "${project_name}" "${build_command}" "${repository_path}"
  
  for cc_tool in ${CC_TOOLS[@]}; do
    project_out_path="${OUTPUT_PATH}/${cc_tool}/${project_name}"
    mkdir -p "${project_out_path}"
    mv "${OUTPUT_PATH}/${cc_tool}/"*.txt "${project_out_path}"
  done
}

source ./config.sh

(cd ${BAZEL_DIR}; bazel build //src:cc_ast_tool)

mkdir -p ${BENCHMARKS_PATH}
mkdir -p "${OUTPUT_PATH}"
for cc_tool in ${CC_TOOLS[@]}; do
  mkdir -p "${OUTPUT_PATH}/${cc_tool}"
done

for i in "${BENCHMARKS_LIST[@]}"; do
  IFS=","; set -- $i
  echo -e "Git repository: ${1}\nBuild command: ${2}"
  run_tools_on_benchmark ${1} ${2}
done

# TODO: count all files and matched files
# Delete all files with less than 4 bytes: '{}\n' empty json is 2 bytes + '\n'
find "${OUTPUT_PATH}" -name "*.txt" -type f -size -4c -delete
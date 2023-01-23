#!/bin/bash

function clone_repository () {
  if [ ! -d ${2} ]; then
    (cd ${BENCHMARKS_PATH}; git clone ${1})
  fi
  echo "Git repository location of '${3}': ${2}"
}

function build_repository () {
  (cd ${3}; eval ${2})
}

function run_tools_on_benchmark () {
  local git_path="$1"
  local build_command="$2"
  local repository_name="$(basename ${git_path})"
  local project_name="${repository_name%.*}"
  local repository_path="${BENCHMARKS_PATH}/${project_name}"
  
  clone_repository "${git_path}" "${repository_path}" "${project_name}"
  build_repository "${project_name}" "${build_command}" "${repository_path}"
  local build_dir="${repository_path}"/build
  jq -M 'unique_by(.file)' "${build_dir}"/compile_commands.json > tmp
  cat tmp > "${build_dir}"/compile_commands.json
  rm tmp
  
  IFS=' '
  for ast_tool in ${CC_TOOLS[@]}; do
    cc_out="${OUTPUT_PATH}/${ast_tool}/${project_name}.txt"
    (cd ${BAZEL_DIR}; \
      bazel run //src:visit_ast --\
        --build_dir="${build_dir}" \
        --ast_tool="${ast_tool}" \
        "${args[@]}" > "${cc_out}")
  done
}

set -e

mkdir -p "$BENCHMARKS_PATH"
mkdir -p "$OUTPUT_PATH"
for cc_tool in ${CC_TOOLS[@]}; do
  mkdir -p "$OUTPUT_PATH"/"$cc_tool"
done

IFS='#'
for i in $BENCHMARKS_LIST; do
  IFS=','; set -- $i
  echo -e "Git repository: $1\nBuild command: $2"
  run_tools_on_benchmark $1 $2
done
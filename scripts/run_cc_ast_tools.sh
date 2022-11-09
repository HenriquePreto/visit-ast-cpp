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
  
  IFS=' '
  for cc_tool in ${CC_TOOLS[@]}; do
    project_out_path="$OUTPUT_PATH"/"$cc_tool"/"$project_name"
    mkdir -p "$project_out_path"
    mv "$OUTPUT_PATH"/"$cc_tool"/*.txt "$project_out_path"
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
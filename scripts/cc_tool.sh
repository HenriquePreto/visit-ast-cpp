#!/bin/bash

SCRIPT=$(realpath "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

source ${SCRIPT_PATH}/config.sh

cc_ins="$(grep -oE "\S*\.(c|cpp|cc)(\s|$)" <<< "$@")"

if [ ! -z "${cc_ins}" ]; then
  for cc_in in ${cc_ins}; do
    base="$(basename ${cc_in})"
    if [ "${base}" = "${cc_in}" ]; then
      cc_in="$(pwd)"/${cc_in}
    fi
    for cc_tool in ${CC_TOOLS[@]}; do
      cc_out="${OUTPUT_PATH}/${cc_tool}/${base}.txt"
      
      (cd ${BAZEL_DIR}; 
       echo -e "$(bazel-bin/src/cc_ast_tool \
        --cc_in "${cc_in}" \
        --cc_tool "${cc_tool}")" > "${cc_out}")
    done
  done
fi
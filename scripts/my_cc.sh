#!/bin/bash

SCRIPT=$(realpath "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

source "${SCRIPT_PATH}"/config.sh

cc_in="$(grep -oE "\S*\.cpp(\s|$)" <<< "$@")"

if [ ! -z "${cc_in}" ]; then
  outname="$(basename ${cc_in})"
  for cc_tool in ${CC_TOOLS[@]}; do
    cc_out="${OUTPUT_PATH}/${cc_tool}/${outname}.txt"
    
    (cd ${BAZEL_DIR}; 
    printf "$(bazel-bin/src/cc_ast_tool \
      --cc_in "${cc_in}" \
      --cc_tool "${cc_tool}")" > "${cc_out}")
  done
fi

clang++ "$@"
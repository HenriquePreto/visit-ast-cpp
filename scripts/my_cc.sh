#!/bin/bash

SCRIPT=$(realpath "$0")
SCRIPT_PATH=$(dirname "$SCRIPT")

source "${SCRIPT_PATH}"/config.sh

clang++ "$@"

cc_in="$(grep -oE "\S*\.cpp(\s|$)" <<< "$*")"

# for cc_tool in ${CC_TOOLS[@]}; do
#   bname="$(basename ${cc_in})"
#   outname="${bname%.*}"
#   cc_out="${OUTPUT_PATH}/${cc_tool}/${outname}.txt"

#   (cd ${BAZEL_DIR}; 
#    bazel-bin/src/cc_ast_tool \
#     --cc_in "${cc_in}" \
#     --cc_tool "${cc_tool}" > "${cc_out}")
# done
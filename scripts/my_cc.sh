#!/bin/bash

# source ./config.sh

clang++ "$@"

# regular_expression="\S*\.cpp(\s|$)"
# cc_in="$(grep -oE ${regular_expression} <<< "$@")"

# for cc_tool in ${CC_TOOLS[@]}; do
#   bname="$(basename ${cc_in})"
#   outname="${bname%.*}"
#   cc_out="${OUTPUT_PATH}/${cc_tool}/${outname}.txt"

#   (cd ${BAZEL_DIR}; 
#    bazel-bin/src/cc_ast_tool \
#     --cc_in "${cc_in}" \
#     --cc_tool "${cc_tool}" > "${cc_out}")
# done
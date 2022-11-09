#!/bin/bash

function is_arg () {
  case "$1" in
    *"my_cxx.sh"* | "-MD" | "-MT" | *".cpp.o"* | *".c.o"* | "-MF" | "-o" | \
     "-MD" | "-c")
      echo false;;
    *)
      echo true;;
  esac
}

function is_cc () {
  if [[ "$1" =~ .*\.c$ ]] || \
     [[ "$1" =~ .*\.cpp$ ]] || \
     [[ "$1" =~ .*\.cc$ ]]
  then
    echo true
  else
    echo false
  fi
}

set -e

declare -a args
declare -a cc_ins
for arg in "$@"; do
  r=$(is_cc "$arg")
  if [ $r = true ]; then
    cc_ins+=("$arg")
    continue
  fi
  r=$(is_arg "$arg")
  if [ $r = true ]; then
    args+=("--undefok")
    args+=("$arg")
  fi
done

if [ ! -z "${cc_ins}" ]; then
  for cc_in in ${cc_ins}; do
    base="$(basename ${cc_in})"
    if [ "${base}" = "${cc_in}" ]; then
      cc_in="$(pwd)"/${cc_in}
    fi
    for cc_tool in ${CC_TOOLS[@]}; do
      cc_out="${OUTPUT_PATH}/${cc_tool}/${base}.txt"
      (cd ${BAZEL_DIR}; \
        bazel run //src:cc_ast_tool --\
         --cc_in="${cc_in}" \
         --cc_tool="${cc_tool}" \
         --ignore_errors="${IGNORE_ERRORS}" \
         "${args[@]}" > "${cc_out}")
    done
  done
fi
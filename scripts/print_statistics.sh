#!/bin/bash

function concat_function_stats() {
  out_dir="$OUTPUT_PATH"/concat_json
  mkdir -p "$out_dir"
  tool="$1"
  repository_name="$2"
  tool_dir="$OUTPUT_PATH"/"$tool"/"$repository_name"
  json_files=$(find "$tool_dir" -name "*.txt")
  IFS=$'\n' read -rd '' -a json_files_list <<< "$json_files"
  output="$out_dir"/"$tool"_"$2".json
  echo '[' > "$output"
  for json_file in ${json_files[@]}; do
    tail -n +2 "$json_file" | sed '$d' >> "$output"
    printf ',' >> "$output"
  done
  truncate -s -1 "$output"
  echo ']' >> "$output"
  echo "$output"
}

function print_function_stats () {
  tool="$1"
  tool_dir="$OUTPUT_PATH"/"$tool"
  echo "-----------------------------------------"
  printf "%s\t|\t%s\t|\t%s\t|\n" "LIBNAME" "ALL" "$tool"
  echo "-----------------------------------------"
  for repository_path in "$tool_dir"/*; do
    repository_name=$(basename $repository_path)
    output_file="$(concat_function_stats "$tool" "$repository_name")"
    count_all="$(jq length "$output_file")"
    # Delete all files with less than 4 bytes: 
    # '{}\n' empty json is 2 bytes + '\n'
    find "${repository_path}" -name "*.txt" -type f -size -4c -delete
    output_file="$(concat_function_stats "$tool" "$repository_name")"
    count_ok="$(jq length "$output_file")"
    printf "%.*s\t|\t%s\t|\t%s\t|\n" 7 \
      ${repository_name} ${count_all} ${count_ok} 
  done
}

# function sort_cast () {
#   json_file="$(concat_function_stats "$tool")"
#   sorted=${OUTPUT_PATH}/cast/sort_all.json
#   cat ${all} | jq -c 'sort_by(.casts, .vars) | reverse | .[]' > ${sorted}
# }

print_function_stats "cast"

print_function_stats "goto"

print_function_stats "nobreak"
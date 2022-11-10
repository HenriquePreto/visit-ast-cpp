#!/usr/local/Cellar/bash/5.2.9/bin/bash

function print_function_stats () {
  out_dir="$OUTPUT_PATH"/stats
  mkdir -p "$out_dir"
  tool="$1"
  tool_dir="$OUTPUT_PATH"/"$tool"
  output="$out_dir"/"$tool"_table.txt
  echo "-----------------------------------------" > "$output"
  printf "%s\t|\t%s\t|\t%s\t|\n" "LIBNAME" "ALL" "$tool" >> "$output"
  echo "-----------------------------------------" >> "$output"
  for repository_path in "$tool_dir"/*; do
    repository_name=$(basename $repository_path)
    total=0
    visited=0
    for json_file in "$repository_path"/*; do
      json_file_total=$(cat "$json_file" | jq .total)
      json_file_visited=$(cat "$json_file" | jq .visited)
      total=$((total + json_file_total))
      visited=$((visited + json_file_visited))
    done
    printf "%.*s\t|\t%s\t|\t%s\t|\n" 7 \
      ${repository_name} ${total} ${visited} >> "$output"
  done
}

function concat_array_files_json() {
  out_dir="$OUTPUT_PATH"/stats
  mkdir -p "$out_dir"
  tool="$1"
  tool_dir="$OUTPUT_PATH"/"$tool"
  json_files=$(find "$tool_dir" -name "*.txt")
  IFS=$'\n' read -rd '' -a json_files_list <<< "$json_files"
  output="$out_dir"/"$tool".json
  echo '[' > "$output"
  for json_file in ${json_files[@]}; do
    # length=$(cat "$json_file" | jq .functions | jq length)
    length=$(cat "$json_file" | jq .visited)
    if [ $length -gt 0 ]; then
      arr_json=$(cat "$json_file" | jq .functions)
      arr_json="${arr_json:2:-2}"
      echo "$arr_json", >> "$output"
    fi
  done
  truncate -s -2 "$output"
  echo ']' >> "$output"
  echo "$output"
}

function print_best_stats () {
  tool="$1"
  attr="$2"
  max=$3
  attr_out="$4"
  json_file="$(concat_array_files_json "$tool")"
  out_dir="$(dirname $json_file)"
  sorted_output="$out_dir"/"$tool"_top_"$3".txt
  cat "$json_file" | \
    jq -c "sort_by($attr) | reverse | .[] | $attr_out" | head -$max \
    > "$sorted_output"
}

MAX=1000

print_function_stats "cast"&

print_function_stats "goto"&

print_function_stats "nobreak"&

print_best_stats "cast" ".casts, .size, .vars, .id" $MAX \
  ".id + \" \" + (.casts | tostring) + \" \" + (.size | tostring) + \
  \" \" + (.vars | tostring)"&

print_best_stats "goto" ".gotos, .labels, .id" $MAX \
  ".id + \" \" + (.gotos | tostring) + \" \" + (.labels | tostring)"&

print_best_stats "nobreak" ".nobreaks, .id" $MAX \
  ".id + \" \" + (.nobreaks | tostring)"&

wait
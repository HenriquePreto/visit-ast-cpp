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
  for repository_file in "$tool_dir"/*; do
    repository_name=$(basename "${repository_file%.*}")
    total=$(cat "$repository_file" | \
      jq -n '[inputs | .total] | reduce .[] as $num (0; .+$num)')
    visited=$(cat "$repository_file" | \
      jq -n '[inputs | .visited] | reduce .[] as $num (0; .+$num)')
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
  output="$out_dir"/"$tool".txt
  rm "$output"
  for json_file in ${json_files[@]}; do
    arr_json=$(cat "$json_file" | jq '.functions | select(length > 0)')
    echo "$arr_json" >> "$output"
  done
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

MAX=15

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
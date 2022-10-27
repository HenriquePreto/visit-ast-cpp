#!/bin/bash

source ./config.sh

# cast statistics
CAST_DIR="${OUTPUT_PATH}"/cast
echo "-----------------------------------------"
printf "%s\t|\t%s\t|\t%s\t|\n" "LIBNAME" "ALL" "CAST"
echo "-----------------------------------------"
for repository_path in ${CAST_DIR}/*; do
  if [ ! -d "$repository_path" ]; then
    continue
  fi
  repository_name=$(basename $repository_path)
  count_all="$(ls ${repository_path} | wc -l)"
  # Delete all files with less than 4 bytes: '{}\n' empty json is 2 bytes + '\n'
  find "${repository_path}" -name "*.txt" -type f -size -4c -delete
  count_ok="$(ls ${repository_path} | wc -l)"
  printf "%s\t|\t%s\t|\t%s\t|\n" ${repository_name} ${count_all} ${count_ok} 
done

cast_files_out=$(find ${CAST_DIR} -name "*.txt")
IFS=$'\n' read -rd '' -a cast_files <<<"${cast_files_out}"
all=${OUTPUT_PATH}/cast/all.json
sorted=${OUTPUT_PATH}/cast/sort_all.json
rm -f ${all} ${sorted}
echo '[' > ${all}
for file in "${cast_files[@]}"; do
  tail -n +2 ${file} | sed '$d' >> ${all}
  printf ',' >> ${all}
done
truncate -s -1 ${all}
echo ']' >> ${all}
cat ${all} | jq -c 'sort_by(.casts, .vars) | reverse | .[]' > ${sorted}

# goto statistics
GOTO_DIR="${OUTPUT_PATH}"/goto
echo "-----------------------------------------"
printf "%s\t|\t%s\t|\t%s\t|\n" "LIBNAME" "ALL" "GOTO"
echo "-----------------------------------------"
for repository_path in ${GOTO_DIR}/*; do
  repository_name=$(basename $repository_path)
  count_all="$(ls ${repository_path} | wc -l)"
  # Delete all files with less than 4 bytes: '{}\n' empty json is 2 bytes + '\n'
  find "${repository_path}" -name "*.txt" -type f -size -4c -delete
  count_ok="$(ls ${repository_path} | wc -l)"
  printf "%s\t|\t%s\t|\t%s\t|\n" ${repository_name} ${count_all} ${count_ok} 
done

# nobreak statistics
NOBREAK_DIR="${OUTPUT_PATH}"/nobreak
echo "-----------------------------------------"
printf "%s\t|\t%s\t|\t%s\t|\n" "LIBNAME" "ALL" "NOBREAK"
echo "-----------------------------------------"
for repository_path in ${NOBREAK_DIR}/*; do
  repository_name=$(basename $repository_path)
  count_all="$(ls ${repository_path} | wc -l)"
  # Delete all files with less than 4 bytes: '{}\n' empty json is 2 bytes + '\n'
  find "${repository_path}" -name "*.txt" -type f -size -4c -delete
  count_ok="$(ls ${repository_path} | wc -l)"
  printf "%s\t|\t%s\t|\t%s\t|\n" ${repository_name} ${count_all} ${count_ok} 
done
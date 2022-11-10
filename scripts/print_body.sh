#!/bin/bash

tool="$1"
function="$2"

STATS_PATH="$OUTPUT_PATH"/stats

cat "$STATS_PATH"/"$tool".json | \
  jq -r ".[] | select(.id==\"$function\") | .body"

if [ "$tool" = "goto" ]; then
  cat "$STATS_PATH"/"$tool".json | \
    jq -r ".[] | select(.id==\"$function\") | .body" | \
    grep "goto" | wc -l
fi
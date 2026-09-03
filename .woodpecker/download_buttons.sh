#!/bin/bash

tail -n-1 src/windows/buttons.tsv | while read line; do
    name="$(echo "$line" | cut -f1)"
    url="$(echo "$line" | cut -f3)"

    echo "$url -> public/buttons/$name"
    curl -m10 -L -sS -o "public/buttons/$name" "$url"
done

find public/buttons/ -iname "*.png" -exec pngcrush -ow -brute {} \;

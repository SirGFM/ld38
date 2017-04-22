#!/bin/bash

i=0
echo -n "" > misc/auto/level_list.h
for file in `ls assets/lvl/*.gfm`; do
    echo "\"lvl/$(basename ${file})\"," >> misc/auto/level_list.h
    i=$((i + 1))
    if [ $(basename ${file}) == "overworld_obj.gfm" ]; then
        echo -n "$((i / 2))" > misc/auto/entry_point.h
    fi
done


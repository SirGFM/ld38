#!/bin/bash

echo -n "" > misc/auto/level_list.h
for file in `ls assets/lvl/*.gfm`; do
    echo "\"lvl/$(basename ${file})\"," >> misc/auto/level_list.h
done


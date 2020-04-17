#!/bin/bash

echo "Run this script from the root of DarkForest repo (so it will be the current directory"

is_nt=false

if [ "$(expr substr $(uname -s) 1 7)" == "MSYS_NT" ]; then
    is_nt=true
fi

if [ "$is_nt" == true ]; then
    fs_scripts/nt/fs_reset.bat
else
    fs_scripts/unix/fs_reset.sh
fi

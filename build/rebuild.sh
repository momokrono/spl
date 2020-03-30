#!/usr/bin/env bash

######################################################################
# @author      : Riccardo Brugo (brugo.riccardo@gmail.com)
# @file        : rebuild
# @created     : Tuesday Mar 31, 2020 00:25:17 CEST
#
# @description : rebuild the project from scratch
######################################################################

# Enable unofficial strict mode
set -euo pipefail # for -u, use `cmd || cmd2` if `cmd` must fail, `cmd || true` if it may fail
IFS=$'\n\t' # internal field separator
# remember to use `trap (cleanup_function) EXIT` for cleanup. To remove trap, use `-` as cleanup_function
# use `readonly` for constants

if [[ "$(pwd | tr '/' '\n' | tail -1)" != "build" ]];
then
    echo "[rebuild] cd build" 1>&2
    cd build
    if [[ "$(pwd | tr '/' '\n' | tail -1)" != "build" ]];
    then
        echo "[rebuild] Error - there must be a \"build\" folder to rebuild the project" 1>&2
        return
    fi
fi

if [[ -d CMakeFiles ]];
then
    rm -Irf CMakeFiles
fi
if [[ -f CMakeCache.txt ]];
then
    rm -Irf CMakeCache.txt
fi

echo "[rebuild] conan install .. --build missing" 1>&2
conan install .. --build missing
echo "[rebuild] cmake .." 1>&2
cmake ..
echo "[rebuild] cmake --build . -j$(nproc)" 1>&2
cmake --build . -j$(nproc)


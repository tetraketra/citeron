#!/bin/bash

# Fixes https://github.com/google/sanitizers/issues/1614.
# Feel free to comment this out if it's unnecessary on your machine.
sudo sysctl vm.mmap_rnd_bits=28 > /dev/null 2>&1
sudo sysctl vm.mmap_rnd_compat_bits=8 > /dev/null 2>&1

# Makes ASAN less of a memory hog during testing.
# This line also exits in `debug_run.sh` for convenience. 
export ASAN_OPTIONS=quarantine_size_mb=128

# GCC args.
FILES=$(find . -print | grep -i "\.c$" | tr -s '\n' ' ')
WARNS="-W -Wall -Wextra -Wno-multichar -Wno-comment -Wno-misleading-indentation -Wno-uninitialized"
FSANS="-fsanitize=address -fsanitize=undefined -fsanitize-address-use-after-scope" # FOR RELEASE, CLEAR THIS AND REMOVE THE DLCLOSE EXPORT
LINKS=""
FLAGS="-Og -g3 -D BUILDFLAG_DEBUG -D BUILDFLAG_LINUX" # BUILDFLAG_WINDOWS, BUILDFLAG_MAC
INCLD=""

echo "\n\nExecuting with..."

echo "FILES: $FILES"
echo "WARNS: $WARNS"
echo "FSANS: $FSANS"
echo "LINKS: $LINKS"
echo "FLAGS: $FLAGS"
echo "INCLD: $INCLD"

# Build and build time(s).
echo "\nBuilding and reporting build time(s)..."

BUILD_START=$(date +%s.%N)
mkdir -p ./citeron/build
gcc-12 \
    $FILES \
    $WARNS \
    $LINKS \
    $FLAGS \
    $FSANS \
    $INCLD \
    -o ./citeron/build/citeron_debug
BUILD_END=$(date +%s.%N)

DIFF=$(echo "$BUILD_END $BUILD_START - p" | dc | xargs printf "%.2f")
echo "./everything"
echo " TOTAL $DIFF"

# Make executable, well, executable.
find ./citeron/build -type f -print -quit | xargs chmod a+x

echo "\n\nExecute \"sh debug_run.sh\" to start the program."
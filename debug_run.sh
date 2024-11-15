#!/bin/bash

# Closing a dll leaks according to ASAN. I include this line (and related file) 
# in every project to keep my ASAN cleaner, even if it's not strictly necessary.
# This is not likely to work on your machine because of the absolute path.
# Comment this line out if you want; it shouldn't change anything in most cases.
export LD_PRELOAD="/usr/lib/gcc/x86_64-linux-gnu/12/libasan.so ./dlclose.so"

# Makes ASAN less of a memory hog during testing.
# This line also exits in `debug_build.sh` for convenience. 
export ASAN_OPTIONS=quarantine_size_mb=128

# Launch.
./citeron/build/citeron_debug
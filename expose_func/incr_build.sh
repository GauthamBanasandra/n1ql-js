#!/usr/bin/env bash
g++ -std=c++11 incr_build.cpp -I/Users/gautham/.cbdepscache/ -L/Users/gautham/.cbdepscache/lib/ -lv8 -o incr_build &&
install_name_tool -change @rpath/libv8.dylib /Users/gautham/.cbdepscache/lib/libv8.dylib incr_build &&
./incr_build inputs/input_incr.js

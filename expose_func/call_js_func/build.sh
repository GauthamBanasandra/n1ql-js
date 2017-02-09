#!/usr/bin/env bash
g++ -std=c++11 call_js.cpp -I/Users/gautham/.cbdepscache/ -L/Users/gautham/.cbdepscache/lib/ -lv8 -o call_js &&
install_name_tool -change @rpath/libv8.dylib /Users/gautham/.cbdepscache/lib/libv8.dylib call_js &&
./call_js ../inputs/input_call_js.js

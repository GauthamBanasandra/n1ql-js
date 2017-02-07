#!/usr/bin/env bash
g++ -std=c++11 "$1".cpp -I/Users/gautham/.cbdepscache/ -L/Users/gautham/.cbdepscache/lib/ -lv8 -o "$1"
install_name_tool -change @rpath/libv8.dylib /Users/gautham/.cbdepscache/lib/libv8.dylib "$1"
#!/usr/bin/env bash
g++ -std=c++11 jsify/jsify.cpp n1ql_js.cpp main.cpp v8/V8Env.cpp utils/Utils.cpp v8/QueryBuilder.cpp v8/ExecJs.cpp query_exec/QueryEngine.cpp -I/Users/gautham/.cbdepscache/ -L/Users/gautham/.cbdepscache/lib/ -lv8 -ll -lcouchbase -o n1ql_js &&
install_name_tool -change @rpath/libv8.dylib /Users/gautham/.cbdepscache/lib/libv8.dylib n1ql_js &&
./n1ql_js inputs/input_jsify.txt | js-beautify
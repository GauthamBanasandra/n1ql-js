#!/usr/bin/env bash
g++ jsify/jsify.cpp n1ql_js.cpp main.cpp -ll &&
./a.out | js-beautify
#!/usr/bin/env bash
cp ../n1ql_js/n1ql_js ./bin/ &&
./bin/n1ql_js ../n1ql_js/inputs/input_db_test.txt | js-beautify
#!/usr/bin/env bash
lex n1ql_js_lex.l
mv lex.yy.c n1ql_js_lex.cpp
g++ n1ql_js_lex.cpp main.cpp -ll
./a.out
rm a.out
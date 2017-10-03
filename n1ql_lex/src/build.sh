#!/usr/bin/env bash
lex n1ql_js_lex.l
mv lex.yy.c n1ql_js_lex.cpp
sed -i "" 's/register//' n1ql_js_lex.cpp
g++ -std=c++11 n1ql_js_lex.cpp main.cpp -ll
./a.out
rm a.out
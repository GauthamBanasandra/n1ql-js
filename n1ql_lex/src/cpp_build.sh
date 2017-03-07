#!/usr/bin/env bash
lex str_input_sep.l
mv lex.yy.c str_input_sep.cpp
g++ str_input_sep.cpp main.cpp -ll
./a.out
rm a.out
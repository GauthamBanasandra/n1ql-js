#!/usr/bin/env bash
lex -o jsify.cpp jsify.l
sed -i "" 's/register//' jsify.cpp
g++ -std=c++11 jsify.cpp main.cpp -ll
./a.out
rm a.out jsify.cpp
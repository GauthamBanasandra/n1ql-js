#!/usr/bin/env bash
g++ jsify/jsify.cpp main.cpp -ll &&
./a.out | js-beautify
#!/bin/bash

# calc build script.

CALC_SOURCES=$(find ./src -type f | grep -v 'result_test.c')
RT_SOURCES=$(find ./src -type f | grep -v 'calc.c')
CC="clang -g -Wall -lm"
INCLUDE=-I./include/
DEFINE="-DNO_GNU_READLINE -DC99_AVAILABLE"
echo $DEFINE

rm calc result_test

echo $SOURCES

$CC -g -Wall $DEFINE $INCLUDE $CALC_SOURCES -o calc
echo ""
echo "------------------------------------------------"
echo ""
$CC -g -Wall $DEFINE $INCLUDE $RT_SOURCES -o result_test

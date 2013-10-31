#!/bin/bash

# calc build script.

CALC_SOURCES=$(find ./src -maxdepth 1 -type f)
RT_SOURCES=$(find ./src -type f | grep -vF -e 'calc.c' -e 'memusage.c')
#MU_SOURCES=$(find ./src -type f | grep -vF -e 'calc.c' -e 'result_test.c')

CC="clang -g -Wall -lm"

INCLUDE="-I./include/"
DEFINE="-DNO_GNU_READLINE -DLONG_DOUBLE_PRECISION -DC99_AVAILABLE"

rm calc result_test memusage

function separator() {
	printf "\n--------------------------------\n"
}

$CC -g -Wall $DEFINE $INCLUDE $CALC_SOURCES -o calc
separator
$CC -g -Wall $DEFINE $INCLUDE $RT_SOURCES -o result_test
separator
#$CC -g -Wall $DEFINE $INCLUDE $MU_SOURCES -o memusage_test 


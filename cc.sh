#!/bin/bash

# calc build script.

CALC_SOURCES=$(find ./src -maxdepth 1 -type f)
RT_SOURCES=$(find ./src -type f | grep -vF -e 'calc.c' -e 'memusage.c')
#MU_SOURCES=$(find ./src -type f | grep -vF -e 'calc.c' -e 'result_test.c')

#CC="clang -g -Wall -lm"
CC="clang -g -Wall -lm -lmpfr -lgmp"
#CC="clang -g -Wall -lm -lreadline"

INCLUDE="-I./include/"

DEFINE="-DLONG_DOUBLE_PRECISION"
#DEFINE="-DUSE_MPFR"

function clean() {
	rm calc result_test memusage
}

clean

$CC -g -Wall $DEFINE $INCLUDE $CALC_SOURCES -o calc 
printf "\n--------------------------------\n"
$CC -g -Wall $DEFINE $INCLUDE $RT_SOURCES -o result_test
printf "\n--------------------------------\n"
#$CC -g -Wall $DEFINE $INCLUDE $MU_SOURCES -o memusage_test 


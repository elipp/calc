#include <stdio.h>

#include "definitions.h"
#include "termtree.h"

typedef struct {
	char* expr;
	fp_t result;
} test_t;

// correct results gathered from wolframalpha
static const test_t tests[] = {
{ "(5+1*2)-23^2+1", -521 },
{ "sin(e*sin(sinh(pi/2)))", 0.8987498494059207598786 },
{ "fact(54/6) + 1/2", 362880.5 },
{ "(2.1)^(-1.2)", 0.410522387808581614598 },
{ "(e)^(2*pi+2*e)", 122976.5497941034940 },
{ "e^(2*pi+2*e)", 122976.5497941034940 },
{ "sin(sin(sin(-1.0)))", -0.6784304773607 },
{ "2*((25+((532+1)/2^2)+2.1*2*3/(1.0-2.0)*sin(1.53)))/3.0 + 1", 98.106989259719 },
{ "-(1.35*3*sin(2))/(2+1)", -1.227551526214670 },
{ "-sin(5 + sin( cos(2.0)/2.0))", 0.996718399031256 },
{ "sin(0.3*pi^3)^3*2  / 31.31+531*(-999*33.25/833-1*e*log(20/tan(20)) - (e/pi+1)^2/3.33)/(301.3+53*sin(31*cos(445)))", -65.4999835337342775 }

};


#ifdef LONG_DOUBLE_PRECISION
#define REPORT(got, should_be) do { printf("got\t\t%.14Lg\nshould be\t%.14Lg\n\n", got, should_be); } while(0)
#else
#define REPORT(got, should_be) do { printf("got\t\t%.14g\nshould be \t%.14g\n\n", got, should_be); } while(0)
#endif

static const fp_t threshold = 0.00001;

int main(int argc, char* argv[]) {
	static const size_t tests_size = sizeof(tests)/sizeof(tests[0]);
	int i = 0;
	int passed = 0;

	puts("calc: Running a (very) small test suite, testing the parser for correctness, not accuracy.\n\n");
	while (i < tests_size) {
		printf("expr: \"%s\"\n", tests[i].expr);

		fp_t real_res;
		if (!parse_mathematical_input(tests[i].expr, &real_res)) {
			//nop
		}

		REPORT(real_res, tests[i].result);
		fp_t delta = tests[i].result - real_res;

		if (fabsl(delta) > threshold) {
			printf("\033[1;31mFAIL! (result delta exceeded rough threshold value %f!)\033[m\n", (double)threshold);
		} else { 
			puts("\033[1;32mPASS. :)\033[m\n");
			++passed; 
		}
		++i;
//		free(current_expr);	// will leak, but hey; its a test program :D
		printf("\n");
	}

	printf("RESULTS:\npassed: %d\ntotal:  %lu.\n", passed, tests_size); 
	return 0; 
}

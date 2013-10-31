#include "chem/chem.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "termtree.h"

static fp_t get_atomic_mass(const char*, int*);

static fp_t get_atomic_mass(const char* arg, int *found) {

	int i = 0;
	while (i < atomic_weight_table_size) {
		if (strcmp(arg, atomic_weight_table[i].key) == 0) break;
		++i;
	}

	if (i >= atomic_weight_table_size) {
		printf("get_atomic_mass: chemical element \"%s\" not found in the table!\n", arg);
		*found = 0;
		return 0.0;
	}
	
	else {
		*found = 1;
		return atomic_weight_table[i].value;
	}

}

fp_t func_molar_mass(const char* arg) {

	// the func_pass_get_result passes the inner argument WITH the outer braces included.
	// also, will need better error handling
	
	char *stripped = strip_surrounding_parentheses(arg);
	
	stripped = strip_all_whitespace(stripped);
	size_t stripped_len = strlen(stripped);	// kinda redundant though

	size_t i = 0;
	fp_t sum = 0.0;

	while (i < stripped_len) {
		if (stripped[i] == '(') {
			// find matching parenthesis	
			const size_t par_beg = i;
			do {
				++i;
			}
			while (stripped[i] != ')' && i < stripped_len);

			if (i >= stripped_len) {
				printf("func_molar_mass: error: unmatched parenthesis!\n");
				break;
			}

			const size_t par_end = i+1;	// for some reason, this is required

			char *parenthesized = substring(stripped, par_beg, par_end - par_beg);
			fp_t inner_molar_mass = func_molar_mass(parenthesized);
		
			++i;

			fp_t factor = 1.0;
			size_t factor_beg_pos = i;
			while (DIGIT(stripped[i]) && i < stripped_len) {
				++i;
			}
			size_t factor_end_pos = i;
			if (i != factor_beg_pos) {
				// trailing digits were found, strtol(d)
				char *factor_string = substring(stripped, factor_beg_pos, factor_end_pos - factor_beg_pos);
				char *endptr;
				factor = to_double_t(factor_string, &endptr);
				sa_free(factor_string);
			} 

			free(parenthesized);
			sum += factor * inner_molar_mass;

		}
		else if (UPPERCASE(stripped[i])) {
			size_t elem_beg_pos = i;
			// go forth until another uppercase letter is encountered

			do {	// one of the few instances where do...while is actually useful
				++i;
			} while (LOWERCASE(stripped[i]) && i < stripped_len);
		
			size_t elem_end_pos = i;

			// find out if there's a digit char trailing the element

			fp_t factor = 1.0;
			size_t factor_beg_pos = i;
			while (DIGIT(stripped[i]) && i < stripped_len) {
				++i;
			}
			size_t factor_end_pos = i;
			if (i != factor_beg_pos) {
				// trailing digits were found, strtol(d)
				char *factor_string = substring(stripped, factor_beg_pos, factor_end_pos - factor_beg_pos);
				char *dummy;	// required by strtold 
				factor = strtold(factor_string, &dummy);
				free(factor_string);
			} 

			char *elem = substring(stripped, elem_beg_pos, elem_end_pos-elem_beg_pos);
			int found;
			fp_t elem_molar_mass = get_atomic_mass(elem, &found);
			free(elem);

			if (found == 0) {
				sum=0;
				break;
			}
			else {
				sum += factor * elem_molar_mass;
			}

		}

	}

	free(stripped);

	return sum;
}

char* func_balance_chemeq(const char* arg) {
	// stub
	return NULL;
}
fp_t func_DBE(const char* arg) {	// the acronym "DBE" refers to the term Double Bond Equivalent, which
	// stub
	return 0;
}



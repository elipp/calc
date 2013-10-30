#ifndef CHEM_H
#define CHEM_H

#include "../definitions.h"
#include "atomic_weights.h"

fp_t func_molar_mass(const char*);
char* func_balance_chemeq(const char*);
fp_t func_DBE(const char*);

#endif

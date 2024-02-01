#ifndef _RANDLIB_H_
#define _RANDLIB_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typeutil.h"

double random2();
int32_t random3();
void srandom2(uint32_t num);
void readseed();
void writeseed();
double normal();
double dexprand();

#endif /* _RANDLIB_H_ */

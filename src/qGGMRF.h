#ifndef ____get_btilde__
#define ____get_btilde__

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "typeutil.h"

double get_btilde(double delta, double b, double sigma_x, double p, double q,
                  double T);
double get_rho(double delta, double b, double sigma_x, double p, double q,
               double T);

#endif /* defined(____get_btilde__) */

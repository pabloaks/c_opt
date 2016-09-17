#ifndef backtest_h
#define backtest_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basic_pricer.h"

double realized_pl(double *spot_series, double notional, double k, double v, double t, double ird, double irf, double hv, int is_call);

double be_vol(double *spot_series, double k, double hv, double t, double ird, double irf);

#endif /*backtest_h */

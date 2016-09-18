#ifndef backtest_h
#define backtest_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basic_pricer.h"

double realized_pl(double *s_arr, double notional, double k, double v, double t, double ird, double irf, double hv, int is_call);

double be_vol(double *s_arr, double k, double hv, double t, double ird, double irf);

double realized_pl_end(double *s_arr, double notional, double k, double v, double st_t, double ed_t, double ird, double irf, double hv, int is_call, double sell_v);

double be_vol_end(double *s_arr, double k, double hv, double st_t, double ed_t, double ird, double irf, double sell_v);

#endif /*backtest_h */

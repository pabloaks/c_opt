#ifndef basic_pricer_h
#define basic_pricer_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double df(double rate, double t);

double phi(double x);

double norm_CDF_inv(double p);

double bs_price(double s, double k, double v, double t, int is_call, double ird, double irf, int is_prem_for);

double bs_call(double s, double k, double v, double t, double ird, double irf);

double bs_put(double s, double k, double v, double t, double ird, double irf);

double bs_str(double s, double k, double v, double t, double ird, double irf);

double bs_delta(double s, double k, double v, double t, int is_call, double ird, double irf);

double implied_vol_bs(double p, double s, double k, double t, int is_call, double ird, double irf, int is_prem_for);

double implied_vol_str(double p, double s, double k, double t, double ird, double irf);

double bs_strike(double s, double delta, double v, double t, int is_call, double ird, double irf, int is_prem_for);

double realized_vol(double *prices, double t_yrs);
#endif /*basic_pricer_h */

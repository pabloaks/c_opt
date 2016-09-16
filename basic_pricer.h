#ifndef basic_pricer_h
#define basic_pricer_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double phi(double x);
double norm_CDF_inv(double p);
double bs_price(double s, double k, double v, double t, int is_call, double ird, double irf, int is_prem_for);
double bs_delta(double s, double k, double v, double t, int is_call, double ird, double irf);
double implied_vol_bs(double p, double s, double k, double t, int is_call, double ird, double irf, int is_prem_for);

double be_vol(double *spot_series, double k, double hv, double t, double ird, double irf);
double bs_strike(double s, double delta, double v, double t, int is_call, double ird, double irf, int is_prem_for);
#endif /*basic_pricer_h */

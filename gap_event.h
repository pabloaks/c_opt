#ifndef gap_event_h
#define gap_event_h
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "basic_pricer.h"

double implied_gap(double k, double s, double t, double ird, double irf, double pre_v, double post_v, double up_f);

double post_vol(double k, double s, double t, double ird, double irf, double pre_v, double egap, double up_f);

double pre_vol(double k, double s, double t, double ird, double irf, double post_v, double egap, double up_f);

#endif /*gap_event_h*/

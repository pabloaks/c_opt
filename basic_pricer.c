#include "basic_pricer.h"

double df(double rate, double t){
    return 1.0 / powl( 1.0 + rate, t);
}

double phi(double x){
    double a[] =  {0.254829592, -0.284496736, 1.421413741, -1.453152027,1.061405429}; 
    double p  =  0.3275911;
    int sign = 1;
    if(x < 0)
        sign = -1;
    double xx = fabs(x) / sqrt(2.0);
    double t = 1.0 / (1.0 + p * xx);
    double y;
    y = 1.0 - (((((a[4]*t + a[3])*t) + a[2])*t + a[1])*t + a[0])*t*exp(-xx*xx);
    return 0.5*(1.0 + sign*y);
}

double norm_CDF_inv(double p){
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    double t;
    if(p < 0.5){
        t = sqrt(-2.0 * log(p));
        t -= ((c[2]*t + c[1])*t + c[0])/(((d[2]*t + d[1])*t + d[0])*t + 1.0);
        return -t;
    }
    else{
        t = sqrt(-2.0 * log(1-p));
        t -= ((c[2]*t + c[1])*t + c[0])/(((d[2]*t + d[1])*t + d[0])*t + 1.0);
        return t;
    }
}

double bs_price(double s, double k, double v, double t, int is_call, double ird, double irf, int is_prem_for){
    double f = s * df(ird, t) / df(irf, t);
    double d1 = (log(f/k) + v*v * t/2) / (v * sqrt(t));
    double d2 = d1 - v * sqrt(t);
    double div_by;
    if(is_prem_for)
        div_by = k / df(ird, t);
    else
        div_by = s / df(irf, t);
    if(is_call)
        return (f * phi(d1) - k * phi(d2)) / div_by;
    else
        return (k * phi(-d2) - f * phi(-d1)) / div_by;
}

double bs_delta(double s, double k, double v, double t, int is_call, double ird, double irf){
    double f = s * df(ird, t) / df(irf, t);
    double d1 = (log(f/k) + v*v * t/2.0) / (v * sqrt(t));
    if(is_call)
        return phi(d1);
    else
        return phi(d1) - 1.0;
}

double implied_vol_bs(double p, double s, double k, double t, int is_call, double ird, double irf, int is_prem_for){
    double hv = 2.00;
    double epsilon = 0.00000001;
    double lv = 0.00;
    int i = 0;
    double mv = 888.88;
    while(((hv - lv) >= epsilon) && (i < 100)){
        mv = (lv + hv) / 2.0;
        if(bs_price(s, k, mv, t, is_call, ird, irf, is_prem_for) > p)
            hv = mv;
        else
            lv = mv;
        ++i;
    }
    return mv;
}

double be_vol(double *spot_series, double k, double hv, double t, double ird, double irf){
    double delta = 0.0;
    double hedge_cost = 0.0;
    double opt_payout;
    int ns = 145;
    double df_dom = df(ird, t);
    double fwd = spot_series[0] * df(ird, t) / df(irf, t);
    int is_call;
    if(fwd < k){
        is_call = 1;
        opt_payout = fmax(0, spot_series[ns-1] - k) / spot_series[ns-1];
    }
    else{
        is_call = 0;
        opt_payout = fmax(0, k - spot_series[ns-1]) / spot_series[ns-1];
    }
    int i;
    double temp_exp;
    double fwd_factor;
    for(i = 1; i < ns; ++i){
        temp_exp = t * (ns - i) / (ns - 1);
        fwd_factor = df(ird, temp_exp) / df(irf, temp_exp);
        double temp_delta = bs_delta(spot_series[i-1], k, hv, temp_exp, is_call, ird, irf) * -k;
        hedge_cost += (temp_delta - delta) / (spot_series[i-1]*fwd_factor);
        delta = temp_delta;
    }
    hedge_cost -= delta/spot_series[ns-1];
    double option_premium = (hedge_cost + opt_payout) * df_dom;
    if(option_premium > 0)
        return implied_vol_bs(option_premium, spot_series[0], k, t, is_call, ird, irf, 0);
    else
        return -implied_vol_bs(-option_premium, spot_series[0], k, t, is_call, ird, irf, 0);
}

double bs_strike(double s, double delta, double v, double t, int is_call, double ird, double irf, int is_prem_for){
    double f = s * df(ird, t) / df(irf, t);
    if(is_call)
        return f/exp(norm_CDF_inv(delta)*v*sqrt(t) - v*v*t/2.0);
    else 
        return f/exp(norm_CDF_inv(1 - delta)*v*sqrt(t) - v*v*t/2.0);
}

#include "backtest.h"

int len_arr(double *arr){
    double *p = arr;
    int i = 0;
    while(*p){
        ++i;
        ++p;
    }
    return i;
}

double realized_pl(double *s_arr, double notional, double k, double v, double t, double ird, double irf, double hv, int is_call){
    double delta = 0.0;
    double hedge_cost = 0.0;
    int ns = len_arr(s_arr);
    double df_dom = df(ird, t);
    double opt_prem;
    double opt_payout;
    int i;
    double temp_exp;
    double fwd_factor;
    double temp_delta;

    opt_prem = notional/df_dom*bs_price(s_arr[0], k, v, t, is_call, ird, irf, 0);

    if(is_call)
        opt_payout = notional * fmax(0, s_arr[ns-1] - k) / s_arr[ns-1];
    else
        opt_payout = notional * fmax(0, k - s_arr[ns-1]) / s_arr[ns-1];

    for(i = 1; i < ns; ++i){
        temp_exp = t * (ns - i) / (ns - 1);
        fwd_factor = df(ird, temp_exp) / df(irf, temp_exp);
        temp_delta = notional * bs_delta(s_arr[i-1], k, hv, temp_exp, is_call, ird, irf) * -k;
        hedge_cost += (temp_delta - delta) / (s_arr[i-1] * fwd_factor);
        delta = temp_delta;
    }
    hedge_cost -= delta/s_arr[ns-1];

    return opt_payout + hedge_cost - opt_prem;
}


double be_vol(double *s_arr, double k, double hv, double t, double ird, double irf){
    double delta = 0.0;
    double hedge_cost = 0.0;
    double opt_payout;
    int ns = len_arr(s_arr);
    double df_dom = df(ird, t);
    double fwd = s_arr[0] * df(ird, t) / df(irf, t);
    int is_call;
    int i;
    double temp_exp;
    double fwd_factor;
    double temp_delta;
    double opt_prem;
    
    if(fwd < k){
        is_call = 1;
        opt_payout = fmax(0, s_arr[ns-1] - k) / s_arr[ns-1];
    }
    else{
        is_call = 0;
        opt_payout = fmax(0, k - s_arr[ns-1]) / s_arr[ns-1];
    }
    
    for(i = 1; i < ns; ++i){
        temp_exp = t * (ns - i) / (ns - 1);
        fwd_factor = df(ird, temp_exp) / df(irf, temp_exp);
        temp_delta = bs_delta(s_arr[i-1], k, hv, temp_exp, is_call, ird, irf) * -k;
        hedge_cost += (temp_delta - delta) / (s_arr[i-1] * fwd_factor);
        delta = temp_delta;
    }
    
    hedge_cost -= delta/s_arr[ns-1];
    opt_prem = (hedge_cost + opt_payout) * df_dom;
    
    if(opt_prem > 0)
        return implied_vol_bs(opt_prem, s_arr[0], k, t, is_call, ird, irf, 0);
    else
        return -implied_vol_bs(-opt_prem, s_arr[0], k, t, is_call, ird, irf, 0);
}

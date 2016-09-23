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
    double opt_prem, opt_payout, temp_exp, fwd_factor, temp_delta;
    int i;

    opt_prem = notional / df_dom*bs_price(s_arr[0], k, v, t, is_call, ird, irf, 0);

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
    int ns = len_arr(s_arr);
    double df_dom = df(ird, t);
    double fwd = s_arr[0] * df(ird, t) / df(irf, t);
    double opt_payout, opt_prem, temp_exp, fwd_factor, temp_delta;
    int is_call, i;
    
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
        return implied_vol_bs(opt_prem,s_arr[0], k, t,is_call,ird,irf,0);
    else
        return -implied_vol_bs(-opt_prem,s_arr[0], k, t,is_call,ird,irf,0);
}

double realized_pl_end(double *s_arr, double notional, double k, double v, double st_t, double ed_t, double ird, double irf, double hv, int is_call, double sell_v){
    double delta = 0.0;
    double hedge_cost = 0.0;
    int ns = len_arr(s_arr);
    double df_dom = df(ird, st_t);
    double term = st_t - ed_t;
    double op_prem, op_payout, temp_exp, fwd_factor, temp_delta;
    int i;

    op_prem = notional / df_dom * bs_price(s_arr[0], k, v, st_t, is_call, ird, irf, 0);

    if(ed_t == 0)
        return realized_pl(s_arr, notional, k, v, st_t,ird,irf, hv,is_call);

    df_dom = df(ird, ed_t);
    op_payout = notional / df_dom * bs_price(s_arr[ns-1], k, sell_v, ed_t, is_call, ird, irf, 0);

    for(i = 1; i <= ns; ++i){
        temp_exp = term * (ns - i) / (ns - 1) + ed_t;
        fwd_factor = df(ird, temp_exp) / df(irf, temp_exp);
        temp_delta = notional * bs_delta(s_arr[i-1], k, hv, temp_exp, is_call, ird, irf) * -k;
        hedge_cost += (temp_delta - delta) / (s_arr[i-1] * fwd_factor);
        delta = temp_delta;
    }
    hedge_cost -= delta/(s_arr[ns-1]*fwd_factor);
    return op_payout + hedge_cost - op_prem;
}

double be_vol_end(double *s_arr, double k, double hv, double st_t, double ed_t, double ird, double irf, double sell_v){
    double delta = 0.0;
    double hedge_cost = 0.0;
    int ns = len_arr(s_arr);
    double df_dom = df(ird, st_t);
    double term = st_t - ed_t;
    double fwd = s_arr[0] * df(ird, st_t) / df(irf, st_t);
    int is_call, i;
    double op_payout, op_prem, temp_exp, fwd_factor, temp_delta;
    
    if(fwd < k)
        is_call = 1;
    else
        is_call = 0;

    if(ed_t == 0)
        return be_vol(s_arr, k, hv, st_t, ird, irf);

    df_dom = df(ird, ed_t);
    op_payout = bs_price(s_arr[ns-1], k, sell_v, ed_t, is_call, ird, irf, 0) / df_dom;

    for(i = 1; i <= ns; ++i){
        temp_exp = term * (ns - i) / (ns - 1) + ed_t;
        fwd_factor = df(ird, temp_exp) / df(irf, temp_exp);
        temp_delta = bs_delta(s_arr[i-1], k, hv, temp_exp, is_call, ird, irf) * -k;
        hedge_cost += (temp_delta - delta) / (s_arr[i-1] * fwd_factor);
        delta = temp_delta;
    }
    hedge_cost -= delta/(s_arr[ns-1] * fwd_factor);
    op_prem = (hedge_cost + op_payout) * df_dom;

    if(op_prem > 0)
        return implied_vol_bs(op_prem, s_arr[0],k,st_t,is_call,ird,irf,0);
    else
        return -implied_vol_bs(-op_prem, s_arr[0],k,st_t,is_call,ird,irf,0);
}

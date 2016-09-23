#include "gap_event.h"

double implied_gap(double k, double s, double t, double ird, double irf, double pre_v, double post_v, double up_f){
    /* will bump the strike by the event gap, instead of the spot, this
     * way there is no need to worry about the currency of the p/l, since
     * spot is not moving, need to think about this more but otherwise 
     * results seems off, way too uneven
     * up_factor = magnitude move up / magnitude move down
     * returns magnitude of move down */
    double p_u = 1.0 / (1.0 + up_f);
    double p_d = 1.0 - p_u;
    double curr_prem = bs_str(s, k, pre_v, t, ird, irf);
    double gap_l = 0.0;
    double gap_h = pre_v*sqrt(3.0*t*M_PI)*1.1*fmax(1.0, 1.0/up_f);
    /* for low strikes can potentially pass a -ve strile in bs_str
     * for very high up_factor, so need to be careful, and way to
     * control it is with gap_high (looks ok for now) */
    double m_gap = (gap_l + gap_h) / 2.0;
    double epsilon = 1.0e-8;
    int i = 0;
    double prem_d, prem_u, temp_prem;
    while (((gap_h - gap_l) >= epsilon) && (i < 100)){
        m_gap = (gap_l + gap_h) / 2.0;
        prem_d = bs_str(s, k*(1 + m_gap), post_v, t, ird, irf);
        prem_u = bs_str(s, k*(1 - m_gap*up_f), post_v, t, ird, irf);
        temp_prem = prem_d*p_d + prem_u*p_u;
        if (temp_prem > curr_prem)
            gap_h = m_gap;
        else
            gap_l = m_gap;
        ++i;
    }
    return m_gap;
}

double post_vol(double k, double s, double t, double ird, double irf, double pre_v, double egap, double up_f){
    /* possible issues: not sure temp_prem is monotonic fn of vol
     * for big post_vol doesn't always match
     * unstable solutions so possible to get some errors
     * need to check for prem(pre_v) > gap, otherwise can't solve */
    double p_u = 1.0 / (1.0 + up_f);
    double p_d = 1.0 - p_u;
    double prem_b = bs_str(s, k, pre_v, t, ird, irf);
    double vol_l = 0.0;
    double vol_h = 10.0 * pre_v;
    double m_vol = (vol_l + vol_h) / 2.0;
    double epsilon = 1.0e-8;
    int i = 0;
    double prem_d, prem_u, temp_prem;
    while (((vol_h - vol_l) >= epsilon) && (i < 100)){
        m_vol = (vol_l + vol_h) / 2.0;
        prem_d = bs_str(s, k*(1 + egap), m_vol, t, ird, irf);
        prem_u = bs_str(s, k*(1 - egap*up_f), m_vol, t, ird, irf);
        temp_prem = prem_d*p_d + prem_u*p_u;
        if (temp_prem > prem_b)
            vol_h = m_vol;
        else
            vol_l = m_vol;
        ++i;
    }
    return m_vol;
}

double pre_vol(double k, double s, double t, double ird, double irf, double post_v, double egap, double up_f){
    double p_u = 1.0 / (1.0 + up_f);
    double p_d = 1.0 - p_u;
    double prem_d = bs_str(s, k*(1 + egap), post_v, t, ird, irf);
    double prem_u = bs_str(s, k*(1 - egap*up_f), post_v, t, ird, irf);
    double prem_post = prem_d*p_d + prem_u*p_u;
    return implied_vol_str(prem_post, s, k, t, ird, irf);
}


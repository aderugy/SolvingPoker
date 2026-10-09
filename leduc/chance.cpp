//
// Created by aderugy on 09/10/2026.

#include "chance.h"

double p_deal_preflop(const int c_oop, const int c_ip) {
    return (c_oop == c_ip ? 2.0 : 4.0) / 30.0;
}

double p_deal_postflop(const int c_flop, const int c_oop, const int c_ip) {
    if (c_flop == c_oop && c_oop == c_ip) {
        return 0.0;
    }

    if (c_flop == c_oop || c_oop == c_ip || c_flop == c_ip) {
        return 4.0 / 120.0;
    }

    return 8.0 / 120.0;
}
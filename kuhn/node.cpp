//
// Created by aderugy on 15/09/2026.
//

#include "node.h"

#include <stdexcept>

#include "consts.h"
#include "player.h"

static char rank(const char c) {
    switch (c) {
        case 'K':
            return 3;
        case 'Q':
            return 2;
        case 'J':
            return 1;
        default:
            return 0;
    }
}

bool is_terminal(const std::string &h) {
    return h == "xx" || h == "bb" || h == "bx" || h.size() == 3;
}

int terminal_oop_utility(const std::string &c, const std::string &h) {
    if (!is_terminal(h))
        throw std::invalid_argument(c + " is not a valid terminal");

    const int winner_showdown = rank(c[OOP]) > rank(c[IP]) ? 1 : -1;

    if (h == "xx")
        return winner_showdown;
    if (h == "bb" || h == "xbb")
        return winner_showdown * 2;
    if (h == "xbx")
        return -1;
    if (h == "bx")
        return 1;

    throw std::invalid_argument(h + " is not a valid terminal");
}

double Node::actionProbability(const int a) const {
    if (a < 0 || a >= 2)
        throw std::invalid_argument("Invalid action probability");

    const double r0 = std::max(0.0, cumulativeRegret[0]);
    const double r1 = std::max(0.0, cumulativeRegret[1]);
    if (const double denom = r0 + r1; denom > 0) return std::max(0.0, cumulativeRegret[a]) / denom;
    return 0.5;
}
double Node::averageStrategy(const int a) const {
    if (a < 0 || a >= 2)
        throw std::invalid_argument("Invalid action probability");

    const double r0 = std::max(0.0, cumulativeStrategy[0]);
    const double r1 = std::max(0.0, cumulativeStrategy[1]);
    if (const double denom = r0 + r1; denom > 0) return std::max(0.0, cumulativeStrategy[a]) / denom;
    return 0.5;
}

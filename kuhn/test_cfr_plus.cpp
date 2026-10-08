//
// Created by aderugy on 08/10/2026.
//

// test_cfr_plus.cpp -- black-box tests for cfr_plus.
//
// build:  g++ -std=c++20 -Ikuhn kuhn/test_cfr_plus.cpp kuhn/cfr_plus.cpp kuhn/cfr.cpp
//              kuhn/exploitability.cpp kuhn/node.cpp kuhn/player.cpp kuhn/tree.cpp -o test_cfr_plus
// run:    ./test_cfr_plus
//
// Only the public API is used (cfr_plus, cfr, the Tree/Node, exploitability). The checks are
// properties CFR+ must have whatever the implementation looks like:
//   - regret-matching+ keeps every cumulative regret >= 0;
//   - on a fresh tree, one call sees the same uniform strategy as vanilla CFR, so it must
//     return the same value and leave regrets equal to max(CFR regret, 0);
//   - the average strategy converges to a Kuhn Nash equilibrium (exploitability -> 0,
//     game value -> -1/18, IP's unique equilibrium strategy, OOP inside the alpha family);
//   - it gets there faster than vanilla CFR.

#include "cfr_plus.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "cfr.h"
#include "consts.h"
#include "exploitability.h"
#include "node.h"
#include "player.h"
#include "tree.h"

namespace {

int g_pass = 0, g_fail = 0;

void check(const bool ok, const std::string &what) {
    if (ok) { ++g_pass; return; }
    ++g_fail;
    std::cout << "  FAIL  " << what << "\n";
}

void check_near(const double got, const double want, const double eps, const std::string &what) {
    check(std::abs(got - want) < eps,
          what + "  (got " + std::to_string(got) + ", want " + std::to_string(want) + ")");
}

constexpr double kEps = 1e-9;
constexpr double kGameValue = -1.0 / 18.0;  // Kuhn value for OOP at equilibrium

const std::vector<std::string> kDeals = {"KQ", "KJ", "QJ", "QK", "JK", "JQ"};

using Solver = double (*)(Tree &, const std::string &, const std::string &, double, double);

Tree solve(const Solver solver, const int iterations) {
    Tree tree = init_tree();
    for (int i = 0; i < iterations; ++i)
        for (const auto &c : kDeals)
            solver(tree, c, "", 1.0 / 6.0, 1.0 / 6.0);
    return tree;
}

// Expected OOP utility when both players follow their average strategies.
double avg_value(const Tree &tree, const std::string &deal, const std::string &h) {
    if (is_terminal(h))
        return terminal_oop_utility(deal, h);
    const Node &n = tree.at(deal[p(h)] + h);
    double v = 0;
    for (int a = 0; a < 2; ++a)
        v += average_strategy(n, a) * avg_value(tree, deal, h + ACTIONS[a]);
    return v;
}

double avg_value(const Tree &tree) {
    double v = 0;
    for (const auto &deal : kDeals)
        v += avg_value(tree, deal, "") / 6.0;
    return v;
}

double bet(const Tree &tree, const std::string &infoset) {
    return average_strategy(tree.at(infoset), BET);
}

// ---------------------------------------------------------------------------

// 1. Regret-matching+: cumulative regrets are clamped at 0, at every point of the run.
void test_regrets_non_negative() {
    std::cout << "cumulative regrets stay >= 0\n";
    Tree tree = init_tree();
    bool ok = true;
    for (int i = 0; i < 200 && ok; ++i) {
        for (const auto &c : kDeals) {
            cfr_plus(tree, c, "", 1.0 / 6.0, 1.0 / 6.0);
            for (const auto &[key, node] : tree)
                for (int a = 0; a < 2; ++a)
                    if (node.cumulativeRegret[a] < 0) {
                        ok = false;
                        check(false, "regret " + key + "[" + std::to_string(a) + "] = "
                                     + std::to_string(node.cumulativeRegret[a]) + " after iteration "
                                     + std::to_string(i) + ", deal " + c);
                    }
        }
    }
    check(ok, "all regrets >= 0 over 200 iterations");
}

// 2. The current strategy is always a valid distribution.
void test_strategy_is_distribution() {
    std::cout << "current and average strategies are distributions\n";
    const Tree tree = solve(cfr_plus, 50);
    for (const auto &[key, node] : tree) {
        const double x = node.actionProbability(CHECK), b = node.actionProbability(BET);
        check(x >= 0 && b >= 0, key + " current probs >= 0");
        check_near(x + b, 1.0, kEps, key + " current probs sum to 1");
        const double ax = average_strategy(node, CHECK), ab = average_strategy(node, BET);
        check(ax >= 0 && ab >= 0, key + " average probs >= 0");
        check_near(ax + ab, 1.0, kEps, key + " average probs sum to 1");
    }
}

// 3. First call on a fresh tree: both algorithms play uniform, and each infoset is visited at
//    most once within one deal, so CFR+ must agree with CFR up to the regret clamp.
void test_first_call_matches_cfr() {
    std::cout << "first call on a fresh tree agrees with vanilla CFR\n";
    for (const auto &c : kDeals) {
        Tree vanilla = init_tree(), plus = init_tree();
        const double v = cfr(vanilla, c, "", 1.0 / 6.0, 1.0 / 6.0);
        const double vp = cfr_plus(plus, c, "", 1.0 / 6.0, 1.0 / 6.0);
        check_near(vp, v, kEps, c + " return value == cfr");

        bool regrets_ok = true;
        for (const auto &[key, node] : vanilla)
            for (int a = 0; a < 2; ++a)
                if (std::abs(plus.at(key).cumulativeRegret[a] - std::max(node.cumulativeRegret[a], 0.0)) > kEps)
                    regrets_ok = false;
        check(regrets_ok, c + " regrets == max(cfr regrets, 0)");
    }
}

// 4. Unreached subtrees are untouched: with OOP's reach at 0 nothing OOP does matters,
//    but the call must still not blow up or produce NaN.
void test_zero_reach_is_finite() {
    std::cout << "zero reach probabilities stay finite\n";
    Tree tree = init_tree();
    for (int i = 0; i < 10; ++i)
        for (const auto &c : kDeals) {
            check(std::isfinite(cfr_plus(tree, c, "", 0.0, 1.0 / 6.0)), c + " reachOop=0 finite");
            check(std::isfinite(cfr_plus(tree, c, "", 1.0 / 6.0, 0.0)), c + " reachIp=0 finite");
        }
    for (const auto &[key, node] : tree)
        for (int a = 0; a < 2; ++a)
            check(std::isfinite(node.cumulativeRegret[a]) && std::isfinite(node.cumulativeStrategy[a]),
                  key + " node values finite");
}

// 5. Convergence: exploitability shrinks and gets close to 0.
void test_convergence(const Tree &t10, const Tree &t100, const Tree &t1000) {
    std::cout << "exploitability shrinks as CFR+ runs\n";
    const double e10 = exploitability(t10), e100 = exploitability(t100), e1000 = exploitability(t1000);
    check(e10 > e100, "e(10) > e(100)  (" + std::to_string(e10) + " vs " + std::to_string(e100) + ")");
    check(e100 > e1000, "e(100) > e(1000)  (" + std::to_string(e100) + " vs " + std::to_string(e1000) + ")");
    check(e1000 > -kEps, "e(1000) >= 0");
}

// 6. CFR+ should beat vanilla CFR at the same iteration budget.
void test_faster_than_cfr(const Tree &plus1000) {
    std::cout << "CFR+ converges faster than vanilla CFR\n";
    const double ep = exploitability(plus1000);
    const double ev = exploitability(solve(cfr, 1000));
    check(ep < ev, "e_cfr+(1000) < e_cfr(1000)  (" + std::to_string(ep) + " vs " + std::to_string(ev) + ")");
}

// 7. The average profile plays the game at its value, -1/18 for OOP.
void test_game_value(const Tree &tree) {
    std::cout << "average profile achieves the game value\n";
    check_near(avg_value(tree), kGameValue, 5e-3, "OOP value ~= -1/18");
}

// 8. Equilibrium strategies. IP's equilibrium is unique; OOP's is the one-parameter family
//    J bets alpha in [0, 1/3], K bets 3*alpha, Q calls with alpha + 1/3.
void test_equilibrium(const Tree &tree) {
    std::cout << "average strategy is a Kuhn equilibrium\n";
    constexpr double tol = 0.05;

    // IP facing a check.
    check_near(bet(tree, "Kx"), 1.0, tol, "IP K bets after check");
    check_near(bet(tree, "Qx"), 0.0, tol, "IP Q checks behind");
    check_near(bet(tree, "Jx"), 1.0 / 3.0, tol, "IP J bluffs 1/3");
    // IP facing a bet (BET == call).
    check_near(bet(tree, "Kb"), 1.0, tol, "IP K calls");
    check_near(bet(tree, "Qb"), 1.0 / 3.0, tol, "IP Q calls 1/3");
    check_near(bet(tree, "Jb"), 0.0, tol, "IP J folds");

    // OOP.
    const double alpha = bet(tree, "J");
    check(alpha > -tol && alpha < 1.0 / 3.0 + tol, "OOP J bets alpha in [0, 1/3]  (alpha = " + std::to_string(alpha) + ")");
    check_near(bet(tree, "Q"), 0.0, tol, "OOP Q checks");
    check_near(bet(tree, "K"), 3 * alpha, tol, "OOP K bets 3*alpha");
    check_near(bet(tree, "Jxb"), 0.0, tol, "OOP J folds to a bet");
    check_near(bet(tree, "Qxb"), alpha + 1.0 / 3.0, tol, "OOP Q calls alpha + 1/3");
    check_near(bet(tree, "Kxb"), 1.0, tol, "OOP K calls");
}

}  // namespace

int main() {
    test_regrets_non_negative();
    test_strategy_is_distribution();
    test_first_call_matches_cfr();
    test_zero_reach_is_finite();

    const Tree t10 = solve(cfr_plus, 10), t100 = solve(cfr_plus, 100), t1000 = solve(cfr_plus, 1000);
    test_convergence(t10, t100, t1000);
    test_faster_than_cfr(t1000);
    test_game_value(t1000);
    test_equilibrium(t1000);

    std::cout << "\n" << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}

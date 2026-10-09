//
// Created by aderugy on 06/10/2026.
//

// test_best_response.cpp -- tests for best_response_value / exploitability (pass 2).
//
// part of kuhn_tests (see test_main.cpp); entry point: run_best_response_tests()
//
// The main check is an independent ORACLE: in Kuhn the best responder has only 6 infosets
// (3 cards x 2 decision histories), so there are just 2^6 = 64 pure strategies. We
// enumerate all of them, play each against the frozen average strategy, and keep the max.
// That is the best-response value by definition, with no reach/argmax bookkeeping to get
// wrong. best_response_value must match it exactly, on any tree.
//
// The oracle reads sigma_bar straight from cumulativeStrategy (NOT actionProbability,
// which is the current regret-matching strategy), so it also catches using the wrong one.
//
// Convention assumed for exploitability: br(OOP) + br(IP)  (the plain sum, a.k.a. NashConv).

#include "exploitability.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "cfr.h"
#include "consts.h"
#include "node.h"
#include "player.h"
#include "tree.h"
#include "test.h"

namespace {

using test::check;
using test::check_near;

constexpr double kEps = 1e-9;
constexpr double kGameValue = -1.0 / 18.0;  // Kuhn value for OOP at equilibrium

const std::vector<std::string> kDeals = {"KQ", "KJ", "QJ", "QK", "JK", "JQ"};

Tree cfr_tree(const int iterations) {
    Tree tree = init_tree();
    for (int i = 0; i < iterations; ++i)
        for (const auto &c : kDeals)
            cfr(tree, c, "", 1.0 / 6.0, 1.0 / 6.0);
    return tree;
}

// Pass 1 then pass 2, the way a caller of the API is expected to chain them.
double br(const Tree &tree, const int hero) {
    ReachMap reach;
    best_response_reach(tree, hero, reach);
    return best_response_value(tree, hero, reach);
}

// Every infoset's average strategy is `avg_action` with probability 1. The regrets are
// set to point at `regret_action`, so the current strategy can differ from the average.
Tree pure_tree(const int avg_action, const int regret_action) {
    Tree tree = init_tree();
    for (auto &[key, node] : tree) {
        node.cumulativeStrategy[avg_action] = 1.0;
        node.cumulativeStrategy[1 - avg_action] = 0.0;
        node.cumulativeRegret[regret_action] = 1.0;
        node.cumulativeRegret[1 - regret_action] = 0.0;
    }
    return tree;
}

// ---------------------------------------------------------------------------
// Oracle

double oracle_avg(const Node &n, const int a) {
    const double sum = n.cumulativeStrategy[0] + n.cumulativeStrategy[1];
    return sum > 0 ? n.cumulativeStrategy[a] / sum : 0.5;
}

// Hero's decision histories: OOP acts at "" and "xb", IP at "x" and "b".
int hero_infoset_index(const char card, const std::string &h) {
    const int c = static_cast<int>(CARDS.find(card));
    const int slot = (h == "" || h == "x") ? 0 : 1;
    return c * 2 + slot;
}

// Value of `deal`+`h` for `hero` playing the pure strategy `pure` (bit i = action at infoset i).
double oracle_walk(const Tree &tree, const int hero, const int pure,
                   const std::string &deal, const std::string &h) {
    if (is_terminal(h))
        return (hero == OOP ? 1 : -1) * terminal_oop_utility(deal, h);

    const int player = p(h);
    if (player == hero) {
        const int a = (pure >> hero_infoset_index(deal[hero], h)) & 1;
        return oracle_walk(tree, hero, pure, deal, h + ACTIONS[a]);
    }

    const Node &n = tree.at(deal[player] + h);
    double v = 0;
    for (int a = 0; a < 2; ++a)
        v += oracle_avg(n, a) * oracle_walk(tree, hero, pure, deal, h + ACTIONS[a]);
    return v;
}

double oracle_br(const Tree &tree, const int hero) {
    double best = -1e18;
    for (int pure = 0; pure < 64; ++pure) {
        double v = 0;
        for (const auto &deal : kDeals)
            v += oracle_walk(tree, hero, pure, deal, "") / 6.0;
        best = std::max(best, v);
    }
    return best;
}

// ---------------------------------------------------------------------------

// 1. Hand-computed: opponent always bets / always calls.
//    OOP BR: K bets (+2), Q checks then calls (0), J checks then folds (-1)  -> 1/3.
//    IP  BR (OOP always bets): K calls (+2), Q folds or calls (0), J folds (-1) -> 1/3.
void test_always_bet_by_hand() {
    std::cout << "hand-computed: opponent always bets/calls\n";
    const Tree tree = pure_tree(BET, BET);
    check_near(br(tree, OOP), 1.0 / 3.0, kEps, "br(OOP) == 1/3");
    check_near(br(tree, IP), 1.0 / 3.0, kEps, "br(IP) == 1/3");
    check_near(exploitability(tree), 2.0 / 3.0, kEps, "exploitability == 2/3");
}

// 2. Same average strategy, but the current (regret-matching) strategy says "always check".
//    The BR is computed against sigma_bar, so the answer must not change.
void test_uses_average_not_current() {
    std::cout << "plays against the AVERAGE strategy, not the current one\n";
    const Tree tree = pure_tree(BET, CHECK);
    check_near(br(tree, OOP), 1.0 / 3.0, kEps, "br(OOP) == 1/3");
    check_near(br(tree, IP), 1.0 / 3.0, kEps, "br(IP) == 1/3");
}

// 3. Exact agreement with the brute-force oracle. A per-state max (BR that peeks at the
//    opponent's card) or a max at opponent nodes would both overshoot here.
void test_matches_oracle(const Tree &tree, const std::string &name) {
    std::cout << "matches brute-force oracle: " << name << "\n";
    for (const int hero : {OOP, IP})
        check_near(br(tree, hero), oracle_br(tree, hero), kEps,
                   name + " br(P" + std::to_string(hero) + ") == oracle");
}

// 4. A best response does at least as well as the game value, so exploitability >= 0,
//    and exploitability is the average of the two BR values.
void test_bounds(const Tree &tree, const std::string &name) {
    std::cout << "bounds: " << name << "\n";
    const double br0 = br(tree, OOP);
    const double br1 = br(tree, IP);
    check(br0 > kGameValue - kEps, name + " br(OOP) >= -1/18");
    check(br1 > -kGameValue - kEps, name + " br(IP) >= +1/18");
    const double e = exploitability(tree);
    check(e > -kEps, name + " exploitability >= 0");
    check_near(e, br0 + br1, kEps, name + " exploitability == br0 + br1");
}

// 5. CFR converges: exploitability shrinks with iterations and gets close to 0.
void test_convergence(const Tree &t10, const Tree &t100, const Tree &t2000) {
    std::cout << "exploitability shrinks as CFR runs\n";
    const double e10 = exploitability(t10), e100 = exploitability(t100), e2000 = exploitability(t2000);
    check(e10 > e100, "e(10) > e(100)");
    check(e100 > e2000, "e(100) > e(2000)");
    check(e2000 < 2e-2, "e(2000) < 0.02  (got " + std::to_string(e2000) + ")");
}

}  // namespace

void run_best_response_tests() {
    const Tree uniform = init_tree();
    const Tree t10 = cfr_tree(10), t100 = cfr_tree(100), t2000 = cfr_tree(2000);

    test_always_bet_by_hand();
    test_uses_average_not_current();

    for (const auto &[tree, name] : {std::pair{&uniform, "uniform"}, {&t10, "cfr-10"},
                                     {&t100, "cfr-100"}, {&t2000, "cfr-2000"}}) {
        test_matches_oracle(*tree, name);
        test_bounds(*tree, name);
    }

    test_convergence(t10, t100, t2000);
}

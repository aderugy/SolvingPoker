//
// Created by aderugy on 15/09/2026.
//

// test_node.cpp -- tests for is_terminal / terminal_utility
//
// part of kuhn_tests (see test_main.cpp); entry point: run_node_tests()
//
// Conventions assumed:
//   c = 2-char deal, c[0] = player 0's card, c[1] = player 1's card. Ranks 'J' < 'Q' < 'K'.
//   h = public action history, 'x' = check/fold-by-checking, 'b' = bet/call.
//   terminal_utility returns the payoff TO PLAYER 0. Player 1's payoff is the negation.

#include "node.h"

#include <climits>
#include <iostream>
#include <string>
#include <vector>

#include "test.h"

namespace {

constexpr int TODO = INT_MIN;  // placeholder for values you haven't worked out yet

using test::check;

void check_eq(const int got, const int want, const std::string &what) {
    if (want == TODO) {
        ++test::g_skip;
        std::cout << "  TODO  " << what << "  (returned " << got << ")\n";
        return;
    }
    if (got == want) { ++test::g_pass; return; }
    ++test::g_fail;
    std::cout << "  FAIL  " << what << "  expected " << want << ", got " << got << "\n";
}

const std::vector<std::string> kDeals = {"JQ", "JK", "QJ", "QK", "KJ", "KQ"};
const std::vector<std::string> kTerminal = {"xx", "xbx", "xbb", "bx", "bb"};
const std::vector<std::string> kNonTerminal = {"", "x", "b", "xb"};

// Histories that end in a showdown (both players still in the pot).
const std::vector<std::string> kShowdown = {"xx", "xbb", "bb"};
// Histories that end in a fold.
const std::vector<std::string> kFold = {"xbx", "bx"};

std::string swap_deal(const std::string &c) { return std::string{c[1], c[0]}; }

// ---------------------------------------------------------------------------

void test_is_terminal() {
    std::cout << "is_terminal\n";
    for (const auto &h : kTerminal)
        check(is_terminal(h), "\"" + h + "\" should be terminal");
    for (const auto &h : kNonTerminal)
        check(!is_terminal(h), "\"" + (h.empty() ? std::string("<empty>") : h) + "\" should NOT be terminal");
}

// ---------------------------------------------------------------------------
// Ground truth. Fill in every TODO with the payoff to PLAYER 0.
// Ante is 1 from each player; a bet is 1 more.

struct Case { const char *cards; const char *history; int expect; };

const std::vector<Case> kCases = {
    // -- both check --------------------------------------------------------
    {"JQ", "xx", -1}, {"JK", "xx", -1}, {"QJ", "xx", 1},
    {"QK", "xx", -1}, {"KJ", "xx", 1}, {"KQ", "xx", 1},

    // -- check, bet, fold --------------------------------------------------
    {"JQ", "xbx", -1}, {"JK", "xbx", -1}, {"QJ", "xbx", -1},
    {"QK", "xbx", -1}, {"KJ", "xbx", -1}, {"KQ", "xbx", -1},

    // -- check, bet, call --------------------------------------------------
    {"JQ", "xbb", -2}, {"JK", "xbb", -2}, {"QJ", "xbb", 2},
    {"QK", "xbb", -2}, {"KJ", "xbb", 2}, {"KQ", "xbb", 2},

    // -- bet, fold ---------------------------------------------------------
    {"JQ", "bx", 1}, {"JK", "bx", 1}, {"QJ", "bx", 1},
    {"QK", "bx", 1}, {"KJ", "bx", 1}, {"KQ", "bx", 1},

    // -- bet, call ---------------------------------------------------------
    {"JQ", "bb", -2}, {"JK", "bb", -2}, {"QJ", "bb", 2},
    {"QK", "bb", -2}, {"KJ", "bb", 2}, {"KQ", "bb", 2},
};

void test_terminal_utility_table() {
    std::cout << "terminal_utility (table)\n";
    for (const auto &tc : kCases)
        check_eq(terminal_oop_utility(tc.cards, tc.history),
                 tc.expect,
                 std::string(tc.cards) + " / " + tc.history);
}

// ---------------------------------------------------------------------------
// Invariants -- these hold whatever the exact numbers are, and catch the
// classic bugs (sign flips, comparing chars in ASCII order, forgetting the
// ante) without needing the table filled in.

void test_invariants() {
    std::cout << "terminal_utility (invariants)\n";

    // 1. No terminal state is worth zero -- there are no ties in Kuhn and
    //    every line puts money at risk.
    for (const auto &c : kDeals)
        for (const auto &h : kTerminal)
            check(terminal_oop_utility(c, h) != 0, "nonzero: " + c + " / " + h);

    // 2. Showdowns are antisymmetric: swapping the two cards swaps the winner,
    //    so the payoff to player 0 must negate.
    for (const auto &c : kDeals)
        for (const auto &h : kShowdown)
            check(terminal_oop_utility(c, h) == -terminal_oop_utility(swap_deal(c), h),
                  "showdown antisymmetry: " + c + " / " + h);

    // 3. Folds don't reach a showdown, so the payoff cannot depend on the deal.
    for (const auto &h : kFold) {
        const int ref = terminal_oop_utility(kDeals[0], h);
        for (const auto &c : kDeals)
            check(terminal_oop_utility(c, h) == ref,
                  "fold is card-independent: " + c + " / " + h);
    }

    // 4. The two fold lines are folded by *different* players, so they must
    //    have opposite signs.
    check(terminal_oop_utility("KJ", "bx") * terminal_oop_utility("KJ", "xbx") < 0,
          "bx and xbx favour opposite players");

    // 5. A called bet moves more money than a check-down, so the showdown
    //    winner must gain strictly more in "bb" than in "xx".
    for (const auto &c : kDeals)
        check(std::abs(terminal_oop_utility(c, "bb")) > std::abs(terminal_oop_utility(c, "xx")),
              "bb pays more than xx: " + c);

    // 6. "xbb" and "bb" both end in a called bet -- same money in the pot.
    for (const auto &c : kDeals)
        check(terminal_oop_utility(c, "xbb") == terminal_oop_utility(c, "bb"),
              "xbb == bb: " + c);
}

}  // namespace

void run_node_tests() {
    test_is_terminal();
    test_terminal_utility_table();
    test_invariants();
}

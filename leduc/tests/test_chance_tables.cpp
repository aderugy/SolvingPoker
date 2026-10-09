//
// Created by aderugy on 09/10/2026.
//

// test_chance_tables.cpp -- tests for the Leduc chance probabilities (chance.h).
//
// build:  cmake --build cmake-build-debug --target leduc_tests
// run:    ./cmake-build-debug/bin/leduc_tests
//
// Notation: W1[i][j]    = p_deal_preflop(i, j)       P(OOP rank i, IP rank j)
//           W2[b][i][j] = p_deal_postflop(b, i, j)   P(OOP rank i, IP rank j, board rank b)
// Ranks: 0 = J, 1 = Q, 2 = K. Deck: 6 cards, 2 per rank.
//
// The most important check is the brute-force one: it enumerates physical cards and never
// uses the closed-form formulas, so it catches a formula that is wrong in the same way
// everywhere (which normalization / symmetry alone would not).

#include <cmath>
#include <cstdio>
#include <string>

#include "chance.h"

namespace {

constexpr int kRanks = 3;
constexpr int kDeck = 6;
constexpr double kTol = 1e-12;

int g_failures = 0;  // failures in the test currently running

#define CHECK_NEAR(actual, expected, tol, where)                                         \
    do {                                                                                 \
        const double a_ = (actual), e_ = (expected);                                     \
        if (!(std::abs(a_ - e_) <= (tol))) {                                             \
            ++g_failures;                                                                \
            std::printf("  FAIL  %s  [%s]  actual %.17g, expected %.17g\n",              \
                        #actual, std::string(where).c_str(), a_, e_);                    \
        }                                                                                \
    } while (0)

const char *kRankName = "JQK";

std::string ij(const int i, const int j) {
    return std::string("i=") + kRankName[i] + " j=" + kRankName[j];
}

std::string bij(const int b, const int i, const int j) {
    return std::string("b=") + kRankName[b] + " " + ij(i, j);
}

double W1(const int i, const int j) { return p_deal_preflop(i, j); }
double W2(const int b, const int i, const int j) { return p_deal_postflop(b, i, j); }

int rank_of(const int card) { return card / 2; }

// Only used by the conditional-board test, which the spec defines in terms of it.
int cnt(const int b, const int i, const int j) { return 2 - (i == b) - (j == b); }

// ---------------------------------------------------------------------------

// 1. Enumerate every ordered pair / triple of distinct physical cards and count by rank.
void brute_force_matches_enumeration() {
    double pairs[kRanks][kRanks] = {};
    double triples[kRanks][kRanks][kRanks] = {};
    int n_pairs = 0, n_triples = 0;

    for (int c0 = 0; c0 < kDeck; ++c0)
        for (int c1 = 0; c1 < kDeck; ++c1) {
            if (c1 == c0) continue;
            ++n_pairs;
            pairs[rank_of(c0)][rank_of(c1)] += 1;
            for (int cb = 0; cb < kDeck; ++cb) {
                if (cb == c0 || cb == c1) continue;
                ++n_triples;
                triples[rank_of(cb)][rank_of(c0)][rank_of(c1)] += 1;
            }
        }

    CHECK_NEAR(n_pairs, 30, 0, "pair count");
    CHECK_NEAR(n_triples, 120, 0, "triple count");

    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j) {
            CHECK_NEAR(W1(i, j), pairs[i][j] / n_pairs, kTol, ij(i, j));
            for (int b = 0; b < kRanks; ++b)
                CHECK_NEAR(W2(b, i, j), triples[b][i][j] / n_triples, kTol, bij(b, i, j));
        }
}

// 2. Hand-computed values.
void exact_values() {
    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j)
            CHECK_NEAR(W1(i, j), (i == j ? 2.0 : 4.0) / 30.0, kTol, ij(i, j));

    // OOP = J, IP = K: one J left, both Qs left, one K left.
    CHECK_NEAR(W2(0, 0, 2), 4.0 / 120.0, kTol, bij(0, 0, 2));
    CHECK_NEAR(W2(1, 0, 2), 8.0 / 120.0, kTol, bij(1, 0, 2));
    CHECK_NEAR(W2(2, 0, 2), 4.0 / 120.0, kTol, bij(2, 0, 2));
}

// 3. Both tables are probability distributions.
void tables_are_normalized() {
    double s1 = 0, s2 = 0;
    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j) {
            s1 += W1(i, j);
            for (int b = 0; b < kRanks; ++b)
                s2 += W2(b, i, j);
        }
    CHECK_NEAR(s1, 1.0, kTol, "sum W1");
    CHECK_NEAR(s2, 1.0, kTol, "sum W2");
}

// 4. Summing the board out of W2 gives W1.
void board_marginalizes_to_preflop() {
    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j) {
            double s = 0;
            for (int b = 0; b < kRanks; ++b) s += W2(b, i, j);
            CHECK_NEAR(s, W1(i, j), kTol, ij(i, j));
        }
}

// 5. P(board | hands) = cards of that rank left / 4, and sums to 1.
void conditional_board_distribution() {
    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j) {
            const double w1 = W1(i, j);
            if (!(w1 > 0)) {
                ++g_failures;
                std::printf("  FAIL  W1 must be > 0 to condition on it  [%s]  actual %.17g\n",
                            ij(i, j).c_str(), w1);
                continue;
            }
            double s = 0;
            for (int b = 0; b < kRanks; ++b) {
                const double cond = W2(b, i, j) / w1;
                s += cond;
                CHECK_NEAR(cond, cnt(b, i, j) / 4.0, kTol, bij(b, i, j));
            }
            CHECK_NEAR(s, 1.0, kTol, "sum over b, " + ij(i, j));
        }
}

// 6. Only two cards per rank: three of a kind across hands + board is impossible.
void impossible_deals_are_zero() {
    for (int b = 0; b < kRanks; ++b)
        CHECK_NEAR(W2(b, b, b), 0.0, kTol, bij(b, b, b));
}

// 7. Swapping the two players' ranks doesn't change the probability.
void tables_are_symmetric() {
    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j) {
            CHECK_NEAR(W1(i, j), W1(j, i), kTol, ij(i, j));
            for (int b = 0; b < kRanks; ++b)
                CHECK_NEAR(W2(b, i, j), W2(b, j, i), kTol, bij(b, i, j));
        }
}

// 8. Every entry is a probability.
void entries_in_unit_interval() {
    for (int i = 0; i < kRanks; ++i)
        for (int j = 0; j < kRanks; ++j) {
            const double w1 = W1(i, j);
            CHECK_NEAR(w1, std::fmin(std::fmax(w1, 0.0), 1.0), 0, "W1 in [0,1], " + ij(i, j));
            for (int b = 0; b < kRanks; ++b) {
                const double w2 = W2(b, i, j);
                CHECK_NEAR(w2, std::fmin(std::fmax(w2, 0.0), 1.0), 0, "W2 in [0,1], " + bij(b, i, j));
            }
        }
}

}  // namespace

int main() {
    const struct { const char *name; void (*run)(); } tests[] = {
        {"brute_force_matches_enumeration", brute_force_matches_enumeration},
        {"exact_values", exact_values},
        {"tables_are_normalized", tables_are_normalized},
        {"board_marginalizes_to_preflop", board_marginalizes_to_preflop},
        {"conditional_board_distribution", conditional_board_distribution},
        {"impossible_deals_are_zero", impossible_deals_are_zero},
        {"tables_are_symmetric", tables_are_symmetric},
        {"entries_in_unit_interval", entries_in_unit_interval},
    };

    int passed = 0, failed = 0;
    for (const auto &t : tests) {
        g_failures = 0;
        std::printf("%s\n", t.name);
        t.run();
        if (g_failures == 0) {
            ++passed;
            std::printf("  ok\n");
        } else {
            ++failed;
            std::printf("  %d check(s) failed\n", g_failures);
        }
    }

    std::printf("\n%d passed, %d failed\n", passed, failed);
    return failed == 0 ? 0 : 1;
}

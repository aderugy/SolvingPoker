//
// Created by aderugy on 21/09/2026.
//
// print_tree -- dump every infoset: average strategy, current strategy, regrets.
// Assumes keys are card + history (e.g. "K", "Qx", "Jxb", "Kb"),
// action 0 = 'x' (check / fold), action 1 = 'b' (bet / call).

#include "tree.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <ostream>
#include <ranges>
#include <string>
#include <vector>

#include "consts.h"
#include "node.h"


void print_tree(Tree tree) {
    auto rank = [](const char c) { return c == 'J' ? 0 : c == 'Q' ? 1 : 2; };

    std::vector<std::string> keys;
    keys.reserve(tree.size());
    for (const auto &key: tree | std::views::keys) keys.push_back(key);

    // Group by history, then by card rank inside each group.
    std::ranges::sort(keys, [&](const std::string &a, const std::string &b) {
        const std::string hb = b.substr(1);
        if (const std::string ha = a.substr(1); ha != hb) return ha.size() != hb.size() ? ha.size() < hb.size() : ha < hb;
        return rank(a[0]) < rank(b[0]);
    });

    std::printf("%-6s %-3s %-11s %-17s %-17s %s\n",
                "infoset", "pl", "actions", "avg strategy", "current", "cum regret");

    std::string last_history = "?";
    for (const auto &key : keys) {
        const Node n = tree.at(key);
        const std::string h = key.substr(1);
        if (h != last_history) { std::printf("\n"); last_history = h; }

        const int player = h.size() % 2;                       // even history -> P0
        const bool facing_bet = !h.empty() && h.back() == 'b';
        const char *labels = facing_bet ? "fold/call" : "check/bet";

        // Average strategy: normalise the strategy sum, uniform if never visited.
        double avg[2];
        const double sum = n.cumulativeStrategy[0] + n.cumulativeStrategy[1];
        for (int a = 0; a < 2; ++a)
            avg[a] = sum > 0 ? n.cumulativeStrategy[a] / sum : 0.5;

        // Current strategy: regret matching.
        double cur[2], pos = std::max(0.0, n.cumulativeRegret[0]) +
                             std::max(0.0, n.cumulativeRegret[1]);
        for (int a = 0; a < 2; ++a)
            cur[a] = pos > 0 ? std::max(0.0, n.cumulativeRegret[a]) / pos : 0.5;

        std::printf("%-7s P%d  %-11s %.3f  %.3f      %.3f  %.3f      %+10.3f %+10.3f\n",
                    key.c_str(), player, labels,
                    avg[0], avg[1], cur[0], cur[1],
                    n.cumulativeRegret[0], n.cumulativeRegret[1]);
    }
}

Tree init_tree() {
    auto tree = std::unordered_map<std::string, Node>();

    auto q = std::vector<std::string>();

    for (const auto c : CARDS) {
        q.emplace_back(1, c);
    }

    while (!q.empty()) {
        auto cur = q.back();
        q.pop_back();

        if (is_terminal(cur.substr(1))) {
            continue;
        }

        tree.emplace(cur, Node());

        auto next = cur + "b";
        q.push_back(next);

        next = cur + "x";
        q.push_back(next);
    }

    return tree;
}
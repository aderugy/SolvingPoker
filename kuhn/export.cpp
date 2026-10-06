//
// Created by aderugy on 06/10/2026.
//

// export.cpp -- runs vanilla CFR on Kuhn and prints the run as JSON on stdout,
// for the web demo (web/scripts/generate-kuhn-cfr.sh).
//
// usage:  kuhn_export <iterations>
//
// Output:
// {
//   "iterations": N,
//   "convergence": [{ "iteration", "exploitability", "brOop", "brIp" }, ...],  // log-spaced
//   "infosets":    [{ "key", "card", "history", "player", "average": [x, b], "current": [x, b] }, ...]
// }

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "cfr.h"
#include "consts.h"
#include "exploitability.h"
#include "node.h"
#include "player.h"
#include "tree.h"

constexpr int MAX_ITERATIONS = 1000000;
constexpr int NUM_CHECKPOINTS = 80;

static double br(const Tree &tree, const int hero) {
    ReachMap reach;
    best_response_reach(tree, hero, reach);
    return best_response_value(tree, hero, reach);
}

// Log-spaced iteration counts in [1, n], always ending on n.
static std::vector<int> checkpoints(const int n) {
    std::vector<int> out;
    for (int k = 0; k < NUM_CHECKPOINTS; k++) {
        const double t = static_cast<double>(k) / (NUM_CHECKPOINTS - 1);
        const int it = std::max(1, static_cast<int>(std::pow(n, t) + 0.5));
        if (out.empty() || it > out.back()) out.push_back(it);
    }
    if (out.back() != n) out.push_back(n);
    return out;
}

int main(const int argc, char **argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: %s <iterations>\n", argv[0]);
        return 2;
    }

    const int n = std::atoi(argv[1]);
    if (n < 1 || n > MAX_ITERATIONS) {
        std::fprintf(stderr, "iterations must be in [1, %d]\n", MAX_ITERATIONS);
        return 2;
    }

    auto tree = init_tree();
    const auto marks = checkpoints(n);
    size_t next_mark = 0;

    std::printf("{\"iterations\":%d,\"convergence\":[", n);
    for (int i = 1; i <= n; i++) {
        for (const auto &c: DEALS) {
            cfr(tree, c, "", 1.0 / 6.0, 1.0 / 6.0);
        }

        if (i == marks[next_mark]) {
            const double br_oop = br(tree, OOP);
            const double br_ip = br(tree, IP);
            std::printf("%s{\"iteration\":%d,\"exploitability\":%.10g,\"brOop\":%.10g,\"brIp\":%.10g}",
                        next_mark ? "," : "", i, br_oop + br_ip, br_oop, br_ip);
            next_mark++;
        }
    }
    std::printf("],\"infosets\":[");

    std::vector<std::string> keys;
    for (const auto &[key, node]: tree) keys.push_back(key);
    std::ranges::sort(keys, [](const std::string &a, const std::string &b) {
        return a.size() != b.size() ? a.size() < b.size() : a < b;
    });

    for (size_t k = 0; k < keys.size(); k++) {
        const auto &key = keys[k];
        const Node &node = tree.at(key);
        const std::string history = key.substr(1);
        std::printf("%s{\"key\":\"%s\",\"card\":\"%c\",\"history\":\"%s\",\"player\":%d,"
                    "\"average\":[%.10g,%.10g],\"current\":[%.10g,%.10g]}",
                    k ? "," : "", key.c_str(), key[0], history.c_str(), p(history),
                    node.averageStrategy(CHECK), node.averageStrategy(BET),
                    node.actionProbability(CHECK), node.actionProbability(BET));
    }
    std::printf("]}\n");
    return 0;
}

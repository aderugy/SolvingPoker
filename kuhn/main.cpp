//
// Created by aderugy on 15/09/2026.
//

#include <iostream>
#include <vector>

#include "cfr.h"
#include "tree.h"

constexpr int NUM_ITERS = 100000;

int main() {
    auto tree = init_tree();

    for (int i = 0; i <= NUM_ITERS; i++) {
        for (const std::string cards[] = {"KQ", "QK", "QJ", "JQ", "KJ", "JK"}; const auto &c: cards) {
            cfr(tree, c, "", 1.0 / 6.0, 1.0 / 6.0);
        }

        if (i % (NUM_ITERS / 10) == 0) {
            std::cout << "ITERATION " << i << std::endl;
            print_tree(tree);
            std::cout << std::endl;
        }
    }
}

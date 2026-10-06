//
// Created by aderugy on 15/09/2026.
//

#ifndef SOLVINGPOKER_NODE_H
#define SOLVINGPOKER_NODE_H
#include <string>

class Node {
public:
    double cumulativeRegret[2];
    double cumulativeStrategy[2];

    Node() = default;

    [[nodiscard]] double actionProbability(int a) const;
    [[nodiscard]] double averageStrategy(int a) const;
};

bool is_terminal(const std::string &h);
int terminal_oop_utility(const std::string &c, const std::string &h);

#endif //SOLVINGPOKER_NODE_H

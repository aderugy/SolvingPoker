//
// Created by aderugy on 21/09/2026.
//

#ifndef SOLVINGPOKER_TREE_H
#define SOLVINGPOKER_TREE_H
#include <unordered_map>

#include "node.h"

typedef std::unordered_map<std::string, Node> Tree;

Tree init_tree();
void print_tree(Tree tree);

#endif //SOLVINGPOKER_TREE_H

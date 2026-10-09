//
// Created by aderugy on 09/10/2026.
//

#ifndef SOLVINGPOKER_NODE_H
#define SOLVINGPOKER_NODE_H

#include <cstdint>
#include "consts.h"

enum class Kind : uint8_t { Decision, Fold, Showdown, Chance };
enum class Action : uint8_t { FOLD = 0, PASSIVE = 1, AGGRO = 2 };

struct Node {
    Kind kind;
    uint8_t player; // actor (Decision) or folder (Fold)
    int8_t board; // -1 in round 1, else board rank
    int16_t child[A];
    int16_t info; // decision index into regret tables, -1 otherwise
    uint8_t contrib[2]; // chips each player has put in
};

#endif //SOLVINGPOKER_NODE_H

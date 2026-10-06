//
// Created by aderugy on 15/09/2026.
//

#include "cfr.h"


#include "consts.h"
#include "node.h"
#include "player.h"
#include "tree.h"

double cfr(Tree &tree, const std::string &c, const std::string &h, const double reachOop, const double reachIp) {
    // The player that has to take a decision
    const int player = p(h);

    // We have reached a terminal node: we return the utility expressed in the current player POV
    if (is_terminal(h)) {
        // Terminal utility returns the utility in P0 POV
        return (player == OOP ? IP : -1) * terminal_oop_utility(c, h);
    }

    double infoset_ev = 0;
    double action_ev[2] = {0, 0};

    // The index of the infoset in the tree
    const std::string infoset = c[player] + h;

    // For each action
    for (int a = 0; a < ACTIONS.length(); a++) {
        const char action = ACTIONS[a];

        // Recursion
        // We compute the counterfactual regret, updating the reach with the strategy at step t
        if (player == OOP) {
            action_ev[a] = -cfr(tree, c, h + action, tree[infoset].actionProbability(a) * reachOop, reachIp);
        }
        else if (player == IP) {
            action_ev[a] = -cfr(tree, c, h + action, reachOop, tree[infoset].actionProbability(a) * reachIp);
        }

        // We update the infoset counterfactual regret weighted by the strategy at step t
        infoset_ev += tree[infoset].actionProbability(a) * action_ev[a];
    }

    // After computing the values of the children: we update the current infoset
    for (int a = 0; a < ACTIONS.length(); a++) {
        // The probability that we reached this infoset regardless of the actions of the active player
        // Basically the product of the probabilities of the actions taken by the opponent and chance nodes
        const double invert_reach = player == OOP ? reachIp : reachOop;

        // The probability that we reached this infoset regardless of the actions of the opponent
        // Basically the product of the probability of the decisions we've taken to be here
        const double reach = player == OOP ? reachOop : reachIp;

        tree[infoset].cumulativeRegret[a] += invert_reach * (action_ev[a] - infoset_ev);
        tree[infoset].cumulativeStrategy[a] += reach * tree[infoset].actionProbability(a);
    }

    return infoset_ev;
}

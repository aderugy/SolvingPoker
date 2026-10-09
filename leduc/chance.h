//
// Created by aderugy on 09/10/2026.
//

#ifndef SOLVINGPOKER_CHANCE_H
#define SOLVINGPOKER_CHANCE_H

/**
 * Probability of a preflop deal
 * @param c_oop Card rank of OOP player
 * @param c_ip Card rank of IP player
 * @return the probability that OOP and IP get dealt these hands preflop
 */
double p_deal_preflop(int c_oop, int c_ip);

/**
 * Probability of a postflop deal
 * @param c_flop Flop card rank
 * @param c_oop Card rank of OOP player
 * @param c_ip Card rank of IP player
 * @return the probability that OOP and IP get dealt these hands preflop and that there is c_flop on the board
 */
double p_deal_postflop(int c_flop, int c_oop, int c_ip);

#endif //SOLVINGPOKER_CHANCE_H

#ifndef DICE_HPP
#define DICE_HPP

#include <cstdlib>  // for rand()

class Dice {
public:
    int base;
    int numDice;
    int sides;

    Dice() : base(0), numDice(0), sides(0) {}

    Dice(int b, int n, int s) : base(b), numDice(n), sides(s) {}

    int roll() const {
        int total = base;
        for (int i = 0; i < numDice; ++i) {
            total += (rand() % sides) + 1;
        }
        return total;
    }
};

#endif
#ifndef NPC_HPP
#define NPC_HPP

#include "point.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include "dice.hpp"
// Ability bitmasks (stored in the attributes field)
#define INTELIGENT 0b10000000
#define TELEPATHIC 0b01000000
#define TUNNELING  0b00100000
#define ERATIC     0b00010000
#define NO_ABILITY 0b0001111

#define hasAbility(attributes, ability) ((attributes & ability) != 0)

class Monster {
public:
    Point *cord;
    unsigned int attributes;
    char texture;
    std::string name;
    std::string description;
    std::string color;
    Dice speed;
    int hitpoints;
    Dice damage;
    int rarity;
    std::vector<std::string> abilityNames;

    // Constructors
    Monster();
    Monster(Point *cord);
    Monster(Point *cord, unsigned int attr, 
    char tex, std::string name, 
    std::string desc, std::vector<std::string> abilityNames, 
    std::string color, Dice speed, int hitpoints, 
    Dice damage, int rarity);

    void print() const;
};

#endif
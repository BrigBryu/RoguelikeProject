#include "npc.hpp"
#include <stdlib.h>

Monster::Monster(Point *cord){
    this->cord = cord;
    this->attributes = 0;
    this->texture = 'M';
    this->name = "Monster";
    this->description = "A monster";
    this->color = "RED";//base num sides
    this->speed = Dice(1, 1, 10);
    this->hitpoints = 10;
    this->damage = Dice(1, 2, 3);
    this->rarity = 1;
    this->abilityNames = std::vector<std::string>();
    this->abilityNames.push_back("monster ability");
    
    if (rand() % 2) this->attributes |= INTELIGENT;
    if (rand() % 2) this->attributes |= TELEPATHIC;
    if (rand() % 2) this->attributes |= TUNNELING;
    if (rand() % 2) this->attributes |= ERATIC;

    this->attributes |= (rand() % 16) & NO_ABILITY;
}

Monster::Monster(Point *cord, unsigned int attr, 
    char tex, std::string name, 
    std::string desc, std::vector<std::string> abilityNames, 
    std::string color, Dice speed, int hitpoints, 
    Dice damage, int rarity){
    this->cord = cord;
    this->attributes = attr;
    this->texture = tex;
    this->name = name;
    this->description = desc;
    this->color = color;
    this->speed = speed;
    this->hitpoints = hitpoints;
    this->damage = damage;
    this->rarity = rarity;
    this->abilityNames = abilityNames;
    if (rand() % 2) this->attributes |= INTELIGENT;
    if (rand() % 2) this->attributes |= TELEPATHIC;
    if (rand() % 2) this->attributes |= TUNNELING;
    if (rand() % 2) this->attributes |= ERATIC;
}

void Monster::print() const{
    std::cout << "Monster: " << this->name << std::endl;
    std::cout << "Description: " << this->description << std::endl;
    std::cout << "Color: " << this->color << std::endl;
}

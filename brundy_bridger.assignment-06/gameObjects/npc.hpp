#ifndef NPC_HPP
#define NPC_HPP
/*
 use unsigned int
 1111 1111
 inteligent telepathic tuneling eratict second byte for movement speed
 */
#define INTELIGENT 0b10000000
#define TELEPATHIC 0b01000000
#define TUNNELING 0b00100000
#define ERATIC 0b00010000
#define NO_ABILITY 0b0001111
//does not have ability == 0
#define hasAbility(atributes, ability) ((atributes & ability) != 0)


#include "point.hpp"

typedef struct{
    Point cord;
    unsigned int atributes;
    char texture;
} NPC;

NPC *makeNPC(Point cord);
unsigned int getSpeed(NPC const *npc);

#endif

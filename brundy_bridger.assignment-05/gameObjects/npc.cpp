#include "npc.hpp"
#include <stdlib.h>

unsigned int getSpeed(NPC const *npc){ //npc is a pointer to a const NPC
    unsigned int speed = npc->atributes & NO_ABILITY;
    return speed + 5; //returns a 5-20 value
}

NPC *makeNPC(Point cord){
    NPC *npc = (NPC*) malloc(sizeof(NPC));
    npc->cord = cord;
    if (rand() % 2) npc->atributes |= INTELIGENT;
    if (rand() % 2) npc->atributes |= TELEPATHIC;
    if (rand() % 2) npc->atributes |= TUNNELING;
    if (rand() % 2) npc->atributes |= ERATIC;

    npc->atributes |= (rand() % 16) & NO_ABILITY;

    char textures[] = { 'p', 'h', 'P', 'D' };
    npc->texture = textures[rand() % (sizeof(textures) / sizeof(textures[0]))];
    return npc;
}



#ifndef SIMULATE_H
#define SIMULATE_H

#include "dungeon.h"
typedef struct move_event {
    int turn;
    int is_pc;
    NPC *npc;
} move_event_t;

int getMoveDelayNPC(NPC *npc);
void processPCMove(Dungeon *dungeon);
void processMonsterMove(Dungeon *dungeon, NPC *npc);
void moveTowards(Point *from, Point target, Dungeon *dungeon, int canTunnel);
int checkCollision(Dungeon *dungeon, NPC *npc);
void simulateMonsters(Dungeon *dungeon, int num_monsters);

void spawnMonsters(Dungeon *dungeon, int num_monsters);

#endif

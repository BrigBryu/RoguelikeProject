#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "dungeon.h"
#include "npc.h"
#include "heap.h"
#include "rectangle.h"
#include "point.h"
#include "simulate.h"

/* ---------------------------------------------------------------------------
   Portable Dijkstra functions:
   They now fill in the provided 2D arrays.
   --------------------------------------------------------------------------- */
void dungeon_dijkstra_non_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);
void dungeon_dijkstra_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);

/* Global Dijkstra maps used by monster AI */
static int dijkstra_non_tunnel[heightScreen][widthScreen];
static int dijkstra_tunnel[heightScreen][widthScreen];

#define PC_SPEED 10
#define PC_MOVE_DELAY (1000 / PC_SPEED)
#define SLEEP_USECS 250000

/* Comparison function for move events in the event queue */
int compare_move_event(const void *a, const void *b) {
    const move_event_t *ea = a;
    const move_event_t *eb = b;
    return (ea->turn - eb->turn);
}

/* Return the delay (in turns) for an NPC’s next move */
int getMoveDelayNPC(NPC *npc) {
    return 1000 / getSpeed(npc);
}

/* Move a character one step toward a target.
   - Non-tunnelers cannot move into rock (hardness > 0).
   - Tunnelers break through rock: they subtract 85 from hardness,
     and if the hardness becomes 0 (or less), the cell is converted to a corridor.
*/
void moveTowards(Point *from, Point target, Dungeon *dungeon, int canTunnel) {
    int dx = 0, dy = 0;
    if (target.x > from->x)
        dx = 1;
    else if (target.x < from->x)
        dx = -1;
    if (target.y > from->y)
        dy = 1;
    else if (target.y < from->y)
        dy = -1;

    int newx = from->x + dx;
    int newy = from->y + dy;

    if (newx < 0 || newx >= widthScreen || newy < 0 || newy >= heightScreen)
        return;
    if (dungeon->tiles[newy][newx].hardness == 255)
        return;
    if (!canTunnel && dungeon->tiles[newy][newx].hardness > 0)
        return;

    if (canTunnel && dungeon->tiles[newy][newx].hardness > 0) {
        int newHardness = dungeon->tiles[newy][newx].hardness - 85;
        if (newHardness <= 0) {
            newHardness = 0;
            dungeon->tiles[newy][newx].type = HALL;
            from->x = newx;
            from->y = newy;
        }
        dungeon->tiles[newy][newx].hardness = newHardness;
        return;
    }
    from->x = newx;
    from->y = newy;
}

/* Process a PC move.
   If the destination cell is rock (hardness > 0 and not immutable),
   the PC bores through it by converting the cell to a corridor.
*/
void processPCMove(Dungeon *dungeon) {
    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    int newx = dungeon->mc.x + dx;
    int newy = dungeon->mc.y + dy;
    if (newx < 0 || newx >= widthScreen || newy < 0 || newy >= heightScreen)
        return;
    if (dungeon->tiles[newy][newx].hardness == 255)
        return;
    if (dungeon->tiles[newy][newx].hardness > 0) {
        /* Bore through the rock: immediately set hardness to 0 and convert to a hall */
        dungeon->tiles[newy][newx].hardness = 0;
        dungeon->tiles[newy][newx].type = HALL;
    }
    dungeon->mc.x = newx;
    dungeon->mc.y = newy;
}

/* Process a monster move.
   - Erratic monsters sometimes move randomly.
   - Non-telepathic monsters only move if the PC is in the same room.
   - Intelligent monsters (or telepathic ones) use the Dijkstra maps.
   - Otherwise, a non-intelligent monster moves directly toward the PC.
   
   Whenever a monster (or erratic monster) moves into a cell with rock,
   if it has tunneling ability it breaks the rock (reducing hardness, and if reduced to 0,
   converting the cell into a corridor).
*/
void processMonsterMove(Dungeon *dungeon, NPC *npc) {
    int is_tele        = (npc->atributes & TELEPATHIC) ? 1 : 0;
    int is_intelligent = (npc->atributes & INTELIGENT) ? 1 : 0;
    int is_erratic     = (npc->atributes & ERATIC) ? 1 : 0;
    int canTunnel      = (npc->atributes & TUNNELING) ? 1 : 0;

    /* Erratic behavior: 50% chance to move randomly */
    if (is_erratic && (rand() % 2 == 0)) {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;
        int newx = npc->cord.x + dx;
        int newy = npc->cord.y + dy;
        if (newx < 0 || newx >= widthScreen || newy < 0 || newy >= heightScreen)
            return;
        if (dungeon->tiles[newy][newx].hardness == 255)
            return;
        if (!canTunnel) {
            /* Non-tunneling erratic monsters only move into open floor */
            if (dungeon->tiles[newy][newx].hardness > 0)
                return;
            npc->cord.x = newx;
            npc->cord.y = newy;
        } else {
            /* Tunneling erratic monsters: if destination is rock, break it */
            if (dungeon->tiles[newy][newx].hardness > 0) {
                int newHardness = dungeon->tiles[newy][newx].hardness - 85;
                if (newHardness <= 0) {
                    newHardness = 0;
                    dungeon->tiles[newy][newx].type = HALL;
                    npc->cord.x = newx;
                    npc->cord.y = newy;
                }
                dungeon->tiles[newy][newx].hardness = newHardness;
            } else {
                npc->cord.x = newx;
                npc->cord.y = newy;
            }
        }
        return;
    }

    /* Determine if the monster knows the PC's location */
    int pc_visible = 0;
    if (is_tele) {
        pc_visible = 1;
    } else {
        for (int i = 0; i < dungeon->numRooms; i++) {
            if (rectangleContainsCord(&(dungeon->rooms[i]), npc->cord.x, npc->cord.y) &&
                rectangleContainsCord(&(dungeon->rooms[i]), dungeon->mc.x, dungeon->mc.y)) {
                pc_visible = 1;
                break;
            }
        }
    }
    if (!pc_visible)
        return;

    /* Intelligent monsters use the Dijkstra maps */
    if (is_intelligent) {
        int best = INT_MAX;
        int best_x = npc->cord.x;
        int best_y = npc->cord.y;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0)
                    continue;
                int nx = npc->cord.x + dx;
                int ny = npc->cord.y + dy;
                if (nx < 0 || nx >= widthScreen || ny < 0 || ny >= heightScreen)
                    continue;
                if (dungeon->tiles[ny][nx].hardness == 255)
                    continue;
                if (!canTunnel && dungeon->tiles[ny][nx].hardness > 0)
                    continue;
                int d = canTunnel ? dijkstra_tunnel[ny][nx] : dijkstra_non_tunnel[ny][nx];
                if (d < best) {
                    best   = d;
                    best_x = nx;
                    best_y = ny;
                }
            }
        }
        /* For tunneling monsters, break rock if necessary */
        if (canTunnel && dungeon->tiles[best_y][best_x].hardness > 0) {
            int newHardness = dungeon->tiles[best_y][best_x].hardness - 85;
            if (newHardness <= 0) {
                newHardness = 0;
                dungeon->tiles[best_y][best_x].type = HALL;
                npc->cord.x = best_x;
                npc->cord.y = best_y;
            }
            dungeon->tiles[best_y][best_x].hardness = newHardness;
        } else {
            npc->cord.x = best_x;
            npc->cord.y = best_y;
        }
        return;
    }

    /* Non-intelligent monsters simply move directly toward the PC */
    moveTowards(&(npc->cord), dungeon->mc, dungeon, canTunnel);
}

/* Check for collision between monsters and the PC (or between monsters).
   Returns 1 if the PC is killed, 0 otherwise.
*/
int checkCollision(Dungeon *dungeon, NPC *npc) {
    if (npc->cord.x == dungeon->mc.x && npc->cord.y == dungeon->mc.y)
        return 1;
    for (int i = 0; i < dungeon->numMonsters; i++) {
        if (dungeon->monsters[i] == npc)
            continue;
        if (dungeon->monsters[i] != NULL &&
            dungeon->monsters[i]->cord.x == npc->cord.x &&
            dungeon->monsters[i]->cord.y == npc->cord.y) {
            dungeon->monsters[i]->texture = 'X';
        }
    }
    return 0;
}

/* Main simulation loop (Discrete Event Simulator) */
void simulateMonsters(Dungeon *dungeon, int num_monsters_default) {
    heap_t event_queue;
    heap_init(&event_queue, compare_move_event, NULL);

    /* Create an initial event for the PC */
    move_event_t *pc_event = malloc(sizeof(*pc_event));
    pc_event->turn  = PC_MOVE_DELAY;
    pc_event->is_pc = 1;
    pc_event->npc   = NULL;
    heap_insert(&event_queue, pc_event);

    renderDungeon(dungeon);

    /* Create initial events for each monster */
    for (int i = 0; i < dungeon->numMonsters; i++) {
        move_event_t *m_event = malloc(sizeof(*m_event));
        m_event->turn  = getMoveDelayNPC(dungeon->monsters[i]);
        m_event->is_pc = 0;
        m_event->npc   = dungeon->monsters[i];
        heap_insert(&event_queue, m_event);
    }

    int game_over = 0;
    int game_turn = 0;

    while (!game_over) {
        move_event_t *event = heap_remove_min(&event_queue);
        game_turn = event->turn;

        if (event->is_pc) {
            /* Process the PC move */
            processPCMove(dungeon);

            /* Update the Dijkstra maps using the portable routines */
            dungeon_dijkstra_non_tunnel(dungeon, dijkstra_non_tunnel);
            dungeon_dijkstra_tunnel(dungeon, dijkstra_tunnel);

            renderDungeon(dungeon);
            usleep(SLEEP_USECS);

            /* Check if the PC stepped onto a monster’s cell */
            for (int i = 0; i < dungeon->numMonsters; i++) {
                if (dungeon->monsters[i] != NULL &&
                    dungeon->mc.x == dungeon->monsters[i]->cord.x &&
                    dungeon->mc.y == dungeon->monsters[i]->cord.y) {
                    printf("PC killed by monster!\n");
                    game_over = 1;
                    break;
                }
            }
            if (!game_over) {
                event->turn += PC_MOVE_DELAY;
                heap_insert(&event_queue, event);
            } else {
                free(event);
            }
        } else {
            /* Process a monster move */
            processMonsterMove(dungeon, event->npc);
            if (checkCollision(dungeon, event->npc)) {
                printf("PC killed by monster!\n");
                game_over = 1;
                free(event);
                break;
            }
            if (event->npc->texture == 'X') {
                free(event);
            } else {
                event->turn += getMoveDelayNPC(event->npc);
                heap_insert(&event_queue, event);
            }
        }

        /* Win condition: all monsters have been eliminated */
        int alive_monsters = 0;
        for (int i = 0; i < dungeon->numMonsters; i++) {
            if (dungeon->monsters[i] != NULL &&
                dungeon->monsters[i]->texture != 'X')
                alive_monsters++;
        }
        if (alive_monsters == 0) {
            printf("PC wins! All monsters defeated.\n");
            game_over = 1;
        }
    }
    heap_delete(&event_queue);
}

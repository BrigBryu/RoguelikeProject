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

void dungeon_dijkstra_non_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);
void dungeon_dijkstra_tunnel(Dungeon *dungeon, int dist[heightScreen][widthScreen]);

static int dijkstra_non_tunnel[heightScreen][widthScreen];
static int dijkstra_tunnel[heightScreen][widthScreen];

#define PC_SPEED 10
#define PC_MOVE_DELAY (1000 / PC_SPEED)
#define SLEEP_USECS 250000

int compare_move_event(const void *a, const void *b) {
    const move_event_t *ea = a;
    const move_event_t *eb = b;
    return (ea->turn - eb->turn);
}

int getMoveDelayNPC(NPC *npc) {
    return 1000 / getSpeed(npc);
}

// Move a character one step
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
        dungeon->tiles[newy][newx].hardness = 0;
        dungeon->tiles[newy][newx].type = HALL;
    }
    dungeon->mc.x = newx;
    dungeon->mc.y = newy;
}

void processMonsterMove(Dungeon *dungeon, NPC *npc) {
    int is_tele        = (npc->atributes & TELEPATHIC) ? 1 : 0;
    int is_intelligent = (npc->atributes & INTELIGENT) ? 1 : 0;
    int is_erratic     = (npc->atributes & ERATIC) ? 1 : 0;
    int canTunnel      = (npc->atributes & TUNNELING) ? 1 : 0;

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
            if (dungeon->tiles[newy][newx].hardness > 0)
                return;
            npc->cord.x = newx;
            npc->cord.y = newy;
        } else {
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

    moveTowards(&(npc->cord), dungeon->mc, dungeon, canTunnel);
}

int checkCollision(Dungeon *dungeon, NPC *npc) {
    return (npc->cord.x == dungeon->mc.x && npc->cord.y == dungeon->mc.y);
}

void simulateMonsters(Dungeon *dungeon, int num_monsters_default) {
    heap_t event_queue;
    heap_init(&event_queue, compare_move_event, NULL);

    move_event_t *pc_event = malloc(sizeof(*pc_event));
    pc_event->turn  = PC_MOVE_DELAY;
    pc_event->is_pc = 1;
    pc_event->npc   = NULL;
    heap_insert(&event_queue, pc_event);

    renderDungeon(dungeon);

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
            processPCMove(dungeon);

            dungeon_dijkstra_non_tunnel(dungeon, dijkstra_non_tunnel);
            dungeon_dijkstra_tunnel(dungeon, dijkstra_tunnel);

            renderDungeon(dungeon);
            usleep(SLEEP_USECS);

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
            processMonsterMove(dungeon, event->npc);
            if (checkCollision(dungeon, event->npc)) {
                printf("PC killed by monster!\n");
                game_over = 1;
                free(event);
                break;
            }
            event->turn += getMoveDelayNPC(event->npc);
            heap_insert(&event_queue, event);
        }

        int alive_monsters = 0;
        for (int i = 0; i < dungeon->numMonsters; i++) {
            if (dungeon->monsters[i] != NULL)
                alive_monsters++;
        }
        if (alive_monsters == 0) {
            printf("PC wins! All monsters defeated.\n");
            game_over = 1;
        }
    }
    heap_delete(&event_queue);
}

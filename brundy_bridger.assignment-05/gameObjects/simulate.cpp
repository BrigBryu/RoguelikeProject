#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "dungeon.hpp"
#include "npc.hpp"
#include "rectangle.hpp"
#include "point.hpp"
#include "simulate.hpp"
static int dijkstra_non_tunnel[heightScreen][widthScreen];
static int dijkstra_tunnel[heightScreen][widthScreen];

void moveTowards(Point *from, Point target, Dungeon *dungeon, int canTunnel) {
    // Check if already at target
    if (from->x == target.x && from->y == target.y) {
        return;
    }
    
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

int processMonsterMove(Dungeon *dungeon, NPC *npc) {
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
            return 0;
        if (dungeon->tiles[newy][newx].hardness == 255)
            return 0;
        if (!canTunnel) {
            if (dungeon->tiles[newy][newx].hardness > 0)
                return 0;
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
        return 0;
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
        return 0;

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

        if (npc->cord.x == dungeon->mc.x && npc->cord.y == dungeon->mc.y) {
            return 1;
        }
        return 0;
    }

    moveTowards(&(npc->cord), dungeon->mc, dungeon, canTunnel);
    
    // Check if monster reached the player after moving
    if (npc->cord.x == dungeon->mc.x && npc->cord.y == dungeon->mc.y) {
        return 1;
    }
    
    return 0;
}

int simulateMonsters(Dungeon *dungeon) {
    dungeon_dijkstra_non_tunnel(dungeon, dijkstra_non_tunnel);
    dungeon_dijkstra_tunnel(dungeon, dijkstra_tunnel);

    int monsterMoved = 0;
    int playerHit = 0;
    
    // Process each monster's move.
    for (int i = 0; i < dungeon->numMonsters; i++) {
        if (dungeon->monsters[i] != NULL) {
            // Store old position to see if monster moved
            int oldX = dungeon->monsters[i]->cord.x;
            int oldY = dungeon->monsters[i]->cord.y;
            
            // If monster hits player, return 1 immediately
            if (processMonsterMove(dungeon, dungeon->monsters[i])) {
                return 1;  // Monster reached player
            }
            
            // Check if monster moved
            if (oldX != dungeon->monsters[i]->cord.x || oldY != dungeon->monsters[i]->cord.y) {
                monsterMoved = 1;
            }
        }
    }
    
    return monsterMoved;  // Return 1 if any monster moved, 0 otherwise
}
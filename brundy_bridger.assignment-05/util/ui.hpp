#ifndef UI_HPP
#define UI_HPP

#include <ncurses.h>
#include "dungeon.hpp"

void initCurses(void);
void endCurses(void);

WINDOW* initGameMapWindow(void);

// Render the dungeon using the map mode stored in dungeon->renderMapMode
// modes: 0 = default map, 1 = non-tunneling distance map, 
//        2 = tunneling distance map, 3 = hardness map
void renderCurses(Dungeon* dungeon, WINDOW* gameWin);

// line 0
void renderMessageLine(const char* message);

//lines 22 and 23
void renderStatus(const char* status1, const char* status2);

#endif

#include <stdio.h>
#include "point.h" //"" double quotes for local header files to include
//should need struct Point but it is typedef struct Point
Point createPoint(int x, int y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

void printDebug(Point p){
    printf("Point at (%d,%d)\n",p.x,p.y);
}

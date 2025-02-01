#include <stdio.h>
#include "point.h" //"" double quotes for local header files to include

struct Point createPoint(int x, int y) {
    struct Point p;
    p.x = x;
    p.y = y;
    return p;
}

void printDebug(struct Point p){
    printf("Point at (%d,%d)\n",p.x,p.y);
}

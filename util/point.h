#ifndef POINT_H
#define POINT_H

typedef struct{
    int x;
    int y;
} Point;

// Interface
Point createPoint(int x, int y);
void printDebug(Point p);
#endif

#ifndef POINT_H
#define POINT_H

typedef struct{
    int x;
    int y;
} Point;

// Interface
Point createPoint(int x, int y);
void printDebug(Point p);
double getDistance(Point *p1, Point *p2);
#endif

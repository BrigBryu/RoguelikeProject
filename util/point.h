#ifndef POINT_H
#define POINT_H

struct Point {
    int x;
    int y;
};

// Interface
struct Point createPoint(int x, int y);
void printDebug(struct Point p);
#endif

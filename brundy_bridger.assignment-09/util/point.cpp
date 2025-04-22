#include <stdio.h>
#include <math.h>
#include "point.hpp" //"" double quotes for local header files to include
//should need struct Point but it is typedef struct Point
Point createPoint(int x, int y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

double getDistance(Point* p1, Point* p2){
    double dx = p1->x - p2->x;
    double dy = p1->y - p2->y;
    double distance = sqrt(dx * dx + dy * dy);
    return distance;
}

void printDebug(Point p){
    printf("Point at (%d,%d)\n",p.x,p.y);
}

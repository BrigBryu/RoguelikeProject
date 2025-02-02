#ifndef RECTANGLE_H
#define RECTANGLE_H
#include "point.h"
typedef struct{
    Point bottomLeft;
    int width;
    int height;
} Rectangle;

Rectangle createRectangle(int x, int y, int width, int height);
Rectangle createRectangleFromPoint(Point* p, int width, int height);

int rectangleContainsCord(Rectangle* rec, int x, int y);
int rectangleContainsPoint(Rectangle* rec, Point* p);

int rectangleIntersectsRectangle(Rectangle* r1, Rectangle* r2);

#endif

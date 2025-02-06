#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "point.h"

typedef struct {
    Point bottomLeft;
    int width;
    int height;
} Rectangle;


Rectangle createRectangle(int x, int y, int width, int height);
Rectangle createRectangleFromPoint(Point* p, int width, int height);

int rectangleContainsCord(Rectangle* rec, int x, int y);
int rectangleContainsPoint(Rectangle* rec, Point* p);

/*
 * Returns 1 if r1 and r2 share interior area
 */
int rectanglesOverlap(Rectangle* r1, Rectangle* r2);

/*
 * Returns 1 if overlap or the gap between is less than one tile
 */
int rectanglesTouch(Rectangle* r1, Rectangle* r2);

void printDebugRectangle(Rectangle r);

Point getRectangleMid(Rectangle* r);
#endif

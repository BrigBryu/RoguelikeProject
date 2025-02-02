#include <stdio.h>
#include "rectangle.h" // "" use double quotes to include a local file

//use the typedef struct {} Rectangle;
Rectangle createRectangle(int x, int y, int width, int height){
    return (Rectangle) {{x,y}, width, height};
}

Rectangle createRectangleFromPoint(Point* p, int width, int height){
    Rectangle rec;
    rec.bottomLeft.x = p->x;
    rec.bottomLeft.y = p->y;
    rec.width = width;
    rec.height = height;
    return rec;
}

int rectangleContainsPoint(Rectangle* rec, Point* p){
    return rectangleContainsCord(rec,p->x,p->y);
}

int rectangleContainsCord(Rectangle* rec, int x, int y) {
    if (x >= rec->bottomLeft.x && x <= rec->bottomLeft.x + rec->width &&
        y >= rec->bottomLeft.y && y <= rec->bottomLeft.y + rec->height) {
        return 1;  // inside the rectangle
    }
    return 0;  // outside the rectangle
}

int rectangleIntersectsRectangle(Rectangle* r1, Rectangle* r2) {
    // left or right
    if (r1->bottomLeft.x + r1->width < r2->bottomLeft.x || 
        r2->bottomLeft.x + r2->width < r1->bottomLeft.x) {
        return 0; // No intersec
    }

    // above or below
    if (r1->bottomLeft.y + r1->height < r2->bottomLeft.y || 
        r2->bottomLeft.y + r2->height < r1->bottomLeft.y) {
        return 0; // No intersect
    }


    return 1; //intersect
}

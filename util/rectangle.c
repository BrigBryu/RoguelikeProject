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
//if r1 and r2 are inside
int rectanglesOverlap(Rectangle* r1, Rectangle* r2) {
    if (r1->bottomLeft.x < r2->bottomLeft.x + r2->width &&
        r1->bottomLeft.x + r1->width > r2->bottomLeft.x &&
        r1->bottomLeft.y < r2->bottomLeft.y + r2->height &&
        r1->bottomLeft.y + r1->height > r2->bottomLeft.y) {
        return 1;
    }
    return 0;
}
//if r1 expanded by 1 tile inclusivily intersects with r2 they touch
int rectanglesTouch(Rectangle* r1, Rectangle* r2) {
    int r1Left   = r1->bottomLeft.x - 1;
    int r1Bottom = r1->bottomLeft.y - 1;
    int r1Right  = r1->bottomLeft.x + r1->width + 1;
    int r1Top    = r1->bottomLeft.y + r1->height + 1;

    int r2Left   = r2->bottomLeft.x;
    int r2Bottom = r2->bottomLeft.y;
    int r2Right  = r2->bottomLeft.x + r2->width;
    int r2Top    = r2->bottomLeft.y + r2->height;


    if (r2Left <= r1Right &&
        r2Right >= r1Left &&
        r2Bottom <= r1Top &&
        r2Top >= r1Bottom) {
        return 1;
    }
    return 0;
}

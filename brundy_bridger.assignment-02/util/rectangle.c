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
    // bottomLeft.x <= x < bottomLeft.x + width
    // bottomLeft.y <= y < bottomLeft.y + height
    if (x >= rec->bottomLeft.x &&
        x <  rec->bottomLeft.x + rec->width &&
        y >= rec->bottomLeft.y &&
        y <  rec->bottomLeft.y + rec->height) {
        return 1;
    }
    return 0;
}

//if r1 and r2 are inside
//
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
    // expanded r1
    Rectangle expanded;
    expanded.bottomLeft.x = r1->bottomLeft.x - 1;
    expanded.bottomLeft.y = r1->bottomLeft.y - 1;
    expanded.width  = r1->width + 2;
    expanded.height = r1->height + 2;
    
    return rectanglesOverlap(&expanded, r2);
}

Point getRectangleMid(Rectangle* r){
    return createPoint(r->bottomLeft.x + r->width / 2,
                       r->bottomLeft.y + r->height / 2);
}


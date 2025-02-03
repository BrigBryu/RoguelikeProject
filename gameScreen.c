#include <stdio.h>
#include "dungeon.h"

int main(){
    Dungeon dungeon;
    printf("Before all rock init:\n");
    renderDungeon(&dungeon);
    setTiles(&dungeon);
    printf("\nAfter all rock init:\n");
    renderDungeon(&dungeon);
    setRooms(&dungeon);
    printf("\nAfter room add\n");
    renderDungeon(&dungeon);
}

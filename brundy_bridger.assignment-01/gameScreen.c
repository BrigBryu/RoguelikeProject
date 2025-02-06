#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "dungeon.h"

int main(){
    
    srand(time(NULL));
    Dungeon dungeon;
    /*
    printf("Before all rock init:\n");
    renderDungeon(&dungeon);
    setTiles(&dungeon);
    printf("\nAfter all rock init:\n");
    renderDungeon(&dungeon);
    setRooms(&dungeon);
    printf("\nAfter room add\n");
    renderDungeon(&dungeon);
    setHalls(&dungeon);
    printf("\nAfter hall add\n");
    renderDungeon(&dungeon);
    populateDungeon(&dungeon);
    printf("\nAfter populate dungeon\n");
    renderDungeon(&dungeon);
    */
    //for(int i = 0; i < 10000; i++){
    //    printf("\n\nCreating the %d map:\n", i);
        dungeon = generateDungeon();
        renderDungeon(&dungeon);
    //}
}

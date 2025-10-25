//Дописал за автором функцию ClearMap
#include <stdio.h>
#include <stdlib.h>

#define mapWidth 80
#define mapHeight 25

char map[mapHeight][mapWidth + 1];

void ClearMap() {
    for (int i = 0; i < mapWidth; i++) 
        map[0][i] = '.';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++) 
        snprintf(map[j], map[0]);
        // В будущем разобраться, почему VS Code предлагает использовать вот это:
        // snprintf(map[j], sizeof(map[j]), "%s", map[0]);
}

int main() {

    return 0;
}
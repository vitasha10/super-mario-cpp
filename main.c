//Дописал за автором функцию ShowMap
#include <stdio.h>
#include <stdlib.h>

#define mapWidth 80
#define mapHeight 25

char map[mapHeight][mapWidth + 1];

void ClearMap() 
{
    for (int i = 0; i < mapWidth; i++) 
        map[0][i] = '.';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++) 
        // snprintf(map[j], map[0]); - не заработало, хотя в видео так показано
        // Исправил на правильный вариант:
        snprintf(map[j], sizeof(map[j]), "%s", map[0]);
        // Объяснение: sizeof(map[j]) возвращает размер строки, что позволяет корректно скопировать содержимое map[0] в map[j].
}

void ShowMap()
{
    map[mapHeight - 1][mapWidth - 1] = '\0';
    for (int j = 0; j < mapHeight; j++) 
        printf("%s", map[j]);
}

int main()
{
    ClearMap();
    ShowMap();
    return 0;
}
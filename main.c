// Теперь уровень можно пройти
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <windows.h>

#define mapWidth 80
#define mapHeight 25

typedef struct SObject {
    float x, y;
    float width, height;
    float vertSpeed;
    BOOL IsFly;
    char cType;
    float horizSpeed;
} TObject;

char map[mapHeight][mapWidth + 1];
TObject mario;

TObject *brick = NULL;
int brickLength;

TObject *moving = NULL;
int movingLength;

int level = 1;


void ClearMap() 
{
    for (int i = 0; i < mapWidth; i++) 
        map[0][i] = ' ';
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

void SetObjectPos(TObject *obj, float xPos, float yPos) 
{
    // Комментарий автора ролика: В данном случае мы меняем значение входной переменной obj, поэтому используем указатель.
    (*obj).x = xPos;
    (*obj).y = yPos;
}

void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType) 
{
    SetObjectPos(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).cType = inType;
    (*obj).horizSpeed = 0.2;
}   

BOOL IsCollision(TObject o1, TObject o2);
void CreateLevel(int lvl);

TObject *GetNewBrick()
{
    brickLength++;
    brick = realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}

void CreateLevel(int lvl)
{
    InitObject(&mario, 39, 10, 3, 3, '@');

    if(lvl == 1)
    {
        brickLength = 0;
        brick = realloc(brick, sizeof(*brick) * brickLength); // не заработало на C++, так как для C++ нужно использовать new, вот вся строка: brick = new TObject[brickLength];
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#'); 
        InitObject(GetNewBrick(), 60, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 80, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 1, '+');
        movingLength = 1;
        moving = realloc(moving, sizeof(*moving) * movingLength);
        InitObject(moving+0, 25, 10, 3, 2, 'o');
    }
    if (lvl == 2)
    {
        brickLength = 0;
        brick = realloc(brick, sizeof(*brick) * brickLength);
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#'); 
        InitObject(GetNewBrick(), 80, 20, 15, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 15, 10, '#');
        InitObject(GetNewBrick(), 160, 10, 15, 15, '+');
    }
}

void VertMoveObject(TObject *obj) 
{
    (*obj).IsFly = TRUE;
    (*obj).vertSpeed += 0.05;
    SetObjectPos(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);
    for (int i = 0; i < brickLength; i++)
    {
        if(IsCollision(*obj, brick[i]))
        {
            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;
            (*obj).IsFly = FALSE;
            if(brick[i].cType == '+')
            {
                level++;
                if(level > 2) level = 1;
                CreateLevel(level);
                Sleep(1000);    
            }
            break;
        }
    }
}

void DeleteMoving(int i)
{
    movingLength--;
    moving[i] = moving[movingLength];
    moving = realloc(moving, sizeof(*moving) * movingLength);
}

void MarioCollision()
{
    for(int i = 0; i < movingLength; i++)
    {
        if(IsCollision(mario, moving[i]))
        {
            if(    (mario.IsFly == TRUE) 
                && (mario.vertSpeed > 0) 
                && (mario.y + mario.height < moving[i].y + moving[i].height * 0.5)
                ) 
            {
                DeleteMoving(i); 
                i--;
                continue;
            } 
            else 
                CreateLevel(level);
        }
    }
}

void HorizonMoveObject(TObject *obj)
{
    obj[0].x += obj[0].horizSpeed;

    for(int i = 0; i < brickLength; i++)
    {
        if(IsCollision(obj[0], brick[i]))
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
            return;
        }
    }
    
    TObject tmp = *obj;
    VertMoveObject(&tmp);
    if(tmp.IsFly == TRUE)
    {
        obj[0].x -= obj[0].horizSpeed;
        obj[0].horizSpeed = -obj[0].horizSpeed;
    }
}

BOOL IsPosInMap(int x, int y)
{
    return ( (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight) );
}

void PutObjectOnMap(TObject obj)
{
    int ix = (int)round(obj.x); // для round нужно подключить math.h, предыдущий коммит.
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < (ix + iWidth); i++)
        for (int j = iy; j < (iy + iHeight); j++)
            if (IsPosInMap(i, j))
                map[j][i] = obj.cType;
}

void HorizonMoveMap(float dx)
{
    mario.x -= dx;
    for (int i = 0; i < brickLength; i++)
        if(IsCollision(mario, brick[i]))
        {
            mario.x += dx;
            return;
        }
    mario.x += dx;
    for (int i = 0; i < brickLength; i++)
        brick[i].x += dx;
    for (int i = 0; i < movingLength; i++)
        moving[i].x += dx;
}

void SetCur(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

BOOL IsCollision(TObject o1, TObject o2)
{
    return ((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
            ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

int main()
{
    CreateLevel(level);
    system("color 9F");
    do {
        ClearMap();

        if((mario.IsFly == FALSE) && (GetKeyState(VK_SPACE) < 0)) mario.vertSpeed = -1;
        if(GetKeyState('A') < 0) HorizonMoveMap(1);
        if(GetKeyState('D') < 0) HorizonMoveMap(-1);

        if(mario.y > mapHeight) CreateLevel(level);

        VertMoveObject(&mario);
        MarioCollision();

        for (int i = 0; i < brickLength; i++)
            PutObjectOnMap(brick[i]);
        for (int i = 0; i < movingLength; i++)
        {    
            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);
            if(moving[i].y > mapHeight) 
            {
                DeleteMoving(i);
                i--;
                continue;
            }
            PutObjectOnMap(moving[i]);
        }
        PutObjectOnMap(mario);
        SetCur(0, 0);
        ShowMap();
        Sleep(10);
    } while (GetKeyState(VK_ESCAPE) >= 0); // Пока не нажата клавиша ESCAPE, для этого нужен #include <windows.h>
    
    return 0;
}
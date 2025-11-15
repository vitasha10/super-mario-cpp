// Теперь уровень можно пройти
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>

#define mapWidth 80
#define mapHeight 25

constexpr float GRAVITY = 0.05f;
constexpr float MOB_HORIZONTAL_SPEED = 0.2f;
constexpr float JUMP_IMPULSE = -1.0f;
constexpr int FRAME_DELAY_MS = 10;
constexpr int POINTS_FOR_ENEMY = 50;
constexpr int POINTS_FOR_COIN = 100;

constexpr char CHAR_BRICK = '#';
constexpr char CHAR_QUESTION_BLOCK = '?';
constexpr char CHAR_USED_BLOCK = '-';
constexpr char CHAR_GOAL = '+';
constexpr char CHAR_ENEMY = 'o';
constexpr char CHAR_COIN = '$';
constexpr char CHAR_MARIO = '@';

typedef struct SObject {
    float x, y;
    float width, height;
    float vertSpeed;
    bool IsFly;
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
int score;
int maxLvl;

void ClearMap();
void ShowMap();
void SetObjectPos(TObject *obj, float xPos, float yPos);
void InitObject(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType);
void PlayerDead();
TObject *GetNewBrick();
TObject *GetNewMoving();
void PutScoreOnMap();
void CreateLevel(int lvl);
void VertMoveObject(TObject *obj);
void DeleteMoving(int i);
void MarioCollision();
void HorizonMoveObject(TObject *obj);
bool IsPosInMap(int x, int y);
void PutObjectOnMap(const TObject obj);
void HorizonMoveMap(float dx);
void SetCur(int x, int y);
bool IsCollision(const TObject o1, const TObject o2);

void ClearMap()
{
    for (int i = 0; i < mapWidth; i++)
        map[0][i] = ' ';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++)
        snprintf(map[j], sizeof(map[j]), "%s", map[0]);
}

void ShowMap()
{
    map[mapHeight - 1][mapWidth] = '\0';
    for (int j = 0; j < mapHeight; j++)
        printf("%s", map[j]);
}

void SetObjectPos(TObject *obj, float xPos, float yPos)
{
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
    (*obj).horizSpeed = MOB_HORIZONTAL_SPEED;
}

void PlayerDead()
{
    system("color 4F");
    Sleep(500);
    CreateLevel(level);
}

TObject *GetNewBrick()
{
    brickLength++;
    brick = realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}

TObject *GetNewMoving()
{
    movingLength++;
    moving = realloc(moving, sizeof(*moving) * movingLength);
    return moving + movingLength - 1;
}

void PutScoreOnMap()
{
    char c[30];
    snprintf(c, sizeof(c), "Score: %d", score);
    int len = strlen(c);
    for (int i = 0; i < len; i++)
    {
        map[1][i + 5] = c[i];
    }
}

void CreateLevel(int lvl)
{
    system("color 9F");
    brickLength = 0;
    brick = realloc(brick, 0);
    movingLength = 0;
    moving = realloc(moving, 0);

    InitObject(&mario, 39, 10, 3, 3, CHAR_MARIO);
    score = 0;

    if (lvl == 1)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 30, 10, 5, 3, CHAR_QUESTION_BLOCK);
        InitObject(GetNewBrick(), 50, 10, 5, 3, CHAR_QUESTION_BLOCK);
        InitObject(GetNewBrick(), 60, 15, 40, 10, CHAR_BRICK);
        InitObject(GetNewBrick(), 60, 5, 10, 3, CHAR_USED_BLOCK);
        InitObject(GetNewBrick(), 70, 5, 5, 3, CHAR_QUESTION_BLOCK);
        InitObject(GetNewBrick(), 75, 5, 5, 3, CHAR_USED_BLOCK);
        InitObject(GetNewBrick(), 80, 5, 5, 3, CHAR_QUESTION_BLOCK);
        InitObject(GetNewBrick(), 85, 5, 10, 3, CHAR_USED_BLOCK);
        InitObject(GetNewBrick(), 100, 20, 20, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 120, 15, 10, 10, CHAR_BRICK);
        InitObject(GetNewBrick(), 150, 20, 40, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 210, 15, 10, 1, CHAR_GOAL);

        InitObject(GetNewMoving(), 25, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 80, 10, 3, 2, CHAR_ENEMY);
    }
    if (lvl == 2)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 60, 15, 10, 10, CHAR_BRICK);
        InitObject(GetNewBrick(), 80, 20, 20, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 120, 15, 10, 10, CHAR_BRICK);
        InitObject(GetNewBrick(), 150, 20, 40, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 210, 15, 10, 1, CHAR_GOAL);

        InitObject(GetNewMoving(), 25, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 80, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 65, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 120, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 160, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 175, 10, 3, 2, CHAR_ENEMY);
    }
    if (lvl == 3)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 80, 20, 15, 5, CHAR_BRICK);
        InitObject(GetNewBrick(), 120, 15, 15, 10, CHAR_BRICK);
        InitObject(GetNewBrick(), 160, 10, 15, 15, CHAR_GOAL);

        InitObject(GetNewMoving(), 25, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 50, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 80, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 90, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 120, 10, 3, 2, CHAR_ENEMY);
        InitObject(GetNewMoving(), 130, 10, 3, 2, CHAR_ENEMY);
    }
    maxLvl = 3;
}

void VertMoveObject(TObject *obj)
{
    (*obj).IsFly = true;
    (*obj).vertSpeed += GRAVITY;
    SetObjectPos(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);
    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision(*obj, brick[i]))
        {
            if (obj[0].vertSpeed > 0)
                obj[0].IsFly = false;

            if ((brick[i].cType == CHAR_QUESTION_BLOCK) && (obj[0].vertSpeed < 0) && (obj == &mario))
            {
                brick[i].cType = CHAR_USED_BLOCK;
                InitObject(GetNewMoving(), brick[i].x, brick[i].y - 3, 3, 2, CHAR_COIN);
                moving[movingLength - 1].vertSpeed = -0.7;
            }

            (*obj).y -= (*obj).vertSpeed;
            (*obj).vertSpeed = 0;

            if (brick[i].cType == CHAR_GOAL)
            {
                level++;
                if (level > maxLvl)
                    level = 1;
                system("color 2F");
                Sleep(500);
                CreateLevel(level);
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
    for (int i = 0; i < movingLength; i++)
    {
        if (IsCollision(mario, moving[i]))
        {
            if (moving[i].cType == CHAR_ENEMY)
            {
                if ((mario.IsFly == true) &&
                    (mario.vertSpeed > 0) &&
                    (mario.y + mario.height < moving[i].y + moving[i].height * 0.5))
                {
                    score += POINTS_FOR_ENEMY;
                    DeleteMoving(i);
                    i--;
                    continue;
                }
                else
                {
                    PlayerDead();
                }
            }
            if (moving[i].cType == CHAR_COIN)
            {
                score += POINTS_FOR_COIN;
                DeleteMoving(i);
                i--;
                continue;
            }
        }
    }
}

void HorizonMoveObject(TObject *obj)
{
    obj[0].x += obj[0].horizSpeed;

    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision(obj[0], brick[i]))
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
            return;
        }
    }

    if (obj[0].cType == CHAR_ENEMY)
    {
        TObject tmp = *obj;
        VertMoveObject(&tmp);
        if (tmp.IsFly == true)
        {
            obj[0].x -= obj[0].horizSpeed;
            obj[0].horizSpeed = -obj[0].horizSpeed;
        }
    }
}

bool IsPosInMap(int x, int y)
{
    return (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight);
}

void PutObjectOnMap(const TObject obj)
{
    int ix = (int)round(obj.x);
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
    {
        if (IsCollision(mario, brick[i]))
        {
            mario.x += dx;
            return;
        }
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

bool IsCollision(const TObject o1, const TObject o2)
{
    return ((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

int main()
{
    CreateLevel(level);

    do
    {
        ClearMap();

        if ((mario.IsFly == false) && (GetKeyState(VK_SPACE) < 0))
            mario.vertSpeed = JUMP_IMPULSE;
        if (GetKeyState('A') < 0)
            HorizonMoveMap(1);
        if (GetKeyState('D') < 0)
            HorizonMoveMap(-1);

        if (mario.y > mapHeight)
            PlayerDead();

        VertMoveObject(&mario);
        MarioCollision();

        for (int i = 0; i < brickLength; i++)
            PutObjectOnMap(brick[i]);
        for (int i = 0; i < movingLength; i++)
        {
            VertMoveObject(moving + i);
            HorizonMoveObject(moving + i);
            if (moving[i].y > mapHeight)
            {
                DeleteMoving(i);
                i--;
                continue;
            }
            PutObjectOnMap(moving[i]);
        }
        PutObjectOnMap(mario);
        PutScoreOnMap();
        SetCur(0, 0);
        ShowMap();
        Sleep(FRAME_DELAY_MS);
    } while (GetKeyState(VK_ESCAPE) >= 0);

    return 0;
}
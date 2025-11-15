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

void clear_map();
void render_map();
void set_object_pos(TObject *obj, float xPos, float yPos);
void init_object(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType);
void handle_player_death();
TObject *add_brick();
TObject *add_mob();
void draw_score_on_map();
void load_level(int lvl);
void move_object_vertical(TObject *obj);
void remove_mob_by_index(int i);
void handle_mario_collisions();
void move_object_horizontal(TObject *obj);
bool is_within_map(int x, int y);
void place_object_on_map(const TObject obj);
void scroll_map_horizontal(float dx);
void set_cursor_pos(int x, int y);
bool check_collision(const TObject o1, const TObject o2);

void clear_map()
{
    for (int i = 0; i < mapWidth; i++)
        map[0][i] = ' ';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++)
        snprintf(map[j], sizeof(map[j]), "%s", map[0]);
}

void render_map()
{
    map[mapHeight - 1][mapWidth] = '\0';
    for (int j = 0; j < mapHeight; j++)
        printf("%s", map[j]);
}

void set_object_pos(TObject *obj, float xPos, float yPos)
{
    obj->x = xPos;
    obj->y = yPos;
}

void init_object(TObject *obj, float xPos, float yPos, float oWidth, float oHeight, char inType)
{
    set_object_pos(obj, xPos, yPos);
    obj->width = oWidth;
    obj->height = oHeight;
    obj->vertSpeed = 0;
    obj->cType = inType;
    obj->horizSpeed = MOB_HORIZONTAL_SPEED;
}

void handle_player_death()
{
    system("color 4F");
    Sleep(500);
    load_level(level);
}

TObject *add_brick()
{
    brickLength++;
    brick = realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}

TObject *add_mob()
{
    movingLength++;
    moving = realloc(moving, sizeof(*moving) * movingLength);
    return moving + movingLength - 1;
}

void draw_score_on_map()
{
    char c[30];
    snprintf(c, sizeof(c), "Score: %d", score);
    int len = strlen(c);
    for (int i = 0; i < len; i++)
    {
        map[1][i + 5] = c[i];
    }
}

void load_level(int lvl)
{
    system("color 9F");
    brickLength = 0;
    brick = realloc(brick, 0);
    movingLength = 0;
    moving = realloc(moving, 0);

    init_object(&mario, 39, 10, 3, 3, CHAR_MARIO);
    score = 0;

    if (lvl == 1)
    {
        init_object(add_brick(), 20, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(), 30, 10, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(), 50, 10, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(), 60, 15, 40, 10, CHAR_BRICK);
        init_object(add_brick(), 60, 5, 10, 3, CHAR_USED_BLOCK);
        init_object(add_brick(), 70, 5, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(), 75, 5, 5, 3, CHAR_USED_BLOCK);
        init_object(add_brick(), 80, 5, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(), 85, 5, 10, 3, CHAR_USED_BLOCK);
        init_object(add_brick(), 100, 20, 20, 5, CHAR_BRICK);
        init_object(add_brick(), 120, 15, 10, 10, CHAR_BRICK);
        init_object(add_brick(), 150, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(), 210, 15, 10, 1, CHAR_GOAL);

        init_object(add_mob(), 25, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 80, 10, 3, 2, CHAR_ENEMY);
    }
    if (lvl == 2)
    {
        init_object(add_brick(), 20, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(), 60, 15, 10, 10, CHAR_BRICK);
        init_object(add_brick(), 80, 20, 20, 5, CHAR_BRICK);
        init_object(add_brick(), 120, 15, 10, 10, CHAR_BRICK);
        init_object(add_brick(), 150, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(), 210, 15, 10, 1, CHAR_GOAL);

        init_object(add_mob(), 25, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 80, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 65, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 120, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 160, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 175, 10, 3, 2, CHAR_ENEMY);
    }
    if (lvl == 3)
    {
        init_object(add_brick(), 20, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(), 80, 20, 15, 5, CHAR_BRICK);
        init_object(add_brick(), 120, 15, 15, 10, CHAR_BRICK);
        init_object(add_brick(), 160, 10, 15, 15, CHAR_GOAL);

        init_object(add_mob(), 25, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 50, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 80, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 90, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 120, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(), 130, 10, 3, 2, CHAR_ENEMY);
    }
    maxLvl = 3;
}

void move_object_vertical(TObject *obj)
{
    obj->IsFly = true;
    obj->vertSpeed += GRAVITY;
    set_object_pos(obj, obj->x, obj->y + obj->vertSpeed);
    for (int i = 0; i < brickLength; i++)
    {
        if (check_collision(*obj, brick[i]))
        {
            if (obj->vertSpeed > 0)
                obj->IsFly = false;

            if ((brick[i].cType == CHAR_QUESTION_BLOCK) && (obj->vertSpeed < 0) && (obj == &mario))
            {
                brick[i].cType = CHAR_USED_BLOCK;
                init_object(add_mob(), brick[i].x, brick[i].y - 3, 3, 2, CHAR_COIN);
                moving[movingLength - 1].vertSpeed = -0.7;
            }

            obj->y -= obj->vertSpeed;
            obj->vertSpeed = 0;

            if (brick[i].cType == CHAR_GOAL)
            {
                level++;
                if (level > maxLvl)
                    level = 1;
                system("color 2F");
                Sleep(500);
                load_level(level);
            }
            break;
        }
    }
}

void remove_mob_by_index(int i)
{
    movingLength--;
    moving[i] = moving[movingLength];
    moving = realloc(moving, sizeof(*moving) * movingLength);
}

void handle_mario_collisions()
{
    for (int i = 0; i < movingLength; i++)
    {
        if (check_collision(mario, moving[i]))
        {
            if (moving[i].cType == CHAR_ENEMY)
            {
                if ((mario.IsFly == true) &&
                    (mario.vertSpeed > 0) &&
                    (mario.y + mario.height < moving[i].y + moving[i].height * 0.5))
                {
                    score += POINTS_FOR_ENEMY;
                    remove_mob_by_index(i);
                    i--;
                    continue;
                }
                else
                {
                    handle_player_death();
                }
            }
            if (moving[i].cType == CHAR_COIN)
            {
                score += POINTS_FOR_COIN;
                remove_mob_by_index(i);
                i--;
                continue;
            }
        }
    }
}

void move_object_horizontal(TObject *obj)
{
    obj->x += obj->horizSpeed;

    for (int i = 0; i < brickLength; i++)
    {
        if (check_collision(*obj, brick[i]))
        {
            obj->x -= obj->horizSpeed;
            obj->horizSpeed = -obj->horizSpeed;
            return;
        }
    }

    if (obj->cType == CHAR_ENEMY)
    {
        TObject tmp = *obj;
        move_object_vertical(&tmp);
        if (tmp.IsFly == true)
        {
            obj->x -= obj->horizSpeed;
            obj->horizSpeed = -obj->horizSpeed;
        }
    }
}

bool is_within_map(int x, int y)
{
    return (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight);
}

void place_object_on_map(const TObject obj)
{
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);

    for (int i = ix; i < (ix + iWidth); i++)
        for (int j = iy; j < (iy + iHeight); j++)
            if (is_within_map(i, j))
                map[j][i] = obj.cType;
}

void scroll_map_horizontal(float dx)
{
    mario.x -= dx;
    for (int i = 0; i < brickLength; i++)
    {
        if (check_collision(mario, brick[i]))
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

void set_cursor_pos(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

bool check_collision(const TObject o1, const TObject o2)
{
    return ((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
           ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}

int main()
{
    load_level(level);

    do
    {
        clear_map();

        if ((mario.IsFly == false) && (GetKeyState(VK_SPACE) < 0))
            mario.vertSpeed = JUMP_IMPULSE;
        if (GetKeyState('A') < 0)
            scroll_map_horizontal(1);
        if (GetKeyState('D') < 0)
            scroll_map_horizontal(-1);

        if (mario.y > mapHeight)
            handle_player_death();

        move_object_vertical(&mario);
        handle_mario_collisions();

        for (int i = 0; i < brickLength; i++)
            place_object_on_map(brick[i]);
        for (int i = 0; i < movingLength; i++)
        {
            move_object_vertical(moving + i);
            move_object_horizontal(moving + i);
            if (moving[i].y > mapHeight)
            {
                remove_mob_by_index(i);
                i--;
                continue;
            }
            place_object_on_map(moving[i]);
        }
        place_object_on_map(mario);
        draw_score_on_map();
        set_cursor_pos(0, 0);
        render_map();
        Sleep(FRAME_DELAY_MS);
    } while (GetKeyState(VK_ESCAPE) >= 0);

    return 0;
}
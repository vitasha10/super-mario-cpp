// Super Mario C++ - Simple platformer game
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <windows.h>

constexpr int MAP_WIDTH = 80;
constexpr int MAP_HEIGHT = 25;

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
    float vy;
    bool is_flying;
    char glyph;
    float vx;
} TObject;

char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1];
TObject mario;

TObject *bricks = NULL;
int bricks_count;

TObject *mobs = NULL;
int mobs_count;

int level = 1;
int score;
int max_level;

void sleep_ms(unsigned ms);
void set_console_color(unsigned short color);

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
bool is_stomp_kill(const TObject& player, const TObject& enemy);
bool is_coin_pickup(const TObject& obj);
void add_score_and_remove_mob(int points, int& index);
void handle_input();
void update_world();
void render_frame();

void sleep_ms(unsigned ms)
{
    Sleep(ms);
}

void set_console_color(unsigned short color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void clear_map()
{
    for (int i = 0; i < MAP_WIDTH; i++)
        screen_buffer[0][i] = ' ';
    screen_buffer[0][MAP_WIDTH] = '\0';
    for (int j = 1; j < MAP_HEIGHT; j++)
        snprintf(screen_buffer[j], sizeof(screen_buffer[j]), "%s", screen_buffer[0]);
}

void render_map()
{
    screen_buffer[MAP_HEIGHT - 1][MAP_WIDTH] = '\0';
    std::string output;
    output.reserve(MAP_HEIGHT * (MAP_WIDTH + 1));
    for (int j = 0; j < MAP_HEIGHT; j++)
    {
        output += screen_buffer[j];
    }
    std::cout << output << std::flush;
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
    obj->vy = 0;
    obj->glyph = inType;
    obj->vx = MOB_HORIZONTAL_SPEED;
}

void handle_player_death()
{
    set_console_color(0x4F);
    sleep_ms(500);
    load_level(level);
}

TObject *add_brick()
{
    bricks_count++;
    bricks = realloc(bricks, sizeof(*bricks) * bricks_count);
    return bricks + bricks_count - 1;
}

TObject *add_mob()
{
    mobs_count++;
    mobs = realloc(mobs, sizeof(*mobs) * mobs_count);
    return mobs + mobs_count - 1;
}

void draw_score_on_map()
{
    std::string score_text = "Score: " + std::to_string(score);
    int len = score_text.length();
    for (int i = 0; i < len; i++)
    {
        screen_buffer[1][i + 5] = score_text[i];
    }
}

void load_level(int lvl)
{
    set_console_color(0x9F);
    bricks_count = 0;
    bricks = realloc(bricks, 0);
    mobs_count = 0;
    mobs = realloc(mobs, 0);

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
    max_level = 3;
}

void move_object_vertical(TObject *obj)
{
    obj->is_flying = true;
    obj->vy += GRAVITY;
    set_object_pos(obj, obj->x, obj->y + obj->vy);
    for (int i = 0; i < bricks_count; i++)
    {
        if (check_collision(*obj, bricks[i]))
        {
            if (obj->vy > 0)
                obj->is_flying = false;

            if ((bricks[i].glyph == CHAR_QUESTION_BLOCK) && (obj->vy < 0) && (obj == &mario))
            {
                bricks[i].glyph = CHAR_USED_BLOCK;
                init_object(add_mob(), bricks[i].x, bricks[i].y - 3, 3, 2, CHAR_COIN);
                mobs[mobs_count - 1].vy = -0.7;
            }

            obj->y -= obj->vy;
            obj->vy = 0;

            // Level transition when hitting goal
            if (bricks[i].glyph == CHAR_GOAL)
            {
                level++;
                if (level > max_level)
                    level = 1;
                set_console_color(0x2F);
                sleep_ms(500);
                load_level(level);
            }
            break;
        }
    }
}

void remove_mob_by_index(int i)
{
    mobs_count--;
    mobs[i] = mobs[mobs_count];
    mobs = realloc(mobs, sizeof(*mobs) * mobs_count);
}

bool is_stomp_kill(const TObject& player, const TObject& enemy)
{
    return (player.is_flying == true) &&
           (player.vy > 0) &&
           (player.y + player.height < enemy.y + enemy.height * 0.5);
}

bool is_coin_pickup(const TObject& obj)
{
    return obj.glyph == CHAR_COIN;
}

void add_score_and_remove_mob(int points, int& index)
{
    score += points;
    remove_mob_by_index(index);
    index--;
}

void handle_mario_collisions()
{
    for (int i = 0; i < mobs_count; i++)
    {
        if (!check_collision(mario, mobs[i]))
            continue;

        if (mobs[i].glyph == CHAR_ENEMY)
        {
            if (is_stomp_kill(mario, mobs[i]))
            {
                add_score_and_remove_mob(POINTS_FOR_ENEMY, i);
                continue;
            }
            handle_player_death();
            return;
        }

        if (is_coin_pickup(mobs[i]))
        {
            add_score_and_remove_mob(POINTS_FOR_COIN, i);
        }
    }
}

void move_object_horizontal(TObject *obj)
{
    obj->x += obj->vx;

    for (int i = 0; i < bricks_count; i++)
    {
        if (check_collision(*obj, bricks[i]))
        {
            obj->x -= obj->vx;
            obj->vx = -obj->vx;
            return;
        }
    }

    if (obj->glyph == CHAR_ENEMY)
    {
        TObject tmp = *obj;
        move_object_vertical(&tmp);
        if (tmp.is_flying == true)
        {
            obj->x -= obj->vx;
            obj->vx = -obj->vx;
        }
    }
}

bool is_within_map(int x, int y)
{
    return (x >= 0) && (x < MAP_WIDTH) && (y >= 0) && (y < MAP_HEIGHT);
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
                screen_buffer[j][i] = obj.glyph;
}

void scroll_map_horizontal(float dx)
{
    mario.x -= dx;
    for (int i = 0; i < bricks_count; i++)
    {
        if (check_collision(mario, bricks[i]))
        {
            mario.x += dx;
            return;
        }
    }
    mario.x += dx;
    for (int i = 0; i < bricks_count; i++)
        bricks[i].x += dx;
    for (int i = 0; i < mobs_count; i++)
        mobs[i].x += dx;
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

void handle_input()
{
    if ((mario.is_flying == false) && (GetKeyState(VK_SPACE) < 0))
        mario.vy = JUMP_IMPULSE;
    if (GetKeyState('A') < 0)
        scroll_map_horizontal(1);
    if (GetKeyState('D') < 0)
        scroll_map_horizontal(-1);
}

void update_world()
{
    if (mario.y > MAP_HEIGHT)
        handle_player_death();

    move_object_vertical(&mario);
    handle_mario_collisions();

    for (int i = 0; i < mobs_count; i++)
    {
        move_object_vertical(mobs + i);
        move_object_horizontal(mobs + i);
        if (mobs[i].y > MAP_HEIGHT)
        {
            remove_mob_by_index(i);
            i--;
        }
    }
}

void render_frame()
{
    clear_map();

    for (int i = 0; i < bricks_count; i++)
        place_object_on_map(bricks[i]);
    for (int i = 0; i < mobs_count; i++)
        place_object_on_map(mobs[i]);
    place_object_on_map(mario);
    draw_score_on_map();

    set_cursor_pos(0, 0);
    render_map();
}

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    load_level(level);

    do
    {
        handle_input();
        update_world();
        render_frame();
        sleep_ms(FRAME_DELAY_MS);
    } while (GetKeyState(VK_ESCAPE) >= 0);

    return 0;
}
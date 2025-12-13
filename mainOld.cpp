// Super Mario C++ - Simple platformer game
// Восстановление логики оригинала с ТРЕБУЕМЫМИ изменениями:
// 1. ВСЕ ОБЪЯВЛЕНИЯ функций по алфавиту
// 2. Затем main()
// 3. ВСЕ РЕАЛИЗАЦИИ функций по алфавиту
// 4. НЕТ глобальных изменяемых переменных (только constexpr-константы)
// 5. ИГРОВОЕ СОСТОЯНИЕ НЕ объединено в одну структуру (без GameState / ООП)
// 6. Поведение максимально соответствует исходному коду

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <windows.h>

// ---------- Константы ----------
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

constexpr WORD DEFAULT_COLOR = 0x9F; // Базовый цвет (фон ярко-синий, текст белый)

struct SObject {
    float x, y;
    float width, height;
    float vy;
    bool is_flying;
    char glyph;
    float vx;
};
using TObject = SObject;

// ---------- ОБЪЯВЛЕНИЯ (Alphabetical) ----------
TObject* add_brick(TObject*& bricks, int& bricks_count);
TObject* add_mob(TObject*& mobs, int& mobs_count);
void add_score_and_remove_mob(int points, int& score, TObject*& mobs, int& mobs_count, int& index);
void apply_color_to_entire_buffer(WORD color);
bool check_collision(const TObject& o1, const TObject& o2);
void clear_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1]);
void draw_score_on_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1], int score);
void handle_input(TObject& mario,
                  TObject* bricks, int bricks_count,
                  TObject* mobs, int mobs_count);
void handle_mario_collisions(TObject& mario,
                             char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                             TObject*& bricks, int& bricks_count,
                             TObject*& mobs, int& mobs_count,
                             int& level, int& score, int& max_level);
void handle_player_death(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                         TObject& mario,
                         TObject*& bricks, int& bricks_count,
                         TObject*& mobs, int& mobs_count,
                         int& level, int& score, int& max_level);
void init_object(TObject* obj, float xPos, float yPos, float oWidth, float oHeight, char inType);
bool is_coin_pickup(const TObject& obj);
bool is_stomp_kill(const TObject& player, const TObject& enemy);
bool is_within_map(int x, int y);
void load_level(int lvl,
                TObject& mario,
                TObject*& bricks, int& bricks_count,
                TObject*& mobs, int& mobs_count,
                int& score, int& max_level);
void move_object_horizontal(TObject* obj,
                            char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                            TObject& mario,
                            TObject*& bricks, int& bricks_count,
                            TObject*& mobs, int& mobs_count,
                            int& level, int& score, int& max_level);
void move_object_vertical(TObject* obj,
                          char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                          TObject& mario,
                          TObject*& bricks, int& bricks_count,
                          TObject*& mobs, int& mobs_count,
                          int& level, int& score, int& max_level);
void place_object_on_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1], const TObject& obj);
void remove_mob_by_index(TObject*& mobs, int& mobs_count, int i);
void render_frame(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                  TObject& mario,
                  TObject* bricks, int bricks_count,
                  TObject* mobs, int mobs_count,
                  int score);
void render_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1]);
void scroll_map_horizontal(float dx,
                           TObject& mario,
                           TObject* bricks, int bricks_count,
                           TObject* mobs, int mobs_count);
void set_console_color(unsigned short color);
void set_cursor_pos(int x, int y);
void set_object_pos(TObject* obj, float xPos, float yPos);
void sleep_ms(unsigned ms);
void update_world(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                  TObject& mario,
                  TObject*& bricks, int& bricks_count,
                  TObject*& mobs, int& mobs_count,
                  int& level, int& score, int& max_level);

// ---------- main ----------
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    // Локальные переменные состояния (были глобальными)
    char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1];
    TObject mario{};
    TObject* bricks = nullptr;
    int bricks_count = 0;
    TObject* mobs = nullptr;
    int mobs_count = 0;
    int level = 1;
    int score = 0;
    int max_level = 0;

    set_console_color(DEFAULT_COLOR);
    load_level(level, mario, bricks, bricks_count, mobs, mobs_count, score, max_level);

    do {
        handle_input(mario, bricks, bricks_count, mobs, mobs_count);
        update_world(screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);
        render_frame(screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, score);
        sleep_ms(FRAME_DELAY_MS);
    } while (GetKeyState(VK_ESCAPE) >= 0);

    std::free(bricks);
    std::free(mobs);
    return 0;
}

// ---------- РЕАЛИЗАЦИИ (Alphabetical) ----------
TObject* add_brick(TObject*& bricks, int& bricks_count) {
    bricks_count++;
    bricks = static_cast<TObject*>(std::realloc(bricks, sizeof(TObject) * bricks_count));
    return bricks + bricks_count - 1;
}

TObject* add_mob(TObject*& mobs, int& mobs_count) {
    mobs_count++;
    mobs = static_cast<TObject*>(std::realloc(mobs, sizeof(TObject) * mobs_count));
    return mobs + mobs_count - 1;
}

void add_score_and_remove_mob(int points, int& score, TObject*& mobs, int& mobs_count, int& index) {
    score += points;
    remove_mob_by_index(mobs, mobs_count, index);
    index--;
}

void apply_color_to_entire_buffer(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y;
    COORD home = {0, 0};
    DWORD written;
    FillConsoleOutputAttribute(hConsole, color, cells, home, &written);
}

bool check_collision(const TObject& o1, const TObject& o2) {
    return (o1.x + o1.width > o2.x) && (o1.x < o2.x + o2.width) &&
           (o1.y + o1.height > o2.y) && (o1.y < o2.y + o2.height);
}

void clear_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1]) {
    for (int i = 0; i < MAP_WIDTH; i++)
        screen_buffer[0][i] = ' ';
    screen_buffer[0][MAP_WIDTH] = '\0';
    for (int j = 1; j < MAP_HEIGHT; j++)
        std::snprintf(screen_buffer[j], sizeof(screen_buffer[j]), "%s", screen_buffer[0]);
}

void draw_score_on_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1], int score) {
    std::string score_text = "Score: " + std::to_string(score);
    int len = static_cast<int>(score_text.length());
    for (int i = 0; i < len; i++)
        if (i + 5 < MAP_WIDTH)
            screen_buffer[1][i + 5] = score_text[i];
}

void handle_input(TObject& mario,
                  TObject* bricks, int bricks_count,
                  TObject* mobs, int mobs_count) {
    (void)bricks; (void)bricks_count; (void)mobs; (void)mobs_count; // Сейчас не нужны напрямую
    if (!mario.is_flying && (GetKeyState(VK_SPACE) < 0))
        mario.vy = JUMP_IMPULSE;
    if (GetKeyState('A') < 0)
        scroll_map_horizontal(1.0f, mario, bricks, bricks_count, mobs, mobs_count);
    if (GetKeyState('D') < 0)
        scroll_map_horizontal(-1.0f, mario, bricks, bricks_count, mobs, mobs_count);
}

void handle_mario_collisions(TObject& mario,
                             char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                             TObject*& bricks, int& bricks_count,
                             TObject*& mobs, int& mobs_count,
                             int& level, int& score, int& max_level) {
    for (int i = 0; i < mobs_count; i++) {
        if (!check_collision(mario, mobs[i]))
            continue;

        if (mobs[i].glyph == CHAR_ENEMY) {
            if (is_stomp_kill(mario, mobs[i])) {
                add_score_and_remove_mob(POINTS_FOR_ENEMY, score, mobs, mobs_count, i);
                continue;
            }
            handle_player_death(screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);
            return;
        }

        if (is_coin_pickup(mobs[i]))
            add_score_and_remove_mob(POINTS_FOR_COIN, score, mobs, mobs_count, i);
    }
}

void handle_player_death(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                         TObject& mario,
                         TObject*& bricks, int& bricks_count,
                         TObject*& mobs, int& mobs_count,
                         int& level, int& score, int& max_level) {
    set_console_color(0x4F);
    render_frame(screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, score);
    sleep_ms(500);
    set_console_color(DEFAULT_COLOR);
    load_level(level, mario, bricks, bricks_count, mobs, mobs_count, score, max_level);
}

void init_object(TObject* obj, float xPos, float yPos, float oWidth, float oHeight, char inType) {
    set_object_pos(obj, xPos, yPos);
    obj->width = oWidth;
    obj->height = oHeight;
    obj->vy = 0;
    obj->glyph = inType;
    obj->vx = MOB_HORIZONTAL_SPEED;
    // В оригинале is_flying НЕ устанавливался здесь (оставляем как есть)
}

bool is_coin_pickup(const TObject& obj) {
    return obj.glyph == CHAR_COIN;
}

bool is_stomp_kill(const TObject& player, const TObject& enemy) {
    return player.is_flying &&
           (player.vy > 0) &&
           (player.y + player.height < enemy.y + enemy.height * 0.5f);
}

bool is_within_map(int x, int y) {
    return (x >= 0) && (x < MAP_WIDTH) && (y >= 0) && (y < MAP_HEIGHT);
}

void load_level(int lvl,
                TObject& mario,
                TObject*& bricks, int& bricks_count,
                TObject*& mobs, int& mobs_count,
                int& score, int& max_level) {
    bricks_count = 0;
    bricks = static_cast<TObject*>(std::realloc(bricks, 0));
    mobs_count = 0;
    mobs = static_cast<TObject*>(std::realloc(mobs, 0));

    init_object(&mario, 39, 10, 3, 3, CHAR_MARIO);
    mario.is_flying = false; // глобал в оригинале инициализировался нулями
    score = 0;

    if (lvl == 1) {
        init_object(add_brick(bricks, bricks_count), 20, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 30, 10, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(bricks, bricks_count), 50, 10, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(bricks, bricks_count), 60, 15, 40, 10, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 60, 5, 10, 3, CHAR_USED_BLOCK);
        init_object(add_brick(bricks, bricks_count), 70, 5, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(bricks, bricks_count), 75, 5, 5, 3, CHAR_USED_BLOCK);
        init_object(add_brick(bricks, bricks_count), 80, 5, 5, 3, CHAR_QUESTION_BLOCK);
        init_object(add_brick(bricks, bricks_count), 85, 5, 10, 3, CHAR_USED_BLOCK);
        init_object(add_brick(bricks, bricks_count), 100, 20, 20, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 120, 15, 10, 10, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 150, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 210, 15, 10, 1, CHAR_GOAL);

        init_object(add_mob(mobs, mobs_count), 25, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 80, 10, 3, 2, CHAR_ENEMY);
    } else if (lvl == 2) {
        init_object(add_brick(bricks, bricks_count), 20, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 60, 15, 10, 10, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 80, 20, 20, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 120, 15, 10, 10, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 150, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 210, 15, 10, 1, CHAR_GOAL);

        init_object(add_mob(mobs, mobs_count), 25, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 80, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 65, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 120, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 160, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 175, 10, 3, 2, CHAR_ENEMY);
    } else if (lvl == 3) {
        init_object(add_brick(bricks, bricks_count), 20, 20, 40, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 80, 20, 15, 5, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 120, 15, 15, 10, CHAR_BRICK);
        init_object(add_brick(bricks, bricks_count), 160, 10, 15, 15, CHAR_GOAL);

        init_object(add_mob(mobs, mobs_count), 25, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 50, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 80, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 90, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 120, 10, 3, 2, CHAR_ENEMY);
        init_object(add_mob(mobs, mobs_count), 130, 10, 3, 2, CHAR_ENEMY);
    }
    max_level = 3;
}

void move_object_horizontal(TObject* obj,
                            char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                            TObject& mario,
                            TObject*& bricks, int& bricks_count,
                            TObject*& mobs, int& mobs_count,
                            int& level, int& score, int& max_level) {
    obj->x += obj->vx;

    for (int i = 0; i < bricks_count; i++) {
        if (check_collision(*obj, bricks[i])) {
            obj->x -= obj->vx;
            obj->vx = -obj->vx;
            return;
        }
    }

    if (obj->glyph == CHAR_ENEMY) {
        TObject tmp = *obj;
        move_object_vertical(&tmp, screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);
        if (tmp.is_flying) {
            obj->x -= obj->vx;
            obj->vx = -obj->vx;
        }
    }
}

void move_object_vertical(TObject* obj,
                          char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                          TObject& mario,
                          TObject*& bricks, int& bricks_count,
                          TObject*& mobs, int& mobs_count,
                          int& level, int& score, int& max_level) {
    obj->is_flying = true;
    obj->vy += GRAVITY;
    set_object_pos(obj, obj->x, obj->y + obj->vy);

    for (int i = 0; i < bricks_count; i++) {
        if (check_collision(*obj, bricks[i])) {
            if (obj->vy > 0)
                obj->is_flying = false;

            if ((bricks[i].glyph == CHAR_QUESTION_BLOCK) && (obj->vy < 0) && (obj == &mario)) {
                bricks[i].glyph = CHAR_USED_BLOCK;
                init_object(add_mob(mobs, mobs_count), bricks[i].x, bricks[i].y - 3, 3, 2, CHAR_COIN);
                mobs[mobs_count - 1].vy = -0.7f;
            }

            obj->y -= obj->vy;
            obj->vy = 0;

            if (bricks[i].glyph == CHAR_GOAL) {
                level++;
                if (level > max_level)
                    level = 1;

                set_console_color(0x2F);
                render_frame(screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, score);
                sleep_ms(500);
                set_console_color(DEFAULT_COLOR);
                load_level(level, mario, bricks, bricks_count, mobs, mobs_count, score, max_level);
            }
            break;
        }
    }
}

void place_object_on_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1], const TObject& obj) {
    int ix = static_cast<int>(std::round(obj.x));
    int iy = static_cast<int>(std::round(obj.y));
    int iWidth = static_cast<int>(std::round(obj.width));
    int iHeight = static_cast<int>(std::round(obj.height));

    for (int i = ix; i < ix + iWidth; i++)
        for (int j = iy; j < iy + iHeight; j++)
            if (is_within_map(i, j))
                screen_buffer[j][i] = obj.glyph;
}

void remove_mob_by_index(TObject*& mobs, int& mobs_count, int i) {
    mobs_count--;
    if (mobs_count > 0) {
        mobs[i] = mobs[mobs_count];
        mobs = static_cast<TObject*>(std::realloc(mobs, sizeof(TObject) * mobs_count));
    } else {
        std::free(mobs);
        mobs = nullptr;
    }
}

void render_frame(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                  TObject& mario,
                  TObject* bricks, int bricks_count,
                  TObject* mobs, int mobs_count,
                  int score) {
    clear_map(screen_buffer);
    for (int i = 0; i < bricks_count; i++)
        place_object_on_map(screen_buffer, bricks[i]);
    for (int i = 0; i < mobs_count; i++)
        place_object_on_map(screen_buffer, mobs[i]);
    place_object_on_map(screen_buffer, mario);
    draw_score_on_map(screen_buffer, score);

    set_cursor_pos(0, 0);
    render_map(screen_buffer);
}

void render_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1]) {
    screen_buffer[MAP_HEIGHT - 1][MAP_WIDTH] = '\0';
    std::string output;
    output.reserve(MAP_HEIGHT * (MAP_WIDTH + 1));
    for (int j = 0; j < MAP_HEIGHT; j++)
        output += screen_buffer[j];
    std::cout << output << std::flush;
}

void scroll_map_horizontal(float dx,
                           TObject& mario,
                           TObject* bricks, int bricks_count,
                           TObject* mobs, int mobs_count) {
    mario.x -= dx;
    for (int i = 0; i < bricks_count; i++)
        if (check_collision(mario, bricks[i])) {
            mario.x += dx;
            return;
        }

    mario.x += dx;
    for (int i = 0; i < bricks_count; i++)
        bricks[i].x += dx;
    for (int i = 0; i < mobs_count; i++)
        mobs[i].x += dx;
}

void set_console_color(unsigned short color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void set_cursor_pos(int x, int y) {
    COORD coord;
    coord.X = static_cast<SHORT>(x);
    coord.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void set_object_pos(TObject* obj, float xPos, float yPos) {
    obj->x = xPos;
    obj->y = yPos;
}

void sleep_ms(unsigned ms) {
    Sleep(ms);
}

void update_world(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1],
                  TObject& mario,
                  TObject*& bricks, int& bricks_count,
                  TObject*& mobs, int& mobs_count,
                  int& level, int& score, int& max_level) {
    if (mario.y > MAP_HEIGHT)
        handle_player_death(screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);

    move_object_vertical(&mario, screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);
    handle_mario_collisions(mario, screen_buffer, bricks, bricks_count, mobs, mobs_count, level, score, max_level);

    for (int i = 0; i < mobs_count; i++) {
        move_object_vertical(mobs + i, screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);
        move_object_horizontal(mobs + i, screen_buffer, mario, bricks, bricks_count, mobs, mobs_count, level, score, max_level);
        if (mobs[i].y > MAP_HEIGHT) {
            remove_mob_by_index(mobs, mobs_count, i);
            i--;
        }
    }
}
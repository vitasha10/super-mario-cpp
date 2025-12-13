// Super Mario C++ - ООП версия
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
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

constexpr WORD DEFAULT_COLOR = 0x9F;

namespace {
bool is_within_map(int x, int y) {
    return (x >= 0) && (x < MAP_WIDTH) && (y >= 0) && (y < MAP_HEIGHT);
}
} // namespace

class Console {
public:
    static void set_color(unsigned short color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    }

    static void set_cursor_pos(int x, int y) {
        COORD coord;
        coord.X = static_cast<SHORT>(x);
        coord.Y = static_cast<SHORT>(y);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    static void sleep_ms(unsigned ms) {
        Sleep(ms);
    }
};

class Object {
public:
    Object() = default;
    Object(float x, float y, float width, float height, char glyph)
        : x_(x), y_(y), width_(width), height_(height), vy_(0.0f), is_flying_(false), glyph_(glyph),
          vx_(0.0f) {}

    float x() const { return x_; }
    float y() const { return y_; }
    float width() const { return width_; }
    float height() const { return height_; }
    float vy() const { return vy_; }
    float vx() const { return vx_; }
    bool is_flying() const { return is_flying_; }
    char glyph() const { return glyph_; }

    void set_position(float x, float y) {
        x_ = x;
        y_ = y;
    }

    void set_vy(float vy) { vy_ = vy; }
    void add_vy(float delta) { vy_ += delta; }
    void set_vx(float vx) { vx_ = vx; }
    void set_flying(bool flying) { is_flying_ = flying; }
    void set_glyph(char glyph) { glyph_ = glyph; }

    bool is_coin() const { return glyph_ == CHAR_COIN; }
    bool is_enemy() const { return glyph_ == CHAR_ENEMY; }
    bool is_goal() const { return glyph_ == CHAR_GOAL; }

    bool collides_with(const Object& other) const {
        return (x_ + width_ > other.x_) && (x_ < other.x_ + other.width_) &&
               (y_ + height_ > other.y_) && (y_ < other.y_ + other.height_);
    }

    void place_on_map(char screen_buffer[MAP_HEIGHT][MAP_WIDTH + 1]) const {
        int ix = static_cast<int>(std::round(x_));
        int iy = static_cast<int>(std::round(y_));
        int iWidth = static_cast<int>(std::round(width_));
        int iHeight = static_cast<int>(std::round(height_));

        for (int i = ix; i < ix + iWidth; i++)
            for (int j = iy; j < iy + iHeight; j++)
                if (is_within_map(i, j))
                    screen_buffer[j][i] = glyph_;
    }

private:
    float x_{};
    float y_{};
    float width_{};
    float height_{};
    float vy_{};
    bool is_flying_{};
    char glyph_{' '};
    float vx_{};
};

class Game {
public:
    Game() {
        Console::set_color(DEFAULT_COLOR);
        load_level(level_);
    }

    void run() {
        do {
            handle_input();
            update_world();
            render_frame();
            Console::sleep_ms(FRAME_DELAY_MS);
        } while (GetKeyState(VK_ESCAPE) >= 0);
    }

private:
    void handle_input() {
        if (!mario_.is_flying() && (GetKeyState(VK_SPACE) < 0))
            mario_.set_vy(JUMP_IMPULSE);
        if (GetKeyState('A') < 0)
            scroll_map(1.0f);
        if (GetKeyState('D') < 0)
            scroll_map(-1.0f);
    }

    void update_world() {
        if (mario_.y() > MAP_HEIGHT)
            handle_player_death();

        move_vertical(mario_, true);
        handle_mario_collisions();

        for (std::size_t i = 0; i < mobs_.size();) {
            move_vertical(mobs_[i], false);
            move_horizontal(mobs_[i]);
            if (mobs_[i].y() > MAP_HEIGHT) {
                mobs_.erase(mobs_.begin() + i);
            } else {
                ++i;
            }
        }
    }

    void render_frame() {
        clear_map();
        for (const auto& brick : bricks_)
            brick.place_on_map(screen_buffer_);
        for (const auto& mob : mobs_)
            mob.place_on_map(screen_buffer_);
        mario_.place_on_map(screen_buffer_);
        draw_score();

        Console::set_cursor_pos(0, 0);
        render_map();
    }

    void clear_map() {
        for (int i = 0; i < MAP_WIDTH; i++)
            screen_buffer_[0][i] = ' ';
        screen_buffer_[0][MAP_WIDTH] = '\0';
        for (int j = 1; j < MAP_HEIGHT; j++)
            std::snprintf(screen_buffer_[j], sizeof(screen_buffer_[j]), "%s", screen_buffer_[0]);
    }

    void draw_score() {
        std::string score_text = "Score: " + std::to_string(score_);
        int len = static_cast<int>(score_text.length());
        for (int i = 0; i < len; i++)
            if (i + 5 < MAP_WIDTH)
                screen_buffer_[1][i + 5] = score_text[i];
    }

    void render_map() {
        screen_buffer_[MAP_HEIGHT - 1][MAP_WIDTH] = '\0';
        std::string output;
        output.reserve(MAP_HEIGHT * (MAP_WIDTH + 1));
        for (int j = 0; j < MAP_HEIGHT; j++)
            output += screen_buffer_[j];
        std::cout << output << std::flush;
    }

    void load_level(int lvl) {
        bricks_.clear();
        mobs_.clear();
        mario_ = Object(39, 10, 3, 3, CHAR_MARIO);
        mario_.set_flying(false);
        score_ = 0;

        if (lvl == 1) {
            add_brick(20, 20, 40, 5, CHAR_BRICK);
            add_brick(30, 10, 5, 3, CHAR_QUESTION_BLOCK);
            add_brick(50, 10, 5, 3, CHAR_QUESTION_BLOCK);
            add_brick(60, 15, 40, 10, CHAR_BRICK);
            add_brick(60, 5, 10, 3, CHAR_USED_BLOCK);
            add_brick(70, 5, 5, 3, CHAR_QUESTION_BLOCK);
            add_brick(75, 5, 5, 3, CHAR_USED_BLOCK);
            add_brick(80, 5, 5, 3, CHAR_QUESTION_BLOCK);
            add_brick(85, 5, 10, 3, CHAR_USED_BLOCK);
            add_brick(100, 20, 20, 5, CHAR_BRICK);
            add_brick(120, 15, 10, 10, CHAR_BRICK);
            add_brick(150, 20, 40, 5, CHAR_BRICK);
            add_brick(210, 15, 10, 1, CHAR_GOAL);

            add_mob(25, 10, 3, 2, CHAR_ENEMY);
            add_mob(80, 10, 3, 2, CHAR_ENEMY);
        } else if (lvl == 2) {
            add_brick(20, 20, 40, 5, CHAR_BRICK);
            add_brick(60, 15, 10, 10, CHAR_BRICK);
            add_brick(80, 20, 20, 5, CHAR_BRICK);
            add_brick(120, 15, 10, 10, CHAR_BRICK);
            add_brick(150, 20, 40, 5, CHAR_BRICK);
            add_brick(210, 15, 10, 1, CHAR_GOAL);

            add_mob(25, 10, 3, 2, CHAR_ENEMY);
            add_mob(80, 10, 3, 2, CHAR_ENEMY);
            add_mob(65, 10, 3, 2, CHAR_ENEMY);
            add_mob(120, 10, 3, 2, CHAR_ENEMY);
            add_mob(160, 10, 3, 2, CHAR_ENEMY);
            add_mob(175, 10, 3, 2, CHAR_ENEMY);
        } else if (lvl == 3) {
            add_brick(20, 20, 40, 5, CHAR_BRICK);
            add_brick(80, 20, 15, 5, CHAR_BRICK);
            add_brick(120, 15, 15, 10, CHAR_BRICK);
            add_brick(160, 10, 15, 15, CHAR_GOAL);

            add_mob(25, 10, 3, 2, CHAR_ENEMY);
            add_mob(50, 10, 3, 2, CHAR_ENEMY);
            add_mob(80, 10, 3, 2, CHAR_ENEMY);
            add_mob(90, 10, 3, 2, CHAR_ENEMY);
            add_mob(120, 10, 3, 2, CHAR_ENEMY);
            add_mob(130, 10, 3, 2, CHAR_ENEMY);
        }
        max_level_ = 3;
    }

    void add_brick(float x, float y, float width, float height, char glyph) {
        bricks_.emplace_back(x, y, width, height, glyph);
    }

    void add_mob(float x, float y, float width, float height, char glyph) {
        mobs_.emplace_back(x, y, width, height, glyph);
        mobs_.back().set_vx(MOB_HORIZONTAL_SPEED);
    }

    void move_vertical(Object& obj, bool handle_special) {
        obj.set_flying(true);
        obj.add_vy(GRAVITY);
        obj.set_position(obj.x(), obj.y() + obj.vy());
        float prev_vy = obj.vy();

        for (auto& brick : bricks_) {
            if (!obj.collides_with(brick))
                continue;

            if (prev_vy > 0)
                obj.set_flying(false);

            if (handle_special && brick.glyph() == CHAR_QUESTION_BLOCK && prev_vy < 0 && obj.glyph() == CHAR_MARIO) {
                brick.set_glyph(CHAR_USED_BLOCK);
                add_mob(brick.x(), brick.y() - 3, 3, 2, CHAR_COIN);
                mobs_.back().set_vy(-0.7f);
            }

            obj.set_position(obj.x(), obj.y() - prev_vy);
            obj.set_vy(0);

            if (handle_special && brick.glyph() == CHAR_GOAL) {
                level_++;
                if (level_ > max_level_)
                    level_ = 1;

                Console::set_color(0x2F);
                render_frame();
                Console::sleep_ms(500);
                Console::set_color(DEFAULT_COLOR);
                load_level(level_);
            }
            break;
        }
    }

    void move_horizontal(Object& obj) {
        obj.set_position(obj.x() + obj.vx(), obj.y());

        for (const auto& brick : bricks_) {
            if (obj.collides_with(brick)) {
                obj.set_position(obj.x() - obj.vx(), obj.y());
                obj.set_vx(-obj.vx());
                return;
            }
        }

        if (obj.is_enemy()) {
            Object tmp = obj;
            move_vertical(tmp, false);
            if (tmp.is_flying()) {
                obj.set_position(obj.x() - obj.vx(), obj.y());
                obj.set_vx(-obj.vx());
            }
        }
    }

    void handle_mario_collisions() {
        for (std::size_t i = 0; i < mobs_.size();) {
            if (!mario_.collides_with(mobs_[i])) {
                ++i;
                continue;
            }

            if (mobs_[i].is_enemy()) {
                if (is_stomp_kill(mobs_[i])) {
                    score_ += POINTS_FOR_ENEMY;
                    mobs_.erase(mobs_.begin() + i);
                    continue;
                }
                handle_player_death();
                return;
            }

            if (mobs_[i].is_coin()) {
                score_ += POINTS_FOR_COIN;
                mobs_.erase(mobs_.begin() + i);
                continue;
            }
            ++i;
        }
    }

    bool is_stomp_kill(const Object& enemy) const {
        return mario_.is_flying() && (mario_.vy() > 0) &&
               (mario_.y() + mario_.height() < enemy.y() + enemy.height() * 0.5f);
    }

    void handle_player_death() {
        Console::set_color(0x4F);
        render_frame();
        Console::sleep_ms(500);
        Console::set_color(DEFAULT_COLOR);
        load_level(level_);
    }

    void scroll_map(float dx) {
        Object probe = mario_;
        probe.set_position(mario_.x() - dx, mario_.y());
        for (const auto& brick : bricks_) {
            if (probe.collides_with(brick))
                return;
        }

        for (auto& brick : bricks_)
            brick.set_position(brick.x() + dx, brick.y());
        for (auto& mob : mobs_)
            mob.set_position(mob.x() + dx, mob.y());
    }

private:
    char screen_buffer_[MAP_HEIGHT][MAP_WIDTH + 1]{};
    Object mario_;
    std::vector<Object> bricks_;
    std::vector<Object> mobs_;
    int level_{1};
    int score_{0};
    int max_level_{0};
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    Game game;
    game.run();
    return 0;
}

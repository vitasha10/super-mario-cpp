НЕОБХОДИМО СОЗДАТЬ НЕСКОЛЬКО КОММИТОВ, КАЖДЫЙ ИЗ КОТОРЫХ БУДЕТ СОДЕРЖАТЬ ОПРЕДЕЛЁННЫЕ ИЗМЕНЕНИЯ.
САМОЕ ГЛАВНОЕ ЧТОБЫ В КАЖДОМ КОММИТЕ ПРОГРАММА РАБОТАЛА И В КОНЦЕ ТОЖЕ

1. Чистописание и нулевой риск
Что сделать:
Привести отступы, скобки, пробелы, пустые строки к одному стилю.
Убрать хвостовые пробелы.
Не менять имена/сигнатуры/типизацию/логику.
Критерий приемки: проект собирается, поведение не изменилось.
Commit message: style: единый формат кода (отступы, скобки, пробелы), без изменения логики
2. Структура однофайловой программы и include’ы
Что сделать:
Отсортировать #include по алфавиту; заменить C-заголовки на C++ аналоги: stdio.h→cstdio, stdlib.h→cstdlib, math.h→cmath, string.h→cstring; оставить windows.h.
Сгруппировать блоки: include → константы/типы → глобальные данные → прототипы → функции → main.
Убедиться, что нет using namespace.
Критерий приемки: сборка успешна на Windows (g++).
Commit message: refactor: упорядочены include’ы и структура файла (C++ заголовки)
3. Вынесение “магических” значений в константы
Что сделать:
Ввести constexpr/const для: ширины/высоты карты, гравитации (0.05f), стартовой скорости по X для мобов (0.2f), импульса прыжка (-1.0f), задержки кадра (10 мс), очков за врага/монету (50/100), символов объектов ('#','?','-','+','o','$','@').
Заменить литералы на константы по всему файлу.
Критерий приемки: компилируется, поведение не изменилось.
Commit message: refactor: вынес магические числа и символы в constexpr константы
4. Исправления по языку C++ и безопасность интерфейсов
Что сделать:
Заменить BOOL/TRUE/FALSE на bool/true/false; функции, возвращающие BOOL, перевести на bool.
Добавить недостающие include (cstring для strlen).
В параметрах, которые только читаются, проставить const (без агрессивных правок).
Критерий приемки: все функции корректны по C++, сборка чистая.
Commit message: refactor: переход на bool и базовая const-correctness для параметров
5. Исправление реальных дефектов и UB-мест
Что сделать:
Исправить ошибку в show_map: map[mapHeight-1][mapWidth-1] = '\0' заменить на корректное обнуление конца строки (индекс mapWidth), чтобы не терять последний символ последней строки.
Проверить корректность формирования строк в clear_map и копирования в остальные строки.
Критерий приемки: визуально карта не теряет последний столбец, сборка успешна.
Commit message: fix: исправлен неверный терминатор строки в последней строке карты 
6. Единый стиль разыменования указателей
Что сделать:
Заменить (*obj).field и obj[0].field на obj->field во всех функциях, где это уместно.
Не менять логику.
Критерий приемки: поведение то же, код читаемее.
Commit message: refactor: унификация стиля разыменования указателей (-> вместо (*obj) и obj[0])
7. Именование функций — глаголы в snake_case (исправление моей ошибки с player_dead)
Что сделать:
Переименовать функции в глагольную форму:
ClearMap→clear_map
ShowMap→render_map
SetObjectPos→set_object_pos
InitObject→init_object
PlayerDead→handle_player_death ← исправляет твоё замечание («не глагол»)
GetNewBrick→add_brick
GetNewMoving→add_mob
PutScoreOnMap→draw_score_on_map
CreateLevel→load_level
VertMoveObject→move_object_vertical
HorizonMoveObject→move_object_horizontal
MarioCollision→handle_mario_collisions
DeleteMoving→remove_mob_by_index
PutObjectOnMap→place_object_on_map
HorizonMoveMap→scroll_map_horizontal
SetCur→set_cursor_pos
IsPosInMap→is_within_map (или check_pos_in_map; выбран is_* как общепринято для предикатов)
IsCollision→check_collision (глагол)
Критерий приемки: проект компилируется, имена консистентны, player_dead устранён.
Commit message: refactor(naming): функции переименованы в глагольной форме (snake_case)
8. Именование переменных и полей структур
Что сделать:
Переименовать поля TObject: x,y,width,height,vertSpeed→vy,horizSpeed→vx, IsFly→is_flying, cType→glyph.
Глобальные: map→screen_buffer (или map_buffer), brick→bricks, brickLength→bricks_count, moving→mobs, movingLength→mobs_count, level/score/maxLvl→max_level.
Локальные параметры привести к осмысленным и/или коротким, но понятным.
Критерий приемки: проект компилируется, стиль имён единый, смыслы не потеряны.
Commit message: refactor(naming): осмысленные имена переменных/полей (snake_case)
9. Разделение ответственности: ввод, обновление, рендер
Что сделать:
Вынести из main три функции: handle_input(), update_world(), render_frame().
Разнести туда логику обработки клавиш, физики/коллизий, отрисовки буфера.
Критерий приемки: главный цикл короткий и читаемый; поведение неизменное.
Commit message: refactor: разделение на handle_input / update_world / render_frame
10. Ускорение вывода: буфер и std::cout
Что сделать:
Переписать render_map/print в единичный вывод кадра из заранее собранной строки/буфера через std::cout; отключить sync_with_stdio и tie для скорости.
draw_score_on_map — без snprintf, использовать std::string/to_string.
Критерий приемки: FPS стабильнее/выше; визуально без изменений.
Commit message: perf: буферизованный вывод кадра через std::cout, отказ от printf/snprintf
11. Обёртки над WinAPI и не забыть system("color")
Что сделать:
Ввести wrappers: sleep_ms(unsigned), set_console_color(unsigned short), set_cursor_pos(int,int).
Заменить Sleep/system("color")/SetConsoleCursorPosition прямые вызовы на обёртки.
Никаких внешних библиотек (ncurses/PDCurses) не добавлять; остаёмся на Windows.
Критерий приемки: функциональность прежняя, без вызова system().
Commit message: refactor: обёртки над WinAPI (сон, цвет, курсор), переделан system("color")
12. Упрощение условий и устранение дублирования
Что сделать:
Вынести предикаты: is_stomp_kill(const TObject&, const TObject&), is_coin_pickup(const TObject&, const TObject&).
Упростить сложные if-выражения в handle_mario_collisions через ранние return/continue.
Устранить повтор начисления очков/удаления моба в одну вспомогательную функцию.
Критерий приемки: читаемость выше, поведение прежнее.
Commit message: refactor: упростил условия коллизий и вынес повторяющийся код
13. Чистка комментариев, порядок определений, .gitignore и скрипты сборки
Что сделать:
Удалить шумные/устаревшие комментарии (заметки про «видео», C-шные замечания про приведение malloc).
Кратко прокомментировать неочевидные места (например, логика перехода уровня).
Привести порядок объявлений/реализаций (логические группы).
Добавить .gitignore (артефакты сборки: *.exe, *.obj, *.o, *.pdb и т.п.).
Добавить два простых скрипта сборки под g++:
Windows Bash/MinGW: build.sh с явным -std=c++17 -O2 -Wall -Wextra -Wpedantic.
batch (build.bat) — опционально, если нужно.
В скриптах показать подключение заголовков и флагов компилятора; внешние библиотеки не требуются.
Критерий приемки: репозиторий чище, сборка прозрачнее.
Commit message: chore: чистка комментариев, упорядочивание кода, .gitignore и скрипты сборки
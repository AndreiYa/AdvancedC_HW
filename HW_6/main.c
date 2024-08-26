#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#define MIN_Y  2
double DELAY = 0.1;
#define PLAYERS 2

enum {LEFT=1, UP, RIGHT, DOWN, STOP_GAME='q'};
enum {MAX_TAIL_SIZE=100, START_TAIL_SIZE=3, MAX_FOOD_SIZE=20, FOOD_EXPIRE_SECONDS=10, SEED_NUMBER=3, CONTROLS=2};


// Здесь храним коды управления змейкой
struct control_buttons
{
    int down;
    int up;
    int left;
    int right;
}control_buttons;

struct control_buttons default_controls[CONTROLS] = {
        {KEY_DOWN, KEY_UP, KEY_LEFT, KEY_RIGHT},
        {'s', 'w', 'a', 'd'}
};

/*
 Голова змейки содержит в себе
 x,y - координаты текущей позиции
 direction - направление движения
 tsize - размер хвоста
 *tail -  ссылка на хвост
 */
typedef struct snake_t
{
    int x;
    int y;
    int direction;
    size_t tsize;
    struct tail_t *tail;
    struct control_buttons controls;
    int enableAI;
    int color_pair;
} snake_t;

/*
 Хвост это массив состоящий из координат x,y
 */
typedef struct tail_t
{
    int x;
    int y;
} tail_t;

struct food
{
    int x;
    int y;
    time_t put_time;
    char point;
    uint8_t enable;
} food[MAX_FOOD_SIZE];

void initFood(struct food f[], size_t size)
{
    struct food init = {0,0,0,0,0};
    for(size_t i=0; i<size; i++)
    {
        f[i] = init;
    }
}

void putFoodSeed(struct food *fp)
{
    int max_x=0, max_y=0;
    char spoint[2]={0};
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(fp->y, fp->x, " ");
    fp->x = rand() % (max_x - 1);
    fp->y = rand() % (max_y - 2) + 1;
    fp->put_time = time(NULL);
    fp->point = '$';
    fp->enable = 1;
    spoint[0] = fp->point;
    mvprintw(fp->y, fp->x, "%s", spoint);
}

void putFood(struct food f[], size_t number_seeds)
{
    for(size_t i = 0; i < number_seeds; i++)
    {
        putFoodSeed(&f[i]);
    }
}

void refreshFood(struct food f[], int nfood)
{
    for(size_t i=0; i<nfood; i++)
    {
        if(f[i].put_time)
        {
            if(!f[i].enable || (time(NULL) - f[i].put_time) > FOOD_EXPIRE_SECONDS)
            {
                putFoodSeed(&f[i]);
            }
        }
    }
}

void initTail(struct tail_t t[], size_t size)
{
    struct tail_t init_t={0,0};
    for(size_t i=0; i<size; i++)
    {
        t[i]=init_t;
    }
}
void initHead(struct snake_t *head, int x, int y)
{
    head->x = x;
    head->y = y;
    head->direction = RIGHT;
}

void initSnake(snake_t *head[], size_t size, int x, int y, int i, int enableAI)
{
    head[i] = (snake_t*) malloc(sizeof(snake_t));
    tail_t* tail = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head[i], x, y);
    head[i]->tail = tail; // прикрепляем к голове хвост
    head[i]->tsize = size+1;
    head[i]->controls = default_controls[i];
    head[i]->enableAI = enableAI;
    head[i]->color_pair = i+1;
}

/*
 Движение головы с учетом текущего направления движения
 */
void go(struct snake_t *head)
{
    char ch = '@';
    int max_x=0, max_y=0;
    getmaxyx(stdscr, max_y, max_x); // macro - размер терминала
    mvprintw(head->y, head->x, " "); // очищаем один символ
    attron(COLOR_PAIR(head->color_pair));
    switch (head->direction)
    {
        case LEFT:
            if (head->x <= 0) // Циклическое движение, чтобы не уходить за пределы экрана
                head->x = max_x;
            mvprintw(head->y, --(head->x), "%c", ch);
            break;
        case RIGHT:
            if (head->x >= max_x)
                head->x = 0;
            mvprintw(head->y, ++(head->x), "%c", ch);
            break;
        case UP:
            if (head->y <= 1)
                head->y = max_y;
            mvprintw(--(head->y), head->x, "%c", ch);
            break;
        case DOWN:
            if (head->y >= max_y)
                head->y = 0;
            mvprintw(++(head->y), head->x, "%c", ch);
            break;
        default:
            break;
    }
    attroff(COLOR_PAIR(head->color_pair));
    refresh();
}

int checkDirection(struct snake_t* snake, int32_t key) {
        if ((key == snake->controls.down) && (snake->direction != UP))
        {
            return 1;
        } else if ((key == snake->controls.up) && (snake->direction != DOWN))
        {
            return 1;
        } else if ((key == snake->controls.right) && (snake->direction != LEFT))
        {
            return 1;
        } else if ((key == snake->controls.left) && (snake->direction != RIGHT))
        {
            return 1;
        }
    return 0;
}

void changeDirection(struct snake_t* snake, const int32_t key)
{
    if (checkDirection(snake, key)) {
        if (key == snake->controls.down)
        {
            snake->direction = DOWN;
        } else if (key == snake->controls.up)
        {
            snake->direction = UP;
        } else if (key == snake->controls.right)
        {
            snake->direction = RIGHT;
        } else if (key == snake->controls.left)
        {
            snake->direction = LEFT;
        }
    }
}

/*
 Движение хвоста с учетом движения головы
 */
void goTail(struct snake_t *head)
{
    char ch = '*';
    mvprintw(head->tail[head->tsize-1].y, head->tail[head->tsize-1].x, " ");
    for(size_t i = head->tsize-1; i>0; i--)
    {
        head->tail[i] = head->tail[i-1];
        if( head->tail[i].y || head->tail[i].x)
            attron(COLOR_PAIR(head->color_pair));
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
            attroff(COLOR_PAIR(head->color_pair));
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

_Bool haveEat(struct snake_t *head, struct food f[])
{
    for(int i=0; i<MAX_FOOD_SIZE-1; i++)
    {
        if ((head->x == f[i].x) && (head->y == f[i].y)) {
            f[i].enable = 0;
            return 1;
        }
    }
    return 0;
}

void addTail(struct snake_t *head)
{
    head->tsize++;
}

int isCrush(snake_t *snake)
{
    for(size_t i = snake->tsize-1; i>0; i--)
    {
        if((snake->tail[i].x == snake->x)&&(snake->tail[i].y == snake->y))
        {
            return 1;
        }
    }
    return 0;
}

void repairSeed(struct food f[], size_t nfood, struct snake_t *head)
{
    for(size_t i=0; i<head->tsize; i++)
    {
        for(size_t j=0; j<nfood; j++)
        {
            if (((f[j].x == head->tail[i].x) && (f[j].y == head->tail[i].y)) && (f[j].enable == 1)) {
                putFoodSeed(&f[j]);
            }
        }
    }
    for(size_t i=0; i<nfood; i++)
    {
        for(size_t j=0; j<nfood; j++)
        {
            if ((f[i].x == f[j].x) && (f[i].y == f[j].y) && (i != j)) {
                putFoodSeed(&f[j]);
            }
        }
    }
}

int distance(const snake_t snake, const struct food food)
{
    return (abs(snake.x - food.x) + abs(snake.y - food.y));
}

void autoChangeDirection(snake_t *snake, struct food food[], int foodSize)
{
    int pointer = 0;
    for(int i=1; i<foodSize; i++)
    {
        pointer = (distance(*snake, food[i]) < distance(*snake, food[pointer])) ? 1 : pointer;
    }

    if((snake->direction == RIGHT || snake->direction == LEFT) && (snake->y != food[pointer].y))
    {
        snake->direction = (food[pointer].y > snake->y) ? DOWN : UP;
    } else if ((snake->direction == DOWN || snake->direction == UP) && (snake->x != food[pointer].x))
    {
        snake->direction = (food[pointer].x > snake->x) ? RIGHT : LEFT;
    }
}

void update(struct snake_t *head, struct food f[], const int32_t key)
{
    clock_t begin = clock();
    if(head->enableAI)
    {
        autoChangeDirection(head, f, SEED_NUMBER);
    } else if(checkDirection(head, key))
    {
        changeDirection(head, key);
    }

    go(head);
    goTail(head);
    refreshFood(food, SEED_NUMBER);
    if (haveEat(head, food))
    {
        addTail(head);
    }
    refresh();
    while ((double) (clock() - begin)/CLOCKS_PER_SEC<DELAY)
    {}
}

int gameMode() {
    int mode = 0;
    while (mode<'1' || mode>'3') {
        clear();

        mvprintw(3, 1, "Select Game Mode:");
        mvprintw(4, 1, "========================");
        mvprintw(5, 2, "1. One Player");
        mvprintw(6, 2, "2. Two Players");
        mvprintw(7, 2, "3. AI Mode");
        mvprintw(8, 1, "=======================");
        mvprintw(9, 1, "Please make your choose");

        refresh();
        mode = getch();
    }
    int modeToNum = mode - '0';
    return modeToNum;
}

int main()
{
    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_BLACK);
    }

    int mode = gameMode();

    clear();
    mvprintw(0, 0,"Use arrows for control. Press 'q' for EXIT");
    refresh();
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER);

    snake_t* snakes[PLAYERS]={NULL,NULL};

    switch(mode)
    {
        case 1:
        {
            initSnake(snakes, START_TAIL_SIZE, 10, 10, 0, 0);
            break;
        }
        case 2:
        {
            initSnake(snakes, START_TAIL_SIZE, 10, 10, 0, 0);
            initSnake(snakes, START_TAIL_SIZE, 30, 10, 1, 0);
            break;
        }
        case 3:
        {
            initSnake(snakes, START_TAIL_SIZE, 10, 10, 0, 0);
            initSnake(snakes, START_TAIL_SIZE, 30, 10, 1, 1);
            break;
        }
        default:
            break;
    }

    int key_pressed=0;
    while( key_pressed != STOP_GAME )
    {
        key_pressed = getch(); // Считываем клавишу
        for(int i=0; i<PLAYERS; i++)
        {
            if(snakes[i])
            {
                update(snakes[i], food, key_pressed);
                if(isCrush(snakes[i]))
                {
                    return 0;
                }
                repairSeed(food, SEED_NUMBER, snakes[i]);
            }

        }
    }
    for(int i=0; i<PLAYERS; i++)
    {
        free(snakes[i]->tail);
        free(snakes[i]);
    }
    endwin(); // Завершаем режим curses mod
    return 0;
}

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

void initSnake(snake_t *head[], size_t size, int x, int y, int i)
{
    head[i] = (snake_t*) malloc(sizeof(snake_t));
    tail_t* tail = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
    initTail(tail, MAX_TAIL_SIZE);
    initHead(head[i], x, y);
    head[i]->tail = tail; // прикрепляем к голове хвост
    head[i]->tsize = size+1;
    head[i]->controls = default_controls[i];

//    tail_t*  tail  = (tail_t*) malloc(MAX_TAIL_SIZE*sizeof(tail_t));
//    initTail(tail, MAX_TAIL_SIZE);
//    initHead(head, x, y);
//    head->tail = tail; // прикрепляем к голове хвост
//    head->tsize = size+1;
//    head->controls = default_controls;
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
            mvprintw(head->tail[i].y, head->tail[i].x, "%c", ch);
    }
    head->tail[0].x = head->x;
    head->tail[0].y = head->y;
}

_Bool haveEat(struct snake_t *head, struct food f[])
{

}

void addTail(struct snake_t *head)
{

}

int isCrush(snake_t *head)
{
    return 0;
}

void update(struct snake_t *head, struct food f[], const int32_t key)
{
    clock_t begin = clock();
    go(head);
    goTail(head);
    if(checkDirection(head, key))
    {
        changeDirection(head, key);
    }
    refreshFood(food, SEED_NUMBER);
//    if (haveEat(head, food))
//    {
//        addTail(head);
//    }
//    if (isCrush(head))
//    {
//        return;
//    }
    refresh();
    while ((double) (clock() - begin)/CLOCKS_PER_SEC<DELAY)
    {}
}

void repairSeed(struct food f[], size_t nfood, struct snake_t *head)
{
    for(size_t i=0; i<head->tsize; i++)
    {
        for(size_t j=0; j<nfood; j++)
        {

        }
    }
    for(size_t i=0; i<nfood; i++)
    {
        for(size_t j=0; j<nfood; j++)
        {

        }
    }
}

int main()
{
    snake_t* snakes[PLAYERS];
    for(int i=0; i<PLAYERS; i++)
    {
        initSnake(snakes,START_TAIL_SIZE,10+i*10,10+i*10,i);
    }
//    snake_t* snake = (snake_t*)malloc(sizeof(snake_t));
//    initSnake(snake,START_TAIL_SIZE,10,10);
    initscr();
    keypad(stdscr, TRUE); // Включаем F1, F2, стрелки и т.д.
    raw();                // Откдючаем line buffering
    noecho();            // Отключаем echo() режим при вызове getch
    curs_set(FALSE);    //Отключаем курсор
    mvprintw(0, 0,"Use arrows for control. Press 'q' for EXIT");
    timeout(0);    //Отключаем таймаут после нажатия клавиши в цикле
    initFood(food, MAX_FOOD_SIZE);
    putFood(food, SEED_NUMBER);
    int key_pressed=0;
    while( key_pressed != STOP_GAME )
    {
        key_pressed = getch(); // Считываем клавишу
        for(int i =0; i<PLAYERS; i++)
        {
            update(snakes[i], food, key_pressed);
            if(isCrush(snakes[i]))
            {
                break;
            }
            repairSeed(food, SEED_NUMBER, snakes[i]);
        }
//        if(key_pressed == PAUSE_GAME)
//        {
//            pause();
//        }

//        update(snake, food, key_pressed);
//        go(snake);
//        goTail(snake);
//        timeout(100); // Задержка при отрисовке
//        refreshFood(food, SEED_NUMBER);
//        if (haveEat(snake, food))
//        {
//            addTail(snake);
//        }
//        if (isCrush(snake))
//        {
//            break;
//        }
//        changeDirection(snake, key_pressed);
    }
    for(int i=0; i<PLAYERS; i++)
    {
//        printExit(snakes[i]);
        free(snakes[i]->tail);
        free(snakes[i]);
    }
    endwin(); // Завершаем режим curses mod
    return 0;
}
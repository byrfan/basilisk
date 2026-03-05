#ifndef SNAKE_HEADER
#define SNAKE_HEADER
#include "util.h"

#define MAX_SNAKE_LEN 128
#define HEAD '@'
#define BODY 'O'
#define DEFAULT_SNAKE_X {40, 41, 42}
#define DEFAULT_SNAKE_Y {12, 12, 12}
#define DEFAULT_SNAKE_LEN 3
#define TICK_SPEED 115 

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} dir_t;

typedef struct {
    size_t x[MAX_SNAKE_LEN];
    size_t y[MAX_SNAKE_LEN];
    dir_t direction; 
    size_t length;
    bool_t alive;

} Snake;

typedef struct {
    size_t x;
    size_t y;
    bool_t alive;
} Fruit;

extern volatile dir_t current_direction;

void draw_border();
void draw_score(size_t score);
void change_direction(Snake* s, dir_t new_dir);
void move_snake_tick(Snake *s);
void eat_fruit(Snake *s, Fruit* f);
void spawn_fruit(Fruit* f);
void persist_fruit(Fruit* f);
void draw_fruit(Fruit* f);
void check_death(Snake* s);
void draw_snake(Snake* s);

#endif

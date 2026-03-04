#include "util.h"
#include "snake.h"

void init_game() {
    draw_border();
    draw_score(0);
    draw_string(78, 3, "*", 0x0F);
}

void run_game() {
    kernel_init();
    init_game();   
    unsigned long last_tick = 0;

    Snake snake = {
        .length = DEFAULT_SNAKE_LEN,
        .direction = DIR_LEFT,
        .x = DEFAULT_SNAKE_X, // implement generate x
        .y = DEFAULT_SNAKE_Y, // implement generate y
        .alive = true
    };

    init_entropy();
    Fruit fruit;
    spawn_fruit(&fruit);

    while(snake.alive) {
        unsigned long now = get_ticks();
        if (now - last_tick >= TICK_SPEED) {
            last_tick = now;

            draw_fruit(&fruit);
            change_direction(&snake, current_direction);
            move_snake_tick(&snake);
            eat_fruit(&snake, &fruit);
            persist_fruit(&fruit);

            draw_score(snake.length - DEFAULT_SNAKE_LEN);
        }
    }
}

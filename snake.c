#include "snake.h"
#include "util.h"

volatile dir_t current_direction = DIR_LEFT;

void draw_border() {
    // Draw top border (row 4)
    for (unsigned short x = 0; x < 80; x++) {
        draw_char(x, 4, '#', 0x07);
        draw_char(x, 23, '#', 0x07);
    }
    
        // Draw left border (rows 5-22)
    for (unsigned short y = 5; y < 23; y++) {
        draw_char(0, y, '#', 0x07);
        draw_char(79, y, '#', 0x07);  // Right border too
    }
}

void draw_score(size_t score) {
    char* label = "Score: ";

    //          x  y
    draw_string(0, 3, label, 0x0F);  // White text
    
    char _score[16]; 
    itoa(score, _score);

    draw_string(strlen(label), 3, _score, 0x0F);
}

int is_opposite(dir_t a, dir_t b) {
    if (a == DIR_UP && b == DIR_DOWN) return 1;
    if (a == DIR_DOWN && b == DIR_UP) return 1;
    if (a == DIR_LEFT && b == DIR_RIGHT) return 1;
    if (a == DIR_RIGHT && b == DIR_LEFT) return 1;
    return 0; 
}

void change_direction(Snake* s, dir_t new_dir) {
    if (!is_opposite(s->direction, new_dir)) {
        s->direction = new_dir;
    }
}


void tick_snake(Snake* s) {
    for (size_t i = s->length - 1; i > 0; i--) {
        s->x[i] = s->x[i - 1];
        s->y[i] = s->y[i - 1];
    }
}

void update_snake(Snake *s) {
    tick_snake(s);

    switch (s->direction) {
        case DIR_UP:    s->y[0]--; break;
        case DIR_DOWN:  s->y[0]++; break;
        case DIR_LEFT:  s->x[0]--; break;
        case DIR_RIGHT: s->x[0]++; break;
    }
}

void self_collision(Snake* s) {
    for(size_t i=0; i<s->length; i++) {
        if ((s->x[0] == s->x[i]) && (s->y[0] == s->y[i]))     
            s->alive=false;
    } 
}

void border_collision(Snake* s) {
    size_t head_x = s->x[0];        
    size_t head_y = s->y[0];

    if((head_x == 79 || head_x == 0) || 
        (head_y == 4 || head_y == 23))
        s->alive = false;
}

void check_death(Snake* s) {
    self_collision(s);
    border_collision(s);
}

void move_snake_tick(Snake *s) {
    size_t old_tail_x = s->x[s->length - 1];
    size_t old_tail_y = s->y[s->length - 1];
   
    size_t old_head_x = s->x[0];
    size_t old_head_y = s->y[0];

    update_snake(s);
    /*
     * Implement death logic here
     *      - Crashing into wall
     *      - Craashing into itself
     */
    // erase old tail
    draw_char(old_tail_x, old_tail_y, ' ', 0x0F);
    // replace hold head
    draw_char(old_head_x, old_head_y, BODY, 0x0F);
    // draw new head
    draw_char(s->x[0], s->y[0], HEAD, 0x0F);
}

void eat_fruit(Snake *s, Fruit* f) {
    if ((s->x[0] == f->x) && (s->y[0] == f->y)) {
        s->length++; 
        f->alive = false;
    }
}

void spawn_fruit(Fruit* f) {
    size_t new_x = rand_between(1, 78); 
    size_t new_y = rand_between(5, 22);

    f->x = new_x;
    f->y = new_y;
    f->alive = true;
}

void persist_fruit(Fruit* f) {
    if (!f->alive) {
        spawn_fruit(f);
    } 
}

void draw_fruit(Fruit* f) { 
    draw_char(f->x, f->y, '*', 0x0C); 
}

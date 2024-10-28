#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ncurses.h>

#define FPS 7
#define KEYUP 65
#define KEYDOWN 66
#define KEYRIGHT 67
#define KEYLEFT 68
#define CONTROL_KEY 27

int WIDTH, HEIGHT, SCORE;

enum Rotation {R_UP,R_DOWN,R_RIGHT, R_LEFT};

enum block_type{
    EMPTY,
    TALE,
    APPLE,
};

struct SnakeBlock{
    int x;
    int y;
    enum block_type BlockType;
};


void draw_map();
int move_tale(struct SnakeBlock* snake_tale[], struct SnakeBlock map[HEIGHT][WIDTH], int* snake_length, enum Rotation rotation);
void create_apple(struct SnakeBlock map[HEIGHT][WIDTH]);

int main(int argc, char *argv[]){
    
    if (argc <= 1){
        printf("CHOOSE GAME MOD\n");
        printf("big - big map\n");
        printf("mid - middle map \n");
        printf("sm - small map\n");
        return 0;
    }

    if (strcmp(argv[1], "big") == 0) {
        WIDTH = HEIGHT = 25;
    } else if (strcmp(argv[1], "mid") == 0) {
        WIDTH = HEIGHT = 17;
    } else {
        WIDTH = HEIGHT = 10;
    }
    

    initscr();
    nodelay(stdscr, true);
    noecho();
    cbreak();
    srand(time(NULL));
    curs_set(false);

    struct SnakeBlock map[HEIGHT][WIDTH]; // create board (I named this MAP)

    for (int y = 0; y <= HEIGHT - 1; y++){
        for (int x = 0; x <= WIDTH - 1; x++){
            map[y][x] = (struct SnakeBlock){x, y, EMPTY};
        }
    }

    int first_key, input_key, result;
    int upd_time = 100 / FPS;
    float start_time = time(NULL);
    int now, last_time = 0;
    int ms = 10000;
    enum Rotation rotation = R_DOWN;
    struct SnakeBlock * snake_array[HEIGHT*WIDTH];
    map[HEIGHT / 2][WIDTH / 2].BlockType = TALE; // <--- create a hat
    snake_array[0] = &map[HEIGHT / 2][WIDTH / 2];

    int snake_length = 1;
    int last_input;
    draw_map();
    mvprintw(HEIGHT+2, 3, "SCORE: %d", SCORE);
    create_apple(map);
    while (true){
        if ((now = clock())/ms - last_time/ms >= upd_time){ // <--- limit fps
            last_time = now;

            first_key = getch();
            if (first_key == CONTROL_KEY){
                getch();
                input_key = getch();
                flushinp();
            }
            if (first_key == CONTROL_KEY && input_key == ERR)
                break;

            switch (input_key) {
                case KEYUP: if (rotation != R_DOWN)rotation = R_UP;
                break;
                case KEYDOWN: if (rotation != R_UP)rotation = R_DOWN;
                break;
                case KEYRIGHT: if (rotation != R_LEFT)rotation = R_RIGHT;
                break;
                case KEYLEFT: if (rotation != R_RIGHT)rotation = R_LEFT;
                break;
            }
            input_key = first_key = ERR;

            result = move_tale(snake_array, map, &snake_length, rotation);
            refresh();
            if (!result){
                endwin();
                printf("    ______   ___       ____   __       ______   ____               \n");
                printf("   / ____/  /   |     /  _/  / /      / ____/  / __ \\             \n");
                printf("  / /_     / /| |     / /   / /      / __/    / / / /              \n");
                printf(" / __/    / ___ |   _/ /   / /___   / /___   / /_/ /  _    _    _  \n");
                printf("/_/      /_/  |_|  /___/  /_____/  /_____/  /_____/  (_)  (_)  (_) \n");
                break;
            }
        }
    }
    
    endwin();
    return 0;
}

void draw_map(){
    mvprintw(0, 0, "+");
    mvprintw(HEIGHT + 1, WIDTH * 2, "+");
    mvprintw(0, WIDTH * 2, "+");
    mvprintw(HEIGHT + 1, 0, "+");
    for (int y = 1; y < HEIGHT + 1; y++){
        mvprintw(y, 0, "|");
        mvprintw(y, WIDTH*2, "|");
    }
    for (int x = 1; x < WIDTH * 2; x++){
        mvprintw(0, x, "-");
        mvprintw(HEIGHT + 1, x, "-");
    }
}

int move_tale(struct SnakeBlock* snake_tale[], struct SnakeBlock map[HEIGHT][WIDTH], int* snake_length, enum Rotation rotation){
    int i;
    int length = *snake_length;
    int hat_x = snake_tale[0]->x; int hat_y = snake_tale[0]->y;
    struct SnakeBlock* last_tale_elem = snake_tale[length - 1];

    snake_tale[length - 1]->BlockType = EMPTY;
    mvprintw(snake_tale[length - 1]->y+1, snake_tale[length - 1]->x*2+1, " ");

    for (i = length - 1; i > 0; i--){
        snake_tale[i] = &map[snake_tale[i - 1]->y][snake_tale[i - 1]->x];
        snake_tale[i]->BlockType = TALE;

        mvprintw(snake_tale[i]->y + 1, snake_tale[i]->x*2 + 1, "#");
    }
    switch (rotation){
        case R_DOWN:
        if (hat_y >= HEIGHT - 1) hat_y = 0;
        else (hat_y)++;
        break;
        case R_UP:
        if (hat_y <= 0) hat_y = HEIGHT - 1;
        else (hat_y)--;
        break;
        case R_RIGHT:
        if (hat_x >= WIDTH - 1) hat_x = 0;
        else (hat_x)++;
        break;
        case R_LEFT:
        if (hat_x <= 0) hat_x = WIDTH - 1;
        else (hat_x)--;
        break;
    }
    mvprintw(hat_y + 1, hat_x*2 + 1, "@");
    if (map[hat_y][hat_x].BlockType == TALE) // <--- check tale collision
        return 0;
    if (map[hat_y][hat_x].BlockType == APPLE){  // <--- check apple collision
        last_tale_elem->BlockType = TALE;
        snake_tale[length] = last_tale_elem;
        (*snake_length)++;
        mvprintw(last_tale_elem->y+1, last_tale_elem->x*2+1, "#");
        create_apple(map);
        mvprintw(HEIGHT + 2, 3, "SCORE: %d", ++SCORE);
    }
    snake_tale[0] = &map[hat_y][hat_x];
    snake_tale[0]->BlockType = TALE;
    return 1;
}

void create_apple(struct SnakeBlock map[HEIGHT][WIDTH]){
    struct SnakeBlock *choose_list[HEIGHT * WIDTH];
    int count = 0;
    for (int y = 0; y <= HEIGHT - 1; y++){
        for (int x = 0; x <= WIDTH - 1; x++){
            if (map[y][x].BlockType == EMPTY){
                choose_list[count] = &map[y][x];
                count++;
            }
        }
    }
    int random_block = rand() % count;
    choose_list[random_block]->BlockType = APPLE;
    mvprintw(choose_list[random_block]->y + 1, choose_list[random_block]->x * 2 + 1, "*");
}

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>

#include "main.h"

int_coord cursor_pos = (int_coord) {0, 0};
int map_height, map_width, starty, startx;
int input; 

char printch;

bool looping = true;

int margin = 2;


unsigned short **init_save_buffer(int_coord map_coords) {
    unsigned short** arr;
    arr = (unsigned short**)malloc(map_coords.x * sizeof(short*)); 
    for (int i = 0; i < map_coords.x; i++)
        arr[i] = (unsigned short*)malloc(map_coords.y * sizeof(short));

    for (int i = 0; i < map_coords.x; i++) {
        for (int j = 0; j < map_coords.y; j++) {
            arr[i][j] = -1; 
        }
    }
    return arr;
}

void deinit_save_buffer(unsigned short **arr, int_coord map_coords) {
    for (int i = 0; i < map_coords.x; i++) {
        free(arr[i]); 
    }
    free(arr); 
}

void write_save_buf_to_file(unsigned short **save_buf, int_coord map_dim, FILE *fileptr) {



}



int main(int argc, char *argv[]) {
    if (argc == 1) {
        map_height = map_width = 10;
    } else if (argc == 2) {
        printf("Please enter coordinates correctly, or leave blank for default size.\n");
    } else {
        map_width = atoi(argv[1]);
        map_height = atoi(argv[2]); 
        

        for (int i = 3; i < argc; i++) {

            // switch statement for flags
            if (argv[i][0] == '-'){
                switch (argv[i][1]) {
                    case 'm': printf("KYS\n"); break;
                }
            }
        }
        printf("map width: %d, map height = %d\n", map_width, map_height);
    }
    int_coord map_dim = (int_coord){map_width, map_height};


    // it's ncursing time woohoo
    initscr();
    cbreak();
    noecho();

    starty = (LINES - map_dim.y) / 2;
	startx = (COLS - map_dim.x) / 2;
    PANEL *panels[3];
    WINDOW *windows[3];

    windows[0] = newwin(map_dim.y+10, map_dim.x+20, starty-5, startx-10); 
    windows[1] = newwin(map_dim.y+margin*2, map_dim.x+margin*2, starty-margin, startx-margin);
    windows[2] = newwin(map_dim.y, map_dim.x, starty, startx);

    box(windows[0], 0, 0);
    box(windows[1], 0, 0);

    mvwprintw(windows[0], 0, 2, " map editor ");
    mvwprintw(windows[1], 0, 2, " map ");

    panels[0] = new_panel(windows[0]);
    panels[1] = new_panel(windows[1]);
    panels[2] = new_panel(windows[2]);

    update_panels();
    doupdate(); 


    unsigned short **save_buf = init_save_buffer(map_dim);

    while (looping) {
        switch (input = getch()){
            case 'q': looping = false; break;

            case KEY_UP: case 'k': cursor_pos.y--; break;
            case KEY_DOWN: case 'j': cursor_pos.y++; break;
            case KEY_LEFT: case 'h': cursor_pos.x--; break;
            case KEY_RIGHT: case 'l': cursor_pos.x++; break;
            
            case '0': case '1': case '2': case '3': case '4': case '5':
                printch = input;
                mvwaddch(windows[2], cursor_pos.y, cursor_pos.x, printch);
                if (cursor_pos.x == map_dim.x && cursor_pos.y != map_dim.y) {
                    cursor_pos.x=0;
                    cursor_pos.y++;
                } else {
                    cursor_pos.x++;
                }
                break;
            
        }
        update_panels();
        doupdate();
    }


    endwin();
    deinit_save_buffer(save_buf, (int_coord){map_dim.y, map_width});
    printf("Save buf deinitialised\n");
    return 0;
}

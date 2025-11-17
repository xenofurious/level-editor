#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>

#include "main.h"

int_coord cursor_pos = (int_coord) {0, 0};
int map_height, map_width, starty, startx;
int input = 0;
char printch;

bool looping = true;

int small_margin = 2;
int large_margin = 4;

// functions
void save(unsigned short **savebuf, int_coord map_dim, char *filename);
void create_new_savefile(unsigned short **savebuf, int_coord map_dim, char *filename);
void update_file(unsigned short **save_buf, int_coord map_dim, char *filename);



unsigned short **init_save_buffer(int_coord map_coords) {
    unsigned short** arr;
    arr = (unsigned short**)malloc(map_coords.x * sizeof(short*)); 
    for (int i = 0; i < map_coords.x; i++)
        arr[i] = (unsigned short*)malloc(map_coords.y * sizeof(short));

    for (int i = 0; i < map_coords.x; i++) {
        for (int j = 0; j < map_coords.y; j++) {
            arr[i][j] = 0;
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

void save(unsigned short **save_buf, int_coord map_dim, char *filename) {
    FILE *fp = fopen(filename, "r+");
    if (fp == NULL) {
        create_new_savefile(save_buf, map_dim, filename);
    } else {
        update_file(save_buf, map_dim, filename);
    }
}

void create_new_savefile(unsigned short **save_buf, int_coord map_dim, char *filename) {
    FILE *fp = fopen(filename, "w");
    fprintf(fp, "%d %d\n", map_dim.x, map_dim.y);
    
    for (int i = 0; i < map_dim.x; i++) {
        for (int j = 0; j < map_dim.y; j++) {
            fprintf(fp, "%hu ", save_buf[i][j]);
        }
        fprintf(fp, "\n");
    }
}

void update_file(unsigned short **save_buf, int_coord map_dim, char *filename) {
    FILE *fp = fopen(filename, "r+");
}

// MAIN //

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
    start_color();
    define_colours();

    starty = (LINES - map_dim.y) / 2;
	startx = (COLS - map_dim.x) / 2;
    PANEL *panels[4];
    WINDOW *windows[4];

    int window_xmargin = 40;
    int window_ymargin = 30;

    windows[0] = newwin(map_dim.y+window_ymargin, map_dim.x+window_xmargin, starty-window_ymargin/2, startx-window_xmargin/2); 
    windows[1] = newwin(map_dim.y+small_margin*2, map_dim.x+small_margin*2, starty-small_margin, startx-small_margin);
    windows[3] = newwin(map_dim.y, map_dim.x, starty, startx);
    windows[2] = newwin(window_ymargin/2 - large_margin*2 + 1, map_dim.x + window_xmargin-large_margin*2, starty+map_dim.y+large_margin, startx-window_xmargin/2 + large_margin);

    box(windows[0], 0, 0);
    box(windows[1], 0, 0);
    box(windows[2], 0, 0);

    mvwprintw(windows[0], 0, 2, " map editor ");
    mvwprintw(windows[1], 0, 2, " map ");

    panels[0] = new_panel(windows[0]);
    panels[1] = new_panel(windows[1]);
    panels[2] = new_panel(windows[2]);
    panels[3] = new_panel(windows[3]);

    update_panels();
    doupdate(); 


    unsigned short **save_buf = init_save_buffer(map_dim);

    while (looping) {
        switch (input = getch()){
            case 'q': looping = false; break;
            
            case KEY_UP: case 'k': 
                cursor_pos.y--; cursor_pos.y = CLAMP_MIN(cursor_pos.y, 0); break;
            case KEY_DOWN: case 'j':
                cursor_pos.y++; cursor_pos.y = CLAMP_MAX(cursor_pos.y, map_dim.y-1); break;
            case KEY_LEFT: case 'h':
                cursor_pos.x--; cursor_pos.x = CLAMP_MIN(cursor_pos.x, 0); break;
            case KEY_RIGHT: case 'l':
                cursor_pos.x++; cursor_pos.x = CLAMP_MAX(cursor_pos.x, map_dim.x-1); break;
            
            case '0': case '1': case '2': case '3': case '4': case '5':
                printch = input;
                mvwaddch(windows[3], cursor_pos.y, cursor_pos.x, printch);
                // add character
                
                save_buf[cursor_pos.y][cursor_pos.x] = printch-'0';

                if (cursor_pos.x == map_dim.x-1 && cursor_pos.y != map_dim.y-1) {
                    cursor_pos.x=0;
                    cursor_pos.y++;
                } else if (cursor_pos.x != map_dim.x-1){
                    cursor_pos.x++;
                } 
                break;

            case 's': save(save_buf, map_dim, "test_map");

        }
        mvwprintw(windows[1], 0, 7, "%d", cursor_pos.x);
        mvwprintw(windows[1], 0, 10, "%d", cursor_pos.y);

        
        wmove(windows[2], cursor_pos.y, cursor_pos.x);
        wrefresh(windows[2]); 
        // line is not visually updating cursor for some reason.
 

        update_panels();
        doupdate();
    }


    endwin();
    deinit_save_buffer(save_buf, (int_coord){map_dim.y, map_width});
    printf("Save buf deinitialised\n");
    return 0;
}

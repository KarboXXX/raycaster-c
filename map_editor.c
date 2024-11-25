#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <form.h>
#include <time.h>

int finish() {
  endwin();
  exit(0);  
}

struct window {
  int border;
  int max_x;
  int max_y;
  int x;
  int y;
};

unsigned char map_edit[64];

struct disket {
  unsigned char buf[65];
};

void move_using_window(WINDOW *window_reference, int y, int x) {
  int ry, rx;
  wmove(window_reference, y, x);
  getsyx(ry, rx);
  move(ry, rx);
}

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("You need to inform at least the file path for the output map binary.\n");
    exit(1);
  }

  /* memset(map_edit, 0, 64*sizeof(map_edit[0])); // start with an empty map */

  {
    FILE *file_ptr;
    file_ptr = fopen(argv[1], "rb");
    if (file_ptr != NULL) {
      while (fread(&map_edit, sizeof(unsigned char)*64, 1, file_ptr)) {
        printf("[start] reading %lu bytes from %s...\n", sizeof(unsigned char)*64, argv[1]);
      }
      fclose(file_ptr);
    }
  }
  
  struct window window, map;
  WINDOW* main_win = initscr();
  if (main_win == NULL) {
    fprintf(stderr, "error when starting ncurses.");
    exit(EXIT_FAILURE);
  }
  
  keypad(stdscr, TRUE);
  noecho();
  cbreak();
  curs_set(2);

  move(0, 0);
  addstr("Move with your arrow keys and press SPACE on squares to toggle air/block on the map!");
  move(1,0);
  addstr("Press S to [S]ave and Q to [Q]uit.");
  move(2, 0);
  refresh();
  
  WINDOW* map_win = newwin(15,45,2,0);
  box(map_win, 0, 0);
  map.border = 1;
  wmove(map_win, 0+map.border, 0+map.border);
  wrefresh(stdscr);
  wrefresh(map_win);

  getyx(map_win, map.y, map.x);
  getyx(stdscr, window.y, window.x);
  getmaxyx(map_win, map.max_y, map.max_x);  

  wrefresh(map_win);
  
  int cxoff = 8;
  int cyoff = 3;
  for (int y=0; y<8; y++) {
    for (int x=0; x<8; x++) {
      move_using_window(map_win, y+cyoff, cxoff+((1+x)*3));
      if (map_edit[x + (8*y)] != 0) waddstr(map_win, "[X]");
      else waddstr(map_win, "[ ]");
      wrefresh(map_win);
    }
  }
  
  size_t coord_str_size = 30;
  char coord_str_buf[coord_str_size];
  strncpy(coord_str_buf, "", coord_str_size);
  
  size_t file_info_bufsize = 15;
  char file_info_str[file_info_bufsize];
  strncpy(file_info_str, "", file_info_bufsize);

  time_t time_saved;
  
  move_using_window(map_win, 1, 1);
  while(1) {
    wrefresh(map_win);

    getmaxyx(map_win, map.max_y, map.max_x);        
    getyx(map_win, map.y, map.x);
    getsyx(window.y, window.x);

    if (time_saved+3 <= time(NULL) && file_info_str[0] == 'S') {
      memset(file_info_str, ' ', sizeof(char)*file_info_bufsize);
      wrefresh(map_win);
    }
    
    move(3,1);
    memset(coord_str_buf, ' ', sizeof(char)*coord_str_size);
    addstr(coord_str_buf);
    move(3,1);
    snprintf(coord_str_buf, coord_str_size, "x: %d (%d) | y: %d (%d)",
             map.x-map.border, window.x, map.y-map.border, window.y);
    addstr(coord_str_buf);
    move(15, 1);
    addstr(file_info_str);
    
    move_using_window(map_win, map.y, map.x);
    
    wrefresh(map_win);
    
    int ch = getch();
    if (ch == KEY_BACKSPACE || ch == 'q') return finish();
    if (ch == KEY_ENTER || ch == ' ') {
      if ((map.x - (cxoff-5)) % 3 == 0 && map.x >= cxoff+4 && map.x <= cxoff+(8*3)+3 /* 5 cuz [ ] is 3, [ ][. is 5, a 5 = 1 (second index) */
          && map.y >= 3 && map.y <= 3*8) { 
        int j = (((map.x - cxoff) / 3) - 1) + ((map.y - 3) * 8);
        if (map_edit[j] == 1) {
          map_edit[j] = 0;
          addch(' ');   
        } else {
          map_edit[j] = 1;
          addch('X');
        }
      }
    }
    if (ch == 's') {
      strncpy(file_info_str, "Saving...", file_info_bufsize);

      {
        FILE *file_ptr;
        file_ptr = fopen(argv[1], "wb");
        if (file_ptr == NULL) {
          fprintf(stderr, "\nError to open the file\n");
          exit(1);
        }
        fwrite(&map_edit, sizeof(unsigned char)*64, 1, file_ptr);
        fclose(file_ptr);
      }
      
      strncpy(file_info_str, "Saved.", file_info_bufsize);
      time(&time_saved);
    }
    if (ch == KEY_RIGHT && map.x+1 != map.max_x-map.border) {
      move_using_window(map_win, map.y, map.x+1);
      /* wrefresh(map_win); */
    }
    if (ch == KEY_LEFT && map.x != map.border) {
      move_using_window(map_win, map.y, map.x-1);
      /* wrefresh(map_win); */
    }
    if (ch == KEY_UP && map.y != map.border) {
      move_using_window(map_win, map.y-1, map.x);
      /* wrefresh(map_win); */
    }
    if (ch == KEY_DOWN && map.y+1 != map.max_y-map.border) {
      move_using_window(map_win, map.y+1, map.x);
      /* wrefresh(map_win); */
    }
  }
  
}


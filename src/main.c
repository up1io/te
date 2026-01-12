#include <curses.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

enum EDITOR_MODE {
  NORMAL = 0,
  INSERT,
};

char *readFile(const char *filename)
{
  FILE *file = fopen(filename, "r");
  assert(file != NULL);

  char *code;
  size_t n = 0;
  char c;

  code = malloc(1000);

  while((c = fgetc(file)) != EOF) {
    code[n++] = (char)c;
  }

  code[n++] = '\0';

  fclose(file);

  return code;
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("please provide a file\n");
    return 1;
  }

  const char *filename = argv[1];

  const char *buf = readFile(filename);

  WINDOW *mainwin = initscr();
  cbreak();
  noecho();
  clear();

  keypad(stdscr, TRUE);

  printw("%s", buf);

  char ch;
  int row = 0;
  int y = 0;
  int mode = NORMAL;

  move(row, y);
  refresh();

  while((ch = wgetch(mainwin)) != 'q') {
    // clear();
    //printw("%s", buf);
    //
    //

    if (mode == INSERT && ch != 27) {
      move(row, y);
      addch(ch);
      move(row, y++);
    } else if (ch == 27) {
      mode = NORMAL;
    }

    if (mode == NORMAL) {
      switch (ch) {
        case 'j':
          row++;
          break;
        case 'k':
          if(row > 0) row--;
          break;
        case 'w':
          y++;
          break;
        case 'b':
          if (y > 0) y--;
          break;
        case 'i':
          mode = INSERT;
          break;
        case 'n':
          mode = NORMAL;
          break;
      }
    }

    mvprintw(0, 40, "X %d Y %d M %d", row, y, mode);

    move(row, y);
    refresh();
  }

  endwin();
  return 0;
}

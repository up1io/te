#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif
 
#include <curses.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

enum EDITOR_MODE {
  NORMAL = 0,
  INSERT,
};

typedef struct {
  char **lines;
  int num_lines;
} buffer;

char *strdup(const char *src) {
    char *dst = malloc(strlen (src) + 1);
    if (dst == NULL) return NULL;
    strcpy(dst, src);
    return dst;
}

int append_line(char ***lines, int *num_lines, const char *new_content)
{
  char **temp = realloc(*lines, (*num_lines + 1) * sizeof(char *)); 
  if (!temp) {
    return -1;
  }

  *lines = temp;

  (*lines)[*num_lines] = strdup(new_content);
  if (!(*lines)[*num_lines]) {
    return -1;
  }

  (*num_lines)++;
  return 0;
}

int readFile(const char *filename, buffer *out_buf)
{
  FILE *file = fopen(filename, "r");
  assert(file != NULL);

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, file)) != -1) {
    line[strcspn(line, "\n")] = '\0';
    int result = append_line(&out_buf->lines, &out_buf->num_lines, line);
    if (result != 0) {
      fclose(file);
      return -1;
    }
  }

  fclose(file);

  return 0;
}

void drawBuffer(const buffer *buf)
{
  for (int i = 0; i < buf->num_lines; i++) {
    mvprintw(i, 0, "%s", buf->lines[i]);
  } 

  refresh();
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("please provide a file\n");
    return 1;
  }

  const char *filename = argv[1];

  buffer buf = {};
  readFile(filename, &buf);

  int w, h;
  int ch;

  initscr();
  cbreak();
  noecho();
  clear();

  cbreak();

  w = COLS;
  h = LINES;

  keypad(stdscr, TRUE);

  // printw("w %d h %d", w, h);
  refresh(); 

  drawBuffer(&buf);
  while ((ch = getch()) != 'q') {
    drawBuffer(&buf);
  }

  endwin();

  return 0;
}

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

enum MOVE_DIRECTION {
  MOVE_DIRECTION_FORWARD = 0,
  MOVE_DIRECTION_BACKWARD,
}

typedef struct {
  char **lines;
  int num_lines;
  int cursor_x;
  int cursor_y;
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

void draw_buffer(const buffer *buf)
{
  for (int i = 0; i < buf->num_lines; i++) {
    mvprintw(i, 0, "%s", buf->lines[i]);
  } 

  move(buf->cursor_y, buf->cursor_x);

  refresh();
}

void move_cursor_until_find_space_ch(buffer *buf, MOVE_DIRECTION dir)
{
  // const char *current_line = buf->lines[buf->cursor_y];

  if (dir == MOVE_DIRECTION_BACKWARD && buf->cursor_x > 0) {
    buf->cursor_x--;
  }
  if (dir == MOVE_DIRECTION_FORWARD) {
    buf->cursor_x++;
  }

  // mvprintw(24, 0, "%s %c", current_line, current_ch);
}

void process_input(buffer *buf, const char ch)
{
  // NOTE(john): basic movement
  switch(ch) {
    case 'k':
      if (buf->cursor_y > 0) buf->cursor_y--;
      break;
    case 'j':
      buf->cursor_y++;
      break;
    case 'h':
      if (buf->cursor_x > 0) buf->cursor_x--;
      break;
    case 'l':
      buf->cursor_x++;
      break;
    case 'w':
      move_cursor_until_find_space_ch(buf, MOVE_DIRECTION_FORWARD);
      break;
    case 'b':
      move_cursor_until_find_space_ch(buf, MOVE_DIRECTION_BACKWARD);
      break;
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("please provide a file\n");
    return 1;
  }

  const char *filename = argv[1];

  buffer buf = {};

  buf.cursor_x = 0;
  buf.cursor_y = 0;

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

  draw_buffer(&buf);
  while ((ch = getch()) != 'q') {
    process_input(&buf, ch);
    draw_buffer(&buf);
  }

  endwin();

  return 0;
}

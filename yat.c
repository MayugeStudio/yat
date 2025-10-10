#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SB_IMPLEMENTATION
#include "sb.h"

#define YAT_TODO_FILE "./todos.yat"
#define DESC_CAPACITY 256


struct Todo {
  int id;
  char desc[DESC_CAPACITY];
  int done;
};

struct Todos {
  struct Todo *items;
  size_t count;
  size_t capacity;
};

void usage()
{
  printf("Usage: yat <SUBCOMMAND>\n");
  printf("\n");
  printf("SUBCOMMANDS:\n");
  printf("\n");
  printf("    yat    <target-dir> - Initialize a specified directory with empty todos\n");
  printf("    add    <todo-name>  - Add todo\n");
  printf("    close  <todo-id>    - Close todo\n");
  printf("    delete <todo-id>    - Delete todo\n");
  printf("    list                - Show all todos\n");
  printf("\n");
}

#define SHIFT(argc, argv) (assert((argc)>0), (argc)--, *(argv)++)

// ----------| Core Functions |----------

bool read_entire_file(const char *filepath, struct StringBuilder *sb)
{
  FILE *f = fopen(filepath, "r");
  if (!f) {
    printf("ERROR: Could not open file: %s\n", filepath);
    return false;
  }
  
  fseek(f, 0, SEEK_END);
  long m = ftell(f);
  fseek(f, 0, SEEK_SET);

  DA_RESERVE(sb, m);

  sb->count = fread(sb->items, sizeof(char), m, f);

  if (ferror(f)) {
    printf("ERROR: Could not read file: %s\n", filepath);
    return false;
  }

  fclose(f);

  SB_APPEND_NULL(sb);

  return true;
}


// TODO: Use StringView
bool parse_line_as_todo(char *line, const size_t line_length, struct Todo *todo, int line_num)
{
  const char *line_start = line;
  size_t rest_chars = line_length;

// TODO: Report line number
#define NEXT(line, rest_chars)                                 \
  do {                                                         \
    (rest_chars)--;                                            \
    if ((rest_chars) == 0) {                                   \
      printf("ERROR:%d: broken line was found\n", line_num); \
      return false;                                            \
    }                                                          \
    (line)++;                                                  \
  } while (0)


  // expect `[`
  if (*line != '[') {
    printf("ERROR:%d: `[` was expected but got `%c`\n", line_num, *line);
    return false;
  }

  NEXT(line, rest_chars);

  // skip ` ` or `x`
  // consider space-character as not completed and `x` as completed
  if (*line == ' ') {
    todo->done = 0;
  } else if (*line == 'x') {
    todo->done = 1;
  } else {
    printf("ERROR:%d: `[` or `x` were expected but got `%c`\n", line_num, *line);
    return false;
  }
  NEXT(line, rest_chars);
  

  // expect `]`
  if (*line != ']') {
    printf("ERROR:%d: `]` was expected but got `%c`\n", line_num, *line);
    return false;
  }
  NEXT(line, rest_chars);

  // expect `:`
  if (*line != ':') {
    printf("ERROR:%d: `:` was expected but got `%c`\n", line_num, *line);
    return false;
  }
  NEXT(line, rest_chars);

  // expect ` ` or ``
  while (*line == ' ') NEXT(line, rest_chars);

  // the rest of chars are considered as a name of todo
  todo->id = 0;

  const size_t name_length = rest_chars+1;
  if (name_length >= DESC_CAPACITY) {
    printf("ERROR:%d: name length has more than %d characters", line_num, DESC_CAPACITY);
    return false;
  }
  memcpy(todo->desc, line, name_length);
  todo->desc[name_length-1] = '\0';

#undef NEXT

  return true;
}

bool read_todos_from_file(const char *filepath, struct Todos *todos)
{
  struct StringBuilder sb = {0};
  if (!read_entire_file(filepath, &sb)) return false;
  size_t line_start_offset = 0;
  int line_num = 0;

  // TODO: Split sb.items by newline char and ignore 0 length line. 

  while (sb.items[line_start_offset] != '\0') {
    // Find newline character
    for (size_t i = line_start_offset; i<sb.count; i++) {
      if (sb.items[i] == '\n') {
        const size_t line_length = i - line_start_offset;

        struct Todo todo = {0};
        if (!parse_line_as_todo(sb.items+line_start_offset, line_length, &todo, line_num)) {
          return false;
        }

        DA_APPEND(todos, todo);

        // Update line_start_offset for next iteration.
        line_start_offset = i+1;
        // Increment line_num
        line_num += 1;
      }
    }
  }

  return true;
}

bool write_todos_to_file(const char *filepath, struct Todos todos)
{
  struct StringBuilder out = {0};

  for (size_t i=0; i<todos.count; ++i) {
    struct Todo todo = todos.items[i];
    char done = todo.done ? 'x' : ' ';
    sb_appendf(&out, "[%c]: %s\n", done, todo.desc);
  }

  // Insert new line at the end of file
  DA_APPEND(&out, '\n');

  FILE *f = fopen(filepath, "w");
  if (!f) {
    printf("ERROR: Could not open file: %s\n", filepath);
    return false;
  }

  int written_count = fwrite(out.items, sizeof(*out.items), out.count, f);

  if (written_count != out.count) {
    printf("ERROR: Could not write file: %s\n", filepath);
  }

  fclose(f);

  return true;
}


void dump_todos(struct Todos todos, bool separator)
{
  if (separator) printf("--------------------\n")
  for (size_t i=0; i<todos.count; ++i) {
    printf("[%c] %s\n", todos.items[i].done ? 'x' : ' ',todos.items[i].desc);
  }
  if (separator) printf("--------------------\n");
}

// ----------| Core Functions End |----------


// ----------| Subcommand Functions |----------
//
// NOTE: All functions are expecting null-terminated string.
bool init_yat();
bool add_todo(const char *name);
bool close_todo(const char *id);
bool delete_todo(const char *id);
bool list_todos();


bool init_yat()
{
  // Check whether yat is already initialized or not.
  FILE *f = fopen(YAT_TODO_FILE, "r");
  if (f) {
    fclose(f);
    printf("ERROR: current directory is already initialized.\n");
    return false;
  }

  // Create yat-data file in the specified directory.
  f = fopen(YAT_TODO_FILE, "w");
  if (!f) {
    printf("ERROR: could not create `"YAT_TODO_FILE"`.\n");
    return false;
  }
  fclose(f);

  return true;
}

bool add_todo(const char *name)
{
  struct Todos todos = {0};
  if (!read_todos_from_file(YAT_TODO_FILE, &todos)) {
    return false;
  }
  dump_todos(todos);

  return true;
}

bool close_todo(const char *id)
{
  return true;
}

bool delete_todo(const char *id)
{
  return true;
}

bool list_todos()
{
  struct StringBuilder sb;
  if (!read_entire_file(YAT_TODO_FILE, &sb)) { return false; }
  printf("%s\n", sb.items);
  return true;
}

int main(int argc, char **argv) {
  const char *program_name = SHIFT(argc, argv);

  if (argc == 0) {
    usage();
    printf("ERROR: no subcommand");
    return -1;
  }

  const char *command_name = SHIFT(argc, argv);

  if (strcmp(command_name, "init") == 0) {
    if (!init_yat()) return -1;

  } else if (strcmp(command_name, "add") == 0) {
    if (argc == 0) {
      usage();
      printf("ERROR: `add` expected the name of todo.");
      return -1;
    }

    const char *todo_name = SHIFT(argc, argv);
    if (!add_todo(todo_name)) return -1;

  } else if (strcmp(command_name, "close") == 0) {
    if (argc == 0) {
      usage();
      printf("ERROR: `close` expected the name of id.");
      return -1;
    }

    const char *todo_id = SHIFT(argc, argv);
    if (!close_todo(todo_id)) return -1;

  } else if (strcmp(command_name, "delete") == 0) {
    if (argc == 0) {
      usage();
      printf("ERROR: `delete` expected the name of id.");
      return -1;
    }

    const char *todo_id = SHIFT(argc, argv);
    if (!delete_todo(todo_id)) return -1;

  } else if (strcmp(command_name, "list") == 0) {
    if (!list_todos()) return -1;
  } else {
    usage();
    printf("ERROR: invalid subcommand\n");
  }

  return 0;
}


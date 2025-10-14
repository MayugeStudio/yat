// Yat (Yet Another Todo app)
//
// This program provide user with 4 kind of commands.
//
// 1. init:   Initialize yat
// 2. add:    Add todo
// 3. close:  Mark todo as complete
// 4. delete: Delete todo
// 5. list:   Show the list of todos
//

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SB_IMPLEMENTATION
#include "sb.h"

#define YAT_TODO_FILE "./todos.yat"
#define DESC_CAPACITY 256

#define NEXT(line, row, col)                                   \
  do {                                                         \
    if (*line == '\0') {                                       \
      printf("ERROR:%d:%d broken line was found\n", row, col); \
      return false;                                            \
    }                                                          \
    (line)++;                                                  \
  } while (0)

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
  printf("    init   <target-dir>        - Initialize a specified directory with empty todos\n");
  printf("    add    <todo-description>  - Add todo\n");
  printf("    close  <todo-id>           - Close todo with the specified id\n");
  printf("    delete <todo-id>           - Delete todo with the specified id\n");
  printf("    list                       - Show all todos\n");
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
bool parse_line_as_todo(char *line, const size_t line_length, struct Todo *todo, int row)
{
  const char *line_start = line;

  char *endptr;
  // expect numbers
  long id = strtol(line_start, &endptr, 10);
  if (endptr == line_start) {
    printf("ERROR:%d:%d expected number but got `%c`", row, (int)(line-line_start), *line);
    return false;
  }
  line = endptr;
  // skip spaces
  while (*line == ' ') line++;

  // expect '|'
  if (*line != '|') {
    printf("ERROR:%d:%d `|` was expected but got `%c`", row, (int)(line-line_start), *line);
  }
  // skip '|'
  NEXT(line, row, (int)(line-line_start));

  // skip spaces after '|'
  while (*line == ' ') line++;

  // expect `0` or `1`
  if (*line == '0') {
    todo->done = 0;
  } else if (*line == '1') {
    todo->done = 1;
  } else {
    printf("ERROR:%d:%d `0` or `1` were expected but got `%c`\n", row, (int)(line-line_start), *line);
    return false;
  }
  // skip `0` or `1`
  NEXT(line, row, (int)(line-line_start));

  // skip spaces after '|'
  while (*line == ' ') line++;
  
  // expect `|`
  if (*line != '|') {
    printf("ERROR:%d:%d `|` was expected but got `%c`\n", row, (int)(line-line_start), *line);
    return false;
  }
  // skip `|`
  NEXT(line, row, (int)(line-line_start));

  // skip ` ` if exists
  while (*line == ' ') NEXT(line, row, (int)(line-line_start));

  // the rest of chars are considered as a name of todo
  todo->id = (int)id;

  const size_t name_length = line_length - (line-line_start);
  if (name_length >= DESC_CAPACITY) {
    printf("ERROR:%d:%d name length has more than %d characters", row, (int)(line-line_start), DESC_CAPACITY);
    return false;
  }
  memcpy(todo->desc, line, name_length);
  todo->desc[name_length] = '\0';

  return true;
}

bool read_todos_from_file(const char *filepath, struct Todos *todos)
{
  struct StringBuilder sb = {0};
  if (!read_entire_file(filepath, &sb)) return false;
  size_t line_start_offset = 0;
  int row = 1;

  while (sb.items[line_start_offset] != '\0') {
    // Find newline character
    for (size_t i = line_start_offset; i<sb.count; i++) {
      if (i == sb.count || sb.items[i] == '\n') {
        const size_t line_length = i - line_start_offset;

        if (line_length == 0) {
          line_start_offset = i+1;
          row += 1;
          break;
        }

        struct Todo todo = {0};
        if (!parse_line_as_todo(sb.items+line_start_offset, line_length, &todo, row)) {
          return false;
        }

        DA_APPEND(todos, todo);

        // Update line_start_offset for next iteration.
        line_start_offset = i+1;
        // Increment line_num
        row += 1;
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
    char done = todo.done ? '1' : '0';
    sb_appendf(&out, "%d | %c | %s\n", todo.id, done, todo.desc);
  }

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
  if (separator) printf("--------------------\n");
  for (size_t i=0; i<todos.count; ++i) {
    printf("[%c]: %s(#%03d)\n", todos.items[i].done ? 'x' : ' ', todos.items[i].desc, todos.items[i].id);
  }
  if (separator) printf("--------------------\n");
}

// ----------| Core Functions End |----------

// ----------| Utility Functions |----------

int get_next_id(struct Todos todos)
{
  int current_max_id = -1;
  if (todos.count == 0) {
    return 1; // Default todo id
  }

  for (int i=0; i<todos.count; ++i) {
    if (current_max_id < todos.items[i].id) {
      current_max_id = todos.items[i].id;
    }
  }

  return current_max_id + 1; // increment current_max_id
}

bool get_todo_index_by_id(struct Todos todos, int id, int *out)
{
  for (int i=0; i<todos.count; ++i) {
    if (todos.items[i].id == id) {
      *out = i;
      return true;
    }
  }

  return false;
}

// ----------| Utility Functions End |----------


// ----------| Subcommand Functions |----------

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
  size_t name_length = strlen(name);
  if (name_length >= DESC_CAPACITY) {
    printf("ERROR: name length has more than %d characters", DESC_CAPACITY);
  }


  struct Todos todos = {0};
  if (!read_todos_from_file(YAT_TODO_FILE, &todos)) {
    return false;
  }

  int next_id = get_next_id(todos);
  
  // Construct new todo struct
  struct Todo todo = {
    .id=next_id,             // id
    .done=0,                 // done
  };
  memcpy(todo.desc, name, name_length);
  todo.desc[name_length] = '\0';

  // Append a todo into existing todos
  DA_APPEND(&todos, todo);
  if (!write_todos_to_file(YAT_TODO_FILE, todos)) {
    return false;
  }

  dump_todos(todos, true);

  printf("Successfully added todo to "YAT_TODO_FILE"\n");

  return true;
}

bool close_todo(int id)
{
  // Read todos from file.
  struct Todos todos = {0};
  if (!read_todos_from_file(YAT_TODO_FILE, &todos)) {
    return false;
  }

  // Find todo with specified id
  int index;
  if (!get_todo_index_by_id(todos, id, &index)) {
    printf("ERROR: todo with id %d not found\n", id);
    return false;
  }


  // Close the todo
  todos.items[index].done = 1;

  // Apply change by writing updated todos to file
  if (!write_todos_to_file(YAT_TODO_FILE, todos)) {
    return false;
  }

  printf("Successfully closed todo(#%03d)\n", todos.items[index].id);

  return true;
}

bool delete_todo(int id)
{
  // Read todos from file.
  struct Todos todos = {0};
  if (!read_todos_from_file(YAT_TODO_FILE, &todos)) {
    return false;
  }

  // Find todo with specified id
  int index;
  if (!get_todo_index_by_id(todos, id, &index)) {
    printf("ERROR: todo with id %d not found\n", id);
    return false;
  }

  // Delete todo with specified id
  for (int i=index; i+1<todos.count; ++i) {
    todos.items[i] = todos.items[i+1];
  }
  todos.count -= 1;

  // Apply change by writing updated todos to file
  if (!write_todos_to_file(YAT_TODO_FILE, todos)) {
    return false;
  }

  printf("Successfully deleted todo(#%03d)\n", id);

  return true;
}

bool list_todos()
{
  struct Todos todos = {0};
  if (!read_todos_from_file(YAT_TODO_FILE, &todos)) return false;
  dump_todos(todos, false);
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
    // Do application level procedure
    if (!add_todo(todo_name)) return -1;

  } else if (strcmp(command_name, "close") == 0) {
    if (argc == 0) {
      usage();
      printf("ERROR: `close` expected the id of the todo.");
      return -1;
    }

    const char *todo_id = SHIFT(argc, argv);

    // Convert char* to long
    char *end;
    long todo_id_long = strtol(todo_id, &end, 10);
    if (*end != '\0') {
      printf("ERROR: invalid integer `%s`\n", todo_id);
      return -1;
    }

    // Do application level procedure
    // TODO: Perform range checking when converting a todo ID from long to int.
    if (!close_todo((int)todo_id_long)) return -1;

  } else if (strcmp(command_name, "delete") == 0) {
    if (argc == 0) {
      usage();
      printf("ERROR: `delete` expected the name of id.");
      return -1;
    }

    const char *todo_id = SHIFT(argc, argv);

    // Convert char* to long
    char *end;
    long todo_id_long = strtol(todo_id, &end, 10);
    if (*end != '\0') {
      printf("ERROR: invalid integer `%s`\n", todo_id);
      return -1;
    }

    // Do application level procedure
    // TODO: Perform range checking when converting a todo ID from long to int.
    if (!delete_todo((int)todo_id_long)) return -1;

  } else if (strcmp(command_name, "list") == 0) {
    // Do application level procedure
    if (!list_todos()) return -1;
  } else {
    usage();
    printf("ERROR: invalid subcommand\n");
  }

  return 0;
}


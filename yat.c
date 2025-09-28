#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sb.h"

#define YAT_TODO_FILE "./todos.yat"

struct Todo {
  int id;
  char desc[256];
  int done;

  struct Todo *child;
};

void usage() {
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
}


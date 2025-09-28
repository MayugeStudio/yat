#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sb.h"

struct Todo {
  int id;
  char desc[256];
  int done;

  struct Todo *child;
};

void usage(const char *program_name) {
  printf("Usage: %s <SUBCOMMAND>\n", program_name);
  printf("SUBCOMMANDS:\n");
  printf("    yat    <target-dir> - Initialize a specified directory with empty todos\n");
  printf("    add    <todo-name>  - Add todo\n");
  printf("    close  <todo-id>    - Close todo\n");
  printf("    delete <todo-id>    - Delete todo\n");
  printf("    list                - Show all todos\n");
}

#define SHIFT(argc, argv) (assert((argc)>0), (argc)--, *(argv)++)

// |-----|-----| Subcommand Functions |-----|-----|
//
// NOTE: All functions are expecting null-terminated string.
bool init_yat(const char *target_dir);
bool add_todo(const char *name);
bool close_todo(const char *id);
bool delete_todo(const char *id);
bool list_todos();


bool init_yat(const char *target_dir)
{
  struct StringBuilder sb = {0};
  SB_APPEND_CSTR(&sb, target_dir);
  SB_APPEND_CSTR(&sb, "/yat-data.bin");
  SB_APPEND_NULL(&sb);

  // Check whether yat is already initialized or not.
  FILE *f = fopen(sb.items, "r");
  if (f) {
    fclose(f);
    printf("ERROR: %s already exists.\n", sb.items);
    return false;
  }

  // Create yat-data file in the specified directory.
  f = fopen(sb.items, "w");
  if (!f) {
    printf("ERROR: could not create `%s`.", sb.items);
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
    usage(program_name);
    printf("ERROR: no subcommand");
    return -1;
  }

  const char *command_name = SHIFT(argc, argv);

  if (strcmp(command_name, "init") == 0) {
    if (argc == 0) {
      usage(program_name);
      printf("ERROR: `init` expected the target directory path.");
      return -1;
    }

    const char *target_dir = SHIFT(argc, argv);
    if (!init_yat(target_dir)) return -1;

  } else if (strcmp(command_name, "add") == 0) {
    if (argc == 0) {
      usage(program_name);
      printf("ERROR: `add` expected the name of todo.");
      return -1;
    }

    const char *todo_name = SHIFT(argc, argv);
    if (!add_todo(todo_name)) return -1;

  } else if (strcmp(command_name, "close") == 0) {
    if (argc == 0) {
      usage(program_name);
      printf("ERROR: `close` expected the name of id.");
      return -1;
    }

    const char *todo_id = SHIFT(argc, argv);
    if (!close_todo(todo_id)) return -1;

  } else if (strcmp(command_name, "delete") == 0) {
    if (argc == 0) {
      usage(program_name);
      printf("ERROR: `delete` expected the name of id.");
      return -1;
    }

    const char *todo_id = SHIFT(argc, argv);
    if (!delete_todo(todo_id)) return -1;

  } else if (strcmp(command_name, "list") == 0) {
    if (!list_todos()) return -1;
  } else {
    usage(program_name);
    printf("ERROR: invalid subcommand\n");
  }
}


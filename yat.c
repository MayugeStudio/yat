#include <assert.h>
#include <stdio.h>
#include <string.h>

struct Todo {
  int id;
  char desc[256];
  int done;

  struct Todo *child;
};

void usage(const char *program_name) {
  printf("Usage: %s <SUBCOMMAND>\n", program_name);
  printf("SUBCOMMANDS:\n");
  printf("    add    <todo-name> - Add todo\n");
  printf("    close  <todo-id>   - Close todo\n");
  printf("    delete <todo-id>   - Delete todo\n");
  printf("    list               - Show all todos\n");
}

#define SHIFT(argc, argv) (assert((argc)>0), (argc)--, *(argv)++)

//int load_tasks_from_

int main(int argc, char **argv) {
  const char *program_name = SHIFT(argc, argv);

  const char *command_name = SHIFT(argc, argv);

  if (strcmp(command_name, "add") == 0) {
    const char *todo_name = SHIFT(argc, argv);

  } else if (strcmp(command_name, "close") == 0) {
    const char *todo_id = SHIFT(argc, argv);

  } else if (strcmp(command_name, "delete") == 0) {
    const char *todo_id = SHIFT(argc, argv);

  } else if (strcmp(command_name, "list") == 0) {

  } else {
    usage(program_name);
    printf("ERROR: invalid subcommand was provided");
  }
}


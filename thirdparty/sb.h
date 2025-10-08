#include "da.h"

struct StringBuilder {
  char *items;
  size_t count;
  size_t capacity;
};

int sb_appendf(struct StringBuilder *sb, const char *fmt, ...);

#define SB_APPEND_NULL(sb) DA_APPEND(sb, '\0')

#define SB_APPEND_SIZED(sb, buf, size) DA_APPEND_MANY(sb, buf, size)

#define SB_APPEND_CSTR(sb, elem)    \
  do {                              \
    const char *buf = (elem);       \
    size_t count = strlen(buf);     \
    DA_APPEND_MANY(sb, buf, count); \
  } while(0)


#ifdef SB_IMPLEMENTATION

#include <stdarg.h>


// This function was Taken from https://github.com/tsoding/nob.h
int sb_appendf(struct StringBuilder *sb, const char *fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  int n = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  // NOTE: the new_capacity needs to be +1 because of the null terminator.
  // However, further below we increase sb->count by n, not n + 1.
  // This is because we don't want the sb to include the null terminator. The user can always sb_append_null() if they want it
  DA_RESERVE(sb, sb->count + n + 1);
  char *dest = sb->items + sb->count;
  va_start(args, fmt);
  vsnprintf(dest, n+1, fmt, args);
  va_end(args);

  sb->count += n;

  return n;
}

#endif // SB_IMPLEMENTATION


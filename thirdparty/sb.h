#include "da.h"

struct StringBuilder {
  char *items;
  size_t count;
  size_t capacity;
};

#define SB_APPEND_NULL(sb) DA_APPEND(sb, '\0')

#define SB_APPEND_SIZED(sb, buf, size) DA_APPEND_MANY(sb, buf, size)

#define SB_APPEND_CSTR(sb, elem)    \
  do {                              \
    const char *buf = (elem);       \
    size_t count = strlen(buf);     \
    DA_APPEND_MANY(sb, buf, count); \
  } while(0)


#ifdef SB_IMPLEMENTATION
#endif // SB_IMPLEMENTATION


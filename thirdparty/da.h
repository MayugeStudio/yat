#include <assert.h>
#include <stdlib.h>

#define DA_INIT_CAPACITY 8
#define DA_MALLOC malloc
#define DA_REALLOC realloc
#define DA_ASSERT assert

#define DA_RESERVE(da, target_capacity)                         \
    do {                                                        \
      if ((da)->capacity == 0) {                                \
        (da)->capacity = DA_INIT_CAPACITY;                      \
      }                                                         \
      while ((target_capacity) < (da)->capacity) {              \
        (da)->capacity *= 2;                                    \
      }                                                         \
      (da)->items = DA_REALLOC((da)->items, (da)->capacity);    \
    } while (0)

#define DA_APPEND(da, item)                                             \
  do {                                                                  \
    if ((da)->capacity == 0) {                                          \
      (da)->capacity = DA_INIT_CAPACITY;                                \
      (da)->count = 0;                                                  \
      (da)->items = DA_MALLOC(DA_INIT_CAPACITY * sizeof(*(da)->items)); \
      if (!(da)->items) DA_ASSERT(false && "buy more RAM lol");         \
    } else if ((da)->capacity == (da)->count) {                         \
      (da)->capacity = (da)->capacity * 2;                              \
      (da)->items = DA_REALLOC((da)->items, (da)->capacity);            \
    }                                                                   \
    (da)->items[(da)->count] = item;                                    \
    (da)->count++;                                                      \
  } while (0)

#define DA_APPEND_MANY(da, elems, items_count)                                          \
  do {                                                                                  \
    if ((da)->capacity == 0) {                                                          \
      (da)->capacity = DA_INIT_CAPACITY;                                                \
    }                                                                                   \
    if ((items_count) > (da)->capacity) {                                               \
      while ((items_count) > (da)->capacity) {                                          \
        (da)->capacity *= 2;                                                            \
      }                                                                                 \
      (da)->items = DA_REALLOC((da)->items, (da)->capacity);                            \
      memcpy((da)->items + (da)->count, (elems), (items_count)*sizeof(*(da)->items));   \
      (da)->count += items_count;                                                       \
    }                                                                                   \
  } while (0)

#ifdef DA_IMPLEMENTATION
#endif // DA_IMPLEMENTATION


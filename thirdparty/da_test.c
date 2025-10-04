#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "da.h"

struct IntArray {
    int *items;
    size_t count;
    size_t capacity;
};

int main() {
    struct IntArray arr = {0};

    printf("=== DA_APPEND test ===\n");
    for (int i = 0; i < 20; i++) {
        DA_APPEND(&arr, i);
        printf("append %d: count=%zu capacity=%zu\n", i, arr.count, arr.capacity);
    }

    printf("=== DA_APPEND_MANY test ===\n");
    int more[5] = {100, 101, 102, 103, 104};
    DA_APPEND_MANY(&arr, more, 5);
    printf("after append_many: count=%zu capacity=%zu\n", arr.count, arr.capacity);

    printf("=== Check elements ===\n");
    for (size_t i = 0; i < arr.count; i++) {
        printf("arr[%zu] = %d\n", i, arr.items[i]);
    }

    printf("=== DA_RESERVE test ===\n");
    DA_RESERVE(&arr, 100);
    printf("after reserve(100): count=%zu capacity=%zu\n", arr.count, arr.capacity);

    free(arr.items);
    return 0;
}

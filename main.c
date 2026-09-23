#include "ctype.h"
#include "stdlib.h"
#include "stdio.h"
#include <unistd.h>

typedef struct Header
{
    size_t len;
    size_t cap;
} Header_t;

#define ARRAY_CAP 128
#define arr_len(arr) ((Header_t *)(arr) - 1)->len
#define arr_cap(arr) ((Header_t *)(arr) - 1)->cap

#define arr_push(arr, x)                                   \
    do                                                     \
    {                                                      \
        (arr) = arr_grow_if_needed((arr), sizeof(*(arr))); \
        (arr)[((Header_t *)(arr) - 1)->len++] = (x);       \
    } while (0)

#define arr_reserve(arr, size) \
    (arr) = arr_reserve_impl((arr), sizeof(*(arr)), size);

#define arr_free(arr)                    \
    do                                   \
    {                                    \
        if ((arr) != NULL)               \
        {                                \
            free((Header_t *)(arr) - 1); \
            (arr) = NULL;                \
        }                                \
    } while (0)

static inline void *arr_reserve_impl(void *arr, size_t element_size, size_t capacity)
{
    Header_t *h = malloc(sizeof(*h) + element_size * capacity);
    if (h == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    h->len = 0;
    h->cap = capacity;

    return h;
}

static inline void *arr_grow_if_needed(void *arr, size_t size)
{
    Header_t *h;

    if (arr == NULL)
    {
        h = malloc(sizeof(*h) + ARRAY_CAP * size);
        if (h == NULL)
        {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        h->len = 0;
        h->cap = ARRAY_CAP;
    }
    else
    {
        h = (Header_t *)arr - 1;

        if (h->len < h->cap)
            return arr;

        size_t cap = h->cap * 2;
        Header_t *tmp = realloc(h, sizeof(*h) + cap * size);
        if (tmp == NULL)
        {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        h = tmp;
        h->cap = cap;
    }

    return h + 1;
}

int main(int argc, char const *argv[])
{
    int *numbers = NULL;
    arr_reserve(numbers, 32);

    for (size_t i = 0; i < 32; i++)
    {
        arr_push(numbers, (int)i);
    }

    int h = 42;
    arr_push(numbers, h);

    // for (size_t i = 0; i < arr_len(numbers); i++)
    // {
    //     printf("%d\n", numbers[i]);
    // }

    printf("%zu\n", arr_len(numbers));
    printf("%zu\n", arr_cap(numbers));
    sleep(30); // пауза на 2 секунды

    arr_free(numbers);

    return 0;
}

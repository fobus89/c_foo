// tests/test_number.c
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition)                             \
    do                                               \
    {                                                \
        if (!(condition))                            \
        {                                            \
            fprintf(stderr, "%s:%d: failed: %s\n",   \
                    __FILE__, __LINE__, #condition); \
            exit(EXIT_FAILURE);                      \
        }                                            \
    } while (0)

static void test_number_at_end(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer("123", &lexer) == LEX_OK);

    token_t token = {0};
    CHECK(next_token(lexer, &token) == LEX_OK);
    CHECK(token.type == TOKEN_NUMBER);
    CHECK(token.len == 3);
    CHECK(token.literal != NULL);
    CHECK(memcmp(token.literal, "123", token.len) == 0);

    free_lexer(lexer);
}

static void test_invalid_number(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer("123abc", &lexer) == LEX_OK);

    token_t token = {0};
    CHECK(next_token(lexer, &token) == LEX_INVALID_NUMBER);
    CHECK(token.type == TOKEN_ILEGALL);
    CHECK(token.len == 6);
    CHECK(token.literal != NULL);
    CHECK(memcmp(token.literal, "123abc", token.len) == 0);

    free_lexer(lexer);
}

int main(void)
{
    test_number_at_end();
    test_invalid_number();

    puts("All tests passed");
    return EXIT_SUCCESS;
}
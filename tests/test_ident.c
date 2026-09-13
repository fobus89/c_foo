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

static void check_ident(const token_t *token, const char *expected)
{
    CHECK(token->type == TOKEN_IDENT);
    CHECK(token->data.c.len == strlen(expected));
    CHECK(token->data.c.literal != NULL);
    CHECK(memcmp(token->data.c.literal, expected, token->data.c.len) == 0);
}

static void test_ident_at_end(void)
{
    const char *inputs[] = {"a", "hello", "Hello123", "_", "_name", "foo_bar2"};

    for (size_t i = 0; i < sizeof inputs / sizeof inputs[0]; i++)
    {
        lexer_t *lexer = NULL;
        CHECK(new_lexer(inputs[i], &lexer) == LEX_OK);

        token_t token = {0};
        CHECK(next_token(lexer, &token) == LEX_OK);
        check_ident(&token, inputs[i]);
        CHECK(lexer->pos == strlen(inputs[i]));
        CHECK(next_token(lexer, &token) == LEX_EOF);
        CHECK(token.type == TOKEN_EOF);

        free_lexer(lexer);
    }
}

static void test_ident_sequence(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer(" \talpha beta2\n_gamma", &lexer) == LEX_OK);
    const char *expected[] = {"alpha", "beta2", "_gamma"};

    token_t token = {0};
    for (size_t i = 0; i < sizeof expected / sizeof expected[0]; i++)
    {
        CHECK(next_token(lexer, &token) == LEX_OK);
        check_ident(&token, expected[i]);
    }
    CHECK(next_token(lexer, &token) == LEX_EOF);
    free_lexer(lexer);
}

static void test_read_ident(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer("name!", &lexer) == LEX_OK);

    token_t token = {0};
    CHECK(read_ident(lexer, &token) == LEX_OK);
    check_ident(&token, "name");
    CHECK(lexer->pos == 4);
    CHECK(next_token(lexer, &token) == LEX_OK);
    CHECK(token.type == TOKEN_BANG);
    CHECK(token.data.c.len == 1);
    CHECK(token.data.c.literal != NULL);
    CHECK(token.data.c.literal[0] == '!');
    CHECK(lexer->pos == 5);
    CHECK(next_token(lexer, &token) == LEX_EOF);
    CHECK(token.type == TOKEN_EOF);
    free_lexer(lexer);
}

static void test_invalid_start(void)
{
    const char *inputs[] = {"", "123", "!", " "};

    for (size_t i = 0; i < sizeof inputs / sizeof inputs[0]; i++)
    {
        lexer_t *lexer = NULL;
        CHECK(new_lexer(inputs[i], &lexer) == LEX_OK);
        token_t token = {0};
        CHECK(read_ident(lexer, &token) == LEX_UNEXPECTED_CHAR);
        CHECK(lexer->pos == 0);
        free_lexer(lexer);
    }
}

static void test_invalid_arguments(void)
{
    token_t token = {0};
    lexer_t invalid = {0};
    CHECK(read_ident(NULL, &token) == LEX_INVALID_INPUT);
    CHECK(read_ident(&invalid, NULL) == LEX_INVALID_INPUT);
    CHECK(read_ident(&invalid, &token) == LEX_INVALID_INPUT);
}

int main(void)
{
    test_invalid_arguments();
    test_invalid_start();
    test_ident_at_end();
    test_ident_sequence();
    test_read_ident();

    puts("All identifier tests passed");
    return EXIT_SUCCESS;
}

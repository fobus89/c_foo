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

typedef struct
{
    const char *literal;
    typetype_t type;
} expected_token_t;

static void check_sequence(const char *input,
                           const expected_token_t *expected, size_t count)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer(input, &lexer) == LEX_OK);

    token_t token = {0};
    for (size_t i = 0; i < count; i++)
    {
        size_t previous_pos = lexer->pos;
        CHECK(next_token(lexer, &token) == LEX_OK);
        CHECK(token.type == expected[i].type);
        CHECK(token.data.c.len == strlen(expected[i].literal));
        CHECK(token.data.c.literal != NULL);
        CHECK(memcmp(token.data.c.literal, expected[i].literal, token.data.c.len) == 0);
        CHECK(lexer->pos > previous_pos);
    }

    CHECK(next_token(lexer, &token) == LEX_EOF);
    CHECK(token.type == TOKEN_EOF);
    CHECK(lexer->pos == strlen(input));
    free_lexer(lexer);
}

static void test_keywords(void)
{
    const expected_token_t expected[] = {
        {"if", TOKEN_IF},
        {"else", TOKEN_ELSE},
        {"while", TOKEN_WHILE},
        {"return", TOKEN_RETURN},
        {"for", TOKEN_FOR},
    };
    check_sequence("if else\twhile\nreturn for", expected,
                   sizeof expected / sizeof expected[0]);
}

static void test_keyword_boundaries(void)
{
    const expected_token_t expected[] = {
        {"iffoo", TOKEN_IDENT},
        {"else2", TOKEN_IDENT},
        {"while_", TOKEN_IDENT},
        {"_return", TOKEN_IDENT},
        {"format", TOKEN_IDENT},
        {"If", TOKEN_IDENT},
        {"FOR", TOKEN_IDENT},
    };
    check_sequence("iffoo else2 while_ _return format If FOR", expected,
                   sizeof expected / sizeof expected[0]);
}

static void test_symbols(void)
{
    const expected_token_t expected[] = {
        {"++", TOKEN_PLUS_PLUS}, {"--", TOKEN_MINUS_MINUS},
        {"**", TOKEN_MULT_MULT}, {"//", TOKEN_DIV_DIV},
        {"==", TOKEN_EQ_EQ}, {"!", TOKEN_BANG},
        {"+", TOKEN_PLUS}, {"-", TOKEN_MINUS},
        {"*", TOKEN_MULT}, {"/", TOKEN_DIV}, {"=", TOKEN_EQ},
    };
    check_sequence("++ -- ** // == ! + - * / =", expected,
                   sizeof expected / sizeof expected[0]);

    // Also check each symbol at EOF, including single-character prefixes.
    for (size_t i = 0; i < sizeof expected / sizeof expected[0]; i++)
        check_sequence(expected[i].literal, &expected[i], 1);
}

static void test_adjacent_tokens(void)
{
    const expected_token_t expected[] = {
        {"if", TOKEN_IF}, {"!", TOKEN_BANG}, {"name", TOKEN_IDENT},
        {"++", TOKEN_PLUS_PLUS}, {"+", TOKEN_PLUS},
        {"==", TOKEN_EQ_EQ}, {"=", TOKEN_EQ},
        {"return", TOKEN_RETURN},
    };
    check_sequence("if!name+++===return", expected,
                   sizeof expected / sizeof expected[0]);
}

int main(void)
{
    test_keywords();
    test_keyword_boundaries();
    test_symbols();
    test_adjacent_tokens();

    puts("All keyword and symbol tests passed");
    return EXIT_SUCCESS;
}

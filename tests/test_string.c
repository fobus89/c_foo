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

static void check_literal(const token_t *token, typetype_t type,
                          const char *expected)
{
    CHECK(token->type == type);
    CHECK(token->len == strlen(expected));
    CHECK(token->literal != NULL);
    CHECK(memcmp(token->literal, expected, token->len) == 0);
}

static void test_string_at_end(void)
{
    // String tokens include their opening and closing quotes.
    const char *inputs[] = {"\"\"", "\"hello\"", "\"hello world\"", "\"123 ! _\""};

    for (size_t i = 0; i < sizeof inputs / sizeof inputs[0]; i++)
    {
        lexer_t *lexer = NULL;
        CHECK(new_lexer(inputs[i], &lexer) == LEX_OK);

        token_t token = {0};
        CHECK(next_token(lexer, &token) == LEX_OK);
        check_literal(&token, TOKEN_STRING, inputs[i]);
        CHECK(lexer->pos == strlen(inputs[i]));
        CHECK(next_token(lexer, &token) == LEX_EOF);
        CHECK(token.type == TOKEN_EOF);
        free_lexer(lexer);
    }
}

static void test_string_sequence(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer(" \t\"one\"\"two\" \n42", &lexer) == LEX_OK);

    token_t token = {0};
    CHECK(next_token(lexer, &token) == LEX_OK);
    check_literal(&token, TOKEN_STRING, "\"one\"");
    CHECK(next_token(lexer, &token) == LEX_OK);
    check_literal(&token, TOKEN_STRING, "\"two\"");
    CHECK(next_token(lexer, &token) == LEX_OK);
    check_literal(&token, TOKEN_NUMBER, "42");
    CHECK(next_token(lexer, &token) == LEX_EOF);
    free_lexer(lexer);
}

static void test_unterminated_string(void)
{
    const char *inputs[] = {"\"", "\"hello", "\"hello world"};

    for (size_t i = 0; i < sizeof inputs / sizeof inputs[0]; i++)
    {
        lexer_t *lexer = NULL;
        CHECK(new_lexer(inputs[i], &lexer) == LEX_OK);

        token_t token = {0};
        CHECK(next_token(lexer, &token) == LEX_UNTERMINATED_STRING);
        check_literal(&token, TOKEN_ILEGALL, inputs[i]);
        CHECK(lexer->pos == strlen(inputs[i]));
        CHECK(next_token(lexer, &token) == LEX_EOF);
        CHECK(token.type == TOKEN_EOF);
        free_lexer(lexer);
    }
}

static void test_read_str(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer("\"hello\"", &lexer) == LEX_OK);

    token_t token = {0};
    CHECK(read_str(lexer, &token) == LEX_OK);
    check_literal(&token, TOKEN_STRING, "\"hello\"");
    CHECK(next_token(lexer, &token) == LEX_EOF);
    free_lexer(lexer);
}

static void test_invalid_arguments(void)
{
    token_t token = {0};
    lexer_t invalid = {0};
    CHECK(read_str(NULL, &token) == LEX_INVALID_INPUT);
    CHECK(read_str(&invalid, NULL) == LEX_INVALID_INPUT);
    CHECK(read_str(&invalid, &token) == LEX_INVALID_INPUT);
}

int main(void)
{
    test_invalid_arguments();
    test_string_at_end();
    test_string_sequence();
    test_unterminated_string();
    test_read_str();

    puts("All string tests passed");
    return EXIT_SUCCESS;
}

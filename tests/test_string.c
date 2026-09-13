#include "library.h"
#include <stdio.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: failed: %s\n", __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

typedef struct
{
    typetype_t type;
    const char *literal;
} expected_t;

static void check_literal(const token_t *token, typetype_t type, const char *text)
{
    CHECK(token->type == type);
    CHECK(token->data.c.len == strlen(text));
    CHECK(token->data.c.literal != NULL);
    CHECK(memcmp(token->data.c.literal, text, token->data.c.len) == 0);
}

static void check_input(const char *input, const expected_t *expected, size_t count)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer(input, &lexer) == LEX_OK);
    token_t token; // The lexer must not read the old output value.
    CHECK(next_token(lexer, &token) == LEX_OK);
    CHECK(token.type == TOKEN_STRING_FORMAT);
    CHECK(token.data.d.count == count);
    for (size_t i = 0; i < count; i++)
    {
        CHECK(token.data.d.t[i].type != TOKEN_STRING_FORMAT);
        check_literal(&token.data.d.t[i], expected[i].type, expected[i].literal);
    }
    CHECK(lexer->pos == strlen(input));
    free_token(&token);
    free_token(&token);
    CHECK(next_token(lexer, &token) == LEX_EOF);
    free_lexer(lexer);
}

static void test_text(void)
{
    const expected_t empty[] = {{TOKEN_STRING, ""}};
    check_input("\"\"", empty, 1);
    const expected_t plain[] = {{TOKEN_STRING, "hello world 123 ! _"}};
    check_input("\"hello world 123 ! _\"", plain, 1);
    const expected_t escapes[] = {{TOKEN_STRING, "a\\{\\}\\n\\r\\t\\\"\\\\z\\q"}};
    check_input("\"a\\{\\}\\n\\r\\t\\\"\\\\z\\q\"", escapes, 1);
    const expected_t last_escape[] = {{TOKEN_STRING, "\\n"}};
    check_input("\"\\n\"", last_escape, 1);
}

static void test_interpolation(void)
{
    const expected_t parts[] = {
        {TOKEN_STRING, "some str "}, {TOKEN_NUMBER, "1"},
        {TOKEN_PLUS, "+"}, {TOKEN_NUMBER, "1"},
        {TOKEN_STRING, " last str "}, {TOKEN_IDENT, "expr"},
    };
    check_input("\"some str {1+1} last str {expr}\"", parts, 6);
    const expected_t adjacent[] = {{TOKEN_IDENT, "a"}, {TOKEN_IDENT, "b"}};
    check_input("\"{a}{b}\"", adjacent, 2);
    check_input("\"{}\"", NULL, 0);
    const expected_t braces[] = {
        {TOKEN_LBRACE, "{"}, {TOKEN_NUMBER, "1"}, {TOKEN_RBRACE, "}"},
    };
    check_input("\"{{1}}\"", braces, 3);
}

static void test_nested_text(void)
{
    const expected_t parts[] = {
        {TOKEN_STRING, "outer "}, {TOKEN_STRING, "1 + 1 "},
        {TOKEN_NUMBER, "2"}, {TOKEN_PLUS, "+"}, {TOKEN_NUMBER, "2"},
        {TOKEN_STRING, " tail"},
    };
    check_input("\"outer { \"1 + 1 {2+2}\" } tail\"", parts, 6);
    const expected_t nested_plain[] = {
        {TOKEN_STRING, "hello\\nworld"}, {TOKEN_PLUS, "+"}, {TOKEN_IDENT, "name"},
    };
    check_input("\"{\"hello\\nworld\"+name}\"", nested_plain, 3);
    const expected_t deep[] = {{TOKEN_STRING, "a"}, {TOKEN_STRING, "b"},
                               {TOKEN_STRING, "c"}};
    check_input("\"a{\"b{\"c\"}\"}\"", deep, 3);
}

static void test_sequence(void)
{
    lexer_t *lexer = NULL;
    CHECK(new_lexer("123 \"one\"\"two\"42", &lexer) == LEX_OK);
    token_t token;
    CHECK(next_token(lexer, &token) == LEX_OK);
    check_literal(&token, TOKEN_NUMBER, "123");
    free_token(&token);
    const char *words[] = {"one", "two"};
    for (size_t i = 0; i < 2; i++)
    {
        CHECK(next_token(lexer, &token) == LEX_OK);
        CHECK(token.type == TOKEN_STRING_FORMAT);
        CHECK(token.data.d.count == 1);
        check_literal(&token.data.d.t[0], TOKEN_STRING, words[i]);
        free_token(&token);
    }
    CHECK(next_token(lexer, &token) == LEX_OK);
    check_literal(&token, TOKEN_NUMBER, "42");
    free_token(&token);
    CHECK(next_token(lexer, &token) == LEX_EOF);
    free_lexer(lexer);
}

static void test_growth(void)
{
    char input[512];
    size_t pos = 0;
    memcpy(input + pos, "\"{\"", 3);
    pos += 3;
    for (size_t i = 0; i < 100; i++)
    {
        memcpy(input + pos, "x{1}", 4);
        pos += 4;
    }
    memcpy(input + pos, "\"}tail\"", 7);
    pos += 7;
    input[pos] = '\0';
    lexer_t *lexer = NULL;
    CHECK(new_lexer(input, &lexer) == LEX_OK);
    token_t token;
    CHECK(read_str(lexer, &token) == LEX_OK);
    CHECK(token.data.d.count == 201);
    for (size_t i = 0; i < 100; i++)
    {
        check_literal(&token.data.d.t[i * 2], TOKEN_STRING, "x");
        check_literal(&token.data.d.t[i * 2 + 1], TOKEN_NUMBER, "1");
    }
    check_literal(&token.data.d.t[200], TOKEN_STRING, "tail");
    CHECK(lexer->pos == pos);
    free_token(&token);
    free_lexer(lexer);
}

static void test_errors(void)
{
    const struct { const char *input; lexer_error_t error; } cases[] = {
        {"\"", LEX_UNTERMINATED_STRING},
        {"\"hello", LEX_UNTERMINATED_STRING},
        {"\"abc\\", LEX_UNTERMINATED_STRING},
        {"\"{", LEX_UNTERMINATED_STRING},
        {"\"{1+1", LEX_UNTERMINATED_STRING},
        {"\"{1   ", LEX_UNTERMINATED_STRING},
        {"\"{\"nested", LEX_UNTERMINATED_STRING},
        {"\"{123abc}", LEX_INVALID_NUMBER},
        {"\"{@}", LEX_UNEXPECTED_CHAR},
    };
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; i++)
    {
        lexer_t *lexer = NULL;
        CHECK(new_lexer(cases[i].input, &lexer) == LEX_OK);
        token_t token;
        CHECK(next_token(lexer, &token) == cases[i].error);
        CHECK(token.type == TOKEN_ILEGALL);
        CHECK(lexer->pos <= strlen(cases[i].input));
        CHECK(token.data.c.len == lexer->pos);
        free_token(&token);
        free_lexer(lexer);
    }
    token_t token;
    lexer_t invalid = {0};
    CHECK(read_str(NULL, &token) == LEX_INVALID_INPUT);
    CHECK(read_str(&invalid, &token) == LEX_INVALID_INPUT);
    CHECK(read_str(&invalid, NULL) == LEX_INVALID_INPUT);
    free_token(NULL);
}

int main(void)
{
    test_text();
    test_interpolation();
    test_nested_text();
    test_sequence();
    test_growth();
    test_errors();
    puts("All string tests passed");
    return EXIT_SUCCESS;
}

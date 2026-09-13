#pragma once
#include <stdlib.h>
#include <string.h>

#define unlikely(x) __builtin_expect(!!(x), 0)

#define LEX_NULL_CHECK(x, ...)     \
    do                             \
    {                              \
        if (unlikely((x) == NULL)) \
        {                          \
            __VA_ARGS__;           \
        }                          \
    } while (0)

#define LEX_CURRENT_UCHAR(lexer) \
    ((unsigned char)((lexer)->data[(lexer)->pos]))

typedef enum
{
    LEX_OK = 0,
    LEX_EOF,
    LEX_INVALID_INPUT,       // lexer или token == NULL
    LEX_OUT_OF_MEMORY,       // если выделяешь память
    LEX_UNEXPECTED_CHAR,     // недопустимый символ
    LEX_UNTERMINATED_STRING, // строка без закрывающей кавычки
    LEX_INVALID_NUMBER       // неправильный формат числа
} lexer_error_t;

typedef enum
{
    TOKEN_EOF,
    TOKEN_ILEGALL,
    TOKEN_NUMBER,
    TOKEN_IDENT,
    TOKEN_STRING,
    TOKEN_STRING_FORMAT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_FOR,
    TOKEN_BANG,
    TOKEN_PLUS_PLUS,
    TOKEN_MINUS_MINUS,
    TOKEN_MULT_MULT,
    TOKEN_DIV_DIV,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_DIV,
    TOKEN_EQ,
    TOKEN_EQ_EQ,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
} typetype_t;

typedef struct
{
    size_t pos;
    char *data;
} lexer_t;

typedef struct token token_t;

struct token
{
    typetype_t type;

    union
    {
        double a;
        long long b;

        struct
        {
            const char *literal;
            size_t len;
        } c;

        struct
        {
            token_t *t;
            size_t count;
        } d;
    } data;
};

typedef struct
{
    const char *word;
    size_t len;
    typetype_t type;
} keyword_t;

#define KEYWORD(word, type) {word, sizeof(word) - 1, type}
static const keyword_t keywords[] = {
    KEYWORD("if", TOKEN_IF),
    KEYWORD("else", TOKEN_ELSE),
    KEYWORD("while", TOKEN_WHILE),
    KEYWORD("return", TOKEN_RETURN),
    KEYWORD("for", TOKEN_FOR),
};
#undef KEYWORD

typedef struct
{
    const char *word;
    size_t len;
    typetype_t type;
} symbol_t;

#define SYMBOL(word, type) {word, sizeof(word) - 1, type}
static const symbol_t symbols[] = {
    SYMBOL("++", TOKEN_PLUS_PLUS),
    SYMBOL("--", TOKEN_MINUS_MINUS),
    SYMBOL("**", TOKEN_MULT_MULT),
    SYMBOL("//", TOKEN_DIV_DIV),
    SYMBOL("==", TOKEN_EQ_EQ),
    SYMBOL("!", TOKEN_BANG),
    SYMBOL("+", TOKEN_PLUS),
    SYMBOL("-", TOKEN_MINUS),
    SYMBOL("*", TOKEN_MULT),
    SYMBOL("/", TOKEN_DIV),
    SYMBOL("=", TOKEN_EQ),
    SYMBOL("{", TOKEN_LBRACE),
    SYMBOL("}", TOKEN_RBRACE),
    SYMBOL("(", TOKEN_LPAREN),
    SYMBOL(")", TOKEN_RPAREN),
};
#undef SYMBOL

lexer_error_t new_lexer(const char *, lexer_t **);
lexer_error_t next_token(lexer_t *, token_t *);
lexer_error_t read_number(lexer_t *, token_t *);
lexer_error_t free_lexer(lexer_t *);
lexer_error_t read_ident(lexer_t *, token_t *);
// Produces a flat TOKEN_STRING_FORMAT array without boundary markers.
// Text (including raw escapes) uses TOKEN_STRING and borrows lexer->data.
// Nested strings append to that same array. Nesting is limited to 128 levels.
lexer_error_t read_str(lexer_t *, token_t *);
// Release a result before reusing its output variable; do not free text slices.
void free_token(token_t *);
lexer_error_t read_keyword(lexer_t *, token_t *);

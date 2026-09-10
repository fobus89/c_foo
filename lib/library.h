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
} typetype_t;

typedef struct
{
    size_t pos;
    char *data;
} lexer_t;

typedef struct
{
    typetype_t type;
    const char *literal;
    size_t len;
} token_t;

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
};
#undef SYMBOL

lexer_error_t new_lexer(const char *, lexer_t **);
lexer_error_t next_token(lexer_t *, token_t *);
lexer_error_t read_number(lexer_t *, token_t *);
lexer_error_t free_lexer(lexer_t *);
lexer_error_t read_ident(lexer_t *, token_t *);
lexer_error_t read_str(lexer_t *, token_t *);
lexer_error_t read_keyword(lexer_t *, token_t *);
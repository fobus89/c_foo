#ifndef C_FOO_LEXER_TOKEN_H
#define C_FOO_LEXER_TOKEN_H

#include "stdlib.h"

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
} token_type_t;

typedef struct token token_t;

struct token
{
    token_type_t type;

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

const char *token_type_to_string(token_type_t type);

#endif
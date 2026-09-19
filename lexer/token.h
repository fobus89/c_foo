#ifndef C_FOO_LEXER_TOKEN_H
#define C_FOO_LEXER_TOKEN_H

#include "stdlib.h"

typedef struct token token_t;
typedef enum token_type token_type_t;

enum token_type
{
    TOKEN_EOF = 0,
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
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_GT_EQ,
    TOKEN_LT_EQ,
    TOKEN_EQ_EQ,
    TOKEN_BANG_EQ,
    TOKEN_AMP_AMP,
    TOKEN_PIPE_PIPE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COUNT,
};

struct token
{
    token_type_t type;
    const char *literal;
    size_t literal_len;
};

const char *token_type_to_string(token_type_t type);

#endif
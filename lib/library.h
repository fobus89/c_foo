#pragma once
#include <stdlib.h>

typedef enum
{
    TOKEN_EOF,
    TOKEN_ILEGALL,
    TOKEN_NUMBER,
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

lexer_t *new_lexer(const char *input);
void free_lexer(lexer_t *lexer);
const token_t next_token(lexer_t *lexer);
const token_t read_number(lexer_t *lexer);
int read_ident(lexer_t *lexer, token_t *out);
int read_str(lexer_t *lexer, token_t *out);
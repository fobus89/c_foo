#ifndef C_FOO_LEXER_LIB_H
#define C_FOO_LEXER_LIB_H

#include "token.h"
#include "stdlib.h"

#define LEN(x) (sizeof(x) / sizeof((x)[0]))

#define LEX_CURRENT_UCHAR(lexer) ((unsigned char)((lexer)->data[(lexer)->pos]))

typedef struct
{
    const char *word;
    size_t len;
    token_type_t type;
} keyword_t;

typedef struct
{
    const char *word;
    size_t len;
    token_type_t type;
} symbol_t;

size_t lookup_keyword(const char *, keyword_t *);
size_t lookup_symbol(const char *literal, symbol_t *symbol);

#endif
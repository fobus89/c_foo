#ifndef C_FOO_LEXER_H
#define C_FOO_LEXER_H

#include "token.h"
#include <stdlib.h>

typedef enum
{
    LEX_OK = 0,
    LEX_EOF,
    LEX_INVALID_INPUT,   // lexer или token == NULL
    LEX_OUT_OF_MEMORY,   // если выделяешь память
    LEX_UNEXPECTED_CHAR, // недопустимый символ
} lexer_error_t;

typedef struct
{
    size_t pos;
    char *data;
} lexer_t;

const char *lex_err_to_string(lexer_error_t type);

lexer_error_t new_lexer(const char *, lexer_t **);
lexer_error_t next_token(lexer_t *, token_t *);
lexer_error_t read_number(lexer_t *, token_t *);
lexer_error_t free_lexer(lexer_t *);
lexer_error_t read_ident(lexer_t *, token_t *);

lexer_error_t read_str(lexer_t *, token_t *);
void free_token(token_t *);
lexer_error_t read_keyword(lexer_t *, token_t *);
#endif

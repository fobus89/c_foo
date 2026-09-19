#include "lexer.h"
#include "lexer_lib.h"
#include "string.h"
#include <ctype.h>

const char *lex_err_to_string(lexer_error_t type)
{
#define LEX_ERR_CASE(type) \
    case type:             \
        return #type

    switch (type)
    {
        LEX_ERR_CASE(LEX_OK);
        LEX_ERR_CASE(LEX_EOF);
        LEX_ERR_CASE(LEX_OUT_OF_MEMORY);
        LEX_ERR_CASE(LEX_INVALID_INPUT);
        LEX_ERR_CASE(LEX_UNEXPECTED_CHAR);
    default:
        return "LEX_UNKNOWN";
    }
#undef LEX_ERR_CASE
}

lexer_error_t new_lexer(const char *input, lexer_t **lexer)
{

    if (lexer == NULL)
    {
        return LEX_INVALID_INPUT;
    }

    *lexer = NULL;

    if (input == NULL)
    {
        return LEX_INVALID_INPUT;
    }

    lexer_t *lex = malloc(sizeof(*lex));
    if (lex == NULL)
    {
        return LEX_OUT_OF_MEMORY;
    }

    lex->data = malloc(strlen(input) + 1);
    if (lex->data == NULL)
    {
        free(lex);
        return LEX_OUT_OF_MEMORY;
    }

    lex->pos = 0;

    strcpy(lex->data, input);

    *lexer = lex;

    return LEX_OK;
}

lexer_error_t free_lexer(lexer_t *lexer)
{

    if (lexer == NULL)
    {
        return LEX_INVALID_INPUT;
    }

    if (lexer->data != NULL)
        free(lexer->data);

    free(lexer);

    return LEX_OK;
}

lexer_error_t next_token(lexer_t *lexer, token_t *token)
{
    while (isspace(LEX_CURRENT_UCHAR(lexer)))
    {
        lexer->pos++;
    }

    if (LEX_CURRENT_UCHAR(lexer) == '\0')
    {
        *token = (token_t){
            .type = TOKEN_EOF,
        };

        return LEX_EOF;
    }

    if (isdigit(LEX_CURRENT_UCHAR(lexer)))
    {
        return read_number(lexer, token);
    }

    if (LEX_CURRENT_UCHAR(lexer) == '"')
    {
        return read_str(lexer, token);
    }

    return read_keyword(lexer, token);
}

lexer_error_t read_number(lexer_t *lexer, token_t *token)
{
    size_t start = lexer->pos;

    while (isdigit(LEX_CURRENT_UCHAR(lexer)))
    {
        lexer->pos++;
    }

    if (isspace(LEX_CURRENT_UCHAR(lexer)) ||
        ispunct(LEX_CURRENT_UCHAR(lexer)) ||
        LEX_CURRENT_UCHAR(lexer) == '\0')
    {
        *token = (token_t){
            .type = TOKEN_NUMBER,
            .literal_len = lexer->pos - start,
            .literal = lexer->data + start,
        };

        return LEX_OK;
    }

    while (!isspace(LEX_CURRENT_UCHAR(lexer)) &&
           LEX_CURRENT_UCHAR(lexer) != '\0')
    {
        lexer->pos++;
    }

    *token = (token_t){
        .type = TOKEN_ILEGALL,
        .literal_len = lexer->pos - start,
        .literal = lexer->data + start,
    };

    return LEX_OK;
}

lexer_error_t read_ident(lexer_t *lexer, token_t *token)
{
    size_t start = lexer->pos;

    if (LEX_CURRENT_UCHAR(lexer) != '_' &&
        !isalpha(LEX_CURRENT_UCHAR(lexer)))
    {
        *token = (token_t){
            .type = TOKEN_ILEGALL,
        };

        return LEX_OK;
    }

    while (
        isalnum(LEX_CURRENT_UCHAR(lexer)) ||
        LEX_CURRENT_UCHAR(lexer) == '_')
    {
        lexer->pos++;
    }

    *token = (token_t){
        .type = TOKEN_IDENT,
        .literal = lexer->data + start,
        .literal_len = lexer->pos - start,
    };

    return LEX_OK;
}

lexer_error_t read_keyword(lexer_t *lexer, token_t *token)
{
    size_t start = lexer->pos;

    symbol_t symbol;

    if (lookup_symbol(lexer->data + lexer->pos, &symbol) == 0)
    {
        lexer->pos += symbol.len;

        *token = (token_t){
            .type = symbol.type,
            .literal = lexer->data + start,
            .literal_len = lexer->pos - start,
        };

        return LEX_OK;
    }

    keyword_t keyword;

    if (lookup_keyword(lexer->data + lexer->pos, &keyword) != 0)
    {
        return read_ident(lexer, token);
    }

    lexer->pos += keyword.len;

    if (isalnum(LEX_CURRENT_UCHAR(lexer)) ||
        LEX_CURRENT_UCHAR(lexer) == '_')
    {
        lexer->pos -= keyword.len;
        return read_ident(lexer, token);
    }

    *token = (token_t){
        .type = keyword.type,
        .literal = lexer->data + start,
        .literal_len = lexer->pos - start,
    };

    return LEX_OK;
}

lexer_error_t read_str(lexer_t *lexer, token_t *token)
{
    size_t start = lexer->pos;

    return LEX_OK;
}
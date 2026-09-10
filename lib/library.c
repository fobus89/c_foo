#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static typetype_t ident_type(const char *literal, size_t len)
{
  for (size_t i = 0; i < sizeof keywords / sizeof keywords[0]; i++)
  {
    if (keywords[i].len == len &&
        memcmp(keywords[i].word, literal, len) == 0)
    {
      return keywords[i].type;
    }
  }

  return TOKEN_IDENT;
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

  if (lexer == NULL || token == NULL)
  {
    return LEX_INVALID_INPUT;
  }

  while (isspace((unsigned char)lexer->data[lexer->pos]))
  {
    lexer->pos++;
  }

  unsigned char ch = (unsigned char)lexer->data[lexer->pos];

  if (ch == '\0')
  {
    *token = (token_t){
        .len = 0,
        .type = TOKEN_EOF,
    };

    return LEX_EOF;
  }

  if (isdigit(ch))
  {
    return read_number(lexer, token);
  }

  if (ch == '"')
  {
    return read_str(lexer, token);
  }

  return read_keyword(lexer, token);
}

lexer_error_t read_number(lexer_t *lexer, token_t *token)
{

  if (lexer == NULL || token == NULL)
    return LEX_INVALID_INPUT;

  if (lexer->data == NULL)
    return LEX_INVALID_INPUT;

  size_t start = lexer->pos;

  if (!isdigit((unsigned char)lexer->data[start]))
    return LEX_INVALID_NUMBER;

  while (isdigit(LEX_CURRENT_UCHAR(lexer)))
    lexer->pos++;

  if (isspace(LEX_CURRENT_UCHAR(lexer)) || LEX_CURRENT_UCHAR(lexer) == '\0')
  {
    *token = (token_t){
        .type = TOKEN_NUMBER,
        .literal = lexer->data + start,
        .len = lexer->pos - start,
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
      .literal = lexer->data + start,
      .len = lexer->pos - start,
  };

  return LEX_INVALID_NUMBER;
}

lexer_error_t read_ident(lexer_t *lexer, token_t *token)
{
  if (lexer == NULL || token == NULL)
    return LEX_INVALID_INPUT;

  if (lexer->data == NULL)
    return LEX_INVALID_INPUT;

  size_t start = lexer->pos;

  unsigned char ch = (unsigned char)lexer->data[start];

  if (!isalpha(ch) && ch != '_')
    return LEX_UNEXPECTED_CHAR;

  while (isalnum(LEX_CURRENT_UCHAR(lexer)) ||
         LEX_CURRENT_UCHAR(lexer) == '_')
    lexer->pos++;

  *token = (token_t){
      .type = TOKEN_IDENT,
      .literal = lexer->data + start,
      .len = lexer->pos - start,
  };

  return LEX_OK;
}

lexer_error_t read_str(lexer_t *lexer, token_t *token)
{
  if (lexer == NULL || token == NULL)
    return LEX_INVALID_INPUT;

  if (lexer->data == NULL)
    return LEX_INVALID_INPUT;

  if (LEX_CURRENT_UCHAR(lexer) != '"')
    return LEX_UNEXPECTED_CHAR;

  size_t start = lexer->pos;
  lexer->pos++;

  while (lexer->data[lexer->pos] != '"' &&
         lexer->data[lexer->pos] != '\0')
  {
    lexer->pos++;
  }

  if (lexer->data[lexer->pos] != '"')
  {
    *token = (token_t){
        .type = TOKEN_ILEGALL,
        .literal = lexer->data + start,
        .len = lexer->pos - start,
    };

    return LEX_UNTERMINATED_STRING;
  }

  lexer->pos++;

  *token = (token_t){
      .type = TOKEN_STRING,
      .literal = lexer->data + start,
      .len = lexer->pos - start,
  };

  return LEX_OK;
}

lexer_error_t read_keyword(lexer_t *lexer, token_t *token)
{

  for (size_t i = 0; i < sizeof(symbols) / sizeof(symbols[0]); i++)
  {
    size_t len = strlen(symbols[i].word);

    if (strncmp(lexer->data + lexer->pos, symbols[i].word, len) == 0)
    {
      *token = (token_t){
          .type = symbols[i].type,
          .literal = lexer->data + lexer->pos,
          .len = len,
      };

      lexer->pos += len;
      return LEX_OK;
    }
  }

  lexer_error_t error = read_ident(lexer, token);
  if (error != LEX_OK)
    return error;

  token->type = ident_type(token->literal, token->len);
  return LEX_OK;
}
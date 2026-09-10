#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

lexer_t *new_lexer(const char *input)
{

  if (input == NULL)
  {
    return NULL;
  }

  lexer_t *lex = (lexer_t *)malloc(sizeof(lexer_t));
  if (lex == NULL)
  {
    return NULL;
  }

  lex->data = malloc(strlen(input) + 1);
  if (lex->data == NULL)
  {
    free_lexer(lex);
    return NULL;
  }

  lex->pos = 0;
  strcpy(lex->data, input);

  return lex;
}

void free_lexer(lexer_t *lexer)
{

  if (lexer == NULL)
  {
    return;
  }

  if (lexer->data != NULL)
    free(lexer->data);

  free(lexer);
}

const token_t next_token(lexer_t *lexer)
{

  while (isspace((unsigned char)lexer->data[lexer->pos]))
  {
    lexer->pos++;
  }

  char ch = lexer->data[lexer->pos];

  if (ch == '\0')
    return (token_t){
        .len = 0,
        .type = TOKEN_EOF,
    };

  if (ch >= '0' || ch <= '9')
  {
    return read_number(lexer);
  }

  return (token_t){
      .len = 0,
      .type = TOKEN_ILEGALL,
  };
}

const token_t read_number(lexer_t *lexer)
{
  size_t start = lexer->pos;

  char ch;

  while ((ch = lexer->data[lexer->pos]) >= '0' && ch <= '9')
  {
    lexer->pos++;
  }

  return (token_t){
      .type = TOKEN_NUMBER,
      .literal = lexer->data + start,
      .len = lexer->pos - start,
  };
}

int read_ident(lexer_t *lexer, token_t *out)
{
}

int read_str(lexer_t *lexer, token_t *out)
{
}
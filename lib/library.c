#include "library.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

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

  if (isspace(LEX_CURRENT_UCHAR(lexer)) ||
      ispunct(LEX_CURRENT_UCHAR(lexer)) ||
      LEX_CURRENT_UCHAR(lexer) == '\0')
  {
    *token = (token_t){
        .type = TOKEN_NUMBER,
        .data.c = {
            .literal = lexer->data + start,
            .len = lexer->pos - start,
        },
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
      .data.c = {
          .literal = lexer->data + start,
          .len = lexer->pos - start,
      },
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
      .data.c = {
          .literal = lexer->data + start,
          .len = lexer->pos - start,
      },
  };

  return LEX_OK;
}

typedef struct
{
  token_t *items;
  size_t count;
  size_t capacity;
} string_parts_t;

static lexer_error_t append_part(string_parts_t *parts, token_t part)
{
  if (parts->count == parts->capacity)
  {
    size_t max_capacity = SIZE_MAX / sizeof *parts->items;
    size_t capacity = parts->capacity;
    if (capacity == max_capacity)
      return LEX_OUT_OF_MEMORY;
    capacity = capacity == 0 ? 24 :
        (capacity > max_capacity / 2 ? max_capacity : capacity * 2);

    token_t *items = realloc(parts->items, capacity * sizeof *items);
    if (items == NULL)
      return LEX_OUT_OF_MEMORY;
    parts->items = items;
    parts->capacity = capacity;
  }
  parts->items[parts->count++] = part;
  return LEX_OK;
}

static lexer_error_t append_text(lexer_t *lexer, string_parts_t *parts,
                                 size_t start)
{
  return append_part(parts, (token_t){
      .type = TOKEN_STRING,
      .data.c = {
          .literal = lexer->data + start,
          .len = lexer->pos - start,
      },
  });
}

static lexer_error_t read_str_parts(lexer_t *lexer, string_parts_t *parts,
                                    size_t depth);

static lexer_error_t read_interpolation(lexer_t *lexer, string_parts_t *parts,
                                        size_t depth)
{
  lexer->pos++; // Only skip the opening interpolation brace.
  size_t braces = 1;
  while (braces > 0)
  {
    while (isspace(LEX_CURRENT_UCHAR(lexer)))
      lexer->pos++;
    if (LEX_CURRENT_UCHAR(lexer) == '\0')
      return LEX_UNTERMINATED_STRING;

    lexer_error_t err;
    if (LEX_CURRENT_UCHAR(lexer) == '"')
    {
      // Nested string text goes into the same flat array, not through read_ident.
      err = read_str_parts(lexer, parts, depth + 1);
      if (err != LEX_OK)
        return err;
      continue;
    }

    token_t part;
    err = next_token(lexer, &part);
    if (err != LEX_OK)
      return err;

    if (part.type == TOKEN_LBRACE)
      braces++;
    else if (part.type == TOKEN_RBRACE && --braces == 0)
      break; // The outer interpolation brace is not a result token.

    err = append_part(parts, part);
    if (err != LEX_OK)
      return err;
  }
  return LEX_OK;
}

static lexer_error_t read_str_parts(lexer_t *lexer, string_parts_t *parts,
                                    size_t depth)
{
  // Reject excessive nesting rather than overflowing the C call stack.
  if (depth >= 128)
    return LEX_INVALID_INPUT;

  lexer->pos++; // Opening quote.
  size_t text_start = lexer->pos;
  bool has_interpolation = false;

  for (;;)
  {
    unsigned char ch = LEX_CURRENT_UCHAR(lexer);
    if (ch == '\0')
      return LEX_UNTERMINATED_STRING;

    // Keep raw escape bytes in the text slice. Escaped quotes/braces are text.
    if (ch == '\\' && lexer->data[lexer->pos + 1] != '\0' &&
        strchr("{}nrt\"\\", lexer->data[lexer->pos + 1]) != NULL)
    {
      lexer->pos += 2;
      continue;
    }

    if (ch == '"')
    {
      // A plain empty string still produces one TOKEN_STRING of length zero.
      if (lexer->pos > text_start || !has_interpolation)
      {
        lexer_error_t err = append_text(lexer, parts, text_start);
        if (err != LEX_OK)
          return err;
      }
      lexer->pos++;
      return LEX_OK;
    }

    if (ch == '{')
    {
      if (lexer->pos > text_start)
      {
        lexer_error_t err = append_text(lexer, parts, text_start);
        if (err != LEX_OK)
          return err;
      }
      has_interpolation = true;
      lexer_error_t err = read_interpolation(lexer, parts, depth);
      if (err != LEX_OK)
        return err;
      text_start = lexer->pos;
      continue;
    }

    lexer->pos++;
  }
}

void free_token(token_t *token)
{
  if (token == NULL)
    return;
  if (token->type == TOKEN_STRING_FORMAT)
    free(token->data.d.t);
  *token = (token_t){.type = TOKEN_EOF};
}

lexer_error_t read_str(lexer_t *lexer, token_t *token)
{
  if (token == NULL)
    return LEX_INVALID_INPUT;
  // The output may be uninitialized. Only the caller releases previous results.
  *token = (token_t){.type = TOKEN_ILEGALL};
  if (lexer == NULL || lexer->data == NULL)
    return LEX_INVALID_INPUT;
  if (LEX_CURRENT_UCHAR(lexer) != '"')
    return LEX_UNEXPECTED_CHAR;

  size_t start = lexer->pos;
  string_parts_t parts = {0};
  lexer_error_t err = read_str_parts(lexer, &parts, 0);
  if (err != LEX_OK)
  {
    free(parts.items);
    token->data.c.literal = lexer->data + start;
    token->data.c.len = lexer->pos - start;
    return err;
  }
  *token = (token_t){
      .type = TOKEN_STRING_FORMAT,
      .data.d = {.t = parts.items, .count = parts.count},
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
          .data.c = {
              .literal = lexer->data + lexer->pos,
              .len = len,
          }};

      lexer->pos += len;
      return LEX_OK;
    }
  }

  lexer_error_t error = read_ident(lexer, token);
  if (error != LEX_OK)
    return error;

  token->type = ident_type(
      token->data.c.literal,
      token->data.c.len);
  return LEX_OK;
}
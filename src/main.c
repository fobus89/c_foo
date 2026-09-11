#include "library.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TOKEN_TYPE_CASE(type) \
  case type:                  \
    return #type

static inline const char *token_type_to_string(typetype_t type)
{
  switch (type)
  {
    TOKEN_TYPE_CASE(TOKEN_EOF);
    TOKEN_TYPE_CASE(TOKEN_ILEGALL);
    TOKEN_TYPE_CASE(TOKEN_NUMBER);
    TOKEN_TYPE_CASE(TOKEN_IDENT);
    TOKEN_TYPE_CASE(TOKEN_STRING);
    TOKEN_TYPE_CASE(TOKEN_IF);
    TOKEN_TYPE_CASE(TOKEN_ELSE);
    TOKEN_TYPE_CASE(TOKEN_WHILE);
    TOKEN_TYPE_CASE(TOKEN_RETURN);
    TOKEN_TYPE_CASE(TOKEN_FOR);
    TOKEN_TYPE_CASE(TOKEN_BANG);
    TOKEN_TYPE_CASE(TOKEN_PLUS_PLUS);
    TOKEN_TYPE_CASE(TOKEN_MINUS_MINUS);
    TOKEN_TYPE_CASE(TOKEN_MULT_MULT);
    TOKEN_TYPE_CASE(TOKEN_DIV_DIV);
    TOKEN_TYPE_CASE(TOKEN_PLUS);
    TOKEN_TYPE_CASE(TOKEN_MINUS);
    TOKEN_TYPE_CASE(TOKEN_MULT);
    TOKEN_TYPE_CASE(TOKEN_DIV);
    TOKEN_TYPE_CASE(TOKEN_EQ);
    TOKEN_TYPE_CASE(TOKEN_EQ_EQ);
    TOKEN_TYPE_CASE(TOKEN_LBRACE);
    TOKEN_TYPE_CASE(TOKEN_RBRACE);
    TOKEN_TYPE_CASE(TOKEN_LPAREN);
    TOKEN_TYPE_CASE(TOKEN_RPAREN);
  default:
    return "TOKEN_UNKNOWN";
  }
}

#undef TOKEN_TYPE_CASE

int read_file(const char *name, char **out)
{
  FILE *file = fopen(name, "rb");

  if (file == NULL)
  {
    perror("fopen");
    return 1;
  }

  if (fseek(file, 0, SEEK_END) != 0)
  {
    fclose(file);
    return 1;
  }

  long size = ftell(file);
  if (size < 0 || fseek(file, 0, SEEK_SET) != 0)
  {
    fclose(file);
    return 1;
  }

  char *data = malloc((size_t)size + 1);
  if (data == NULL)
  {
    fclose(file);
    return 1;
  }

  size_t len = fread(data, 1, (size_t)size, file);
  if (ferror(file))
  {
    free(data);
    fclose(file);
    return 1;
  }

  data[len] = '\0';

  fclose(file);

  *out = data;

  return 0;
}

int main(void)
{

  char *data = NULL;

  if (read_file("test.txt", &data) != 0)
  {
    return 1;
  }

  lexer_t *lex;

  if (LEX_OK != new_lexer(data, &lex))
  {
    return 1;
  }

  token_t tok;

  while (next_token(lex, &tok) == LEX_OK)
  {
    // printf("%s -> %.*s\n", token_type_to_string(tok.type), (int)tok.len, tok.literal);
  }

  // printf()
  // memcmp()

  // printf("%.*s", (int)out->length, out->literal); // выведет 123
}

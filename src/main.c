#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    TOKEN_TYPE_CASE(TOKEN_STRING_FORMAT);
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

static void print_token(const token_t *tok)
{
  if (tok->type == TOKEN_STRING_FORMAT)
  {
    for (size_t i = 0; i < tok->data.d.count; i++)
      print_token(&tok->data.d.t[i]);

    return;
  }

  printf("%s -> '%.*s'\n", token_type_to_string(tok->type),
         (int)tok->data.c.len, tok->data.c.literal);
}

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

void add(int *restrict a, int *restrict b)
{
  *a += *b;
}

int main(void)
{

  float a = 5;
  int b = 0;

  memcpy(&b, &a, sizeof(int));

  printf("%f %d \n", a, b);

  return 0;
  char *data = NULL;

  if (read_file("test.txt", &data) != 0)
  {
    return 1;
  }

  lexer_t *lex = NULL;
  lexer_error_t err = new_lexer(data, &lex);
  free(data);
  if (err != LEX_OK)
    return EXIT_FAILURE;

  token_t tok = {0};
  while ((err = next_token(lex, &tok)) == LEX_OK)
  {
    print_token(&tok);
    free_token(&tok);
  }

  if (err != LEX_EOF)
    fprintf(stderr, "Lexer error %d at byte %zu\n", (int)err, lex->pos);
  free_token(&tok);
  free_lexer(lex);
  return err == LEX_EOF ? EXIT_SUCCESS : EXIT_FAILURE;
}

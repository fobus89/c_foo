#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <inttypes.h>

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

expr_t nud_int_literal(parser_t *p)
{
  token_t token = p->tokens[p->pos];
  p->pos++;

  int64_t number = 0;

  for (size_t i = 0; i < token.data.c.len; i++)
  {
    number = number * 10 + (token.data.c.literal[i] - '0');
  }

  char a[token.data.c.len + 1];
  memcpy(a, token.data.c.literal, token.data.c.len);
  a[token.data.c.len] = '\0';

  printf("%s\n", a);

  return (expr_t){
      .kind = VALUE_INT,
      .data = (void *)number,
  };
}

expr_t nud_ident_literal(parser_t *p)
{
  token_t *token = &p->tokens[p->pos];
  p->pos++;

  return (expr_t){
      .kind = VALUE_STRING,
      .data = (void *)token,
  };
}

int main(void)
{

  struct expr2 ex = {.data = 1, .eval = read_file};

  return 0;
  char *data = NULL;

  if (read_file("test.txt", &data) != 0)
  {
    return 1;
  }

  parser_t *parser = new_parser(data);
  free(data);

  nud_register(parser, TOKEN_NUMBER, nud_int_literal);
  nud_register(parser, TOKEN_IDENT, nud_ident_literal);

  expr_t expr;

  while ((expr = parse_stmt(parser)).kind != EXPR_INVALID)
  {

    switch (expr.kind)
    {
    case EXPR_NUMBER:
      printf("%" PRIdPTR "\n", (intptr_t)expr.data);
      break;
    case EXPR_IDENT:
      token_t *token = expr.data;
      printf("%.*s\n", (int)token->data.c.len, token->data.c.literal);
      break;
    }
  }

  // while ((err = next_token(lex, &tok)) == LEX_OK)
  // {

  //   printf("%s -> '%.*s'\n", token_type_to_string(tok.type),
  //          (int)tok.data.c.len, tok.data.c.literal);

  //   // print_token(&tok);
  //   // free_token(&tok);
  // }

  return EXIT_SUCCESS;
}

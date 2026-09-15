#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_token(token_t *)
{
}

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

int main(void)
{

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

    printf("%s -> '%.*s'\n", token_type_to_string(tok.type),
           (int)tok.data.c.len, tok.data.c.literal);

    // print_token(&tok);
    // free_token(&tok);
  }

  if (err != LEX_EOF)
    fprintf(stderr, "Lexer error %d at byte %zu %s\n", (int)err, lex->pos, lex_err_to_string(err));

  free_token(&tok);
  free_lexer(lex);
  return err == LEX_EOF ? EXIT_SUCCESS : EXIT_FAILURE;
}

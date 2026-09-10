#include "library.h"
#include <stdio.h>
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
    TOKEN_TYPE_CASE(TOKEN_IF);
    TOKEN_TYPE_CASE(TOKEN_ELSE);
    TOKEN_TYPE_CASE(TOKEN_WHILE);
    TOKEN_TYPE_CASE(TOKEN_RETURN);
    TOKEN_TYPE_CASE(TOKEN_BANG);
    TOKEN_TYPE_CASE(TOKEN_PLUS_PLUS);
    TOKEN_TYPE_CASE(TOKEN_MINUS_MINUS);
    TOKEN_TYPE_CASE(TOKEN_MULT_MULT);
    TOKEN_TYPE_CASE(TOKEN_DIV_DIV);
    TOKEN_TYPE_CASE(TOKEN_PLUS);
    TOKEN_TYPE_CASE(TOKEN_MINUS);
    TOKEN_TYPE_CASE(TOKEN_MULT);
    TOKEN_TYPE_CASE(TOKEN_DIV);
  default:
    return "TOKEN_UNKNOWN";
  }
}

#undef TOKEN_TYPE_CASE

int main(void)
{
  lexer_t *lex;

  if (LEX_OK != new_lexer("let! x = 123", &lex))
  {
    return 1;
  }

  token_t tok;

  while (next_token(lex, &tok) == LEX_OK)
  {
    printf("%s -> %.*s\n", token_type_to_string(tok.type), (int)tok.len, tok.literal);
  }

  // memcmp()

  // printf("%.*s", (int)out->length, out->literal); // выведет 123
}

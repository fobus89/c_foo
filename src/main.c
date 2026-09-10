#include "library.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
  lexer_t *lex = new_lexer("12    22 444 55");

  for (size_t i = 0; i < 6; i++)
  {
    token_t tok = next_token(lex);

    if (tok.type != TOKEN_EOF)
      printf("%.*s\n", (int)tok.len, tok.literal);
  }
  // memcmp()

  // printf("%.*s", (int)out->length, out->literal); // выведет 123
}

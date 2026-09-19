#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

typedef struct assign_expr
{
  char *name;
  expr_t expr;
} assign_expr_t;

typedef struct body_expr
{
  expr_t *body;
  size_t len;
} body_expr_t;

typedef struct if_expr
{
  expr_t then;
  expr_t then_body;
} if_expr_t;

typedef struct binary_expr
{
  token_type_t op;
  expr_t left;
  expr_t right;
} binary_expr_t;

value_t eval(expr_t expr);

static void print_eval(const expr_t expr)
{

  const value_t val = eval(expr);

  switch (val.type)
  {
  case VALUE_INT:
    printf("%" PRIdPTR "\n", (intptr_t)val.data);
    break;
  case VALUE_IDENT:
    const token_t *token = val.data;
    printf("%.*s\n", (int)token->literal_len, token->literal);
    break;
  default:
    break;
  }
}

static const value_t eval_body(const void *data)
{
  const body_expr_t *body = data;
  value_t result = {0};

  for (size_t i = 0; i < body->len; i++)
  {
    result = eval(body->body[i]);
  }

  return result;
}

static const value_t eval_if(const void *data)
{
  const if_expr_t *if_expr = data;

  value_t result = eval(if_expr->then);

  if (!result.data)
  {
    return (value_t){
        .type = VALUE_NONE,
    };
  }

  return eval(if_expr->then_body);
}

static const value_t eval_number(const void *data)
{
  return (value_t){
      .type = VALUE_INT,
      .data = data,
  };
}

static const value_t eval_ident(const void *data)
{
  return (value_t){
      .type = VALUE_IDENT,
      .data = data,
  };
}

static const value_t eval_binary(const void *data)
{
  const binary_expr_t *binary = data;

  const int64_t left = (int64_t)eval(binary->left).data;
  const int64_t right = (int64_t)eval(binary->right).data;

  int64_t result = 0;

  switch (binary->op)
  {
  case TOKEN_PLUS:
    result = left + right;
    break;
  case TOKEN_MINUS:
    result = left - right;
    break;
  case TOKEN_DIV:
    result = left / right;
    break;
  case TOKEN_MULT:
    result = left * right;
    break;
  case TOKEN_AMP_AMP:
    result = left && right;
    break;
  case TOKEN_PIPE_PIPE:
    result = left || right;
    break;
  case TOKEN_GT:
    result = left > right;
    break;
  case TOKEN_LT:
    result = left < right;
    break;
  case TOKEN_GT_EQ:
    result = left >= right;
    break;
  case TOKEN_LT_EQ:
    result = left <= right;
    break;
  case TOKEN_BANG_EQ:
    result = left != right;
    break;
  case TOKEN_EQ_EQ:
    result = left == right;
    break;
  default:
    printf("error op %c\n", binary->op);
    exit(EXIT_FAILURE);
    break;
  }

  return (value_t){
      .type = VALUE_INT,
      .data = (void *)result,
  };
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

expr_t nud_int_literal(parser_t *p)
{
  token_t token = p->tokens[p->pos];
  p->pos++;

  int64_t number = 0;

  for (size_t i = 0; i < token.literal_len; i++)
  {
    number = number * 10 + (token.literal[i] - '0');
  }

  return (expr_t){
      .eval = eval_number,
      .kind = EXPR_NUMBER,
      .data = (void *)number,
  };
}

expr_t nud_group(parser_t *p)
{

  {
    const token_t token = p->tokens[p->pos];
    if (token.type != TOKEN_LPAREN)
    {
      return (expr_t){
          .kind = EXPR_INVALID,
      };
    }
    p->pos++;
  }

  expr_t expr = parse_expr(p, LOWEST);

  {
    const token_t token = p->tokens[p->pos];
    if (token.type != TOKEN_RPAREN)
    {
      return (expr_t){
          .kind = EXPR_INVALID,
      };
    }
    p->pos++;
  }

  return expr;
}

expr_t nud_ident_literal(parser_t *p)
{
  const token_t *token = &p->tokens[p->pos];
  p->pos++;

  return (expr_t){
      .eval = eval_ident,
      .kind = EXPR_IDENT,
      .data = (void *)token,
  };
}

expr_t nud_body(parser_t *p)
{
  const token_t *token = &p->tokens[p->pos];
  p->pos++;

  expr_t *body_exprs = malloc(sizeof(expr_t) * 32);
  size_t body_len = 0;

  while (current(p) != TOKEN_RBRACE)
  {
    body_exprs[body_len++] = parse_expr(p, LOWEST);
  }

  p->pos++;

  body_expr_t *body = malloc(sizeof(body_expr_t));

  *body = (body_expr_t){
      .body = body_exprs,
      .len = body_len,
  };

  return (expr_t){
      .kind = EXPR_BODY,
      .data = body,
      .eval = eval_body,
  };
}

expr_t nud_if(parser_t *p)
{
  const token_t *token = &p->tokens[p->pos];
  p->pos++;

  expr_t cond = parse_expr(p, LOWEST);
  expr_t body = parse_expr(p, LOWEST);

  if_expr_t *if_expr = malloc(sizeof(if_expr_t));

  *if_expr = (if_expr_t){
      .then = cond,
      .then_body = body,
  };

  return (expr_t){
      .kind = EXPR_IF,
      .eval = eval_if,
      .data = if_expr,
  };
}

expr_t led_binary(parser_t *p, expr_t left, binding_power_t bp)
{
  const token_t token = p->tokens[p->pos];
  p->pos++;

  expr_t right = parse_expr(p, bp);

  binary_expr_t *binary = malloc(sizeof(binary_expr_t));

  *binary = (binary_expr_t){
      .op = token.type,
      .left = left,
      .right = right,
  };

  return (expr_t){
      .kind = EXPR_BINARY,
      .data = binary,
      .eval = eval_binary,
  };
}

value_t eval(expr_t expr)
{
  return expr.eval(expr.data);
}

int main(void)
{

  char *data = NULL;

  if (read_file("test.txt", &data) != 0)
  {
    return 1;
  }

  parser_t *parser = new_parser(data);
  free(data);

  nud_register(parser, TOKEN_NUMBER, nud_int_literal);
  nud_register(parser, TOKEN_IDENT, nud_ident_literal);
  nud_register(parser, TOKEN_LPAREN, nud_group);
  nud_register(parser, TOKEN_LBRACE, nud_body);
  nud_register(parser, TOKEN_IF, nud_if);

  // math
  led_register(parser, TOKEN_PLUS, ADDITIVE, led_binary);
  led_register(parser, TOKEN_MINUS, ADDITIVE, led_binary);
  led_register(parser, TOKEN_DIV, MUPTIPLICATIVE, led_binary);
  led_register(parser, TOKEN_MULT, MUPTIPLICATIVE, led_binary);
  // Comparison
  led_register(parser, TOKEN_GT, RELATIONAL, led_binary);
  led_register(parser, TOKEN_LT, RELATIONAL, led_binary);
  led_register(parser, TOKEN_GT_EQ, RELATIONAL, led_binary);
  led_register(parser, TOKEN_LT_EQ, RELATIONAL, led_binary);
  led_register(parser, TOKEN_BANG_EQ, RELATIONAL, led_binary);
  led_register(parser, TOKEN_EQ_EQ, RELATIONAL, led_binary);
  // Logical
  led_register(parser, TOKEN_AMP_AMP, LOGICAL, led_binary);
  led_register(parser, TOKEN_PIPE_PIPE, LOGICAL, led_binary);
  led_register(parser, TOKEN_AND, LOGICAL, led_binary);
  led_register(parser, TOKEN_OR, LOGICAL, led_binary);

  expr_t expr;

  while ((expr = parse_stmt(parser)).kind != EXPR_INVALID)
  {
    print_eval(expr);
  }

  // printf("%d\n", expr.kind);

  return EXIT_SUCCESS;
}

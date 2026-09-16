#include "lexer.h"

typedef struct expr expr_t;
typedef struct parser parser_t;
typedef enum binding_power binding_power_t;

typedef expr_t (*stmt_handler_type)(parser_t *);
typedef expr_t (*nud_handler_type)(parser_t *);
typedef expr_t (*led_handler_type)(parser_t *, expr_t, binding_power_t);

enum binding_power
{
    BP_NONE = 0,
    LOWEST,
    COMMA,
    ASSIGMENT,
    LOGICAL,
    RELATIONAL,
    ADDITIVE,
    MUPTIPLICATIVE,
    UNARY,
    CALL,
    MEMBER,
    PRIMARY,
    HIGHEST,
};

typedef enum expr_kind
{
    EXPR_INVALID,
    EXPR_NUMBER,
    EXPR_IDENT,
    EXPR_UNARY,
    EXPR_BINARY,
} expr_kind_t;

struct expr
{
    expr_kind_t kind;
    void *data;
};

struct parser
{
    lexer_t *lexer;
    size_t pos;
    size_t count;
    token_t *tokes;
    nud_handler_type nud[TOKEN_COUNT];
    led_handler_type led[TOKEN_COUNT];
    stmt_handler_type stmt[TOKEN_COUNT];
    binding_power_t bp[TOKEN_COUNT];
};

parser_t *new_parser(const char *input);

expr_t *parse_number(parser_t *);
expr_t *parse_binary(parser_t *);
expr_t *parse_unary(parser_t *);

void nud_register(parser_t *p, token_type_t t, nud_handler_type h);
void led_register(parser_t *p, token_type_t t, binding_power_t bp, led_handler_type h);

void stmt_register(parser_t *p, token_type_t t, stmt_handler_type h);

stmt_handler_type stmt(parser_t *p, token_type_t t);
nud_handler_type nud(parser_t *p, token_type_t t);
led_handler_type led(parser_t *p, token_type_t t);
binding_power_t bp(parser_t *p, token_type_t t);

expr_t parse_stmt(parser_t *p);
expr_t parse_expr(parser_t *p, binding_power_t bp);

token_type_t current(parser_t *);
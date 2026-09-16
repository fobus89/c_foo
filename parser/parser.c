#include "parser.h"
#include <stdint.h>

typedef struct binary_expr
{
    token_type_t op;
    expr_t *left;
    expr_t *right;
} binary_expr_t;

typedef struct number_expr
{
    int64_t value;
} number_expr_t;

void nud_register(parser_t *p, token_type_t t, nud_handler_type h)
{
    p->nud[t] = h;
}

void stmt_register(parser_t *p, token_type_t t, stmt_handler_type h)
{
    p->bp[t] = LOWEST;
    p->stmt[t] = h;
}

void led_register(parser_t *p, token_type_t t, binding_power_t bp, led_handler_type h)
{
    p->bp[t] = bp;
    p->led[t] = h;
}

stmt_handler_type stmt(parser_t *p, token_type_t t)
{
    return p->stmt[t];
}

nud_handler_type nud(parser_t *p, token_type_t t)
{
    return p->nud[t];
}

led_handler_type led(parser_t *p, token_type_t t)
{
    return p->led[t];
}

binding_power_t bp(parser_t *p, token_type_t t)
{
    return p->bp[t];
}

expr_t parse_stmt(parser_t *p)
{
    stmt_handler_type handler = p->stmt[current(p)];
    if (handler != NULL)
    {
        return handler(p);
    }

    return parse_expr(p, LOWEST);
}

expr_t parse_expr(parser_t *p, binding_power_t bp)
{
    token_type_t nud_tok = current(p);

    nud_handler_type nud_handler = p->nud[nud_tok];
    if (nud_handler == NULL)
    {
        return (expr_t){
            .kind = EXPR_INVALID,
        };
    }

    expr_t left = nud_handler(p);
    if (left.kind == EXPR_INVALID)
    {
        return left;
    }

    while (1)
    {

        token_type_t curr_tok = current(p);

        binding_power_t cur_bp = p->bp[curr_tok];
        if (cur_bp <= bp)
        {
            break;
        }

        led_handler_type handler = p->led[curr_tok];
        if (handler == NULL)
        {
            return (expr_t){
                .kind = EXPR_INVALID,
            };
        }

        left = handler(p, left, cur_bp);
        if (left.kind == EXPR_INVALID)
        {
            return left;
        }
    }

    return left;
}

const token_type_t current(parser_t *p)
{
    const token_t token = p->tokes[p->pos];

    return token.type;
}

parser_t *new_parser(const char *input)
{
    lexer_t *lex = NULL;

    new_lexer(input, &lex);

    token_t tok = {0};
    lexer_error_t err;

    token_t *tokes = malloc(sizeof(token_t) * 128);
    size_t count = 0;
    while ((err = next_token(lex, &tok)) == LEX_OK)
    {
        tokes[count++] = tok;
    }

    parser_t *parser = malloc(sizeof(parser_t));

    *parser = (parser_t){
        .lexer = lex,
        .pos = 0,
        .count = count,
        .bp = {0},
        .led = {0},
        .nud = {0},
        .stmt = {0},
        .tokes = tokes,
    };

    return parser;
}
#include "token.h"
#include "stdlib.h"
#include "string.h"
#include "lexer_lib.h"

size_t lookup_keyword(const char *literal, keyword_t *keyword)
{

#define KEYWORD(word, type)          \
    {                                \
        word, sizeof(word) - 1, type \
    }
    static const keyword_t keywords[] = {
        KEYWORD("if", TOKEN_IF),
        KEYWORD("else", TOKEN_ELSE),
        KEYWORD("while", TOKEN_WHILE),
        KEYWORD("return", TOKEN_RETURN),
        KEYWORD("for", TOKEN_FOR),
    };
#undef KEYWORD

    for (size_t i = 0; i < LEN(keywords); i++)
    {
        const keyword_t *tmp_keyword = &keywords[i];

        if (memcmp(tmp_keyword->word, literal, tmp_keyword->len) == 0)
        {
            *keyword = *tmp_keyword;
            return 0;
        }
    }

    return 1;
}

size_t lookup_symbol(const char *literal, symbol_t *symbol)
{

#define SYMBOL(word, type)           \
    {                                \
        word, sizeof(word) - 1, type \
    }
    static const symbol_t symbols[] = {
        SYMBOL("++", TOKEN_PLUS_PLUS),
        SYMBOL("--", TOKEN_MINUS_MINUS),
        SYMBOL("**", TOKEN_MULT_MULT),
        SYMBOL("//", TOKEN_DIV_DIV),
        SYMBOL("==", TOKEN_EQ_EQ),
        SYMBOL(">=", TOKEN_GT_EQ),
        SYMBOL("<=", TOKEN_LT_EQ),
        SYMBOL("!=", TOKEN_BANG_EQ),
        SYMBOL("&&", TOKEN_AMP_AMP),
        SYMBOL("||", TOKEN_PIPE_PIPE),
        SYMBOL("!", TOKEN_BANG),
        SYMBOL("+", TOKEN_PLUS),
        SYMBOL("-", TOKEN_MINUS),
        SYMBOL("*", TOKEN_MULT),
        SYMBOL("/", TOKEN_DIV),
        SYMBOL("=", TOKEN_EQ),
        SYMBOL("{", TOKEN_LBRACE),
        SYMBOL("}", TOKEN_RBRACE),
        SYMBOL("(", TOKEN_LPAREN),
        SYMBOL(")", TOKEN_RPAREN),
        SYMBOL(">", TOKEN_GT),
        SYMBOL("<", TOKEN_LT),
        SYMBOL("&", TOKEN_AND),
        SYMBOL("|", TOKEN_OR),
    };
#undef SYMBOL

    for (size_t i = 0; i < LEN(symbols); i++)
    {
        const symbol_t *tmp_symbol = &symbols[i];

        if (memcmp(tmp_symbol->word, literal, tmp_symbol->len) == 0)
        {
            *symbol = *tmp_symbol;
            return 0;
        }
    }

    return 1;
}
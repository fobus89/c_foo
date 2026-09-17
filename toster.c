#include <stdio.h>
#include <stdlib.h>

typedef struct expr expr_t;
typedef struct binary binary_t;
typedef struct number number_t;
typedef struct value value_t;

enum value_type
{
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING
};

struct value
{
    unsigned int ty;

    union
    {
        int i;
        float f;
        char *s;
    } u;
};

struct expr
{
    const void *data;
    value_t (*eval)(const void *data);
};

struct binary
{
    char op;
    expr_t left;
    expr_t right;
};

struct number
{
    int value;
};

static value_t evaluate(expr_t e)
{
    return e.eval(e.data);
}

static value_t eval_number(const void *data)
{
    const number_t *n = data;

    return (value_t){
        .ty = VALUE_INT,
        .u.i = n->value,
    };
}

#define UF(x) (((x).ty == VALUE_INT) ? (x).u.i : (x).u.f)

static value_t eval_binary(const void *data)
{
    const binary_t *b = data;

    const value_t left = evaluate(b->left);
    const value_t right = evaluate(b->right);

    const float l = UF(left);
    const float r = UF(right);

    float result = 0;

    switch (b->op)
    {
    case '+':
        result = l + r;
        break;
    case '-':
        result = l - r;
        break;
    case '*':
        result = l * r;
        break;
    case '/':
        if (r == 0)
        {
            fprintf(stderr, "Division by zero\n");
            exit(EXIT_FAILURE);
        }
        result = l / r;
        break;
    }

    return (value_t){
        .ty = VALUE_INT,
        .u.i = result,
    };
}

int main(void)
{
    const number_t one = {.value = 3};
    const number_t two = {.value = 3};
    const number_t three = {.value = 3};

    const binary_t mul = {
        .op = '+',
        .left = {.data = &two, .eval = eval_number},
        .right = {.data = &three, .eval = eval_number},
    };

    const binary_t add = {
        .op = '+',
        .left = {.data = &one, .eval = eval_number},
        .right = {.data = &mul, .eval = eval_binary},
    };

    const expr_t root = {.data = &add, .eval = eval_binary};
    const value_t result = evaluate(root);

    printf("%d\n", result.u.i);
}

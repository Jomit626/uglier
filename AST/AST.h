#include <stdlib.h>
typedef enum
{
    identifier,
    number,
    string,
    unary,
    biary,
    expr_sqr,
    expr_brace,
    addr_loc,
    func_call,
    statement_expr,
    statement_block,
    statement_if,
    statement_while,
    statement_do_while,
    statement_for,
    statement_return,
    statement_continue,
    statement_break,
    type,
    type_dec,
    variable_decare,
    type_braced,
    func_declare,
    func_define,
} struct_type;

struct string_data
{
    char *str;
    unsigned flag;
};

struct number_data
{
    char *number;
    unsigned flag;
};

struct identifier_data
{
    char *name;
    unsigned flag;
};

struct biary_data
{
    char *op;
    struct AST *op1, *op2;
};

struct unary_data
{
    char *op;
    struct AST *op1;
    unsigned flag;
};

struct statement_expr_data
{
    struct AST *expr;
};

struct expr_sqr_data
{
    struct AST *expr;
};

struct addr_loc_data
{
    struct AST *addr;
    struct AST *offset;
};

struct expr_brace_data
{
    struct AST *expr;
};

struct func_call_data
{
    struct AST *func;
    struct AST **para;
    size_t n;
};

struct statement_block_data
{
    struct AST **statements;
    int n;
};
struct statement_if_data
{
    struct AST *condiction;
    struct AST *statement_success;
    struct AST *statement_fail;
};
struct statement_while_data
{
    struct AST *condiction;
    struct AST *loop;
};
struct statement_do_while_data
{
    struct AST *action;
    struct AST *condiction;
};
struct statement_for_data
{
    struct AST *init, *cond, *update;
    struct AST *loop;
};
struct statement_return_data
{
    struct AST *expr;
};
#define TYPENAME_CHAR 1
#define TYPENAME_SHORT 2
#define TYPENAME_USHORT 3
#define TYPENAME_UNSIGNED 4
#define TYPENAME_INT 5
#define TYPENAME_UINT 6
#define TYPENAME_LONG 7
#define TYPENAME_ULONG 8
#define TYPENAME_LLONG 9
#define TYPENAME_ULLONG 10
#define TYPENAME_FLOAT 11
#define TYPENAME_DOUBLE 12
#define TYPENAME_LDOUBLE 13
#define TYPENAME_VOID 14

#define TYPENAME_PROCESS1(x)                      \
    do                                            \
    {                                             \
        token *tok_out = tokens[0];               \
        AST *node = AST_new_node(type);           \
        free(tokens[0]->data);                    \
        node->data.type_data.idx = TYPENAME_CHAR; \
        tok_out->data = node;                     \
    } while (0)

#define TYPENAME_PROCESS2(x)                      \
    do                                            \
    {                                             \
        token *tok_out = tokens[0];               \
        AST *node = AST_new_node(type);           \
        free(tokens[0]->data);                    \
        free(tokens[1]->data);                    \
        node->data.type_data.idx = TYPENAME_CHAR; \
        tok_out->data = node;                     \
    } while (0)

#define TYPENAME_PROCESS3(x)                      \
    do                                            \
    {                                             \
        token *tok_out = tokens[0];               \
        AST *node = AST_new_node(type);           \
        free(tokens[0]->data);                    \
        free(tokens[1]->data);                    \
        free(tokens[2]->data);                    \
        node->data.type_data.idx = TYPENAME_CHAR; \
        tok_out->data = node;                     \
    } while (0)

#define TYPE_DEC_AUTO (1 << 0)
#define TYPE_DEC_REGISTER (1 << 1)
#define TYPE_DEC_VOLATILE (1 << 2)
#define TYPE_DEC_CONST (1 << 3)
#define TYPE_DEC_STATIC (1 << 4)
#define TYPE_DEC_EXTERN (1 << 5)

struct type_data
{
    unsigned idx;
};
struct type_dec_data
{
    unsigned flag;
};
struct variable_decare_data
{
    unsigned flag;
    unsigned type_id;
    struct AST *expr;
};

struct func_data
{
    struct AST *statement;
    struct AST *return_type;
    struct AST **para;
    int n;
};

#define DATA(name) struct name##_data name##_data;

typedef struct AST
{
    struct_type type;
    union {
        DATA(identifier);
        DATA(number);
        DATA(string);
        DATA(unary);
        DATA(biary);
        DATA(expr_sqr);
        DATA(expr_brace);
        DATA(addr_loc);
        DATA(func_call);
        DATA(statement_expr);
        DATA(statement_block);
        DATA(statement_if);
        DATA(statement_while);
        DATA(statement_do_while);
        DATA(statement_for);
        DATA(statement_return);
        DATA(type);
        DATA(type_dec);
        DATA(variable_decare);
        DATA(func);
    } data;

} AST;

AST *AST_new_node(struct_type type);
AST **AST_new_child_list(size_t n);
void AST_free(AST *n);
void AST_print(AST *n);

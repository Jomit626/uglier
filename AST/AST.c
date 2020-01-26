#include <stdlib.h>

#include "AST.h"

#include "../parse/parse_gen.h"

#define BLANK_BUFF_SIZE 2048
hash_table *tab;
char *blanks;
#define PRINT_N_BLANK(n__,...) do{printf("%s",blanks + BLANK_BUFF_SIZE - 1 - (n__));printf(__VA_ARGS__);} while(0);
void AST_init(){
    tab = create_hash_table(1024);
    blanks = (char*)malloc(sizeof(char)*BLANK_BUFF_SIZE);
    for(int i=0;i<BLANK_BUFF_SIZE-1;i++){
        blanks[i] = ' ';
    }
    blanks[BLANK_BUFF_SIZE-1] = 0;
}
AST * AST_new_node(struct_type type){
    AST * n = (AST*)malloc(sizeof(AST));
    n->type = type;
    return n;
}
AST** AST_new_child_list(size_t n){
    return (AST**)malloc(sizeof(AST)*n);
}

void AST_free(AST *n) {
    switch (n->type)
    {
    //case /* constant-expression */:
        /* code */
        break;
    
    default:
        break;
    }

    free(n);
}
void AST_print0(AST *n, int depth){
switch (n->type)
{
case identifier:
    PRINT_N_BLANK(depth*2,"identifier : %s\n", n->data.identifier_data.name);
    break;
case number:
    PRINT_N_BLANK(depth*2,"number : %s\n", n->data.number_data.number);
    break;
case string:
    PRINT_N_BLANK(depth*2,"string : %s\n", n->data.string_data.str);
    break;
case biary:
    PRINT_N_BLANK(depth*2,"biary op : %s\n", n->data.biary_data.op);
    PRINT_N_BLANK(depth*2,"op1 : \n");AST_print0(n->data.biary_data.op1,depth+1);
    PRINT_N_BLANK(depth*2,"op2 : \n");AST_print0(n->data.biary_data.op2,depth+1);
    break;
case unary:
    PRINT_N_BLANK(depth*2,"unary op : %s\n", n->data.unary_data.op);
    PRINT_N_BLANK(depth*2,"op1 : \n");AST_print0(n->data.unary_data.op1,depth+1);
    break;
case statement_expr:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case expr_sqr:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case expr_brace:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case addr_loc:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case func_call:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_block:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_if:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_while:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_do_while:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_for:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_return:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_continue:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case statement_break:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case type:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case type_dec:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case variable_decare:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case type_braced:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case func_declare:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
case func_define:
    PRINT_N_BLANK(depth*2,"statement : \n");AST_print0(n->data.statement_expr_data.expr,depth+1);
    break;
default:
    break;
}
}

void AST_print(AST *n){
    AST_init();
    AST_print0(n,0);
}
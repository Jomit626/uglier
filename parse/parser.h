#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED
#include "token.h"
#include "../lex/lex_anal.h"

#include "../utilities/utility.h"

// Graph

#define LEFT_ASSOCIATIVE 1
#define RIGHT_ASSOCIATIVE 0
#define DEFAULT_ASSOCIATIVITY 0

typedef unsigned int priority;
typedef enum {
    none = DEFAULT_ASSOCIATIVITY,
    right = RIGHT_ASSOCIATIVE,
    left = LEFT_ASSOCIATIVE,
} associativity;

typedef struct parse_edge
{
    int token;
    int next;
    int tail;
    int head;
    priority p;
    associativity a;
} parse_edge;

// remember to rewrite parse_gen.c:parse_gen_dump() after changer this structure
typedef struct parse_rule
{
    int out_token;  
    int end_status;
    void(*func)(struct parse_rule*,token **,int);
} parse_rule;
// this one too
typedef void(*rule_func)(struct parse_rule*,token **,int);

typedef struct parse_state
{
    int out_rule;
    int first_edge;
}parse_state;


typedef struct runtime
{
    parse_state *cur_state;
    parse_edge *pre_edge;
    int length;
}parse_runtime;


typedef struct parse_env
{
    parse_runtime *runtime;

    parse_state *states;
    parse_edge *edges;
    parse_rule *rules;
    hash_table *token_map;

    parse_runtime *runtime_stack;

    token* *token_stack;
    int token_top;

    int tokens_cnt;
    token *tokens_head;
    token *tokens_tail;

    lex_env *lex_env;
    int reach_end_of_file;
}parse_env;

parse_env* parser_new_env();
int parser_pase(parse_env* env);
#endif
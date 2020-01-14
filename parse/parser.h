#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED
#include "token.h"
#include "../lex/lex_anal.h"

#include "../utilities/utility.h"

#define LEFT_ASSOCIATIVE 1
#define RIGHT_ASSOCIATIVE 0
#define NONE_ASSOCIATIVE 0

// Graph

#define MAX_RULE_TOKEN 16
#define MAX_RULE_P 128

typedef struct parse_edge
{
    int end;
    int token;
    int next;
} parse_edge;

typedef struct priority {
    int token;
    char priority;
    char associative;
}parse_priority;

typedef struct token_info
{
    int size;
    parse_priority *ps;
} token_info;

typedef struct parse_rule
{
    int length;
    int tokens[MAX_RULE_TOKEN];
    int out_token;
    int end_status;
    void*(*func)(struct parse_rule*,void*);
    char associative; //0 none 1 left 2 right
    char priority;
} parse_rule;

typedef void*(*rule_func)(struct parse_rule*,void*);

typedef struct parse_node
{
    int first_edge;
    int rule;
    char associative;
    char priority;
}parse_node;



typedef struct parse_env
{
    int status;
    int pre_status;

    parse_node *nodes;
    parse_edge *edges;
    parse_rule *rules;
    token_info *token_info;
    hash_table *token_map;

    int *status_stack;
    int status_top;

    token* *token_stack;
    int token_top;

    int tokens_cnt;
    token *tokens_head;
    token *tokens_tail;

    lex_env *lex_env;
    int reach_end_of_file;
}parse_env;

parse_env* parser_new_env();
int parse(parse_env* env);
#endif
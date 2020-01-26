#include "parser.h"

int parse_gen_init();
void add_edge(int from, int to, int token, priority p, associativity a);
int parse_gen_add_rule(int token);
void parse_gen_rule_set_func(int idx, char* func);
int parse_gen_rule_add_token(int idx, int token, priority p, associativity a);

void parse_gen_rule_end_at(int idx, int state);
void parse_gen_analyse();
void parse_gen_assign_to_env(parse_env* env);

void parse_gen_dump(FILE *f);

void cat_token(parse_rule *r, token **tokens,int n);
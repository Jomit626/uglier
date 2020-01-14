#include "parser.h"

int parse_gen_init();

int parse_gen_add_rule(int token);
void parse_gen_rule_set_asscoiative(int idx, char a);
void parse_gen_rule_set_priority(int idx, char p);
void parse_gen_rule_set_func(int idx, char* func);
void parse_gen_rule_add_token(int idx, int token);

void parse_gen_analyse();
void parse_gen_assign_to_env(parse_env* env);

void parse_gen_dump(FILE *f);

void *cat_token(parse_rule *r, void *t);
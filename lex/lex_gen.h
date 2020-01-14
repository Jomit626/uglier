#include <stdio.h>

void lex_gen_init(int n);
lex_node* lex_gen_get_nodes();
int lex_gen_add_token(int token_id,const char* s);
void lex_gen_dump(FILE* f);
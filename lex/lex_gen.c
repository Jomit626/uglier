#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "./lex_anal.h"
#include "../utilities/utility.h"
static lex_node* nodes;
static int word_cnt;
static int state_cnt;

lex_node* lex_gen_get_nodes(){
    return nodes;
}

void lex_gen_init(int n){
    nodes = (lex_node*)calloc(n,sizeof(lex_node));

    word_cnt = 5;
    state_cnt = CHAR_MAX;
}

int lex_gen_add_token(int token_id,const char* s){
    char first_c = *s++;
    if(first_c <= 0)
        return 0;
    
    lex_node *parent = &nodes[first_c];
    parent->c = first_c;
    for(;*s;s++){
        char c = *s;
        int idx;
        for(idx = parent->first_child;idx>0&&nodes[idx].c!=c;idx =nodes[idx].next_silb);

        if(idx>0&&nodes[idx].c==c){
            parent = &nodes[idx];
        } else {
            state_cnt +=1;
            nodes[state_cnt].c = c;
            nodes[state_cnt].out = 0;
            nodes[state_cnt].first_child = 0;
            nodes[state_cnt].next_silb = parent->first_child;
            parent->first_child = state_cnt;

            parent = &nodes[state_cnt];
        }
    }
    word_cnt += 1;
    parent->out = token_id;

    return word_cnt;
}

void lex_gen_dump(FILE* f){
    fprintf(f,  "\n"
                "//*******************************************\n"
                "//Lex nodes\n"
                "//*******************************************\n"
                "#include \"./lex/lex_anal.h\"\n"
    );
    DUMP_OBJ(f,nodes,state_cnt);
}
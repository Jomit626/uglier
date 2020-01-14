#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "./lex_anal.h"

static lex_node* nodes;
static int word_cnt;
static int node_cnt;

lex_node* lex_gen_get_nodes(){
    return nodes;
}

void lex_gen_init(int n){
    nodes = (lex_node*)calloc(n,sizeof(lex_node));

    word_cnt = 5;
    node_cnt = CHAR_MAX;
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
            node_cnt +=1;
            nodes[node_cnt].c = c;
            nodes[node_cnt].out = 0;
            nodes[node_cnt].first_child = 0;
            nodes[node_cnt].next_silb = parent->first_child;
            parent->first_child = node_cnt;

            parent = &nodes[node_cnt];
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
                "\n"
                "lex_node lex_nodes[]={\n"
    );
    for(int i=0;i<=node_cnt;i++){
        fprintf(f,"{%4d,%4d,%4d,%4d},\n",nodes[i].c,nodes[i].out,nodes[i].first_child,nodes[i].next_silb);
    }
    fprintf(f,  "};\n\n");
}
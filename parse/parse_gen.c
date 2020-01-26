#include <stdint.h>

#include "../utilities/utility.h"
#include "parse_gen.h"

#define MAX_EDGE_NUM 1024
#define MAX_RULE_NUM 1024
#define MAX_NODE_NUM 1024
#define MAX_INFO_NUM 1024

static hash_table *token_map;
static parse_state *states;

static int state_cnt = 1; //

static parse_edge *edges;
static int edge_cnt;
static parse_rule *rules;
static char **rules_func_str;
static int rule_cnt;

void cat_token(parse_rule *r, token **tokens,int n)
{
    char buff[2048];
    buff[0] = 0;
    msg("cat %d\n",n);
    for (int i = 0; i < n; i++)
    {
        strcat(buff, (char *)tokens[i]->data);
        free(tokens[i]->data);
    }
    printf("%d:%s\n", r->out_token, buff);
    tokens[0]->data = SAVESTR(buff);
    return;
}

void parse_gen_assign_to_env(parse_env *env)
{
    env->states = states;
    env->token_map = token_map;
    env->edges = edges;
    env->rules = rules;
}
int parse_gen_init()
{
    token_map = create_hash_table(128);
    edges = (parse_edge *)calloc(MAX_EDGE_NUM, sizeof(parse_edge));
    rules = (parse_rule *)calloc(MAX_RULE_NUM, sizeof(parse_rule));
    states = (parse_state *)calloc(MAX_NODE_NUM, sizeof(parse_state));
    rules_func_str = (char **)calloc(MAX_RULE_NUM, sizeof(char *));
    state_cnt = 1;
    edge_cnt = 0;
    rule_cnt = 0;
    return 0;
}

int parse_gen_add_rule(int token)
{
    rule_cnt++;
    rules[rule_cnt].out_token = token;
    rules[rule_cnt].end_status = 1;
    rules[rule_cnt].func = cat_token; // TODO:
    rules_func_str[rule_cnt] = 0;
    return rule_cnt;
}

void parse_gen_rule_set_func(int idx, char *func)
{
    rules_func_str[idx] = func;
}
int parse_gen_rule_add_token(int idx, int token, priority p, associativity a)
{
    parse_rule *r = &rules[idx];
    parse_state *node = &states[r->end_status];
    int edge_idx = node->first_edge;
    for (; edge_idx > 0 && edges[edge_idx].token != token; edge_idx = edges[edge_idx].next)
        ;

    if (edge_idx > 0)
    {
        r->end_status = edges[edge_idx].head;
    }
    else
    {
        state_cnt++;
        add_edge(r->end_status, state_cnt, token, p, a);
        r->end_status = state_cnt;
    }
    return r->end_status;
}

void add_edge(int from, int to, int token, priority p, associativity a)
{
    edge_cnt++;
    edges[edge_cnt].tail = from;
    edges[edge_cnt].head = to;
    edges[edge_cnt].token = token;
    edges[edge_cnt].next = states[from].first_edge;
    edges[edge_cnt].p = p;
    edges[edge_cnt].a = a;
    states[from].first_edge = edge_cnt;
}

void parse_gen_rule_end_at(int idx, int state){
    if(states[state].out_rule)
        return; // TODO:
    else 
        states[state].out_rule = idx;
}

void parse_gen_dump(FILE *f){
    fprintf(f,  "\n"
                "//*******************************************\n"
                "//Parse data\n"
                "//Thid file\n"
                "//*******************************************\n"
                "#include \"./parse/parser.h\"\n"
                "#include \"./parse/parse_gen.h\"\n"
                "\n");
    int *func_id = (int *)malloc(sizeof(int)*rule_cnt);
    for(int i=0;i<=rule_cnt;i++){
        if(rules_func_str[i] == 0)
        {
            func_id[i] = -1;
        } else if (rules_func_str[i] == (void*)-1){
            
            for(int j=i-1;j>=0;j++){
                if(func_id[j] != -1){
                    func_id[i] = func_id[j];
                    break;
                }
            }
        } else{
            func_id[i] = i;
            char * s = rules_func_str[i];
            fprintf(f,  "void rule%d_func(parse_rule *r, token **tokens,int n)",i);
            fprintf(f,"%s",s);
            fprintf(f,"\n");
        }
    }
    DUMP_OBJ(f,states,state_cnt);
    //DUMP_OBJ(f,rules,rule_cnt);
    fprintf(f,"parse_rule rules[]={\n");
    for(int i=0;i<=rule_cnt;i++){
        if(func_id[i] != -1){
            fprintf(f,"{%d,%d,rule%d_func},\n",rules[i].out_token,rules[i].end_status,func_id[i]);
        }
        else {
            fprintf(f,"{%d,%d,cat_token},\n",rules[i].out_token,rules[i].end_status);
        }
    }
    fprintf(f,"};\n");

    DUMP_OBJ(f,edges,edge_cnt);
    free(func_id);
}
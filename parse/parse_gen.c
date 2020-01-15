#include "parse_gen.h"

#define MAX_EDGE_NUM 1024
#define MAX_RULE_NUM 1024
#define MAX_NODE_NUM 1024
#define MAX_INFO_NUM 1024

static hash_table *token_map;
static parse_state *nodes;

static int node_cnt = 1; //

static parse_edge *edges;
static int edge_cnt;
static parse_rule *rules;
static char **rules_func_str;
static int rule_cnt;
//static token_info *info;
//static int info_cnt;

void *cat_token(parse_rule *r, void *t,int l)
{
    int len = r->length;
    token **tokens = (token **)t;
    char buff[2048];
    buff[0] = 0;
    msg("cat %d\n",l);
    //strcat(buff, "(");
    for (int i = 0; i < l; i++)
    {

        strcat(buff, (char *)tokens[i]->data);

        //printf("token:{%d,%s} \n",tokens[i]->id,(char*)tokens[i]->data);
        free(tokens[i]->data);
        free(tokens[i]);
    }
    //strcat(buff, ")");
    printf("%d:%s\n", r->out_token, buff);
    return new_token(r->out_token, SAVESTR(buff), NULL);
}

void parse_gen_assign_to_env(parse_env *env)
{
    env->states = nodes;
    env->token_map = token_map;
    env->edges = edges;
    env->rules = rules;
    //env->token_info = info;
}
int parse_gen_init()
{
    token_map = create_hash_table(128);
    edges = (parse_edge *)calloc(MAX_EDGE_NUM, sizeof(parse_edge));
    rules = (parse_rule *)calloc(MAX_RULE_NUM, sizeof(parse_rule));
    nodes = (parse_state *)calloc(MAX_NODE_NUM, sizeof(parse_state));
    //info = (token_info*)calloc(MAX_NODE_NUM,sizeof(token_info));
    rules_func_str = (char **)calloc(MAX_RULE_NUM, sizeof(char *));
    node_cnt = 1;
    edge_cnt = 0;
    rule_cnt = 0;
    //info_cnt = MAX_NODE_NUM;
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
//static void info_add(int idx,int tok,char p,char a){
//    token_info *inf = &info[idx];
//    if(inf->size < 0){
//        inf->size = 0;
//        inf->ps = (parse_priority*)malloc(sizeof(parse_priority)*1280);
//    }
//    inf->ps[inf->size].token = tok;
//    inf->ps[inf->size].associative = a;
//    inf->ps[inf->size++].priority = p;
//}
//void parse_gen_analyse()
//{
//    for (int i = 1; i <= rule_cnt; i++)
//    {
//        parse_rule *r = &rules[i];
//        int r_tail_token_idx = r->length - 1;
//        int r_tail_token = r->tokens[r_tail_token_idx];
//        int r_head_token = r->tokens[0];
//        int r_out_token = r->out_token;
//
//        nodes[r->end_status].rule = i;
//
//        if(r_tail_token == r_head_token && r_head_token == r_out_token){
//            info[r_tail_token].size = -1;
//        }
//    }
//
//    for(int i=0;i<=rule_cnt;i++){
//        parse_rule *r = &rules[i];
//        int len = r->length - 1;
//        for(int j=0;j<len;j++){
//            int tok = r->tokens[j];
//            if(info[tok].size == 0)
//                continue;
//            j++;
//            int nxt_tok = r->tokens[j];
//            info_add(tok,nxt_tok,r->priority,r->associative);
//        }
//        int out = r->out_token;
//        if(info[out].size != 0){
//            info_add(out,r->tokens[0],r->priority,r->associative);
//        }
//    }
//}
//void parse_gen_rule_set_asscoiative(int idx, char a)
//{
//    rules[idx].associative = a;
//}
//void parse_gen_rule_set_priority(int idx, char p)
//{
//    rules[idx].priority = p;
//}
void parse_gen_rule_set_func(int idx, char *func)
{
    rules_func_str[idx] = func;
}
int parse_gen_rule_add_token(int idx, int token, priority p, associativity a)
{
    parse_rule *r = &rules[idx];
    if (r->length > MAX_RULE_TOKEN)
        return 0; // TODO:
    r->tokens[r->length] = token;
    r->length++;
    parse_state *node = &nodes[r->end_status];
    int edge_idx = node->first_edge;
    for (; edge_idx > 0 && edges[edge_idx].token != token; edge_idx = edges[edge_idx].next)
        ;

    if (edge_idx > 0)
    {
        r->end_status = edges[edge_idx].tail;
    }
    else
    {
        node_cnt++;
        add_edge(r->end_status, node_cnt, token, p, a);
        r->end_status = node_cnt;
    }
    return r->end_status;
}

void add_edge(int from, int to, int token, priority p, associativity a)
{
    edge_cnt++;
    edges[edge_cnt].head = from;
    edges[edge_cnt].tail = to;
    edges[edge_cnt].token = token;
    edges[edge_cnt].next = nodes[from].first_edge;
    edges[edge_cnt].p = p;
    edges[edge_cnt].a = a;
    nodes[from].first_edge = edge_cnt;
}

void parse_gen_rule_end_at(int idx, int state){
    parse_rule *r = &rules[idx];
    if(nodes[state].out_rule)
        return; // TODO:
    else 
        nodes[state].out_rule = idx;
}
//void parse_gen_dump(FILE *f){
//    fprintf(f,  "\n"
//                "//*******************************************\n"
//                "//Parse data\n"
//                "//*******************************************\n"
//                "#include \"./parse/parser.h\"\n"
//                "#include \"./parse/parse_gen.h\"\n"
//                "\n");
//    for(int i=0;i<=rule_cnt;i++){
//        if(!rules_func_str[i])
//            continue;
//        char * s = rules_func_str[i];
//        fprintf(f,  "void *rule%d_func(parse_rule *r, void *t)",i);
//        fprintf(f,"%s",s);
//        fprintf(f,"\n");
//    }
//
//     fprintf(f, "parse_node parse_nodes[]={\n"
//    );
//    for(int i=0;i<=node_cnt;i++){
//        fprintf(f,"{%d,%d,%d,%d},\n",nodes[i].first_edge,nodes[i].rule,nodes[i].associative,nodes[i].priority);
//    }
//    fprintf(f,  "};\n\n");
//
//    fprintf(f,"parse_edge parse_edges[]={\n");
//    for(int i=0;i<=edge_cnt;i++){
//        fprintf(f,"{%d,%d,%d},\n",edges[i].end,edges[i].token,edges[i].next);
//    }
//    fprintf(f,  "};\n\n");
//
//    fprintf(f,"parse_rule parse_rules[]={\n");
//    for(int i=0;i<=rule_cnt;i++){
//        fprintf(f,"{%d,{",rules[i].length);
//        for(int j=0;j<MAX_RULE_TOKEN;j++)
//            fprintf(f,"%d,",rules[i].tokens[j]);
//        if(rules_func_str[i])
//        fprintf(f,"},%d,%d,rule%d_func,%d,%d},\n",rules[i].out_token,rules[i].end_status, i,rules[i].associative,rules[i].priority);
//        else
//        fprintf(f,"},%d,%d,cat_token,%d,%d},\n",rules[i].out_token,rules[i].end_status,rules[i].associative,rules[i].priority);
//    }
//    fprintf(f,  "};\n\n");
//
//    fprintf(f,"token_info token_infos[%d];\n",info_cnt);
//    for(int i=0;i<=info_cnt;i++){
//        if(info[i].size==0)
//            continue;
//        fprintf(f,"parse_priority parse_priority%d[%d]={\n",i,info[i].size);
//        for(int j=0;j<info[i].size;j++){
//            fprintf(f,"{%d,%d,%d},\n",info[i].ps[j].token,info[i].ps[j].priority,info[i].ps[j].associative);
//        }
//        fprintf(f,  "};\n");
//    }
//    fprintf(f,  "\nparse_env* get_parse_env(){\n");
//
//    for(int i=0;i<=info_cnt;i++){
//        if(info[i].size==0)
//            continue;
//        fprintf(f,  "token_infos[%d].size=%d;\ntoken_infos[%d].ps=parse_priority%d;\n",i,info[i].size,i,i);
//    }
//    fprintf(f,      "parse_env* parse_env = parser_new_env();\n"
//                    "parse_env->lex_env = lex_new_env(lex_nodes);\n"
//                    "lex_init(parse_env->lex_env);\n"
//                    "parse_env->nodes = parse_nodes;\n"
//                    "parse_env->edges = parse_edges;\n"
//                    "parse_env->rules = parse_rules;\n"
//                    "parse_env->token_info = token_infos;\n"
//                "}\n");
//}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

void parser_fit(parse_env *env);

static int tokens_empty(parse_env *env);
static void tokens_push_back(parse_env *env, token* tok);
static void tokens_push_front(parse_env *env, token* tok);
static token* tokens_front(parse_env *env);
static token* tokens_pop(parse_env *env);

#define STATUS_START 1

#define MAX_STACK_LENGTH 1024

#define TOKENS_TOP() (env->token_stack[env->token_top])
#define TOKENS_POP() (env->token_stack[env->token_top--])
#define TOKENS_PUSH(x) (env->token_stack[++env->token_top] = (x))
#define TOKENS_STACK_EMPTY (env->token_top <= 0)

void go(parse_env *env, parse_edge *e)
{
    msg("goto %d\n", e->head);
    env->runtime->pre_edge = e;
    env->runtime->length++;
    env->runtime->cur_state = &env->states[e->head];
}

void shift(parse_env *env, int x, parse_edge* pre_edge, int length)
{
    msg("shift %d\n", x);

    //env->runtime_top++;
    env->runtime += sizeof(parse_runtime);
    env->runtime->length = length;
    env->runtime->pre_edge = pre_edge;
    env->runtime->cur_state = &env->states[x];

}

void reduce(parse_env *env, int rule_idx)
{
    parse_rule *r = &env->rules[rule_idx];
    int length = env->runtime->length;
    msg("reduce %d\n", r->out_token);
    if (env->runtime > env->runtime_stack){
        env->runtime -= sizeof(parse_runtime);
    }
    else{
        env->runtime->cur_state = &env->states[STATUS_START];
        env->runtime->length = 0;
    }
    token** tokens = &env->token_stack[env->token_top - length + 1];
    env->token_top -= length;
    if (r->func)
        r->func(r, tokens,length);
    for(int i=1;i<length;i++){
        free(tokens[i]);
    }
    tokens[0]->id = r->out_token;
    tokens_push_front(env,tokens[0]);
}

parse_env *parser_new_env()
{
    parse_env *env = (parse_env *)malloc(sizeof(parse_env));

    env->runtime = \
    env->runtime_stack = (parse_runtime*)malloc(sizeof(parse_runtime) * MAX_STACK_LENGTH);
    
    env->token_stack = (token **)malloc(sizeof(token *) * MAX_STACK_LENGTH);
    env->token_top = 0;

    env->tokens_head = env->tokens_tail = new_token(0,NULL,NULL);
    env->tokens_cnt = 0;

    return env;
}

static int tokens_empty(parse_env *env){
    return env->tokens_head == env->tokens_tail;
}

static void tokens_push_back(parse_env *env, token* tok){
    //msg("push back\n");
    env->tokens_cnt++;
    env->tokens_tail->next = tok;
    env->tokens_tail = tok;
}

static void tokens_push_front(parse_env *env, token* tok){
    env->tokens_cnt++;
    tok->next = env->tokens_head->next;
    env->tokens_head->next = tok;
}

static token* tokens_front(parse_env *env){
    return env->tokens_head->next;
}

static token* tokens_pop(parse_env *env){
    //msg("pop\n");
    env->tokens_cnt--;
    token *t =env->tokens_head->next;
    env->tokens_head->next = t->next;
    if(env->tokens_cnt == 0)
        env->tokens_tail = env->tokens_head;
    return t;
}
token *get_token_from_lex(parse_env *env)
{
    char c = 0;
    lex_env *lex_env = env->lex_env;
    token *tok;

    while (!lex_env->out && c != EOF)
    {
        c = getchar();
        lex_fit(lex_env, c);
    }
    if (lex_env->out)
    {
        tok = new_token(lex_env->out, SAVESTR(lex_env->word),NULL);
        lex_env->out = 0;
    }
    else if (c == EOF)
    {   
        if(!env->reach_end_of_file){
            env->reach_end_of_file = 1;
            tok = new_token(-1,NULL,NULL);
        } else {
            tok = NULL;
        }

    }
    return tok;
}
int get_token(parse_env *env)
{
    token *tok = get_token_from_lex(env);
    if (tok)
    {
        tokens_push_back(env,tok);
        return 1;
    }
    return 0;
}
int look_ahead(parse_env *env)
{
    token *tok;
    while(env->tokens_cnt < 2){
        tok = get_token_from_lex(env);
        if(!tok)
            break;
        tokens_push_back(env,tok);
    }
    if(!tok)
        return 0;
    else 
        return 1;
}

int parser_pase(parse_env *env)
{
    env->runtime->cur_state = &env->states[STATUS_START];
    while ((get_token(env) || !tokens_empty(env)) && env->runtime->cur_state != env->states)
    {
        //if(env->runtime_stack > 1024)
        //   break;
        parser_fit(env);
    }
    return 1;
}
inline static parse_edge* find_edge(parse_env *env, parse_state *state, token* tok){
    int e = state->first_edge;
    for (; e > 0 && env->edges[e].token != tok->id; e = env->edges[e].next);
    if (e > 0)
        return &env->edges[e];
    else 
        return NULL;
}

int dfs(parse_env *env,parse_state* state, int tok){
    if(env->rules[state->out_rule].out_token == tok)
        return 1;
    int e = state->first_edge;
    int found = 0;
    for (; e > 0 && env->edges[e].head != env->edges[e].tail; e = env->edges[e].next){
        found = dfs(env,&env->states[env->edges[e].head],tok);
        if(found)
            break;
    }
    return found;
}

parse_edge* rescursion(parse_env *env, token* tok1, token *tok2){
    parse_state *state = &env->states[STATUS_START];
    parse_edge *edge = find_edge(env,state, tok1);

    if(edge){
        state = &env->states[edge->head];
        edge = find_edge(env,state,tok2);
    }
    if(dfs(env,state,tok1->id))
        return edge;
    else 
        return NULL;
}


void parser_fit(parse_env *env)
{
    parse_state *cur_state = env->runtime->cur_state;
    parse_edge *e1,*e2,*pre_edge = env->runtime->pre_edge;
    token* tok = tokens_front(env);
    msg("%d:get token %d:%s\n", env->runtime->cur_state - env->states, tok->id, (char *)tok->data);
    e1 = find_edge(env,cur_state,tok);
    if (e1)
    {
        if(look_ahead(env)){
            token * nxt_tok = tok->next;
            if(pre_edge && (e2 = rescursion(env,tok,nxt_tok))){
                msg("re:\npre p:%d a:%d\ne2  p:%d a:%d\n",pre_edge->p,pre_edge->a,e2->p,e2->a);
                if(pre_edge->p == e2->p){
                    if(pre_edge->a == LEFT_ASSOCIATIVE){
                        tokens_pop(env);
                        TOKENS_PUSH(tok);
                        go(env,e1);
                    }
                    else if (pre_edge->a == RIGHT_ASSOCIATIVE){
                        tokens_pop(env);
                        tokens_pop(env);
                        TOKENS_PUSH(tok);
                        TOKENS_PUSH(nxt_tok);
                        shift(env,e2->head,e2,2);
                    }
                } else if (pre_edge->p > e2->p){
                    tokens_pop(env);
                    TOKENS_PUSH(tok);
                    go(env,e1);
                } else if (pre_edge->p < e2->p){
                    tokens_pop(env);
                    tokens_pop(env);
                    TOKENS_PUSH(tok);
                    TOKENS_PUSH(nxt_tok);
                    shift(env,e2->head,e2,2);
                }
            } else {
                tokens_pop(env);
                TOKENS_PUSH(tok);
                go(env,e1);
            }
        } else {
            tokens_pop(env);
            TOKENS_PUSH(tok);
            go(env,e1);
        }
    }
    else
    {
        if (cur_state->out_rule)
        {
            //tokens_pop(env);
            reduce(env, cur_state->out_rule);
        } else {
            if(env->runtime->cur_state == &env->states[STATUS_START]){
                env->runtime->cur_state = 0;
                msg("ERROR!\n");
                exit(99);
                return;
            }
            shift(env,STATUS_START,NULL,0);
        }
    }
}
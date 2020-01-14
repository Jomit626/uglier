#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

void parser_fit(parse_env *env);

static int tokens_empty(parse_env *env);
static int tokens_push_back(parse_env *env, token* tok);
static int tokens_push_front(parse_env *env, token* tok);
static token* tokens_front(parse_env *env);
static token* tokens_pop(parse_env *env);

#define STATUS_START 1

#define MAX_STACK_LENGTH 1024
#define STATUS_TOP(x) (x = env->status_stack[env->status_top])
#define STATUS_POP() (env->status_stack[env->status_top--])
#define STATUS_PUSH(x) (env->status_stack[++env->status_top] = (x))
#define STATUS_STACK_EMPTY (env->status_top <= 0)

#define TOKENS_TOP() (env->token_stack[env->token_top])
#define TOKENS_POP() (env->token_stack[env->token_top--])
#define TOKENS_PUSH(x) (env->token_stack[++env->token_top] = (x))
#define TOKENS_STACK_EMPTY (env->token_top <= 0)

void gogoto(parse_env *env, int x)
{
    msg("goto %d\n", x);
    env->pre_status = env->status;
    env->status = x;
}

void backward(parse_env *env)
{
    msg("backward %d\n", env->pre_status);
    env->status = env->pre_status;
    TOKENS_POP();
}

void shift(parse_env *env, int x)
{
    msg("shift %d\n", x);
    STATUS_PUSH(env->status);
    env->status = x;
}

void reduce(parse_env *env, parse_rule *r)
{
    msg("reduce %d\n", r->out_token);
    if (env->status_top > 0)
        env->status = STATUS_POP();
    else
        env->status = 1;
    token *t;
    if (r->func)
    {
        t = r->func(r, &env->token_stack[env->token_top - r->length + 1]); // TODO: wtf
    }
    else
    {
        t = new_token(r->out_token, NULL,NULL);
    }
    env->token_top -= r->length;
    tokens_push_front(env,t);
}

parse_env *parser_new_env()
{
    parse_env *env = (parse_env *)malloc(sizeof(parse_env));

    env->status_stack = (int *)malloc(sizeof(int) * MAX_STACK_LENGTH);
    env->status_top = 0;
    env->token_stack = (token **)malloc(sizeof(token *) * MAX_STACK_LENGTH);
    env->token_top = 0;

    env->tokens_head = env->tokens_tail = new_token(0,NULL,NULL);
    env->tokens_cnt = 0;
    env->pre_status = 0;
    env->status = 1;
    return env;
}

static int tokens_empty(parse_env *env){
    return env->tokens_head == env->tokens_tail;
}

static int tokens_push_back(parse_env *env, token* tok){
    //msg("push back\n");
    env->tokens_cnt++;
    env->tokens_tail->next = tok;
    env->tokens_tail = tok;
}

static int tokens_push_front(parse_env *env, token* tok){
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
    //msg("get\n");
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

int parse(parse_env *env)
{
    while ((get_token(env) || !tokens_empty(env)) && env->status > 0)
    {
        if(env->status_top > 1024)
            break;
        parser_fit(env);
    }
    return 1;
}

void parser_fit(parse_env *env)
{
    parse_node *cur_node = &env->nodes[env->status];
    token* token = tokens_front(env);
    msg("%d:get token %d:%s\n", env->status, token->id, (char *)token->data);
    int edge_idx = cur_node->first_edge;
    for (; edge_idx > 0 && env->edges[edge_idx].token != token->id; edge_idx = env->edges[edge_idx].next)
        ;

    if (edge_idx > 0)
    {
        parse_edge *e = &env->edges[edge_idx];
        int next_state = e->end;
        token_info *info = &env->token_info[token->id];
        // deal with recursion
        if (env->status != STATUS_START && info->size)
        {
            msg("recursion\n");
            if(look_ahead(env)){
                int next_tok = token->next->id;
                parse_node *next_node = &env->nodes[next_state];
                for(int i=0;i<info->size;i++){
                    if(info->ps[i].token == next_tok){
                        parse_priority *p = &info->ps[i];
                        msg("cur:%d:tokid:%d:%d ass:%d\n",next_node->priority,next_tok,p->priority,next_node->associative);
                        if(next_node->priority > p->priority || (next_node->priority == p->priority && next_node->associative == LEFT_ASSOCIATIVE)){
                            tokens_pop(env);
                            TOKENS_PUSH(token);
                            gogoto(env, next_state); //reduce in next step
                            return;
                        } else {
                            shift(env,STATUS_START);
                            return;
                        }
                        break;
                    }
                }
                // no expandable
                msg("DDDDDDDDDDDDDDDDD%d\n",cur_node->associative);
                //if(cur_node->associative == LEFT_ASSOCIATIVE){
                            tokens_pop(env);
                            TOKENS_PUSH(token);
                            gogoto(env, next_state); //reduce in next step
                //} else {
                //    shift(env,STATUS_START);
//
                //}
                return;
            }
        }
        tokens_pop(env);
        TOKENS_PUSH(token);
        gogoto(env, next_state);
        return;
    }
    else
    {
        if (cur_node->rule)
        {
            //tokens_pop(env);
            reduce(env, &env->rules[cur_node->rule]);
        } else {
            if(env->status == 1){
                env->status = 0;
                msg("ERROR!\n");
                return;
            }
            shift(env,STATUS_START);
        }
    }
}
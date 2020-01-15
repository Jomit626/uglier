#include <stdio.h>

#include "lex/lex_anal.h"
#include "lex/lex_gen.h"
#include "parse/parser.h"
#include "parse/parse_gen.h"
int debug = 0;
#include "./utilities/utility.h"

lex_env *self_env;
parse_env *target_env;
const int token_number = TOKEN_NUMBER;
const int token_identifier = TOKEN_IDENTIFIER;
const int token_string = TOKEN_STRING;
const int token_char = TOKEN_CONS_CHAR;
int token_endofline;
int token_semicolon,token_colon,token_left_asso,token_right_asso,token_left_branket,token_right_branket,token_pound;
int token_neg,token_star,token_question;

FILE* grammar_file;

void init(){
    lex_gen_init(256);
    self_env = lex_new_env(lex_gen_get_nodes());
    //token_endofline = lex_gen_add_token(14,"\n");
    token_semicolon = lex_gen_add_token(6,";");
    token_colon = lex_gen_add_token(7,":");
    token_left_asso = lex_gen_add_token(8,"<");
    token_right_asso = lex_gen_add_token(9,">");
    token_left_branket = lex_gen_add_token(10,"{");
    token_right_branket = lex_gen_add_token(11,"}");
    token_semicolon = lex_gen_add_token(12,";");
    token_pound= lex_gen_add_token(13,"#");
    token_neg = lex_gen_add_token(14,"-");
    token_star = lex_gen_add_token(15,"*");
    token_question = lex_gen_add_token(16,"?");
    lex_gen_init(2048);
    parse_gen_init();
    target_env = parser_new_env();
    target_env->lex_env = lex_new_env(lex_gen_get_nodes());
    lex_init(target_env->lex_env);
    parse_gen_assign_to_env(target_env);

    // buildin identifier // TODO: 
    hash_table *tab = target_env->token_map;
    hash_table_item *item = search_item(tab,"parser_identifier",1);
    item->value = token_identifier;
    item = search_item(tab,"parser_number",1);
    item->value = token_number;
    item = search_item(tab,"parser_string",1);
    item->value = token_string;
    item = search_item(tab,"parser_char",1);
    item->value = token_char;
}

static int get_token(lex_env* env){
    char c = getc(grammar_file);
    while (c != EOF)
    {
        lex_fit(env,c);
        if(env->out){
            int token = env->out;
            env->out = 0;

            return token;
        }
        c = getc(grammar_file);
    }
    return 0;
}
static int search_token(hash_table* tab,const char* s){
    hash_table_item* item = search_item(tab,s,1);
    int tok;
    if(item->value == 0){
        tok = item->value = tab->nentities;
    } else {
        tok = item->value;
    }
    return tok;
}
static int search_token_new_to_lex(hash_table* tab,const char* s){
    hash_table_item* item = search_item(tab,s,1);
    int tok;
    if(item->value == 0){
        tok = item->value = tab->nentities;
        lex_gen_add_token(tok,s + 1);
    } else {
        tok = item->value;
    }
    return tok;
}


int main(int argc, char** argv){
    if(argc > 1)
        debug  = 1;
    init();
    lex_init(self_env);
    grammar_file = fopen("grammar","r");

    int s = 0;
    int rule_id;

    char *word = self_env->word;

    hash_table *tab = target_env->token_map;
    char func_buff[50000];
    int func_size = 0;

    int neg = 0, rescur = 0, a = 0, p = DEFAULT_ASSOCIATIVITY;
    int mult = 0, opt = 0;
    int pending_edge_start = 0;
    int rule_end = 0;
    char c;
    for(int tok=get_token(self_env);tok;tok=get_token(self_env)){
        //printf("%-4d:%s\n",tok,word);
        if(tok == token_pound)
            s = 4;
        if(tok == token_neg){
            neg = 1;
            continue;
        } else if (tok !=token_number){
            neg = 0;
        }
        switch (s)
        {
        case 0:
            if(tok == token_identifier){
                tok = search_token(tab,word);
                rule_id = parse_gen_add_rule(tok);
                p = 0, a = DEFAULT_ASSOCIATIVITY;
                mult = 0,pending_edge_start = 0;
                opt = 0;
                rule_end = 1;
                //msg("add rule of %d:%s -> %d\n",rule_id,word,tok);
                //parse_gen_rule_set_func(rule_id,cat_token);
                s = 1;
            }
            break;
        case 1:
            if(tok==token_colon){
                s = 2;
            }
            break;
        case 2:
            if(tok==token_number){
                p = atoi(word);
            }else if(tok==token_left_asso){
                a = LEFT_ASSOCIATIVE;
            }else if(tok==token_right_asso){
                a = RIGHT_ASSOCIATIVE;
            }else if(tok==token_identifier){
                int pre_state = rule_end;
                tok = search_token(tab,word);
                msg("add %s a:%d p:%d from %d",word,a,p,rule_end);
                rule_end = parse_gen_rule_add_token(rule_id,tok,p,a);
                msg("to %d\n",rule_end);
                if(pending_edge_start){
                    msg("add from %d to %d\n",pending_edge_start,rule_end);
                    add_edge(pending_edge_start,rule_end,tok,p,a);
                    pending_edge_start = 0;
                }
                if(mult){
                    mult = 0;
                    pending_edge_start = pre_state;
                    msg("add from %d to %d\n",rule_end,rule_end);
                    add_edge(rule_end,rule_end,tok,p,a);
                }
                if(opt){
                    opt = 0;
                    pending_edge_start = pre_state;
                }
                p = 0, a = DEFAULT_ASSOCIATIVITY;
            }else if(tok==token_char){
                int pre_state = rule_end;
                word[2] = 0;
                msg("add %s a:%d p:%d from %d",word,a,p,rule_end);
                tok = search_token_new_to_lex(tab,word);
                rule_end = parse_gen_rule_add_token(rule_id,tok,p,a);
                msg("to %d\n",rule_end);
                if(pending_edge_start){
                    msg("add from %d to %d\n",pending_edge_start,rule_end);
                    add_edge(pending_edge_start,rule_end,tok,p,a);
                    pending_edge_start = 0;
                }
                if(mult){
                    mult = 0;
                    pending_edge_start = pre_state;
                    msg("add from %d to %d\n",rule_end,rule_end);
                    add_edge(rule_end,rule_end,tok,p,a);
                }
                if(opt){
                    opt = 0;
                    pending_edge_start = pre_state;
                }
                p = 0, a = DEFAULT_ASSOCIATIVITY;
            }else if(tok==token_string){
                int pre_state = rule_end;
                msg("add %s a:%d p:%d from %d",word,a,p,rule_end);
                word[self_env->wlen - 1] = 0;
                tok = search_token_new_to_lex(tab,word);
                rule_end = parse_gen_rule_add_token(rule_id,tok,p,a);
                msg("to %d\n",rule_end);
                if(pending_edge_start){
                    msg("add from %d to %d\n",pending_edge_start,rule_end);
                    add_edge(pending_edge_start,rule_end,tok,p,a);
                    pending_edge_start = 0;
                }
                if(opt){
                    opt = 0;
                    pending_edge_start = pre_state;
                }
                if(mult){
                    mult = 0;
                    pending_edge_start = pre_state;
                    msg("add from %d to %d\n",rule_end,rule_end);
                    add_edge(rule_end,rule_end,tok,p,a);
                }
                p = 0, a = DEFAULT_ASSOCIATIVITY;
            }else if(tok == token_semicolon){
                msg("end at %d\n",rule_end);
                parse_gen_rule_end_at(rule_id,rule_end);
                if(pending_edge_start){
                    parse_gen_rule_end_at(rule_id,pending_edge_start);
                }
                s = 0;
            } else if(tok == token_left_branket){
                int cnt = 1;
                char c = getc(grammar_file);
                func_size = 1;
                func_buff[0] = '{';
                while(cnt && c != EOF){
                    func_buff[func_size++] = c;
                    putchar(c);
                    if(c == '{') cnt++;
                    if(c == '}') cnt--;
                    c = getc(grammar_file);
                }
                func_buff[func_size++] = '\n';
                func_buff[func_size++] = 0;
                parse_gen_rule_set_func(rule_id,SAVESTR(func_buff));
                s = 0;
            } else if (tok == token_star){
                mult = 1;
            }else if (tok == token_question){
                opt = 1;
            }
            break;
        case 3:
            if(tok == token_endofline)
                s = 0;
            break;
        case 4:
            {
                char c = getc(grammar_file);
                while(c!='\n' && c != EOF)
                    c = getc(grammar_file);
                s = 0;
            }
            break;
        default:
            break;
        }
    }
    //return 1;
    //msg("parse grammar done.\n");
    freopen("test","r",stdin);
    //FILE *f = fopen("data.c","w");
    //parse_gen_analyse();
    //for(int i=0;i<tab->nentities;i++){
    //    hash_table_item* item = tab->buckets[i];
    //    while(item){
    //        printf("%s\n",item->key);
    //        item = item->next;
    //    }
    //}
    //lex_gen_dump(f);
    //parse_gen_dump(f);
    parse(target_env);
}
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "./lex_anal.h"

#define is_alpha(x) (((x) >= 'a'&&(x)<='z')||((x) >= 'A'&&(x)<='Z'))
#define is_num(x) ((x) >= '0'&&(x)<='9')
#define is_vaild_identifier_start(x) (((x) >= 'a'&&(x)<='z')||((x) >= 'A'&&(x)<='Z')||(x)=='_')
#define is_identifier(x) (is_vaild_identifier_start(x)||is_num(x))


//special words

typedef enum hardcored_state{
    spec_word,
    init,
    start,
    identifier,
    dec_num,
    num_0,
    hex_num,
    oct_num,
    bin_num,
    string,
    schar,
    //endofline,
    identifier_out,
    num_out,
    spec_word_out,
    string_out,
    schar_out,
    error_state,
    backward,
} state_id;

lex_env* lex_new_env(const lex_node *nodes){
    lex_env* env = (lex_env *)malloc(sizeof(lex_env));
    env->nodes = nodes;
    env->word = NULL;
    lex_init(env);
    return env;
}

void lex_init(lex_env *env)
{
    env->c = 0;
    env->state = start;
    env->errno = 0;

    if (env->word)
        free(env->word);
    env->word = (char *)malloc(sizeof(char) * LEX_WORD_BUFF_SIZE);
    env->wlen = 0;

    return;
}

void state_action(lex_env *env){
switch(env->state){
    case spec_word:
        if(env->is_spec_word_identifier &&!(is_num(env->c)||is_alpha(env->c)||(env->c == '_')))
            env->is_spec_word_identifier = 0;
        
    case identifier:case dec_num:case num_0:
    case hex_num:case oct_num:case bin_num:
    case string: case schar:
        if (env->wlen > LEX_WORD_BUFF_SIZE){
            env->state = error_state;
            env->errno = LEX_ERROR_WORD_BUFF_OVERFLOW;
            break;
        }
        env->word[env->wlen++] = env->c;
        break;
    // todo better output
    //case endofline:
    //    env->out = TOKEN_ENDOFLINE;
    //    env->word[0] = 0;
    //    env->state = start;
    break;
    case spec_word_out:
        env->out = env->nodes[env->word_node].out;
        env->word[env->wlen] = 0;
        env->state = backward;
    break;
    case identifier_out:
        env->out = TOKEN_IDENTIFIER;
        env->word[env->wlen] = 0;
        env->state = backward;
    break;
    case num_out:
        env->out = TOKEN_NUMBER;
        env->word[env->wlen] = 0;
        env->state = backward;
    break;
    case string_out:
        env->out = TOKEN_STRING;
        env->word[env->wlen++] = '"';
        env->word[env->wlen] = 0;
        env->state = init;
    break;
    case schar_out:
        env->out = TOKEN_CONS_CHAR;
        env->word[env->wlen++] = '\'';
        env->word[env->wlen] = 0;
        env->state = init;
    break;
    case error_state:
        env->word[env->wlen] = 0;
        exit(0);
    break;
}
}

void lex_fit(lex_env *env,char c)
{
    if(env->state == backward){
        env->state = start;
        env->wlen = 0;
        env->word[0] = 0;
        lex_fit(env,env->c);
    } else if (env->state == init){
        env->state = start;
        env->wlen = 0;
        env->word[0] = 0;
    }
    env->c = c;
        int idx;
switch(env->state){
    case spec_word:
        idx = env->nodes[env->word_node].first_child;
        for(;idx>0&&env->nodes[idx].c!=c;idx =env->nodes[idx].next_silb);

        if(idx>0&&env->nodes[idx].c==c)
            env->word_node = idx;
        else {
            int end = env->nodes[env->word_node].out;
            int c_identifier = is_identifier(c);
            if(end && (!c_identifier || !env->is_spec_word_identifier))
                env->state = spec_word_out;
            else if(c_identifier && env->is_spec_word_identifier)
                env->state = identifier;
            else if(!end && !c_identifier && env->is_spec_word_identifier)
                env->state = identifier_out;
            else  
                env->state = error_state;
        }
    break;
    case start:
        // spec words
        if(env->nodes[c].c == c){
            env->state = spec_word;
            env->word_node = c;
            if(is_alpha(c)||(c == '_'))
                env->is_spec_word_identifier = 1;
            else 
                env->is_spec_word_identifier = 0;
        } else if(is_alpha(c)||(c == '_'))
            env->state = identifier;
        else if((c >= '1'&&c<='9'))
            env->state = dec_num;
        else if(c == ' ' || c == '\t')
            env->state = start;
        else if(c == '\n')
            env->state = start;
        else if(c == '"')
            env->state = string;
        else if(c == '\'')
            env->state = schar;
        if(c == '0')
            env->state = num_0;
    break;
    case identifier:
        env->state = is_num(c)||is_alpha(c)||(c == '_') ? env->state : identifier_out;
    break;
    case identifier_out:

    break;
    case dec_num:
        env->state = is_num(c)||(c == '_') ? env->state : num_out;
    break;
    case num_0:
        if((c >= '0'&&c<='9'))
            env->state = oct_num;
        else if (c == 'x' || c == 'X')
            env->state = hex_num;
        else if (c == 'b' || c == 'B')
            env->state = bin_num;
        else 
            env->state = num_out;
    break;
    case hex_num:
        env->state = (is_num(c)||(c >= 'a'&&c<='f')||(c >= 'A'&&c<='F')||(c == '_')) ? env->state : num_out;
    break;
    case oct_num:
        env->state = ((c >='0' &&  c <= '7') || c == '_') ? env->state : num_out;
    break;
    case bin_num:
        env->state = (c == '0' || c == '1' || c == '_') ? env->state : num_out;
    break;
    case string:
        if(c == '"' && !env->is_escaped)
            env->state = string_out;
        if(c == '\\') env->is_escaped = !env->is_escaped;
        else env->is_escaped = 0;
    break;  
    case schar:
        if(c == '\''&& !env->is_escaped)
            env->state = schar_out;
        if(c == '\\') env->is_escaped = !env->is_escaped;
        else env->is_escaped = 0;
    break;
    case num_out:

    break;
}
    state_action(env);
}

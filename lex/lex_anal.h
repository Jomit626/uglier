#ifndef LEX_HEAD_INCLUDED
#define LEX_HEAD_INCLUDED

typedef struct node{
    char c;
    int out;
    int first_child;
    int next_silb;
}lex_node;

typedef struct lex_env
{
    int errno;
    char c;
    char *word;
    int wlen;
    int out;
    int state;

    int is_spec_word_identifier;
    int word_node;

    int is_escaped;
    const lex_node *nodes;
}lex_env;

#define LEX_WORD_BUFF_SIZE 123942

// built in tokens
#define TOKEN_NUMBER 1
#define TOKEN_IDENTIFIER 2
#define TOKEN_STRING 3
#define TOKEN_CONS_CHAR 4

// TODO: out put queue;


#define LEX_ERROR_WORD_BUFF_OVERFLOW 1
lex_env* lex_new_env(const lex_node*);
void lex_init(lex_env *env);
void lex_fit(lex_env *env,char c);

#endif
#include "token.h"

token* new_token(int id,void* data,token* next){
    token *t = (token*)malloc(sizeof(token));
    t->id = id;
    t->data = data;
    t->next = next;
    return t;
}


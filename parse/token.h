#ifndef TOKEN_HEAD
#define TOKEN_HEAD

#include <stdlib.h>
#include "../utilities/hash_map.h"

typedef struct token
{
    int id;
    void* data;
    struct token *next;
}token;

token* new_token(int id,void* data,token* next);

#endif
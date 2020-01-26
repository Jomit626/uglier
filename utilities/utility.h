#ifndef UTILITIES_INCLUDED
#define UTILITIES_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utilities/hash_map.h"
#define DEBUG
#define STREQ(a, b) ((a)[0] == (b)[0] && strcmp(a, b) == 0)
#define SAVESTR(s) (strcpy((char *)malloc(strlen(s) + 1), s))
#ifdef DEBUG
#define msg(...) do{printf(__VA_ARGS__);}while(0);
#else
#define msg(...)
#endif
#define DUMP_OBJ(f__,x__,n__) (dump_obj(f__,#x__,x__,sizeof(*x__)*(n__)))
void dump_obj(FILE *f, char* name, void *obj, size_t n);
#endif
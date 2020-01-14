#ifndef UTILITIES_INCLUDED
#define UTILITIES_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utilities/hash_map.h"

#define STREQ(a, b) ((a)[0] == (b)[0] && strcmp(a, b) == 0)
#define SAVESTR(s) (strcpy((char *)malloc(strlen(s) + 1), s))

#define msg(...) do{printf(__VA_ARGS__);}while(0);
//#define msg(...)
#endif
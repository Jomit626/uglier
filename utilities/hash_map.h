/**
 * 
 */
#ifndef Hash_Map
#define Hash_Map

#include "utility.h"

#define DEFAULT_BUCKETS 8

typedef struct table_item
{
    struct table_item *next;
    const char *key;
    int value;
    int key_hash;
} hash_table_item;
typedef hash_table_item VAR;
typedef struct
{
    hash_table_item **buckets;
    int nbuckets;
    int nentities;
} hash_table;

hash_table *create_hash_table(int nbuckets);

int hash_string(const char *s);

hash_table_item *search_item(hash_table *tab, const char *key, int create);
#endif
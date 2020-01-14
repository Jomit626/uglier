/**
 * 
 */
#include "hash_map.h"

hash_table *create_hash_table(int nbuckets)
{
    hash_table *tab = (hash_table *)malloc(sizeof(hash_table));

    nbuckets = nbuckets > 0 ? nbuckets : DEFAULT_BUCKETS;
    hash_table_item **buckets = (hash_table_item **)malloc(sizeof(hash_table_item *) * nbuckets);

    for (int i = 0; i < nbuckets; i++)
    {
        buckets[i] = (hash_table_item *)NULL;
    }
    tab->buckets = buckets;
    tab->nbuckets = nbuckets;
    tab->nentities = 0;

    return tab;
}

#define OFFSET 2166136261
#define PRIME 16777619

int hash_string(const char *s)
{
    int k = OFFSET;
    for (; *s; s++)
    {
        k *= PRIME;
        k ^= *s;
    }
    return k;
}

hash_table_item *search_item(hash_table *tab, const char *key, int create)
{
    int key_hash = hash_string(key);
    int bucket = key_hash & (tab->nbuckets - 1);

    hash_table_item *list;
    for (list = tab->buckets[bucket]; list; list = list->next)
    {
        if (list->key_hash == key_hash && STREQ(list->key, key))
        {
            return list;
        }
    }
    if (!create)
        return NULL;
    // no found, create new item
    list = tab->buckets[bucket];
    hash_table_item *item = (hash_table_item *)malloc(sizeof(hash_table_item));

    item->value = 0;
    item->key = SAVESTR(key);
    item->key_hash = key_hash;

    item->next = list;
    tab->buckets[bucket] = item;
    tab->nentities++;

    return item;
}
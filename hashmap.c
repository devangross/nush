
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// This provides strlcpy
// See "man strlcpy"
#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"


// return a hash of the key
int
hash(char* key)
{
    long hash = 0;
    for (int ii = 0; key[ii]; ++ii) {
        hash = hash * 13 + key[ii];
    }
    return hash;
}


// create a hashmap with a given capacity
hashmap*
make_hashmap_presize(int nn)
{
    hashmap* hh = calloc(1, sizeof(hashmap));
    hh->size = 0;
    hh->capacity = nn;
    hh->data = calloc(hh->capacity, sizeof(hashmap_pair));
    return hh;
}


// create a hashmap of capacity 4
hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}


// free the hashmap and its data
void
free_hashmap(hashmap* hh)
{
    free(hh->data);
    free(hh);
}


// returns whether the key has a value in the map
int
hashmap_has(hashmap* hh, char* kk)
{
    return (hashmap_get(hh, kk) != NULL);
}


// get the value at the given key
char*
hashmap_get(hashmap* hh, char* kk)
{
    int start = hash(kk) % hh->capacity;
    
    for (int ii = start; ii < hh->capacity + start; ++ii) {
        hashmap_pair pp = hh->data[ii % hh->capacity];
        if (strcmp(pp.key, kk) == 0 && pp.used) {
            return pp.val;
        }
    }
    return NULL; 
}


// create a new data array of double the size and copy old data into new one
void
hashmap_grow(hashmap* hh) {
    long old_cap = hh->capacity;
    hashmap_pair* old_data = hh->data;
    hh->capacity *=  2;
    hh->data = calloc(hh->capacity, sizeof(hashmap_pair));
    hh->size = 0;
    for (int ii = 0; ii < old_cap; ++ii) {
        hashmap_pair pp = old_data[ii];
        if (pp.used) {
            hashmap_put(hh, pp.key, pp.val);
        }
    }
    free(old_data);
}


// put key, value pair into hashmap, resizing data array if load factor is met
void
hashmap_put(hashmap* hh, char* kk, char* vv) {
    double load_factor = ((double) hh->size) / ((double) hh->capacity);

    if (load_factor >= 0.5) {
        hashmap_grow(hh);
    }

    long start = hash(kk) % hh->capacity;

    for (int ii = start; ii < hh->capacity + start; ++ii) {
        if (strcmp(hh->data[ii % hh->capacity].key, kk) == 0) {
            strlcpy(hh->data[ii % hh->capacity].key, kk, 4);
            free(hh->data[ii % hh->capacity].val);
            hh->data[ii % hh->capacity].val = strdup(vv);
            hh->data[ii % hh->capacity].used = true;
            hh->data[ii % hh->capacity].tomb = false;
            break;
        }
        if (!hh->data[ii % hh->capacity].used && !hh->data[ii % hh->capacity].tomb) {
            strlcpy(hh->data[ii % hh->capacity].key, kk, 4);
            hh->data[ii % hh->capacity].val = strdup(vv);
            hh->data[ii % hh->capacity].used = true;
            hh->data[ii % hh->capacity].tomb = false;
            hh->size += 1;
            break;
        }
    }
}


// remove the value at the given key
void
hashmap_del(hashmap* hh, char* kk)
{
    int start = hash(kk) % hh->capacity;
    for (int ii = start; ii < hh->capacity + start; ++ii) {
        if (strcmp(hh->data[ii % hh->capacity].key, kk) == 0){
            hh->data[ii % hh->capacity].used = false;
            hh->data[ii % hh->capacity].tomb = true;
        }
    }
}


// return the pair at the given index
hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    return hh->data[ii];
}


// print all active key, value pairs
void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    for (int ii = 0; ii < hh->capacity; ++ii) {
        hashmap_pair pp = hh->data[ii];
        if (pp.used && !pp.tomb) {
            printf("%d: %s => %s\n", ii, pp.key, pp.val);
        }
    }
}

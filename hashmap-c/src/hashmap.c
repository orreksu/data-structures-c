
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"

int
hash(char* key, long nn)
{
    int hh = 23;
    for (int ii = 0; key[ii]; ++ii) {
        hh = hh * 67 + key[ii];
    }
    
    return hh & (nn - 1);   // &(nn-1) = %nn
}

// creates used, untombed pair with given key 'kk' and value 'vv'
hashmap_pair
make_pair(char* kk, int vv) {
    hashmap_pair pp;
    strlcpy(pp.key, kk, 4);   
    pp.key[3] = 0;
    pp.val = vv;
    pp.used = 1;
    pp.tomb = 0;
    
    return pp;
}

hashmap*
make_hashmap_presize(int nn)
{
    hashmap* hh = malloc(sizeof(hashmap));
    hh->size = 0;
    hh->capacity = nn;
    hh->data = calloc(hh->capacity, sizeof(hashmap_pair));

    return hh;
}

hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}

void
free_hashmap(hashmap* hh)
{
    free(hh->data);
    free(hh);
}

void 
grow_hashmap(hashmap* hh)
{
    hashmap_pair* old = hh->data;
    long old_capacity = hh->capacity;

    hh->capacity *= 2;
    hh->data = calloc(hh->capacity, sizeof(hashmap_pair));

    for (long ii = 0; ii < old_capacity; ++ii) {
        hashmap_pair curr = old[ii];

        if (!curr.tomb && curr.used) {
            hashmap_put(hh, curr.key, curr.val);
        }    
    }

    free(old);
}

int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}

// returns value associated with the key 'kk'
// returns -1 for key not found
int
hashmap_get(hashmap* hh, char* kk)
{
    long ii = hash(kk, hh->capacity);
    int loop = 0;
    for ( ; ii < hh->capacity; ++ii) {
        hashmap_pair curr = hh->data[ii];

        if (!curr.tomb && curr.used && strcmp(curr.key, kk) == 0) {
            return curr.val;
        }

        // not in the hashmap
        if (!curr.tomb && !curr.used) {
            break;
        }
    }

    return -1;
}

void
hashmap_put(hashmap* hh, char* kk, int vv)
{
    float load_factor = 0.5;
    if (hh->size >= load_factor * hh->capacity) {
        grow_hashmap(hh);
    }

    long ii = hash(kk, hh->capacity);
    for ( ; ii < hh->capacity; ++ii) {
        hashmap_pair curr = hh->data[ii];

        // adds new pair with value 'vv', and key 'kk'
        if (curr.tomb || !curr.used) {
            hh->data[ii] = make_pair(kk, vv);
            hh->size += 1;
            break;
        }

        // insets the value 'vv' for pair with key 'kk'
        if (strcmp(curr.key, kk) == 0) {
            hh->data[ii].val = vv;
            break;
        }
    }
}
    
// removes any value associated with key by tombing it
void
hashmap_del(hashmap* hh, char* kk)
{   
    long ii = hash(kk, hh->capacity);
    for ( ; ii < hh->capacity; ++ii) {
        hashmap_pair curr = hh->data[ii];

        if (!curr.tomb && curr.used && strcmp(curr.key, kk) == 0) {
            hh->data[ii].tomb = 1;
            hh->size -= 1;
            break;
        }

        // not in the hashmap
        if (!curr.tomb && !curr.used) {
            break;
        }
    }
}

hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    return hh->data[ii];
}

// prints all the keys and values currently in the map, in storage order
void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    
    for (long ii = 0; ii < hh->capacity; ++ii) {
        hashmap_pair curr = hh->data[ii];
        if (!curr.tomb && curr.used) {
            printf("Key: %s, Value: %d\n", curr.key, curr.val);
        }
    }
}

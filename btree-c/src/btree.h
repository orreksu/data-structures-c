#ifndef BTREE_H
#define BTREE_H

#define ORDER 5
#define MAX_KEYS (ORDER - 1)

typedef struct btree_pair {
    char key[4];            // item is empty if key[0] == '\0'
    int  val;
} btree_pair;

typedef struct btree {
    int             used;   // number of used entries
    int             size;   // size of the full tree
    btree_pair      data[MAX_KEYS];
    struct btree*   kids[ORDER];
} btree;

btree*      make_btree();
void        free_btree(btree* bt);

int         btree_has(btree* bt, char* kk);

int         btree_get(btree* bt, char* kk);
void        btree_put(btree* bt, char* kk, int vv);
void        btree_del(btree* bt, char* kk);

int         btree_keys(btree* bt, char** keys);
void        btree_dump(btree* bt);

#endif

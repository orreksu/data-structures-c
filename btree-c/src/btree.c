
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include <bsd/string.h>

#include "btree.h"


/* =========================== STATIC FUNCTIONS =============================*/
static int          is_leaf(btree* bt);
static int          is_empty(btree_pair* pair);
static int          is_greater(btree_pair* pair, char* kk);
static int          is_matching(btree_pair* pair, char* kk);
static int          find_pos(btree* bt, char* kk);

static btree_pair*  btree_find(btree* bt, char* kk);
static void         swap_pairs(btree_pair* aa, btree_pair* bb);
static void         swap_nodes(btree** aa, btree** bb);
static int          node_calc_size(btree* bt);

static btree*       btree_split_left(btree* bt);
static btree*       btree_split_right(btree* bt, btree_pair lp, btree* lk);
static btree_pair   btree_split_mid(btree* bt);
static void         btree_split(btree* bt, btree_pair pair, btree* last);

static btree*       btree_insert_leaf(btree* bt, btree_pair pair);
static btree*       btree_insert_branch(btree* bt, btree_pair pair);
static btree*       btree_insert(btree* bt, btree_pair pair);

static void         node_dump(btree* bt, int dd);




/* ============================= ALLOCATION ================================ */
/* Allocates memory for a B-tree */
btree*
make_btree()
{
    return calloc(1, sizeof(btree));
}

/* Frees memory taken up by a B-tree */
void
free_btree(btree* bt)
{
    for (int ii = 0; ii < ORDER; ++ii) {
        if (bt->kids[ii]) {
            free_btree(bt->kids[ii]);
        }
    }
    
    free(bt);
}




/* =========================== GENERAL HELPERS ============================= */
/* Is this tree a leaf? */
static
int
is_leaf(btree* bt)
{
    for (int ii = 0; ii < ORDER; ++ii) {
        if (bt->kids[ii]) {
            return 0;
        }
    }
    
    return 1;
}

/* Is given pair free? */
static
int
is_empty(btree_pair* pair)
{
    return pair->key[0] == '\0';
}

/* Is given pair greater than the key? */
static
int
is_greater(btree_pair* pair, char* kk)
{
    return strcmp(pair->key, kk) > 0;
}

/* Is given pair has the key? */
static
int
is_matching(btree_pair* pair, char* kk)
{
    return strcmp(pair->key, kk) == 0;
}




/* ============================ FIND ENTRY ================================= */
/* Returns a pair with matching key, if does not exists returns NULL */
static
btree_pair*
btree_find(btree* bt, char* kk)
{
    // btree is NULL or empty, so nothing can be found
    if (bt == NULL || bt->size == 0) {
        return 0;
    }
    
    // loop through all pairs in this node
    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        // reference to the curr btree pair
        btree_pair* item = &(bt->data[ii]);
        
        // pair is the target
        if (is_matching(item, kk)) {
            return item;
        }
        
        // pair is empty or greater, recurse on the child to the left
        if (is_empty(item) || is_greater(item, kk)){
            return btree_find(bt->kids[ii], kk);
        }
    }
    
    // all nodes are non-empty and has keys less than target,
    // recurse on the last child
    return btree_find(bt->kids[MAX_KEYS], kk);
}

/* Does this tree have a pair with a matching key? */
int
btree_has(btree* bt, char* kk)
{
    return btree_find(bt, kk) != 0;
}

/* Get the value of the pair with the given key */
int
btree_get(btree* bt, char* kk)
{
    btree_pair* item = btree_find(bt, kk);
    return item->val;
}




/* =========================== SWAP HELPERS ================================ */
/* Swaps two given pairs */
static
void
swap_pairs(btree_pair* aa, btree_pair* bb)
{
    char tmp_key[4];
    strlcpy(tmp_key, aa->key, 4);
    strlcpy(aa->key, bb->key, 4);
    strlcpy(bb->key, tmp_key, 4);

    int tmp_val = aa->val;
    aa->val = bb->val;
    bb->val = tmp_val;
}

/* Swaps two given nodes */
static
void
swap_nodes(btree** aa, btree** bb)
{
    btree* tmp_btree;
    tmp_btree = *aa;
    *aa = *bb;
    *bb = tmp_btree;
}




/* ============================= ADD ENTRY ================================= */
/* Adds new entry into btree and balance */
void
btree_put(btree* bt, char* kk, int vv)
{
    // create an item with given key and value
    btree_pair item;
    strlcpy(item.key, kk, 4);
    item.val = vv;
    
    btree* new_root = btree_insert(bt, item);
    
    // switch root if necessary
    if (new_root != 0) {
        swap_nodes(&(bt), &(new_root));
    }
}

/* Calculates size of the nodes */
static
int
node_calc_size(btree* bt)
{
    int size = 0;
    
    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        if (bt->kids[ii]) {
            size += bt->kids[ii]->size;
        }
        
        if (!is_empty(&(bt->data[ii]))) {
            size += 1;
        }
    }
    
    if (bt->kids[MAX_KEYS]) {
        size += bt->kids[MAX_KEYS]->size;
    }
    
    return size;
}




/* ========================== SPLIT HELPERS ================================ */
/* Creates a left node for spliting */
static
btree*
btree_split_left(btree* bt)
{
    btree* node = make_btree();
    int mid = MAX_KEYS / 2;
    
    for (int ii = 0; ii < mid; ++ii) {
        node->data[ii] = bt->data[ii];
        node->kids[ii] = bt->kids[ii];
    }
    
    node->kids[mid] = bt->kids[mid];
    node->used = MAX_KEYS / 2;
    node->size = node_calc_size(node);
    
    return node;
}

/* Creates a right node for spliting */
static
btree*
btree_split_right(btree* bt, btree_pair lp, btree* lk)
{
    int i0 = MAX_KEYS / 2 + 1;
    
    btree* node = make_btree();
    
    int ii = i0;
    int jj = 0;
    
    for (; ii < MAX_KEYS; ++ii) {
        jj = ii - i0;
        node->data[jj] = bt->data[ii];
        node->kids[jj] = bt->kids[ii];
    }
    
    jj = ii - i0;
    node->kids[jj] = bt->kids[ii];

    swap_pairs(&(node->data[jj]), &lp);
    node->kids[jj + 1] = lk;
    node->used = MAX_KEYS / 2;
    node->size = node_calc_size(node);
    
    return node;
}

/* Finds the middle pair */
static
btree_pair
btree_split_mid(btree* bt)
{
    int mid = MAX_KEYS / 2;
    return bt->data[mid];
}

/* Splits the tree, inserts the pair */
static
void
btree_split(btree* bt, btree_pair pair, btree* last)
{
    btree* left = btree_split_left(bt);
    btree* right = btree_split_right(bt, pair, last);
    btree_pair mpair = btree_split_mid(bt);

    memset(bt, 0, sizeof(btree));
    bt->used = 1;
    bt->size = left->size + right->size + 1;
    bt->data[0] = mpair;
    bt->kids[0] = left;
    bt->kids[1] = right;
}




/* ========================= INSERT HELPERS ================================ */
/* Inserts given pair into the leaf node */
static
btree*
btree_insert_leaf(btree* bt, btree_pair pair)
{
    // loop through all the items in the node to insert pair
    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        // reference to the curr btree pair
        btree_pair* item = &(bt->data[ii]);
        
        // if item is matching
        if (is_matching(item, pair.key)) {
            swap_pairs(&(pair), &(bt->data[ii]));
            return 0;
        }
        
        // if item is empty
        if (is_empty(item)) {
            swap_pairs(&(pair), &(bt->data[ii]));
            bt->size++;
            bt->used++;
            return 0;
        }
        
        // if item is greater then pair
        if (is_greater(item, pair.key)) {
            swap_pairs(&(pair), &(bt->data[ii]));
        }
    }
    
    // if we got here, we overfloaded and need to split
    btree_split(bt, pair, NULL);
    
    return bt;
}

/* Inserts given pair into the branch node */
static
btree*
btree_insert_branch(btree* bt, btree_pair pair)
{
    // index should point to the child, on which to recurse
    int index = -1;
    
    // loop through all the entries
    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        // reference to the curr btree pair
        btree_pair* item = &(bt->data[ii]);
        
        // if mathcing, simply replace
        if (is_matching(item, pair.key)) {
            swap_pairs(&(pair), &(bt->data[ii]));
            return 0;
        }
        
        // if empty or greater assign index and break out
        if (is_empty(item) || is_greater(item, pair.key)) {
            index = ii;
            break;
        }
    }
    
    // if index was not updated, make it point to the last child
    if (index == -1) {
        index = MAX_KEYS;
    }
    
    // attempt to insert the pair
    assert(bt->kids[index]);
    btree* orphan = btree_insert(bt->kids[index], pair);
    
    // the recursive insert didn't result in a split, update size
    if (orphan == 0) {
        bt->size = node_calc_size(bt);
        return 0;
    }
    
    // recursive insert resulted in a split
    // we need to update the current "bt" to contain orphans data
    // place left child of the orphan to the orphan's position
    bt->kids[index] = orphan->kids[0];
    
    // insert the pair and the right child appropriately
    btree_pair orphan_pair = orphan->data[0];
    btree* right_child = orphan->kids[1];
    
    // loop through all the items in the node to insert pair
    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        // reference to the curr btree pair
        btree_pair* item = &(bt->data[ii]);
        
        // if item is matching, place item and return
        if (is_matching(item, orphan_pair.key)) {
            swap_pairs(&(orphan_pair), &(bt->data[ii]));
            swap_nodes(&(right_child), &(bt->kids[ii+1]));
            free(orphan);
            return 0;
        }
        
        // if item is empty, place item, update size and return
        if (is_empty(item)) {
            swap_pairs(&(orphan_pair), &(bt->data[ii]));
            swap_nodes(&(right_child), &(bt->kids[ii+1]));
            bt->size++;
            bt->used++;
            free(orphan);
            return 0;
        }
        
        // if item is greater then pair, keep swapping
        if (is_greater(item, orphan_pair.key)) {
            swap_pairs(&(orphan_pair), &(bt->data[ii]));
            swap_nodes(&(right_child), &(bt->kids[ii+1]));
        }
    }
    
    // if we got here, we overfloaded and need to split
    btree_split(bt, orphan_pair, right_child);
    
    free(orphan);
    return bt;
}

/* Inserts given pair into the tree */
static
btree*
btree_insert(btree* bt, btree_pair pair)
{
    btree* node = 0;
    
    // tree is a leaf, insert into the leaf
    if (is_leaf(bt)) {
        node = btree_insert_leaf(bt, pair);
    }
    
    // tree is a branch, insert into the branch
    else {
        node = btree_insert_branch(bt, pair);
    }
    
    // error testing, ensures the correct size
    if (node_calc_size(bt) != bt->size) {
        btree_dump(bt);
        fflush(stdout);
        abort();
    }
    
    return node;
}




/* =========================== DELETE ENTRY ================================ */
/* Deletes an entry with the given key */
void
btree_del(btree* bt, char* kk)
{
    // do nothing
    assert(0);
}




/* ============================== KEYS ===================================== */
/* Feels up the given array with all the keys from the tree */
/* Returns number of keys added */
int
btree_keys(btree* bt, char** keys)
{
    int off = 0;
    
    // loop through all children and entries
    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        // if child is not empty,
        // add keys from the child and increase offset
        if (bt->kids[ii]) {
            off += btree_keys(bt->kids[ii], keys + off);
        }

        // if entry is not empty
        // add its key to the array and increase offset
        if (!is_empty(&(bt->data[ii]))) {
            keys[off] = bt->data[ii].key;
            off += 1;
        }
    }
    
    // last child
    if (bt->kids[MAX_KEYS]) {
        off += btree_keys(bt->kids[MAX_KEYS], keys + off);
    }
    
    return off;
}




/* ============================ PRINT OUT ================================== */
/* Prints out a tree node, at given depth */
static
void
node_dump(btree* bt, int dd)
{
    char sp[2*dd + 1];
    memset(sp, ' ', 2*dd);
    sp[dd] = '\0';

    printf("\n%s== node dump ==\n", sp);
    printf("%ssize: %d, used: %d, depth: %d\n", sp, bt->size, bt->used, dd);

    for (int ii = 0; ii < MAX_KEYS; ++ii) {
        
        if (bt->kids[ii]) {
            node_dump(bt->kids[ii], dd + 1);
        }

        btree_pair item = bt->data[ii];
        if (is_empty(&item)) {
            continue;
        }
        
        printf("%s#%d= %c%c%c => %d\n", sp, ii,
            item.key[0], item.key[1], item.key[2], item.val);
    }

    if (bt->kids[MAX_KEYS]) {
        node_dump(bt->kids[MAX_KEYS], dd + 1);
    }

    printf("\n");
}

/* Prints out the whole b-tree */
void
btree_dump(btree* bt)
{
    node_dump(bt, 0);
}

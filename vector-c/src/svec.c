/* This file is lecture notes from CS 3650, Fall 2018 */
/* Author: Nat Tuck */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 2;
    return sv;
}

void
free_svec(svec* sv)
{
    for (int ii = 0; ii < sv->size; ++ii) {
        free(sv->data[ii]);
    }

    free(sv->data);
    sv->size = 0;
    sv->capacity = 0;
    free(sv);
}

char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = item;
}

void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    if (ii >= sv->capacity) {
        sv->capacity *= 2;
        sv->data = (char**)realloc(sv->data, sv->capacity * sizeof(char*));
    }
    
    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

void
svec_swap(svec* sv, int ii, int jj)
{
    char* tmp = sv->data[ii];
    sv->data[ii] = sv->data[jj];
    sv->data[jj] = tmp;    
}

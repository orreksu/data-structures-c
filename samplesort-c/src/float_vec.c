// Author: Nat Tuck
// CS3650 starter code

#include <stdlib.h>
#include <stdio.h>

#include "float_vec.h"

fvec*
make_fvec(long nn)
{
    fvec* xs = malloc(sizeof(fvec));
    xs->size = 0;
    xs->cap  = (nn > 1) ? nn : 2;
    xs->data = malloc(xs->cap * sizeof(float));
    return xs;
}

void
free_fvec(fvec* xs)
{
    free(xs->data);
    free(xs);
}

void
fvec_push(fvec* xs, float xx)
{
    if (xs->size >= xs->cap) {
        xs->cap *= 2;
        xs->data = realloc(xs->data, xs->cap * sizeof(float));
    }

    xs->data[xs->size] = xx;
    xs->size += 1;
}

void
fvec_print(fvec* xs)
{
    for (int ii = 0; ii < xs->size; ++ii) {
        printf("[%.04f]\n", xs->data[ii]);
    }
    printf("\n");
}



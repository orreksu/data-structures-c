// Author: Nat Tuck
// CS3650 starter code

#ifndef FLOAT_VEC_H
#define FLOAT_VEC_H

typedef struct fvec {
    long size;
    long cap;
    float* data;
} fvec;

fvec* make_fvec(long nn);
void free_fvec(fvec* xs);

void fvec_push(fvec* xs, float xx);
void fvec_print(fvec* xs);

#endif

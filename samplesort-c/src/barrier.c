// Author: Nat Tuck
// CS3650 starter code
// Modified by Oleksandr Litus

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>

#include "barrier.h"

barrier*
make_barrier(int count)
{
    int rv;
    barrier* barr = malloc(sizeof(barrier));

    if ((long) barr == -1) {
        perror("mmap(barrier)");
        abort();
    }

    rv = pthread_mutex_init(&(barr->mutex), NULL);
    if (rv == -1) {
        perror("pthread_mutex_init(mutex)");
        abort();
    }

    rv = pthread_cond_init(&(barr->cond), NULL);
    if (rv == -1) {
        perror("pthread_cond_init(cv)");
        abort();
    }

    barr->count = count;
    barr->seen  = 0;
    return barr;
}

void
barrier_wait(barrier* barr)
{
    int rv;

    // Lock the mutex
    rv = pthread_mutex_lock(&(barr->mutex));
    if (rv == -1) {
        perror("pthread_mutex_lock(mutex)");
        abort();
    }

    // Increase seen by one
    barr->seen += 1;

    if (barr->seen < barr->count) {
        // In case not the last, wait
        rv = pthread_cond_wait(&(barr->cond), &(barr->mutex));
        if (rv == -1) {
            perror("pthread_cond_wait(barrier1)");
            abort();
        }
    }
    else {
        // In case the last thread, tell others to start
        rv = pthread_cond_broadcast(&(barr->cond));
        if (rv == -1) {
            perror("pthread_cond_broadcast(cond)");
            abort();
        }
    }

    // Unclock teh mutex
    rv = pthread_mutex_unlock(&(barr->mutex));
    if (rv == -1) {
        perror("pthread_mutex_unlock(mutex)");
        abort();
    }
}

void
free_barrier(barrier* barr)
{
    free(barr);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <bsd/string.h>

#include "btree.h"

void
chomp(char* text)
{
    for(int ii = 0; text[ii]; ++ii) {
        if (text[ii] == '\n') {
            text[ii] = 0;
            return;
        }
    }
}

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage:\n  %s input N\n", argv[0]);
        return 1;
    }

    FILE* fh = fopen(argv[1], "r");
    if (!fh) {
        perror("open failed");
        return 1;
    }

    long NN = atol(argv[2]);

    btree* trips = make_btree();
    char temp[128];
    char trip[4];

    while (1) {
        char* line = fgets(temp, 128, fh);
        if (!line) {
            break;
        }

        chomp(line);

        int zz = strlen(line) - 2;
        for (int ii = 0; ii < zz; ++ii) {
            for (int jj = 0; jj < 3; ++jj) {
                trip[jj] = line[ii + jj];
            }
            trip[3] = 0;

            if (btree_has(trips, trip)) {
                int count = btree_get(trips, trip);
                btree_put(trips, trip, count + 1);
            }
            else {
                btree_put(trips, trip, 1);
            }
        }
    }

    fclose(fh);

    btree_pair tops[NN];
    for (int ii = 0; ii < NN; ++ii) {
        tops[ii].val = 0;
    }

    char* prev = "";
    char** keys = malloc(trips->size * sizeof(char*));
    btree_keys(trips, keys);

    for (int ii = 0; ii < trips->size; ++ii) {
        char* key = keys[ii];

        // Verify ascending order.
        assert(strncmp(prev, key, 4) < 0);
        prev = key;

        int count = btree_get(trips, key);
        btree_pair pair;
        strlcpy(pair.key, key, 4);
        pair.val = count;

        for (int jj = 0; jj < NN; ++jj) {
            if (count > tops[jj].val) {
                btree_pair tmp = tops[jj];
                tops[jj] = pair;
                pair = tmp;
            }
        }
    }

    free(keys);

    for (int ii = 0; ii < NN; ++ii) {
        btree_pair pair = tops[ii];
        printf("%d\t%s\n", pair.val, pair.key);
    }

    free_btree(trips);
    return 0;
}

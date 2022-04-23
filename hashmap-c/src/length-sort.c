#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "svec.h"

// sorts from shortest to longest string
// algorithm: https://en.wikipedia.org/wiki/Gnome_sort
void
sort(svec* xs)
{
    int pos = 0;
    while (pos < xs->size) {
        if (pos == 0 || strlen(xs->data[pos]) >= strlen(xs->data[pos-1])) {
            ++pos;
        }
        else {
            svec_swap(xs, pos, pos-1); 
            --pos;
        }
    }
}

// removes the trailing "\n" from the given string
// Reference: www.cplusplus.com/reference/cstring/strcspn
void
chomp(char* text)
{
    text[strcspn(text, "\n")] = 0;
}

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage:\n  %s input-file\n", argv[0]);
        return 1;
    }

    FILE* fh = fopen(argv[1], "r");
    if (!fh) {
        perror("open failed");
        return 1;
    }

    svec* xs = make_svec();
    char temp[128]; 

    while (1) {
        char* line = fgets(temp, 128, fh);
        if (!line) {
            break;
        }

        line = strdup(line);    // allocates string in memory

        chomp(line);
        svec_push_back(xs, line);
    }
    
    fclose(fh);

    sort(xs);

    for (int ii = 0; ii < xs->size; ++ii) {
        char* line = svec_get(xs, ii);
        printf("%s\n", line);
    }

    free_svec(xs);
    return 0;
}

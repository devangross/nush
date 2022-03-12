#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

// create an svec of capacity 2
svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 2;
    return sv;
}


// free the svec and all its data
void
free_svec(svec* sv)
{
    int ii = 0;
    for (int ii = 0; ii < sv->size; ii++) {
        if (sv->data[ii] != 0) {
            free(sv->data[ii]);
        }
    }
    
    free(sv->data);
    free(sv);
}


// get the value at the given index
char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}


// put the given item at the given index in the svec
void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);
}

// determines whether the svec contains the given item
int
svec_has(svec* sv, char* item) {
    int ii = 0;
    for (ii; ii < sv->size; ii++) {
        if (strcmp(svec_get(sv, ii), item) == 0) {
            return ii;
        }
    }
    return -1;
}

// double the capacity of the svec if capacity is reached
void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;

    if (ii >= sv->capacity) {
        sv->capacity *= 2;
        sv->data = realloc(sv->data, sv->capacity * sizeof(char*));
    }

    sv->size = ii + 1;
    svec_put(sv, ii, item);
}


/// swap the items at the two given indices in the svec
void
svec_swap(svec* sv, int ii, int jj)
{
    char* item_ii = sv->data[ii];
    char* item_jj = sv->data[jj];

    sv->data[ii] = item_jj;
    sv->data[jj] = item_ii;
}

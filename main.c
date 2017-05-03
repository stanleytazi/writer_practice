#include <stdlib.h>
#include <stdio.h>
#include "writer.h"


typedef struct{
    int type;
    int intVal;
    char *strVal;
} tuple;

int main() {
    char *test = "testfile";
    bp__writer_t *w = malloc(sizeof(bp__writer_t));
    tuple t;
    tuple *d = malloc(sizeof(tuple));
    t.type = 1;
    t.intVal = 20;
    uint64_t offset = 0;
    uint64_t sizee = sizeof(tuple);
    bp__writer_create(w, test);
    //bp__writer_write(w, &t, &offset, &sizee);
    bp__writer_read(w, offset, &sizee, (void **)&d);
    bp__writer_destroy(w);

    printf("d: type=%d, intVal=%d\n",d->type, d->intVal);
    return 0;
}


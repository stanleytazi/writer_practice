#include <stdlib.h>
#include <stdio.h>
#include "writer.h"
#include <arpa/inet.h>
#include <string.h>

typedef struct{
    uint32_t type;
    uint32_t intVal;
    char strVal[24];
} tuple;

int main() {
    char *test = "testfile";
    bp__writer_t *w = malloc(sizeof(bp__writer_t));
    tuple t;
    tuple *d = malloc(sizeof(tuple));
    char *tm = malloc(32);
    char *dm ;

    t.type = 1;
    t.intVal = 20;
    strcpy(t.strVal, "Stanley");

    uint64_t offset = 0;
    uint64_t sizee = sizeof(tuple);
    
    *(uint32_t *)(tm + 0) = htonl(t.type);
    *(uint32_t *)(tm + 4) = htonl(t.intVal);
    memcpy((tm+8), t.strVal, 24);
    bp__writer_create(w, test);
    //bp__writer_write(w, tm, &offset, &sizee);
    printf("offset = %llu, size = %llu\n", offset, sizee);
    bp__writer_read(w, offset, &sizee, (void **)&dm);
    bp__writer_destroy(w);
    d->type = ntohl(*(uint32_t *)(dm + 0));
    d->intVal = ntohl(*(uint32_t *)(dm + 4));
    memcpy(d->strVal, dm + 8, 24);
    printf("d: type=%d, intVal=%d, strVal=%s\n",d->type, d->intVal, d->strVal);
    return 0;
}


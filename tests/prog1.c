#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>

static int *f1(void) {
    int *x;

    printf("++f1 malloc...\n");
    x = malloc(10);
    if (!x) {
        printf("--f1 malloc error...\n");
    } else {
        printf("--f1 malloc...\n");
    }

    printf("++f1 free...\n");
    /* free(x); */
    printf("--f1 free...\n");
    return NULL;
}


int main(void) {
    void *p;

    printf("++main malloc...\n");
    p = malloc(10);
    if (!p) {
        printf("--main malloc error...\n");
        return 1;
    } else {
        printf("--main malloc...\n");
    }
    printf("++main free...\n");
    /* free(p);                           #<{(| freeing memory from heap |)}># */
    printf("--main free...\n");


    p = f1();

    return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define POOL_SIZE 1024
#define MAX_ALLOC (1024 * 1024)

static void alloc(void **pool, int amount) {
    int i;
    printf("Alloc %d\n", amount);
    for(i = 0; i < POOL_SIZE; i++) {
        if (pool[i] == NULL) {
            pool[i] = malloc(random() % MAX_ALLOC);
            amount--;
            if (amount == 0) {
                break;
            }
        }
    }
}

static void release(void **pool, int amount) {
    int i;
    printf("Release: %d\n", amount);
    for(i = 0; i < POOL_SIZE; i++) {
        if (pool[i] != NULL) {
            free(pool[i]);
            pool[i] = NULL;
            amount--;
            if(amount == 0) {
                break;
            }
        }
    }
}

int main(int argc, char **argv) {
    void *pool[POOL_SIZE];
    int i;
    int count = 0;

    memset(pool, 0, sizeof(void *) * POOL_SIZE);
    for(i = 0; i < 1114; i++) {
        switch(random() % 2) {
            case 0:
                alloc(pool, random() % POOL_SIZE);
            break;
            case 1:
                release(pool, random() % POOL_SIZE);
            break;
            default:
                printf("EER\n");
                abort();
        }
    }

    for(i = 0; i < POOL_SIZE; i++) {
        if (pool[i] != NULL){
            count++;
        }
    }

    printf("Still allocated: %d\n", count);
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define THREADS 16
#define POOL_SIZE 1024
#define MAX_ALLOC (1024)

#define D(x) 

static void alloc(void **pool, int amount) {
    int i;
    D(printf("Alloc %d\n", amount));
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
    D(printf("Release: %d\n", amount));
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

void *thread_job(void *arg) {
    void *pool[POOL_SIZE];
    int i;
    unsigned long count = 0;

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

    printf("Still allocated: %lu\n", count);
    return (void *)count;
}

int main(int argc, char **argv) {
    pthread_t threads[THREADS];
    int i;
    unsigned long tcount = 0;
    unsigned long retval;

    for(i = 0; i < THREADS; i++) {
        if (pthread_create(&(threads[i]), NULL, thread_job, NULL) != 0) {
            printf("Fail to alloacte thread %d\n", i);
        }
    }

    printf("Waiting for threads\n");
    for(i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i], ((void **)&retval)) != 0) {
            printf("Fail to wait for thread %d\n", i);
        } else {
            tcount +=retval;
        }
    }
    printf("Total still alloacted: %lu\n", tcount);

    return 0;

}

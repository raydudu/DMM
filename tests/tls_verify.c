#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

static __thread int tid;

void *tid_check(void *arg) {
    long id = (long)arg;
    tid = id;
    sleep(1);
    if (id != tid) {
        printf("FAIL!\n");
    }
    return NULL;
}

#define THREADS 16

int main(int argc, char **argv) {
    pthread_t threads[THREADS];
    int i;

    for(i = 0; i < THREADS; i++) {
        if (pthread_create(&(threads[i]), NULL, tid_check, (void *)(long)random()) != 0) {
            printf("Fail to alloacte thread %d\n", i);
        }
    }

    printf("Waiting for threads\n");
    for(i = 0; i < THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("Fail to wait for thread %d\n", i);
        }
    }

    return 0;
}

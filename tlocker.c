#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/syscall.h>
#include <pthread.h>

#include "tlocker.h"

#define D(x)
#define TLOCKER_MAX_TID 128

static pid_t tids[TLOCKER_MAX_TID];
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int tlocker_acquire() {
    int i;
    int rem = -1;
    pid_t ctid = syscall(SYS_gettid);
    D(printf("aquiring %d\n", ctid));

    pthread_mutex_lock(&mtx);
    for (i = 0; i < TLOCKER_MAX_TID; i++ ){
        if (rem == -1 && tids[i] == 0) {
            rem = i;
        }
        if (tids[i] == ctid) {
            D(printf("already locked\n"));
            pthread_mutex_unlock(&mtx);
            return 0;
        }
    }

    assert(rem != -1);

    if (i == TLOCKER_MAX_TID) {
        D(printf("Locking at position %d\n", rem));
        tids[rem] = ctid;
        pthread_mutex_unlock(&mtx);
        return 1;
    }
    pthread_mutex_unlock(&mtx);
    D(printf("LOCK FAILED for %d\n", ctid));
    return 0;
//TODO: fixme
}

void tlocker_release() {
    int i;
    pid_t ctid = syscall(SYS_gettid);

    pthread_mutex_lock(&mtx);
    for (i = 0; i < TLOCKER_MAX_TID; i++) {
        if(tids[i] == ctid) {
            tids[i] = 0;
            D(printf("unlocking %d at position %d\n", ctid, i));
            pthread_mutex_unlock(&mtx);
            return;
        }
    }
    pthread_mutex_unlock(&mtx);
    D(printf("UNLOCK FAILED for %d\n", ctid));
    assert(i == TLOCKER_MAX_TID);
}

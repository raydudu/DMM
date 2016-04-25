#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/syscall.h>

#include "tlocker.h"

#define D(x)
#define TLOCKER_MAX_TID 128

static pid_t tids[TLOCKER_MAX_TID];

int tlocker_acquire() {
    int i;
    int rem = -1;
    pid_t ctid = syscall(SYS_gettid);
    D(printf("aquiring %d\n", ctid));

    for (i = 0; i < TLOCKER_MAX_TID; i++ ){
        if (rem == -1 && tids[i] == 0) {
            rem = i;
        }
        if (tids[i] == ctid) {
            D(printf("already locked\n"));
            return 0;
        }
    }

    assert(rem != -1);

    if (i == TLOCKER_MAX_TID) {
        D(printf("Locking at position %d\n", rem));
        tids[rem] = ctid;
        return 1;
    }
    D(printf("LOCK FAILED for %d\n", ctid));
    return 0;
//TODO: fixme
}

void tlocker_release() {
    int i;
    pid_t ctid = syscall(SYS_gettid);

    for (i = 0; i < TLOCKER_MAX_TID; i++) {
        if(tids[i] == ctid) {
            tids[i] = 0;
            D(printf("unlocking %d at position %d\n", ctid, i));
            return;
        }
    }
    D(printf("UNLOCK FAILED for %d\n", ctid));
    assert(i == TLOCKER_MAX_TID);
}

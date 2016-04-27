#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "hooks.h"
#include "config.h"

#define FNAME_MAX_LEN 256
static char *dumps_location;
static pid_t pid;
static int dump_seq = 0;

static void dmm_make_snapshot(int sig) {
    char fn[FNAME_MAX_LEN];
    FILE *fd;
    int save_error = 0;

    snprintf(fn, FNAME_MAX_LEN, "%s/dmm_%d_%04d.json", dumps_location, pid, dump_seq);
    printf("Saving allocation snapshot to: %s\n", fn);
    fd = fopen(fn, "w");
    if (fd == NULL) {
        printf("ERROR: failed to open snapshot file: %s\n", strerror(errno));
        return;
    }

    if (stdlib_snapshot(fd) != 0) {
        printf("Fail to save STDLIB allocations snapshot: %s\n", strerror(errno));
        save_error = 1;
    }


    if (fclose(fd) != 0) {
        printf("Fail to close file: %s\n", strerror(errno));
    }

    if (!save_error) {
        printf("Allocation snapshot succesfully saved to: %s\n", fn);
    } else {
        printf("Allocation snapshot NOT saved\n");
    }

    dump_seq++;
}

static void __attribute__((constructor)) dmm_construct(void) {
    printf("DMM initializing...\n");
    //TODO: redesign if glib support will be addded.
    printf("If you are debugging glib based application please export G_SLICE=always-malloc\n");

    dumps_location = getenv("DMM_DUMP_LOCATION");
    if (dumps_location == NULL) {
        dumps_location = DMM_DEFAULT_DUMP_LOCATION;
    }

    pid = getpid();
    if (signal(SIGUSR2, dmm_make_snapshot) == SIG_ERR) {
        printf ("WARNING: fail to registers signal handler, only resulting dump will be generated\n");
    } else {
        printf("To generate allocation snapshot please execute: kill -USR2 %d\n", pid);
    }

    stdlib_init();

    printf("DMM initialized!\n");
}

static void __attribute__((destructor)) dmm_destruct(void) {
     printf("DMM exiting...\n");
     dmm_make_snapshot(0);

     stdlib_release();

}


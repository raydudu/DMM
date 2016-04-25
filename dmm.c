#include <stdio.h>

#include "hooks.h"

static void onlibload(void) __attribute__((constructor));
static void onlibunload(void) __attribute__((destructor));

void onlibload(void) {
    printf("DMM init\n");
}

static void onlibunload(void) {
     printf("DMM exit\n");
     hooks_stdlib_release();
}


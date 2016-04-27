#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

#include "tlocker.h"
#include "backtrace.h"
#include "symhelper.h"
#include "allocdb.h"
#include "config.h"

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#define D(x)

static allocdb_t *allocs = NULL;

static void *(*libc_malloc)(size_t) = NULL;
static void (*libc_free)(void *) = NULL;
static void *(*libc_calloc)(size_t nmemb, size_t size) = NULL;
static void *(*libc_realloc)(void *ptr, size_t size) = NULL;
static void *(*libc_memalign)(size_t alignment, size_t size) = NULL;

static inline int lock_and_find(void **sym, char *name) {
    int lock = tlocker_acquire();

    if (unlikely(*sym == NULL)) {
        *sym = find_sym("*libc.so*", name);
    }

    if (unlikely(allocs == NULL && lock)) {
        allocs = allocdb_create();
    }

    return lock;
}

void *malloc(size_t size) {
    void *p;

    if (!lock_and_find((void **)&libc_malloc, "malloc")) {
        return libc_malloc(size);
    }

    D(printf("++malloc(%zd)\n", size));
    p = libc_malloc(size);
    if (p != NULL) {
        allocdb_log_alloc(allocs, p, size);
    }
    D(printf("--malloc %p\n", p));

    tlocker_release();

    return p;
}

void free(void *ptr) {

    if (!lock_and_find((void **)&libc_free, "free")) {
        return libc_free(ptr);
    }

    D(printf("++free(%p)\n", ptr));
    libc_free(ptr);
    allocdb_log_release(allocs, ptr);
    D(printf("--free\n"));

    tlocker_release();
}

void *calloc(size_t nmemb, size_t size) {
    void *p;

    if (!lock_and_find((void **)&libc_calloc, "calloc")) {
        return libc_calloc(nmemb, size);
    }

    D(printf("++calloc(%zd, %zd)\n",nmemb, size));
    p = libc_calloc(nmemb, size);
    if (p != NULL) {
        allocdb_log_alloc(allocs, p, nmemb * size);
    }
    D(printf("--calloc %p\n", p));

    tlocker_release();

    return p;
}

void *realloc(void *ptr, size_t size) {
    void *p;
    if (!lock_and_find((void **)&libc_realloc, "realloc")) {
        return libc_realloc(ptr, size);
    }

    D(printf("++realloc(%p, %zd)\n", ptr, size));
    p = libc_realloc(ptr, size);
    if (ptr == NULL && p != NULL) {
        allocdb_log_alloc(allocs, p, size);
        goto exit;
    }
    if (size == 0) {
        allocdb_log_release(allocs, ptr);
        goto exit;
    }
    if (p != NULL) {
        allocdb_log_realloc(allocs, ptr, p, size);
    }
exit:
    D(printf("--realloc %p\n", p));

    tlocker_release();

    return p;
}

void *memalign(size_t alignment, size_t size) {
    void *p;

    if (!lock_and_find((void **)&libc_memalign, "memalign")) {
        return libc_memalign(alignment, size);
    }

    D(printf("++memalign(%zd)\n", size));
    p = libc_memalign(alignment, size);
    if (p != NULL) {
        allocdb_log_alloc(allocs, p, size);
    }
    D(printf("--memalign %p\n", p));

    tlocker_release();

    return p;
}

void *pvalloc(size_t size) {
    /* TODO: proper implementaiton */
    return memalign(DMM_DEFAULT_ALIGNMENT, size);
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
    /* TODO: proper implementaiton */
    *memptr = memalign(DMM_DEFAULT_ALIGNMENT, size);
    return 0;
}

void *aligned_alloc(size_t alignment, size_t size) {
    /* TODO: proper implementaiton */
    return memalign(DMM_DEFAULT_ALIGNMENT, size);
}

void *valloc(size_t size) {
    /* TODO: proper implementaiton */
    return memalign(DMM_DEFAULT_ALIGNMENT, size);
}

void stdlib_init() {
}

int stdlib_snapshot(FILE *fd) {
    int ret = -1;
    tlocker_acquire();

    if (allocs != NULL) {
        ret = allocdb_dump(allocs, fd, "STDLIB");
    }

    tlocker_release();

    return ret;
}

void stdlib_release(){
    tlocker_acquire();

    if (allocs != NULL) {
        allocdb_release(allocs);
        allocs = NULL;
    }

    tlocker_release();
}

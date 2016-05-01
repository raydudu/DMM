#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <wchar.h>

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
static char *(*libc_strdup)(const char *s) = NULL;
static char *(*libc_strndup)(const char *s, size_t n) = NULL;
static wchar_t *(*libc_wcsdup)(const wchar_t *s) = NULL;

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

#undef malloc
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

#undef free
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

#undef calloc
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

#undef realloc
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

#undef memalign
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

#undef pvalloc
void *pvalloc(size_t size) {
    /* TODO: proper implementaiton */
    return memalign(DMM_DEFAULT_ALIGNMENT, size);
}

#undef posix_memalign
int posix_memalign(void **memptr, size_t alignment, size_t size) {
    /* TODO: proper implementaiton */
    *memptr = memalign(DMM_DEFAULT_ALIGNMENT, size);
    return 0;
}

#undef aligned_alloc
void *aligned_alloc(size_t alignment, size_t size) {
    /* TODO: proper implementaiton */
    return memalign(DMM_DEFAULT_ALIGNMENT, size);
}

#undef valloc
void *valloc(size_t size) {
    /* TODO: proper implementaiton */
    return memalign(DMM_DEFAULT_ALIGNMENT, size);
}

#undef strdup
char *strdup(const char *s) {
    void *p;

    if (!lock_and_find((void **)&libc_strdup, "strdup")) {
        return libc_strdup(s);
    }

    D(printf("++strdup(%s)\n", s));
    p = libc_strdup(s);
    if (p != NULL) {
        allocdb_log_alloc(allocs, p, strlen(p));
    }
    D(printf("--strdup %p\n", p));

    tlocker_release();

    return p;
}

#undef strndup
char *strndup(const char *s, size_t n) {
    void *p;

    if (!lock_and_find((void **)&libc_strndup, "strndup")) {
        return libc_strndup(s, n);
    }

    D(printf("++strndup(%s, %zd)\n", s, n));
    p = libc_strndup(s, n);
    if (p != NULL) {
        allocdb_log_alloc(allocs, p, strlen(p));
    }
    D(printf("--strdup %p\n", p));

    tlocker_release();

    return p;
}

#undef wcsdup
wchar_t *wcsdup(const wchar_t *s) {
    void *p;

    if (!lock_and_find((void **)&libc_wcsdup, "wcsdup")) {
        return libc_wcsdup(s);
    }

    D(printf("++wstrdup(%s)\n", s));
    p = libc_wcsdup(s);
    if (p != NULL) {
        allocdb_log_alloc(allocs, p, wcslen(p));
    }
    D(printf("--strdup %p\n", p));

    tlocker_release();

    return p;

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

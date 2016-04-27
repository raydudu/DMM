#ifndef __ALLOCDB_H__
#define __ALLOCDB_H__

#include <stdlib.h>
#include <stdio.h>
#include <htable.h>

typedef struct allocdb_t allocdb_t;
typedef struct allocdb_alloc_t allocdb_alloc_t;
typedef struct allocdb_bt_t allocdb_bt_t;

struct allocdb_bt_t {
    htable_entry_t e; /* must be first element */
    size_t allocs_num;
    allocdb_alloc_t *allocs;
};

struct allocdb_alloc_t {
    htable_entry_t e; /* must be first element */
    size_t size;
    allocdb_bt_t *backtrace;
    allocdb_alloc_t *next;
    allocdb_alloc_t *prev;
};

allocdb_t *allocdb_create();

void allocdb_log_alloc(allocdb_t *db, void *addr, size_t size);
void allocdb_log_release(allocdb_t *db, void *addr);
void allocdb_log_realloc(allocdb_t *db, void *old_addr, void *new_addr, size_t new_size);
void allocdb_release(allocdb_t *db);

int allocdb_dump(allocdb_t *db, FILE *fd, char *subsys);

#endif

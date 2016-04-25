#ifndef __ALLOCDB_H__
#define __ALLOCDB_H__

#include <stdlib.h>

typedef struct allocdb_t allocdb_t;

typedef struct allocdb_allocation_t {
    struct allocdb_allocation_t *next;
    void *addr;
    size_t size;
} allocdb_allocation_t;

typedef struct allocdb_place_t {
    /* backtrace is stored as a key in htable_entry_t */
    size_t allocs_num;
    allocdb_allocation_t *allocs;
} allocdb_place_t;

allocdb_t *allocdb_create();

void allocdb_log_alloc(allocdb_t *db, void *addr, size_t size);
void allocdb_log_release(allocdb_t *db, void *addr);
void allocdb_log_realloc(allocdb_t *db, void *old_addr, void *new_addr, size_t new_size);
void allocdb_release(allocdb_t *db);

void allocdb_dump(allocdb_t *db);

#endif

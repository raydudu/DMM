#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "config.h"
#include <htable.h>
#include "backtrace.h"
#include "allocdb.h"

allocdb_t *allocdb_create() {
    htable_t *h;
    h = htable_new(DMM_HTABLE_SIZE);
    assert(h != NULL);
    return (allocdb_t *)h;
}

static htable_entry_t *allocdb_alloc(void **bt, int bt_len) {
    htable_entry_t *e;

    e = malloc(sizeof(htable_entry_t) + sizeof(void *) * bt_len +
            sizeof(allocdb_place_t));
    assert(e != NULL);
    e->key = e + 1;
    memcpy(e->key, bt, sizeof(void *) * bt_len);
    e->key_len = bt_len;
    e->data = e->key + sizeof(void *) * bt_len;
    memset(e->data, 0, sizeof(allocdb_place_t));

    return e;
}

static void allocdb_free(htable_entry_t *e) {
    allocdb_allocation_t *a;
    allocdb_allocation_t *ta;

    a = ((allocdb_place_t *)e->data)->allocs;
    while (a != NULL) {
        ta = a;
        a = a->next;
        free(ta);
    }

    free(e);
}

void allocdb_log_alloc(allocdb_t *db, void *addr, size_t size) {
    void *bt[DMM_MAX_BACKTRACE_LEN];
    int bt_len;
    htable_entry_t *e;
    allocdb_place_t *p;
    allocdb_allocation_t *a;

    bt_len = get_backtrace(bt, DMM_MAX_BACKTRACE_LEN);
    assert(bt_len != 0);

    e = htable_get((htable_t *)db, bt, bt_len);
    if (e == NULL) {
        //first allocation
        e = allocdb_alloc(bt, bt_len);
        htable_push((htable_t *)db, e);
    }

    p = (allocdb_place_t *)e->data;
    p->allocs_num++;
    a = malloc(sizeof(allocdb_allocation_t));
    assert(a != NULL);
    a->addr = addr;
    a->size = size;
    a->next = p->allocs;
    p->allocs = a;
}

void allocdb_log_release(allocdb_t *db, void *addr) {
    void *bt[DMM_MAX_BACKTRACE_LEN];
    int bt_len;
    htable_entry_t *e;
    allocdb_place_t *p;
    allocdb_allocation_t *a;
    allocdb_allocation_t *pa;

    bt_len = get_backtrace(bt, DMM_MAX_BACKTRACE_LEN);
    assert(bt_len != 0);

    e = htable_get((htable_t *)db, bt, bt_len);
    if (e == NULL) {
        //error condition
        return;
    }
    p = (allocdb_place_t *)e->data;
    a = p->allocs;
    pa = NULL;

    while(a != NULL) {
        if (a->addr == addr) {
            if (pa == NULL) {
                p->allocs = a->next;
            } else {
                pa->next = a->next;
            }
            break;
        }
        pa = a;
        a = a->next;
    }
    if (a == NULL) {
        //error condition
        return;
    }

    p->allocs_num--;
    free(a);
    if (p->allocs_num == 0) {
        htable_pop((htable_t *)db, e->key, e->key_len);
        free(e);
    }
}

void allocdb_log_realloc(allocdb_t *db, void *old_addr, void *new_addr, size_t new_size) {
    void *bt[DMM_MAX_BACKTRACE_LEN];
    int bt_len;
    htable_entry_t *e;
    allocdb_allocation_t *a;

    bt_len = get_backtrace(bt, DMM_MAX_BACKTRACE_LEN);
    assert(bt_len != 0);

    e = htable_get((htable_t *)db, bt, bt_len);
    if (e == NULL) {
        //error condition
        return;
    }

    a = ((allocdb_place_t *)e->data)->allocs;

    while(a != NULL) {
        if (a->addr == old_addr) {
            a->addr = new_addr;
            a->size = new_size;
            break;
        }
        a = a->next;
    }
    if (a == NULL) {
        //error condition
    }
}

void allocdb_release(allocdb_t *db) {
    htable_delete((htable_t *)db, allocdb_free);
}


static void allocdb_print(htable_entry_t *e) {
    allocdb_place_t *p;
    allocdb_allocation_t *a;
    print_backtrace(e->key, e->key_len);
    p = (allocdb_place_t *)e->data;
    printf("  TOTAL ALLOCS: %zu\n", p->allocs_num);
    a = p->allocs;
    while (a != NULL) {
        printf("    ALLOC: [%p, %zd]\n", a->addr, a->size);
        a = a->next;
    }
}

void allocdb_dump(allocdb_t *db) {
    printf("++ALLOCATIONS\n");
    htable_foreach((htable_t *)db, allocdb_print);
    printf("--ALLOCATIONS\n");
}


#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "config.h"
#include <htable.h>
#include <json2f.h>
#include "backtrace.h"
#include "allocdb.h"

#define D(x) x

struct allocdb_t {
    htable_t *allocs;
    htable_t *backtraces;
    pthread_mutex_t mtx;
};

static allocdb_bt_t *allocdb_newbt(void **bt, int bt_len) {
    allocdb_bt_t *p;

    p = malloc(sizeof(*p) + sizeof(void *) * bt_len);
    assert(p != NULL);

    memset(p, 0, sizeof(*p));
    p->e.key = p  + 1;
    memcpy(p->e.key, bt, sizeof(void *) * bt_len);
    p->e.key_len = bt_len;

    return p;
}

static void allocdb_freebt(allocdb_bt_t *p) {
    assert(p->allocs_num == 0 && p->allocs == NULL);
    free(p);
}

static allocdb_alloc_t *allocdb_newalloc(void *addr, size_t size, allocdb_bt_t *p) {
    allocdb_alloc_t *a;

    a = malloc(sizeof(*a) + sizeof(addr));
    assert(a != NULL);

    memset(a, 0, sizeof(*a));
    a->e.key = a + 1;
    *((void **)(a->e.key)) = addr;
    a->e.key_len = sizeof(addr);
    a->size = size;
    a->backtrace = p;

    a->prev = NULL;
    a->next = p->allocs;
    if (p->allocs != NULL) {
        p->allocs->prev = a;
    }
    p->allocs = a;
    p->allocs_num++;

    return a;
}

static void allocdb_freealloc(allocdb_alloc_t *a) {

    assert(a->backtrace->allocs_num > 0);

    a->backtrace->allocs_num--;

    if (a->next != NULL) {
        a->next->prev = a->prev;
    }

    if(a->prev != NULL) {
        a->prev->next = a->next;
    } else {
        assert(a->backtrace->allocs == a);
        a->backtrace->allocs = a->next;
    }

    free(a);
}

void allocdb_log_alloc(allocdb_t *db, void *addr, size_t size) {
    void *bt[DMM_MAX_BACKTRACE_LEN];
    int bt_len;
    allocdb_bt_t *p;
    allocdb_alloc_t *a;

    bt_len = get_backtrace(bt, DMM_MAX_BACKTRACE_LEN);
    assert(bt_len != 0);

    pthread_mutex_lock(&db->mtx);
    p = (allocdb_bt_t *)htable_get(db->backtraces, bt, bt_len);
    if (p == NULL) {
        //first allocation
        p = allocdb_newbt(bt, bt_len);
        htable_push(db->backtraces, &p->e);
    }

    a = allocdb_newalloc(addr, size, p);
    htable_push(db->allocs, &a->e);
    pthread_mutex_unlock(&db->mtx);
}

void allocdb_log_release(allocdb_t *db, void *addr) {
    allocdb_alloc_t *a;
    allocdb_bt_t *p;

    pthread_mutex_lock(&db->mtx);

    a = (allocdb_alloc_t *)htable_get(db->allocs, &addr, sizeof(addr));
    if (a == NULL) {
        pthread_mutex_unlock(&db->mtx);
        void *bt[DMM_MAX_BACKTRACE_LEN];
        int bt_len;
        bt_len = get_backtrace(bt, DMM_MAX_BACKTRACE_LEN);
        assert(bt_len != 0);
        D(printf("%s: Release of nonregistered alloc\n", __func__));
        print_backtrace(bt, bt_len);
        //TODO: error condition, add to separate list?
        return;
    }

    p = a->backtrace;
    htable_scrap(db->allocs, &a->e);
    allocdb_freealloc(a);
    if (p->allocs_num == 0) {
        htable_scrap(db->backtraces, &p->e);
        allocdb_freebt(p);
    }

    pthread_mutex_unlock(&db->mtx);
}

void allocdb_log_realloc(allocdb_t *db, void *old_addr, void *new_addr, size_t new_size) {
    //TODO: rewrite with structures reusability in mind
    allocdb_log_release(db, old_addr);
    allocdb_log_alloc(db, new_addr, new_size);
}

allocdb_t *allocdb_create() {
    allocdb_t *db;

    db = malloc(sizeof(*db));
    assert(db != NULL);

    db->allocs = htable_new(DMM_HTABLE_SIZE);
    db->backtraces = htable_new(DMM_HTABLE_SIZE);
    assert(db->allocs != NULL && db->backtraces != NULL);
    pthread_mutex_init(&db->mtx, NULL);

    D(printf("%s: Allocate tables\n", __func__));
    return db;
}


void allocdb_release(allocdb_t *db) {
    htable_delete(db->allocs, (void (*)(struct htable_entry_t *))allocdb_freealloc);
    htable_delete(db->backtraces, (void (*)(struct htable_entry_t *))allocdb_freebt);
    pthread_mutex_destroy(&db->mtx);
    free(db);
    D(printf("%s: Release tables\n", __func__));
}


static int allocdb_print(htable_entry_t *e, json2f_t *jd) {
    allocdb_bt_t *p = (allocdb_bt_t *)e;
    allocdb_alloc_t *a;
    char **bt;

    json2f_obj(jd);
        json2f_namedarr(jd, "backtrace");
            bt = get_backtrace_symbols(p->e.key, p->e.key_len);
            json2f_arr_str(jd, bt, p->e.key_len);
            free(bt);
        json2f_arr_end(jd);
        json2f_long(jd, "allocations_amount", p->allocs_num);
        json2f_namedarr(jd, "allocations");
            for(a = p->allocs; a != NULL; a = a->next){
                json2f_arr(jd);
                    json2f_arr_ulong(jd,  ((unsigned long *)a->e.key), 1);
                    json2f_arr_ulong(jd, &(a->size), 1);
                json2f_arr_end(jd);
            }
        json2f_arr_end(jd);
    json2f_obj_end(jd);

    return json2f_geterr(jd);
}

int allocdb_dump(allocdb_t *db, FILE *fd, char *subsys) {
    json2f_t jd;
    int ret;

    D(printf("++%s\n", __func__));
    json2f_init(&jd, fd);

    json2f_obj(&jd);
        json2f_str(&jd, "subsystem", subsys);
        json2f_namedarr(&jd, "allocations");

            pthread_mutex_lock(&db->mtx);
            ret = htable_foreach(db->backtraces, (htable_callback_fn)allocdb_print, &jd);
            pthread_mutex_unlock(&db->mtx);

        json2f_arr_end(&jd);
    json2f_obj_end(&jd);
    printf("--%s\n", __func__);
    return ret;
}


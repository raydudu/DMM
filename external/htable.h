#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <stdlib.h>

// #define HTABLE_DEBUG

typedef struct htable_entry_t {
    struct htable_entry_t *next;
    void *key;
    size_t key_len;
    void *data;
} htable_entry_t;

typedef struct htable_t htable_t;

typedef void (*htable_callback_fn)(htable_entry_t *);

htable_t *htable_new(size_t size);
void htable_push(htable_t *table, htable_entry_t *entry);
htable_entry_t *htable_get(htable_t *table, void *key, size_t key_len);
htable_entry_t *htable_pop(htable_t *table, void *key, size_t key_len);

void htable_delete(htable_t *table, void (*entry_free)(htable_entry_t *));

void htable_foreach(htable_t *table, htable_callback_fn);

#ifdef HTABLE_DEBUG
void htable_dump(htable_t *table);
#else
#define htable_dump(x)
#endif

#endif /* __HTABLE_H__ */


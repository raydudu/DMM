/*
 * Copyright (c) 2013 Ray Dudu <raydudu@gmail.com>
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <string.h>

#include "htable.h"

#ifdef HTABLE_DEBUG
#include <stdio.h>
#define D(x) x
#else
#define D(x)
#endif


struct htable_t {
    htable_entry_t **entries;
    size_t size;
};

static unsigned _hash(unsigned *key, size_t key_len, size_t size) {
    unsigned hash = 0;
    int len = key_len / sizeof(hash);
    int rem = key_len % sizeof(hash) - 1;

    while (rem >= 0) {
        hash += ((unsigned)(((char *)(key + len))[rem])) << rem * 8;
        rem--;
    }
    len--;

    while (len >= 0) {
        hash += key[len];
        len--;
    }

    return hash % size;
}


htable_t *htable_new(size_t size) {
    htable_t *table;

    table = malloc(sizeof(htable_t) + sizeof(htable_entry_t *) * size);
    if (table == NULL) {
        return NULL;
    }

    memset(table, 0, sizeof(htable_t) + sizeof(htable_entry_t *) * size);
    table->entries = (htable_entry_t **)(table + 1);
    table->size = size;

    return table;
}

void htable_push(htable_t *table, htable_entry_t *entry) {
    unsigned hash;

    hash = _hash((unsigned *)entry->key, entry->key_len, table->size);

    entry->next = table->entries[hash];
    table->entries[hash] = entry;
    D(printf("%s: h:%04x(%p[%zd], %p)\n", __func__, hash, entry->key, entry->key_len, entry->data));
}

htable_entry_t *htable_get(htable_t *table, void *key, size_t key_len) {
    unsigned hash;
    htable_entry_t *e;

    hash = _hash((unsigned *)key, key_len, table->size);
    e = table->entries[hash];

    while (e != NULL) {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0) {
            break;
        }
        e = e->next;
    }

    D(if (e) printf("%s: h:%04x(%p[%zd], %p)\n", __func__, hash, e->key, e->key_len, e->data);
            else printf("%s h:%04x(NULL)\n", __func__, hash));
    return e;
}

htable_entry_t *htable_pop(htable_t *table, void *key, size_t key_len) {
    unsigned hash;
    htable_entry_t *e;
    htable_entry_t *pe;

    hash = _hash((unsigned *)key, key_len, table->size);
    e = table->entries[hash];
    pe = NULL;

    while (e != NULL) {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0) {
            if (pe == NULL) {
                table->entries[hash] = e->next;
            } else {
                pe->next = e->next;
            }
            break;
        }
        pe = e;
        e = e->next;
    }

    D(if (e) printf("%s: h:%04x(%p[%zd], %p)\n", __func__, hash, e->key, e->key_len, e->data);
            else printf("%s h:%04x(NULL)\n", __func__, hash));

    return e;
}

void htable_delete(htable_t *table, void (*entry_free)(htable_entry_t *)) {
    size_t i;
    htable_entry_t *e;
    htable_entry_t *te;

    for (i = 0; i < table->size; i++) {
        if (table->entries[i] != NULL) {
            e = table->entries[i];
            while (e != NULL) {
                D(printf("%s: h:%04zx(%p[%zd], %p)\n", __func__, i, e->key, e->key_len, e->data));
                te = e;
                e = e->next;
                entry_free(te);
            }
        }
    }

    free(table);
}

void htable_foreach(htable_t *table, htable_callback_fn callback) {
    size_t i;
    for (i = 0; i < table->size; i++) {
        if (table->entries[i] != NULL) {
            htable_entry_t *e = table->entries[i];
            while (e != NULL) {
                callback(e);
                e = e->next;
            }
        }
    }
}

#ifdef HTABLE_DEBUG
void htable_dump(htable_t *table) {
    size_t i;
    for (i = 0; i < table->size; i++) {
        printf( "[%04zu]: ", i);
        if (table->entries[i] != NULL) {
            htable_entry_t *e = table->entries[i];
            while (e != NULL) {
                printf("(%p[%zd], %p) ", e->key, e->key_len, e->data);
                e = e->next;
            }
        }
        printf("\n");
    }
}
#endif

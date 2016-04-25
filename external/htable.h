/*
 * Copyright (c) 2013-2016 Ray Dudu
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
#ifndef __HTABLE_H__
#define __HTABLE_H__

#include <stdlib.h>

// #define HTABLE_DEBUG

typedef struct htable_entry_t {
    struct htable_entry_t *next;
    void *key;
    size_t key_len;
    void *data[0];
} htable_entry_t;

typedef struct htable_t htable_t;

typedef void (*htable_callback_fn)(htable_entry_t *);

htable_t *htable_new(size_t size);
void htable_push(htable_t *table, htable_entry_t *entry);
htable_entry_t *htable_get(htable_t *table, void *key, size_t key_len);
htable_entry_t *htable_pop(htable_t *table, void *key, size_t key_len);
void htable_scrap(htable_t *table, htable_entry_t *entry);

void htable_delete(htable_t *table, void (*entry_free)(htable_entry_t *));

void htable_foreach(htable_t *table, htable_callback_fn);

#ifdef HTABLE_DEBUG
void htable_dump(htable_t *table);
#else
#define htable_dump(x)
#endif

#endif /* __HTABLE_H__ */


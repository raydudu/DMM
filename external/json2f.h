/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Ray Dudu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef __JSON2F_H__
#define __JSON2F_H__

#include <stdio.h>
#include <errno.h>

/* Simple JSON geneator helper which directly writes to file */

typedef struct {
    FILE *fd;
    int error;
    char delim;
} json2f_t;

#define JSON2F_INIT(f) { \
    .fd = f, \
    .error = 0, \
    .delim = ' ', \
}
//TODO: null, double named objects, anon arrays

void inline json2f_init(json2f_t *jd, FILE *fd) {
    jd->fd = fd;
    jd->error = 0;
    jd->delim = ' ';
}

void inline json2f_obj(json2f_t *jd) {
    if (jd->error) {
        return;
    }
    if (fprintf(jd->fd, "%c{", jd->delim) < 0) {
        jd->error = errno;
    }
    jd->delim = ' ';
}

void inline json2f_namedobj(json2f_t *jd, char *name) {
    if (jd->error) {
        return;
    }
    if (fprintf(jd->fd, "%c\"%s\":{", jd->delim, name) < 0) {
        jd->error = errno;
    }
    jd->delim = ' ';
}

void inline json2f_obj_end(json2f_t *jd) {
    if (jd->error) {
        return;
    }
    if (fprintf(jd->fd, "}\n") < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_int(json2f_t *jd, char *name, int i) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\"%s\":%d", jd->delim, name, i) < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_long(json2f_t *jd, char *name, long l) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\"%s\":%ld", jd->delim, name, l) < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_ulong(json2f_t *jd, char *name, unsigned long ul) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\"%s\":%lu", jd->delim, name, ul) < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_str(json2f_t *jd, char *name, char *s) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\"%s\":\"%s\"", jd->delim, name, s) < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_bool(json2f_t *jd, char *name, int b) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\"%s\":%s", jd->delim, name, b?"true":"false") < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_namedarr(json2f_t *jd, char *name) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\"%s\":[", jd->delim, name) < 0) {
        jd->error = errno;
    }
    jd->delim = ' ';
}

void inline json2f_arr(json2f_t *jd) {
    if (jd->error) {
        return;
    }
    if(fprintf(jd->fd, "%c\[", jd->delim) < 0) {
        jd->error = errno;
    }
    jd->delim = ' ';
}

void inline json2f_arr_end(json2f_t *jd) {
    if (jd->error) {
        return;
    }
    if (fprintf(jd->fd, "]") < 0) {
        jd->error = errno;
    }
    jd->delim = ',';
}

void inline json2f_arr_str(json2f_t *jd, char **arr, int len) {
    int i;
    if (jd->error) {
        return;
    }
    for (i = 0; i < len; i++) {
        if (fprintf(jd->fd, "%c\"%s\"", jd->delim, arr[i]) < 0) {
            jd->error = errno;
            break;
        }
        jd->delim = ',';
    }
}

void inline json2f_arr_int(json2f_t *jd, int *arr, int len) {
    int i;
    if (jd->error) {
        return;
    }
    for (i = 0; i < len; i++) {
        if (fprintf(jd->fd, "%c%d", jd->delim, arr[i]) < 0) {
            jd->error = errno;
            break;
        }
        jd->delim = ',';
    }
}

void inline json2f_arr_long(json2f_t *jd, long *arr, int len) {
    int i;
    if (jd->error) {
        return;
    }
    for (i = 0; i < len; i++) {
        if (fprintf(jd->fd, "%c%ld", jd->delim, arr[i]) < 0) {
            jd->error = errno;
            break;
        }
        jd->delim = ',';
    }
}

void inline json2f_arr_ulong(json2f_t *jd, unsigned long *arr, int len) {
    int i;
    if (jd->error) {
        return;
    }
    for (i = 0; i < len; i++) {
        if (fprintf(jd->fd, "%c%ld", jd->delim, arr[i]) < 0) {
            jd->error = errno;
            break;
        }
        jd->delim = ',';
    }
}

int inline json2f_geterr(json2f_t *jd) {
    return jd->error;
}

#endif /* __JSON2F_H__ */

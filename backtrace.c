#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>
#include <assert.h>

void print_backtrace(void **addr, int len) {
    int i;
    char **symbollist = backtrace_symbols(addr, len);

    for(i = 0; i < len && addr[i] != NULL; i++) {
        printf("[%02d]: %s\n", i, symbollist[i]);
    }
    free(symbollist);
}

#ifdef __arm__
/* long fp = *(topfp - 3);
 * long sp = *(topfp - 2);
 * long lr = *(topfp - 1);
 * long pc = *(topfp - 0);
 */
int get_backtrace(void **addr, int len) {
    int i;
    long *topfp;

    assert(addr != NULL && len > 0);

    /* topfp = get_fp(); */
    asm volatile("mov %[fp], fp" : [fp] "=r" (topfp));

    /* Top frame */
    addr[0] = *(topfp); /* pc */

    /* Middle frames */
    for (i = 0; i < len && *(topfp - 3) != 0; i++) {
        addr[i] = *(topfp - 1); /* lr */
        topfp = (long *)(*(topfp - 3)); /* fp */
    }

    /* Top frame */
    if (i != len) {
        addr[i] = *(topfp); /* pc */
    }
    return i;
}
#else
/* Genric implementation of backtracer.
 * May not work in some conditions
 */

int get_backtrace(void **addr, int len) {
    int i;

    backtrace(addr, len);
    for (i = 0; i < len; i++) {
        if (addr[i] == NULL) {
            return i;
        }
    }

    return len;
}

#endif

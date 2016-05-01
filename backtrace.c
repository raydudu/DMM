#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <execinfo.h>
#include <assert.h>

#include "config.h"

char **get_backtrace_symbols(void **addr, int len) {
    return backtrace_symbols(addr, len);
}

void print_backtrace(void **addr, int len) {
    int i;
    char **symbollist = get_backtrace_symbols(addr, len);

    for(i = 0; i < len && addr[i] != NULL; i++) {
        printf("[%02d]: %s\n", i, symbollist[i]);
    }
    free(symbollist);
}

void __attribute__ ((unused)) out_of_stack_marker() {
};

#ifdef DMM_OWN_BACKTRACE
#  ifdef __arm__
/* long fp = *(topfp - 3);
 * long sp = *(topfp - 2);
 * long lr = *(topfp - 1);
 * long pc = *(topfp - 0);
 */

//assuming stack is 16M
//TODO use getrlimit(RLIMIT_STACK ... or parse /proc/self/maps
#define STACK_ADDR_MASK (-1L << (CHAR_BIT * sizeof(unsigned long) - 8))

int get_backtrace(void **addr, int len) {
    int i;
    unsigned long **topfp;
    unsigned long sp;

    assert(addr != NULL && len > 0);

    asm volatile("mov %[topfp], fp" : [topfp] "=r" (topfp));
    asm volatile("mov %[sp], sp" : [sp] "=r" (sp));
    sp = sp & STACK_ADDR_MASK;

    /* Top frame */
    //addr[0] = *(topfp); /* pc */

    /* Middle frames */
    for (i = 0; i < len; i++) {
        if (*(topfp - 3) == NULL) {
            // Bottom frame, lr invalid
            addr[i] = *(topfp); /* pc */
            i++;
            break;
        }
        if (((unsigned long)(*(topfp - 3)) & STACK_ADDR_MASK) != sp) {
            // Next fp is out of stack, lr valid, pc useless
            addr[i] = *(topfp - 1); /* lr */
            i++;
            if (i < len) {
                addr[i] = &out_of_stack_marker;
                i++;
            }
            break;
        }

        addr[i] = *(topfp - 1); /* lr */
        topfp = (unsigned long **)*(topfp - 3); /* fp */
    }

    return i;
}
#  else
#    error "Now OWN backtrace for the platform"
#  endif
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

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <execinfo.h>
#include <assert.h>

#include "config.h"

void print_backtrace(void **addr, int len) {
    int i;
    char **symbollist = backtrace_symbols(addr, len);

    for(i = 0; i < len && addr[i] != NULL; i++) {
        printf("[%02d]: %s\n", i, symbollist[i]);
    }
    free(symbollist);
}

static void out_of_stack_marker() {
};

#ifdef DMM_OWN_BACKTRACE
#  ifdef __arm__
/* long fp = *(topfp - 3);
 * long sp = *(topfp - 2);
 * long lr = *(topfp - 1);
 * long pc = *(topfp - 0);
 */

//assuming stack is less than 64k
#define STACK_ADDR_MASK (((1UL << 16) - 1) << (CHAR_BIT * sizeof(unsigned long) - 16))

int get_backtrace(void **addr, int len) {
    int i;
    unsigned long **topfp;
    unsigned long sp;

    assert(addr != NULL && len > 0);

    /* topfp = get_fp(); */
    asm volatile("mov %[topfp], fp" : [topfp] "=r" (topfp));
    asm volatile("mov %[sp], sp" : [sp] "=r" (sp));
    sp &= STACK_ADDR_MASK;

    /* Top frame */
    //addr[0] = *(topfp); /* pc */

    /* Middle frames */
    for (i = 0; i < len; i++) {
        if (((unsigned long)topfp & STACK_ADDR_MASK) != sp) {
            //out of stack
            addr[i] = &out_of_stack_marker;
            break;
        }

        if (*(topfp - 3) == NULL) {
            addr[i] = *(topfp); /* pc */
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

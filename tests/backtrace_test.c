#include <stdio.h>

#include "../backtrace.h"


static void __attribute__ ((noinline)) f1() {
    int len;
    void *arr[32];

    printf("f1: ++get_backtrace\n");
    len = get_backtrace(arr, 32);
    printf("f1: --get_backtrace %d\n", len);
    printf("f1: ++print_backtrace\n");
    print_backtrace(arr, len);
    printf("f1: --print_backtrace\n");

}

int main(int argc, char **argv) {
    int len;
    void *arr[32];

    printf("++get_backtrace\n");
    len = get_backtrace(arr, 32);
    printf("--get_backtrace %d\n", len);
    printf("++print_backtrace\n");
    print_backtrace(arr, len);
    printf("--print_backtrace\n");

    f1();

    return 0;
}

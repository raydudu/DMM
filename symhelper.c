#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#ifdef DMM_WITH_ELFHACKS
#include <elfhacks.h>

void *find_sym(char *lib, char *sym) {
	eh_obj_t libh;
    void *symaddr;
	if(eh_find_obj(&libh, lib)) {
		printf("%s: %s not found in memory\n", __func__, lib);
        abort();
	}

	if(eh_find_sym(&libh, sym, &symaddr)) {
		printf("%s: %s not found in %s", __func__, sym, lib);
        abort();
	}

	eh_destroy_obj(&libh);
    return symaddr;
}

#else

void *find_sym(char *lib, char *sym) {
    void *symaddr;

    symaddr = dlsym(RTLD_NEXT, sym);
    assert(symaddr != NULL);
    return symaddr;
}
#endif

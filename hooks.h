#ifndef __HOOKS_H__
#define __HOOKS_H__

#include <stdio.h>

void stdlib_init();
int stdlib_snapshot(FILE *fd);
void stdlib_release();
#endif /* __HOOKS_H__ */

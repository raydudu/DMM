#ifndef __BACKTRACE_H__
#define __BACKTRACE_H__

int get_backtrace(void **addr, int len);
void print_backtrace(void **addr, int len);

#endif /* __BACKTRACE_H__ */

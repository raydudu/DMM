#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __arm__
#define DMM_WITH_ELFHACKS
#define DMM_OWN_BACKTRACE
#elif defined (__i386__) || defined(__x86_64__)
//#define DMM_WITH_ELFHACKS
#elif defined (__mips__)
//#define DMM_WITH_ELFHACKS
#else
#error "Please define your architecture"
#endif

#define DMM_HTABLE_SIZE 16381

#define DMM_MAX_BACKTRACE_LEN 32

#define DMM_DEFAULT_ALIGNMENT 8

#define DMM_DEFAULT_DUMP_LOCATION "./"

#define DMM_DUMP_SIGNALNO (SIGRTMIN + 5)

#endif /* __CONFIG_H__ */

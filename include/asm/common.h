#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __ASM__
#define EXPORT_SYM(x) .globl x
#define IMPORT_SYM(x) .extern x

#endif

#endif

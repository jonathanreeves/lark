#ifndef __TRAPS_H__
#define __TRAPS_H__

#ifndef __ASM__
void traps_init(void);

/* these are actually functions defined in tbase.S */

/* general trap with or without error code */
extern void trap_with_err(void);
extern void trap_no_err(void);

/* traps with specific implementations */
extern void trap_divide(void);
extern void trap_breakpoint(void);
extern void trap_opcode(void);
extern void trap_double(void);
extern void trap_bad_tss(void);
extern void trap_stack(void);
extern void trap_gpf(void);
extern void trap_page(void);

#endif

#endif

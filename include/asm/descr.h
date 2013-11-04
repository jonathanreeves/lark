#ifndef __DESCR_H__
#define __DESCR_H__


#ifdef __ASM__

#else

#include <lark/types.h>

void load_gdt_cs(u32 base, u32 size_kb, u8 idx);
void load_gdt_ds(u32 base, u32 size_kb, u8 idx); 
void load_gdt_tss(u32 base, u8 idx); 
void load_idt_descriptor(u32 addr, u16 sel, u8 idx);

extern u32 kgdt_ptr;
extern u32 kidt_ptr;
extern u8 *kidt;
extern u8 *kgdt;
extern u8 *ktss;

#endif

#endif


#ifndef __SYSINFO_H__
#define __SYSINFO_H__

/* support the usual 256 interrupts */
#define IDT_MAX_INTERRUPTS 256
/* support 32 gdt entries */
#define GDT_MAX_SELECTORS 32
/* size of the above structure in bytes (keep this up to date) */
#define BOOT_INFO_SIZE (13*4)

#define LARK_KERNEL_CS_IDX 1
#define LARK_KERNEL_DS_IDX 2
#define LARK_KERNEL_TSS_IDX 3

#define LARK_KERNEL_CS (LARK_KERNEL_CS_IDX << 3)
#define LARK_KERNEL_DS (LARK_KERNEL_DS_IDX << 3)
#define LARK_KERNEL_TSS (LARK_KERNEL_TSS_IDX << 3)

#define SYS_MEM_START 0x0
#define LARK_IRQ_START 0x20

/* this is safe because stack grows down and BIOS doesn't use anything
 * below this point
 */
#define LARK_INIT_STACK 0xA0000

#endif

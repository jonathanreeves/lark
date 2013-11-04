#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#ifndef __ASM__

typedef struct __aout_symbol_table {
	unsigned long tabsize;
	unsigned long strsize;
	unsigned long addr;
	unsigned long reserved;
} aout_symbol_table_t;

typedef struct __elf_section_header_table {
	unsigned long num;
	unsigned long size;
	unsigned long addr;
	unsigned long shndx;
} elf_section_header_table_t;


typedef struct __grub_boot_info {
	unsigned long flags;
	unsigned long mem_lower;
	unsigned long mem_upper;
	unsigned long boot_device;
	unsigned long cmdline;
	unsigned long mods_count;
	unsigned long mods_addr;
	union {
		aout_symbol_table_t aout_sym;
		elf_section_header_table_t elf_sec;
	} u;
	unsigned long mmap_length;
	unsigned long mmap_addr;
} grub_boot_info_t;

#endif

#define MBOOT_PAGE_ALIGN 1
#define MBOOT_MEM_INFO 2
#define MBOOT_MAGIC 0x1badb002
#define MBOOT_FLAGS (MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO)
#define MBOOT_CHECKSUM -(MBOOT_MAGIC + MBOOT_FLAGS)

#endif

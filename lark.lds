/* the LARK linker script, by Jonathan Reeves. Taken primarily from
 * the default ld linker script and edited appropriately for LARK. 
 * For information on which sections should be included, use objdump
 * on relocatable object files
 */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kernel_head)
SECTIONS
{
	. = 0x00100000;
	_text = .;
	.text           :
	{
		*(.text)
	} =0x90909090
	_etext = .;

	.rodata		: { *(.rodata .rodata.* .gnu.linkonce.r.*) }
	.rodata1	: { *(.rodata1) }

	. = ALIGN(4096);
	.data           :
	{
		*(.data)
	}
	_edata = .;
	. = ALIGN(4096);
	__bss_start = .;
	.bss            :
	{
		*(.bss)
		*(COMMON) 
	}
	. = ALIGN(4);
	__bss_end = .;

	_end = .;
	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }

	/DISCARD/ : { *(.note.GNU-stack) }

}

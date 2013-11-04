#ifndef __PCIBIOS_H__
#define __PCIBIOS_H__

void pcibios_init(void);

#define PCIBIOS_LOWLIM 0x000e0000
#define PCIBIOS_HIGHLIM 0x000fffff
#define PCIBIOS_SIG (('_' << 0) | ('3' << 8) | ('2' << 16) | ('_' << 24))
#define PCIBIOS_SERVICE (('$' << 0) | ('P' << 8) | ('C' << 16) | ('I' << 24))

/* varios bios functions */
#define PCIBIOS_FUNC_ID 0xb1 /* this is in ah */
#define PCIBIOS_PRESENT 0x01 /* the rest are in al */
#define PCIBIOS_FIND_DEV 0x02
#define PCIBIOS_FIND_CLASS_CODE 0x03
#define PCIBIOS_SPECIAL_CYCLE 0x06
#define PCIBIOS_READ_CONFIG_BYTE 0x08
#define PCIBIOS_READ_CONFIG_WORD 0x09
#define PCIBIOS_READ_CONFIG_DWORD 0x0a
#define PCIBIOS_WRITE_CONFIG_BYTE 0x0b
#define PCIBIOS_WRITE_CONFIG_WORD 0x0c
#define PCIBIOS_WRITE_CONFIG_DWORD 0x0d
#define PCIBIOS_GET_IRQ_ROUTE 0x0e
#define PCIBIOS_SET_PIC_IRQ 0x0f

/* return codes */
#define PCIBIOS_SUCESS 0x00
#define PCIBIOS_NOT_SUPPORTED 0x81
#define PCIBIOS_BAD_VENDOR 0x83
#define PCIBIOS_DEV_NOT_FOUND 0x86
#define PCIBIOS_BAD_REG_NUM 0x87
#define PCIBIOS_SET_FAILED 0x88
#define PCIBIOS_BUF_SMALL 0x89

#endif

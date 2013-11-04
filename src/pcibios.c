#include <asm/pcibios.h>
#include <lark/video.h>
#include <asm/sysinfo.h>

#include <lib/string.h>

/* this will get filled in as we go */
static struct pcibios {
	struct service_dir {
		unsigned int sig;
		unsigned int entry;
		unsigned char rev;
		unsigned char len;
		unsigned char checksum;
		unsigned char pad[5];
	} service_dir;

	/* the standard leaves this open for more than one service */
	struct service {
		unsigned int base;
		unsigned int len;
		unsigned int offset;
	} service;

	unsigned char present;
	unsigned char hw_mech;
	unsigned char if_major;
	unsigned char if_minor;
	unsigned char lastbus;
} pcibios = {{0, 0, 0, 0, 0, {0, 0, 0, 0, 0}}, {0, 0, 0}, 0, 0, 0, 0, 0};

/* keep these far pointers separate */
static struct {
	unsigned long addr;
	unsigned short cs;
} service_dir_far = {0, LARK_KERNEL_CS};

static struct {
	unsigned long addr;
	unsigned short cs;
} pcibios_far = {0, LARK_KERNEL_CS};

static inline char do_checksum(char *dat, int nbytes)
{
	int i;
	char sum = 0;
	for(i = 0; i < nbytes; i++) {
		sum += dat[i];
	}
	return sum;
}


unsigned int pcibios_get_service_dir(struct pcibios *dat)
{
	unsigned int addr;
	struct service_dir *bios32;

	for(addr = PCIBIOS_LOWLIM; addr < PCIBIOS_HIGHLIM; addr += 16){
		bios32 = (struct service_dir *)addr;
		/* first check for signature */
		if (bios32->sig != PCIBIOS_SIG) {
			continue;
		}

		/* signature found, now do the checksum */
		if( !do_checksum((char *)bios32, (bios32->len << 4)) ) {
			/* we found the bios */
			memcpy(&dat->service_dir, bios32, sizeof(struct service_dir));
			return addr;
		} 
	}

	/* if we made it here, no bios was found */	
	return 0;
}

unsigned int pcibios_get_service(struct pcibios *dat, unsigned int service)
{
	unsigned int ax, base, len, offset;

	asm volatile(
		"lcall *(%%esi)\n\t"
		: "=a" (ax), "=b" (base), "=c" (len), "=d" (offset)
		: "0" (service), "1" (0), "S" (&service_dir_far)
		: "memory"
	);

	switch(ax & 0x00ff) {
		case 0x00:
			dat->service.base = base;
			dat->service.len = len;
			dat->service.offset = offset;
			return base+offset;
		case 0x80:
			return 0;
		case 0x81:
			return 0;
		default:
			printk("PCI BIOS bug found\n");
			return 0;
	}
	return 0;
}

#define PCI_PRESENT_SIG (('P' << 0) | ('C' << 8) | ('I' << 16) | (' ' << 24))
#define PCI_PRESENT_FUNC ( (PCIBIOS_FUNC_ID<<8) | PCIBIOS_PRESENT )
int pcibios_present(struct pcibios *info)
{
	int ax, bx, cx;
	char al, ah;
	int sig;

	asm volatile(
		"lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=a" (ax), "=b" (bx), "=c" (cx), "=d" (sig)
		: "0" (PCI_PRESENT_FUNC), "S" (&pcibios_far)
		: "memory"
	); 

	al = (ax & 0x000000ff);
	ah = (ax & 0x0000ff00)>>8;

	if (ah == 0 && sig == PCI_PRESENT_SIG) {
		info->present = ah;
		info->hw_mech = al;
		info->if_major = ((bx & 0x0000ff00) >> 8);
		info->if_minor = (bx & 0x00ff);
		info->lastbus = (cx & 0x00ff);
	}
	return ah;
}

/* FIXME: implement these from the standard
pcibios_find_device
pcibios_find_class
pcibios_get_irqinfo
pcibios_set_pci_irq
*/

#define PCIBIOS_SPECIAL_FUNC ( (PCIBIOS_FUNC_ID<<8) | PCIBIOS_SPECIAL_CYCLE )
int pcibios_special_cycle(int bus, unsigned int data)
{
	int ax;

	asm volatile(
		"lcall *(%%esi); cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=a" (ax)
		: "0" (PCIBIOS_SPECIAL_FUNC), 
		  "b" ((bus & 0x00ff) << 8),
		  "d" (data),
		  "S" (&pcibios_far)
		: "memory"
	);

	return ((ax & 0x0000ff00)>>8); 
}


int pcibios_read_config(int bus, int dev, int reg, int len, unsigned char *dat)
{
	unsigned int res; 
	unsigned int ax;
	unsigned int bx;

	switch(len) {
		case 1:
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_READ_CONFIG_BYTE;
			break;
		case 2:
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_READ_CONFIG_WORD;
			break;
		case 4:
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_READ_CONFIG_DWORD;
			break;
		default:
			printk("PCIBIOS: warning, bad size for bios read\n");
			printk("PCIBIOS: defaulting to dword\n");
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_READ_CONFIG_DWORD;
	}

	bx = (bus << 8) | dev;
	asm volatile(
		"lcall *(%%esi)\n\t"
		: "=a" (res), "=c" (*dat)
		: "0" (ax), "b" (bx), "D" (reg), "S" (&pcibios_far)
		: "memory"
	);
	return ((res & 0x0000ff00) >> 8);
}

int pcibios_write_config(int bus, int dev, int reg, int len, unsigned short dat)
{
	unsigned int res; 
	unsigned int ax;
	unsigned int bx;

	switch(len) {
		case 1:
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_WRITE_CONFIG_BYTE;
			break;
		case 2:
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_WRITE_CONFIG_WORD;
			break;
		case 4:
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_WRITE_CONFIG_DWORD;
			break;
		default:
			printk("PCIBIOS: warning, bad size for bios write\n");
			printk("PCIBIOS: defaulting to dword\n");
			ax = (PCIBIOS_FUNC_ID<<8) | PCIBIOS_WRITE_CONFIG_DWORD;

	}

	bx = (bus << 8) | dev;
	asm volatile(
		"lcall *(%%esi)\n\t"
		: "=a" (res) 
		: "0" (ax), "b" (bx), "c" (dat), "D" (reg), "S" (&pcibios_far)
		: "memory"
	);
	return ((res & 0x0000ff00) >> 8);

}


void pcibios_init(void)
{
	unsigned int addr;
	int err;

	addr = pcibios_get_service_dir(&pcibios);
	if (addr == 0) {
		return;
	} else {
		service_dir_far.addr = pcibios.service_dir.entry;
		printk("\nPCI BIOS: service directory found at 0x%08x\n", addr);
	}

	addr = pcibios_get_service(&pcibios, PCIBIOS_SERVICE);
	if(addr == 0){
		return;
	} else {
		pcibios_far.addr = addr;
		printk("PCI BIOS: PCIBIOS service found at 0x%08x\n", addr);
	}

	err = pcibios_present(&pcibios);
	if(err != 0) {
		printk("PCI BIOS: PCIBIOS service not available\n");
		return;
	}
	printk("PCI BIOS: PCIBIOS version number is %d.%d\n\n", pcibios.if_major, pcibios.if_minor);
}

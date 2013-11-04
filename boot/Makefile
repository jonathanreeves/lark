KERNEL=$(TOPLEVEL)/lark
GRUB=/sbin/grub
UID=$(shell whoami)

all: fd.img
ifeq ($(BOOT), grub)
.PHONY: fd.img
ifneq ($(UID), root)
fd.img:
	@echo "****************************************"
	@echo "grub boot image requires root privileges" 
	@echo "try 'make boot BOOT=other' if you don't"
	@echo "have such privileges."
	@echo "****************************************"
else
fd.img: device.map stage1 stage2 $(KERNEL) menu.lst
	dd if=/dev/zero of=$@ bs=1440k count=1
	/sbin/mke2fs -F $@
	mount -o loop $@ /mnt
	cp stage1 /mnt
	cp stage2 /mnt
	cp $(KERNEL) /mnt
	cp menu.lst /mnt
	umount /mnt
	echo 'install=(fd0)/stage1 (fd0) (fd0)/stage2 0x8000 p (fd0)/menu.lst'| $(GRUB) --batch --device-map=device.map 

endif

.PHONY: device.map
device.map:
	echo '(fd0) fd.img' > $@

.PHONY: menu.lst
menu.lst:
	echo -e 'timeout 0\n#Entry 0\ntitle lark\nroot (fd0)\nkernel /lark' > $@
else

fd.img: bootsect.bin setup.bin lark.bin 
	cat bootsect.bin setup.bin lark.bin > $@

bootsect.bin: bootsect.S setup.bin lark.bin
	$(CC) -c bootsect.S $(SFLAGS) \
	-DNUM_SETUP_SECTORS=`./numsecs setup.bin` \
	-DNUM_KERN_SECTORS=`./numsecs lark.bin`
	$(OBJCOPY) -O binary bootsect.o $@

setup.bin: setup.S lark.bin
	gcc -c setup.S $(SFLAGS) -DNUM_KERN_SECTORS=`./numsecs lark.bin`
	$(OBJCOPY) -O binary setup.o $@
	./pad $@ 512

lark.bin: $(KERNEL)
	$(OBJCOPY) -O binary $(KERNEL) $@

endif

clean:
	rm -f fd.img
	rm -f menu.lst
	rm -f device.map
	rm -f *.bin *.o

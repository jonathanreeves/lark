# Top level makefile for LARK
# LARK stands for Lightweight Arbiter or Realtime Kernel

TOPLEVEL=$(PWD)

CC=gcc
AS=gcc
LD=ld
AR=ar
NM=nm
OBJCOPY=objcopy

INCLUDEDIR=$(TOPLEVEL)/include
CFLAGS=-fomit-frame-pointer -O2 -Wall -I$(INCLUDEDIR)
SFLAGS=-I$(INCLUDEDIR)
LDFLAGS=

export CC
export AS
export LD
export AR
export OBJCOPY

export CFLAGS
export SFLAGS
export LDFLAGS
export INCLUDEDIR
export TOPLEVEL

ifndef BOOT
BOOT=other
endif
export BOOT

KSECTIONS=src
LINKSCRIPT=lark.lds

all: lark 

.PHONY: src
src: 
	$(MAKE) -C $@


boot: lark
	$(MAKE) -C $@

lark: $(KSECTIONS) $(LINKSCRIPT) Makefile
	$(LD) -T $(LINKSCRIPT) $(patsubst %,%/lark.o,$(KSECTIONS)) \
		$(LDFLAGS) -o $@ 
	$(NM) $@ > system.map
	

include Rules.mak

clean:
	rm -rf *.o
	rm -f lark
	rm -f system.map
	$(MAKE) -C src clean
	$(MAKE) -C boot clean

#
# Makefile for the linux memory manager.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...

CFLAGS += -I ../include -m32

.c.o:
	$(CC) $(CFLAGS) -c $<
.s.o:
	$(AS) -o $*.o $<
.c.s:
	$(CC) $(CFLAGS) -S $<

OBJS	= memory.o swap.o mmap.o kmalloc.o vmalloc.o

mm.o: $(OBJS)
	$(LD) -m elf_i386 -r -o mm.o $(OBJS)

dep:
	$(CPP) -M *.c -I../include > .depend

#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif

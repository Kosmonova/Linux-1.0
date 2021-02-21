#
# Makefile for the linux MS-DOS-filesystem routines.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

CFLAGS += -I../../include -m32 -D__KERNEL__ -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce #-Wall

.c.s:
	$(CC) $(CFLAGS) -S $<
.c.o:
	$(CC) $(CFLAGS) -c $<
.s.o:
	$(AS) -o $*.o $<

OBJS=	namei.o inode.o file.o dir.o misc.o fat.o

msdos.o: $(OBJS)
	$(LD) -r -m elf_i386 -o msdos.o $(OBJS)

dep:
	$(CPP) -M *.c -I../../include > .depend

#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif

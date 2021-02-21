#
# Makefile for the linux ext2-filesystem routines.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

CFLAGS += -I../../include -D__KERNEL__ -m32 -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce #-Wall

.c.s:
	$(CC) $(CFLAGS) -S $<
.c.o:
	$(CC) $(CFLAGS) -c $<
.s.o:
	$(AS) -o $*.o $<

OBJS=	acl.o balloc.o bitmap.o dcache.o dir.o file.o fsync.o \
	ialloc.o inode.o ioctl.o namei.o super.o symlink.o truncate.o

ext2.o: $(OBJS)
	$(LD) -r -m elf_i386 -o ext2.o $(OBJS)

dep:
	$(CPP) -M -I../../include *.c > .depend

clean:
	@rm *.o
#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif

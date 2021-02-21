#
# Makefile for the linux ext-filesystem routines.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

CFLAGS  = -g -m32 -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce -I../../include #-Wall

.c.s:
	$(CC) $(CFLAGS) -S $<
.c.o:
	$(CC) $(CFLAGS) -c $<
.s.o:
	$(AS) -o $*.o $<

OBJS=	freelists.o truncate.o namei.o inode.o \
	file.o dir.o symlink.o fsync.o

ext.o: $(OBJS)
	$(LD) -r -o ext.o $(OBJS)

dep:
	$(CPP) -M *.c -I../../include > .depend

#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif

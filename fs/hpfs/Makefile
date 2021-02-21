#
# Makefile for the linux HPFS filesystem routines.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

CFLAGS  = -g -m32 -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce #-Wall

.c.s:
	$(CC) $(CFLAGS) -S $<
.c.o:
	$(CC) $(CFLAGS) -c $<
.s.o:
	$(AS) -o $*.o $<

OBJS=	hpfs_fs.o

hpfs.o: $(OBJS)
	ln -f hpfs_fs.o hpfs.o

dep:
	$(CPP) -M *.c -I../../include > .depend

#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif

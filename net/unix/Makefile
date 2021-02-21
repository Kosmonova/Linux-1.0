#
# Makefile for the UNIX Protocol Family.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...

CFLAGS += -I../../include -D__KERNEL__ -m32 -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce

.c.o:
	$(CC) $(CFLAGS) \
	-c -o $*.o $<
.s.o:
	$(AS) -o $*.o $<
.c.s:
	$(CC) $(CFLAGS) \
	-S -o $*.s $<

OBJS	= sock.o proc.o

unix.o: $(OBJS)
	$(LD) -r -m elf_i386 -o unix.o $(OBJS)

dep:
	$(CPP) -M *.c -I../../include > .depend

tar:
	tar -cvf /dev/f1 .

clean:
	@rm *.o

#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif

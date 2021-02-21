#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

CFLAGS += -I../include -D__KERNEL__ -m32 -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce
# LDFLAGS = -m elf_i386

.S.s:
	$(CPP) -I../include -traditional $< -o $*.s
.c.s:
	$(CC) $(CFLAGS) -S $<
.s.o:
	$(AS) -c -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) -I../include -c $<

OBJS  = sched.o sys_call.o traps.o irq.o dma.o fork.o \
 	panic.o printk.o vsprintf.o sys.o module.o ksyms.o exit.o \
	signal.o mktime.o ptrace.o ioport.o itimer.o \
	info.o ldt.o time.o

all: kernel.o

kernel.o: $(OBJS)
	$(LD) -m elf_i386 -r -o kernel.o $(OBJS)
	sync

sys_call.s: sys_call.S

sys_call.o: sys_call.s

sched.o: sched.c
	$(CC) $(CFLAGS) $(PROFILING) -I../include -fno-omit-frame-pointer -c $<

ksyms.lst: ksyms.S ../include/linux/autoconf.h
	$(CPP) -I../include $(CFLAGS) $< > $@
 
ksyms.s: ksyms.sh ksyms.lst
	sh $< > $@

ksyms.o: ksyms.s

dep:
	$(CPP) -M -I ../include *.c > .depend

clean:
	@rm *.o

dummy:

#
# include a dependency file if one exists
#
ifeq (.depend,$(wildcard .depend))
include .depend
endif



HEAD = head.o
SYSTEM = ../tools/zSystem

zOBJECTS = $(HEAD) inflate.o unzip.o misc.o

CFLAGS += -g -DSTDC_HEADERS -I ../include -I../include/linux  -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce $(TEST)

LDFLAGS	= -m elf_i386 -e startup_32 

.c.s:
	$(CC) $(CFLAGS) -S -o $*.s $<
.s.o:
	$(AS) -c -o $*.o $<
.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $<
.S.o:
	$(CC) $(CFLAGS) -c -o $*.o $<

all:	zSystem

zSystem:	piggy.o $(zOBJECTS)
	$(LD) $(LDFLAGS) -o zSystem -Ttext 1000 $(zOBJECTS) piggy.o
	@nm zSystem | grep -v '\(compiled\)\|\(\.o$$\)\|\( a \)' | \
		sort > zSystem.map

clean:
	@rm *.o *.s zSystem piggyback xtract

head.o:	head.S
	gcc -D__KERNEL__ -DSTDC_HEADERS -I ../include -I../include/linux  -fno-builtin -fno-stack-protector -fomit-frame-pointer -fstrength-reduce -E -traditional -Iinclude head.S -o head.s
	$(AS) -c -o head.o head.s

xtract: xtract.c
	$(CC) -I../include xtract.c -o xtract 

piggyback: piggyback.c
	$(CC) -I../include piggyback.c -o piggyback 

piggy.o:	$(SYSTEM) xtract piggyback
	@cat $(SYSTEM) | gzip -9 | ./piggyback > piggy.o

$(SYSTEM):
	$(MAKE) -C .. tools/zSystem




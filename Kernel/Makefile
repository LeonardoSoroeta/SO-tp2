include Makefile.inc

KERNEL=kernel.bin
SOURCES=$(wildcard *.c) $(wildcard arch/*.c) $(wildcard ds/*.c) $(wildcard ipc/*.c) $(wildcard sys/*.c) $(wildcard util/*.c)
SOURCES_ASM=$(wildcard asm/*.asm) $(wildcard arch/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.asm.o)
LOADERSRC=loader.asm

# LIBGCC=/Users/martin/Documents/TPSO/cross/lib/gcc/x86_64-elf/4.9.0/libgcc.a
ifdef LINUX
LIBGCC=
else
LIBGCC=/usr/local/cross/lib/gcc/x86_64-elf/4.9.0/libgcc.a
endif

LOADEROBJECT=$(LOADERSRC:.asm=.asm.o)
STATICLIBS=${LIBGCC}

THEINCLUDES=-I./include -I./
ifndef LINUX
THEINCLUDES+=$(CROSSROOT)/include
endif

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM) $(STATICLIBS) -Map map

%.o: %.c
		$(GCC) $(GCCFLAGS) $(THEINCLUDES) -c $< -o $@

%.asm.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o *.o *.bin sys/*.o arch/*.o

.PHONY: all clean

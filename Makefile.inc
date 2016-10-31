ifndef LINUX
BASE := $(dir $(lastword $(MAKEFILE_LIST)))
-include ${BASE}/Toolchain.inc
endif

ifdef LINUX
CROSSROOT=
else
CROSSROOT=/usr/local/cross/lib/gcc/x86_64-elf/4.9.0
endif
# CROSSROOT=/Users/martin/Documents/TPSO/cross/lib/gcc/x86_64-elf/4.9.0

GCC?=gcc
LD?=ld
AR?=ar
ASM?=nasm

# -mno-sse -mno-sse2 Preguntar si se puede sacar
CFLAGS=-m64 -fno-exceptions -fno-asynchronous-unwind-tables -mno-mmx -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -mno-red-zone  -ffreestanding -nostdlib -fno-common -pipe
STDFLAGS=-std=c99 -Wall -Wno-multichar -Werror
GCCFLAGS=${CFLAGS} ${STDFLAGS}
ARFLAGS=rvs
ASMFLAGS=-felf64
include ../Makefile.inc

ECHOSOURCES=echo.c
ECHOOBJECTS=$(ECHOSOURCES:.c=.o)
ECHOMODULE=echo.bin

HALTSOURCES=halt.c
HALTOBJECTS=$(HALTSOURCES:.c=.o)
HALTMODULE=halt.bin

all: $(ECHOMODULE) $(HALTMODULE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(ECHOMODULE): $(ECHOOBJECTS)
	$(GCC) $(GCCFLAGS) -o ../payload/$(ECHOMODULE) $(LIBC) $(ECHOOBJECTS) ../libc.a

$(HALTMODULE): $(HALTOBJECTS)
	$(GCC) $(GCCFLAGS) -o ../payload/$(HALTMODULE) $(LIBC) $(HALTOBJECTS) ../libc.a

clean:
	rm -rf echo/*.o

.PHONY: all clean

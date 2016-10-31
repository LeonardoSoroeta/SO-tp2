MP=mp.bin
SOURCES=$(wildcard *.c)
ifndef LINUX
SOURCES+=/usr/local/lib/libargp.a
endif

all: $(MP)

$(MP): $(SOURCES)
	gcc $(SOURCES) -o $(MP)

clean:
	rm -rf $(MP)

.PHONY: all clean

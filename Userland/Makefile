SAMPLE_DATA=01-sampleDataModule.bin

all:	clean libc shell sampleDataModule sampleIdleModule # commands

libc:
	@cd libc; make
	@echo "LibC built"

shell:	libc
	@cd shell; make
	@echo "Shell built"

sampleIdleModule:	libc
	@cd idle; make
	@echo "Idle built"

# commands: shell
# 	@cd commands; make
# 	@echo "Commands built"

sampleDataModule: commands
	gcc fsloader/main.c -o fsloader.pgm; ./fsloader.pgm; cat headers.bin content.bin > $(SAMPLE_DATA)
	@echo "Data module built"

clean:
	@cd shell; make clean
	@cd libc; make clean
	@cd commands; make clean
	- rm -rf *.bin


.PHONY: clean all libc shell sampleDataModule

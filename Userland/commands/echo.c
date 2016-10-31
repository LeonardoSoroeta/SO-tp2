#include <stdio.h>
#include <libc.h>

int main(char** args, int argc) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s ", args[i]);
	}
	putc('\n');

	return 0;
}

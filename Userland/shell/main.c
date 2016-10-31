#include <stdio.h>
#include <shell.h>
#include <stdint.h>
#include <stdlib.h>
#include <libc.h>
#include <game.h>
#include <sound.h>

void sig_handler(int sig);

void sig_handler(int sig) {

	//fprintf(STDERR, "Received signal %d\n", sig);

}

void start_sound();

void doA();
void doB();

void doA() {
	printf("A\n");
}

void doB() {
	printf("B\n");
}

int main(unsigned int pcount, char * pgname[], void * pgptrs[]) {

	pid_t gamePID;
	// int fds[2];
	// char *test = "ta";
	// char a = 'a';

	// pipe(fds);

	// printf("fds[0]=%d fds[1]=%d\n", fds[0], fds[1]);

	gamePID = fork("game");

	// if (gamePID == 0) {
	// 	printf("A %d\n",gamePID );
	// 	doA();
	// 	while(1);
	// } else {
	// 	printf("B %d\n",gamePID );
	// 	doB();
	// 	while(1);
	// }

	if (gamePID == 0) {

		// write(fds[1], test, 1);

		game();

	} else {

		start_sound();
	
	}

	// while (1);


	exit(0);
	return 0;
}


void start_sound() {

	pid_t soundPID;

	soundPID = fork("sound");

	if (soundPID == 0) {
		sound();
	} else {
		// read(fds[0], &a, 1);

		// printf("pipe received %c\n", a);

		init_shell();

		register_event(MOUSE, &mouse_event);

		signal(&sig_handler);

		while (1) {
			update_shell();
		}
	}

}


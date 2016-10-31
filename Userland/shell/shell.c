#include <shell.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <libc.h>
#include <command.h>


unsigned char listening_mouse = 1;

char *shell_buffer;
int curr_pos = 0;

char game_enabled = 0;

uint32_t history_index = 0;
uint32_t history_inserting_index = 0;
history_entry_t *cmd_history;

cmd_entry_t *cmd_table;
int cmd_count = (sizeof(cmd_table) / sizeof(cmd_entry_t));

void init_shell() {

	shell_buffer = (char*)malloc(SHELL_BUFFER_SIZE);

	clean_buffer();

	cmd_table = (cmd_entry_t*)calloc(32, sizeof(cmd_entry_t));

	initialize_cmd_table();

	cmd_history = (history_entry_t*)calloc(100, sizeof(history_entry_t));

	cmd_history[0].buffer = (char*)calloc(128, sizeof(char));

	print_shell_text();
}

void print_shell_text() {
	printf(SHELL_TEXT);
}

void print_shell_error() {
	fprintf(STDERR, "We are sorry, \"%s\" command does not exist. \nFor full list of commands please type: \"help\"\n", shell_buffer);
}

void update_shell() {
	char key;
	key = getc();

	refresh_shell(key);
}

void refresh_shell(char key) {

	if (key == 0) {
		return;
	}

	if (key == '\n') {
		// print newline
		putc('\n');
		// mark end of buffer
		shell_buffer[curr_pos] = 0;

		// push_history();
		
		// increase number of commands executed
		history_inserting_index++;
		// update index
		history_index = history_inserting_index;

		cmd_history[history_inserting_index].buffer = calloc(128, sizeof(char));

		execute_command(shell_buffer);
		print_shell_text();
		clean_buffer();

	} else if ( key == '\b' ) {
		if ( curr_pos > 0) {
			putc('\b');
			// update in parallel
			shell_buffer[--curr_pos] = '\0';
			cmd_history[history_inserting_index].buffer[curr_pos] = '\0';
		} else {
			make_beep();
		}

	} else if (curr_pos >= SHELL_BUFFER_SIZE - 2) {
		//sound beep
		make_beep();
		return;

	} else {
		putc(key);
		// update in parallel
		shell_buffer[curr_pos] = key;
		cmd_history[history_inserting_index].buffer[curr_pos] = key;

		curr_pos++;
	}
}

void push_history() {
	while (cmd_history[history_index].buffer != 0)
		history_index++;
	history_index++;
}

void execute_command(char* buffer) {
	int argc,
	cmd_len;
	char * args[64];
	int res;
	// pid_t pid;
	// int *ret = (int*)0;

	int cmd_no = parse_command(buffer);
	if( cmd_no == -1) {
		print_shell_error();
		return;
	}

	cmd_len = strlen(cmd_table[cmd_no].name);
	argc = get_arguments(buffer + cmd_len, args);
	init_proc( (void*)cmd_table[cmd_no].func, cmd_table[cmd_no].name, argc, args);

	res = getresult();

	// waitpid(pid, ret);

	if (res != 0) {
		fprintf(STDERR, "Exit with status %d\n", res);
	}
	// cmd_table[cmd_no].func(argc, args);
}

int parse_command(char* buffer) {
	int i = 0,
	cmd_no = -1;

	if(buffer[0] == '\0') {
		return -1;
	}

	while (cmd_no == -1 && i < cmd_count) {
		if (substr(cmd_table[i].name, buffer)) {
			cmd_no = i;
		}
		i++;
	}

	i = strlen(cmd_table[cmd_no].name);
	char next = buffer[i];

	if(next == ' ' || next == '\0'){
		return cmd_no;
	}

	return -1;
}

unsigned int get_arguments(char* buffer, char ** args) {
	int arg = 0;	// current argument
	int i = 0;		// current buffer index

	char * ptr = NULL;

	while (buffer[i] != '\0' && buffer[i] == ' ') {
		i++;
	}

	while (buffer[i] != '\0' && arg < 64) {
	// guard condition
		if (buffer[i] == ' ') {
			ptr = NULL;
			buffer[i] = '\0';
			i++;
			continue;
		}

		if (ptr == NULL) {
			ptr = buffer + i;
			args[arg] = ptr;
			arg++;
		}

		i++;
	}
	return arg;
}


void fill_buffer(char *s) {
	int i = 0;
	while (*s != 0) {
		shell_buffer[i++] = *s;
		s++;
	}
}

void fill_shell(char *s) {
	// int i = 0;
	clean_shell();
	while (*s != 0) {
		refresh_shell(*s);
		s++;
	}
}

void clean_buffer() {
	int i = 0;
	while(i < SHELL_BUFFER_SIZE && i < curr_pos) {
		shell_buffer[i] = '\0';
		i++;
	}
	curr_pos = 0;
}

void clean_shell() {
	int i = 0;
	int b_curr_pos = curr_pos;
	while(i < SHELL_BUFFER_SIZE && i < b_curr_pos) {
		refresh_shell('\b');
		i++;
	}
	// printf("curr pos at %d\n", curr_pos);
}

//Returns the index of the command in the command table, if such command does not exist, returns -1
int get_cmd_index(char * cmd_name)  {
	int i;
	for(i = 0; i < cmd_count; i++) {
		if (strcmp(cmd_name, cmd_table[i].name) == 0) {
			return i;
		}
	}
	return -1;
}


cmd_entry_t* get_command_table() {
	return cmd_table;
}


//Prints list of available commands
void print_commands() {
	int i = 0;
	int max_width = 0;
	int aux = 0;
	for (i = 0; i < cmd_count && cmd_table[i].name != 0; i+=2) {
		max_width = max_width < ( aux = strlen(cmd_table[i].name)) ? aux : max_width;
	}

	for(i = 0 ; i < cmd_count && cmd_table[i].name != 0; i+=2) {
		aux = strlen(cmd_table[i].name);
		printf("\t%s", cmd_table[i].name);

		for (; aux < max_width; aux++) {
			putc(' ');
		}
		if ( i+1 < cmd_count ) {
			putc('\t');
			printf("%s", cmd_table[i+1].name);
		}
		putc('\n');
	}

}

void show_history(int argc, char **argv) {
	int i = 0;
	for (i = 0; i < history_inserting_index; i++) {
		printf("%s\n", cmd_history[i].buffer);
	}
	exit(0);
}

int get_cmd_count() {
	return cmd_count;
}

void run_job() {

	int cmd_no = -1;
	char *pname;

	pname = getpname();

	cmd_no = get_cmd_index(pname);

	if (cmd_no == -1) {
		exit(1);
	}
	
	cmd_table[cmd_no].func(0,NULL);
	// commands(0, NULL);

}

void job_processor(int argc, char **argv) {

	pid_t pid;

	while (argc--) {
		pid = fork(argv[argc]);

		if (pid == 0) {
			run_job();
		}
	}

	// pid_t pid;
	// int i;

	// for (; i < argc; i++) {

	// 	if ( parse_command(argv[i]) == -1) {
	// 		fprintf(STDERR, "Command %s does not exist\n", argv[i]);
	// 		exit(1);
	// 	}
	// }

	// printf("Tasks to run:\n");

	// for (i = 0; i < argc; i++) {
	// 	printf("%d. %s\n", i+1, argv[i]);
	// }

	// for (; argc > 0; argc--) {

	// 	pid = fork(argv[argc-1]);

	// 	if (pid == 0) {

	// 		run_job();

	// 	} else {

	// 		// printf("pid = %d\n", pid);

	// 	}

	// }


	exit(0);

}

void malloc_test(int argc, char **argv) {

	void *b20;

	printf("Requesting 20 bytes\n");

	b20 = malloc(20);

	printf("malloc(20)=0x%x\n", (uint64_t)b20);

	printf("Requesting 160 bytes\n");

	b20 = malloc(160);

	printf("malloc(160)=0x%x\n", (uint64_t)b20);

	printf("Requesting 1260 bytes\n");

	b20 = malloc(1260);

	printf("malloc(1260)=0x%x\n", (uint64_t)b20);

	printf("Requesting 11 bytes\n");

	b20 = malloc(11);

	printf("malloc(11)=0x%x\n", (uint64_t)b20);

	

	exit(0);
}


void add_command(int it, char* name, func_t func, char* help) {

	cmd_table[it].name = name;
	cmd_table[it].help = help;
	cmd_table[it].func = func;
}

void show_ipc(int argc, char** argv) {
	

	printf("1. Signals\n");
	printf("2. Pipes\n");
	printf("3. Named Pipes\n");

	exit(0);
}

void initialize_cmd_table() {
	/**
	 * Command names
	 */
	
	int i = 0;

	// create_cmd(i++, "echo", &echo, "Echo repeats the input string following echo statement \n example: \"echo Hello I am using echo\"");
	// create_cmd(i++, "clear", &clear, "Clears the screen, uses no arguments, therefore will ignore any ones received\n");
	// create_cmd(i++, "date", &date, "Prints current system date on screen with format: \"dd/mm/yy\"\n");
	add_command(i++,"echo",&echo, "Echo repeats the input string following echo statement \n example: \"echo Hello I am using echo\"");
	add_command(i++,"clear",&clear, "Clears the screen");
	add_command(i++,"date",&date, "Prints current system date on screen with format: \"dd/mm/yy\"\n");
	add_command(i++,"time",&time, "Prints current system time on screenwith format: \"hs:mm:ss\"\n");
	add_command(i++,"man",&help, "Displays information about following command, syntaxt: \"man \"command_name\"\"\n");
	add_command(i++,"help",&commands, "Displays list of available commands\n");
	add_command(i++,"commands",&commands, "Displays list of available commands\n");
	add_command(i++,"beep",&beep, "Beeps a short sound");
	add_command(i++,"play_sound",&play_sound, "Play one of the available sounds");
	add_command(i++,"history", &show_history, "Displays a record of all executed commands\n");
	add_command(i++,"show_mouse", &mouse_show, "Shows the mouse cursor (enabled by default)\n");
	add_command(i++,"hide_mouse", &mouse_hide, "Hides the mouse cursor\n");
	add_command(i++,"mouse_sensitivity", &mouse_sensitivity, "Sets the sensitivity of the mouse (1-100)\n");
	add_command(i++,"mouse_test", &mouse_test, "Shows a demo of mouse use\n");
	add_command(i++,"shutdown", &halt_system, "Halts the system\n");
	add_command(i++,"exit", &halt_system, "Alias from shutdown. Halts the system\n");
	add_command(i++,"getpid", &cmd_getpid, "Gets the process ID\n");
	add_command(i++,"getppid", &cmd_getppid, "Gets the process\' parent ID\n");
	add_command(i++,"jobs", &job_processor, "Runs following command in background, syntaxt: \"jobs \"command_name\"\"\n");
	add_command(i++,"malloc_test", &malloc_test, "Shows a demo of malloc use\n");
	add_command(i++,"while1", &while1, "Runs an infinite loop\n");
	add_command(i++,"ps", &cmd_ps, "Displays a snapshot of current processes\n");
	add_command(i++,"kill", &cmd_kill, "Terminates a given process, syntaxt: \"kill PID\n");
	add_command(i++,"stop", &cmd_stop, "Stops a given process, syntaxt: \"stop PID\"\n");
	add_command(i++,"cont", &cmd_cont, "Wake a given stopped process, syntaxt: \"cont PID\"\n");
	add_command(i++,"ls", &cmd_ls, "Displays list of available files\n");
	add_command(i++,"play", &game_start, "Starts the classic \'Asteroids\' game\n");
	add_command(i++,"cat", &cat, "Concatenate files and print on the standart output\n");
	add_command(i++,"music_on", &music_on, "Enable music (if disabled)\n");
	add_command(i++,"music_off", &music_off, "Disable music (if enabled)\n");
	add_command(i++,"ipc", &show_ipc, "Lists available IPC\n");

	cmd_count = i;
}

void history_go_back() {

	if (history_index > 0) {
		fill_shell(cmd_history[--history_index].buffer);
	}

}

void history_go_forward() {


	if (history_index < history_inserting_index) {
		fill_shell(cmd_history[++history_index].buffer);
	}

}

int mouse_event(event_id a, uint64_t x, uint64_t y, uint64_t flag) {

	if (listening_mouse) {

		if (flag & 0x1) {
			// Left Click
			history_go_back();
		}

		if (flag & 0x2) {
			// Right Click
			history_go_forward();
		}

		if (flag & 0x4) {
			// Middle Click
			if (*shell_buffer != 0)
				refresh_shell('\n');
		}

	} else {
		if (!game_enabled && flag & 0x1) {
			unhang_click();
		}
	}
	

	return 0;
}

void set_listening_mouse(unsigned char b) {
	listening_mouse = b;
}

void set_game_enabled(unsigned char b) {
	game_enabled = b;
}

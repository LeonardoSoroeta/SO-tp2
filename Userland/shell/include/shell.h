#ifndef __SHELL_H__
#define __SHELL_H__
#include <command.h>
#include <stdint.h>
#include <libc.h>

#define SHELL_BUFFER_SIZE 128
#define SHELL_TEXT "$> "

typedef void (*func_t)(int argc, char** argv);

typedef struct {
	char* name;
	char* help;
	func_t func;
} cmd_entry_t;

typedef struct {
	char *buffer;
} history_entry_t;

void init_shell(void);
void update_shell(void);
void refresh_shell(char);
void push_history(void);
void execute_command(char*);
int parse_command(char*);
void fill_buffer(char*);
void fill_shell(char*);
void clean_buffer(void);
void clean_shell(void);
unsigned int get_arguments(char* buffer, char ** args);
int get_cmd_index(char *);
cmd_entry_t* get_command_table(void);
void print_commands(void);
void show_history();
int get_cmd_count(void);
void initialize_cmd_table(void);
void print_shell_text();
void print_shell_error();

void history_go_back();
void history_go_forward();

int mouse_event(event_id a, uint64_t b, uint64_t c, uint64_t d);
void set_listening_mouse(unsigned char b);
void set_game_enabled(unsigned char b);

#endif
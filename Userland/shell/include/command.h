#ifndef __COMMANDS_H__
#define __COMMANDS_H__
#include <shell.h>
#include <stdint.h>

#define INVALID_DATE "Invalid date inserted, please respect dd/mm/yy format & insert a valid date.\n"
#define INVALID_TIME "No arguments were sent to command settime format musut be ss:mm:hh \n"
#define INVALID_SCREEN_SAVER_TIME "Invalid time delay was introduced for screen saver, please use following format:\n \"ss:mm:hh\". Note timer cannot exceed 24hs nor be shorter than 5 seconds\n"

typedef enum {
	kReadingInitial,
	kReadingDuration,
	kReadingPitch,
	kReadingOctave
} NoteParserState;

typedef enum {
	NOTE_C = 0,
	NOTE_C_SHARP,
	NOTE_D,
	NOTE_D_SHARP,
	NOTE_E,
	NOTE_F,
	NOTE_F_SHARP,
	NOTE_G,
	NOTE_G_SHARP,
	NOTE_A,
	NOTE_A_SHARP,
	NOTE_B,
	NOTE_P
} note_id;

typedef struct {
	uint8_t duration;
	uint8_t pitch;
	uint8_t octave;
} note_t;

typedef struct {
	char title[30];
	uint8_t default_duration;
	uint8_t default_octave;
	uint16_t beat;
	note_t notes[100];
	uint8_t read_notes;
} song_t;

void echo(int, char**);
void while1(int, char**);
void clear(int, char**);
void date(int, char**);
void time(int, char**);
void set_date(int, char**);
void settime(int, char**);
int parse_date(char*, int*, int*, int*);
int parse_time(char*, int*, int*, int*);
int is_num(char);
int valid_time(int, int, int);
int valid_date(int, int, int);
int is_leap_year(int);
void getchar_cmd(int, char**);
void printf_cmd(int, char**);
void scanf_cmd(int, char**);
void reset_vect(char vec[]);
void help_error_print();
void help(int, char**); 
void halt_system(int, char**);
void beep(int, char**);
void mouse_hide(int, char**);
void mouse_show(int, char**);
void mouse_sensitivity(int, char**);
void mouse_test(int, char**);
void credits(int, char**);
void play_sound(int, char**);

void cmd_getpid(int argc, char** argv);
void cmd_getppid(int argc, char** argv);
void cmd_ps(int argc, char**argv);
void cmd_kill(int argc, char**argv);
void cmd_stop(int argc, char**argv);
void cmd_cont(int argc, char**argv);
void vid_mode_test(int argc, char** argv);
void cmd_ls(int argc, char** argv);
void game_start(int argc, char** argv);
void music_on(int argc, char** argv);
void music_off(int argc, char** argv);
void cat(int argc, char** argv);


void commands(int, char**);
void print_ascii_table(int, char**);
void setcolor(int, char**);
void screen_saver_delay(int, char**);

void unhang_click();
void hang_for_click();

int parse_sounds(char* raw, song_t* parsed_songs);
void parse_sound(char *raw, song_t* parse_song);
void parse_title(char *raw, song_t* parse_song);
void parse_header(char *raw, song_t* parse_song);
void parse_body(char *raw, song_t* parse_song);

#endif
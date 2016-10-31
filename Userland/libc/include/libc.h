#ifndef __LIBC_H__
#define __LIBC_H__

#include <stdint.h>

/* common structs */

typedef uint64_t pid_t;

typedef void (*sig_t)(int);

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint8_t year;
} date_t;

typedef enum {
	READ = 0,		// 0
	WRITE,			// 1
	OPTS,				// 2
	GET_TIME,		// 3
	SET_TIME,		// 4
	BEEP,				// 5
	PLAY_SOUND, // 6
	EVENT,			// 7
	WAIT,				// 8
	SHUTDOWN,		// 9
	HALT,				// 10 A
	FORK, 			// 11 B
	GETPID, 		// 12 C
	GETPPID, 		// 13 D
	GETRESULT, 	// 14 E
	WAITPID,		// 15 F
	INIT_PROC,	// 16 10
	VIDMODE,		// 17 11
	MALLOC,			// 18 12
	CALLOC,			// 19 13
	FREE,				// 20 14
	PS,					// 21 15
	KILL,				// 22 16
	STOP,				// 23 17
	HOLD, 			// 24 18
	CONT,				// 25 19
	SIGNAL,			// 26 1a
	BEGINCTX,		// 27 1b
	MKFIFO,			// 28 1c
	LS,					// 29 1d
	FOPEN,			// 30 1e
	FSIZE,			// 31 1f
	SIZE,				// 32 20
	FCLOSE,			// 33 21
	CLOSE,			// 34 22
	PIPE,				// 35 23
	DUP2,				// 36 24
	FSEEK,			// 37 25
	SEEK,				// 38 26
	SCREENSHOT,	// 39 27
	GETPNAME,		// 40 28
	FSTART,			// 41 29
	START,			// 42 2a
	YIELD,			// 43 2b
	EXIT				// 44 2c
} syscall_id;

typedef enum {
	KEYBOARD = 0,
	TIMER_TICK,
	MOUSE = 3
} event_id;

/* syscalls */

extern uint64_t _syscaller(int, int, int, int, int);

int get_time(date_t *);
int set_time(date_t *);
int write(int fd, char * str, int size);
int read(int fd, char * str, int size);
int beepwo(uint32_t, uint32_t);
int make_beep();
int shutdown(void);
int halt(void);
int set_opts(uint8_t fd, uint32_t request, uint16_t options);
int register_event(uint8_t id, int(*)(event_id, uint64_t, uint64_t, uint64_t));
int wait(uint64_t);
pid_t fork(char*);
char* getpname(void);
pid_t getpid(void);
pid_t getppid(void);
int vid_mode(void);
int getresult(void);
int ps();
int kill(pid_t);
int stop(pid_t);
int hold(pid_t);
int cont(pid_t);
int signal(sig_t);
int waitpid(pid_t, int*);
int mkfifo(char*);
pid_t init_proc(void*,char*, int, char**);
void *malloc(uint32_t);
void *calloc(uint32_t, uint32_t);
int free(void*);
int ls();
int fopen(char* path);
uint64_t fsize(char* path);
uint64_t size(int fd);
void* fstart(char* path);
void* start(int fd);
uint64_t fseek(char* path, uint64_t offset);
uint64_t seek(int fd, uint64_t offset);
int fclose(char* path);
int close(int fd);
int pipe(int fds[2]);
int dup2(int fd1, int fd2);
int yield(void);

uint64_t ctx_begin();
uint32_t* screenshot();

int exit(int);

#define REQUEST_CLEAR_SCREEN 0
#define REQUEST_SET_COLOR 1
#define REQUEST_RESET 2
#define REQUEST_HIDE 3
#define REQUEST_SHOW 4
#define REQUEST_SENSITIVTY 5

#define COLOR_BLACK					(char)0
#define COLOR_BLUE					(char)1
#define COLOR_GREEN					(char)2
#define COLOR_CYAN					(char)3
#define COLOR_RED						(char)4
#define COLOR_MAGENTA				(char)5
#define COLOR_BROWN					(char)6
#define COLOR_LIGHT_GREY		(char)7
#define COLOR_DARK_GREY			(char)8
#define COLOR_LIGHT_BLUE		(char)9
#define COLOR_LIGHT_GREEN		(char)10
#define COLOR_LIGHT_CYAN		(char)11
#define COLOR_LIGHT_RED			(char)12
#define COLOR_LIGHT_MAGENTA	(char)13
#define COLOR_LIGHT_YELLOW	(char)14
#define COLOR_WHITE					(char)15

#define build_colour(fg, bg) ( (bg << 4) | (fg & 0x0f) )

#endif
#include "include/libc.h"
#include "include/stdio.h"

int write(int fd, char * str, int size) {
	return _syscaller((int)WRITE, fd, (uintptr_t)str, size, 0xe);
}

int read(int fd, char * str, int size) {
	return _syscaller((int)READ, fd, (uintptr_t)str, size, 0xe);
}

int make_beep() {
	return _syscaller((int)BEEP, 0xe, 0xe, 0xe, 0xe);
}

int beepwo(uint32_t length, uint32_t freq) {
	return _syscaller((int)PLAY_SOUND, length, freq, 0xe, 0xe);
}

int get_time(date_t *date) {
	return _syscaller((int)GET_TIME, (uintptr_t)date, 0xe, 0xe, 0xe);
}

int set_time(date_t *date) {
	return _syscaller((int)SET_TIME, (uintptr_t)date, 0xe, 0xe, 0xe);
}

int halt(void) {
	return _syscaller((int)HALT, 0xe, 0xe, 0xe, 0xe);
}

int shutdown(void) {
	return _syscaller((int)SHUTDOWN, 0xe, 0xe, 0xe, 0xe);
}

int set_opts(uint8_t fd, uint32_t request, uint16_t options) {
	return _syscaller((int)OPTS, fd, request, options, 0xe);
}

int register_event(uint8_t id, int (*listener)(event_id, uint64_t, uint64_t, uint64_t)) {
	return _syscaller((int)EVENT, id, (uintptr_t)listener, 0xe, 0xe);
}

int wait(uint64_t millis) {
	return _syscaller((int)WAIT, millis, 0xe, 0xe, 0xe);
}

pid_t fork(char *name) {
	return (pid_t)_syscaller((int)FORK, (uintptr_t)__builtin_return_address(0), (uintptr_t)name, 0xe, 0xe);
}

// pid_t fork(char *name) {
// 	pid_t pid;
// 	// pid = (pid_t)_syscaller((int)FORK, (uintptr_t)__builtin_return_address(0), (uintptr_t)name, 0xe, 0xe);
// 	pid = (pid_t)_syscaller((int)FORK, (uintptr_t)&&r, (uintptr_t)name, 0xe, 0xe);
// r:
// 	return pid;
// }

int ps() {
	return _syscaller((int)PS, 0xe, 0xe, 0xe, 0xe);
}

int kill(pid_t pid) {
	return _syscaller((int)KILL, pid, 0xe, 0xe, 0xe);
}

int stop(pid_t pid) {
	return _syscaller((int)STOP, pid, 0xe, 0xe, 0xe);
}

int hold(pid_t pid) {
	return _syscaller((int)HOLD, pid, 0xe, 0xe, 0xe);
}

int cont(pid_t pid) {
	return _syscaller((int)CONT, pid, 0xe, 0xe, 0xe);
}

int signal(sig_t sig) {
	return _syscaller((int)SIGNAL, (uintptr_t)sig, 0xe, 0xe, 0xe);
}

char* getpname() {
	return (char*)(uintptr_t)_syscaller((int)GETPNAME, 0xe, 0xe, 0xe, 0xe);
}

pid_t getpid() {
	return (pid_t)_syscaller((int)GETPID, 0xe, 0xe, 0xe, 0xe);
}

pid_t getppid() {
	return (pid_t)_syscaller((int)GETPPID, 0xe, 0xe, 0xe, 0xe);
}

int vid_mode() {
	return _syscaller((int)VIDMODE, 0xe, 0xe, 0xe, 0xe);
}

int getresult() {
	return _syscaller((int)GETRESULT, 0xe, 0xe, 0xe, 0xe);
}

int waitpid(pid_t pid, int* exit_code) {
	return _syscaller((int)WAITPID, pid, (uintptr_t)exit_code, 0xe, 0xe);
}

pid_t init_proc(void *ptr, char *name, int argc, char **argv) {
	return (pid_t)_syscaller((int)INIT_PROC, (uintptr_t)ptr, (uintptr_t)name, argc, (uintptr_t)argv);
}

void *malloc(uint32_t size) {
	return (void*)(uintptr_t)_syscaller((int)MALLOC, size, 0xe, 0xe, 0xe);
}

void *calloc(uint32_t amount, uint32_t size) {
	return (void*)(uintptr_t)_syscaller((int)CALLOC, amount, size, 0xe, 0xe);
}

int free(void* ptr) {
	return _syscaller((int)FREE, (uintptr_t)ptr, 0xe, 0xe, 0xe);
}

int mkfifo(char* ptr) {
	return _syscaller((int)MKFIFO, (uintptr_t)ptr, 0xe, 0xe, 0xe);
}

int fopen(char* ptr) {
	return _syscaller((int)FOPEN, (uintptr_t)ptr, 0xe, 0xe, 0xe);
}

uint64_t fsize(char* ptr) {
	return _syscaller((int)FSIZE, (uintptr_t)ptr, 0xe, 0xe, 0xe);
}

uint64_t size(int fd) {
	return _syscaller((int)SIZE, fd, 0xe, 0xe, 0xe);
}

void* fstart(char* ptr) {
	return (void*)(uintptr_t)_syscaller((int)FSTART, (uintptr_t)ptr, 0xe, 0xe, 0xe);
}

void* start(int fd) {
	return (void*)(uintptr_t)_syscaller((int)START, fd, 0xe, 0xe, 0xe);
}

uint64_t fseek(char* ptr, uint64_t offset) {
	return _syscaller((int)FSEEK, (uintptr_t)ptr, offset, 0xe, 0xe);
}

uint64_t seek(int fd, uint64_t offset) {
	return _syscaller((int)SEEK, fd, offset, 0xe, 0xe);
}

int fclose(char* ptr) {
	return _syscaller((int)FCLOSE, (uintptr_t)ptr, 0xe, 0xe, 0xe);
}

int close(int fd) {
	return _syscaller((int)CLOSE, fd, 0xe, 0xe, 0xe);
}

int pipe(int fds[2]) {
	return _syscaller((int)PIPE, (uintptr_t)fds, 0xe, 0xe, 0xe);
}

int dup2(int fd1, int fd2) {
	return _syscaller((int)DUP2, fd1, fd2, 0xe, 0xe);
}

int ls() {
	return _syscaller((int)LS, 0xe, 0xe, 0xe, 0xe);
}

uint64_t ctx_begin() {
	return _syscaller((int)BEGINCTX, 0xe, 0xe, 0xe, 0xe);
}

uint32_t* screenshot() {
	return (uint32_t*)_syscaller((int)SCREENSHOT, 0xe, 0xe, 0xe, 0xe);
}

int exit(int code) {
	return _syscaller((int)EXIT, code, 0xe, 0xe, 0xe);
}

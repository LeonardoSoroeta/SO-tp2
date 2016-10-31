#ifndef __FS_ARCH_H__
#define __FS_ARCH_H__

#include <arch/types.h>
#include <arch/process.h>

typedef struct {
	void *start;
	int id;
	char *name;
	uint32_t size;
	uint32_t offset;
	char visible;
} file_t;

typedef struct {
	
	file_t *file;
	int fd;

} fd_t;

typedef struct fd_t FILE;

void init_fs();
void* file_from_fd(int fd);

int fopenp(char *path, process_t *process);
int fopen(char *path);
int fclosep(char *path, process_t *process);
int fclose(char *path);
int close(int fd);
uint64_t fsize(char* path);
uint64_t size(int fd);
uint64_t fseek(char* path, uint64_t offset);
uint64_t seek(int fd, uint64_t offset);
void* fstart(char* path);
void* start(int fd);

int mkfifo(char*);
int pipe(int *fds);
int dup2(int fd1, int fd2);
void add_file(file_t *file);

int ls();

#endif

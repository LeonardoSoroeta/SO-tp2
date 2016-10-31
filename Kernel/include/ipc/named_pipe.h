#ifndef __IPC_NPIPE_H__
#define __IPC_NPIPE_H__


#include <arch/fs.h>

uint32_t write_to_fd(fd_t *fd, char* str, uint32_t size);
uint32_t read_from_fd(fd_t* fd, char* buffer, uint32_t size);

// int mkfifo(char *path);

#endif
#include <ipc/named_pipe.h>
#include <arch/fs.h>
#include <arch/scheduler.h>
#include <sys/logging.h>

uint32_t write_to_fd(fd_t *fd, char* str, uint32_t size) {

	uint32_t i = 0;
	file_t* file;
	char *start_write;

	file = fd->file;

	start_write = (char*)((char*)file->start + file->size);

	logf("writing fd=%d ptr=%x\n", fd->fd, start_write);

	if (size == 1) {

		logf("c=%c\n", *str);

		start_write[0] = *str;

		file->size++;

	} else {

		while (*str != '\0' && size--) {
			*(char*)(fd->file->start+fd->file->size++) = *str++;
			
			i++;
			// fd->file->size++;

			// str++;
		}
	}

	logs("wrote\n");

	return i;
}

uint32_t read_from_fd(fd_t* fd, char* buffer, uint32_t size) {

	uint32_t i = 0;
	file_t *file;
	process_queue_t* task;

	task = current_task();

	file = fd->file;

	do {
		task->current->state = IO;
		task->ttl = 10+i;
		if (file->size > file->offset) {
			buffer[i++] = *(char*)(file->start+file->offset++);
		} 
	} while (i < size);

	return i;
}


#include <arch/fs.h>
#include <arch/types.h>
#include <sys/video.h>
#include <arch/kmem.h>
#include <sys/logging.h>
#include <stddef.h>
#include <util/util.h>
#include <arch/scheduler.h>
#include <arch/pmem.h>
#include <sys/vga.h>

#define FS_START 0xc00000

#define FS_FD_START 10

// typedef struct {
// 	void *start;
// 	int id;
// 	char *name;
// 	uint32_t size;
// } file_t;

// typedef struct {
	
// 	file_t *file;
// 	int fd;

// } fd_t;

// static file_t **files;
static list_t *filesystem;

static void create_standard_files();

static void create_standard_files() {

	file_t* stdin;
	file_t* stdout;
	file_t* stderr;
	file_t* stdmouse;

	stdin = (file_t*)kmalloc(sizeof(file_t));
	stdout = (file_t*)kmalloc(sizeof(file_t));
	stderr = (file_t*)kmalloc(sizeof(file_t));
	stdmouse = (file_t*)kmalloc(sizeof(file_t));

	stdin->start = NULL;
	stdout->start = NULL;
	stderr->start = NULL;
	stdmouse->start = NULL;

	stdin->id = 0;
	stdout->id = 1;
	stderr->id = 2;
	stdmouse->id = 3;

	stdin->name = "stdin";
	stdout->name = "stdout";
	stderr->name = "stderr";
	stdmouse->name = "stdmouse";

	stdin->size = 0;
	stdout->size = 0;
	stderr->size = 0;
	stdmouse->size = 0;

	stdin->offset = 0;
	stdout->offset = 0;
	stderr->offset = 0;
	stdmouse->offset = 0;

	stdin->visible = 0;
	stdout->visible = 0;
	stderr->visible = 0;
	stdmouse->visible = 0;

	// files[0] = stdin;
	// files[1] = stdout;
	// files[2] = stderr;

	list_insert(filesystem, (void*)stdin);
	list_insert(filesystem, (void*)stdout);
	list_insert(filesystem, (void*)stderr);
	list_insert(filesystem, (void*)stdmouse);

}

void init_fs() {

	uint32_t i = 0, j = 0;
	uint32_t length = 0;
	uint32_t offset = 0;
	uint32_t total_files = 0;
	file_t *aux_file;
	node_t *it;

	// get total files
	// first byte has total files
	total_files = *(uint32_t*)(FS_START);

	filesystem = list_create();

	// files = (file_t**)kmalloc(sizeof(file_t*)*(total_files+3));

	create_standard_files();

	offset += sizeof(uint32_t);

	logs("found ");
	logi(total_files);
	logs(" files\n");

	for (; i < total_files; i++) {

		length = *(uint32_t*)(intptr_t)(FS_START+offset);
		offset += sizeof(uint32_t);

		aux_file = (file_t*)kmalloc(sizeof(file_t));

		list_insert(filesystem, (void*)aux_file);

		// files[i+3] = aux_file;

		aux_file->name = (char*)kmalloc(length+1);
		for (j = 0; j < length; j++) {
			logc(*(char*)(intptr_t)(FS_START+offset) );
			aux_file->name[j] = *(char*)(intptr_t)(FS_START+offset);
			offset++;
		}
		aux_file->name[j] = '\0';
		lognl();

		if (length % 4 != 0) {
			offset += (length / 4 + 1) * 4 - length;
		}

		aux_file->id = i+FS_FD_START;

		aux_file->visible = 1;

	}

	it = list_get(filesystem, 4);

	for (i = 0; i < total_files; i++) {

		aux_file = (file_t*)it->value;

		// offset
		length = *(uint32_t*)(intptr_t)(FS_START+offset);
		offset += sizeof(uint32_t);

		aux_file->start = (void*)(intptr_t)(FS_START+length);

		// size
		length = *(uint32_t*)(intptr_t)(FS_START+offset);
		aux_file->size = length;
		offset += sizeof(uint32_t);

		// aux_file->size = *(uint32_t*)(intptr_t)(FS_START+offset)-2;
		// files[i+3]->size = *(uint32_t*)(intptr_t)(FS_START+offset);

		aux_file->offset = 0;

		logs("file ");
		logs(aux_file->name);
		logs(" starts at ");
		loghex((int64_t)aux_file->start);
		logs(" size: ");
		logi(aux_file->size);
		logs(" bytes");
		lognl();

		it = it->next;
	}

	// total_files += 3;

	// files[i].start = (void*)(intptr_t)( ptr + *(uint32_t*)ptr);
}

int mkfifo(char *path) {

	file_t *fifo;
	process_t *process;
	fd_t *fd;

	process = current_process();

	fifo = (file_t*)kmalloc(sizeof(file_t));

	list_insert(filesystem, (void*)fifo);

	fifo->start = pmalloc(1);
	fifo->size = 0;
	fifo->name = path;
	fifo->id = filesystem->length;
	fifo->visible = 0;

	fd = (fd_t*)kmalloc(sizeof(fd_t));

	list_insert(process->fd, fd);

	fd->file = fifo;
	fd->fd = process->fd->length;

	return fd->fd;
}

int dup2(int fd1, int fd2) {

	process_t *process;
	fd_t* pfd1;
	fd_t* pfd2;
	node_t *node;

	process = current_process();

	node = list_get(process->fd, fd1-1);

	if (node == NULL)
		return -1;

	pfd1 = (fd_t*)node->value;

	node = list_get(process->fd, fd2-1);

	if (node == NULL)
		return -1;

	pfd2 = (fd_t*)node->value;

	// lo que se escribe en fd2 va a fd1
	
	pfd2->file = pfd1->file;

	return 0;

}

int pipe(int *fds) {

	file_t *pipe;
	process_t *process;
	fd_t *fd1;
	fd_t *fd2;

	process = current_process();

	pipe = (file_t*)kmalloc(sizeof(file_t));

	list_insert(filesystem, (void*)pipe);

	pipe->start = pmalloc(1);
	pipe->size = 0;
	pipe->name = "pipe";
	pipe->id = filesystem->length;
	pipe->visible = 0;

	fd1 = (fd_t*)kmalloc(sizeof(fd_t));

	list_insert(process->fd, fd1);

	fd1->file = pipe;
	fd1->fd = process->fd->length;

	fd2 = (fd_t*)kmalloc(sizeof(fd_t));

	list_insert(process->fd, fd2);

	fd2->file = pipe;
	fd2->fd = process->fd->length;

	fds[0] = fd1->fd;
	fds[1] = fd2->fd;

	return 0;

}

void add_file(file_t *file) {

	list_insert(filesystem, (void*)file);

}

void *file_from_fd(int fd) {

	process_t *process;
	fd_t *curr_fd;

	process = current_process();

	foreach(node, process->fd) {

		curr_fd = (fd_t*)node->value;

		if (curr_fd->fd == fd) {
			return curr_fd->file->start;
		}

	}

	return NULL;
}

int fopenp(char *path, process_t *process) {

	fd_t* result;
	file_t* file;

	foreach(node, filesystem) {

		file = (file_t*)node->value;

		if (strcmp(file->name, path) == 0) {

			result = (fd_t*)kmalloc(sizeof(fd_t));
			result->file = file;
			result->fd = process->fd->length+1;

			list_insert(process->fd, result);

			return result->fd;
		}

	}
	return -1;
}

int fclosep(char *path, process_t *process) {
	
	file_t* file;
	fd_t* curr_fd;

	foreach(node, process->fd) {

		curr_fd = (fd_t*)node->value;
		file = curr_fd->file;

		if (strcmp(file->name, path) == 0) {
			
			list_delete(process->fd, node);

			return 0;
		}

	}
	return -1;
}

int fopen(char *path) {
	return fopenp(path, current_process());
}

int fclose(char *path) {
	return fclosep(path, current_process());
}

int close(int fd) {
	
	node_t     *node;
	process_t* process;

	process = current_process();

	node = list_get(process->fd, fd-1);

	if (node == NULL) {
		return -1;
	}

	list_delete(process->fd, node);

	return 0;
}


uint64_t fsize(char* path) {
	file_t* file;
	fd_t* curr_fd;
	process_t *process;

	process = current_process();

	foreach(node, process->fd) {

		curr_fd = (fd_t*)node->value;
		file = curr_fd->file;

		if (strcmp(file->name, path) == 0) {
			
			return file->size-2;
		}

	}
	return -1;
}

uint64_t size(int fd) {
	
	node_t* node;
	process_t *process;

	process = current_process();

	node = list_get(process->fd, fd-1);

	if (node == NULL)
		return -1;

	return ((fd_t*)node->value)->file->size-2;
}

uint64_t fseek(char* path, uint64_t offset) {
	file_t* file;
	fd_t* curr_fd;
	process_t *process;

	process = current_process();

	foreach(node, process->fd) {

		curr_fd = (fd_t*)node->value;
		file = curr_fd->file;

		if (strcmp(file->name, path) == 0) {
			
			file->offset = offset;

			return offset;
		}

	}
	return -1;
}

uint64_t seek(int fd, uint64_t offset) {
	
	node_t* node;
	process_t *process;

	process = current_process();

	node = list_get(process->fd, fd-1);

	if (node == NULL)
		return -1;

	((fd_t*)node->value)->file->offset = offset;

	return offset;
}



void* fstart(char* path) {
	file_t* file;

	foreach(node, filesystem) {

		file = (file_t*)node->value;

		if (strcmp(file->name, path) == 0) {
			
			return file->start;
		}

	}
	return NULL;
}

void* start(int fd) {
	
	node_t* node;
	process_t *process;

	process = current_process();

	node = list_get(process->fd, fd-1);

	if (node == NULL)
		return NULL;

	return ((fd_t*)node->value)->file->start;
}

int ls() {

	file_t *curr;
	uint16_t len;

	vstring("#\tNAME\t\t\t\tSIZE\t\t\t\tADDRESS\n");
	vstring("----------------------------------\n");

	foreach(node, filesystem) {

		curr = (file_t*)node->value;

		if (!curr->visible)
			continue;

		vputi(curr->id);

		if (curr->id < 10) {
			vchar(' ');
		}
		vchar(' ');

		vstring(curr->name);

		len = 12 - strlen(curr->name);

		while (len--) {
			vchar(' ');
		}

		vputi(curr->size-2);
		vstring(" bytes");

		len = 12 - intlen(curr->size-2);

		while (len--) {
			vchar(' ');
		}

		vputhex((int64_t)curr->start);

		vchar('\n');

	}

	return 0;
}

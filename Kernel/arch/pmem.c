#include <arch/buddy.h>
#include <arch/pmem.h>
#include <arch/hard.h>
#include <arch/types.h>
#include <sys/video.h>
#include <string.h>
#include <sys/logging.h>
#include <arch/paging.h>
#include <arch/kmem.h>

#define MEMORY_WIDTH 64
#define BLOCK_SIZE 0x1000
#define TOTAL_MEMORY (_get_memory_size() * 1024 * 1024)
#define MEMORY_START 0x1000000 // 16 MiB
// #define KERNEL_HEAP 0x8000000
#define AVAILABLE_MEMORY fit_align_4k(TOTAL_MEMORY - MEMORY_START)
#define MEMORY_END TOTAL_MEMORY
#define TOTAL_BLOCKS (AVAILABLE_MEMORY / BLOCK_SIZE)

static uint64_t paged_size(uint64_t size);

uint64_t fit_align_4k(uint64_t size) {
	uint64_t r = 0;
	while (size >>= 1)
		r++;
	return 1<<(r-1);
}

static uint64_t paged_size(uint64_t size) {
	uint64_t fits;

	fits = size / BLOCK_SIZE;

	return BLOCK_SIZE*(fits+1);
}

void* pcalloc(uint32_t amount, uint64_t size) {

	void *mem;

	mem = pmalloc(amount*size);
	memset(mem, 0, amount*size);

	return mem;

}

void* pmalloc(uint64_t size) {
	uint64_t ptr = 0;

	// logs("malloc size ");
	// logi(paged_size(size));
	// lognl();

	ptr = buddy_alloc(paged_size(size));
	if (ptr < 0) {
		return NULL;
	}
	
	// logs("malloc size ");
	// logi(size);
	// lognl();

	// logs("giving index=");
	// logi(ptr);
	// lognl();

	// pkalloc();

	// return (void*)(intptr_t)(MEMORY_END - ptr);
	return (void*)(intptr_t)(MEMORY_START + ptr);
}

void pfree(void *ptr) {

	// buddy_free((uint64_t)(MEMORY_END - (uint64_t)ptr));
	buddy_free((uint64_t)(ptr - MEMORY_START));
}

void init_mem(void) {

	logs("Initializing memory\n");
	logs("Available memory: ");
	logi(AVAILABLE_MEMORY);
	logs(" bytes\n");
	logs("Starting at: ");
	loghex(MEMORY_START);
	logs("\nEnding at: ");
	loghex(MEMORY_END);
	logc('\n');

	buddy_init(AVAILABLE_MEMORY);

}

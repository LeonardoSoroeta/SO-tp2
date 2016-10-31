#include <arch/paging.h>
#include <sys/video.h>
#include <sys/logging.h>
#include <lib.h>
#include <arch/kmem.h>
#include <arch/pmem.h>
#include <arch/interrupts.h>
#include <arch/hard.h>
#include <arch/cpu.h>
#include <arch/scheduler.h>
#include <sys/sound.h>

#define B_P 0 // present
#define B_RW 1 // read write
#define B_US 2 // user supervisor
#define B_PWT 3
#define B_PCD 4
#define B_A 5

#define PAGES_ADDR_START 0x200000
#define TABLE_LENGTH 512
#define PAGE_SIZE 0x1000

#define PML4_START PAGES_ADDR_START // 1 tabla 1 entrada
#define PDP_START (PML4_START+TABLE_LENGTH*sizeof(table_entry_t)) // 1 tabla 4 entradas
#define PD_START (PDP_START+TABLE_LENGTH*sizeof(table_entry_t)) // 4 tablas 512 entradas c/u
#define P_START (PD_START+4*TABLE_LENGTH*sizeof(table_entry_t)) // 512 tablas

// Kernel HEAP

#define TOTAL_MEMORY (_get_memory_size() * 1024 * 1024)
#define AVAILABLE_MEMORY 0x8000000 // 128 MiB for Kernel RAM heap
#define MEMORY_START TOTAL_MEMORY - AVAILABLE_MEMORY

typedef struct {
	ddword present   : 1;
	ddword writeable : 1;
	ddword user			 : 1;
	ddword pwt       : 1; // write through
	ddword pcd       : 1; // cache
	ddword a         : 1;
	ddword ign       : 1;
	ddword mbz    	 : 2;
	ddword avl    	 : 3;
	ddword addr  	 	 : 40;
	ddword avail 		 : 11;
	ddword nx        : 1;
} table_entry_t;

typedef struct {
	ddword ignored   : 3;
	ddword pwt       : 1; // write through
	ddword pcd       : 1; // cache
	ddword mbz       : 7; // must be zero
	ddword addr      : 40;
	ddword mbz2      : 12;
} cr3_t;

typedef table_entry_t pml4_entry_t;
typedef table_entry_t pdp_entry_t;
typedef table_entry_t pd_entry_t;
typedef table_entry_t p_entry_t;

// 512 entries?
static pml4_entry_t *pml4t = (pml4_entry_t*)PML4_START;//PML4_START;
static p_entry_t *page_table = (p_entry_t*)P_START;
static pd_entry_t *page_directory_table = (pd_entry_t*)PD_START;
static uint64_t last_pt_index = 0, last_pd_index = 0;

// info
// https://github.com/0xAX/linux-insides/blob/master/Theory/Paging.md

/**
 *
 *		In binary it will be:
 *		1111111111111111 111111111 111111110 000001000 000000000 000000000000
 *		       63:48        47:39     38:30     29:21     20:12      11:0
 *		virtual address is split in parts as described above:
 *		
 *		63:48 - 16 bits not used;
 *		47:39 - 9  bits store an index into the paging structure level-4;
 *		38:30 - 9  bits store index into the paging structure level-3;
 *		29:21 - 9  bits store an index into the paging structure level-2;
 *		20:12 - 9  bits store an index into the paging structure level-1;
 *		11:0 -  12 bits provide the offset into the physical page in byte.
 *
 * 
 */

/**
 * CR3
 *
 * 63                  52 51                                                        32
 *  --------------------------------------------------------------------------------
 * |                     |                                                          |
 * |    Reserved MBZ     |            Address of the top level structure            |
 * |                     |                                                          |
 *  --------------------------------------------------------------------------------
 * 31                                  12 11            5     4     3 2             0
 *  --------------------------------------------------------------------------------
 * |                                     |               |  P  |  P  |              |
 * |  Address of the top level structure |   Reserved    |  C  |  W  |    Reserved  |
 * |                                     |               |  D  |  T  |              |
 *  --------------------------------------------------------------------------------
 * These fields have the following meanings:

 * Bits 63:52 - reserved must be 0.
 * Bits 51:12 - stores the address of the top level paging structure;
 * Reserved - reserved must be 0;
 * Bits 4 : 3 - PWT or Page-Level Writethrough and PCD or Page-level cache disable indicate. These bits control the way the page or Page Table is handled by the hardware cache;
 * Bits 2 : 0 - ignored;
 *
 * 
 */

// static uint64_t* cr3;

static cr3_t cr3;

// static void init_cr3(uint64_t*);

extern uint64_t _get_cr3(void);
extern void _set_cr3(uint64_t);

static void setup_kernel_identity_mapping();
static void setup_kernel_identity_heap();

static void setup_kernel_identity_heap() {

#if 0
	int i = 0;
	p_entry_t *page_table = (p_entry_t*)P_START;
	pd_entry_t *page_directory_table = (pd_entry_t*)PD_START;

	logs("setting ");
	logi(AVAILABLE_MEMORY / PAGE_SIZE);
	logs(" pages\n");

	for (; i < AVAILABLE_MEMORY / PAGE_SIZE; i++) {

		memset((void*)(page_table+(TABLE_LENGTH*2+i)), 0, sizeof(table_entry_t));
		page_table[TABLE_LENGTH*3+i].present = yes;
		page_table[TABLE_LENGTH*3+i].writeable = yes;
		page_table[TABLE_LENGTH*3+i].user = no;

		page_table[TABLE_LENGTH*3+i].addr = (ddword)(MEMORY_START+i*PAGE_SIZE);
	}

	logs("end of pages: ");
	loghex((int64_t)&page_table[TABLE_LENGTH*3+i]);
	lognl();

	logs("setting ");
	logi(AVAILABLE_MEMORY / PAGE_SIZE / TABLE_LENGTH);
	logs(" directories\n");

	for (i = 3; i < AVAILABLE_MEMORY / PAGE_SIZE / TABLE_LENGTH; i++) {

		memset((void*)(page_directory_table+i), 0, sizeof(table_entry_t));
		page_directory_table[i].present = yes;
		page_directory_table[i].writeable = yes;
		page_directory_table[i].user = no;

		page_directory_table[i].addr = (ddword)(P_START+TABLE_LENGTH*i*sizeof(table_entry_t));

	}
#endif
}

void give_new_page() {

	if (last_pt_index % TABLE_LENGTH == 0) {
		memset((void*)(page_directory_table+last_pd_index), 0, sizeof(table_entry_t));
		page_directory_table[last_pd_index].present = yes;
		page_directory_table[last_pd_index].writeable = yes;
		page_directory_table[last_pd_index].user = no;

		logs("page table directory[");
		logi(last_pd_index);
		logs("] -> ");
		loghex((int64_t)(P_START + last_pd_index*TABLE_LENGTH*sizeof(table_entry_t) ));
		lognl();
		page_directory_table[last_pd_index].addr = (ddword)(P_START + last_pd_index*TABLE_LENGTH*sizeof(table_entry_t)) >> 12;

		logs("init_pdt");
		logi(last_pd_index);
		logs(":: pdt=");
		loghex((int64_t)*(int64_t*)&page_directory_table[last_pd_index]);
		lognl();

		last_pd_index++;
	}

	memset((void*)(page_table+last_pt_index), 0, sizeof(table_entry_t));
	page_table[last_pt_index].present = yes;
	page_table[last_pt_index].writeable = yes;
	page_table[last_pt_index].user = no;
	page_table[last_pt_index].pwt = yes;

	page_table[last_pt_index].addr = (ddword)(last_pt_index*PAGE_SIZE) >> 12;

	last_pt_index++;
}

static void setup_kernel_identity_mapping() {

	uint64_t i = 0;
	pdp_entry_t *page_directory_pointer_table = (pdp_entry_t*)PDP_START;

	logs("Setting up page table\n");
	logs("Starting at: ");
	loghex((int64_t)page_table);
	lognl();

	for (i = 0; i < TABLE_LENGTH*512*4; i++) {
		memset((void*)(page_table+i), 0, sizeof(table_entry_t));
		page_table[i].present = yes;
		page_table[i].writeable = yes;
		page_table[i].user = no;
		page_table[i].pwt = yes;

		page_table[i].addr = (ddword)(i*PAGE_SIZE) >> 12;

		last_pt_index = i+1;
	}

	logs("Last page: ");
	loghex((int64_t)(page_table+TABLE_LENGTH*512*4-1));
	lognl();

	// for (i = TABLE_LENGTH*2; i < TABLE_LENGTH*3; i++) {
	// 	memset((void*)(page_table+i), 0, sizeof(table_entry_t));
	// 	page_table[i].present = yes;
	// 	page_table[i].writeable = yes;
	// 	page_table[i].user = yes;
	// 	page_table[i].pwt = yes;

	// 	page_table[i].addr = (ddword)(i*PAGE_SIZE) >> 12;
	// }

	logs("Setting up page directory\n");
	logs("Starting at: ");
	loghex((int64_t)page_directory_table);
	lognl();

	for (i = 0; i < 512*4; i++) {
		memset((void*)(page_directory_table+i), 0, sizeof(table_entry_t));
		page_directory_table[i].present = yes;
		page_directory_table[i].writeable = yes;
		page_directory_table[i].user = no;

		// logs("page table directory[");
		// logi(i);
		// logs("] -> ");
		// loghex((int64_t)(P_START + i*TABLE_LENGTH*sizeof(table_entry_t) ));
		// lognl();
		page_directory_table[i].addr = (ddword)(P_START + i*TABLE_LENGTH*sizeof(table_entry_t)) >> 12;

		// logs("init_pdt");
		// logi(i);
		// logs(":: pdt=");
		// loghex((int64_t)*(int64_t*)&page_directory_table[i]);
		// lognl();

		last_pd_index = i+1;
	}

	logs("Setting up page directory pointer\n");
	logs("Starting at: ");
	loghex((int64_t)page_directory_pointer_table);
	lognl();

	for (i = 0; i < 4; i++) {

		memset((void*)(page_directory_pointer_table+i), 0, sizeof(table_entry_t));
		page_directory_pointer_table[i].present = yes;
		page_directory_pointer_table[i].writeable = yes;
		page_directory_pointer_table[i].user = no;

		page_directory_pointer_table[i].a = yes;

		logs("page table directory* -> ");
		loghex((int64_t)PD_START);
		lognl();
		page_directory_pointer_table[i].addr = (ddword)(PD_START + i*TABLE_LENGTH*sizeof(table_entry_t)) >> 12;

		logs("init_pdpt:: pdpt=");
		loghex((int64_t)*(int64_t*)&page_directory_pointer_table[i]);
		lognl();

	}

	logs("Setting up pml4\n");

	memset((void*)pml4t, 0, sizeof(table_entry_t));
	pml4t[0].present = yes;
	pml4t[0].writeable = yes;
	pml4t[0].user = no;
	pml4t[0].a = yes;

	logs("pml4 -> ");
	loghex((int64_t)PDP_START);
	lognl();
	pml4t[0].addr = (ddword)(PDP_START) >> 12;

	logs("init_pml4:: pml4=");
	loghex((int64_t)*(int64_t*)&pml4t[0]);
	lognl();
}

void init_paging() {

	logs("Setting up paging\n");

	setup_kernel_identity_mapping();
	setup_kernel_identity_heap();

	logs("init_pml4t:: pml4t=");
	loghex((int64_t)pml4t);
	lognl();

	// _cli();

	// _cli();
	// _halt();

	memset((void*)&cr3, 0, sizeof(cr3_t));

	cr3.pwt = yes;
	cr3.addr = ((ddword)pml4t) >> 12;

	logs("init_cr3:: cr3=");
	loghex((int64_t)*(int64_t*)&cr3);
	lognl();

	_set_cr3((uint64_t)*(uint64_t*)&cr3);

	// _set_cr3((uint64_t)0x3000);

	// os_dump_regs();

	// _halt();

	// _sti();

	// logs("init_cr3:: cr3=");
	// loghex((int64_t)_get_cr3());
	// lognl();

	// init_cr3(pml4t);

	// 1. chequear que cr3 apunte bien y no haya que shiftear
	// 2. cargar las tablas

	// init_cr3((uint64_t*)0x2008);
}

void on_page_fault(uint64_t cr2, uint64_t rip, uint64_t error_code) {

	logf("PF error:%x cr2:%x ip:%x\n", error_code, cr2, rip);

	ps();

	pause();

}

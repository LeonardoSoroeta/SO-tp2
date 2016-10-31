#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>

#include <arch/arch.h>
#include <sys/video.h>
#include <sys/vga.h>
#include <sys/timer.h>
#include <sys/keyboard.h>
#include <sys/syscalls.h>
#include <sys/sound.h>
#include <sys/mouse.h>
#include <arch/serial.h>
#include <arch/fs.h>
#include <arch/kmem.h>
#include <arch/pmem.h>
#include <arch/hard.h>
#include <arch/paging.h>
#include <sys/logging.h>
#include <arch/scheduler.h>
#include <sys/bmp.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

// static const uint64_t PageSize = 0x10000;

static void * const sampleIdleModuleAddress = (void*)0x120000;
static void * const sampleCodeModuleAddress = (void*)0xb00000;
static void * const sampleDataModuleAddress = (void*)0xc00000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {

	return (void*)0x19ff00;

	// return (void*)(
	// 	(uint64_t)&endOfKernel
	// 	+ PageSize * 8				//The size of the stack itself, 64KiB
	// 	- sizeof(uint64_t)			//Begin at the top of the stack
	// 	);
}

void * initializeKernelBinary()
{

	void * moduleAddresses[] = {
		sampleIdleModuleAddress,
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

void play_intro() {

	uint8_t i = 0;

	clc();

	println("El TP de arqui esta booteando...");

	wait(700);

	println("Hacete un cafecito. Ya arrancamos");

	wait(1000);

	clc();

	println("	            ____________________________________________________");
	println("            /                                                    \\");
	println("           |    _____________________________________________     |");
	println("           |   |                                             |    |");
	println("           |   |  OS$> make me a sandwich                    |    |");
	println("           |   |  make it yourself                           |    |");
	println("           |   |  OS$> sudo make me a sandwich               |    |");
	println("           |   |  alright...                                 |    |");
	println("           |   |  OS$> _                                     |    |");
	println("           |   |                                             |    |");
	println("           |   |                                             |    |");
	println("           |   |                                             |    |");
	println("           |   |                                             |    |");
	println("           |   |                                             |    |");
	println("           |   |                                             |    |");
	println("           |   |                                             |    |");
	println("           |   |_____________________________________________|    |");
	println("           |                                                      |");
	println("            \\_____________________________________________________/");
	println("                   \\_______________________________________/");
	println("                _______________________________________________");
	println("             _-'    .-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.  --- `-_");
	println("          _-'.-.-. .---.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.--.  .-.-.`-_");
	println("       _-'.-.-.-. .---.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-`__`. .-.-.-.`-_");
	println("    _-'.-.-.-.-. .-----.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-----. .-.-.-.-.`-_");

	wait(2000);

	move_up();

	for (; i < 80; i++) {
		putc('*');
		wait(50);
	}

	clc();
}

int main() {

	// int w = 100;
	// int h = 100;
	// int i = 0;
	// int j = 0;

	clc();

	putc('c');
	putc('c');
	putc('c');
	putc('c');
	putc('c');
	putc('c');
	putc('c');
	putc('c');
	putc('c');
	putc('c');

	// TODO:reorder
	init_serial();
	logs("\n\n\n\n== Serial OK ===\n");

	logs("Stack starts at: ");
	loghex((int64_t)getStackBase());
	lognl();

	init_idt();
	logs("== IDT OK ===\n");
	init_interrupts();
	logs("== Interrupts OK ===\n");
	init_timer();
	logs("== Timer OK ===\n");
	init_syscalls();
	logs("== Syscalls OK ===\n");
	init_kheap();
	logs("== Kernel heap OK ===\n");
	init_mem();
	logs("== Page allocator OK ===\n");
	init_paging();
	logs("== Paging OK ===\n");
	init_mouse();
	logs("== Mouse OK ===\n");
	init_keyboard();
	logs("== Keyboard OK ===\n");
	init_vga();
	logs("== VGA OK ===\n");
	init_fs();
	logs("== FS OK ===\n");
	init_scheduling();
	logs("== Scheduler OK ===\n");
	init_sound();
	logs("== Sound OK ===\n");
	// play_intro();


	vclc();
	logs("screen cleared\n");


	// draw_polygon(ctx, 20, 20, 5, 15, 0x0000ff);


	// vpoly(100, 100, 5, 20, 0x0000ff);

	/**
	 *
	 * init_sched()
	 *
	 * sched_enqueue( sampleCodeModuleAddress ) con un stack desde cerapio
	 * // nada mas pq el tt se va a encargar
	 */

	// scheduler_enqueue("echo.bin");

	// give_new_page();
	
	logs("Enqueing userland\n");

	scheduler_enqueue(sampleCodeModuleAddress, "Userland", 0, (char**)0);
	_start_scheduling();

	// scheduler_enqueue(sampleIdleModuleAddress, "Kernel");
	// scheduler_enqueue(sampleCodeModuleAddress, "Userland");
	// Con esto ejecutamos Userland
	// ((EntryPoint)sampleCodeModuleAddress)();

	return 0;
}

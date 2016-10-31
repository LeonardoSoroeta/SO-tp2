#include <arch/interrupts_handler.h>
#include <sys/syscalls.h>
#include <sys/keyboard.h>
#include <sys/video.h>
#include <sys/sound.h>
#include <sys/mouse.h>
#include <sys/timer.h>
#include <util/util.h>
#include <sys/clock.h>
#include <arch/arch.h>
#include <arch/scheduler.h>
#include <sys/logging.h>
#include <sys/bochs.h>
#include <sys/malloc.h>
#include <sys/vga.h>
#include <arch/fs.h>
#include <ipc/named_pipe.h>

static ddword on_ack_syscall(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword);

static ddword syscall_read(ddword fd, ddword buf, ddword size, ddword);
static ddword syscall_write(ddword fd, ddword buf, ddword size, ddword);
static ddword syscall_get_time(ddword time_ptr, ddword arg2, ddword arg3, ddword);
static ddword syscall_set_time(ddword time_ptr, ddword arg2, ddword arg3, ddword);
static ddword syscall_beep(ddword length, ddword freq, ddword arg3, ddword);
static ddword syscall_shutdown(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_halt(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_set_opts(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_event(ddword id, ddword func_ptr, ddword arg3, ddword);
static ddword syscall_wait(ddword millis, ddword arg2, ddword arg3, ddword);
static ddword syscall_fork(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_getpid(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_getpname(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_getppid(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_getresult(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_waitpid(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_init_proc(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_vid_mode(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_malloc(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_calloc(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_free(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_ps(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_kill(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_stop(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_hold(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_cont(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_signal(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_begin_ctx(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_mkfifo(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_ls(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_fopen(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_fclose(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_close(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_pipe(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_dup2(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_fsize(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_size(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_fstart(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_start(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_fseek(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_seek(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_screenshot(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_yield(ddword arg1, ddword arg2, ddword arg3, ddword);
static ddword syscall_exit(ddword arg1, ddword arg2, ddword arg3, ddword);

ddword (*syscalls[EXIT-READ+1])(ddword, ddword, ddword, ddword);
static char* song_repository_addr = (char*)0x500000;
static uint32_t song_read_offset = 0;

static ddword syscall_set_opts(ddword fd, ddword request, ddword params, ddword arg) {

	if (fd == STDOUT) {	

		if (request == REQUEST_CLEAR_SCREEN) {

			vclcmid();

		} else if (request == REQUEST_SET_COLOR) {

			set_colour_raw(params);
		} else if (request == REQUEST_RESET) {
			vsetfgcolor(BLACK_COLOR);
			vsetbgcolor(WHITE_COLOR);
			// set_colour(COLOR_BLACK, COLOR_LIGHT_GREY);
		}
	} else if (fd == STDMOUSE) {

		if (request == REQUEST_HIDE) {
			mouse_set_visible(no);
		} else if (request == REQUEST_SHOW) {
			mouse_set_visible(yes);
		} else if (request == REQUEST_SENSITIVTY) {
			mouse_set_sensitivity(params);
		}
	} else {

		seek(fd, 0);

	}


	return 0;
}

static ddword syscall_shutdown(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	_cli();
	vclc();
	pause();
	// strlen("Puede Apagar su Equipo") = 22
	vstringxy( (VGA_WIDTH - 22*CHARS(1))/2 , VGA_HEIGHT/2, "Puede Apagar su Equipo");
	_halt();

	return 0;
}

static ddword syscall_halt(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	_halt();

	return 0;
}

static ddword syscall_fork(ddword entry_point, ddword name, ddword argc, ddword argv) {

	pid_t pid;

	logs("\n<fork\n");

	pid = prepare_fork((void*)entry_point, (char*)name);

	logs("fork>\n\n");

	return pid;
}

static ddword syscall_ps(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	ps();
	return 0;
}

static ddword syscall_yield(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	scheduler_switch();
	return 0;
}

static ddword syscall_kill(ddword pid, ddword arg2, ddword arg3, ddword arg4) {
	return kill((pid_t)pid);
}

static ddword syscall_stop(ddword pid, ddword arg2, ddword arg3, ddword arg4) {
	return stop((pid_t)pid);
}

static ddword syscall_hold(ddword pid, ddword arg2, ddword arg3, ddword arg4) {
	return hold((pid_t)pid);
}

static ddword syscall_cont(ddword pid, ddword arg2, ddword arg3, ddword arg4) {
	return cont((pid_t)pid);
}

static ddword syscall_signal(ddword sig, ddword arg2, ddword arg3, ddword arg4) {
	return signal((sig_t)sig);
}

static ddword syscall_begin_ctx(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	unsigned char* ptr = get_buffer();
	logs("buffer ");
	loghex((int64_t)ptr);
	lognl();
	return (uintptr_t)ptr;
}

static ddword syscall_getpid(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	return getpid();
}
static ddword syscall_getpname(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	return (uintptr_t)current_process()->name;
}

static ddword syscall_getppid(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	return getppid();
}

static ddword syscall_getresult(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	return getresult();
}

static ddword syscall_malloc(ddword size, ddword arg2, ddword arg3, ddword arg4) {
	return (uintptr_t)malloc(current_process(), size);
}

static ddword syscall_calloc(ddword amount, ddword size, ddword arg3, ddword arg4) {
	return (uintptr_t)calloc(current_process(), amount, size);
}

static ddword syscall_free(ddword ptr, ddword arg2, ddword arg3, ddword arg4) {
	free(current_process(), (void*)ptr);
	return 0;
}

static ddword syscall_mkfifo(ddword name, ddword arg2, ddword arg3, ddword arg4) {
	return mkfifo((char*)name);
}

static ddword syscall_fopen(ddword name, ddword arg2, ddword arg3, ddword arg4) {
	return fopen((char*)name);
}

static ddword syscall_fclose(ddword name, ddword arg2, ddword arg3, ddword arg4) {
	return fclose((char*)name);
}

static ddword syscall_close(ddword fd, ddword arg2, ddword arg3, ddword arg4) {
	return close(fd);
}

static ddword syscall_pipe(ddword fds, ddword arg2, ddword arg3, ddword arg4) {
	return pipe((int*)fds);
}

static ddword syscall_dup2(ddword fd1, ddword fd2, ddword arg3, ddword arg4) {
	return dup2(fd1, fd2);
}

static ddword syscall_fsize(ddword path, ddword arg2, ddword arg3, ddword arg4) {
	return fsize((char*)path);
}

static ddword syscall_size(ddword fd, ddword arg2, ddword arg3, ddword arg4) {
	return size(fd);
}

static ddword syscall_fstart(ddword path, ddword arg2, ddword arg3, ddword arg4) {
	return (ddword)fstart((char*)path);
}

static ddword syscall_start(ddword fd, ddword arg2, ddword arg3, ddword arg4) {
	return (ddword)start(fd);
}

static ddword syscall_fseek(ddword path, ddword offset, ddword arg3, ddword arg4) {
	return fseek((char*)path, offset);
}

static ddword syscall_seek(ddword fd, ddword offset, ddword arg3, ddword arg4) {
	return seek(fd, offset);
}

static ddword syscall_screenshot(ddword fd, ddword offset, ddword arg3, ddword arg4) {
	return (ddword)take_screenshot();
}

static ddword syscall_ls(ddword name, ddword arg2, ddword arg3, ddword arg4) {
	return ls();
}

static ddword syscall_vid_mode(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	
	logs("setting vid mode on\n");

	_cli();

	bochs_vbe_set_mode(1024, 768, 24);

	_sti();

	return 0;
}

static ddword syscall_waitpid(ddword pid, ddword exit_code, ddword argc, ddword argv) {

	logs("<wpid\n");

	waitpid((pid_t)pid, (int*)exit_code);

	logs("wpid>\n");

	return 0;
}

static ddword syscall_init_proc(ddword entry_point, ddword name, ddword argc, ddword argv) {

	pid_t pid;

	logs("<iproc\n");
	// logs((char*)name);
	// lognl();

	pid = scheduler_enqueue((void*)entry_point, (char*)name, (int)argc, (char**)argv);

	logs("iproc>\n");

	return pid;
}

static ddword syscall_exit(ddword code, ddword arg2, ddword arg3, ddword arg4) {

	logs("exit\n");

	exit(code);

	logs(";;exit\n");

	return 0;
}

static ddword syscall_get_time(ddword time_ptr, ddword arg2, ddword arg3, ddword arg4) {
	date_t *time = (date_t*)time_ptr;
	uint8_t registerB = clock_get(REGB);

	uint8_t sec 	= clock_get_seconds();
	uint8_t min 	= clock_get_minutes();
	uint8_t hour	= clock_get_hours();
	uint8_t mon	= clock_get_month();
	uint8_t year	= clock_get_year();
	uint8_t day	= clock_get_day_of_month();

    // Convert BCD to binary values if necessary
	if (!(registerB & 0x04)) {
		sec 	= BCD2BIN(sec);
		min 	= BCD2BIN(min);
		hour 	= BCD2BIN(hour);
		day 	= BCD2BIN(day);
		mon 	= BCD2BIN(mon);
		year 	= BCD2BIN(year);
	}

	// Convert 12 hour clock to 24 hour clock if necessary
	if (!(registerB & 0x02) && (hour & 0x80)) {
		hour = ((hour & 0x7F) + 12) % 24;
	}

	time->sec 	= sec;
	time->min 	= min;
	time->hour	= hour;
	time->mon	= mon;
	time->year	= year;
	time->day	= day;

	return 0;
}

static ddword syscall_set_time(ddword time_ptr, ddword arg2, ddword arg3, ddword arg4) {
	date_t *time = (date_t*)time_ptr;

	uint8_t registerB = clock_get(REGB);

	uint8_t sec 	= time->sec;
	uint8_t min 	= time->min;
	uint8_t hour	= time->hour;
	uint8_t mon 	= time->mon;
	uint8_t year	= time->year;
	uint8_t day 	= time->day;

	if (!(registerB & 0x04)) {
		sec 	= BIN2BCD(sec);
		min 	= BIN2BCD(min);
		hour 	= BIN2BCD(hour);
		day 	= BIN2BCD(day);
		mon 	= BIN2BCD(mon);
		year 	= BIN2BCD(year);

	}

	if (!(registerB & 0x02) && (hour & 0x80)) {
		hour = ((hour & 0x7F) + 12) % 24;
	}

	clock_set_seconds(sec);
	clock_set_minutes(min);
	clock_set_hours(hour);
	clock_set_month(mon);
	clock_set_year(year);
	clock_set_day_of_month(day);

	return 0;
}

static ddword syscall_read(ddword fd, ddword buf, ddword size, ddword arg4) {
	int i = 0;
	char c;
	char *buffer;
	process_t *process;
	process_queue_t *task;
	node_t *node;
	fd_t *curr_fd;
	buffer = (char*)buf;

	process = current_process();
	task = current_task();

	if (fd == STDIN) {

		do {
			process->state = IO;
			task->ttl = 5+i;
			if (!is_buffer_empty()) {
				c = get_key();
				buffer[i++] = c;
			} 
		} while (i < size);

		process->state = RUNNING;

		return i;
	} else if (fd == STDFILE) {

		do {
			buffer[i++] = *( song_repository_addr + song_read_offset++ );
		} while ( i < size && *( song_repository_addr + song_read_offset ) != 0);
		process->state = RUNNING;
		return i;
	} else {

		node = list_get(process->fd, fd-1);

		curr_fd = (fd_t*)node->value;

		i = read_from_fd(curr_fd, buffer, size);

		process->state = RUNNING;

		return i;
	}

	return 0;
}

static ddword syscall_write(ddword fd, ddword buf, ddword size, ddword arg4) {

	char* buffer;
	process_t *process;
	node_t *node;
	fd_t *curr_fd;
	buffer = (char*)buf;


	if (fd == STDERR) {
		vsetbgcolor(WHITE_COLOR);
		vsetfgcolor(RED_COLOR);
		// set_colour(COLOR_BLACK, COLOR_RED);
	}

	if (fd == STDOUT || fd == STDERR) {
		if (size > 1) {
			vstring(buffer);
		} else {
			vchar(*buffer);
		}
	} else {

		process = current_process();

		node = list_get(process->fd, fd-1);

		if (!node) {
			logs("no such fd\n");
			return 0;
		}

		curr_fd = (fd_t*)node->value;

		logf("fd=%d c=%c\n", curr_fd->fd, *buffer);

		return write_to_fd(curr_fd, buffer, size);

	}

	if (fd == STDIN) {
		if (size > 1) {
			logs(buffer);
		} else {
			logc(*buffer);
		}
	}

	if (fd == STDERR) {
		vsetbgcolor(WHITE_COLOR);
		vsetfgcolor(BLACK_COLOR);
		// set_colour(COLOR_BLACK, COLOR_LIGHT_GREY);
	}
	return size;
}

static ddword syscall_beep(ddword arg1, ddword arg2, ddword arg3, ddword arg4) {
	beep();

	return 0;
}

static ddword syscall_play_sound(ddword length, ddword freq, ddword arg3, ddword arg4) {
	
	beeplf(length, freq);

	return 0;
}

static ddword syscall_event(ddword id, ddword func_ptr, ddword arg3, ddword arg4) {

	listener_t listener;

	listener = ( ddword(*)(syscall_id, ddword, ddword, ddword, ddword) )func_ptr;

	if (id == MOUSE) {
		
		add_mouse_listener(listener);
	} else if (id == KEYBOARD) {
		add_keyboard_listener(listener);
	} else {
		add_listener(id, listener);
	}

	return 0;
}

static ddword syscall_wait(ddword millis, ddword arg2, ddword arg3, ddword arg4) {

	process_queue_t *task;

	task = current_task();

	task->ttl = millis;
	task->current->state = SLEEP;


	_sim_interrupt();
	
	// wait(millis);

	return 0;
}

/**
 * Listener de syscalls
 * @param id   id del syscall
 * @param arg1 argumento 1
 * @param arg2 argumento 2
 * @param arg3 argumento 3
 */
static ddword on_ack_syscall(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	return syscalls[(uint8_t)id](arg1, arg2, arg3, arg4);
}

/**
 * Fija los listeners de las interrupciones
 */
void init_syscalls() {

	listener_t syscall_listener;

	syscalls[READ]       = &syscall_read;
	syscalls[WRITE]      = &syscall_write;
	syscalls[OPTS]       = &syscall_set_opts;
	syscalls[GET_TIME]   = &syscall_get_time;
	syscalls[SET_TIME]   = &syscall_set_time;
	syscalls[BEEP]       = &syscall_beep;
	syscalls[PLAY_SOUND] = &syscall_play_sound;
	syscalls[EVENT]      = &syscall_event;
	syscalls[WAIT]       = &syscall_wait;
	syscalls[SHUTDOWN]   = &syscall_shutdown;
	syscalls[HALT]       = &syscall_halt;
	syscalls[FORK]       = &syscall_fork;
	syscalls[GETPID]     = &syscall_getpid;
	syscalls[GETPPID]    = &syscall_getppid;
	syscalls[GETRESULT]  = &syscall_getresult;
	syscalls[WAITPID]    = &syscall_waitpid;
	syscalls[INIT_PROC]  = &syscall_init_proc;
	syscalls[VIDMODE]    = &syscall_vid_mode;
	syscalls[MALLOC]     = &syscall_malloc;
	syscalls[CALLOC]     = &syscall_calloc;
	syscalls[FREE]       = &syscall_free;
	syscalls[PS]         = &syscall_ps;
	syscalls[KILL]       = &syscall_kill;
	syscalls[STOP]       = &syscall_stop;
	syscalls[HOLD]       = &syscall_hold;
	syscalls[CONT]       = &syscall_cont;
	syscalls[SIGNAL]     = &syscall_signal;
	syscalls[BEGINCTX]   = &syscall_begin_ctx;
	syscalls[MKFIFO]     = &syscall_mkfifo;
	syscalls[FOPEN]      = &syscall_fopen;
	syscalls[FSIZE]      = &syscall_fsize;
	syscalls[SIZE]       = &syscall_size;
	syscalls[FSTART]     = &syscall_fstart;
	syscalls[START]      = &syscall_start;
	syscalls[FCLOSE]     = &syscall_fclose;
	syscalls[CLOSE]      = &syscall_close;
	syscalls[LS]         = &syscall_ls;
	syscalls[PIPE]       = &syscall_pipe;
	syscalls[DUP2]       = &syscall_dup2;
	syscalls[FSEEK]      = &syscall_fseek;
	syscalls[SEEK]       = &syscall_seek;
	syscalls[SCREENSHOT] = &syscall_screenshot;
	syscalls[GETPNAME]   = &syscall_getpname;
	syscalls[YIELD]			 = &syscall_yield;
	syscalls[EXIT]       = &syscall_exit;

	syscall_listener = &on_ack_syscall;

	add_listener(SYSCALL, syscall_listener);

}


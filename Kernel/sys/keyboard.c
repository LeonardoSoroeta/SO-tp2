#include <sys/keyboard.h>
#include <arch/arch.h>
#include <sys/logging.h>
#include <sys/video.h>

static ddword on_ack_keyboard(syscall_id, ddword, ddword, ddword, ddword);

static listener_pool_t event_listeners;

keyboard_status_t keyboard_status;
keyboard_buffer_t keyboard_buffer;

unsigned char KEYBOARD_MAP[2][TOTAL_KEYS] = {
	{
		0, NOT_PRINTABLE,'1','2','3','4','5','6','7','8','9','0','\'','<','\b',
		'\t','q','w','e','r','t','y','u','i','o','p','+','*','\n',
		NOT_PRINTABLE,'a','s','d','f','g','h','j','k','l',NOT_PRINTABLE,'{','}',
		NOT_PRINTABLE, '\\', 'z','x','c','v','b','n','m',',','.','-',NOT_PRINTABLE,NOT_PRINTABLE,
		NOT_PRINTABLE,SPACEBAR,NOT_PRINTABLE
	},
	{
		0, NOT_PRINTABLE,'!','@','#','$','%','&','/','(',')','=','?','>','\b',
		'\t','Q','W','E','R','T','Y','U','I','O','P','[',']','\n',
		NOT_PRINTABLE,'A','S','D','F','G','H','J','K','L',NOT_PRINTABLE,'{','}',
		NOT_PRINTABLE, '|', 'Z','X','C','V','B','N','M',';',':','_',NOT_PRINTABLE,NOT_PRINTABLE,
		NOT_PRINTABLE,SPACEBAR,NOT_PRINTABLE
	}
};



bool is_buffer_empty() {
	return (keyboard_buffer.buffer[keyboard_buffer.read_index] == EMPTY);
}

bool is_alpha(unsigned char keycode) {
	return
		(
			(keycode >= 0x10 && keycode <= 0x19) ||
			(keycode >= 0x1E && keycode <= 0x26) ||
			(keycode >= 0x2C && keycode <= 0x32)
		);
}

void insert_key(unsigned char ascii_value) {

	keyboard_buffer.buffer[keyboard_buffer.write_index++] = ascii_value;

	if (keyboard_buffer.write_index == KEYBOARD_BUFFER_SIZE) {
		keyboard_buffer.write_index = 0;
	}
}

unsigned char get_key() {

	unsigned char key = keyboard_buffer.buffer[keyboard_buffer.read_index];
	keyboard_buffer.buffer[keyboard_buffer.read_index++] = EMPTY;

	if (keyboard_buffer.read_index == KEYBOARD_BUFFER_SIZE) {
		keyboard_buffer.read_index = 0;
	}

	return key;
}

void key_received(unsigned char keycode) {

	bool _is_alpha;
	bool alternate;
	int i = 0;
	int captured;

	unsigned char ascii_value;

	if (keycode == CAPS_LOCK) {
		keyboard_status.caps_lock_enabled = !keyboard_status.caps_lock_enabled;
	}
	else if (keycode == LS_PRESSED || keycode == RS_PRESSED) {
		keyboard_status.shift_enabled = TRUE;
	}
	else if (keycode == RS_RELEASED || keycode == LS_RELEASED) {
		keyboard_status.shift_enabled = FALSE;
	}
	else if(!(keycode & KEY_RELEASED)) {

		_is_alpha = is_alpha(keycode);
		alternate = (_is_alpha && keyboard_status.caps_lock_enabled);

		if (keyboard_status.shift_enabled) {
			alternate = !alternate;
		}

		ascii_value = KEYBOARD_MAP[alternate][keycode];

		if (!(ascii_value == NOT_PRINTABLE)) {
			for (; i < event_listeners.size; i++) {
				captured = event_listeners.listeners[i](KEYBOARD, ascii_value, 0xe, 0xe, 0xe);
			}

			if (!captured) {
				insert_key(ascii_value);
			}
		}

	}

}

static ddword on_ack_keyboard(syscall_id id, ddword arg1, ddword arg2, ddword arg3, ddword arg4) {

	unsigned char status = _inport(KEYBOARD_STATUS_PORT);
	unsigned char keycode = _inport(KEYBOARD_DATA_PORT);

	if (status &  0x01) {
		key_received(keycode);
	}

	return 0;

}

void register_listener() {

	listener_t keyboard_listener;

	keyboard_listener = &on_ack_keyboard;

	add_listener(KEYBOARD, keyboard_listener);

}

void init_keyboard() {

	int i = 0;

	for (; i < KEYBOARD_BUFFER_SIZE; i++) {
		keyboard_buffer.buffer[i] = EMPTY;
	}

	event_listeners.size = 0;

	register_listener();

}

error_t add_keyboard_listener(listener_t listener) {

	if ( event_listeners.size >= MAX_LISTENERS_ALLOWED ) {
		return 1;
	}

	event_listeners.listeners[event_listeners.size++] = listener;

	return 0;

}

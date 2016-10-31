#include <arch/arch.h>
#include <sys/syscalls.h>
#include <sys/mouse.h>

#include <sys/video.h>
#include <sys/vga.h>

#include <sys/logging.h>

static uint8_t sensitivty = 2;

static bool is_visible = yes;

static listener_pool_t event_listeners;

void mouse_write(uint8_t a_write);
uint8_t mouse_read();
void mouse_install();
void mouse_wait(uint8_t a_type);
static ddword on_mouse_event(syscall_id, ddword, ddword, ddword, ddword);

void init_mouse() {

  listener_t mouse_listener;

  mouse_install();

  mouse_listener = &on_mouse_event;

  event_listeners.size = 0;

  add_listener(MOUSE, mouse_listener);
}

error_t add_mouse_listener(listener_t listener) {

  if ( event_listeners.size >= MAX_LISTENERS_ALLOWED ) {
    return 1;
  }

  event_listeners.listeners[event_listeners.size++] = listener;

  return 0;

}

//http://forum.osdev.org/viewtopic.php?t=10247
//Mouse.inc by SANiK
//License: Use as you wish, except to cause damage
uint8_t mouse_cycle = 0;     //unsigned char
int8_t mouse_byte[3];    //signed char
int8_t mouse_x=0;         //signed char
int8_t mouse_y=0;         //signed char

int16_t x = 0, y = 0;


static ddword on_mouse_event(syscall_id id, ddword rdi, ddword rsi, ddword rdx, ddword r10) {

  int i = 0;
  uint8_t buttons_flag = 0;

  if (is_visible)
    vunset_cursor(x/sensitivty, BUFFER_ROW-y/sensitivty);

  if (mouse_cycle == 0) {

    mouse_byte[0] = _inport(0x60);
    mouse_cycle++;

  } else if (mouse_cycle == 1) {

    mouse_byte[1] = _inport(0x60);
    mouse_cycle++;

  } else {

    mouse_byte[2] = _inport(0x60);
    mouse_x = mouse_byte[1];
    mouse_y = mouse_byte[2];
    if (mouse_byte[0] & 0x1) {
      buttons_flag |= 1 << 0;
    }
    if (mouse_byte[0] & 0x2) {
      buttons_flag |= 1 << 1;
    }
    if (mouse_byte[0] & 0x4) {
      buttons_flag |= 1 << 2;
    }

    mouse_cycle = 0;

    x += mouse_x;
    y += mouse_y;

    if (x/sensitivty < 0 || x/sensitivty >= VGA_WIDTH-16) {
      x -= mouse_x;
    }

    if (BUFFER_ROW-y/sensitivty < 0 || BUFFER_ROW-y/sensitivty >= VGA_HEIGHT-16) {
      y -= mouse_y;
    }
  }
  if (mouse_cycle == 0) {

    // logs("x=");
    // logi(x);
    // logs(" y=");
    // logi(y);
    // logs(" mx=");
    // logi(mouse_x);
    // logs(" my=");
    // logi(mouse_y);
    // lognl();

    if (is_visible) {
      vset_cursor(x/sensitivty, BUFFER_ROW-y/sensitivty);
    }
    for (; i < event_listeners.size; i++) {
      event_listeners.listeners[i](MOUSE, x/sensitivty, BUFFER_ROW-y/sensitivty, buttons_flag, 0xe);
    }
  }

  return 0;

  // prints("x: ");
  // puti(x);
  // prints(" y: ");
  // puti(y);
  // putc('\n');
  
}

void mouse_set_visible(bool b) {
  if (is_visible && !b) {
    is_visible = b;
    vunset_cursor(x/sensitivty, BUFFER_ROW - y/sensitivty);
  } else if (!is_visible && b) {
    is_visible = b;
    vset_cursor(x/sensitivty, BUFFER_ROW - y/sensitivty);
  }
}

void mouse_set_sensitivity(uint8_t s) {
  vunset_cursor(x/sensitivty, BUFFER_ROW - y/sensitivty);
  sensitivty = s;
  vset_cursor(x/sensitivty, BUFFER_ROW - y/sensitivty);
}

void mouse_wait(uint8_t a_type) {
  dword _time_out = 100000; //unsigned int
  if ( a_type == 0) {
    while(_time_out--) {
      if((_inport(0x64) & 1)==1) {
        return;
      }
    }
    return;
  }
  else {
    while(_time_out--) {
      if((_inport(0x64) & 2) == 0) {
        return;
      }
    }
    return;
  }
}

void mouse_write(uint8_t a_write) {
  //Wait to be able to send a command
  mouse_wait(1);
  //Tell the mouse we are sending a command
  _outport(0x64, 0xD4);
  //Wait for the final part
  mouse_wait(1);
  //Finally write
  _outport(0x60, a_write);
}

uint8_t mouse_read() {
  //Get's response from mouse
  mouse_wait(0); 
  return _inport(0x60);
}

void mouse_install() {
  byte _status;  //unsigned char

  //Enable the auxiliary mouse device
  mouse_wait(1);
  _outport(0x64, 0xA8);
  
  //Enable the interrupts
  mouse_wait(1);
  _outport(0x64, 0x20);
  mouse_wait(0);
  _status=(_inport(0x60) | 2);
  
  mouse_wait(1);
  _outport(0x64, 0x60);
  
  mouse_wait(1);
  _outport(0x60, _status);
  
  //Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read();  //Acknowledge
  
  //Enable the mouse
  mouse_write(0xF4);
  mouse_read();  //Acknowledge

}

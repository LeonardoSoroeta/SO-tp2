#include <sys/vga.h>
#include <arch/serial.h>
#include <util/util.h>
#include <sys/bochs.h>
#include <arch/hard.h>
#include <sys/fonts/font8x8_latin.h>
#include <sys/logging.h>
#include <sys/timer.h>
#include <arch/pmem.h>
#include <stdarg.h>

/**
 *
 *  en userland los syscalls son:
 *
 *  - begincontext: devuelve un
 *  puntero a la zona de memoria
 *  donde dibujar
 *  - commitcontext: dibuja y hace un clear
 * 
 */


// unsigned fb_seg;
// unsigned long* vga_mem = (unsigned long*)0xA0000;

static void vnumber(va_list ap, char c);
static void vvfprintf(char * fmt, va_list ap);

static unsigned char* vga;
static uint16_t current_offset_x = 0;
static uint16_t current_offset_y = 0;

static uint32_t fg_color = BLACK_COLOR;
static uint32_t bg_color = WHITE_COLOR;

static uint32_t* screenshot;

static uint8_t is_cursor_visible = 0;
static uint32_t cursor[16][16] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x7D7D7D, 0x7D7D7D, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0xFDFDFD, 0x7D7D7D, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0xFDFDFD, 0x666666, 0xFDFDFD, 0xFDFDFD, 0x7D7D7D, 0x7D7D7D, 0x7D7D7D, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x3A3A3A, 0xFDFDFD, 0x666666, 0x7D7D7D, 0x666666, 0xFDFDFD, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x7D7D7D, 0x666666, 0x7D7D7D, 0x000000, 0x7D7D7D, 0x666666, 0xFDFDFD, 0x7D7D7D, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x7D7D7D, 0x000000, 0x000000, 0x000000, 0x7D7D7D, 0x666666, 0xFDFDFD, 0x7D7D7D, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x7D7D7D, 0x666666, 0x7D7D7D, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x7D7D7D, 0x000000, 0x000000, 0x00, 0x00 }
};

static uint32_t cursorbkp[16][16] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00, 0x00 }
};

void init_vga() {
	vga = (unsigned char*)_get_vga_address();
	current_offset_x = INIT_X;
	current_offset_y = INIT_Y;

	// get pages for entire screen
	screenshot = (uint32_t*)pmalloc(0x300*0x1000+1);

	logs("Setting up VGA\n");
	logs("Buffer at: ");
	loghex((uint64_t)vga);
	lognl();

	logs("x=");
	logi(current_offset_x);
	logs(" y=");
	logi(current_offset_y);
	logs(" maxh=");
	logi(MAXH);
	logs(" maxv=");
	logi(MAXV);
	lognl();

	// bochs_vbe_exit_mode();

	// bochs_vbe_set_mode(1024, 768, 24);
}

unsigned char* get_buffer() {
	return vga;
}

uint32_t vgetcolor(uint16_t x, uint16_t y) {
	return COLOR_RGB(vga[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_R], vga[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_G], vga[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_B]);
}

void vputpixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
	vga[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_R] = r & 0xff;
	vga[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_G] = g & 0xff;
	vga[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_B] = b & 0xff;
}

void vputpixelc(uint16_t x, uint16_t y, uint32_t color) {
	vputpixel(x, y, COLOR_R(color), COLOR_G(color), COLOR_B(color));
}

void vrectfill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
	uint16_t i = 0, j = 0;
	for (i = y; i < h+y; i++) {
		for (j = x; j < w+x; j++) {
			vputpixelc(j, i, color);
		}
	}
}

void vrect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
	uint16_t j = 0;
	for (j = x; j < w+x; j++) {
		vputpixelc(j, y, color);
		vputpixelc(j, y+h, color);
	}
	for (j = y; j < h+y; j++) {
		vputpixelc(x, j, color);
		vputpixelc(x+w, j, color);
	}
}

void vline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) {
	uint16_t i = x1 > x2 ? x2 : x1;
	uint16_t end = x1 > x2 ? x1 : x2;
	uint16_t ymin = y1 > y2 ? y2 : y1;
	float m = (float)(y2-y1)/(x2-x1);
	for (; i <= end; i++) {
		vputpixelc(i, (uint16_t)(ymin+i*m), color);
	}
}

void vellipse(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color) {
	double i = 0;
	for (; i < TWOPI; i+=TWOPI/360) {
		vputpixelc(xc+w/2*cos(i), yc+h/2*sin(i), color);
	}
}

void vellipsefill(uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color) {
	double i = 0, j = 0;
	for (; i <= TWOPI; i+=TWOPI/360) {
		for (j = xc-w/2*cos(i); j <= xc+w/2*cos(i); j++) {
			vputpixelc(j, yc-h/2*sin(i), color);
		}
	}
}

void vpoly(uint16_t xc, uint16_t yc, uint8_t sides, uint16_t r, uint32_t color) {
	
	uint8_t i = 0;

	for (; i < sides; i++) {
		// logi( xc + r * cos( TWOPI * i / sides ) );
		// logc(' ');
		// logi( yc + sin( TWOPI * i / sides ) );
		vline(xc + r * cos( TWOPI * i / sides ), yc + r * sin( TWOPI * i / sides ), xc + r * cos( TWOPI * (i+1) / sides ), yc + r * sin( TWOPI * (i+1) / sides ), color );

	}


}

void vcircle(uint16_t h, uint16_t k, uint16_t r, uint32_t color) {
	vellipse(h, k, r, r, color);
}

void vcirclefill(uint16_t h, uint16_t k, uint16_t r, uint32_t color) {
	vellipsefill(h, k, r, r, color);
}

void vsetfgcolor(uint32_t color) {
	fg_color = color;
}

void vsetbgcolor(uint32_t color) {
	bg_color = color;
}

void vcharxy(uint16_t x, uint16_t y, unsigned char c) {
	uint16_t i = 0, j = 0;
	char *glyph;
	if (c > 127) {
		c += 64; // bug fix
		glyph = font8x8_ext_latin[c-160];
	} else {
		glyph = font8x8_basic[c];
	}
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			if (glyph[i] & (1 << j)) {
				vputpixelc(x+j, y+i, fg_color);
			}
		}
	}
}

void vnl() {

	current_offset_x = INIT_X;
	current_offset_y += CHAR_HEIGHT + LINE_SPACING;

	if (current_offset_y >= MAXV) {
		vmove_up();

		current_offset_y -= LINES(1);
	}

}

void vchar(unsigned char c) {
	if (c == '\n') {
		vnl();
	} else if ( c == '\b') {
		current_offset_x -= (CHAR_WIDTH + CHAR_SPACING);
		if (current_offset_x < INIT_X) {
			current_offset_x = MAXH - CHAR_SPACING - CHAR_WIDTH;
			current_offset_y -= (LINE_SPACING + CHAR_HEIGHT);
		}
		vrectfill(current_offset_x, current_offset_y, CHAR_WIDTH, CHAR_HEIGHT, bg_color);
	} else if ( c == '\t') {
		vchar(' ');
		vchar(' ');
	} else {
		vcharxy(current_offset_x, current_offset_y, c);
		current_offset_x += CHAR_WIDTH + CHAR_SPACING;
		if (current_offset_x >= MAXH) {
			current_offset_x = INIT_X;
			current_offset_y += LINE_SPACING + CHAR_HEIGHT;
		}
		// if (current_offset_y >= MAXV) {
		// 	vmove_up();
		// }
	}
}

void vstringxy(uint16_t x, uint16_t y, char* str) {

	while (*str != '\0') {

		vcharxy(x, y, *str);

		x += CHARS(1);

		str++;
	}

}

void vstring(char *str) {
	while (*str != '\0') {
		vchar(*str);
		str++;
	}
}

void vclc() {

	current_offset_x = INIT_X;
	current_offset_y = INIT_Y;

	vrectfill(0, 0, VGA_WIDTH, VGA_HEIGHT, bg_color);
}

void vclcmid() {

	current_offset_x = INIT_X;
	current_offset_y = INIT_Y;

	vrectfill(0, 0, VGA_WIDTH/2, VGA_HEIGHT, bg_color);
}

void vclr(uint16_t row) {
	vrectfill(INIT_X, INIT_Y+row*(CHAR_HEIGHT+LINE_SPACING), MAXH, CHAR_HEIGHT, bg_color);
}

void vcopy(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t x2, uint16_t y2) {

	uint16_t i;
	uint16_t j;

	for (i = 0; i < h; i++) {

		for (j = 0; j < w; j++) {

			vputpixelc(x2+j, y2+i, vgetcolor(x1+j,y1+i));

		}

	}

}

void vmove_up() {

	uint16_t i;
	uint16_t j;

	for (i = INIT_Y+LINES(1); i < MAXV; i += LINES(1)) {

		for (j = INIT_X; j < MAXH; j += CHARS(1)) {


			vcopy(j, i, CHAR_WIDTH, CHAR_HEIGHT, j, i-LINES(1));

		}
	}

	vclr(MAXCHARV-1);
	vclr(MAXCHARV);
	// vclr(MAXCHARV+1);
	
}

void vputnum(int64_t i, uint8_t base) {
	char *buf = "", *res;

	res = itoa(i, buf, base);

	vstring(res);
	
}

void vputbin(int64_t i) {
	vputnum(i, 2);
}

void vputi(int64_t i) {

	string s;

	s = itoa10(i);

	// putnum(i, 10);
	vstring(s);
}

void vputhex(int64_t i) {
	// putnum(i, 16);
	// 
	string s;

	s = itoa16(i);

	// putnum(i, 10);
	vstring("0x");
	vstring(s);
}

void vset_cursor(uint16_t x, uint16_t y) {

	uint16_t i = 0;
	uint16_t j = 0;

	for (; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			cursorbkp[i][j] = vgetcolor(x+j, y+i);
			if (cursor[i][j] != 0x00) {
				vputpixelc(x+j, y+i, cursor[i][j]);
			}
			
		}
	}

}

void vunset_cursor(uint16_t x, uint16_t y) {

	uint16_t i = 0;
	uint16_t j = 0;

	for (; i < 16; i++) {
		for (j = 0; j < 16; j++) {
			if (is_cursor_visible != 0) {
			vputpixelc(x+j, y+i, cursorbkp[i][j]);
			}else{
			vputpixelc(x+j, y+i, vgetcolor(x+j, y+i));
			}
		}
	}
	is_cursor_visible = 1;
}

void vset_invisible_cursor(){
	is_cursor_visible = 0;
}

uint32_t* take_screenshot() {

	uint16_t i = 0;
	uint16_t j = 0;

	for (; i < VGA_HEIGHT; i++) {
		for (j = 0; j < VGA_WIDTH; j++) {
			screenshot[i*VGA_WIDTH+j] = vgetcolor(j, i); 
		}
	}

	return screenshot;
}

void vprintf(char * fmt, ...) {
	va_list ap;
	
	va_start(ap, fmt);
	vvfprintf(fmt, ap);
	va_end(ap);
}

static void vnumber(va_list ap, char c) {

	switch (c) {
		case 's':
			vstring(va_arg(ap, char *));
			break;

		case 'd':
			vputi(va_arg(ap, int));
			break;

		case 'o':
			vputhex(va_arg(ap, int));
			break;

		case 'x':
			vputhex(va_arg(ap, int));
			break;

		case 'c':
			vchar(va_arg(ap, int));
			break;

		default:
			vchar(c);
		
	}
}

static void vvfprintf(char * fmt, va_list ap) {
	char c;
	char * ptr = fmt;
	int in_fmt = 0;

	while ((c = *ptr++) != (char) 0) {
		if (in_fmt && c == '%') {
			in_fmt = 0;
			vchar(c);
		} else if (in_fmt) {
			vnumber(ap, c);
			in_fmt = 0;
		} else if (c == '%') {
			in_fmt = c;
		} else {
			vchar(c);
		}
	}
}
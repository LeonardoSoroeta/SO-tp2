#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stdint.h>
#include <stdlib.h>

// http://wiki.osdev.org/Drawing_In_Protected_Mode
#define VGA_WIDTH 1024
#define VGA_HEIGHT 768

#define VGA_DEPTH 24
#define VGA_PITCH VGA_WIDTH*VGA_DEPTH/8
#define VGA_PIXELWIDTH VGA_DEPTH/8

#define COLOR_RGB(r, g, b) (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 255))
#define COLOR_R(color) ((color >> 16) & 255)
#define COLOR_G(color) ((color >> 8) & 255)
#define COLOR_B(color) ((color >> 0) & 255)

#define ATTR_R 0
#define ATTR_G 1
#define ATTR_B 2

#define BLACK_COLOR 0x000000
#define WHITE_COLOR 0xffffff
#define RED_COLOR 0x0000ff
#define GREEN_COLOR 0x00ff00
#define BLUE_COLOR 0xff0000

// taken from CakeOS

typedef struct {
	unsigned short type;     //Should be "BM"
	unsigned int size;       //Size in DWORDS of file
	unsigned int reserved;   //Reserved 1 and 2 (should be 0)
	unsigned int offset;     //Data
	unsigned int header_size; //Should be 40
	unsigned int width;      //Width in px
	unsigned int height;     //Height in px
	unsigned short planes;   //Number of planes used
	unsigned short bits_per_pixel;      //BPP (1, 4, 8 or 24)
	unsigned int compression;         //Should be 0
	unsigned int size_image;  //Size of image in bytes
	unsigned int x_pixels_per_meter;     //erm...? (usually 0)  
	unsigned int y_pixels_per_meter;     //... (usually 0)
	unsigned int colors_used;          //Number of colours used
	unsigned int colors_important;     //Important colours
} __attribute__((packed)) bmp_header_t;

typedef enum {
	shape_type_rect,
	shape_type_ellipse,
	shape_type_bmp
} shape_type_t;

typedef struct gfx_node {

	void *node;
	shape_type_t shape_type;
	int id;

	struct gfx_node *next;

} gfx_node;

typedef struct {

	unsigned char *mem_addr;

	double width;
	double height;

	double x;
	double y;

	gfx_node *initial_node;

} gfx_context;

typedef struct {

	uint16_t x;
	uint16_t y;

	uint16_t last_x;
	uint16_t last_y;

	uint16_t width;
	uint16_t height;

	uint32_t color;

	char visible;

	// uint32_t* background;

	// char border;
	// uint32_t border_color;

} gfx_rect;

typedef struct {

	uint16_t x;
	uint16_t y;

	uint16_t last_x;
	uint16_t last_y;

	char visible;

	bmp_header_t* header;

} gfx_bmp;

typedef struct {

	uint16_t xc;
	uint16_t yc;

	uint16_t last_xc;
	uint16_t last_yc;

	uint16_t width;
	uint16_t height;

	uint32_t color;

	char visible;

	// uint32_t* background;

	// char border;
	// uint32_t border_color;

} gfx_ellipse;

uint16_t translate_x(gfx_context* ctx, int32_t x);
uint16_t translate_y(gfx_context* ctx, int32_t y);

int displaybmp(gfx_context *ctx, int x, int y, bmp_header_t* bmp);

gfx_context* begin_context(uint16_t w, uint16_t h, uint16_t x, uint16_t y);

void set_pixel_rgb(gfx_context* ctx, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);
void set_pixel_color(gfx_context* ctx, uint16_t x, uint16_t y, uint32_t color);
uint32_t get_pixel_color(gfx_context* ctx, int32_t x, int32_t y);

void ctx_clear(gfx_context *ctx);

void draw_rect_fill(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void draw_rect(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
void draw_linev(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t h, uint32_t color);
void draw_lineh(gfx_context* ctx, uint16_t x1, uint16_t y1, uint16_t w, uint32_t color);
void draw_line(gfx_context* ctx, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
void draw_ellipse(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color);
void draw_ellipse_fill(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color);
void draw_circle(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t r, uint32_t color);
void draw_circle_fill(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t r, uint32_t color);
void draw_polygon(gfx_context* ctx, uint16_t xc, uint16_t yc, uint8_t sides, uint16_t r, uint32_t color);
void draw_polygon_fill(gfx_context* ctx, uint16_t xc, uint16_t yc, uint8_t sides, uint16_t r, uint32_t color);


gfx_rect *rect_create(gfx_context*, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
gfx_rect *rect_tick(gfx_context*, gfx_rect*);
void rect_free(gfx_rect*);

gfx_ellipse *ellipse_create(gfx_context*, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color);
gfx_ellipse *ellipse_tick(gfx_context*, gfx_ellipse*);
void ellipse_free(gfx_ellipse*);

gfx_bmp *bmp_create(gfx_context *ctx, bmp_header_t* header, uint16_t x, uint16_t y);
gfx_bmp *bmp_tick(gfx_context*, gfx_bmp*);
void bmp_free(gfx_bmp*);

gfx_node *node_create(void *shape, shape_type_t shape_type);
gfx_context *add_node(gfx_context *ctx, gfx_node* node);
gfx_context *remove_node(gfx_context* ctx, void* node);
void free_node(gfx_node* node);

gfx_context *context_tick(gfx_context* ctx);

#endif
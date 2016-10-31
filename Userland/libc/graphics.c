#include <graphics.h>
#include <libc.h>
#include <math.h>
#include <stdio.h>
#include <stddef.h>

#define BACKGROUND_COLOR 0xededed

static inline double radian_to_degree(double radians);
static inline double degree_to_radian(double degree);

uint16_t translate_x(gfx_context* ctx, int32_t x) {
	return ctx->x + x;
}

uint16_t translate_y(gfx_context* ctx, int32_t y) {
	return ctx->y + y;
}

static inline double radian_to_degree(double radians) {
	return radians * 57.296;
}

static inline double degree_to_radian(double degree) {
	return degree * 0.017;
}

gfx_context* begin_context(uint16_t width, uint16_t height, uint16_t x, uint16_t y) {

	gfx_context* ctx;

	ctx = (gfx_context*)malloc(sizeof(gfx_context));

	ctx->mem_addr = (unsigned char*)ctx_begin();

	ctx->width = width;
	ctx->height = height;

	ctx->x = x;
	ctx->y = y;

	ctx->initial_node = NULL;

	return ctx;

}

gfx_node *node_create(void *shape, shape_type_t shape_type) {

	gfx_node *node;

	node = (gfx_node*)malloc(sizeof(gfx_node));

	node->node = shape;
	node->shape_type = shape_type;
	node->next = NULL;
	node->id = -1;

	return node;

}

gfx_context *add_node(gfx_context *ctx, gfx_node* node) {

	gfx_node *initial, *prev = NULL;

	if (!ctx->initial_node) {
		ctx->initial_node = node;
		ctx->initial_node->id = 0;
	} else {

		initial = ctx->initial_node;

		do {
			prev = initial;
			initial = initial->next;
		} while (initial != NULL);

		node->id = prev->id + 1;
		prev->next = node;

	}

	return ctx;
}

gfx_context *remove_node(gfx_context* ctx, void* node) {

	gfx_node *curr, *prev = NULL;

	if (!ctx->initial_node) {
		return ctx;
	} else {

		curr = ctx->initial_node;

		do {
			if (curr->node == node) {


				// switch (curr->shape_type) {
				// 	case shape_type_rect:
				// 		((gfx_rect*)curr->node)->visible = 0;
				// 		break;
				// 	case shape_type_ellipse:
				// 		((gfx_ellipse*)curr->node)->visible = 0;
				// 		break;
				// 	case shape_type_bmp:
				// 		((gfx_bmp*)curr->node)->visible = 0;
				// 		break;
				// }

				if (!prev) {
					ctx->initial_node = ctx->initial_node->next;
				} else {
					prev->next = curr->next;
				}
				// curr->visible = 0;
				// free_node(curr);
				return ctx;
			}
			prev = curr;
			curr = curr->next;
		} while (curr != NULL);
	}

	return ctx;

}

void free_node(gfx_node* node) {

	free(node->node);
	free(node);

}

void set_pixel_rgb(gfx_context *ctx, uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b) {
	ctx->mem_addr[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_R] = r & 0xff;
	ctx->mem_addr[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_G] = g & 0xff;
	ctx->mem_addr[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_B] = b & 0xff;
}

uint32_t get_pixel_color(gfx_context* ctx, int32_t x, int32_t y) {
	x = translate_x(ctx, x);
	y = translate_y(ctx, y);
	return COLOR_RGB(ctx->mem_addr[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_R], ctx->mem_addr[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_G], ctx->mem_addr[x*VGA_PIXELWIDTH+y*VGA_PITCH+ATTR_B]);
}

void set_pixel_color(gfx_context *ctx, uint16_t x, uint16_t y, uint32_t color) {
	set_pixel_rgb(ctx, x, y, COLOR_R(color), COLOR_G(color), COLOR_B(color));
}

void ctx_clear(gfx_context *ctx) {
	draw_rect_fill(ctx, 0, 0, ctx->width, ctx->height, BACKGROUND_COLOR);
}

void draw_rect_fill(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
	uint16_t i = 0, j = 0;
	x = translate_x(ctx, x);
	y = translate_y(ctx, y);
	for (i = y; i < h+y; i++) {
		for (j = x; j < w+x; j++) {
			set_pixel_color(ctx, j, i, color);
		}
	}
}

void draw_rect(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {
	uint16_t j = 0;
	x = translate_x(ctx, x);
	y = translate_y(ctx, y);
	for (j = x; j < w+x; j++) {
		set_pixel_color(ctx, j, y, color);
		set_pixel_color(ctx, j, y+h, color);
	}
	for (j = y; j < h+y; j++) {
		set_pixel_color(ctx, x, j, color);
		set_pixel_color(ctx, x+w, j, color);
	}
}

void draw_linev(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t h, uint32_t color) {
	uint16_t i = 0;
	x = translate_x(ctx, x);
	y = translate_y(ctx, y);
	for (; i < h; i++) {
		set_pixel_color(ctx, x, y+i, color);
	}
}

void draw_lineh(gfx_context* ctx, uint16_t x, uint16_t y, uint16_t w, uint32_t color) {
	uint16_t i = 0;
	x = translate_x(ctx, x);
	y = translate_y(ctx, y);
	for (; i < w; i++) {
		set_pixel_color(ctx, x+i, y, color);
	}
}


void draw_line(gfx_context* ctx, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) {
	uint16_t i;
	uint16_t end;
	uint16_t ymin;
	float m;

	x1 = translate_x(ctx, x1);
	x2 = translate_x(ctx, x2);
	y1 = translate_y(ctx, y2);
	y2 = translate_y(ctx, y2);
	
	i = x1 > x2 ? x2 : x1;
	end = x1 > x2 ? x1 : x2;
	ymin = y1 > y2 ? y2 : y1;
	m = (float)(y2-y1)/(x2-x1);

	for (; i <= end; i++) {
		set_pixel_color(ctx, i, (uint16_t)(ymin+i*m), color);
	}
}

void draw_ellipse(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color) {
	double i = 0;
	xc = translate_x(ctx, xc);
	yc = translate_y(ctx, yc);
	for (; i < TWOPI; i+=TWOPI/360) {
		set_pixel_color(ctx, xc+w/2*cos(i), yc+h/2*sin(i), color);
	}
}

void draw_ellipse_fill(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color) {
	double i = 0, j = 0;
	xc = translate_x(ctx, xc);
	yc = translate_y(ctx, yc);
	for (; i <= TWOPI; i+=TWOPI/360) {
		for (j = xc-w/2*cos(i); j <= xc+w/2*cos(i); j++) {
			set_pixel_color(ctx, j, yc-h/2*sin(i), color);
		}
	}
}

void draw_circle(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t r, uint32_t color) {
	draw_ellipse(ctx, xc, yc, r, r, color);
}

void draw_circle_fill(gfx_context* ctx, uint16_t xc, uint16_t yc, uint16_t r, uint32_t color) {
	draw_ellipse_fill(ctx, xc, yc, r, r, color);
}

void draw_polygon(gfx_context* ctx, uint16_t xc, uint16_t yc, uint8_t sides, uint16_t r, uint32_t color) {
	
	uint8_t i = 0;
	xc = translate_x(ctx, xc);
	yc = translate_y(ctx, yc);

	for (; i < sides; i++) {
		draw_line(ctx, xc + r * cos( TWOPI * i / sides ), yc + sin( TWOPI * i / sides ), xc + r * cos( TWOPI * (i+1) / sides ), yc + sin( TWOPI * (i+1) / sides ), color );

	}


}

void draw_polygon_fill(gfx_context* ctx, uint16_t xc, uint16_t yc, uint8_t sides, uint16_t r, uint32_t color) {
	draw_ellipse_fill(ctx, xc, yc, r, r, color);
}

gfx_rect *rect_create(gfx_context *ctx, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t color) {

	gfx_rect* rect;

	rect = (gfx_rect*)malloc(sizeof(gfx_rect));

	if (!rect) {
		fprintf(STDERR, "rect is null\n");
		return NULL;
	}

	rect->x = x;
	rect->y = y;

	rect->last_x = x;
	rect->last_y = y;

	rect->width = w;
	rect->height = h;

	rect->color = color;

	rect->visible = 1;


	return rect;
}

gfx_rect *rect_tick(gfx_context *ctx, gfx_rect* rect) {

	// dont redraw if is has not moved
	// if (rect->last_x == rect->x && rect->last_y == rect->y) {
	// 	return rect;
	// }

	if (!rect->visible) {
		return rect;
	}

	draw_rect_fill(ctx, rect->last_x, rect->last_y, rect->width, rect->height, BACKGROUND_COLOR);
	draw_rect_fill(ctx, rect->x, rect->y, rect->width, rect->height, rect->color);

	rect->last_x = rect->x;
	rect->last_y = rect->y;

	return rect;
}

gfx_bmp *bmp_create(gfx_context *ctx, bmp_header_t* header, uint16_t x, uint16_t y) {

	gfx_bmp* bmp;

	bmp = (gfx_bmp*)malloc(sizeof(gfx_bmp));

	bmp->x = x;
	bmp->y = y;

	bmp->last_x = x;
	bmp->last_y = y;

	bmp->header = header;

	bmp->visible = 1;

	return bmp;
}

gfx_bmp *bmp_tick(gfx_context *ctx, gfx_bmp* bmp) {

	// dont redraw if is has not moved
	// if (bmp->last_x == bmp->x && bmp->last_y == bmp->y) {
	// 	return bmp;
	// }

	if (!bmp->visible) {
		return bmp;
	}

	draw_rect_fill(ctx, bmp->last_x, bmp->last_y, bmp->header->width, bmp->header->height+1, BACKGROUND_COLOR);
	displaybmp(ctx, bmp->x, bmp->y, bmp->header);

	bmp->last_x = bmp->x;
	bmp->last_y = bmp->y;

	return bmp;
}

gfx_ellipse *ellipse_create(gfx_context *ctx, uint16_t xc, uint16_t yc, uint16_t w, uint16_t h, uint32_t color) {

	gfx_ellipse* ellipse;

	ellipse = (gfx_ellipse*)malloc(sizeof(gfx_ellipse));

	if (!ellipse) {
		fprintf(STDERR, "ellipse is null\n");
		return NULL;
	}

	ellipse->xc = xc;
	ellipse->yc = yc;

	ellipse->last_xc = xc;
	ellipse->last_yc = yc;

	ellipse->width = w;
	ellipse->height = h;

	ellipse->color = color;

	ellipse->visible = 1;


	return ellipse;
}

gfx_ellipse *ellipse_tick(gfx_context *ctx, gfx_ellipse* ellipse) {

	// dont redraw if there is no movement
	// if (ellipse->last_xc == ellipse->xc && ellipse->last_yc == ellipse->yc) {
	// 	return ellipse;
	// }

	if (!ellipse->visible) {
		return ellipse;
	}

	draw_ellipse_fill(ctx, ellipse->last_xc, ellipse->last_yc, ellipse->width, ellipse->height, BACKGROUND_COLOR);
	draw_ellipse_fill(ctx, ellipse->xc, ellipse->yc, ellipse->width, ellipse->height, ellipse->color);

	ellipse->last_xc = ellipse->xc;
	ellipse->last_yc = ellipse->yc;

	return ellipse;
}


gfx_context *context_tick(gfx_context *ctx) {

	gfx_node *node;
	shape_type_t st;

	node = ctx->initial_node;

	do {

		st = node->shape_type;

		if (st == shape_type_rect) {
			rect_tick(ctx, (gfx_rect*)node->node);
		} else if (st == shape_type_ellipse) {
			ellipse_tick(ctx, (gfx_ellipse*)node->node);
		} else if (st == shape_type_bmp) {
			bmp_tick(ctx, (gfx_bmp*)node->node);
		}


		node = node->next;
	} while (node != NULL);


	return ctx;
}





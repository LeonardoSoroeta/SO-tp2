#include <bmp.h>
#include <graphics.h>

// taken from CakeOS
// 

/* CakeOS */

//     WIP++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

static int displaybmp_8bit(gfx_context *ctx, int x, int y, bmp_header_t* bmp);
static int displaybmp_32bit(gfx_context *ctx, int x, int y, bmp_header_t* bmp);

static int padding(int n);

static int padding(int n) {

	if ( n % 4 == 0 )
		return 0;

	return (n / 4 + 1) * 4 - n;

}

/* FOR EACH PIXEL

R = 54 + (Color*4)+2
G = 54 + (Color*4)+1
B = 54 + (Color*4)
finalpixel = R<<16 | G << 8 | B 

where Color is the value at the offset */

int displaybmp(gfx_context *ctx, int x, int y, bmp_header_t* bmp) {

	if (bmp->type != 0x4D42) {
		return 1;
	}

	if (bmp->compression != 0) {
		return 2;
	}

	x = translate_x(ctx, x);
	y = translate_y(ctx, y);

	switch (bmp->bits_per_pixel) {
		case 1:
			break;

		case 4:
			break;

		case 8:
			return displaybmp_8bit(ctx, x, y, bmp);
		case 24:
			return displaybmp_32bit(ctx, x, y, bmp);
		case 32:
			return displaybmp_32bit(ctx, x, y, bmp);
		default:
			return 5;
			break;
	}
	return 0;   
}

int displaybmp_8bit(gfx_context *ctx, int x, int y, bmp_header_t* bmp) {
	
	uint32_t i, j;
	uint32_t w, h;
	uint8_t color;
	uint8_t* pixel_ptr;

	h = bmp->height;
	w = bmp->width;

	if (bmp->bits_per_pixel != 8) {
		return 1;
	}

	for ( i = 0; i < h; i++) {

		for (j = 0; j < w; j++) {

			color = *pixel_ptr++;

			set_pixel_color(ctx, x+j, y+(h-i), COLOR_RGB( ((color & 0xE0) >> 5) * 17, ((color & 0x1C) >> 2) * 17, (color & 0x03) * 67 ) );

		}

		pixel_ptr += padding(w);

	}

	for (y = h-1; y >= 0; y--) {
		for (x = 0; x < w; x++) {
			//Only print actual pixels, for null values just iterate
			if (x <= bmp->width) {
				//TODO: get RGB value from quad/pallette
				set_pixel_color(ctx, x, y, (uint32_t)(uintptr_t)(( ((void*)bmp) + bmp->offset )+(y*w)+x));
			}
		}
	}
	return 0;
}

int displaybmp_32bit(gfx_context *ctx, int x, int y, bmp_header_t* bmp) {
	
	uint32_t i, j;
	uint32_t w, h;
	uint8_t R, G, B;
	uint8_t* pixel_ptr;
	h = bmp->height;
	w = bmp->width;

	if (bmp->bits_per_pixel != 24 && bmp->bits_per_pixel != 32) {
		return 2432;
	}

	pixel_ptr = (uint8_t*)(((void*)bmp) + bmp->offset);

	for ( i = 0; i < h; i++) {

		for (j = 0; j < w; j++) {

			R = *pixel_ptr++;
			G = *pixel_ptr++;
			B = *pixel_ptr++;

			set_pixel_color(ctx, x+j, y+(h-i), COLOR_RGB(R, G, B));

		}

		pixel_ptr += padding(w);

	}

	return 0;
}

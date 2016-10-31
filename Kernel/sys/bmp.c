#include <sys/bmp.h>
#include <sys/vga.h>
#include <sys/logging.h>

// taken from CakeOS
// 

static int padding(int n);

static int padding(int n) {

	if ( n % 4 == 0 )
		return 0;

	return (n / 4 + 1) * 4 - n;

}

/* CakeOS */

//     WIP++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

/* FOR EACH PIXEL

R = 54 + (Color*4)+2
G = 54 + (Color*4)+1
B = 54 + (Color*4)
finalpixel = R<<16 | G << 8 | B 

where Color is the value at the offset */

int displaybmp(int x, int y, bmp_header_t* bmp) {

	logf("type:%x size:%d offset:%d hs:%d width:%d height:%d planes:%d bpp:%d comp:%d size:%d\n", bmp->type, bmp->size, bmp->offset, bmp->header_size, bmp->width, bmp->height, bmp->planes, bmp->bits_per_pixel, bmp->compression, bmp->size_image);

	if (bmp->type != 0x4D42) {
		return 1;
	}

	if (bmp->compression != 0) {
		return 2;
	}

	

	switch (bmp->bits_per_pixel) {
		case 1:
			break;

		case 4:
			break;

		case 8:
			return displaybmp_8bit(x, y, bmp);
		case 24:
			return displaybmp_32bit(x, y, bmp);
		case 32:
			return displaybmp_32bit(x, y, bmp);
		default:
			return 5;
			break;
	}
	return 0;
}

int displaybmp_8bit(int x, int y, bmp_header_t* bmp) {
	

	uint32_t i, j, color;
	uint32_t w, h;
	h = bmp->height;
	w = bmp->width;

	// while (w % 4) {
	// 	//Width not multiple of 4
	// 	w++;
	// }

	if (bmp->bits_per_pixel != 8) {
		return 8;
	}

	for ( i = h-1; i >= 0; i--) {

		for (j = 0; j < w; j++) {

			color = *(uint8_t*)( ((void*)bmp) + bmp->offset + (i*w)+j );

			vputpixelc(x+j, y+i, color);

		}

	}
	return 0;
}

int displaybmp_32bit(int x, int y, bmp_header_t* bmp) {
	
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

	logf("padding(%d)=%d", w, padding(w));

	for ( i = 0; i < h; i++) {

		for (j = 0; j < w; j++) {

			R = *pixel_ptr++;
			G = *pixel_ptr++;
			B = *pixel_ptr++;

			vputpixelc(x+j, y+(h-i), COLOR_RGB(R, G, B));

		}

		pixel_ptr += padding(w);

	}

	return 0;
}

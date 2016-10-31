#ifndef __SYS_BMP_H__
#define __SYS_BMP_H__

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


int displaybmp(int x, int y, bmp_header_t* bmp);

int displaybmp_8bit(int x, int y, bmp_header_t* bmp);
int displaybmp_32bit(int x, int y, bmp_header_t* bmp);

#endif
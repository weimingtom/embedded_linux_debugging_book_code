/*
 * $Id: fv.c
 * $Desp: draw jpeg to framebuffer
 * $Author: rockins
 * $Date: Wed Jan  3 20:15:49 CST 2007
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <jpeglib.h>
#include <jerror.h>

#define	FB_DEV	"/dev/fb0"

/***************** function declaration ******************/
void            usage(char *msg);
unsigned short  RGB888toRGB565(unsigned char red,
							   unsigned char green, unsigned char blue);
int             fb_open(char *fb_device);
int             fb_close(int fd);
int             fb_stat(int fd, int *width, int *height, int *depth);
void           *fb_mmap(int fd, unsigned int screensize);
int             fb_munmap(void *start, size_t length);
int             fb_pixel(void *fbmem, int width, int height,
						 int x, int y, unsigned short color);

/************ function implementation ********************/
int
main(int argc, char *argv[])
{
	/*
	 * declaration for jpeg decompression
	 */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE           *infile;
	unsigned char  *buffer;

	/*
	 * declaration for framebuffer device
	 */
	int             fbdev;
	char           *fb_device;
	unsigned char  *fbmem;
	unsigned int    screensize;
	unsigned int    fb_width;
	unsigned int    fb_height;
	unsigned int    fb_depth;
	unsigned int    x;
	unsigned int    y;

	/*
	 * check auguments
	 */
	if (argc != 2) {
		usage("insuffient auguments");
		exit(-1);
	}

	/*
	 * open framebuffer device
	 */
	if ((fb_device = getenv("FRAMEBUFFER")) == NULL)
		fb_device = FB_DEV;
	fbdev = fb_open(fb_device);

	/*
	 * get status of framebuffer device
	 */
	fb_stat(fbdev, &fb_width, &fb_height, &fb_depth);

	/*
	 * map framebuffer device to shared memory
	 */
	screensize = fb_width * fb_height * fb_depth / 8;
	fbmem = fb_mmap(fbdev, screensize);

	/*
	 * open input jpeg file 
	 */
	if ((infile = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "open %s failed\n", argv[1]);
		exit(-1);
	}

	/*
	 * init jpeg decompress object error handler
	 */
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	/*
	 * bind jpeg decompress object to infile
	 */
	jpeg_stdio_src(&cinfo, infile);


	/*
	 * read jpeg header
	 */
	jpeg_read_header(&cinfo, TRUE);

	/*
	 * decompress process.
	 * note: after jpeg_start_decompress() is called
	 * the dimension infomation will be known,
	 * so allocate memory buffer for scanline immediately
	 */
	jpeg_start_decompress(&cinfo);
	if ((cinfo.output_width > fb_width) ||
		(cinfo.output_height > fb_height)) {
		printf("too large JPEG file,cannot display\n");
		return (-1);
	}

	buffer = (unsigned char *) malloc(cinfo.output_width *
									  cinfo.output_components);
	y = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		if (fb_depth == 16) {
			unsigned short  color;
			for (x = 0; x < cinfo.output_width; x++) {
				color = RGB888toRGB565(buffer[x * 3], 
						buffer[x * 3 + 1], buffer[x * 3 + 2]);
				fb_pixel(fbmem, fb_width, fb_height, x, y, color);
			}
		} else if (fb_depth == 24) {
			memcpy((unsigned char *) fbmem + y * fb_width * 3,
				   buffer, cinfo.output_width * cinfo.output_components);
		}
		y++;					// next scanline
	}

	/*
	 * finish decompress, destroy decompress object
	 */
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	/*
	 * release memory buffer
	 */
	free(buffer);

	/*
	 * close jpeg inputing file
	 */
	fclose(infile);

	/*
	 * unmap framebuffer's shared memory
	 */
	fb_munmap(fbmem, screensize);

	/*
	 * close framebuffer device
	 */
	fb_close(fbdev);

	return (0);
}

void
usage(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	printf("Usage: fv some-jpeg-file.jpg\n");
}

/*
 * convert 24bit RGB888 to 16bit RGB565 color format
 */
unsigned short
RGB888toRGB565(unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned short  B = (blue >> 3) & 0x001F;
	unsigned short  G = ((green >> 3) << 5) & 0x07E0;
	unsigned short  R = ((red >> 3) << 11) & 0xF800;

	return (unsigned short) (R | G | B);
}

/*
 * open framebuffer device.
 * return positive file descriptor if success,
 * else return -1.
 */
int
fb_open(char *fb_device)
{
	int             fd;

	if ((fd = open(fb_device, O_RDWR)) < 0) {
		perror(__func__);
		return (-1);
	}
	return (fd);
}

/*
 * get framebuffer's width,height,and depth.
 * return 0 if success, else return -1.
 */
int
fb_stat(int fd, int *width, int *height, int *depth)
{
	struct fb_fix_screeninfo fb_finfo;
	struct fb_var_screeninfo fb_vinfo;

	if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo)) {
		perror(__func__);
		return (-1);
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_vinfo)) {
		perror(__func__);
		return (-1);
	}

	*width = fb_vinfo.xres;
	*height = fb_vinfo.yres;
	*depth = fb_vinfo.bits_per_pixel;

	return (0);
}

/*
 * map shared memory to framebuffer device.
 * return maped memory if success,
 * else return -1, as mmap dose.
 */
void *
fb_mmap(int fd, unsigned int screensize)
{
	void *	fbmem;

	if ((fbmem = mmap(0, screensize, PROT_READ | PROT_WRITE,
					  MAP_SHARED, fd, 0)) == MAP_FAILED) {
		perror(__func__);
		return (void *) (-1);
	}

	return (fbmem);
}

/*
 * unmap map memory for framebuffer device.
 */
int
fb_munmap(void *start, size_t length)
{
	return (munmap(start, length));
}

/*
 * close framebuffer device
 */
int
fb_close(int fd)
{
	return (close(fd));
}

/*
 * display a pixel on the framebuffer device.
 * fbmem is the starting memory of framebuffer,
 * width and height are dimension of framebuffer,
 * x and y are the coordinates to display,
 * color is the pixel's color value.
 * return 0 if success, otherwise return -1.
 */
int
fb_pixel(void *fbmem, int width, int height,
		 int x, int y, unsigned short color)
{
	if ((x > width) || (y > height))
		return (-1);

	unsigned short *dst = ((unsigned short *) fbmem + y * width + x);

	*dst = color;
	return (0);
}

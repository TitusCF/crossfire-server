/*
 * static char *rcsid_png_c =
 *   "$Id$";
 */
/*
    Crossfire client, a client program for the crossfire program.

    Copyright (C) 2000 Mark Wedel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to mwedel@scruz.net
*/

/* This is a light weight png -> xpixmap function.  Most of the code is from
 * the example png documentation.
 * I wrote this because I could not find a simple function that did this -
 * most all libraries out there tended to do a lot more than I needed for
 * crossfire - in addition, imLib actually has bugs which prevents it from
 * rendering some images properly.
 *
 * This function is far from complete, but does the job and removes the need
 * for yet another library.
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <png.h>
#include <X11/Xlib.h>


/* Defines for PNG return values */
/* These should be in a header file, but currently our calling functions
 * routines just check for nonzero return status and don't really care
 * why the load failed.
 */
#define PNGX_NOFILE	1
#define PNGX_OUTOFMEM	2
#define PNGX_DATA	3

static char *data_cp;
static int data_len, data_start;

void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
    memcpy(data, data_cp + data_start, length);
    data_start += length;
}

int png_to_xpixmap(Display *display, Drawable draw, char *data, int len,
		   Pixmap *pix, Pixmap *mask, Colormap cmap,
		   unsigned long *width, unsigned long *height)
{
    static char *pixels=NULL;
    static int pixels_byte=0, rows_byte=0;
    static png_bytepp	rows=NULL;

    png_structp	png_ptr=NULL;
    png_infop	info_ptr=NULL, end_info=NULL;
    png_colorp	palette;
    FILE    *fp;
    int bit_depth, color_type, interlace_type, compression_type, filter_type,
	red,green,blue, alpha,bpp, x,y, lred=-1, lgreen=-1,lblue=-1,
	has_alpha=0, num_palette;
    XColor  scolor;
    GC	gc, gc_alpha;

    data_len=len;
    data_cp = data;
    data_start=0;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				     NULL, NULL, NULL);
    if (!png_ptr) {
	return PNGX_OUTOFMEM;
    }
    info_ptr = png_create_info_struct (png_ptr);

    if (!info_ptr) {
	png_destroy_read_struct (&png_ptr, NULL, NULL);
	return PNGX_OUTOFMEM;
    }
    end_info = png_create_info_struct (png_ptr);
    if (!end_info) {
	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
	return PNGX_OUTOFMEM;
    }
    if (setjmp (png_ptr->jmpbuf)) {
	png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);
	fclose(fp);
	return PNGX_DATA;
    }

    png_set_read_fn(png_ptr, NULL, user_read_data);
    png_read_info (png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, width, height, &bit_depth,
		 &color_type, &interlace_type, &compression_type, &filter_type);

    if (color_type == PNG_COLOR_TYPE_PALETTE &&
            bit_depth <= 8) {

                /* Convert indexed images to RGB */
                png_set_expand (png_ptr);

    } else if (color_type == PNG_COLOR_TYPE_GRAY &&
                   bit_depth < 8) {

                /* Convert grayscale to RGB */
                png_set_expand (png_ptr);

    } else if (png_get_valid (png_ptr,
                                  info_ptr, PNG_INFO_tRNS)) {

                /* If we have transparency header, convert it to alpha
                   channel */
                png_set_expand(png_ptr);

    } else if (bit_depth < 8) {

                /* If we have < 8 scale it up to 8 */
                png_set_expand(png_ptr);


                /* Conceivably, png_set_packing() is a better idea;
                 * God only knows how libpng works
                 */
    }
        /* If we are 16-bit, convert to 8-bit */
    if (bit_depth == 16) {
                png_set_strip_16(png_ptr);
    }

        /* If gray scale, convert to RGB */
    if (color_type == PNG_COLOR_TYPE_GRAY ||
            color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
                png_set_gray_to_rgb(png_ptr);
    }

        /* If interlaced, handle that */
    if (interlace_type != PNG_INTERLACE_NONE) {
                png_set_interlace_handling(png_ptr);
    }

    /* Update the info the reflect our transformations */
    png_read_update_info(png_ptr, info_ptr);
    /* re-read due to transformations just made */
    png_get_IHDR(png_ptr, info_ptr, width, height, &bit_depth,
		 &color_type, &interlace_type, &compression_type, &filter_type);
    if (color_type & PNG_COLOR_MASK_ALPHA)
                bpp = 4;
    else
                bpp = 3;

    /* Allocate the memory we need once, and increase it if necessary.
     * This is more efficient the allocating this block of memory every time.
     */
    if (pixels_byte==0) {
	pixels = (char*)malloc(*width * *height * bpp);
	pixels_byte =*width * *height * bpp;
    } else if ((*width * *height * bpp) > pixels_byte) {
	realloc(pixels, *width * *height * bpp);
	pixels_byte =*width * *height * bpp;
    }

    if (!pixels) {
	pixels_byte=0;
	png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);
	return PNGX_OUTOFMEM;
    }
    if (rows_byte == 0) {
	rows =(png_bytepp) malloc(sizeof(char*) * *height);
	rows_byte=*height;
    } else if (*height > rows_byte) {
	rows =(png_bytepp) realloc(rows, sizeof(char*) * *height);
	rows_byte=*height;
    }
    if (!rows) {
	pixels_byte=0;
	png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);
	return PNGX_OUTOFMEM;
    }

    for (y=0; y<*height; y++) 
	rows[y] = pixels + y * *width * bpp;

    png_read_image(png_ptr, rows);
#if 0
    fprintf(stderr,"image is %d X %d, bpp=%d, color_type=%d\n",
	    *width, *height, bpp, color_type);
#endif

    *pix = XCreatePixmap(display, draw, *width, *height, 
			DefaultDepth(display,  DefaultScreen(display)));

    gc=XCreateGC(display, *pix, 0, NULL);
    XSetFunction(display, gc, GXcopy);
    XSetPlaneMask(display, gc, AllPlanes);

    if (color_type & PNG_COLOR_MASK_ALPHA) {
	/* The foreground/background colors are not really
	 * colors, but rather values to set in the mask.
	 * The values used below work properly on at least
	 * 8 bit and 16 bit display - using things like
	 * blackpixel & whitepixel does NO work on
	 * both types of display.
	 */
	*mask=XCreatePixmap(display ,draw, *width, *height,1);
	gc_alpha=XCreateGC(display, *mask, 0, NULL);
	XSetFunction(display, gc_alpha, GXcopy);
	XSetPlaneMask(display, gc_alpha, AllPlanes);
	XSetForeground(display, gc_alpha, 1);
	XFillRectangle(display, *mask, gc_alpha, 0, 0, *width, *height);
	scolor.pixel=1;
	XSetForeground(display, gc_alpha, 0);
	has_alpha=1;
    }
    else {
	*mask = None;
	gc_alpha = None;    /* Prevent compile warnings */
    }

    /* If the png image has a palette, we use the method of 
     * cycling through the colors and drawing those points as
     * we find them.  This is much faster than allocing the color
     * and setting the color on the GC (it translates into a 
     * startup time of a second or two vs 4 or 5 seconds if
     * we use the second method).  However, not all images have
     * palettes, so we need the backup method.
     */
    if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
	int i;

	for (i=0; i<num_palette; i++) {
	    scolor.red = (palette[i].red << 8) + palette[i].red;
	    scolor.green = (palette[i].green << 8) + palette[i].green;
	    scolor.blue = (palette[i].blue << 8) + palette[i].blue;
	    if (!XAllocColor(display, cmap, &scolor)) {
		fprintf(stderr,"Unable to allocate color %d %d %d\n",
			     palette[i].red, palette[i].green, palette[i].blue);
	    }
	    XSetForeground(display, gc, scolor.pixel);
	    for (y=0; y<*height; y++) {
		for (x=0; x<*width; x++) {
		    if (has_alpha) {
			alpha = rows[y][x*bpp+3];
			/* Transparent bit */
			if (alpha==0) {
				XDrawPoint(display, *mask, gc_alpha, x, y);
			}
		    }
		    if ((rows[y][x*bpp] == palette[i].red) &&
			(rows[y][x*bpp+1] == palette[i].green) &&
			(rows[y][x*bpp+2] == palette[i].blue)) 
			XDrawPoint(display, *pix, gc, x,y);
		}
	    }
	} /* for i loop */
    } else {
	/* this method cycles through the image once only, setting
	 * the color as we go along.  IT is pretty slow - probably
	 * using many gc's and cycling through them may be faster for
	 * the limited color images that crossfire uses (if you re-use
	 * this code for more photo realistic images, this may not be
	 * much different in performance).  We could get more clever by
	 * allocating a bunch of gc's and then searching through them
	 * for the colors so we don't need to do XAllocColor's and
	 * XSetForegrounds.
	 */
      for (y=0; y<*height; y++) {
	for (x=0; x<*width; x++) {
	    red=rows[y][x*bpp];
	    green=rows[y][x*bpp+1];
	    blue=rows[y][x*bpp+2];
	    if (has_alpha) {
		alpha = rows[y][x*bpp+3];
		/* Transparent bit */
		if (alpha==0) {
		    XDrawPoint(display, *mask, gc_alpha, x, y);
		}
	    }
	    /* Only go through the color of color setting if it is
	     * different than our last drawn color.
	     * Note we can not compare against the values in the scolor
	     * structure because XAllocColor can change the values
	     * depending on the screen type.
	     */
	    if ( ( red != lred) ||
		        ( green != lgreen) ||
		        ( blue != lblue)) {
		scolor.red = (red << 8) + red;
		scolor.green = (green << 8) + green;
		scolor.blue = (blue << 8) + blue;
		lred = red;
		lgreen=green;
		lblue=blue;
		if (!XAllocColor(display, cmap, &scolor)) {
		    fprintf(stderr,"Unable to allocate color %d %d %d\n",
			    red,green,blue);
		}
		XSetForeground(display, gc, scolor.pixel);
	    }
	    XDrawPoint(display, *pix, gc, x,y);
	}
      }
    }
    if (has_alpha)
	XFreeGC(display, gc_alpha);
    XFreeGC(display, gc);
    png_destroy_read_struct (&png_ptr, &info_ptr, &end_info);
    return 0;
}
#if 0
int main(int argc, char *argv[])
{
    Display *disp;
    char    data[256],buf[1024*1024];
    int	    fd,i,z, alpha;
    unsigned long  width, height;
    Window  window;
    XSetWindowAttributes    wattrs;
    Pixmap  pix, mask;
    GC	    gc;

    if (argc!=2) {
	fprintf(stderr,"Usage: %s <filename>\n", argv[0]);
	exit(1);
    }

    if (!(disp=XOpenDisplay(NULL))) {
	fprintf(stderr,"Unable to open display\n");
	exit(1);
    }

    wattrs.backing_store = WhenMapped;
    wattrs.background_pixel = WhitePixel(disp, DefaultScreen(disp));

    window=XCreateWindow(disp, DefaultRootWindow(disp), 0, 0,
	 32, 32, 0, CopyFromParent, InputOutput, CopyFromParent,
	CWBackingStore|CWBackPixel, &wattrs);

    i = open(argv[1], O_RDONLY);
    z=read(i, buf, 1024*1024);
    close(i);
    fprintf(stderr,"Read %d bytes from %s\n", z, argv[1]);
    png_to_xpixmap(disp, window, buf, z, &pix, &mask, DefaultColormap(disp,
				DefaultScreen(disp)), &width, &height);
    XResizeWindow(disp, window, width, height);
    if (!window) {
	fprintf(stderr,"Unable to create window\n");
	exit(1);
    }
    XMapWindow(disp, window);
    XFlush(disp);
    gc=XCreateGC(disp, pix, 0, NULL);
    if (mask)
	XSetClipMask(disp, gc, mask);

    /* A simple way to display the image without needing to worry
     * about exposures and redraws.
     */
    for (i=0; i<30; i++) {
	XCopyArea(disp, pix, window, gc, 0, 0, width, height, 0 , 0);
	XFlush(disp);
	sleep(1);
    }

    exit(0);
}


#endif

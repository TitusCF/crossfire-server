/*
 * static char *rcsid_png_c =
 *   "$Id$";
 */
/*
    Crossfire client, a client program for the crossfire program.

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team

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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
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
#include <X11/Xutil.h>


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
static XImage   *ximage;
static int rmask=0, bmask=0,gmask=0,need_color_alloc=0, rshift=16, bshift=0, gshift=8,
    rev_rshift=0, rev_gshift=0, rev_bshift=0;
static int colors_alloced=0, private_cmap=0, colormap_size;
struct Pngx_Color_Values {
    unsigned char   red, green, blue;
    long    pixel_value;
} *color_values;

#define COLOR_FACTOR       3
#define BRIGHTNESS_FACTOR  1

/* This function is used to find the pixel and return it 
 * to the caller.  We store what pixels we have already allocated
 * and try to find a match against that.  The reason for this is that
 * XAllocColor is a very slow routine. Before my optimizations,
 * png loading took about 140 seconds, of which 60 seconds of that
 * was in XAllocColor calls.
 */
static long pngx_find_color(Display *display, Colormap *cmap, int red, int green, int blue)
{

    int i, closeness=0xffffff, close_entry=-1, tmpclose;
    XColor  scolor;

    for (i=0; i<colors_alloced; i++) {
	if ((color_values[i].red == red) && (color_values[i].green == green) && 
	    (color_values[i].blue == blue)) return color_values[i].pixel_value;

	tmpclose = COLOR_FACTOR * (abs(red - color_values[i].red) +
				   abs(green - color_values[i].green) +
				   abs(blue - color_values[i].blue)) +
		    BRIGHTNESS_FACTOR * abs((red + green + blue) -
				(color_values[i].red + color_values[i].green + color_values[i].blue));

	/* I already know that 8 bit is not enough to hold all the PNG colors,
	 * so lets do some early optimization
	 */
	if (tmpclose < 3) return color_values[i].pixel_value;
	if (tmpclose < closeness) {
	    closeness = tmpclose;
	    close_entry = i;
	}
    }

    /* If the colormap is full, no reason to do anything more */
    if (colors_alloced == colormap_size) 
	return color_values[close_entry].pixel_value;


    /* If we get here, we haven't cached the color */

    scolor.red = (red << 8) + red;
    scolor.green = (green << 8) + green;
    scolor.blue = (blue << 8) + blue;


again:
    if (!XAllocColor(display, *cmap, &scolor)) {
	if (!private_cmap) {
	    fprintf(stderr,"Going to private colormap after %d allocs\n", colors_alloced);
	    *cmap = XCopyColormapAndFree(display, *cmap);
	    private_cmap=1;
	    goto again;
	}
	else {
#if 0
	    fprintf(stderr,"Unable to allocate color %d %d %d, %d colors alloced, will use closeness value %d\n",
		    red, green, blue, colors_alloced, closeness);
#endif
	    colors_alloced = colormap_size;	/* Colormap is exhausted */
	    return color_values[close_entry].pixel_value;
	}
    }
    color_values[colors_alloced].red = red;
    color_values[colors_alloced].green = green;
    color_values[colors_alloced].blue = blue;
    color_values[colors_alloced].pixel_value= scolor.pixel;
    colors_alloced++;
    return scolor.pixel;
}



static void user_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
    memcpy(data, data_cp + data_start, length);
    data_start += length;
}

int init_pngx_loader(Display *display)
{
    int pad,depth;
    XVisualInfo xvinfo, *xvret;
    Visual *visual;

    depth = DefaultDepth(display, DefaultScreen(display));
    visual = DefaultVisual(display, DefaultScreen(display));
    xvinfo.visualid = XVisualIDFromVisual(visual);
    xvret = XGetVisualInfo(display, VisualIDMask, &xvinfo, &pad);
    if (pad != 1) {
	fprintf(stderr,"XGetVisual found %d matching visuals?\n", pad);
	return 1;
    }
    rmask = xvret -> red_mask;
    gmask = xvret -> green_mask;
    bmask = xvret -> blue_mask;
    /* We need to figure out how many bits to shift.  Thats what this
     * following block of code does.  We can't presume to use just
     * 16, 8, 0 bits for RGB respectively, as if you are on 16 bit,
     * that is not correct.  There may be a much easier way to do this -
     * it is just bit manipulation.  Note that we want to preserver
     * the most significant bits, so these shift values can very
     * well be negative, in which case we need to know that -
     * the shift operators don't work with negative values.
     * An example is 5 bits for blue - in that case, we really
     * want to shfit right (>>) by 3 bits.
     */
    rshift=0;
    if (rmask) {
	while (!((1 << rshift) & rmask)) rshift++;
	while (((1 << rshift) & rmask)) rshift++;
	rshift -= 8;
	if (rshift < 0 ) {
	    rev_rshift=1;
	    rshift = -rshift;
	}
    }
    gshift=0;
    if (gmask) {
	while (!((1 << gshift) & gmask)) gshift++;
	while (((1 << gshift) & gmask)) gshift++;
	gshift -= 8;
	if (gshift < 0 ) {
	    rev_gshift=1;
	    gshift = -gshift;
	}
    }
    bshift=0;
    if (bmask) {
	while (!((1 << bshift) & bmask)) bshift++;
	while (((1 << bshift) & bmask)) bshift++;
	bshift -= 8;
	if (bshift < 0 ) {
	    rev_bshift=1;
	    bshift = -bshift;
	}
    }
    
	
    if (xvret->class==PseudoColor) {
	need_color_alloc=1;
	if (xvret->colormap_size>256) {
	    fprintf(stderr,"One a pseudocolor visual, but colormap has %d entries?\n", xvret->colormap_size);
	}
	color_values=malloc(sizeof(struct Pngx_Color_Values) * xvret->colormap_size);
	colormap_size = xvret->colormap_size-1;	/* comparing # of alloced colors against this */
    }
    XFree(xvret);

    if (depth>16) pad = 32;
    else if (depth > 8) pad = 16;
    else pad = 8;

    ximage = XCreateImage(display, visual,
		      depth,
		      ZPixmap, 0, 0, 
		      256, 256,  pad, 0);
    if (!ximage) {
	fprintf(stderr,"Failed to create Ximage\n");
	return 1;
    }
    ximage->data = malloc(ximage->bytes_per_line * 256);
    if (!ximage->data) {
	fprintf(stderr,"Failed to create Ximage data\n");
	return 1;
    }
    return 0;
}


int png_to_xpixmap(Display *display, Drawable draw, char *data, int len,
		   Pixmap *pix, Pixmap *mask, Colormap *cmap,
		   unsigned long *width, unsigned long *height)
{
    static char *pixels=NULL;
    static int pixels_byte=0, rows_byte=0;
    static png_bytepp	rows=NULL;
    
    png_structp	png_ptr=NULL;
    png_infop	info_ptr=NULL, end_info=NULL;
    int bit_depth, color_type, interlace_type, compression_type, filter_type,
	red,green,blue, lastred=-1, lastgreen=-1, lastblue=-1,alpha,bpp, x,y,
	has_alpha=0,cmask, lastcmask=-1, lastcolor=-1;
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
	pixels=realloc(pixels, *width * *height * bpp);
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
	XSetForeground(display, gc_alpha, 0);
	has_alpha=1;
    }
    else {
	*mask = None;
	gc_alpha = None;    /* Prevent compile warnings */
    }

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
	    if (need_color_alloc) {
		/* We only use cmask to avoid calling pngx_find_color repeatedly.
		 * when the color has not changed from the last pixel.
		 */
		if ((lastred != red) && (lastgreen != green) && (lastblue != blue)) {
		    lastcolor = pngx_find_color(display, cmap, red, green, blue);
		    lastcmask = cmask;
		}
		XPutPixel(ximage, x, y, lastcolor);
	    } else {
		if ((lastred != red) && (lastgreen != green) && (lastblue != blue)) {
		    if (rev_rshift) red >>= rshift;
		    else red <<= rshift;
		    if (rev_gshift) green >>= gshift;
		    else green <<= gshift;
		    if (rev_bshift) blue >>= bshift;
		    else blue <<= bshift;

		    cmask = (red & rmask) | (green  & gmask) | (blue  & bmask);
		}
		XPutPixel(ximage, x, y, cmask);
	    }
	}
    }

    XPutImage(display, *pix, gc, ximage, 0, 0, 0, 0, *width, *height);
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

/*
 * static char *rcsid_xutil_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

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


#include <autoconf.h>
/* Stupid pngconf.h file has a check to see if _SETJMP_H is
 * already defined, and if so, it generates a compilation error.
 * I have no idea what they were thinking - wasn't the point of those
 * defines so that you didn't need to worry about order or including
 * the same file multiple times?
 */
#ifdef HAVE_LIBPNG
#include "png.c"
#endif

#include <Defines.h>
#include <loader.h>

#define IMAGE_BUF 65536

/*
 * ReadPixmaps(): When color pixmaps are used instead of fonts, this function
 * does the actual reading of pixmap-file.  This function is based largely on
 * the ReadBitmaps function.
 */

/* New method: Pixmaps are stored as a montage on the disk (in several
 * files).  This way, we only need to call XCreatePixmap... a couple
 * times, and the rest are XCopyArea.  This is much faster, since
 * the XPM library does not seem to be especially efficient about loading
 * large numbers of pixmaps.
 *
 * Return true if we have gone to a private colormap.
 *
 * type is type of images to load.  If Dm_Bitmaps, cmap, and masks
 * can be null.
 */

/* Useful when trying to optimize load time some */
#define IMAGE_TIME_LOAD

int ReadImages(Display *gdisp, Pixmap **pixmaps, Pixmap **masks,
    Colormap *cmap, enum DisplayMode type) {

    Window	root = RootWindow (gdisp,DefaultScreen(gdisp));
    int		use_private_cmap=0,num, compressed, len,i;
    FILE	*infile;
    char	*cp, databuf[IMAGE_BUF], filename[MAX_BUF];
#ifdef IMAGE_TIME_LOAD
    time_t	start_time = time(NULL);
#endif

    /* This function is called before the game gc's are created.  So
     * we create one for our own use here.
     */  
    GC	gc= XCreateGC(gdisp, root, 0, NULL);

    if (!nrofpixmaps)
        read_bmap_names();

    /* The processes that create a pixmap already allocates the space for
     * the pixmap data.  Therefor, only space for the pointers to that data
     * needs to be allocated.  The same might apply for the function
     * that creates bitmaps below, but I am not as sure in that case.
     * Mark Wedel 
     */

    *pixmaps = (Pixmap *) malloc(sizeof(Pixmap *) * nrofpixmaps);
    if (type==Dm_Png)
	*masks = (Pixmap *) malloc(sizeof(Pixmap *) * nrofpixmaps);

    for (i=0; i < nrofpixmaps; i++)
      (*pixmaps)[i] = 0;

    LOG(llevDebug,"Building images...");

    if (type==Dm_Png) {
	sprintf(filename,"%s/crossfire.0",settings.datadir);
#ifdef HAVE_LIBPNG
	init_pngx_loader(gdisp);
#endif
    }

    if ((infile = open_and_uncompress(filename,0,&compressed))==NULL) {
        LOG(llevError,"Unable to open %s\n", filename);
        abort();
    }
    i=0;
    while (fgets(databuf,MAX_BUF,infile)) {

	/* First, verify that that image header line is OK */
        if(strncmp(databuf,"IMAGE ",6)!=0) {
	    LOG(llevError,"ReadImages:Bad image line - not IMAGE, instead\n%s",databuf);
            abort();
	}
        num = atoi(databuf+6);
        if (num<0 || num > nrofpixmaps) {
            LOG(llevError,"Pixmap number less than zero: %d, %s\n",num, databuf);
            abort();
	}
	/* Skip accross the number data */
	for (cp=databuf+6; *cp!=' '; cp++) ;
	len = atoi(cp);
	if (len==0 || len>IMAGE_BUF) {
	    LOG(llevError,"ReadImages: length not valid: %d\n%s",
                    len,databuf);
                abort();
	}
	if (fread(databuf, 1, len, infile)!=len) {
           LOG(llevError,"read_client_images: Did not read desired amount of data, wanted %d\n%s",
                    len, databuf);
                    abort();
	}
	i++;
	if (!(i % 100)) LOG(llevDebug,".");

	if (type==Dm_Png) {
#ifdef HAVE_LIBPNG
	    unsigned long  x,y;
	    if (png_to_xpixmap(gdisp, root, databuf, len, 
			   &(*pixmaps)[num], &(*masks)[num], cmap, &x, &y)) {

			LOG(llevError,"Error loading png file.\n");
	    }
#endif
	}
    }
    close_and_delete(infile, compressed);
    XFreeGC(gdisp, gc);

    /* Check for any holes.  There should not be.  This is an interim check
     * to use until the xbm_values is removed.  The program that 
     * creates the bitmap/pixmap files just adds the bitmaps, so holes should
     * not be in the image file.
     */
    for (i = 0; i < nrofpixmaps; i++)
	if ((*pixmaps)[i] == 0) {
	    LOG(llevDebug, "Warning, pixmap %d is not defined, setting it to blank\n", i);
	    (*pixmaps)[i] = (*pixmaps)[blank_face->number];
	}
    LOG(llevDebug,"done.\n");
#ifdef IMAGE_TIME_LOAD
    fprintf(stderr,"Creation of images took %ld seconds\n", time(NULL) - start_time);
#endif
    return use_private_cmap;
}

/* This frees all the pixmaps.  This not only makes for better code,
 * some XServers may not free the memory used by the pixmaps unless it is done
 * this way.  For color pixmaps, this will be called twice - the
 * first time, the normal pixmaps will be passed through, the second
 * time, it will be called with the masks as the pixmaps argument.
 */

void free_pixmaps(Display *gdisp, Pixmap *pixmaps)
{
    int i;

    for (i=0; i < nrofpixmaps; i++) {
	if (pixmaps[i]!=0) {
	    XFreePixmap(gdisp, pixmaps[i]);
	    pixmaps[i] = 0;
	}
    }
}


/*
 * static char *rcsid_xutil_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to master@rahul.net
*/

#include <global.h>
#include <funcpoint.h>
#include <loader.h>

#include <Defines.h>

#ifdef HAVE_LIBXPM
#include <X11/xpm.h>
#endif


/*
 * Converts between Fontindex and XChar2b types.
 * Used in global.h, for draw_face (in face, FontindextoXChars 
 * is defined to this function name
 */

XChar2b fontindex_to_XChar2b(Fontindex s)
{
  XChar2b c;

  c.byte1 = s/256;
  c.byte2 = s%256;

  return c;
}


/*
 * ReadPixmaps(): When color pixmaps are used instead of fonts, this function
 * does the actual reading of pixmap-file.  This function is based largely on
 * the ReadBitmaps function.  By Mark Wedel (master@rahul.net)
 */

/* New method: Pixmaps are stored as a montage on the disk (in several
 * files).  This way, we only need to call XCreatePixmap... a couple
 * times, and the rest are XCopyArea.  This is much faster, since
 * the XPM library does not seem to be especially efficient about loading
 * large numbers of pixmaps.
 *
 * Return true if we have gone to a private colormap.
 */

int ReadPixmaps(Display *gdisp, Pixmap **pixmaps, Pixmap **masks,
    Colormap *cmap) {
#ifdef HAVE_LIBXPM

    Window	root = RootWindow (gdisp,DefaultScreen(gdisp));
    XpmAttributes xpmatribs;
    int		use_private_cmap=0,num, compressed, len,i, error;
    FILE	*infile;
    char	buf[MAX_BUF], *cur, databuf[HUGE_BUF], filename[MAX_BUF];

    /* This function is called before the game gc's are created.  So
     * we create one for our own use here.
     */  
    GC	gc= XCreateGC(gdisp, root, 0, NULL);
    if (*cmap) {
	xpmatribs.valuemask = XpmColormap;
	xpmatribs.colormap=*cmap;
    }
    else  xpmatribs.valuemask=0;

    if (!nrofpixmaps)
	nrofpixmaps = ReadBmapNames ();

    /* The processes that create a pixmap already allocates the space for
     * the pixmap data.  Therefor, only space for the pointers to that data
     * needs to be allocated.  The same might apply for the function
     * that creates bitmaps below, but I am not as sure in that case.
     * Mark Wedel (master@rahul.net)
     */

    *pixmaps = (Pixmap *) malloc(sizeof(Pixmap *) * nrofpixmaps);
    *masks = (Pixmap *) malloc(sizeof(Pixmap *) * nrofpixmaps);
    for (i=0; i < nrofpixmaps; i++)
      (*pixmaps)[i] = 0;

    LOG(llevDebug,"Building color pixmaps...");
    sprintf(filename,"%s/crossfire.xpm",settings.datadir);
    if ((infile = open_and_uncompress(filename,0,&compressed))==NULL) {
        LOG(llevError,"Unable to open %s\n", filename);
        abort();
    }
    i=0;
    while(1) {
        if (!fgets(filename,MAX_BUF,infile)) break;

	/* First, verify that that image header line is OK */
        if(strncmp(filename,"ESRV_XPM ",9)!=0 || filename[14] != ' ') {
            fprintf(stderr,"whoa, bad esrv_xpm line; not ESRV_XPM ...\n%s",filename);
            abort();
	}
        num = atoi(filename+9);
        if (num<0 || num > nrofpixmaps) {
            LOG(llevError,"Pixmap number less than zero: %d, %s\n",num, filename);
            abort();
	}
	cur = databuf;
	/* Now read in all the image data */
	while(1) {
            fgets(buf,500,infile);
            if (*buf == '\0') {
                fprintf(stderr,"whoa, pixmap #%d not terminated??\n",num);
                abort();
	    }
            if (strcmp(buf,"ESRV_XPM_END\n")==0)
                break;
            len = strlen(buf);
            if (cur+len > databuf+HUGE_BUF) {
                LOG(llevError,"Overflow of MAX_BUF in read_client_images, image %d\n", num);
                abort();
	    }
            strcpy(cur,buf);
            cur += len;
	}
	i++;
	if (!(i % 100)) LOG(llevDebug,".");
again:	error=XpmCreatePixmapFromBuffer(gdisp, root, databuf,
	      &(*pixmaps)[num], &(*masks)[num], &xpmatribs);
	if (error!=XpmSuccess) {
		if (error==XpmColorFailed && !use_private_cmap) {
			LOG(llevError,"XPM could not allocate colors - trying to switch to private colormap..");
			if (!(*cmap=XCopyColormapAndFree(gdisp, *cmap)))
			    LOG(llevError,"Failed...\n");
			else
			    LOG(llevError,"Successful...\n");
			xpmatribs.colormap=*cmap;
			use_private_cmap=1;
			goto again;
		}
		LOG(llevError,"Error creating pixmap %s, error %d.\n",filename, error);
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
	    pixmaps[i] = pixmaps[blank_face->number];
	}
    LOG(llevDebug,"done.\n");
    return use_private_cmap;
#else
    return 0;	/* Prevents some warning messages */
#endif
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

/*
 * ReadBitmaps(): When bitmaps are used instead of fonts, this function
 * does the actual reading of bitmap-file, and returns the
 * bitmaps array.  It assumes the bitmap file found in the LIBDIR 
 * directory.
 */
  
  
Pixmap *ReadBitmaps(Display *d) {
    char buf[MAX_BUF];
    FILE *fp;
    int i, count = 0,comp;
    Pixmap *pixmaps;
  
    if (!nrofpixmaps)
	nrofpixmaps = ReadBmapNames ();

    pixmaps = (Pixmap *) malloc(sizeof(Pixmap) * nrofpixmaps);
    for (i=0; i < nrofpixmaps; i++)
	pixmaps[i] = 0;
  
    sprintf (buf,"%s/crossfire.cfb",settings.datadir);
    if ((fp = open_and_uncompress(buf,0,&comp))==NULL) {
	perror("Can't open crossfire.cfb file");
	exit(-1);
      }
  
    LOG(llevDebug,"Building ximages...");
    for (i=0; i<nroffiles; i++) {
	if(pixmaps[i] != 0) {
	    LOG(llevError,"Warning: two entries in bmaps: %d\n",i);
	    continue;
	}

	if (fread (buf, 24 * 3, 1, fp) != 1) {
	    LOG(llevError,"Warning: cannot read from file\n");
	    break;
	}

	pixmaps[i] =  XCreateBitmapFromData
	    (d, RootWindow (d, DefaultScreen(d)), buf, 24, 24);

	if (!pixmaps[i]) {
	    LOG(llevError,"Warning: Cannot create Pixmap %d\n",i);
	    pixmaps[i] = 0;
	}

	if(++count > CHECK_ACTIVE_MAPS/10) {
	    printf (".");
	    fflush (stdout);
	    count = 0;
	    (*process_active_maps_func)();
	}
    }
    close_and_delete(fp, comp);

      LOG(llevDebug,"done\n");

    /*
     * Now fill out the unused holes with pointers to a blank pixmap
     * to avoid crashes in case trying to draw a nonexistant pixmap.
     */
    for (i = 0; i < nrofpixmaps; i++)
	if (pixmaps[i] == 0)
	    pixmaps[i] = pixmaps[blank_face->number];
    return pixmaps;
}


/*
 * allocate_colors() tries to get enough colors for the game-window.
 * If it fails, it tries to use a private colormap.
 * If that also fails, it return 1, indicating to the calling
 * function that black and white should be used.
 *
 * Function arguments became a bit more complicated in v0.91.5.
 * The functionality is the same, but instead of passing the player
 * structure, just pass the values that are needed.  This enables
 * the new client to use the function also.
 *
 * Original function by Tyler Van Gorder (tvangod@icst.csuchico.edu),
 * June 1, 1992
 */

int allocate_colors(Display *disp, Window w, long screen_num,
	Colormap *colormap, XColor discolor[NUM_COLORS])
{
  int i, tried = 0, depth=0, iscolor;
  Status status;
  Visual *vis;
  XColor exactcolor;

  iscolor = 1;
  vis = DefaultVisual(disp,screen_num);
  if (vis->class >= StaticColor) {
    *colormap = DefaultColormap(disp,screen_num);
    depth = DefaultDepth(disp,screen_num);
  }
  else {
    *colormap=(Colormap )NULL;
    LOG(llevError,"Switching to black and white.\n");
    LOG(llevError,"You have a black and white terminal.\n");
    return 0;
  }
try_private:
  if (depth > 3 && iscolor) {
    unsigned long pixels[13];
    for (i=0; i<13; i++){
      status = XLookupColor(disp,*colormap, colorname[i][1],&exactcolor,
                            &discolor[i]);
      if (!status){
        LOG(llevError,"Can't find colour %s.\n", colorname[i]);
        LOG(llevError,"Switching to black and white.\n");
        iscolor = 0;
        break;
      }
      status = XAllocColor(disp,*colormap,&discolor[i]);
      if (!status) {
        if (!tried) {
          LOG(llevError, "Not enough colours. Trying a private colourmap.\n");
          XFreeColors(disp, *colormap, pixels, i-1, 0);
          *colormap = XCreateColormap(disp, w, vis, AllocNone);
          XSetWindowColormap(disp, w, *colormap);
          tried = 1;
          goto try_private;
        } else {
          LOG(llevError, "Failed. Switching to black and white.\n");
          iscolor = 0;
          break;
        }
      }
      pixels[i] = discolor[i].pixel;
    }
  }
  return iscolor;
}

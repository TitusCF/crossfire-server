#include <Defines.h>
#include <global.h>
#include <debug.h>
#include <X11.h>
#include <CrUtil.h>


extern    Pixmap *pixmaps;       /* list of pixmaps */
extern    Pixmap *masks;       /* list of pixmaps */
extern XChar2b fontindex_to_XChar2b ( Fontindex s );

/*
 *Function: FaceDraw
 *Member-Of: Cr
 *Description:
 * Draw face into position.
 *Commentary:
 * There are inplicit use of use_pixmaps and color_pix, that should
 * be removed by better structuring of program.
 */
void FaceDraw (Widget w, GC gc, New_Face *face, int x, int y) {
    XChar xbuf;

    if(displaymode==Dm_Pixmap || displaymode==Dm_Png) {
	XSetClipMask(XtDisplay (w), gc, masks[face->number]);
	XSetClipOrigin(XtDisplay (w), gc, x, y);
	XCopyArea(XtDisplay (w), pixmaps[face->number],
		  XtWindow(w), gc, 0, 0, FontSize, FontSize, x, y);
    } else {
	if (HAS_COLOUR(w)) {
	    XSetForeground (XtDisplay (w), gc, discolor[face->fg].pixel);
	    XSetBackground (XtDisplay (w), gc, discolor[face->bg].pixel);
	}
	else {
	    XSetForeground (XtDisplay (w), gc, BlackPixel(XtDisplay(w),	
		XScreenNumberOfScreen(XtScreen(w))));
	    XSetBackground (XtDisplay (w), gc, WhitePixel(XtDisplay(w),
		XScreenNumberOfScreen(XtScreen(w))));
	}
	if (displaymode==Dm_Bitmap) {
	    XCopyPlane(XtDisplay (w), pixmaps[face->number], 
		       XtWindow(w), gc, 0, 0, FontSize, FontSize, x, y, 1);
	} else {
	    xbuf = FontindexToXChar(face->number);
	    XDRAWIMAGESTRING (XtDisplay (w),XtWindow(w), gc, 
			      x, y + 24, &xbuf, 1);

	}
    }
}

/*
 *Function: GCCreate
 *Member-Of: Cr
 *Description:
 * Create suitable GC for Cr widgets.
 *Commentary:
 * There are inplicit use of use_pixmaps and color_pix, that should
 * be removed by better structuring of program.
 */
GC GCCreate (Widget w) {
    XGCValues values;
    GC gc;
    if (displaymode!=Dm_Font)
	gc= XtAllocateGC (w, 0, 0L, NULL, GCBackground | GCForeground, 0);
    else {
	values.font = XLoadFont (XtDisplay(w),FONTNAME);
	gc= XtAllocateGC 
		(w, 0, GCFont, &values, GCBackground | GCForeground, 0);
    }
    XSetGraphicsExposures(XtDisplay(w), gc, False);
   return gc;
}



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

    if(displaymode==Dm_Png) {
	XSetClipMask(XtDisplay (w), gc, masks[face->number]);
	XSetClipOrigin(XtDisplay (w), gc, x, y);
	XCopyArea(XtDisplay (w), pixmaps[face->number],
		  XtWindow(w), gc, 0, 0, FontSize, FontSize, x, y);
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
    GC gc;
    gc= XtAllocateGC (w, 0, 0L, NULL, GCBackground | GCForeground, 0);
    XSetGraphicsExposures(XtDisplay(w), gc, False);
   return gc;
}



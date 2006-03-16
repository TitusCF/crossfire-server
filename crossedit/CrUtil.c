#include <Defines.h>
#include <global.h>
#include <debug.h>
#include <X11.h>
#include <CrUtil.h>


extern    Pixmap *pixmaps;       /* list of pixmaps */
extern    Pixmap *masks;       /* list of pixmaps */



/* Draw one of the tiles in a (potentially big) face.
 *
 * w, gc - Where you want to paint the face, um.
 * x, y - The pixel co-ordinates of w where you want the face to appear.
 *
 * x_offset, y_offset:
 *
 *   Which part of the big face you want to draw; how many tiles down and
 *   right from the head you want to offset into the face.
 *
 *   If you want to draw the bottom-right corner of a 2 wide, 3 tall face,
 *   provide x_offset y_offset 1, 2.
 *
 *   If you want the top-left corner or are drawing a single-tile face,
 *   give 0, 0.
 */
void DrawFacePart (
    Widget w, GC gc, New_Face * face, int x, int y,
    int x_offset, int y_offset
) {
    int num = face->number;

    if (displaymode == Dm_Png) {
        XSetClipMask(XtDisplay(w), gc, masks[num]);
        /* On the other hand, x and y will be bigger, so the mask will be in
           the right place. */
        XSetClipOrigin(XtDisplay(w), gc,
            x - FontSize * x_offset,
            y - FontSize * y_offset);
        XCopyArea(XtDisplay(w), pixmaps[num], XtWindow(w), gc,
            FontSize * x_offset, FontSize * y_offset,
            FontSize, FontSize,
            x, y);
    }
}

/* Draws one tile of op on w, gc at pixel (x, y).
 *
 * This does the Right Thing with multi-tile objects: It paints the
 * particular part of the face when op is not the head, even for a big face.
 *
 * When op has a big face, DrawPartObject() calculates the offset to the
 * head to draw the right part of the big face for op.
 */
void DrawPartObject(Widget w, GC gc, object * op, int x, int y) {

    int x_offset = 0, y_offset = 0;

    if (op->head != NULL) {
        if (op->face == op->head->face) {
            /* We're not the head, but we've got the same face. Therefore,
               we've got a big face; get the offset in tiles. */
            x_offset = op->x - op->head->x;
            y_offset = op->y - op->head->y;
        }
    }

    DrawFacePart(w, gc,
        op->face,
        x, y,
        x_offset, y_offset);
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



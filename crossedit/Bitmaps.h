#ifndef _Bitmaps_h
#define _Bitmaps_h
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct _Bitmaps {
	Pixmap	edit;
	Pixmap	mark;
	Pixmap	notify;
} Bitmaps;

extern Bitmaps bitmaps;
extern void BitmapsCreate(Display *d);
#endif /* _Bitmaps_h */

#include "Posix.h"
#include "Bitmaps.h"
#include "./bitmaps/edit.bm"
#include "./bitmaps/mark.bm"
#include "./bitmaps/notify.bm"

Bitmaps bitmaps;

void BitmapsCreate(Display *d)
{
bitmaps.edit = XCreateBitmapFromData
	(d,DefaultRootWindow(d),
	(char*)edit_bits,
	edit_width,
	edit_height);
bitmaps.mark = XCreateBitmapFromData
	(d,DefaultRootWindow(d),
	(char*)mark_bits,
	mark_width,
	mark_height);
bitmaps.notify = XCreateBitmapFromData
	(d,DefaultRootWindow(d),
	(char*)notify_bits,
	notify_width,
	notify_height);
}



#ifndef _Defines_h_
#define _Defines_h_

#include <Posix.h>
#include <X11/Intrinsic.h>
#include <global.h>

#ifndef CrossEdit
#define CrossEdit
#endif

/*** kludge ***/
#ifdef FONTSIZE 
#undef FONTSIZE
#endif
#define FONTSIZE 24

#define WIZARD_MODE

#define STACK_SIZE 40
#define STACK_MIN 4
#define STACK_MAX 12
#define MAPLIST_SIZE 10
#define STYPELIST_SIZE 10
#define FILENAME_SIZE  PATH_MAX

#ifndef NeXT
#define EXIT_FAILURE 1
#endif

/*
 * Map size limits
 */
#define MapMinWidth  1
#define MapMinHeight 1
#define MapMaxWidth  100
#define MapMaxHeight 100

#define AppClass "Crossedit"

typedef struct _Edit *Edit;
typedef struct _App *App;
typedef struct _Attr *Attr;

typedef struct mapdef *Map;

extern enum DisplayMode displaymode;

/*typedef struct obj *Object;*/

/* temporary kludge */

extern object *MapGetObjectZ (Map, int, int, int);
extern object *MapGetRealObject (Map, int, int, int);
#if 0
extern void MapInsertObjectZ(Map,object *, int, int, int);
#endif
extern int ObjectGetForeground(object *);
extern int ObjectGetBackground(object *);
extern void  InitializeColors (Display *dpy);
extern object *ObjectCreateArch (archetype *);

extern XColor discolor[];

void CrEditSelect(Widget w,XRectangle rect);

#define XDRAWIMAGESTRING(disp,win,gc,x,y,str,len) \
XDrawImageString16(disp,win,gc,x,y,str,len)
#define FontindexToXChar(s) fontindex_to_XChar2b(s)
#define XCharToFontindex(s) ((Fontindex) ((s).byte1*256 +  (s).byte2))

#ifndef XChar2b
typedef XChar2b XChar; 
#endif

/*
 * Made this a macro since it's mostly used in time-critical areas:
 * (Hopefully a good optimizer will be able to move the if() out of
 * any loop where this macro is used.  That wouldn't have been possible
 * if this was a function!)
 */

/*
 * Have two version of this macro.  One if Xpm_Pix is defined, another
 * if Xpm_Pix is not defined.  IT would be better to actually have the
 * #ifdef's inside the macro itself, instead of having two versions, but
 * I was not able to get that to work.
 * Mark Wedel (mark@pyramid.com)
 */

#ifdef HAVE_LIBXPM
#define draw_face(pl,win,gc,x,y,face) \
{ \
  if (pl->color_pixmaps) \
  { \
    XSetClipMask(pl->gdisp, gc, pl->masks[(face)]); \
    XSetClipOrigin(pl->gdisp, gc, x, (y) - 24); \
    XCopyArea(pl->gdisp, pl->pixmaps[(face)], win, gc, 0, 0, 24, 24, \
	(unsigned int) (x), (unsigned int) ((y) - 24)); \
  } \
  else if(pl->use_pixmaps) \
  { \
    XCopyPlane(pl->gdisp,pl->pixmaps[(face)],win,gc,0,0,24,24, \
               (unsigned int) (x),(unsigned int) ((y) - 24),1); \
  } \
  else \
  { \
    XChar buf; \
    buf = FontindexToXChar((Fontindex) (face)); \
    XDRAWIMAGESTRING(pl->gdisp,win,gc,x,y,&buf,1); \
  } \
}
#else
#define draw_face(pl,win,gc,x,y,face) \
{ \
  if(pl->use_pixmaps) \
  { \
    XCopyPlane(pl->gdisp,pl->pixmaps[(face)],win,gc,0,0,24,24, \
               (unsigned int) (x),(unsigned int) ((y) - 24),1); \
  } \
  else \
  { \
    XChar buf; \
    buf = FontindexToXChar((Fontindex) (face)); \
    XDRAWIMAGESTRING(pl->gdisp,win,gc,x,y,&buf,1); \
  } \
}
#endif
/* Useful macro */
#define HAS_COLOUR(widg) (PlanesOfScreen(XtScreen(widg)) > 1)

#endif


/*** end of Defines.h ***/


#ifndef _CrEditP_h
#define _CrEditP_h

#include <CrEdit.h>
#include <X11.h>

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

/**********************************************************************
 * class                                                              *
 **********************************************************************/

typedef struct {
  int empty;
} CrEditClassPart;

typedef struct _CrEditClassRec {
  CoreClassPart core_class;
  CrEditClassPart crEdit_class;
} CrEditClassRec;

extern CrEditClassRec crEditClassRec;

/**********************************************************************
 * instance                                                           *
 **********************************************************************/

typedef struct {
    GC gc;          /* */
    Map map;        /* */
    int stacking;   /* */
    int fontSize;   /* */
    int show_weak_walls; /* */
    Boolean selectArea; /* */
    XSegment seg;   /* */

    XtCallbackList insertCallbacks;
    XtCallbackList selectCallbacks;
    XtCallbackList propsCallbacks;
    XtCallbackList deleteCallbacks;
    XtCallbackList alignCallbacks;
    XtCallbackList feedCallbacks;
} CrEditPart;

typedef struct _CrEditRec {
  CorePart core;
  CrEditPart crEdit;
} CrEditRec;

#endif /* _CrEdit_h */




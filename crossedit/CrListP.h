#ifndef _CrListP_h
#define _CrListP_h

#include <CrList.h>

#include <X11/CoreP.h>

/**********************************************************************
 * class                                                              *
 **********************************************************************/

typedef struct {
    int empty;
} CrListClassPart;

typedef struct _CrListClassRec {
    CoreClassPart core_class;
    CrListClassPart crList_class;
} CrListClassRec;

extern CrListClassRec crListClassRec;

/**********************************************************************
 * instance                                                           *
 **********************************************************************/

typedef struct {
    GC gc;
    XFontStruct *font;
    GC          normal_GC;
    Pixel       foreground;
    /* Boolean adjust; */
    XtCallbackList selectCallbacks; /* called when selecting */
    XtCallbackList insertCallbacks; /* called when inserting */
    XtCallbackList deleteCallbacks; /* called when  */
    
    CrListNode (*next)(XtPointer, XtPointer); /* called when  */
    void *element;
    XtPointer *caller;
} CrListPart;

typedef struct _CrListRec {
    CorePart core;
    CrListPart crList;
} CrListRec;

#endif /* _CrList_h */




#ifndef _CrEdit_h
#define _CrEdit_h

/**********************************************************************
 CrEdit resources:
 
 Name         Class        RepType    DefaultValue       Notice
 ----------------------------------------------------------------------
 object       Object       Pointer    NULL               for program

 **********************************************************************/

typedef struct _CrEditClassRec *CrEditWidgetClass;
typedef struct _CrEditRec      *CrEditWidget;

#define XtNstacking "stacking"
#define XtCStacking "Stacking"

#define XtNshow_weak_walls "show_weak_walls"
#define XtCShow_weak_walls "Show_weak_walls"

#define XtNselectArea "selectArea"
#define XtCSelectArea "SelectArea"

#define XtNmap "map"
#define XtCMap "Map"

#define XtNselectCallback "selectCallback"
#define XtCSelectCallback "SelectCallback"

#define XtNpropsCallback "propsCallback"
#define XtCPropsCallback "PropsCallback"

#define XtNinsertCallback "insertCallback"
#define XtCInsertCallback "InsertCallback"

#define XtNdeleteCallback "deleteCallback"
#define XtCDeleteCallback "DeleteCallback"

#define XtNalignCallback "alignCallback"
#define XtCAlignCallback "AlignCallback"

#define XtNfeedCallback "feedCallback"
#define XtCFeedCallback "FeedCallback"

extern WidgetClass crEditWidgetClass;

#include <Defines.h>

typedef struct CrEditCall {
    XRectangle rect;
    int z;
    Map map;
} *CrEditCall;

/*
 * interface
 */
extern void CrEditRefresh ( Widget w, XRectangle rect );
extern void CrEditBorderOff ( Widget w );

#endif /* _CrEdit_h */

#ifndef _CrList_h
#define _CrList_h

#include <Defines.h>

/**********************************************************************
 CrList resources:
 
 Name         Class        RepType    DefaultValue       Notice
 ----------------------------------------------------------------------
 object       Object       Pointer    NULL               for program

 **********************************************************************/

typedef struct _CrListClassRec *CrListWidgetClass;
typedef struct _CrListRec      *CrListWidget;

#if 0
#define XtNobject "object"
#define XtCObject "Object"

#define XtNarchetype "archetype"
#define XtCArchetype "Archetype"

#define XtNadjust "adjust"
#define XtCAdjust "Adjust"

#define XtNmap "map"
#define XtCMap "Map"

#define XtNinventory "inventory"
#define XtCInventory "Inventory"
#endif

#define XtNselectCallback "selectCallback"
#define XtCSelectCallback "SelectCallback"

#define XtNinsertCallback "insertCallback"
#define XtCInsertCallback "InsertCallback"

#define XtNdeleteCallback "deleteCallback"
#define XtCDeleteCallback "DeleteCallback"

#define XtNpackage "package"
#define XtCPackage "Package"

#define XtNnext "next"
#define XtCNext "Next"

extern WidgetClass crListWidgetClass;

/*
 * return type of registered next-function
 */
typedef struct _CrListNode {
    void *ptr;                /* pointer to list node */
    char *name;               /* name of object in node */
    New_Face *face;                /* bitmap image of object */
} *CrListNode;

/*
 * callback data
 */
typedef struct _CrListCall {
    void *node; /* pointer to list node */
    int index;  /* index to list node from top, neg. for empty list */
} *CrListCall;

#endif /* _CrList_h */

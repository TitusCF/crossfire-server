#ifndef _CrFace_h
#define _CrFace_h

/**********************************************************************
 CrFace resources:
 
 Name         Class        RepType    DefaultValue       Notice
 ----------------------------------------------------------------------
 object       Object       Pointer    NULL               for program

 **********************************************************************/

typedef struct _CrFaceClassRec *CrFaceWidgetClass;
typedef struct _CrFaceRec      *CrFaceWidget;

#define XtNobject "object"
#define XtCObject "Object"

#define XtNadjust "adjust"
#define XtCAdjust "Adjust"

extern WidgetClass crFaceWidgetClass;

#endif /* _CrFace_h */

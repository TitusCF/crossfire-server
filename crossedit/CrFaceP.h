#ifndef _CrFaceP_h
#define _CrFaceP_h

#include "CrFace.h"
#include "X11.h"

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

/**********************************************************************
 * class                                                              *
 **********************************************************************/

typedef struct {
  int empty;
} CrFaceClassPart;

typedef struct _CrFaceClassRec {
  CoreClassPart core_class;
  CrFaceClassPart crFace_class;
} CrFaceClassRec;

extern CrFaceClassRec crFaceClassRec;

/**********************************************************************
 * instance                                                           *
 **********************************************************************/

typedef struct {
  GC gc;
  object *ob;
  Boolean adjust;
} CrFacePart;

typedef struct _CrFaceRec {
  CorePart core;
  CrFacePart crFace;
} CrFaceRec;

#endif /* _CrFace_h */




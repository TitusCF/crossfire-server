
#ifndef _Defines_h_
#define _Defines_h_

#include <Posix.h>
#include <X11/Intrinsic.h>
#include <global.h>

#ifndef CrossEdit
#define CrossEdit
#endif

extern int FontSize;

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
typedef struct _MapAttr *MapAttr;

typedef struct mapdef *Map;

extern enum DisplayMode displaymode;

/*typedef struct obj *Object;*/

/* temporary kludge */

extern XColor discolor[];

void CrEditSelect(Widget w,XRectangle rect);

enum DisplayMode {Dm_Png};

/* Useful macro */
#define HAS_COLOUR(widg) (PlanesOfScreen(XtScreen(widg)) > 1)

#endif


/*** end of Defines.h ***/


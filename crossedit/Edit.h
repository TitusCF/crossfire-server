#ifndef _Edit_h
#define _Edit_h

#include "Defines.h"
#include "Cnv.h"

typedef enum _EditReturn {
    EditOk = 1, EditError
}   EditReturn;


typedef enum _EditType {
    Regular = 1, Pick, Wall, ClipBoard
}   EditType;

struct _Edit{
    struct _Edit *next;   /* in list of editors */
    App app;              /* in this application context */
    MapAttr mapattr;      /* map attributes */
    Widget w;		  /* mapEditor widget */
    Widget view;	  /* viewport of mapEditor */
    Widget shell;	  /* shell widget of map */
    Map emap;             /* map-struct itself */
    unsigned int modified:1;       /* map has bee touched */
    unsigned int read_only:1;      /* cannot modify */
    unsigned int overwrite:1;      /* deletes object under insertion */
    unsigned int auto_choose:1;    /* select object to inserted */
    struct {
#if 0
	Widget path;
	Widget size;
	Widget start;
#endif
	Widget refresh;
	Widget read_only;
	Widget weak_walls;
	Widget overwrite;
	Widget auto_choose;
	Widget stacking;
    }   iw;
    EditType type;
};

extern const XRectangle EditRectAll;

/*
 * macros
 */
#define EditGetPath(self) (self->emap ? self->emap->path : NULL)
#define EditGetMap(self) (self->emap)
#define EditIsModified(self) (self->modified)

/*
 * members
 */
extern void EditResizeScroll ( Edit self, int width, int height, int dx, int dy );
extern Edit EditCreate ( App app, EditType type, String path );
extern void EditDestroy ( Edit self );
extern void EditUpdate ( Edit self );
extern EditReturn EditSave ( Edit self );
extern EditReturn EditLoad ( Edit self );
extern void EditPerformFill ( Edit self, int x, int y );
extern void EditPerformFillBelow ( Edit self, int x, int y );
extern void EditFillRectangle ( Edit self, XRectangle rec );
extern void EditWipeRectangle ( Edit self, XRectangle rec );
extern void EditShaveRectangle ( Edit self, XRectangle rec );
extern void EditShaveRectangleBelow ( Edit self, XRectangle rec );
extern void EditCopyRectangle ( Edit self, Edit src, XRectangle rect, int sx, int sy );
extern void EditDeletePoint ( Edit self, int x, int y );
extern void EditModified ( Edit self );
extern void EditUnmodified ( Edit self );
extern Boolean EditDelete ( Edit self, int x, int y, int z );
extern Boolean EditInsert ( Edit self, int x, int y, int z );
extern Boolean EditObjectInsert ( Edit self, object *obj, int x, int y, int z );
extern void EditSetPath ( Edit self, String path );

#endif /* _Edit_h */



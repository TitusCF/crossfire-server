#ifndef _Attr_h
#define _Attr_h

#include <Defines.h>
#include <Cnv.h>


/**********************************************************************/

/*** variables, index ***/
typedef enum {
    I_Path, I_X, I_Y, I_Weight, I_Connect, I_Hp, 
    I_Trigger, I_Sacrifice, I_Count, I_Lockcode, 
    I_Direction, I_Rotation, I_NoPick, I_Unique, 
    I_WeightL, I_Brand, I_Maker
} AttrIndex;

typedef enum _AttrType { 
    TypeString,
    TypeToggle 
} AttrType;

typedef void (*AttrFunc)(object *, char *, XtPointer);

typedef struct {
    String label;
    AttrType type;
    AttrFunc getValue;
    AttrFunc putValue;
} AttrDef;

typedef struct {
    Widget cont;
    Widget value;
} AttrTags;

struct _Attr {
    struct _Attr *attr;
    App app;
    Widget w;                /* not used */
    unsigned long flags;     /* editor-type T_<> */
    int attrnumber;
    object *op;              /* object to edit */
    Edit client;
    Widget shell;            /* popup-shell */
    struct {
	Widget name;         /* text */
	Widget msg;          /* command */
	Widget inv;          /* command */
	Widget exact;        /* for dumbing object data */
	Widget face;         /* face of object */
	Widget vars;         /* to get list of variables; command */
	Widget var;          /* variable name of object; text */
	Widget value;        /* value of variable; text */
    } iw;
    struct {
	Widget shell;
	Widget list;
    } vars;
    AttrTags *tags;
#if 1
    AttrDef *desc;
#endif
    Boolean isup;            /* is window popped up  */
    Boolean modified;        /* should not used */
    CnvBrowse dump;          /* showing window for dump_object */  
};

/*
 * macro inteface
 */
#define AttrGetObject(self) (self->op)
#define AttrIsup(self)      (self->isup)
#define AttrSetIsup(self,b) {self->isup = b;}

/*
 * function inteface
 */
extern Attr AttrCreate ( char *name, App app, object *ob, 
			AttrDef *desc, unsigned long flags, Edit);
extern void AttrChange ( Attr self, object *ob, unsigned long flags, Edit);
extern void AttrDestroy ( Attr self );
extern void AttrApply ( Attr self );

extern int GetType (object *tmp);

extern AttrDef AttrDescription[];

#endif /* _Attr_h */

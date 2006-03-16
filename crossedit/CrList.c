#include <Posix.h>
#include <App.h>
#include <X11.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <debug.h>

#include <CrListP.h>
#include <CrUtil.h>

/**********************************************************************
 * resources
 **********************************************************************/

#define Offset(field) XtOffsetOf(CrListRec,crList.field)
static XtResource resources[] = {

  { XtNselectCallback,XtCSelectCallback,XtRCallback,sizeof(XtPointer),
    Offset(selectCallbacks),XtRCallback,NULL },

  { XtNinsertCallback,XtCInsertCallback,XtRCallback,sizeof(XtPointer),
    Offset(insertCallbacks),XtRCallback,NULL },

  { XtNdeleteCallback,XtCDeleteCallback,XtRCallback,sizeof(XtPointer),
    Offset(deleteCallbacks),XtRCallback,NULL },

  { XtNnext, XtCNext,XtRPointer,sizeof(XtPointer),
    Offset(next),XtRPointer,NULL },

  { XtNpackage,XtCPackage,XtRPointer,sizeof(XtPointer),
    Offset(caller),XtRPointer,NULL },

  { XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
    Offset(font),XtRString, XtDefaultFont},

  { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
    Offset(foreground), XtRString, XtDefaultForeground},
};
#undef Offset

/**********************************************************************
 * declarations
 **********************************************************************/

static void ClassInitialize(void);
static void Initialize(Widget req,Widget new,Arg args[],Cardinal *num);
static void Destroy(Widget w);
static void Resize(Widget w);
static void Redisplay(Widget w,XEvent *event,Region region);
static Boolean SetValues(Widget cur,Widget req,Widget new,
			 Arg args[],Cardinal *num);
static void Insert(Widget,XEvent*,String*,Cardinal*);
static void Select(Widget,XEvent*,String*,Cardinal*);
static void Delete(Widget,XEvent*,String*,Cardinal*);

/**********************************************************************
 * actions & translations
 **********************************************************************/

static XtActionsRec actionsList[] = {
  {"insert", Insert},
  {"select", Select},
  {"delete", Delete},
};

static char defaultTranslations[] = "\
<Btn1Up>:        insert() \n\
<Btn2Up>:        select() \n\
<Btn3Up>:        delete() \n\
";

/**********************************************************************
 * class
 **********************************************************************/

CrListClassRec crListClassRec = {
  { /*** Core ***/
    /* superclass            */ (WidgetClass) &widgetClassRec,    
    /* class_name            */ "CrList",
    /* widget_size           */ sizeof(CrListRec) ,
    /* class_initialize      */ ClassInitialize ,
    /* class_part_initialize */ NULL ,
    /* class_inited          */ FALSE ,
    /* initialize            */ Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ actionsList,
    /* num_actions           */ XtNumber(actionsList),
    /* resources             */ resources,
    /* num_resources         */ XtNumber(resources),
    /* xrm_class             */ NULLQUARK,
    /* compress_motion       */ TRUE, 
    /* compress_exposure     */ TRUE,
    /* compress_enterleave   */ TRUE,
    /* visible_interest      */ FALSE,
    /* destroy               */ Destroy,
    /* resize                */ Resize,
    /* expose                */ Redisplay,
    /* set_values            */ SetValues,
    /* set_values_hook       */ NULL,
    /* set_values_almost     */ XtInheritSetValuesAlmost,
    /* get_values_hook       */ NULL,
    /* accept_focus          */ NULL,
    /* version               */ XtVersion,
    /* callback_private      */ NULL,
    /* tm_table              */ defaultTranslations,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
  },
  { /*** CrList ***/
     /* empty                    */ 0,				
  }
};

WidgetClass crListWidgetClass = (WidgetClass)&crListClassRec;

/**********************************************************************
 * privates
 **********************************************************************/

static void SetSize(Widget w)
{
    CrListWidget self = (CrListWidget)w;
    int i = 0;
    Dimension width,height,rWidth,rHeight;
    CrListNode node = 0;

    /*** find out size ***/
    if (!self->crList.next) {
	debug ("No Next function for CrListWidget!\n");
	return;
    }

    do {
	node = self->crList.next
	    ((XtPointer)self->crList.caller, (XtPointer)node);
	i++;
    } while (node);

    height = --i * FontSize;
    width = self->core.width;
    
    /*** do request from parent container ***/
    switch(XtMakeResizeRequest(w,width,height,&rWidth,&rHeight)) {
      case XtGeometryYes:
      case XtGeometryNo:
	break;
      case XtGeometryAlmost:
	width = rWidth;
	height = rHeight;
	XtMakeResizeRequest(w,width,height,&rWidth,&rHeight);
	break;
      default:
	XtAppWarning(XtWidgetToApplicationContext(w),
		     "CrList: unknown geometry return");
	break;
    }
    debug3("CrList-%s-Setting size to %dx%d\n",
	   XtName(w),
	   self->core.width,
	   self->core.height);
}

/*
 * function: get y's node in list from top
 * y       : index
 * return  : 
 */
static CrListNode GetYNode (Widget w, int y)
{
    CrListWidget self = (CrListWidget)w;
    CrListNode node = NULL;
    int i = 0;

    if (!self->crList.next) {
	debug ("No Next function for CrListWidget!\n");
	return 0;
    }

    node = self->crList.next ((XtPointer)self->crList.caller, (XtPointer)0);
    for (i = 0; node && i < y; i++)
	node = self->crList.next 
	    ((XtPointer)self->crList.caller, (XtPointer)node);

    return (node);
}

/* hmm */
static CrListCall GetYCall (Widget w, int y)
{
    CrListNode node = GetYNode (w, y);
    static struct _CrListCall ret;
    
    ret.index = -1; /* empty list */
    ret.node = NULL;
    if (!node)
	return &ret;
    ret.index = y;
    ret.node = node->ptr;
    return &ret;
}

/**********************************************************************
 * actions procedures
 **********************************************************************/

static void Insert(Widget w,XEvent *e,String *argv,Cardinal *argc)
{
    CrListWidget self = (CrListWidget)w;
    CrListCall ret =  GetYCall(w, e->xbutton.y / FontSize);

    if (!self->crList.insertCallbacks)
	return;
    debug2("CrList-%s-Insert() %d\n",XtName(w),ret->index);

    XtCallCallbackList (w,self->crList.insertCallbacks, (XtPointer) ret);
    SetSize(w);
    Redisplay(w,e,NULL);
}

static void Select(Widget new,XEvent *e,String *argv,Cardinal *argc)
{
    CrListWidget self = (CrListWidget)new;
    CrListCall ret =  GetYCall(new, e->xbutton.y / FontSize);

    if (ret->index < 0)	return;
    debug1("%s-Select()\n",self->core.name);
    XtCallCallbackList (new,self->crList.selectCallbacks, (XtPointer) ret);
}

static void Delete(Widget new,XEvent *e,String *argv,Cardinal *argc)
{
    CrListWidget self = (CrListWidget)new;
    CrListCall ret =  GetYCall(new, e->xbutton.y / FontSize);

    if (ret->index < 0 || !self->crList.deleteCallbacks)
	return;
    debug2("CrList-%s-Delete() %d\n",XtName(new),ret->index);
    XtCallCallbackList	(new,self->crList.deleteCallbacks, (XtPointer) ret);
    SetSize(new);
    Redisplay(new,e,NULL);
}

/**********************************************************************
 * members
 **********************************************************************/

static void ClassInitialize(void)
{
    debug0("CrList-ClassInitialize()\n");
}

static void Initialize(Widget req,Widget new,Arg args[],Cardinal *num)
{
    CrListWidget w = (CrListWidget)new;
    XGCValues   values;
    
    debug1("CrList-%s-Initialize()\n",XtName(new));
    if(w->crList.caller == NULL) 
	CnvDie(new,"Object has no container");

    if (!w->core.width)
	w->core.width++;
    SetSize(new);    
    w->crList.gc = GCCreate (new);
    values.font = w->crList.font->fid;
    values.foreground   = w->crList.foreground;
    w->crList.normal_GC = XtGetGC 
	(new, (unsigned) (GCForeground | GCFont), &values);
}

static void Destroy(Widget w)
{
  /* CrListWidget self = (CrListWidget)w; */
  debug1("CrList-%s-Destroy()\n",XtName(w));
}

static void Resize(Widget w)
{
  /* CrListWidget self = (CrListWidget)w; */
  debug0("Resize()\n");
}


static void Redisplay(Widget w,XEvent *event,Region region)
{
    CrListWidget self = (CrListWidget)w;
    CrListNode node;
    int y;
    XRectangle rect;
    
    debug0("CrList-Redisplay()\n");

    if (region) {
	XClipBox(region,&rect);
	rect.height = rect.height / FontSize + 2;
	rect.y /= FontSize;
	XClearArea(XtDisplay(w),XtWindow(w), 0, rect.y * FontSize, 
		   0, rect.height * FontSize - 1, False);
    } else {
	rect.y = 0;
	XClearWindow (XtDisplay(w),XtWindow(w));
    }
    node = (CrListNode)GetYNode(w, rect.y);

    for (y = rect.y; node && (!region || rect.height--); y++) {
	debug2 ("%s Drawing %s\n", self->core.name,node->name);
	/*DrawObject(w,0,y,node);*/
	DrawFacePart(w, self->crList.gc, node->face, 0, y * FontSize, 0, 0);
	XDrawString(XtDisplay(w),XtWindow(w),
		    self->crList.normal_GC 
		    /*DefaultGCOfScreen(XtScreen(w))*/,
		    FontSize + 8, y * FontSize + 16,
		    node->name,strlen(node->name));
	node = self->crList.next 
	    ((XtPointer)self->crList.caller, (XtPointer)node);
    }
}

static Boolean SetValues(Widget cur,Widget req,Widget new,
			 Arg args[],Cardinal *num)
{
  /* CrListWidget self = (CrListWidget)new; */
  debug0("SetValues()\n");
  SetSize(new); 
  return True;
}

/**********************************************************************
 * public
 **********************************************************************/

/*** end of CrList.c ***/









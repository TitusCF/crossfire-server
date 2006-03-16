#include <Defines.h> /* off */
#include <global.h>
#include <debug.h>
#include <Cnv.h>
#include <CrFaceP.h>
#include <CrUtil.h>

/**********************************************************************
 * resources
 **********************************************************************/

#define Offset(field) XtOffsetOf(CrFaceRec,crFace.field)
static XtResource resources[] = {
  { XtNobject,XtCObject,XtRPointer,sizeof(XtPointer), /* caddr_t ??? */
    Offset(ob),XtRPointer,NULL },
  { XtNadjust,XtCAdjust,XtRBool,sizeof(Boolean),
    Offset(adjust),XtRString,"true" }, 
};
#undef Offset

static void ClassInitialize(void);
static void Initialize(Widget req,Widget new,Arg args[],Cardinal *num);
static void Destroy(Widget w);
static void Resize(Widget w);
static void Redisplay(Widget w,XEvent *event,Region region);
static Boolean SetValues(Widget cur,Widget req,Widget new,
			 Arg args[],Cardinal *num);

/**********************************************************************
 * class structure
 **********************************************************************/

CrFaceClassRec crFaceClassRec = {
  { /****** Core ******/
    /* superclass            */ (WidgetClass) &widgetClassRec,    
    /* class_name            */ "CrFace",
    /* widget_size           */ sizeof(CrFaceRec) ,
    /* class_initialize      */ ClassInitialize ,
    /* class_part_initialize */ NULL ,
    /* class_inited          */ FALSE ,
    /* initialize            */ Initialize,
    /* initialize_hook       */ NULL,
    /* realize               */ XtInheritRealize,
    /* actions               */ NULL,
    /* num_actions           */ 0 ,
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
    /* tm_table              */ NULL,
    /* query_geometry        */ XtInheritQueryGeometry,
    /* display_accelerator   */ XtInheritDisplayAccelerator,
    /* extension             */ NULL
  },
  { /****** CrFace ******/
     /* empty                    */ 0,				
  }
};

WidgetClass crFaceWidgetClass = (WidgetClass)&crFaceClassRec;

/**********************************************************************
 * privates
 **********************************************************************/

static int DrawObject(Widget wd, int x, int y, object * tmp)
{
    CrFaceWidget w = (CrFaceWidget)wd;

    if (!tmp)
	tmp = &empty_archetype->clone;

    DrawPartObject(wd, w->crFace.gc, tmp, x * FontSize, y * FontSize);
    return (0);
}

static void GetConst(Widget w,int *minx,int *miny,int *maxx,int *maxy)
{
#if 0
  archetype *at;
  CrFaceWidget self = (CrFaceWidget)w;

  if(self->crFace.ob == NULL) return;
  *minx = 0, *miny = 0, *maxx = 0, *maxy = 0;
  for (at = self->crFace.ob->arch; at; at = at->more) {
    if (*maxx < at->clone.x) *maxx = at->clone.x;
    if (*maxy < at->clone.y) *maxy = at->clone.x;
    if (*minx > at->clone.x) *minx = at->clone.x;
    if (*miny > at->clone.y) *miny = at->clone.y;
  }
#else
  object *obj;
  CrFaceWidget self = (CrFaceWidget)w;

  if(self->crFace.ob == NULL) return;
  *minx = 0, *miny = 0, *maxx = 0, *maxy = 0;
  for (obj = self->crFace.ob; obj; obj = obj->more) {
    if (*maxx < obj->x) *maxx = obj->x;
    if (*maxy < obj->y) *maxy = obj->y;
    if (*minx > obj->x) *minx = obj->x;
    if (*miny > obj->y) *miny = obj->y;
  }
#endif
}

static void GetSize(Widget w,Dimension *width,Dimension *height)
{
  CrFaceWidget self = (CrFaceWidget)w;
  int minx, miny, maxx,maxy;

  if(!self->crFace.ob) return;
  GetConst(w,&minx,&miny,&maxx,&maxy);
  *width = (maxx - minx + 1) * FontSize;
  *height = (maxy - miny + 1) * FontSize;
}

/**********************************************************************
 * members
 **********************************************************************/

static void ClassInitialize(void)
{
  debug0 ("ClassInitialize()\n");
}

static void Initialize(Widget req,Widget new,Arg args[],Cardinal *num)
{
    CrFaceWidget w = (CrFaceWidget)new;

    /* NULLed allowed */
    if(w->crFace.ob == NULL) 
	w->core.width = 1;

    if(w->core.width < 10) w->core.width = 96;
    if(w->core.height < 10) w->core.height = 48;

    if(w->crFace.adjust) GetSize(new,&(w->core.width),&(w->core.height));
    w->crFace.gc = GCCreate (new);
    debug0 ("CrFace-Initialize()\n");
}

static void Destroy(Widget w)
{
  debug("Destroy()\n");
}

static void Resize(Widget w)
{
  debug("Resize()\n");
}


static void Redisplay(Widget w,XEvent *event,Region region)
{
#if 0
  CrFaceWidget self = (CrFaceWidget)w;
  archetype *at;
  int minx = 0, miny = 0, maxx = 0,maxy;

  debug("Redisplay()\n");
  if(self->crFace.ob == NULL) {
      XClearWindow(XtDisplay(w),XtWindow(w));
      return;
  }

  GetConst(w,&minx,&miny,&maxx,&maxy);
  for (at = self->crFace.ob->arch;at; at = at->more) {
    DrawObject (w,
		at->clone.x - minx, 
		at->clone.y - miny, 
		&at->clone);
  }
#else
  CrFaceWidget self = (CrFaceWidget)w;
  object *obj;
  int minx = 0, miny = 0, maxx = 0,maxy;
  int x,y;

  if (event->xexpose.count!=0) return;
  debug("Redisplay()\n");
  if(self->crFace.ob == NULL) {
      XClearWindow(XtDisplay(w),XtWindow(w));
      return;
  } else {
      GetConst(w,&minx,&miny,&maxx,&maxy);
      x = self->crFace.ob->x - minx;
      y = self->crFace.ob->y - miny;
      for (obj = self->crFace.ob;obj; obj = obj->more) {
	  DrawObject (w, 
		      obj->x - minx - x, 
		      obj->y - miny - y, 
		      obj);
      }
  }
#endif
}

static Boolean SetValues(Widget cur,Widget req,Widget new,
			 Arg args[],Cardinal *num)
{
  CrFaceWidget w = (CrFaceWidget)new;
  debug("SetValues()\n");
  if(w->crFace.adjust) GetSize(new,&(w->core.width),&(w->core.height));

  if(w->crFace.ob) { /* object selected */
      if(w->crFace.ob->head) w->crFace.ob = w->crFace.ob->head;
  }
  return True;
}

/*** end of CrFace.c ***/









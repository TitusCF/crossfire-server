/*
 * static char *rcsid_map_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

#include "Defines.h"
#include "Ansi.h"
#include "Xaw.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "global.h"
#include "debug.h"

#include "Cnv.h"
#include "CrEditP.h"
#include "CrUtil.h"

extern    Pixmap *pixmaps;       /* list of pixmaps */

/**********************************************************************
 * resources
 **********************************************************************/

static XtResource resources[] = {
#define Offset(field) XtOffsetOf(CrEditRec, crEdit.field)
    { 
	XtNinsertCallback,
	XtCInsertCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(insertCallbacks),
	XtRCallback,
	NULL
    },{
	XtNselectCallback,
	XtCSelectCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(selectCallbacks),
	XtRCallback,
	NULL
    },{
	XtNpropsCallback,
	XtCPropsCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(propsCallbacks),
	XtRCallback,
	NULL
    },{
	XtNdeleteCallback,
	XtCDeleteCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(deleteCallbacks),
	XtRCallback,
	NULL
    },{
	XtNalignCallback,
	XtCAlignCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(alignCallbacks),
	XtRCallback,
	NULL
    },{
	XtNfeedCallback,
	XtCFeedCallback,
	XtRCallback,
	sizeof(XtCallbackList),
	Offset(feedCallbacks),
	XtRCallback,
	NULL
    },{
	XtNmap,
	XtCMap,
	XtRPointer,
	sizeof(XtPointer),
	Offset(map),
	XtRPointer,
	NULL
    },{
	XtNstacking,
	XtCStacking,
	XtRInt,
	sizeof(int),
	Offset(stacking),
	XtRImmediate,
	(XtPointer)0
    },{
	XtNshow_weak_walls,
	XtCShow_weak_walls,
	XtRInt,
	sizeof(int),
	Offset(show_weak_walls),
	XtRImmediate,
	(XtPointer)0
    },{
	XtNselectArea,
	XtCSelectArea,
	XtRBoolean,
	sizeof(Boolean),
	Offset(selectArea),
	XtRBoolean,
	(XtPointer)NULL,
    }
#undef offset
};

/**********************************************************************
 * declarations
 **********************************************************************/

static void UpdatePosition (Widget w, int x, int y,Boolean inv);
static void ClassInitialize(void);
static void Initialize(Widget req,Widget new,Arg args[],Cardinal *num);
static void Destroy(Widget w);
static void Resize(Widget w);
static void Redisplay(Widget w,XEvent *event,Region region);
static Boolean SetValues(Widget cur,Widget req,Widget new,
			 Arg args[],Cardinal *num);

#define EmptyObject "editor_archetype"

/**********************************************************************
 * actions & translations
 **********************************************************************/

static void InsertAc ( Widget w, XEvent * event, 
		      String * argv, Cardinal * argc );
static void DeleteAc ( Widget w, XEvent * event, 
		      String * argv, Cardinal * argc );
static void PropsAc ( Widget w, XEvent * event, 
		      String * argv, Cardinal * argc );
static void MoveAc ( Widget w, XEvent * e, 
		    String * argv, Cardinal * argc );
static void ResizeAc ( Widget w, XEvent * e, 
		      String * argv, Cardinal * argc );
static void ScrollAc ( Widget w, XEvent * e, 
		      String * argv, Cardinal * argc );
static void SelectBeginAc ( Widget w, XEvent * e, 
			   String * argv, Cardinal * argc );
static void SelectExpandAc ( Widget w, XEvent * e, 
			    String * argv, Cardinal * argc );
static void SelectEndAc ( Widget w, XEvent * e, 
			 String * argv, Cardinal * argc );
static void FeedAc ( Widget w, XEvent * e, 
		    String * argv, Cardinal * argc );

static XtActionsRec actions[] = {
  {"Insert", InsertAc},
  {"SelectBegin", SelectBeginAc},
  {"SelectExpand", SelectExpandAc},
  {"SelectEnd", SelectEndAc},
  {"Props", PropsAc},
  {"Delete", DeleteAc},
  {"Feed", FeedAc},
  {"Move", MoveAc},
  {"Resize", ResizeAc},
  {"Scroll", ScrollAc},
};

static char translations[] = "\
	<Btn1Up>:         Insert(archetype) \n\
	<Btn1Motion>:     Insert(archetype) \n\
	Ctrl<Btn2Up>:     Feed(coordinate) \n\
	Ctrl<Btn2Motion>: Feed(coordinate) \n\
	Shift<Btn2Up>:    Props() \n\
	None<Btn2Down>:   SelectBegin() \n\
	None<Btn2Motion>: SelectExpand() \n\
	None<Btn2Up>:     SelectEnd() \n\
	<Btn3Down>:       Delete(archetype) \n\
	<Btn3Motion>:     Delete(archetype) \n\
        Ctrl<Key>Right:   Resize(right) \n\
	Ctrl<Key>Left:    Resize(left) \n\
        Ctrl<Key>Down:    Resize(down) \n\
	Ctrl<Key>Up:      Resize(up) \n\
        Shift<Key>Right:  Scroll(right) \n\
	Shift<Key>Left:   Scroll(left) \n\
        Shift<Key>Down:   Scroll(down) \n\
	Shift<Key>Up:     Scroll(up) \n\
        <Key>Right:       Move(right) \n\
	<Key>Left:        Move(left) \n\
        <Key>Down:        Move(down) \n\
	<Key>Up:          Move(up) \n\
";

/**********************************************************************
 * class record
 **********************************************************************/

CrEditClassRec crEditClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass)&widgetClassRec,
    /* class_name		*/	"CrEdit",
    /* widget_size		*/	sizeof(CrEditRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	actions,
    /* num_actions		*/	XtNumber(actions),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	translations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* crEdit fields */
    /* empty			*/	0
  }
};

WidgetClass crEditWidgetClass = (WidgetClass)&crEditClassRec;

/**********************************************************************
 * inner functions 
 **********************************************************************/

/*
 *
 */
static void SegToRec(XSegment *seg,XRectangle *rec)
{
    /*** calculate rectangle ***/
    rec->x = seg->x1 < seg->x2 ? seg->x1 : seg->x2;
    rec->y = seg->y1 < seg->y2 ? seg->y1 : seg->y2;
    rec->width = abs(seg->x1 - seg->x2) + 1;
    rec->height = abs(seg->y1 - seg->y2) + 1;
}

static void SetSize(Widget w)
{
    CrEditWidget self = (CrEditWidget)w;
    XtWidgetGeometry request,reply;
    /*Dimension width,height,rWidth,rHeight;*/

    /*** determine size to ask for ***/
    if(self->crEdit.map) {
	request.width = MAP_WIDTH(self->crEdit.map) * self->crEdit.fontSize;
	request.height = MAP_HEIGHT(self->crEdit.map) * self->crEdit.fontSize;
    } else {
	request.width = self->crEdit.fontSize;
	request.height = self->crEdit.fontSize;
    }
    request.x = request.y = 0;
    request.request_mode = CWWidth | CWHeight;
    
    /*** ask it ***/
    switch(XtMakeGeometryRequest(w,&request,&reply)) {
    case XtGeometryYes:
	break;
    case XtGeometryNo:
	debug1 ("%s: May not change my geometry\n",XtName(w));
	break;
    case XtGeometryAlmost:
	XtMakeGeometryRequest(w,&reply,&reply);
	break;
    default:
	CnvDie(w,"Unknown geometry request return");
	break;
    };
    /*
    XSendEvent(XtDisplay(w),XtWindow(w),False,ResizeRequest,NULL);
    */
    debug3("%s: setting widget size to %dx%d\n",XtName(w),self->core.width,
	   self->core.height);
}
	
/**********************************************************************
 **********************************************************************/


static void AnimateCursor (Widget w, int x, int y) {
    /* Currently, this was only supported with bitmap mode.
     * I'm sure soething clever could be done in other modes.
     */

}



/**********************************************************************
 * drawing functions
 * hierarchy:
 *   DrawRectangle DrawBorder
 *   UpdatePosition
 *   DrawObject
 **********************************************************************/

/*
 * inv : draw inverse colored object if True
 */


/*
 * member: update object stack at point in map
 * x,y   : point
 * inv   : draw inverse
 */
static void UpdatePosition (Widget w, int x, int y,Boolean inv)
{
    CrEditWidget self = (CrEditWidget)w;
    object *op;
    int i;

    if(out_of_map(self->crEdit.map,x,y)) return; 
    
    if (self->crEdit.stacking) {
	/* stacking mode */
	float xb, yb;
	XClearArea (XtDisplay(w), XtWindow(w), 
		    x * self->crEdit.fontSize, 
		    y * self->crEdit.fontSize, 
		    self->crEdit.fontSize, 
		    self->crEdit.fontSize, False);
	for (i = 0, op = get_map_ob (self->crEdit.map, x, y); 
	     op;
	     i++, op = op->above);
	if (i > 0) {
	    int diff = self->crEdit.fontSize - FontSize;
	    int depth = diff / STACK_MIN + 1;
	    float dist = 0;

	    if (depth < 1)
		depth = 1;
	    for (op = get_map_ob (self->crEdit.map, x, y); i > depth; i--)
		op = op->above;

	    i--;
	    if (i) {
		dist = (diff - 1) / (float)i;
		if (dist > STACK_MAX)
		    dist = STACK_MAX;
	    }

	    xb = diff + x * self->crEdit.fontSize - 1;
	    yb = diff + y * self->crEdit.fontSize - 1;

	    while (op) {
		if (QUERY_FLAG(op, FLAG_TEAR_DOWN) && self->crEdit.show_weak_walls) {
		    New_Face * f;
		    f = &new_faces[GET_ANIMATION(op,NUM_ANIMATIONS(op)/2)];
		    DrawFacePart(w, self->crEdit.gc, f, xb, yb, 0, 0);
		} else {
		    DrawPartObject(w, self->crEdit.gc, op, xb, yb);
		}

		if (HAS_COLOUR(w)) {
		    XSetForeground(XtDisplay(w), self->crEdit.gc, 
				   BlackPixelOfScreen(XtScreen(w)));
		}
		XDrawRectangle (XtDisplay(w), 
				XtWindow(w), self->crEdit.gc,
				(int) xb, (int) yb, FontSize -1, FontSize -1);
		xb -= dist;
		yb -= dist;
		op = op->above;
	    }
	}
    } else { /* Normal map drawing routine */
        New_Face *f;

        if (GET_MAP_FLAGS(self->crEdit.map, x, y) & P_NEED_UPDATE) {
            update_position(self->crEdit.map, x, y);
        }

        if (displaymode==Dm_Png) {
	    int l;

	    for (l = 0; l < MAP_LAYERS; l++) {
		op = GET_MAP_FACE_OBJ(self->crEdit.map, x, y, l);
		if (op != NULL)
		    DrawPartObject(w, self->crEdit.gc, op,
				   x * self->crEdit.fontSize,
				   y * self->crEdit.fontSize);
	    }
	}
	op=get_map_ob(self->crEdit.map, x, y);
	while (op && op->above) op=op->above;
	if (op == NULL) {
	    ; /* Paint nothing */
	} else if (QUERY_FLAG(op, FLAG_TEAR_DOWN) && self->crEdit.show_weak_walls) {
            f = &new_faces[GET_ANIMATION(op, NUM_ANIMATIONS(op)/2)];

            if (displaymode == Dm_Png || f->number != blank_face->number)
                DrawFacePart (w, self->crEdit.gc, f,
                    x * self->crEdit.fontSize,
                    y * self->crEdit.fontSize,
                    0, 0);
        } else {
            if (displaymode == Dm_Png || op->face->number != blank_face->number)
                DrawPartObject (w, self->crEdit.gc, op,
                    x * self->crEdit.fontSize,
                    y * self->crEdit.fontSize);
        }
    }
    return;
}

/*
 * member: draw filled rectangle
 * rec   : rectangle
 * inv   : True = inverse
 */
static void DrawRectangle(Widget w,XRectangle area,Boolean inv)
{
    CrEditWidget self = (CrEditWidget)w;
    int i, j;

    if (!self->crEdit.map) return;
    for (j = area.x; 
	 j < area.x + area.width &&  j < MAP_WIDTH(self->crEdit.map); 
	 j++) {
	for (i = area.y; 
	     i < area.y + area.height && i < MAP_HEIGHT(self->crEdit.map); 
	     i++) {
	    UpdatePosition (w,j,i,inv);
	}
    }
}

/*
 * member: draw open square
 * seg   : start and end points of square
 * inv   : border of square are inversed
 */
static void DrawBorder(Widget w,XSegment seg,Boolean inv)
{
    CrEditWidget self = (CrEditWidget)w;
    int i,x1,x2,y1,y2;
    
    if (!self->crEdit.map) return;
    x1 = seg.x1 < seg.x2 ? seg.x1 : seg.x2;
    y1 = seg.y1 < seg.y2 ? seg.y1 : seg.y2;
    x2 = abs(seg.x1 - seg.x2) + x1;
    y2 = abs(seg.y1 - seg.y2) + y1;

    if(x1 > MAP_WIDTH(self->crEdit.map) ||
       x2 > MAP_WIDTH(self->crEdit.map) ||
       y1 > MAP_HEIGHT(self->crEdit.map) ||
       y2 > MAP_HEIGHT(self->crEdit.map)) return;

    if (inv) {
	XDrawRectangle (XtDisplay(w), XtWindow(w), 
			DefaultGC(XtDisplay(w), XScreenNumberOfScreen(XtScreen(w))), 
			x1 * self->crEdit.fontSize, 
			y1 * self->crEdit.fontSize, 
			(x2 - x1 + 1) * self->crEdit.fontSize - 1, 
			(y2 - y1 + 1) * self->crEdit.fontSize - 1);
	XDrawRectangle (XtDisplay(w), XtWindow(w), 
			DefaultGC(XtDisplay(w), XScreenNumberOfScreen(XtScreen(w))), 
			x1 * self->crEdit.fontSize + 1, 
			y1 * self->crEdit.fontSize + 1, 
			(x2 - x1 + 1) * self->crEdit.fontSize - 3, 
			(y2 - y1 + 1) * self->crEdit.fontSize - 3);
    } else {

    for(i=x1; 
	i <= x2; 
	i++) {
	UpdatePosition(w,i,seg.y1,inv);
	    if (y1 != y2)
	UpdatePosition(w,i,seg.y2,inv);
    }
	for(i=y1 + 1; 
	    i <= (y2 - 1);
	i++) {
	UpdatePosition(w,seg.x1,i,inv);
	    if (x1 != x2)
	UpdatePosition(w,seg.x2,i,inv);
    }
    }
}

/*
 * member:
 * x,y   :
 * return:
 */
static int CalcIndex (Widget w, int x, int y) {
    CrEditWidget self = (CrEditWidget)w; 
    object *op;
    int i, j = 0;
    int xp, yp;
    float xb, yb;
    
    xp = x / self->crEdit.fontSize;
    yp = y / self->crEdit.fontSize;

    if (out_of_map (self->crEdit.map, xp, yp))
	return 0;

    for (i = 0, op = get_map_ob (self->crEdit.map, xp, yp); 
	 op;
	 i++, op = op->above);

    if (i > 0) {
	int diff = self->crEdit.fontSize - FontSize;
	int depth = diff / STACK_MIN;
	float dist = 0;
	
	if (depth == 0)
	    depth = 1;
	if (i > depth)
	    i = depth;

	i--;
	if (i) {
	    dist = (diff - 1) / (float)i;
	    if (dist > STACK_MAX)
		dist = STACK_MAX;
	}

	xb = self->crEdit.fontSize - i * dist;
	yb = self->crEdit.fontSize - i * dist;

	x %= self->crEdit.fontSize;
	y %= self->crEdit.fontSize;

	debug4 ("%dx%d %d %f\n", x, y, i, dist);
	for (j = 0; j <= i; j++) {
	    if (x < xb && y < yb)	
		break;
	    xb += dist;
	    yb += dist;
	}
    }
    return (j);
}

/*
 * no borders drawing set
 */
static void BorderOff(Widget w)
{
    CrEditWidget self = (CrEditWidget)w;
    self->crEdit.seg.x1 = 0;
    self->crEdit.seg.y1 = 0;
    self->crEdit.seg.x2 = 10000;
    self->crEdit.seg.y2 = 10000;
}

/**********************************************************************
 * actions
 **********************************************************************/

/* 
 * action: insert object to map 
 */
static void InsertAc (Widget w, XEvent * event, 
		      String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x, y;
    static int oldx, oldy;
    struct CrEditCall call;

    AnimateCursor (w, event->xbutton.x, event->xbutton.y);
    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    if(XtHasCallbacks(w,XtNinsertCallback) == XtCallbackHasNone) return;
    /*** fill the call strcture ***/
    call.rect.x = x;
    call.rect.y = y;
    call.rect.width = 0;
    call.rect.height = 0;
    call.map = self->crEdit.map;
    call.z = self->crEdit.stacking ? 
	CalcIndex (w,event->xbutton.x,event->xbutton.y) : 0;
    
    if (!(event->type == MotionNotify  && x == oldx && y == oldy)) {
	XtCallCallbackList(w,self->crEdit.insertCallbacks,(XtPointer)&call);
    }
    oldx = x;
    oldy = y;
}

/*
 * action: set begin of selection
 */
static void SelectBeginAc(Widget w, XEvent * event, 
			  String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x,y;

    AnimateCursor (w, event->xbutton.x, event->xbutton.y);
    if(!self->crEdit.selectArea) return;
    DrawBorder(w,self->crEdit.seg,False);

    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    debug2("SelectBeginAc() %dx%d\n",x,y);

    if(out_of_map(self->crEdit.map,x,y)) return;    

    self->crEdit.seg.x1 = self->crEdit.seg.x2 = x;
    self->crEdit.seg.y1 = self->crEdit.seg.y2 = y;
    DrawBorder(w,self->crEdit.seg,True);
}

/*
 * action: expand selection
 */
static void SelectExpandAc(Widget w, XEvent * event, 
			   String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x,y;
    static int oldx, oldy;

    AnimateCursor (w, event->xbutton.x, event->xbutton.y);
    if(!self->crEdit.selectArea) return;

    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    if (x >= MAP_WIDTH(self->crEdit.map) )
	x = MAP_WIDTH(self->crEdit.map) - 1;
    if (x < 0)
	x = 0;

    if (y >= MAP_HEIGHT(self->crEdit.map) )
	y = MAP_HEIGHT(self->crEdit.map) - 1;
    if (y < 0)
	y = 0;

    if (!(event->type == MotionNotify  && x == oldx && y == oldy)) {
	DrawBorder(w,self->crEdit.seg,False);
	self->crEdit.seg.x2 = x;
	self->crEdit.seg.y2 = y;
	DrawBorder(w,self->crEdit.seg,True);
    }
    oldx = x;
    oldy = y;
}

/*
 * action: get end of selection
 */
static void SelectEndAc(Widget w, XEvent * event,
			String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x,y;
    struct CrEditCall call;

    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    debug2("SelectEndAc() %dx%d\n",x,y);

    if (x >= MAP_WIDTH(self->crEdit.map) )
	x = MAP_WIDTH(self->crEdit.map) - 1;
    if (x < 0)
	x = 0;

    if (y >= MAP_HEIGHT(self->crEdit.map) )
	y = MAP_HEIGHT(self->crEdit.map) - 1;
    if (y < 0)
	y = 0;

    /*** fill the call strcture ***/
    if(self->crEdit.selectArea) {
	SegToRec(&self->crEdit.seg,&call.rect);
    } else {
	call.rect.x = x;
	call.rect.y = y;
	call.rect.width = 0;
	call.rect.height = 0;
    }
    call.map = self->crEdit.map;
    call.z = 0;

    XtCallCallbackList(w,self->crEdit.selectCallbacks,(XtPointer)&call);
}

/* 
 * action: 
 */
static void PropsAc(Widget w, XEvent * event, 
		     String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x, y;
    struct CrEditCall call;

    AnimateCursor (w, event->xbutton.x, event->xbutton.y);
    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    /*** fill the call strcture ***/
    call.rect.x = x;
    call.rect.y = y;
    call.rect.width = 0;
    call.rect.height = 0;
    call.map = self->crEdit.map;
    call.z = self->crEdit.stacking ? CalcIndex 
	(w,event->xbutton.x, event->xbutton.y) : 0;

    XtCallCallbackList(w,self->crEdit.propsCallbacks,(XtPointer)&call);
}

/* 
 * action: delete object from map 
 */
static void DeleteAc(Widget w, XEvent * event, 
		     String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x, y;
    static int oldx, oldy;
    struct CrEditCall call;

    AnimateCursor (w, event->xbutton.x, event->xbutton.y);
    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    if (!(event->type == MotionNotify  && x == oldx && y == oldy)) {
	/*** fill the call strcture ***/
	call.rect.x = x;
	call.rect.y = y;
	call.rect.width = 0;
	call.rect.height = 0;
	call.map = self->crEdit.map;
	call.z = self->crEdit.stacking ? CalcIndex 
	    (w,event->xbutton.x, event->xbutton.y) : 0;

	XtCallCallbackList(w,self->crEdit.deleteCallbacks,(XtPointer)&call);
	UpdatePosition(w,x,y,False);
    }
    oldx = x;
    oldy = y;
}

/* 
 * action: feed point
 */
static void FeedAc (Widget w, XEvent * event, String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    int x, y;
    static int oldx, oldy;
    struct CrEditCall call;
    
    x = event->xbutton.x / self->crEdit.fontSize;
    y = event->xbutton.y / self->crEdit.fontSize;

    if (!(event->type == MotionNotify  && x == oldx && y == oldy)) {
	/*** fill the call strcture ***/
	call.rect.x = x;
	call.rect.y = y;
	call.rect.width = 0;
	call.rect.height = 0;
	call.map = self->crEdit.map;
	call.z = self->crEdit.stacking ? CalcIndex 
	    (w,event->xbutton.x, event->xbutton.y) : 0;

	XtCallCallbackList(w,self->crEdit.feedCallbacks,(XtPointer)&call);
    }

    oldx = x;
    oldy = y;
}

/*
 * action: resize to directions
 */
static void ResizeAc (Widget w, XEvent * e, String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    struct CrEditCall call;
    char buf[BUFSIZ];

    call.map = self->crEdit.map;
    call.rect.x = 0;
    call.rect.y = 0;
    call.rect.width = MAP_WIDTH(self->crEdit.map);
    call.rect.height = MAP_HEIGHT(self->crEdit.map);
    call.z = 0;
    if (argv) {
	if (!strcmp (argv[0], "right")) {
	    call.rect.width++;
	} else if (!strcmp (argv[0], "left")) {
	    call.rect.width--;
	} else if (!strcmp (argv[0], "down")) {
	    call.rect.height++;
	} else if (!strcmp (argv[0], "up")) {
	    call.rect.height--;
	} else {
	    sprintf(buf,"bad argument %s",argv[0]);
	    CnvWarn(w,buf);
	}
    }
    XtCallCallbackList(w,self->crEdit.alignCallbacks,(XtPointer)&call);
}

/*
 * action: scroll around to directions
 */
static void ScrollAc (Widget w, XEvent * e, String * argv, Cardinal * argc)
{
    CrEditWidget self = (CrEditWidget)w;
    struct CrEditCall call;
    char buf[BUFSIZ];

    call.map = self->crEdit.map;
    call.rect.x = 0;
    call.rect.y = 0;
    call.rect.width = MAP_WIDTH(self->crEdit.map);
    call.rect.height = MAP_HEIGHT(self->crEdit.map);
    call.z = 0;

    /*
    debug4("CrEdit::ScrollAc() by %dx%d+%d+%d",call->rect.width,
	   call->rect.height,call->rect.x,call->rect.y); */

    if (argv) {
	if (!strcmp (argv[0], "right")) {
	    call.rect.x++;
	} else if (!strcmp (argv[0], "left")) {
	    call.rect.x--;
	} else if (!strcmp (argv[0], "down")) {
	    call.rect.y++;
	} else if (!strcmp (argv[0], "up")) {
	    call.rect.y--;
	} else {
	    sprintf(buf,"bad argument %s",argv[0]);
	    CnvWarn(w,buf);
	}
	XtCallCallbackList(w,self->crEdit.alignCallbacks,(XtPointer)&call);
    }
}
    
/*
 * action: move clipped window in viewport
 */
static void MoveAc (Widget w, XEvent * e, String * argv, Cardinal * argc)
{
    /* CrEditWidget self = (CrEditWidget)w; */
    Widget horizontal, vertical;

    if (argv) {
	horizontal = XtNameToWidget (XtParent (w), "horizontal");
	vertical = XtNameToWidget (XtParent (w), "vertical");

	if (!strcmp (argv[0], "right")) {
	    String argv[1];
	    argv[0] = XtNewString ("Forward");
	    XtCallActionProc (horizontal, "StartScroll", e, argv, 1);
	    XtFree (argv[0]);
	    argv[0] = XtNewString ("Proportional");
	    XtCallActionProc (horizontal, "NotifyScroll", e, argv, 1);
	    XtFree (argv[0]);
	    XtCallActionProc (horizontal, "EndScroll", e, NULL, 0);
	} else if (!strcmp (argv[0], "left")) {
	    String argv[1];
	    argv[0] = XtNewString ("Backward");
	    XtCallActionProc (horizontal, "StartScroll", e, argv, 1);
	    XtFree (argv[0]);
	    argv[0] = XtNewString ("Proportional");
	    XtCallActionProc (horizontal, "NotifyScroll", e, argv, 1);
	    XtFree (argv[0]);
	    XtCallActionProc (horizontal, "EndScroll", e, NULL, 0);
	} else if (!strcmp (argv[0], "down")) {
	    String argv[1];
	    argv[0] = XtNewString ("Forward");
	    XtCallActionProc (vertical, "StartScroll", e, argv, 1);
	    XtFree (argv[0]);
	    argv[0] = XtNewString ("Proportional");
	    XtCallActionProc (vertical, "NotifyScroll", e, argv, 1);
	    XtFree (argv[0]);
	    XtCallActionProc (vertical, "EndScroll", e, NULL, 0);
	} else if (!strcmp (argv[0], "up")) {
	    String argv[1];
	    argv[0] = XtNewString ("Backward");
	    XtCallActionProc (vertical, "StartScroll", e, argv, 1);
	    XtFree (argv[0]);
	    argv[0] = XtNewString ("Proportional");
	    XtCallActionProc (vertical, "NotifyScroll", e, argv, 1);
	    XtFree (argv[0]);
	    XtCallActionProc (vertical, "EndScroll", e, NULL, 0);
	}
    }
}

/**********************************************************************
 * members
 **********************************************************************/

static void ClassInitialize(void)
{
    debug0("CrEdit-ClassInitialize()\n");

}

static void Initialize(Widget req,Widget new,Arg args[],Cardinal *num)
{ 
    CrEditWidget self = (CrEditWidget)new;
	
    debug1("CrEdit-%s-Initialize()\n",XtName(new));

    /*** allocate gc ***/
    self->crEdit.gc = GCCreate (new);
    self->crEdit.fontSize = FontSize;
    SetSize(new);
    BorderOff(new);
}

static void Destroy(Widget w)
{
    CrEditWidget self = (CrEditWidget)w;
    debug1("CrEdit-%s-Destroy()\n",XtName(w));
    XtReleaseGC(w,self->crEdit.gc);
}

static void Resize(Widget w)
{
    /* CrEditWidget self = (CrEditWidget)w; */
    debug1("CrEdit-%s-Resize()\n",XtName(w));
    SetSize(w);
}

static void Redisplay(Widget w,XEvent *event,Region region)
{
    CrEditWidget self = (CrEditWidget)w;
    XRectangle rect, new;

    if(!region) return;
    XClipBox(region,&rect);
    new.x = rect.x / self->crEdit.fontSize;
    new.y = rect.y / self->crEdit.fontSize;
    new.width = (rect.x + rect.width) / self->crEdit.fontSize - new.x + 1;
    new.height = (rect.y + rect.height) / self->crEdit.fontSize - new.y + 1;

    debug5("CrEdit-%s-Expose() %dx%d+%d+%d\n",XtName(w),
	   new.width, new.height, new.x, new.y);

    DrawRectangle(w,new,False);
    if (self->crEdit.selectArea)
	DrawBorder(w, self->crEdit.seg, True);
}

static Boolean SetValues(Widget cur,Widget req,Widget new,
			 Arg args[],Cardinal *num)
{
    Boolean redpy = False;

    CrEditWidget self = (CrEditWidget)new;
    CrEditWidget old = (CrEditWidget)cur;
    debug1("CrEdit-%s-SetValues()\n",XtName(new));
    
    /*** stacking change ***/
    if(self->crEdit.stacking != old->crEdit.stacking) {
	self->crEdit.fontSize = FontSize + self->crEdit.stacking;
	SetSize(new);
	redpy = True;
    }
    /*** map change ***/
    if(self->crEdit.map != old->crEdit.map) {
	SetSize(new);
	redpy = True;
    }
    /*** weak wall change ***/
    if(self->crEdit.show_weak_walls != old->crEdit.show_weak_walls) {
	SetSize(new);
	redpy = True;
    }
    return redpy; /* redisplay */
}

/**********************************************************************
 * public
 **********************************************************************/

/*
 * member: draw rectangle part of w
 * rect  : rectangle, width = height = 10000 to draw all
 */
void CrEditRefresh(Widget w,XRectangle rect)
{
    CrEditWidget self = (CrEditWidget)w;
    if(!w) return;


    debug4("CrEditRefresh() %dx%d+%d+%d\n",
	   rect.width,
	   rect.height,
	   rect.x,
	   rect.y
	   );
    /* for all */
    if(rect.width == 10000 && rect.height == 10000) {
	XClearWindow(XtDisplay(w),XtWindow(w));
	SetSize(w); /* for partially fix to sizing probem */
    }
    DrawRectangle(w,rect,False);

    if (self->crEdit.selectArea)
	DrawBorder(w, self->crEdit.seg, True);
}

/*
 * atc: True: selection-border drawing active, other no draw
 */
void CrEditBorderOff(Widget w)
{
    CrEditWidget self = (CrEditWidget)w;

    debug1("%s-CrEditBorderOff()\n",XtName(w));
    DrawBorder(w,self->crEdit.seg,False);
    BorderOff(w);
}

/*
 * member: 
 * rect  : 
 */
void CrEditSelect(Widget w,XRectangle rect)
{
    CrEditWidget self = (CrEditWidget)w;
    if(!w) return;


    debug4("CrEditRefresh() %dx%d+%d+%d\n",
	   rect.width,
	   rect.height,
	   rect.x,
	   rect.y
	   );
    /* for all */
    if(rect.width == 10000 && rect.height == 10000) {
	XClearWindow(XtDisplay(w),XtWindow(w));
	SetSize(w); /* for partially fix to sizing probem */
    }

    if (self->crEdit.selectArea) {
	DrawBorder(w, self->crEdit.seg, False);
	self->crEdit.seg.x1 = rect.x;
	self->crEdit.seg.y1 = rect.y;
	self->crEdit.seg.x2 = rect.x + rect.width;
	self->crEdit.seg.y2 = rect.y + rect.height;
	DrawBorder(w, self->crEdit.seg, True);
    }
}

/*** end of CrEdit.c ***/

/*
 *  CrossEdit - game world editor
 *  Copyright (C) 1993 Jarkko Sonninen & Petri Heinila
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  The authors can be reached via e-mail to Jarkko.Sonninen@lut.fi
 *  or Petri.Heinila@lut.fi .
 */

#include "Defines.h"
#include "global.h"

#include "Ansi.h"
#include "X11.h"

#include "CrList.h"
#include "CrFace.h"
#include "CrEdit.h"

#include "Edit.h"
#include "Attr.h"
#include "Cnv.h"
#include "Bitmaps.h"
#include "App.h"
#include "debug.h"

#include <proto.h>

Pixmap *pixmaps;	/* list of pixmaps */
Pixmap *masks;		/* list of masks */
int FontSize;		/* Size of font (really images) */
#define E_EDITABLE   (E_MONSTER | E_EXIT | E_TREASURE | E_BACKGROUND | \
                     E_DOOR | E_SPECIAL | E_SHOP | E_NORMAL | E_FALSE_WALL)

static ArchFlagsRec archFlags[] = {
    { 0, "toggle",	E_EDITABLE },
    { 0, "monster",	E_MONSTER },
    { 0, "exit",	E_EXIT },
    { 0, "treasure",	E_TREASURE },
    { 0, "background",	E_BACKGROUND },
    { 0, "door",	E_DOOR },
    { 0, "special",	E_SPECIAL },
    { 0, "shop",	E_SHOP },
    { 0, "normal",	E_NORMAL },
    { 0, "false wall",	E_FALSE_WALL },
    { 0, "wall",	E_WALL },
    { 0, "equipment",	E_EQUIPMENT },
    { 0, "other",	E_OTHER },
    { 0, "artifact",	E_ARTIFACT },
    { 0, NULL, 		0 },
};

/*
 * plaah: where to put these ???
 * 0.91.9 - moved to start of file - needed for ReadPixmaps function.
 */
XColor discolor[13];
Colormap colormap=(Colormap)NULL;


/**********************************************************************
 * private
 **********************************************************************/

static void AppDestroy(App self);
static Edit AppEditInsert(App self,String path,EditType type);
static void AppEditAttach(App self,Edit edit);

#if 0
static void AbsToCr(App self,String abs)
{
  char mapdir[PATH_MAX+1],path[PATH_MAX+1],*current,*filename;

  strcpy(path,abs);

  current = strlen(mapdir) + path;
  filename = strrchr(path,'/');
  *filename = 0;
  filename++;
  strcpy(self->path->current,current);
  strcpy(self->path->filename,filename);
}
#endif

static void Picks(XtPointer client,String entryPath)
{
  App self = (App)client; 
  String path;
  char mapdir[PATH_MAX+1];
  int i=0;

  sprintf(mapdir,"%s/%s",settings.datadir,settings.mapdir);
  for(path = entryPath; mapdir[i++] == *path++;);

  debug1("Picks() %s\n",path);
  AppEditInsert(self,path,Pick);
}

static void Walls(XtPointer client,String entryPath)
{
  App self = (App)client;
  String path;
  char mapdir[PATH_MAX+1];
  int i = 0;

  sprintf(mapdir,"%s/%s",settings.datadir,settings.mapdir);
  for(path = entryPath; mapdir[i++] == *path++;);

  debug1("Walls() %s\n",path);
  AppEditInsert(self,path,Wall);
}

static void Info(XtPointer client,String entryPath)
{
  App self = (App)client; 

  CnvBrowseShowFile(self->info,entryPath);
}


/**********************************************************************
 * select 
 **********************************************************************/

/*
 * member: filter archetype list
 * at    : requested archetype
 * return: True, if show out
 */
static Boolean AppArchFilter (App self,archetype * at)
{
    if(self->arch.all) return True;
    if (!at->editable) return False;
    return (self->arch.flags & at->editable) ? True : False;
}

/*
 * function: give widget head of list
 */
static CrListNode Next(XtPointer client,XtPointer call)
{
  App self = (App)client;
  CrListNode retNode = (CrListNode)call;
  static struct _CrListNode node;
  archetype *at;


  if(retNode) {
      at = ((archetype *)retNode->ptr)->next;
  } else { /* begin */
      at = first_archetype;
  }

  while (at && !AppArchFilter(self,at))
      at = at->next;  
  
  if(at) {
    node.face = at->clone.face; 
    node.name = at->name;
    node.ptr = (XtPointer)at;
    return &node;
  }
  return (CrListNode)NULL;
}

/* 
 * create attributes from inventory
 */
static void SelectCb(Widget w,XtPointer client,XtPointer call)
{
    CrListCall ret = (CrListCall)call;
    App self = (App)client;

    debug0 ("SelectCb\n");
    /* self->item.wall_map = NULL; */
    AppItemSet (self,NULL,& ((archetype *) ret->node)->clone,0);
    /*
    if(self->attr.isup) {
	AttrSetArch(&self->attr,self->item.at);
    }
    */
    return;
}

/**********************************************************************
 * look 
 **********************************************************************/

/*
 * function: give widget head of list
 */
static CrListNode lookNext(XtPointer client,XtPointer call)
{
    App self = (App)client;
    CrListNode retNode = (CrListNode)call;
    static struct _CrListNode node;
    object *op = NULL;

    if(self->look.edit == NULL) return NULL;
   
    if(retNode) { /* next */
	op = ((object *)retNode->ptr)->below;
    } else {
	/* begin */
	op = MapGetObjectZ(self->look.edit->emap,
			   self->look.rect.x,
			   self->look.rect.y,0);
    }
    if(op) {
	node.face = op->face; 
	node.name = op->name;
	node.ptr = (XtPointer)op;
	    return &node;
    }
    return (CrListNode)NULL;
}

/* 
 * 
 */
static void lookSelectCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    CrListCall ret = (CrListCall)call;
    object *ob;

    ob = ret->node;
    if (ob->head)
	ob = ob->head;
    if(!self->attr) {
	self->attr = AttrCreate 
	    ("attr", self, ob, AttrDescription, GetType(ob), self->look.edit);
    } else {
	AttrChange(self->attr,ob, GetType(ob), self->look.edit);
    }
    return;
}

/*
 * callback: insert object to look window
 */
static void lookInsertCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    CrListCall ret = (CrListCall)call;

    EditInsert (self->look.edit, 
		self->look.rect.x, self->look.rect.y, ret->index);
    return;
}

/*
 * callback: delete object from look window
 */
static void lookDeleteCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    CrListCall ret = (CrListCall)call;
    EditDelete (self->look.edit, self->look.rect.x, self->look.rect.y, 
		    ret->index);
    return;
}
/**********************************************************************
 * File-menu
 *   New
 *   Open
 *   CrossFire
 *   Quit
 **********************************************************************/

static void AppNewCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    AppEditInsert(self,NULL,Regular);
}

#if 0
static void AppClipCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    AppEditInsert(self,NULL,ClipBoard);
}
#endif


static void AppOpenCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    char path[PATH_MAX+1];
    if(CnvPathSelect(self->path) != CnvPathOk) return;
    sprintf(path,"%s/%s",self->path->current,self->path->filename);
    AppEditInsert(self,path,Regular);
}

static void AppCrossfireCb  (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    int pid;
    char *prog,*arg1;

    if(!self->res.cmdCrossfire) {
	CnvNotify("No command defined to *cmdCrossfire:","Continue",NULL);
	return;
    }
    prog = strtok(self->res.cmdCrossfire," \t");
    arg1 = strtok(NULL," \t"); /* kludge for -pix */
    switch(pid = fork()) {
    case -1:
	CnvNotify("Cannot open new process","OK",NULL);
	return;
    case 0: /* child */
	if(execlp(prog,prog,arg1,NULL) == -1) {
	    char buf[BUFSIZ];
	    sprintf(buf,"cannot execute \"%s\"",self->res.cmdCrossfire);
	    CnvWarn(self->shell,buf);
	    exit(0);
	}
    default: /* parent */
	debug2("CbMainCrossfire() %d %s started\n",pid,self->res.cmdCrossfire);
	return;
    }
}

/*
 *
 */
static void AppQuitCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;

    AppDestroy(self);
    XtDestroyApplicationContext (XtWidgetToApplicationContext (w));
    exit (0);
}

/*
 * file-menu definition
 */
static CnvMenuRec fileMenu[] = {
    {"new"      ,AppNewCb},
    {"open"     ,AppOpenCb},
#if 0
    {"clipboard",AppClipCb}, 
#endif
    {"-----"    ,NULL},
    {"crossfire",AppCrossfireCb},
    {"-----"    ,NULL},
    {"quit"     ,AppQuitCb},
    {""         ,NULL}
}; 

/**********************************************************************
 * toggle-menu
 **********************************************************************/

static void dirtyfixscrollbar (Widget w) {
    Widget s = XtParent (w);
    XawViewportSetCoordinates (s, 0, 0);
    /* undocumented function */
}


static void ToggleFlagCb(Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    int i;

    for (i = 0; archFlags[i].name; i++) {
	if (archFlags[i].w == w) {
	    self->arch.flags ^= archFlags[i].flags;
	    break;
	}
    }
    dirtyfixscrollbar (self->arch.w);
    XtVaSetValues (self->arch.w, XtNpackage, self, NULL);
    for (i = 0; archFlags[i].name; i++) {
	XtVaSetValues
	    (archFlags[i].w,
	     XtNleftBitmap, 
	     (self->arch.flags & archFlags[i].flags) ? bitmaps.mark : None,
	     NULL);
    }
    return;
}


static void ToggleAllCb(Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;

    self->arch.all = !self->arch.all;
    dirtyfixscrollbar (self->arch.w);

    XtVaSetValues (self->arch.w, XtNpackage, self, NULL);
    XtVaSetValues(w,
		  XtNleftBitmap, 
		  self->arch.all ? bitmaps.mark : None,
		  NULL);
}

static void ToggleClipCb(Widget w, XtPointer client, XtPointer call)
{
  App self = (App)client;

  self->clipon = !self->clipon;
  XtVaSetValues(w,
		XtNleftBitmap, 
		self->clipon ? bitmaps.mark : None,
		NULL);
  if(self->clipon)
      XtPopup(self->clip->shell,XtGrabNone);
  else
      XtPopdown(self->clip->shell);
}

static void AppToggleMenu(App self,String name,Widget parent)
{
    Widget shell,entry;
    int i;

    shell = XtVaCreatePopupShell
	(name,simpleMenuWidgetClass,parent,
	 NULL);
    
    entry = XtVaCreateManagedWidget
	("all",smeBSBObjectClass,shell,
	 XtNleftBitmap,
	 self->arch.all ? bitmaps.mark : None,
	 NULL);
    XtAddCallback(entry,XtNcallback,ToggleAllCb,(XtPointer)self);
    
    for (i = 0; archFlags[i].name; i++) {
	archFlags[i].w = XtVaCreateManagedWidget
	    (archFlags[i].name, smeBSBObjectClass,shell,
	     XtNleftBitmap,
	     (self->arch.flags  & archFlags[i].flags) ? 
	     bitmaps.mark : None,
	     NULL);
	XtAddCallback
	    (archFlags[i].w,XtNcallback,ToggleFlagCb,(XtPointer)self);
    }

    XtVaCreateManagedWidget ("line", smeLineObjectClass, shell, NULL);
    entry = XtVaCreateManagedWidget
	("clipboard",smeBSBObjectClass,shell,
	 XtNleftBitmap,
	 self->clipon ? bitmaps.mark : None,
	 NULL);
    XtAddCallback(entry,XtNcallback,ToggleClipCb,(XtPointer)self);
}

/**********************************************************************
 * editMenu callbacks (cut,copy,paste,...)
 **********************************************************************/

/* 
 * callback: Cut
 */
static void CutCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    XRectangle rect;

    debug0 ("AppCutCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select area to Cut","Continue",NULL);
	return;
    }
    EditResizeScroll(self->clip,self->look.rect.width,
		     self->look.rect.height,0,0);
    /*
    EditCopyRectangle(self->look.edit,self->clip,EditRectAll,
		      self->look.rect.x,self->look.rect.y);
		      */
    rect.x = rect.y = 0;
    rect.width = self->look.rect.width;
    rect.height = self->look.rect.height;
    EditWipeRectangle(self->clip,rect);
    EditCopyRectangle(self->clip,self->look.edit,self->look.rect,0,0);
    EditWipeRectangle(self->look.edit,self->look.rect);

    EditModified(self->look.edit);
    CrEditRefresh(self->look.edit->w,self->look.rect);
    AppSelectUnset(self);
}

/* 
 * callback: Copy
 */
static void CopyCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    XRectangle rect;
    
    debug0 ("AppCopyCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select area to Copy","Continue",NULL);
	return;
    }
    EditResizeScroll(self->clip,self->look.rect.width,
		     self->look.rect.height,0,0);
    rect.x = rect.y = 0;
    rect.width = self->look.rect.width;
    rect.height = self->look.rect.height;
    EditWipeRectangle(self->clip,rect);
    EditCopyRectangle(self->clip,self->look.edit,self->look.rect,0,0);

    AppSelectUnset(self);
}

/* 
 * callback: Paste
 */
static void PasteCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
   
    debug0 ("AppPasteCb()\n");

    if(!self->look.edit) {
	CnvNotify("Select point to Paste","Continue",NULL);
	return;
    }
    EditCopyRectangle(self->look.edit,self->clip,EditRectAll,
		      self->look.rect.x,self->look.rect.y);

    EditModified(self->look.edit);

    AppSelectUnset(self);
}

/* 
 * callback: Fill
 */
static void FillCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    
    debug0 ("AppFillCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select point to Fill","Continue",NULL);
	return;
    }
    EditPerformFill(self->look.edit,self->look.rect.x,self->look.rect.y);
    EditModified(self->look.edit);
    AppSelectUnset(self);
}

/* 
 * callback: FillBelow
 */
static void FillBelowCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    
    debug0 ("AppFillCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select point to Fill","Continue",NULL);
	return;
    }
    EditPerformFillBelow(self->look.edit,self->look.rect.x,self->look.rect.y);
    EditModified(self->look.edit);
    AppSelectUnset(self);
}

/* 
 * callback: Box
 */
static void BoxCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;

    debug0 ("AppBoxCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select area to fill","Continue",NULL);
	return;
    }
    EditFillRectangle(self->look.edit, self->look.rect);
    EditModified(self->look.edit);
    AppSelectUnset(self);
}

/* 
 * callback: Wipe
 */
static void WipeCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    
    debug0 ("AppWipeCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select area to Wipe","Continue",NULL);
	return;
    }
    
    EditShaveRectangle(self->look.edit, self->look.rect);
    EditModified(self->look.edit);
    AppSelectUnset(self);
}

/* 
 * callback: WipeBelow
 */
static void WipeBelowCb (Widget w, XtPointer client, XtPointer call)
{
    App self = (App)client;
    
    debug0 ("AppWipeBelowCb()\n");
    if(!self->look.edit) {
	CnvNotify("Select area to Wipe","Continue",NULL);
	return;
    }
    
    EditShaveRectangleBelow(self->look.edit, self->look.rect);
    EditModified(self->look.edit);
    AppSelectUnset(self);
}

/*
 * menu definition
 */
static CnvMenuRec editMenu[] = {
    {"cut"  ,CutCb},
    {"copy" ,CopyCb},
    {"paste",PasteCb},
    {"-----",NULL},
    {"fill" ,FillCb},
    {"fillbelow" ,FillBelowCb},
    {"box" ,BoxCb},
    {"wipe" ,WipeCb},
    {"wipebelow" ,WipeBelowCb},
    {"",     NULL}
}; 

/**********************************************************************
 * layout 
 **********************************************************************/

/*
 * member: create application window layout 
 */
static void Layout(App self)
{
    Widget pane, box,use,view;
    char path[PATH_MAX+1];
    
    /*** vertical Pane of widgets ***/
    pane = XtCreateManagedWidget 
	("pane", panedWidgetClass, self->shell, 
	 NULL, 0);
    
    /*** menubar ***/
    box = XtVaCreateManagedWidget 
	("box", boxWidgetClass, pane,
	 XtNorientation, XtorientHorizontal,
	 NULL);
    use = XtVaCreateManagedWidget 
      ("fileButton",menuButtonWidgetClass, box,
       XtNmenuName,"appFileMenu",
       NULL);
    CnvMenu("appFileMenu",use,fileMenu,(XtPointer)self);
 
    use = XtVaCreateManagedWidget 
	("infoButton",menuButtonWidgetClass, box,
	 XtNmenuName,"info",
	 NULL);
    sprintf(path,"%s/%s",settings.datadir,"doc");
    self->infof = CnvFilesCreate("info",use,Info,(XtPointer)self,path);
    
    /*** look ***/
    self->look.info = XtVaCreateManagedWidget
	("info",labelWidgetClass,pane,
	 NULL);

    view = XtVaCreateManagedWidget 
	("view", viewportWidgetClass,pane,
	 NULL);
    self->look.w = XtVaCreateManagedWidget 
	("cross",crListWidgetClass,view, 
	 XtNpackage, self,
	 XtNnext, lookNext,
	 NULL);
    XtAddCallback(self->look.w,XtNinsertCallback,lookInsertCb,
		  (XtPointer)self);
    XtAddCallback(self->look.w,XtNselectCallback,lookSelectCb,
		  (XtPointer)self);
    XtAddCallback(self->look.w,XtNdeleteCallback,lookDeleteCb,
		  (XtPointer)self);
    
    /*** arch ***/
    box = XtVaCreateManagedWidget 
	("box", boxWidgetClass, pane, 
	 XtNorientation, XtorientHorizontal,
	 NULL);
    use = XtVaCreateManagedWidget 
	("archButton",menuButtonWidgetClass, box,
	 XtNmenuName,"toggle",
	 NULL);
    AppToggleMenu(self,"toggle",use);
    use = XtVaCreateManagedWidget 
	("pickButton",menuButtonWidgetClass, box,
	 XtNmenuName,"picks",
	 NULL);
    sprintf(path,"%s/%s/%s",settings.datadir,settings.mapdir,"editor/picks");
    self->picks = CnvFilesCreate("picks",use,Picks,(XtPointer)self,path);
    use = XtVaCreateManagedWidget 
      ("wallButton",menuButtonWidgetClass, box,
       NULL);
    sprintf(path,"%s/%s/%s",settings.datadir,settings.mapdir,"editor/walls");
    self->walls = CnvFilesCreate("menu",use,Walls,(XtPointer)self,path);

    use = XtVaCreateManagedWidget 
	("arch", viewportWidgetClass, pane, 
	 NULL);
    self->arch.w = XtVaCreateManagedWidget 
	("cross",crListWidgetClass,use, 
	 XtNpackage, self,
	 XtNnext, Next,
	 NULL);
    XtAddCallback(self->arch.w,XtNselectCallback,SelectCb,(XtPointer)self);
#if 0
    XtAddCallback(self->arch.w,XtNdeleteCallback,SelectCb,(XtPointer)self);
    XtAddCallback(self->arch.w,XtNinsertCallback,SelectCb,(XtPointer)self);
#endif

    /*** item ***/
    box = XtVaCreateManagedWidget 
	("item", formWidgetClass, pane, 
	 XtNorientation, XtorientVertical,
	 NULL);
    self->item.name = XtVaCreateManagedWidget
	("name",labelWidgetClass,box,
	 NULL);
    self->item.face = XtVaCreateManagedWidget
	("face",crFaceWidgetClass,box,
	 XtNfromVert, self->item.name,
	 NULL);

    /*** used on other places ***/
    self->look.menu = CnvMenu        /* cut,copy,paste-menu on editors */
	("mapEdit",self->shell,editMenu,(XtPointer)self);
    self->info = CnvBrowseCreate("infoFile",self->shell, NULL);
    /* browsing text */
}

/**********************************************************************
 * public
 **********************************************************************/

/*
 * member       : create one application main window
 * appCon       : on self Xt application context
 * displayString: on self display
 * argc         : number of command line params
 * argv         : list of command line params
 */
App AppCreate(XtAppContext appCon,
	      String displayString,
	      XtResource resources[],
	      Cardinal resourcesNum,
	      XrmOptionDescRec *options,
	      Cardinal optionsNum,
	      int *argc,
	      char *argv[])
{
    char buf[BUFSIZ];
    char path[PATH_MAX+1];
    App self;


    /*** initialize ***/
    self = (App)XtMalloc(sizeof(struct _App));
    memset(self,0,sizeof(struct _App));
    self->display = XtOpenDisplay
	(appCon,
	 NULL,NULL,AppClass,
	 options,optionsNum,
	 argc,argv);
    if(!self->display) {
	sprintf(buf,"Cannot open display %s",displayString);
	XtAppError(appCon,buf);
	exit(EXIT_FAILURE);
    }

    BitmapsCreate(self->display); 
    self->shell = XtVaAppCreateShell
	(NULL,AppClass,
	 applicationShellWidgetClass,
	 self->display,
	 XtNtitle,AppClass,
	 XtNiconName,AppClass,
	 XtNiconPixmap,bitmaps.edit,
	 NULL);
    self->attr = NULL;
    self->edit = NULL;
    self->clip = NULL;
    self->clipon = 0;
    self->path = NULL;
    self->look.edit = NULL;
    self->arch.flags= E_EDITABLE;
    self->arch.all = 0;
    self->item.clone = NULL;
    self->item.wall_map = NULL;
    self->item.edit = NULL;

    /*** ***/
    XtGetApplicationResources 
	(self->shell, 
	 (XtPointer) & self->res,
	 resources, resourcesNum, 
	 NULL, 0);

    /* Default */
    displaymode=Dm_Png;
    FontSize=32;

    CnvInitialize(self->shell);


    /*** creating ***/
    sprintf(path,"%s/%s",settings.datadir,settings.mapdir);
    self->path = CnvPathCreate("fileSelect",path, "");
    self->clip = EditCreate(self,ClipBoard,"/Clipboard"); /* separate from */
    Layout(self);
    XtRealizeWidget (self->shell);
    XtRealizeWidget(self->clip->shell);
    /* I move this down here because I want all the widgets to get their
     * colors before the images hog all of them.
     */
    colormap = DefaultColormap(self->display, DefaultScreen(self->display));
    if (ReadImages(self->display, &pixmaps, &masks, &colormap, displaymode)) {
	    /* We really should do something better than this */
	    fprintf(stderr,"Not enough space in colormap - switch colormap.\n");
/*	    exit(1);*/
    }
    if (colormap)
	XtVaSetValues(self->shell, XtNcolormap, colormap, NULL);
    AppUpdate(self);
    return self;
}

/*
 * member: vanish application
 */
static void AppDestroy(App self)
{
    Edit edit;
    Edit temp;

    debug0("AppDestroy()");
    /*
    XUnloadFont (XtDisplay (self->shell), self->font);
    XtReleaseGC (self->shell, self->gc);
    XtReleaseGC (self->shell, self->text_gc);
    */

    if(self->attr) {
	AttrDestroy(self->attr);
    }
    self->attr = NULL;
    edit = self->edit;
    while(edit) {
      temp = edit->next;
      EditDestroy(edit);
      edit = temp;
    }
}

/*
 *
 *Description:
 * update all things 
 */
void AppUpdate(App self)
{
    char buf[BUFSIZ];
    object *obj;

    debug0 ("AppUpdate()\n");
    /*** look ***/
    XtVaSetValues (self->look.w, XtNpackage, self, NULL); /*** pseudo ***/
    if(self->look.edit) {
	sprintf(buf,"Look: %dx%d+%d+%d",
		self->look.rect.width,
		self->look.rect.height,
		self->look.rect.x,
		self->look.rect.y);

#if 0
	if (self->attr) {
	    object *ob = MapGetObjectZ (self->look.edit->emap, 
					self->look.rect.x, 
					self->look.rect.y,0);
	    AttrChange(self->attr, ob, GetType(ob));
	}
#endif
    } else {
	sprintf(buf,"Look: (no map)");
    }
    XtVaSetValues(self->look.info,
		  XtNlabel,buf,
		  NULL);

    /*** item ***/
    if((obj = AppItemGetObject(self)) && obj->arch) {
	if(AppItemGetMap(self)) {
	    XtVaSetValues(self->item.name,
			  XtNlabel,"(auto-joining)",
			  NULL);
	    if((obj = get_map_ob(AppItemGetMap(self),0,
				AppItemGetWall(self)))) {
		XtVaSetValues(self->item.face,
			      XtNobject,
			      obj,
			      NULL);
	    } else {
		debug0("App-No object to show\n");
	    }
	} else {
	    XtVaSetValues(self->item.name,
			  XtNlabel,AppItemGetObject(self)->arch->name,
			  NULL);
	    XtVaSetValues(self->item.face,
			  XtNobject, AppItemGetObject(self),
			  NULL);
	}
    } else {
	XtVaSetValues(self->item.name,
		      XtNlabel, "Not Selected",
		      NULL);
	XtVaSetValues(self->item.face,
		      XtNobject,NULL,
		      NULL);
    }
}

/*
 *
 */
void AppSelectSet(App self,Edit edit,XRectangle rect)
{
    if(edit == NULL) {
	debug0("AppSelectSet() WARN try set NULL Edit\n");
	return;
    }
    if(self->look.edit && self->look.edit != edit)
	CrEditBorderOff(self->look.edit->w);
    self->look.edit = edit;
    self->look.rect = rect;
    AppUpdate(self);
}

/*
 *
 */
void AppSelectUnset(App self)
{
    if (self->look.edit == NULL) return;
    CrEditBorderOff(self->look.edit->w);
    CrEditRefresh(self->look.edit->w,self->look.rect);
    self->look.edit = NULL;
    AppUpdate(self);
}

/* 
 * member : item - window to show inserted archetype
 * edit   : object is selectted from self map, NULL if from
 *          main window or no selection
 * obj    : self object is selected to cloning
 * wallSet:
 */
void AppItemSet (App self, Edit edit,object *obj,int wallSet)
{
    self->item.edit = NULL;
    self->item.wall_map = NULL;
    self->item.clone = NULL;
    self->item.wall_set = AppItemNoWall;

    if(edit) {             /* from map */
	self->item.edit = edit;
	if(wallSet > AppItemNoWall) { /* wall map */
	    self->item.wall_map = edit->emap;
	    self->item.wall_set = wallSet;
	    self->item.clone = get_map_ob(edit->emap,0,wallSet);
	} else {           /* other */
	    self->item.clone = obj;
	}
    } else if(obj) {               /* from main window */
	self->item.clone = obj;
    } else {
	/* debug("AppItemSet() strange selection\n"); */
/*	return;*/
    }
    AppUpdate(self);
}



/*
 * member: add editor
 */
static Edit AppEditInsert(App self,String path,EditType type)
{
    Edit edit;
    Edit editor;

    /*** check if exist ***/
    /* Dragon Master */
    for(editor = self->edit; editor; editor = editor->next)
      if(path != NULL && !strcmp(editor->emap->path, path)) {
	/*** save, if modified ***/
	if (editor->modified) {
	  switch (CnvNotify ("Map modified, discard changes?",
			     "OK","Save Changes","Cancel",NULL)) {
	    case 1:
	      EditLoad(editor);
	      break;
	    case 2:
	      EditSave(editor);
	      break;
	    default:
	      break;
	  }
	}
	XRaiseWindow(editor->app->display, XtWindow(editor->shell));
	return editor;
      }

    /*** create new one ***/
    if((edit = EditCreate(self,type,path)) == NULL) {
	return NULL;
    }
    AppEditAttach(self,edit);
    return edit;
}

/*
 * attach Edit to App environment
 */
static void AppEditAttach(App self,Edit edit)
{
    debug1("AppEditAttach() %s\n",EditGetPath(edit));
    /*** attach edit to list ***/
    edit->next = self->edit;
    self->edit = edit;

    edit->app = self;
}

/*
 * deattach edit from list
 */
void AppEditDeattach(App self,Edit edit)
{
    Edit oldPrev,old;
    debug1("AppEditdeattach() %s\n",EditGetPath(edit));

    oldPrev = old = NULL;
    for(old = self->edit;
	old && old != edit;
	oldPrev = old, old = old->next)
	debug1("AppEditDeattach() %s\n",EditGetPath(old));

    if(!old) {
	CnvWarn(self->shell,"Trying delete unlinked edit");
    } else {
	if(!oldPrev) { /* first */
	    self->edit = old->next;
	} else {
	    oldPrev->next = old->next;
	}
    }
    /* edit->app = NULL; */
}

/*** end of App.c ***/

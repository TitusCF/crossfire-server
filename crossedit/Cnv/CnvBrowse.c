#include <Cnv.h>

/**********************************************************************
 * widget
 **********************************************************************/

static void CancelCb(Widget w,XtPointer client,XtPointer call)
{
  CnvBrowse self = (CnvBrowse)client;
  XtPopdown(self->shell);
  self->isup = False;
}

static void OkCb(Widget w,XtPointer client,XtPointer call)
{
  CnvBrowse self = (CnvBrowse)client;
  String str;

  XtPopdown(self->shell);
  XtUnmanageChild(self->ok);
  self->isup = False;
  XtVaGetValues(self->text,
		XtNstring,&str,
		NULL);
  (*self->proc)(self,client,str);
}

/**********************************************************************
 * privates
 **********************************************************************/

static void Layout(CnvBrowse self,String name,Widget parent)
{
  Widget vpaned,hbox,cancel;

  self->shell = XtVaCreatePopupShell
      (name,topLevelShellWidgetClass,parent,
       NULL);
  vpaned = XtVaCreateManagedWidget
      ("vpaned",panedWidgetClass,self->shell,
       XtNorientation,XtorientVertical,
       NULL);
  hbox = XtVaCreateManagedWidget
      ("hbox",boxWidgetClass,vpaned,
       XtNorientation,XtorientHorizontal,
       XtNshowGrip,False,
       NULL);

  cancel = XtVaCreateManagedWidget
      ("cancel",commandWidgetClass,hbox,
       NULL);
  XtAddCallback(cancel,XtNcallback,CancelCb,(XtPointer)self);

  self->ok = XtVaCreateWidget
      ("ok",commandWidgetClass,hbox,
       NULL);
  XtAddCallback(self->ok,XtNcallback,OkCb,(XtPointer)self);

  self->file = XtVaCreateWidget
      ("file",labelWidgetClass,hbox,
       NULL);
  self->text = XtVaCreateManagedWidget
      ("text",asciiTextWidgetClass,vpaned,
       XtNeditType,XawtextEdit,
       NULL);
}

static void CnvBrowseShow(CnvBrowse self)
{
  XtUnmanageChild(self->file);
  if(self->isup == False) XtPopup(self->shell,XtGrabNone);
}

/**********************************************************************
 * members
 **********************************************************************/

/*
 * parent:
 * byName:
 */
CnvBrowse CnvBrowseCreate
(String name,Widget parent,CnvBrowseEditProc proc)
{
    CnvBrowse self;
    
    /* alloc & init state */
    self = (CnvBrowse)XtMalloc(sizeof(struct CnvBrowse));
    self->shell = NULL;
    self->text = NULL;
    self->file = NULL;
    self->isup = False;
    self->proc = proc;
   
    Layout(self,name,parent);
    return self;
}

/*
 *
 */
void CnvBrowseDestroy(CnvBrowse self)
{
  XtDestroyWidget(self->shell);
  XtFree((char*)self);
}

/*
 * show file
 */
void CnvBrowseShowFile(CnvBrowse self,String file)
{
    if(!(self && file)) return;
    CnvBrowseShow(self);  
    XtManageChild(self->file);
    XtVaSetValues(self->text,
		  XtNtype,XawAsciiFile,
		  XtNstring,file,
		  XtNeditType,XawtextRead,
		  NULL);
    XtVaSetValues(self->file,
		  XtNlabel,file,
		  NULL);
}

/*
 * show string
 */
void CnvBrowseShowString(CnvBrowse self,String str)
{
    if(!(self && str)) return;
    CnvBrowseShow(self);
    XtVaSetValues(self->text,
		  XtNtype,XawAsciiString,
		  XtNstring,str,
		  XtNeditType,XawtextRead,
		  NULL);
}

/*
 * edit string, finish by callback
 */
void CnvBrowseEditString(CnvBrowse self,String str)
{
    if(!(self && str && self->proc)) return;
    XtManageChild(self->ok);
    CnvBrowseShow(self);
    XtVaSetValues(self->text,
		  XtNtype,XawAsciiString,
		  XtNstring,str,
		  XtNeditType,XawtextEdit,
		  NULL);
}

/*** end of CnvBrowse.c ***/

#include "Cnv.h"
#include <debug.h>

typedef struct App {
    Widget shell;
    XtAppContext con;
    CnvBrowse CnvBrowse;
    CnvFiles files;
    CnvPath path;
} *App;

static void QuitCb(Widget w,XtPointer client,XtPointer call)
{
    XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
    exit(0);
}

static void CnvBrowseFileCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    debug0("CnvBrowseFileCb()\n");
    CnvBrowseShowFile(self->CnvBrowse,"test.txt");
}

static void CnvBrowseStringCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    debug0("CnvBrowseStringCb()\n");
    CnvBrowseShowString(self->CnvBrowse,"Joopa joo");
}

static void CnvBrowseEditStringCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    debug0("CnvBrowseEditStringCb()\n");
    CnvBrowseEditString(self->CnvBrowse,"Joopa jee");
}

void EditOk(CnvBrowse b,XtPointer client,String str)
{
    debug0("EditOk()\n");
    printf("%s\n",str);
}

static void CnvPathCb(Widget w,XtPointer client,XtPointer call)
{
    App self = (App)client;
    char path[PATH_MAX+1];

    debug0("CnvPathCb()\n");
    CnvPathSelectAbs(self->path,path);
    debug1("CnvPathCb() %s\n",path);
}

static void CnvNotifyCb(Widget w,XtPointer client,XtPointer call)
{

    debug0("CnvNotifyCb()\n");
    switch(CnvNotify("Test here","OK","Cancel","Help",NULL)) {
    case 1:
	debug0("OK selected\n");
	break;
    case 2:
	debug0("Cancel selected\n");
	break;
    case 3:
	debug0("Help selected\n");
	break;
    default:
	debug0("Error\n");
    }
}

static void CnvPromptCb(Widget w,XtPointer client,XtPointer call)
{

    char ans[CnvPromptMax+1];

    switch(CnvPrompt("Test here","joo",ans,"OK","Cancel","Help",NULL)) {
    case 1:
	debug0("OK selected\n");
	break;
    case 2:
	debug0("Cancel selected\n");
	break;
    case 3:
	debug0("Help selected\n");
	break;
    default:
	debug0("Error\n");
    }
    debug1("CnvPromptCb() %s prompted\n",ans);
}

static CnvMenuRec mainMenu[] = {
    {"notify",CnvNotifyCb},
    {"prompt",CnvPromptCb},
    {"---",   NULL},
    {"quit",  QuitCb},
    {"",      NULL}
};

void CnvFilesCb(XtPointer client,String path)
{
    debug1("CnvFilesCb() %s\n",path);
}

static void Layout(App self,Display *display)
{
    Widget cont,use;

    self->shell = XtVaAppCreateShell
	("test","Test",applicationShellWidgetClass,
	 display,
	 NULL);
    CnvInitialize(self->shell);

    cont = XtVaCreateManagedWidget
	("cont",boxWidgetClass,self->shell,
	 NULL);

    use = XtVaCreateManagedWidget
	("menu",menuButtonWidgetClass,cont,
	 XtNmenuName,"mainMenu",
	 NULL);
    CnvMenu("mainMenu",use,mainMenu,(XtPointer)self);

    use = XtVaCreateManagedWidget
	("files",menuButtonWidgetClass,cont,
	 XtNmenuName,"filesMenu",
	 NULL);
    self->files = CnvFilesCreate("filesMenu",use,CnvFilesCb,(XtPointer)self,".");

    use = XtVaCreateManagedWidget
	("cnvBrowseFile",commandWidgetClass,cont,
         NULL);
    XtAddCallback(use,XtNcallback,CnvBrowseFileCb,(XtPointer)self);

    use = XtVaCreateManagedWidget
        ("cnvBrowseString",commandWidgetClass,cont,
         NULL);
    XtAddCallback(use,XtNcallback,CnvBrowseStringCb,(XtPointer)self);

    use = XtVaCreateManagedWidget
        ("cnvBrowseEdit",commandWidgetClass,cont,
         NULL);
    XtAddCallback(use,XtNcallback,CnvBrowseEditStringCb,(XtPointer)self);

    use = XtVaCreateManagedWidget
        ("cnvPath",commandWidgetClass,cont,
         NULL);
    XtAddCallback(use,XtNcallback,CnvPathCb,(XtPointer)self);

    self->CnvBrowse = CnvBrowseCreate("cnvBrowse",cont,EditOk);
    self->path = CnvPathCreate("cnvPath",".",".");
}

int main(unsigned int argc,char *argv[])

{
    struct App app;
    Display *display;

    XtToolkitInitialize();
    app.con = XtCreateApplicationContext();
    display = XtOpenDisplay
	(app.con,
	 NULL,"Test",NULL, /* display,name,class */
	 NULL,0,           /* options */
	 (signed int *) &argc,argv);
    Layout(&app,display);

    XtRealizeWidget(app.shell);
    XtAppMainLoop(app.con);
    return 0;
}

/*** end of test.c ***/

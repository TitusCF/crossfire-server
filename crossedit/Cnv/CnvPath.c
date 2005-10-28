/*
 * CnvPath - file selector
 * Copyright (C) 1993 Petri Heinila
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Author can be connected by email from hevi@lut.fi
 */

#include <Cnv.h>
#include <debug.h>
#include <assert.h>

/**********************************************************************
 * declarations
 **********************************************************************/

static void CnvPathListInit(CnvPath p);
static void CnvPathListGet(CnvPath p,String path);
static void CnvPathEnter(CnvPath self);
static Boolean hasActions = False;
static CnvPath tmpCnvPath = NULL;

/**********************************************************************
 * actions
 **********************************************************************/

static void CnvPathReturn(Widget w,XEvent *e,String *argv, Cardinal *argc);

static XtActionsRec actions[] = {
  {"CnvPathReturn", CnvPathReturn}  /* when user accept */
};

static void CnvPathReturn(Widget w,XEvent *e,String *argv, Cardinal *argc)
{
    CnvPath self = (CnvPath)tmpCnvPath;
    assert(self);
    CnvPathEnter(self);
}

/**********************************************************************
 * widgets
 **********************************************************************/

static void CnvPathCancelCb(Widget w,XtPointer client,XtPointer call)
{
  CnvPath p = (CnvPath)client;
  p->hold = CnvPathCancel;
}

static void CnvPathEnterCb(Widget w,XtPointer client,XtPointer call)
{
  CnvPath self = (CnvPath)client;
  CnvPathEnter(self);
}

static void CnvPathSelectFileCb(Widget w,XtPointer client,XtPointer call)
{
  CnvPath p = (CnvPath)client;
  XawListReturnStruct *ret = (XawListReturnStruct*)call;

  XtVaSetValues(p->text,
		XtNstring,ret->string,
		NULL);
}

static void CnvPathSelectDirCb(Widget w,XtPointer client,XtPointer call)
{
  CnvPath p = (CnvPath)client;
  XawListReturnStruct *ret = (XawListReturnStruct*)call;
  char path[PATH_MAX+1];

  if(strcmp(ret->string,"..") == 0) {
    char *str;
    pathcpy(path,p->current);
    str = path + strlen(path);
    while(*str != '/' && str > path) str--;
    *str = 0;
  } else   if(strcmp(ret->string,".") == 0) {
    sprintf(path,"%s",p->current);        
  } else {
    sprintf(path,"%s/%s",p->current,ret->string);    
  }
  p->hold = CnvPathHold;
  CnvPathListGet(p,path);
}

/**********************************************************************
 * Layout
 **********************************************************************/

static void Layout(CnvPath self,String name)
{
  Widget cont,cancel,fileView,dirView,ok;

  self->shell = XtVaCreatePopupShell
    (name,transientShellWidgetClass,cnv->shell,NULL);
  cont = XtVaCreateManagedWidget
    ("cont",formWidgetClass,self->shell,
     NULL);
  self->cwd = XtVaCreateManagedWidget
    ("cwd",labelWidgetClass,cont,
     /*
     XtNfromHoriz,NULL,
     XtNfromVert,NULL,
     */
     NULL);
  self->text = XtVaCreateManagedWidget
    ("text",asciiTextWidgetClass,cont,
     XtNtype,XawAsciiString,
     XtNeditType,XawtextEdit,
     XtNfromHoriz,self->cwd,
     XtNfromVert,NULL,
     XtNaccelerators,XtParseAcceleratorTable
     ("#override <Key>Return: CnvPathReturn() \n"),
     XtNtranslations,XtParseTranslationTable
     ("#override <Key>Tab: CnvNop()\n"
      "<Key>Return: CnvPathReturn() \n"),
     NULL);
  fileView = XtVaCreateManagedWidget
    ("fileView",viewportWidgetClass,cont,
     XtNfromHoriz,NULL,
     XtNfromVert,self->cwd,
     NULL);
  self->fileList = XtVaCreateManagedWidget
    ("fileList",listWidgetClass,fileView,
     XtNtranslations,XtParseTranslationTable
     ("#override <Btn1Up>(2): Set() Notify() CnvPathReturn()\n" 
      "<Btn1Down>: Set() Notify() \n"),
     NULL);
  XtAddCallback(self->fileList,XtNcallback,CnvPathSelectFileCb,
		(XtPointer)self);
  dirView = XtVaCreateManagedWidget
    ("dirView",viewportWidgetClass,cont,
     XtNfromHoriz,fileView,
     XtNfromVert,self->cwd,
     NULL);
  self->dirList = XtVaCreateManagedWidget
    ("dirList",listWidgetClass,dirView,
     NULL);
  XtAddCallback(self->dirList,XtNcallback,CnvPathSelectDirCb,(XtPointer)self);
  ok = XtVaCreateManagedWidget
    ("ok",commandWidgetClass,cont,
     XtNfromHoriz,NULL,
     XtNfromVert,fileView,
     NULL);
  XtAddCallback(ok,XtNcallback,CnvPathEnterCb,(XtPointer)self);
  cancel = XtVaCreateManagedWidget
    ("cancel",commandWidgetClass,cont,
     XtNfromHoriz,ok,
     XtNfromVert,fileView,
     NULL);
  XtAddCallback(cancel,XtNcallback,CnvPathCancelCb,(XtPointer)self);
  XtInstallAllAccelerators(cont,cont);
}

/**********************************************************************
 * 
 **********************************************************************/

/*
 * function: select a filepath
 */
static void CnvPathEnter(CnvPath self)
{
    String str;
    
    self->hold = CnvPathOk;
    XtVaGetValues(self->text,
		  XtNstring,&str,
		  NULL);
    namecpy(self->filename,str);
}

/*
 * implicit: App app
 */
static void CnvPathListInit(CnvPath p)
{
  int i;

  for(i=0; i < EntryMax; i++) {
    /* debug1("i:%i\n",i); */
    p->fileTable[i] = NULL;
  }
  for(i=0; i < EntryMax; i++) {
    /* debug1("i2:%i\n",i); */
    p->dirTable[i] = NULL;
  }
  p->fileNro = 0;
  p->dirNro = 0;
  *(p->filename) = 0;
}

/*
 * compare funtion for sorting in CnvPathListGet()
 */
static int StrCmp (const void **s1, const void **s2)
{
    return strcmp (*s1, *s2);
}

/*
 * read files and directories from given absolute dir
 * update them into Path structure. directoty is a crossfire
 * part of file name ie. LibDir/MapDir<dirname>, althought
 * it is absolute name in it part, ie it have to begin with '/'.
 */
static void CnvPathListGet(CnvPath self,String directory)
{
  DIR *dir;
#ifdef NeXT
  struct direct *dirent;
#else
  struct dirent *dirent;
#endif
  struct stat statbuf;
  Cardinal i,fileOld,dirOld;
  char path[PATH_MAX+1],full[PATH_MAX+1];

  /*** form full dir name ***/
  sprintf(full,"%s%s",self->root,directory);
  debug1("CnvPathListGet(), rescan %s\n",full);

  /*** open it ***/
  if(CnvPathNoAccess(self->shell, full)) return;
  if((dir = opendir(full)) == NULL) CnvDie(self->shell,full);
  XtVaSetValues(self->cwd,
	      XtNlabel,directory,
	      NULL);
  pathcpy(self->current,directory);

  /*** read dir & files ***/
  fileOld = self->fileNro;
  dirOld = self->dirNro;
  self->fileNro = 0;
  self->dirNro = 0;
  while((dirent = readdir(dir)) != NULL) {
    sprintf(path,"%s/%s",full,dirent->d_name);

    if (stat ((char *) path, &statbuf) == -1) {
      perror (path);
      continue;
    }
    if(S_ISDIR(statbuf.st_mode)) {
      if(self->dirNro >= EntryMax) continue;

      XtFree(self->dirTable[self->dirNro]);
      self->dirTable[self->dirNro] = XtNewString(dirent->d_name);

      self->dirNro++;
    } else {
      if(self->fileNro >= EntryMax) continue;

      XtFree(self->fileTable[self->fileNro]);
      self->fileTable[self->fileNro] = XtNewString(dirent->d_name);

      self->fileNro++;
    }
  }
  closedir(dir);

  /*** free strings ***/
  for(i = self->fileNro; i < fileOld; i++) {
    XtFree(self->fileTable[i]);
    self->fileTable[i] = NULL;
  }
  for(i = self->dirNro; i < dirOld; i++) {
    XtFree(self->dirTable[i]);
    self->dirTable[i] = NULL;
  }

  /*** sort tables ***/
  qsort(self->fileTable,self->fileNro,sizeof(String),(int (*)())StrCmp);
  qsort(self->dirTable,self->dirNro,sizeof(String),(int (*)())StrCmp);

  /*** update ***/
  XawListChange(self->fileList,self->fileTable,self->fileNro,0,True);
  XawListChange(self->dirList,self->dirTable,self->dirNro,0,True);
}

/**********************************************************************
 * public
 **********************************************************************/

/*
 * create new instance of file-selector, no show
 * dir is crossfire path eg "/"
 * name: name of this module
 * root: all 
 * dir : starting directory
 */
CnvPath CnvPathCreate(String name,String root,String dir)
{


    CnvPath self;

    debug3("CnvPathCreate() %s %s %s\n",name,root,dir);
    self  = (CnvPath) XtMalloc (sizeof (struct CnvPath));
    memset(self,0,sizeof(struct CnvPath));
    pathcpy(self->root,root);

    /*** initialize translations if not done ***/
    if(hasActions == False) {
	XtAppAddActions(XtWidgetToApplicationContext(cnv->shell),
			actions,
			XtNumber(actions));
	hasActions = True;
    }
    Layout(self,name);
    CnvPathListInit(self);
    CnvCenterWidget(self->shell);
    CnvPathListGet(self,dir);
    return self;
}

/*
 * remove instance of file-selector
 */
void CnvPathDestroy(CnvPath self)
{
  Cardinal i;
  debug("CnvPathDestroy()\n");

  for(i=0;i<self->dirNro;i++) XtFree(self->dirTable[i]);
  for(i=0;i<self->fileNro;i++) XtFree(self->fileTable[i]);
  XtDestroyWidget(self->shell);
}

/*
 * member: select a filepath
 * return: PathResponce & results are in Path-structure 
 */
CnvPathResponce CnvPathSelect(CnvPath self)
{
  if(tmpCnvPath) {
    CnvNotify("File Selector already in use",
	      "Continue",NULL);
    return CnvPathCancel;
  }
  XtVaSetValues(self->text,
		XtNstring,self->filename,
		NULL);
  /*** round ***/
  tmpCnvPath = self;
  self->hold = CnvPathHold;
  XtPopup(self->shell,XtGrabExclusive);
  while(self->hold == CnvPathHold) {
    XtAppProcessEvent(XtWidgetToApplicationContext(self->shell),XtIMXEvent);
  }
  XtPopdown(self->shell);
  tmpCnvPath = NULL;
  return self->hold;
}

/*
 * member: select a filepath
 * return: PathResponce & results are in Path-structure 
 */
CnvPathResponce CnvPathSelectAbs(CnvPath self,String path)
{
  if(tmpCnvPath) {
    CnvNotify("File Selector already in use",
	      "Continue",NULL);
    return CnvPathCancel;
  }
  XtVaSetValues(self->text,
		XtNstring,self->filename,
		NULL);
  /*** round ***/
  tmpCnvPath = self;
  self->hold = CnvPathHold;
  XtPopup(self->shell,XtGrabExclusive);
  while(self->hold == CnvPathHold) {
    XtAppProcessEvent(XtWidgetToApplicationContext(self->shell),XtIMXEvent);
  }
  XtPopdown(self->shell);
  tmpCnvPath = NULL;
  sprintf(path,"%s%s/%s",self->root,self->current,self->filename);
  return self->hold;
}

/**********************************************************************
 * utility
 **********************************************************************/

/*
 * - path is full absolute name
 * - return true if NO access
 */
Boolean CnvPathNoAccess(Widget w, String path)
{
  char buf[BUFSIZ];

  if (access (path, R_OK | X_OK) == -1) {
    switch (errno) {
    case EACCES:
      sprintf (buf, "Permission denied for %s", path);
      CnvNotify (buf,Continue,NULL);
      break;
    case ENOENT:
      sprintf (buf, "%s does not exist", path);
      CnvNotify (buf,Continue,NULL);
      break;
    case EIO:
      sprintf (buf, "IO error in accessing %s", path);
      CnvNotify (buf,Continue,NULL);
      break;
    case ENAMETOOLONG:
    case ENOTDIR:
      sprintf (buf, "Error in pathname for %s", path);
      CnvNotify (buf,Continue);
      break;
    default:
      sprintf (buf, "Error accessing to %s", path);
      CnvNotify (buf,Continue,NULL);
      break;
    }
    return True;
  } else {
    return False;
  }
} 

/* 
 * - path is full absolute name
 * - return true if NO access
 */
Boolean CnvPathNoWrite(Widget w, String path)
{
  char buf[BUFSIZ];

  if (access (path, R_OK | W_OK) == -1) {
    switch (errno) {
    case EACCES:
      sprintf (buf, "Permission denied for %s", path);
      CnvNotify (buf,Continue,NULL);
      break;
    case ENOENT:
      sprintf (buf, "%s does not exist", path);
      CnvNotify (buf,Continue,NULL);
      break;
    case EIO:
      sprintf (buf, "IO error in accessing %s", path);
      CnvNotify (buf,Continue,NULL);
      break;
    case ENAMETOOLONG:
    case ENOTDIR:
      sprintf (buf, "Error in pathname for %s", path);
      CnvNotify (buf,Continue);
      break;
    default:
      sprintf (buf, "Error accessing to %s", path);
      CnvNotify (buf,Continue,NULL);
      break;
    }
    return True;
  } else {
    return False;
  }
} 

/*** end of CnvPath.c ***/



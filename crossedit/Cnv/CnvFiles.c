#include <Cnv.h>
#include <Posix.h>

/**********************************************************************
 * private 
 **********************************************************************/

static void entryCb(Widget w,XtPointer client,XtPointer call)
{
  CnvFiles self = (CnvFiles)client;
  char path[PATH_MAX+1];

  /* entry = XawSimpleMenuGetActiveEntry(w); */
  sprintf(path,"%s/%s",self->directory,XtName(w));
  (*self->proc)(self->client,path);
}

static void Malfunction(CnvFiles self,String msg)
{
  char buf[BUFSIZ];

  sprintf(buf,"%s %s",msg,self->directory);
  CnvWarn(self->shell,buf);
  XtVaCreateManagedWidget
    ("No Data",smeBSBObjectClass,self->shell,
     NULL);
}

/**********************************************************************
 * public 
 **********************************************************************/

/*
 * member   :
 * name     : resource-name
 * app      : application level shell-widget
 * directory: directory to read menu from 
 * proc     : callback to be called
 * client   : 
 * return   : gives created menu
 */
CnvFiles CnvFilesCreate(String name,
			Widget parent,
			CnvFilesProc proc,
			XtPointer client,
			String directory)
{
    CnvFiles self;

    self = (CnvFiles)XtMalloc(sizeof(struct CnvFiles));
    self->shell = NULL;
    self->name = XtNewString(name);
    self->proc = proc;
    self->client = client;
    CnvFilesRescan(self,name,parent,directory);
    return self;
}

/*
 * member: remove CnvFiles packege
 * return: status
 */
void CnvFilesDestroy(CnvFiles self)
{
  XtDestroyWidget(self->shell);
  XtFree(self->name);
  self->shell = NULL;
}

/*
 * member   : make menu from directory 
 * directory: absolute directoryname to scan
 * return   : gives created menu
 */
Widget CnvFilesRescan(CnvFiles self,String name,Widget parent,
		      String directory)
{
  char buf[BUFSIZ];
  char path[PATH_MAX+1];
  DIR *dir;
#ifdef NeXT
  struct direct *dirent;
#else
  struct dirent *dirent;
#endif
  struct stat statbuf;
  Widget entry;
  int cnt = 0;

  /*** create shell ***/
  if(self->shell != NULL) XtDestroyWidget(self->shell);
  self->shell = XtVaCreatePopupShell
    (name,simpleMenuWidgetClass,parent,
     NULL);

  /*** can read directory ? ***/
  pathcpy(self->directory,directory);
  if(access(self->directory,R_OK | X_OK) == -1) {
    Malfunction(self,"has no access");
    return NULL;
  }

  /*** open dir ***/
  if((dir = opendir(self->directory)) == NULL) {
    Malfunction(self,"cannot open");
    return NULL;
  } 

  /*** make menu ***/
  while((dirent = readdir(dir)) != NULL) {
    if(!strcmp(dirent->d_name,".") || !strcmp(dirent->d_name,"..")) 
      continue;
    sprintf(path,"%s/%s",directory,dirent->d_name);
    if(stat((char*)path,&statbuf) == -1) {
      sprintf(buf,"%s cannot stat",path);
      CnvWarn(self->shell,buf);
      return NULL;      
    }
    if( S_ISREG(statbuf.st_mode) && !access(path, R_OK) ) {
      /*** create menu selection ***/
      entry = XtVaCreateManagedWidget
	(dirent->d_name,smeBSBObjectClass,self->shell,
	 NULL);
      XtAddCallback(entry,XtNcallback,entryCb,(XtPointer)self);
      cnt++;
    }
  }
  closedir(dir);
  if(!cnt) {
    Malfunction(self,"empty");
    return NULL;
  }
  return self->shell;
}

/*** end of CnvFiles.c ***/






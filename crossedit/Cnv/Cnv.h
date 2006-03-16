/*
 * Cnv.h - generic GUI utilities
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

#ifndef _Cnv_h
#define _Cnv_h

#include <Posix.h>
#include <Xaw.h>
#include <Ansi.h>
#include <config.h>
#include <includes.h>

/*
 * generic macros
 */
#define pathcpy(dst,src) {strncpy(dst,src,PATH_MAX);dst[PATH_MAX]='\0';}
#define namecpy(dst,src) {strncpy(dst,src,NAME_MAX);dst[NAME_MAX]='\0';}

/*
 * menu
 */ 
typedef struct CnvMenuRec {
    const char *label;
    void (*func) (Widget, XtPointer, XtPointer);
} CnvMenuRec;
extern Widget CnvMenu(String, Widget,CnvMenuRec[], XtPointer);

/**********************************************************************
 * CnvPath
 **********************************************************************/

#define EntryMax (_POSIX_ARG_MAX / 2)

typedef enum {
  CnvPathHold,
  CnvPathOk,
  CnvPathCancel
} CnvPathResponce;

/*
 * 
 */
typedef struct CnvPath {
    Widget shell;               /* shell widget */
    Widget dirList;             /* list widget for directories */
    Widget fileList;            /* list widget for files */
    Widget cwd;                 /* label widget for current working dir */
    Widget text;                /* text field */
    CnvPathResponce hold;          /* for internal use */
    String dirTable[EntryMax];  /* table of dirnames, alloc dyn */
    String fileTable[EntryMax]; /* table of filename, alloc dyn */
    Cardinal fileNro;           /* number of files in current directory */
    Cardinal dirNro;            /* number of directories in current dir */
    /* char dirname[PATH_MAX+1];  name of current directory, abs. */
    char current[PATH_MAX+1];   /* LibDir/MapDir<current> */
    char filename[NAME_MAX+1];  /* last selected file, rel to dir */
    char root[PATH_MAX+1];      /* directory names are prefixed by this */
} *CnvPath;

extern CnvPath CnvPathCreate (String name,String root, String dir);
extern void CnvPathDestroy ( CnvPath self );
extern Boolean CnvPathNoAccess ( Widget w, String path );
extern Boolean CnvPathNoWrite ( Widget w, String path );
extern CnvPathResponce CnvPathSelect ( CnvPath self );


/**********************************************************************
 * CnvFiles
 **********************************************************************/

typedef void (*CnvFilesProc)(XtPointer client,String path);

typedef struct CnvFiles {
  /* private */
  Widget shell;             /* shellwidget containing menu */ 
  String name;              /* menu name, to be not ambiguous */
  char directory[PATH_MAX+1]; /* directory to get menu */
  CnvFilesProc proc;           /* function when menu item selected */
  XtPointer client;         /* package pointer */
  /* public */
} *CnvFiles; 

extern CnvFiles CnvFilesCreate(String,Widget,CnvFilesProc,XtPointer,String);
extern void CnvFilesDestroy ( CnvFiles  );
extern Widget CnvFilesRescan ( CnvFiles, String,Widget,String directory );

/**********************************************************************
 * CnvBrowse
 **********************************************************************/
typedef struct CnvBrowse *CnvBrowse;
/*
 * call this function when finished editing
 */
typedef void (*CnvBrowseEditProc)(CnvBrowse self,XtPointer client,String str);

/*
 * object for showing text 
 */
struct CnvBrowse {
  Widget shell;
  Widget text;
  Widget file;
  Widget ok;
  Boolean isup;
  CnvBrowseEditProc proc;
};

typedef enum CnvBrowseReturn {
  CnvBrowseOk,
  CnvBrowseError
} CnvBrowseReturn;

/*
 * inteface
 */
extern CnvBrowse CnvBrowseCreate ( String name, Widget parent,
				  CnvBrowseEditProc proc);
extern void CnvBrowseDestroy ( CnvBrowse self );
extern void CnvBrowseShowFile ( CnvBrowse self, String file );
extern void CnvBrowseShowString ( CnvBrowse self, String str );
extern void CnvBrowseEditString ( CnvBrowse self, String str );

/**********************************************************************
 * Cnv
 **********************************************************************/
typedef struct Cnv {
    Widget shell;      /* application level shell widget */
    struct {
	Pixmap excloff;
	Pixmap exclon;
	Pixmap flagoff;
	Pixmap flagon;
	Pixmap notify;  /* face for CnvNotify */
	Pixmap prompt;  /* face for CnvPrompt */
	Pixmap submenu;
    } xbm;
} *Cnv;
extern Cnv cnv;        /* this is global for all */

/*
 * utility functions
 */
extern void CnvCenterWidget ( Widget w );
extern void CnvDie ( Widget self, String msg );
extern Widget CnvGetShell ( Widget child );
extern int CnvNotify (String msg, ... );
extern int CnvPrompt ( String msg, String sgg, String ans, ...);
extern void CnvWarn ( Widget self, String msg );
extern void CnvInitialize(Widget shell);
extern CnvPathResponce CnvPathSelectAbs(CnvPath self,String path);


#endif	/* _Cnv_h */

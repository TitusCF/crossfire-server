/*
	CrossEdit, A Editor for CrossFire, A Multiplayer game
	for X-windows	

	Copyright (C) 1993 Jarkko Sonninen & Petri Heinila

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
	
	The authors can be reached via e-mail to Jarkko.Sonninen@lut.fi
	or Petri.Heinila@lut.fi .
*/

#ifndef _App_h
#define _App_h

#include "Defines.h"
#include "Edit.h"
#include "Cnv.h"

/**********************************************************************/

typedef struct {
    Widget w;
    const char *name;
    unsigned long flags;
}   ArchFlagsRec;


typedef struct {
    Widget w;
    unsigned long flags;
    unsigned int all:1;
}   Arch;

/*
 * item to insert 
 */
typedef struct {
    Widget w;            /* ??? */
    Widget name;         /* name of insrted object */
    Widget face;         /* inserted object looks like this */
    Edit edit;           /* cloned object comes from this Edit */
    object *clone;       /* clone this object */
    int wall_set;        /* if wall use this set */
    mapstruct *wall_map; /* if wall use this Map */
}   Item;
/*
 * Item or area to look about
 */
typedef struct {
    Widget w;        /* ??? */
    Widget info;     /* ??? */
    Widget menu;     /* ??? */
    Edit edit;       /* last selected area fro this editor */
    XRectangle rect; /* rectangle over selected area */
}   Look;

/*
 * Application Resources. Some can be setted by command line flags
 */
typedef struct {
    Dimension mapWidth;  /* maximum width of shown map */
    Dimension mapHeight; /* maximum height of shown map */
    Boolean usePixmaps;  /* use pixmaps instead fonts */
    Boolean useColorPixmaps;  /* use color pixmaps instead fonts */
    Boolean usePng;		/* use png images instead fonts */
    String cmdCrossfire; /* to run crossfire */
    String cmdReset;     /* to run reset on crossfire */
    String cmdGoto;      /* to run goto on crossfire */
    String creator;      /* user configurable own name */
    String email;        /* user configurable internet address  */
} AppRes;

/*
 * Application domain data
 */
struct _App {
    Display *display; /* user in this display */
    Widget shell;     /* application level shell widget */
    Edit edit;        /* list ofmap editors */
    Edit clip;        /* clipboard (editor) */
    unsigned int clipon:1;     /* is clipboard up */
    Look look;        /* look part in application dialog */
    Arch arch;        /* archetype list in application dialog */
    Item item;        /* insertion item part in application dialog */
    CnvPath path;     /* file selector */
    AppRes res;       /* application resources */
    Attr attr;        /* to edit objects attributes */
    CnvFiles picks;   /* pick some objects */
    CnvFiles walls;   /* pick some walls */
    CnvFiles infof;   /* documentation files */
    CnvBrowse info;   /* to show docs */
};

extern Pixmap *pixmaps;       /* list of pixmaps */

/*
 * Macro interface
 */
#define AppItemGetEdit(a) ((a)->item.edit)
#define AppItemGetMap(a) ((a)->item.wall_map)
#define AppItemGetWall(a) ((a)->item.wall_set)
#define AppItemGetObject(a) ((a)->item.clone)

#define AppItemNoWall -1

/*
 * Function interface
 */
extern App AppCreate ( XtAppContext appCon, String displayString, XtResource resources[], Cardinal resourcesNum, XrmOptionDescRec *options, Cardinal optionsNum, int *argc, char *argv[] );
extern void AppUpdate ( App self );
extern void AppSelectSet ( App self, Edit edit, XRectangle rect );
extern void AppSelectUnset ( App self );
extern void AppItemSet ( App self, Edit edit, object *obj, int wallSet );
extern void AppEditDeattach ( App self, Edit edit );

#endif /* _App_h */



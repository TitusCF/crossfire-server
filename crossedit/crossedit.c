/*
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

#include <Posix.h>
#include "App.h"
#include "X11.h"
#include "debug.h"

/**********************************************************************
 * application resources 
 **********************************************************************/

#define Offset(field) XtOffsetOf(AppRes, field)
static XtResource resources[] = {
    {"mapWidth", "MapWidth", XtRDimension, sizeof (Dimension),
    Offset (mapWidth), XtRDimension, NULL},
    {"mapHeight", "MapHeight", XtRDimension, sizeof (Dimension),
    Offset (mapHeight), XtRDimension, NULL},
    {"usePixmaps", "UsePixmaps", XtRBoolean, sizeof (XtRBoolean),
    Offset (usePixmaps), XtRImmediate, False},
    {"useColorPixmaps", "UseColorPixmaps", XtRBoolean, sizeof (XtRBoolean),
     Offset (useColorPixmaps), XtRImmediate, False},
    {"usePng", "UsePng", XtRBoolean, sizeof (XtRBoolean),
     Offset (usePng), XtRImmediate, False},
    {"cmdCrossfire", "CmdCrossfire", XtRString, sizeof (XtRString),
    Offset (cmdCrossfire), XtRString, NULL},
    {"cmdReset", "CmdReset", XtRString, sizeof (XtRString),
    Offset (cmdReset), XtRString, NULL},
    {"creator", "Creator", XtRString, sizeof (XtRString),
    Offset (creator), XtRString, "No Body"},
    {"email", "Email", XtRString, sizeof (XtRString),
    Offset (email), XtRString, "none@foo.bar"},
};
#undef Offset

static XrmOptionDescRec options[] = {
    {"-pix","*usePixmaps",XrmoptionNoArg,"TRUE"},
    {"-xpm","*useColorPixmaps",XrmoptionNoArg,"TRUE"},
    {"-png","*usePng",XrmoptionNoArg,"TRUE"},

}; 

static String fallback_resources[] = {
#include <Crossedit.ad.h>
    NULL
};

enum DisplayMode displaymode=Dm_Png;

static void EditDirtyAc(Widget w, XEvent * event, String * argv, Cardinal * argc) {
    debug0 ("EditDirtyAc\n");
}

static void Nop(Widget w, XEvent * event, String * argv, Cardinal * argc) {}
XtActionsRec mainActions[] = {
  {"Nop", Nop},
  {"EditKey", EditDirtyAc},
};

/**********************************************************************
 * main
 **********************************************************************/

int main (int argc, char **argv, char **env)
{
    XtAppContext app_con;

    App app;

/* Crossedit seems to have so many memory problems, this seems like
 * a really good idea.
 */
#ifdef DEBUG_MALLOC_LEVEL
  malloc_debug(DEBUG_MALLOC_LEVEL);
#endif


    /* crossfire init */
    init_globals();
    init_library ();
    init_regions();
    init_archetypes ();
    editor = TRUE;
#if 0
    /* Enable this if you want people to be able to edit maps
     * in a different directory other than the master map
     * directory.  Note that the 'editor' directory of maps
     * needs to be in LIBDIR/maps (LIBDIR/maps being where
     * the maps will be saved.
     *
     * An alternative method would be to have a link to a high
     * level directory from the master map source (ie, something
     * like 'ln -s /users .LIBDIR.  Then users can select the users
     * directory in the map editor, which will take them to the
     * directory above their home directory, select their home directory,
     * and then whatever directory they want to save the maps in.
     */
    if (getenv("LIBDIR")) LibDir = getenv("LIBDIR");
#endif

    /*XtAppInitialize (&app_con, "Crossedit", NULL, 0,  &argc, argv, 
		     fallback_resources, NULL, 0);*/

    XtToolkitInitialize ();
    app_con = XtCreateApplicationContext();    
#if 1
    XtAppSetFallbackResources (app_con, fallback_resources);
#endif
    XtAppAddActions(app_con,mainActions, XtNumber (mainActions));

    /*XtGetApplicationResources (app.shell, (XtPointer) & app->res,
			       resources, XtNumber (resources), NULL, 0);*/

    app = AppCreate(app_con, "Crossedit", resources, XtNumber (resources),
		    options,XtNumber(options), &argc, argv);


    /*** usage ***/
    if (argc > 1) {
      fprintf(stderr,"Usage: crossedit [-options]\n\t[-options]:\tXt-options\n\n");
      exit(2);
    }

    XtRealizeWidget (app->shell);
    XtAppMainLoop (app_con);
    exit(0);
}

/*** End of crossfire.c ***/

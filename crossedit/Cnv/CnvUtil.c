/*
 * CnvUtil.c - miscallenous utilities in one function region
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

/*
 * include images
 */
#include "excloff.xbm"
#include "exclon.xbm"
#include "flagoff.xbm"
#include "flagon.xbm"
#include "notify.xbm"
#include "prompt.xbm"
#include "submenu.xbm"

/**********************************************************************
 * Translations
 **********************************************************************/

static void CnvNop(Widget w,XEvent *e,String *argv,Cardinal *argc)
{
    debug("CnvNop()\n");
}

static XtActionsRec CnvActions[] = {
    { "CnvNop", CnvNop }, 
};

/**********************************************************************
 * Initialization
 **********************************************************************/

Cnv cnv;

/*
 * initialize Cnv
 * CAUTION: used global data, usage restricted to ONE display
 */
void CnvInitialize(Widget shell)
{
    debug1("CnvInitialize() to %s\n",XtName(shell));
    cnv = (Cnv)XtMalloc(sizeof(struct Cnv));
    cnv->shell = shell;
    /*
     * initialize images
     */
    cnv->xbm.excloff = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)excloff_bits,
	 excloff_width,
	 excloff_height);
    cnv->xbm.exclon = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)exclon_bits,
	 exclon_width,
	 exclon_height);
    cnv->xbm.flagoff = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)flagoff_bits,
	 flagoff_width,
	 flagoff_height);
    cnv->xbm.flagon = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)flagon_bits,
	 flagon_width,
	 flagon_height);
    cnv->xbm.notify = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)notify_bits,
	 notify_width,
	 notify_height);
    cnv->xbm.prompt = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)prompt_bits,
	 prompt_width,
	 prompt_height);
    cnv->xbm.submenu = XCreateBitmapFromData
        (XtDisplay(shell),DefaultRootWindow(XtDisplay(shell)),
	 (char*)submenu_bits,
	 submenu_width,
	 submenu_height);
    /*
     * actions
     */
    XtAppAddActions(XtWidgetToApplicationContext(cnv->shell),
		    CnvActions,
		    XtNumber(CnvActions));
}

/**********************************************************************
 * Utility
 **********************************************************************/

/*
 * get shell widget of child
 */
Widget CnvGetShell (Widget child)
{
    Widget use;

    for (use = child;
      use != 0 && XtIsShell (use) == False;
      use = XtParent (use)
      )
	/* Widget cannot live without top shell */
	if (use == 0) {
	    fprintf (stderr, "getShell: %s\n", "no shell found");
	}
    return use;
}

/*
 * center widget w into rootwindow
 */
void CnvCenterWidget (Widget w)
{
    Position x, y;
    Dimension width, height, border;

    /* center dialog in root window */
    XtRealizeWidget (w);
    XtVaGetValues (w, XtNwidth, &width, XtNheight, &height,
      XtNborderWidth, &border, NULL);
    x = (WidthOfScreen (XtScreen (w)) - width - border) / 2;
    y = (HeightOfScreen (XtScreen (w)) - height - border) / 2;
    XtVaSetValues (w, XtNx, x, XtNy, y, NULL);
}

#if 0
static String CnvResourceFilePath (Widget w)
{
    String path = 0;

    if (getenv ("XUSERFILESEARCHPATH")) {
	path = XtResolvePathname (XtDisplay (w),
	  NULL,
	  NULL,
	  NULL,
	  getenv ("XUSERFILESEARCHPATH"),
	  NULL, 0,
	  NULL
	  );
    }
    if (path) {
	return path;
    } else {
	path = XtResolvePathname (XtDisplay (w),
	  "app-defaults",	/* type */
	  NULL,
	  NULL,			/* suffix */
	  NULL,			/* default; XFILESEARCHPATH */
	  NULL, 0,		/* substitutions, numof */
	  NULL
	  );
    }
    return path;
}
#endif
/*
 * function: message to stderr and exit
 * self    : any widget
 * msg     : given message
 */
void CnvDie(Widget self,String msg)
{
  char buf[BUFSIZ];

  sprintf(buf,
	  "%s: %s",
	  XtName(self),
	  msg);
  XtAppError(XtWidgetToApplicationContext(self),buf);
}

/*
 * function: message to stderr
 * self    : any widget
 * msg     : given message
 */
void CnvWarn(Widget self,String msg)
{
  char buf[BUFSIZ];

  sprintf(buf,
	  "%s: %s",
	  XtName(self),
	  msg);
  XtAppWarning(XtWidgetToApplicationContext(self),buf);
}

/*** end of CnvUtil.c ***/

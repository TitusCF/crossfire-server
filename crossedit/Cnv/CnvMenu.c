#include <Cnv.h>

Widget CnvMenu(String name, Widget par,CnvMenuRec rec[], XtPointer p)
{
    Widget shell, w;
    int i;

    shell = XtVaCreatePopupShell (name, simpleMenuWidgetClass, par,
				  NULL);

    for (i = 0; *rec[i].label; i++) {
	if (strncmp (rec[i].label, "---", 3)) {
	    w = XtVaCreateManagedWidget 
		(rec[i].label, smeBSBObjectClass, shell, NULL);
	    XtAddCallback (w, XtNcallback, rec[i].func, p);
	    XtInstallAccelerators(par, w);
	} else
	    XtVaCreateManagedWidget ("line", smeLineObjectClass, shell, NULL);
    }


    return shell;
}

/*** end of CnvMenu.c ***/

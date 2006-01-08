#include <Cnv.h>
#include <Ansi.h>
#include <Xaw.h>
#include "debug.h" /* debug1 */

/**********************************************************************
 * prompt
 **********************************************************************/

static int CnvPromptSelect = 0;
static int CnvPromptActionFlag = 0;
static char CnvPromptString[CnvPromptMax+1];
static Widget CnvPromptWidgetText;
static void CnvPromptCb (Widget w, XtPointer client, XtPointer call);

/*
 *
 */
static void CnvPromptAction (Widget w, XEvent * event, String * argv, Cardinal * argc)
{
    CnvPromptCb (w, (XtPointer) 1, NULL);	/* send Ok to callback */
}

/*
 *
 */
XtActionsRec CnvPromptActionTable[] = {
    { "CnvPromptAction", CnvPromptAction }
};

/*
 *
 */
static void CnvPromptCb (Widget w, XtPointer client, XtPointer call)
{
    char *t;
    CnvPromptSelect = (int) client;	/* set cardinal */
    XtVaGetValues(CnvPromptWidgetText, XtNstring, &t, NULL);
    
    snprintf(CnvPromptString, sizeof(CnvPromptString), "%s", t);

    XtDestroyWidget (CnvGetShell (w));
}

int CnvPrompt(String msg,String def,String ans,...)
{
    va_list al;
    Widget shell,cont,sign,use = NULL;
    int i;
    String str;

    CnvPromptString[0] = '\0';

    if (!CnvPromptActionFlag) {
	XtAppContext a = XtWidgetToApplicationContext (cnv->shell);
	XtAppAddActions (a,CnvPromptActionTable, 
			 XtNumber (CnvPromptActionTable));
    }

    /*** Layout ***/
    shell = XtVaCreatePopupShell
	(msg,transientShellWidgetClass,cnv->shell,
	 NULL);
    cont = XtVaCreateManagedWidget
	("cont",formWidgetClass,shell,
	 NULL);
    sign = XtVaCreateManagedWidget
	("sign",labelWidgetClass,cont,
	 XtNbitmap,cnv->xbm.prompt,
	 XtNborderWidth,0,
	 XtNheight,34,
	 NULL);
    (void) XtVaCreateManagedWidget
	("label",labelWidgetClass,cont,
	 XtNlabel,msg,
	 XtNborderWidth,0,
	 XtNheight,34,
	 XtNwidth,300,
	 XtNfromHoriz,sign,
	 NULL);
    CnvPromptWidgetText = XtVaCreateManagedWidget
	("prompt",asciiTextWidgetClass,cont,
	 XtNresizable,True,
	 XtNfromVert,sign,
	 XtNstring,def,
	 XtNeditType,XawtextEdit,
	 XtNaccelerators,
	 XtParseAcceleratorTable
	 ("#override <Key>Return: CnvPromptAction() \n"),
	 XtNtranslations,XtParseTranslationTable
	 ("#override <Key>Return: CnvPromptAction() \n"
	  "<Key>Tab: CnvNop()\n"),
	 XtNwidth,336,
	 NULL);
    
    /* Put the cursor at the end of the line. */
    XtVaSetValues(CnvPromptWidgetText, 
        XtNinsertPosition, strlen(def),
        NULL);
    
    XtInstallAllAccelerators(cont,cont);
    
    /*** button-list ***/
    va_start(al,ans);
    for(i=1;(str = (String)va_arg(al,String)) != NULL; i++) {
	use = XtVaCreateManagedWidget
	    (str,commandWidgetClass,cont,
	     XtNfromHoriz, use,
	     XtNfromVert,CnvPromptWidgetText,
	     NULL);
	XtAddCallback(use,XtNcallback,CnvPromptCb,(XtPointer)i);
    }
    va_end(al);
    /*** looping ***/
    CnvCenterWidget(shell);
    XtPopup(shell,XtGrabExclusive);
    while(!CnvPromptSelect) {
	XtAppProcessEvent(XtWidgetToApplicationContext(shell),XtIMXEvent);
    }
    i = CnvPromptSelect;
    CnvPromptSelect = 0;
    
    debug1("CnvPrompt '%s'\n", CnvPromptString);
    strncpy(ans,CnvPromptString,CnvPromptMax);
    ans[CnvPromptMax] = '\0';
    return i;
}

/*** end of CnvPrompt.c ***/

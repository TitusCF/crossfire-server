#include <Cnv.h>
#include <Ansi.h>
#include <Xaw.h>

/**********************************************************************
 * prompt
 **********************************************************************/

static int CnvPromptSelect = 0;
static int CnvPromptActionFlag = 0;
static char *CnvPromptString;
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
    CnvPromptSelect = (int) client;	/* set cardinal */
    XtVaGetValues (CnvPromptWidgetText, XtNstring, &CnvPromptString, NULL);

    XtDestroyWidget (CnvGetShell (w));
}

int CnvPrompt(String msg,String def,String ans,...)
{
    va_list al;
    Widget shell,cont,sign,use = NULL;
    int i;
    String str;

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
    strncpy(ans,CnvPromptString,CnvPromptMax);
    return i;
}

/*** end of CnvPrompt.c ***/

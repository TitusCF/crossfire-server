#include <Cnv.h>

/**********************************************************************
 * notify
 **********************************************************************/

static int CnvNotifyResponce;

/*
 * callback: get responce
 */
static void CnvNotifyCb(Widget w,XtPointer client,XtPointer call)
{
  CnvNotifyResponce = (int)client;
  XtDestroyWidget(CnvGetShell(w));
}

/*
 * function: show message for user and get responce
 * msg     : message string
 * ...     : user responces, terminated by NULL, eg. "OK","Cancel",NULL
 * return  : selected correspondin responce, eg. OK=1, Cancel=2 or
 *           0 on error
 */
int CnvNotify(String msg,...)
{
  Widget popup,form,use=0,sign;
  va_list al;
  String str;
  int i;

  /* creating widgets */
  popup = XtVaCreatePopupShell("notify",transientShellWidgetClass,cnv->shell,
			       NULL);
  form = XtVaCreateManagedWidget("form",formWidgetClass,popup,
				 NULL);
  sign = XtVaCreateManagedWidget("sign",labelWidgetClass,form,
				 XtNbitmap,cnv->xbm.notify,
				 XtNborderWidth,0,
				 XtNheight,34,
				 NULL);
  (void)XtVaCreateManagedWidget("label",labelWidgetClass,form,
				  XtNlabel,msg,
				  XtNborderWidth,0,
				  XtNheight,34,
				  XtNwidth,300,
				  XtNfromHoriz,sign,
				  NULL);
  /* creating given actions */
  va_start(al,msg);
  for(i=1;(str = (String)va_arg(al,String)) != NULL;i++) {
    use = XtVaCreateManagedWidget(str,commandWidgetClass,form,
				  XtNfromHoriz,i == 1 ? NULL : use,
				  XtNfromVert,sign,
				  NULL);
    XtAddCallback(use,XtNcallback,CnvNotifyCb,(XtPointer)i);
  }
  va_end(al);

  /* looping */
  CnvCenterWidget(popup);
  XtPopup(popup,XtGrabExclusive);
  CnvNotifyResponce = 0;
  while(!CnvNotifyResponce) {
    XtAppProcessEvent(XtWidgetToApplicationContext(popup),XtIMXEvent);
  }
  return CnvNotifyResponce;
}

/*** End of CnvNotify.c ***/

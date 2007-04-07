/*****************************************************************************/
/* Template for version 2.0 plugins.                                         */
/* Contact: yann.chachkoff@myrealbox.com                                     */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001-2005 by Chachkoff Yann (Feel free to deliver your complaints)     */
/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000 Mark Wedel                                            */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */
/*****************************************************************************/

/* First let's include the header file needed                                */

#include <plugin_template.h>
#include <stdarg.h>

f_plug_api gethook;
f_plug_api registerGlobalEvent;
f_plug_api unregisterGlobalEvent;

CFPContext* context_stack;
CFPContext* current_context;
static int current_command = -999;

void initContextStack()
{
    current_context = NULL;
    context_stack = NULL;
}

void pushContext(CFPContext* context)
{
    CFPContext* stack_context;
    if (current_context == NULL)
    {
        context_stack = context;
        context->down = NULL;
    }
    else
    {
        context->down = current_context;
    }
    current_context = context;
}

CFPContext* popContext()
{
    CFPContext* oldcontext;
    if (current_context != NULL)
    {
        oldcontext = current_context;
        current_context = current_context->down;
        return oldcontext;
    }
    else
        return NULL;
}

CF_PLUGIN int initPlugin(const char* iversion, f_plug_api gethooksptr)
{
    gethook = gethooksptr;
    cf_init_plugin( gethook );

    cf_log(llevDebug, PLUGIN_VERSION " init\n");

    /* Place your initialization code here */
    return 0;
}

CF_PLUGIN void* getPluginProperty(int* type, ...)
{
    va_list args;
    char* propname;
    int i;
    va_start(args, type);
    propname = va_arg(args, char *);
    cf_log(llevDebug, "Property name: %s\n", propname);

    if (!strcmp(propname, "Identification"))
    {
        va_end(args);
        return PLUGIN_NAME;
    }
    else if (!strcmp(propname, "FullName"))
    {
        va_end(args);
        return PLUGIN_VERSION;
    }
    return NULL;
}

CF_PLUGIN int runPluginCommand(object* op, char* params)
{
    return -1;
}

CF_PLUGIN int postInitPlugin()
{
    int hooktype = 1;
    int rtype = 0;

    cf_log(llevDebug, PLUGIN_VERSION " post init\n");
    registerGlobalEvent =   gethook(&rtype,hooktype,"cfapi_system_register_global_event");
    unregisterGlobalEvent = gethook(&rtype,hooktype,"cfapi_system_unregister_global_event");
    cf_init_plugin( gethook );
    initContextStack();
    /* Pick the global events you want to monitor from this plugin */
    /*registerGlobalEvent(NULL,EVENT_BORN,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_CLOCK,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_CRASH,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_PLAYER_DEATH,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_GKILL,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_LOGIN,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_LOGOUT,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPENTER,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPLEAVE,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MAPRESET,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_REMOVE,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_SHOUT,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_TELL,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_MUZZLE,PLUGIN_NAME,globalEventListener);
    registerGlobalEvent(NULL,EVENT_KICK,PLUGIN_NAME,globalEventListener);*/
    return 0;
}

CF_PLUGIN void* globalEventListener(int* type, ...)
{
    va_list args;
    static int rv=0;
    CFPContext* context;
    context = malloc(sizeof(CFPContext));
    char* buf;
    player* pl;
    object* op;

    va_start(args, type);
    context->event_code = va_arg(args, int);
    printf("****** Global event listener called ***********\n");
    printf("- Event code: %d\n", context->event_code);

    context->message[0]=0;

    context->who         = NULL;
    context->activator   = NULL;
    context->third       = NULL;
    context->event       = NULL;
    rv = context->returnvalue = 0;
    switch(context->event_code)
    {
        case EVENT_CRASH:
            printf( "Unimplemented for now\n");
            break;
        case EVENT_BORN:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_PLAYER_DEATH:
            context->who = va_arg(args, object*);
            break;
        case EVENT_GKILL:
            context->who = va_arg(args, object*);
            context->activator = va_arg(args, object*);
            break;
        case EVENT_LOGIN:
            pl = va_arg(args, player*);
            context->activator = pl->ob;
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_LOGOUT:
            pl = va_arg(args, player*);
            context->activator = pl->ob;
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_REMOVE:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_SHOUT:
            context->activator = va_arg(args, object*);
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_MUZZLE:
            context->activator = va_arg(args, object*);
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_KICK:
            context->activator = va_arg(args, object*);
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_MAPENTER:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_MAPLEAVE:
            context->activator = va_arg(args, object*);
            break;
        case EVENT_CLOCK:
            break;
        case EVENT_MAPRESET:
            buf = va_arg(args, char*);
            if (buf !=0)
                strcpy(context->message,buf);
            break;
        case EVENT_TELL:
            break;
    }
    va_end(args);
    context->returnvalue = 0;

    pushContext(context);
    /* Put your plugin action(s) here */
    context = popContext();
    rv = context->returnvalue;
    free(context);
    cf_log(llevDebug, "*********** Execution complete ****************\n");

    return &rv;
}

CF_PLUGIN void* eventListener(int* type, ...)
{
    static int rv=0;
    va_list args;
    char* buf;
    CFPContext* context;

    context = malloc(sizeof(CFPContext));

    context->message[0]=0;

    va_start(args,type);

    context->who         = va_arg(args, object*);
    context->activator   = va_arg(args, object*);
    context->third       = va_arg(args, object*);
    buf                  = va_arg(args, char*);
    if (buf !=0)
        strcpy(context->message,buf);
    context->fix         = va_arg(args, int);
    context->event       = va_arg(args, object*);
    context->event_code  = context->event->subtype;
    strncpy(context->options, sizeof(context->options), context->event->name);
    context->returnvalue = 0;
    va_end(args);

    pushContext(context);
    /* Put your plugin action(s) here */
    context = popContext();
    rv = context->returnvalue;
    free(context);
    cf_log(llevDebug, "Execution complete");
    return &rv;
}

CF_PLUGIN int   closePlugin()
{
    cf_log(llevDebug, PLUGIN_VERSION " closing\n");
    return 0;
}


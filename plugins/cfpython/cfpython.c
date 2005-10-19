/*****************************************************************************/
/* CFPython - A Python module for Crossfire RPG.                             */
/*****************************************************************************/
/* This is the third version of the Crossfire Scripting Engine.              */
/* The first version used Guile. It was directly integrated in the server    */
/* code, but since Guile wasn't perceived as an easy-to-learn, easy-to-use   */
/* language by many, it was dropped in favor of Python.                      */
/* The second version, CFPython 1.0, was included as a plugin and provided   */
/* just about the same level of functionality the current version has. But   */
/* it used a rather counter-intuitive, procedural way of presenting things.  */
/*                                                                           */
/* CFPython 2.0 aims at correcting many of the design flaws crippling the    */
/* older version. It is also the first plugin to be implemented using the    */
/* new interface, that doesn't need awkward stuff like the horrible CFParm   */
/* structure. For the Python writer, things should probably be easier and    */
/* lead to more readable code: instead of writing "CFPython.getObjectXPos(ob)*/
/* he/she now can simply write "ob.X".                                       */
/*                                                                           */
/*****************************************************************************/
/* Please note that it is still very beta - some of the functions may not    */
/* work as expected and could even cause the server to crash.                */
/*****************************************************************************/
/* Version history:                                                          */
/* 0.1  "Ophiuchus"   - Initial Alpha release                                */
/* 0.5  "Stalingrad"  - Message length overflow corrected.                   */
/* 0.6  "Kharkov"     - Message and Write correctly redefined.               */
/* 0.7  "Koursk"      - Setting informations implemented.                    */
/* 1.0a "Petersburg"  - Last "old-fashioned" version, never submitted to CVS.*/
/* 2.0  "Arkangelsk"  - First release of the 2.x series.                     */
/*****************************************************************************/
/* Version: 2.0beta8 (also known as "Alexander")                             */
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
/*                                                                           */ /*****************************************************************************/

/* First let's include the header file needed                                */

#include <cfpython.h>
#include <stdarg.h>

#define PYTHON_DEBUG   /* give us some general infos out */

f_plug_api gethook;
f_plug_api registerGlobalEvent;
f_plug_api unregisterGlobalEvent;
f_plug_api systemDirectory;
f_plug_api reCmp;

static void set_exception(const char *fmt, ...);
static PyObject* createCFObject(PyObject* self, PyObject* args);
static PyObject* createCFObjectByName(PyObject* self, PyObject* args);
static PyObject* getCFPythonVersion(PyObject* self, PyObject* args);
static PyObject* getReturnValue(PyObject* self, PyObject* args);
static PyObject* setReturnValue(PyObject* self, PyObject* args);
static PyObject* matchString(PyObject* self, PyObject* args);
static PyObject* findPlayer(PyObject* self, PyObject* args);
static PyObject* readyMap(PyObject* self, PyObject* args);
static PyObject* getCostFlagTrue(PyObject* self, PyObject* args);
static PyObject* getCostFlagBuy(PyObject* self, PyObject* args);
static PyObject* getCostFlagSell(PyObject* self, PyObject* args);
static PyObject* getCostFlagNoBargain(PyObject* self, PyObject* args);
static PyObject* getCostFlagIdentified(PyObject* self, PyObject* args);
static PyObject* getCostFlagNotCursed(PyObject* self, PyObject* args);
static PyObject* getDirectionNorthEast(PyObject* self, PyObject* args);
static PyObject* getDirectionEast(PyObject* self, PyObject* args);
static PyObject* getDirectionSouthEast(PyObject* self, PyObject* args);
static PyObject* getDirectionSouth(PyObject* self, PyObject* args);
static PyObject* getDirectionSouthWest(PyObject* self, PyObject* args);
static PyObject* getDirectionWest(PyObject* self, PyObject* args);
static PyObject* getDirectionNorthWest(PyObject* self, PyObject* args);
static PyObject* getDirectionNorth(PyObject* self, PyObject* args);
static PyObject* getMapDirectory(PyObject* self, PyObject* args);
static PyObject* getUniqueDirectory(PyObject* self, PyObject* args);
static PyObject* getTempDirectory(PyObject* self, PyObject* args);
static PyObject* getConfigDirectory(PyObject* self, PyObject* args);
static PyObject* getLocalDirectory(PyObject* self, PyObject* args);
static PyObject* getPlayerDirectory(PyObject* self, PyObject* args);
static PyObject* getDataDirectory(PyObject* self, PyObject* args);
static PyObject* getWhoAmI(PyObject* self, PyObject* args);
static PyObject* getWhoIsActivator(PyObject* self, PyObject* args);
static PyObject* getWhoIsThird(PyObject* self, PyObject* args);
static PyObject* getWhatIsMessage(PyObject* self, PyObject* args);
static PyObject* getScriptName(PyObject* self, PyObject* args);
static PyObject* getScriptParameters(PyObject* self, PyObject* args);
static PyObject* registerCommand(PyObject* self, PyObject* args);
static PyObject* registerGEvent(PyObject* self, PyObject* args);
static PyObject* unregisterGEvent(PyObject* self, PyObject* args);
static PyObject* CFPythonError;

/* Set up an Python exception object. */
static void set_exception(const char *fmt, ...)
{
    char buf[1024];
    va_list arg;

    va_start(arg, fmt);
    vsnprintf(buf, sizeof(buf), fmt, arg);
    va_end(arg);

    PyErr_SetString(PyExc_ValueError, buf);
}

static PyMethodDef CFPythonMethods[] = {
    {"WhoAmI",              getWhoAmI,              METH_VARARGS},
    {"WhoIsActivator",      getWhoIsActivator,      METH_VARARGS},
    {"WhoIsOther",          getWhoIsThird,          METH_VARARGS},
    {"WhatIsMessage",       getWhatIsMessage,       METH_VARARGS},
    {"ScriptName",          getScriptName,          METH_VARARGS},
    {"ScriptParameters",    getScriptParameters,    METH_VARARGS},
    {"MapDirectory",        getMapDirectory,        METH_VARARGS},
    {"UniqueDirectory",     getUniqueDirectory,     METH_VARARGS},
    {"TempDirectory",       getTempDirectory,       METH_VARARGS},
    {"ConfigDirectory",     getConfigDirectory,     METH_VARARGS},
    {"LocalDirectory",      getLocalDirectory,      METH_VARARGS},
    {"PlayerDirectory",     getPlayerDirectory,     METH_VARARGS},
    {"DataDirectory",       getDataDirectory,       METH_VARARGS},
    {"DirectionNorth",      getDirectionNorth,      METH_VARARGS},
    {"DirectionNorthEast",  getDirectionNorthEast,  METH_VARARGS},
    {"DirectionEast",       getDirectionEast,       METH_VARARGS},
    {"DirectionSouthEast",  getDirectionSouthEast,  METH_VARARGS},
    {"DirectionSouth",      getDirectionSouth,      METH_VARARGS},
    {"DirectionSouthWest",  getDirectionSouthWest,  METH_VARARGS},
    {"DirectionWest",       getDirectionWest,       METH_VARARGS},
    {"DirectionNorthWest",  getDirectionNorthWest,  METH_VARARGS},
    {"CostFlagTrue",        getCostFlagTrue,        METH_VARARGS},
    {"CostFlagBuy",         getCostFlagBuy,         METH_VARARGS},
    {"CostFlagSell",        getCostFlagSell,        METH_VARARGS},
    {"CostFlagNoBargain",   getCostFlagNoBargain,   METH_VARARGS},
    {"CostFlagIdentified",  getCostFlagIdentified,  METH_VARARGS},
    {"CostFlagNotCursed",   getCostFlagNotCursed,   METH_VARARGS},
    {"ReadyMap",            readyMap,               METH_VARARGS},
    {"FindPlayer",          findPlayer,             METH_VARARGS},
    {"MatchString",         matchString,            METH_VARARGS},
    {"getReturnValue",      getReturnValue,         METH_VARARGS},
    {"setReturnValue",      setReturnValue,         METH_VARARGS},
    {"PluginVersion",       getCFPythonVersion,     METH_VARARGS},
    {"CreateObject",        createCFObject,         METH_VARARGS},
    {"CreateObjectByName",  createCFObjectByName,   METH_VARARGS},
    {"RegisterCommand",     registerCommand,        METH_VARARGS},
    {"RegisterGlobalEvent", registerGEvent,         METH_VARARGS},
    {"UnregisterGlobalEvent",unregisterGEvent,      METH_VARARGS},
    {NULL, NULL}
};

CFPContext* context_stack;
CFPContext* current_context;
static int current_command = -999;

static PyObject* registerGEvent(PyObject* self, PyObject* args)
{
    int eventcode;
    if (!PyArg_ParseTuple(args,"i",&eventcode))
        return NULL;

    registerGlobalEvent(NULL,eventcode,PLUGIN_NAME,globalEventListener);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject* unregisterGEvent(PyObject* self, PyObject* args)
{
    int eventcode;
    if (!PyArg_ParseTuple(args,"i",&eventcode))
        return NULL;

    unregisterGlobalEvent(NULL,EVENT_TELL,PLUGIN_NAME);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* createCFObject(PyObject* self, PyObject* args)
{
    object* op;
    op = cf_create_object();

    if (op == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    return Crossfire_Object_wrap(op);
}
static PyObject* createCFObjectByName(PyObject* self, PyObject* args)
{
    char* obname;
    object* op;

    if (!PyArg_ParseTuple(args,"s",&obname))
        return NULL;

    op = cf_create_object_by_name(obname);

    if (op == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    return Crossfire_Object_wrap(op);
}
static PyObject* getCFPythonVersion(PyObject* self, PyObject* args)
{
    int i=2044;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getReturnValue(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i", current_context->returnvalue);
}
static PyObject* setReturnValue(PyObject* self, PyObject* args)
{
    int i;
    if (!PyArg_ParseTuple(args,"i",&i))
        return NULL;
    current_context->returnvalue = i;
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject* matchString(PyObject* self, PyObject* args)
{
    char *premiere;
    char *seconde;
    char *result;
    int val;
    if (!PyArg_ParseTuple(args,"ss",&premiere,&seconde))
        return NULL;

    result = reCmp( &val, premiere, seconde );
    if (result != NULL)
        return Py_BuildValue("i",1);
    else
        return Py_BuildValue("i",0);
}
static PyObject* findPlayer(PyObject* self, PyObject* args)
{
    player *foundpl;
    char* txt;

    if (!PyArg_ParseTuple(args,"s",&txt))
        return NULL;

    foundpl = cf_player_find(txt);

    if (foundpl!=NULL)
        return Py_BuildValue("O",Crossfire_Object_wrap(foundpl->ob));
    else
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
}
static PyObject* readyMap(PyObject* self, PyObject* args)
{
    char* mapname;
    mapstruct* map;

    if (!PyArg_ParseTuple(args,"s",&mapname))
        return NULL;

    map = cf_map_get_map(mapname);

    if (map == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    return Crossfire_Map_wrap(map);
}
static PyObject* getCostFlagTrue(PyObject* self, PyObject* args)
{
    int i=F_TRUE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getCostFlagBuy(PyObject* self, PyObject* args)
{
    int i=F_BUY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getCostFlagSell(PyObject* self, PyObject* args)
{
    int i=F_SELL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getCostFlagNoBargain(PyObject* self, PyObject* args)
{
    int i=F_NO_BARGAIN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getCostFlagIdentified(PyObject* self, PyObject* args)
{
    int i=F_IDENTIFIED;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getCostFlagNotCursed(PyObject* self, PyObject* args)
{
    int i=F_NOT_CURSED;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionNorthEast(PyObject* self, PyObject* args)
{
    int i=2;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionEast(PyObject* self, PyObject* args)
{
    int i=3;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionSouthEast(PyObject* self, PyObject* args)
{
    int i=4;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionSouth(PyObject* self, PyObject* args)
{
    int i=5;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionSouthWest(PyObject* self, PyObject* args)
{
    int i=6;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionWest(PyObject* self, PyObject* args)
{
    int i=7;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionNorthWest(PyObject* self, PyObject* args)
{
    int i=8;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getDirectionNorth(PyObject* self, PyObject* args)
{
    int i=1;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
}
static PyObject* getMapDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,0));
}
static PyObject* getUniqueDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,1));
}
static PyObject* getTempDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,2));
}
static PyObject* getConfigDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,3));
}
static PyObject* getLocalDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,4));
}
static PyObject* getPlayerDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,5));
}
static PyObject* getDataDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv,6));
}
static PyObject* getWhoAmI(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return current_context->who;
}
static PyObject* getWhoIsActivator(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return current_context->activator;
}
static PyObject* getWhoIsThird(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return current_context->third;
}
static PyObject* getWhatIsMessage(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;

    if (current_context->message == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    else
        return Py_BuildValue("s",current_context->message);
}
static PyObject* getScriptName(PyObject* self, PyObject* args)
{
    char* buf;

    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", current_context->script);
}
static PyObject* getScriptParameters(PyObject* self, PyObject* args)
{
    char* buf;

    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s", current_context->options);
}
static PyObject* registerCommand(PyObject* self, PyObject* args)
{
    char *cmdname;
    char *scriptname;
    double cmdspeed;
    int i;

    if (!PyArg_ParseTuple(args, "ssd",&cmdname,&scriptname,&cmdspeed))
        return NULL;

    if (cmdspeed < 0)
    {
        set_exception("speed must not be negative");
        return NULL;
    }

    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        if (CustomCommand[i].name != NULL)
        {
            if (!strcmp(CustomCommand[i].name,cmdname))
            {
                set_exception("command '%s' is already registered", cmdname);
                return NULL;
            }
        }
    }
    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        if (CustomCommand[i].name == NULL)
        {
            CustomCommand[i].name = cf_strdup_local(cmdname);
            CustomCommand[i].script = cf_strdup_local(scriptname);
            CustomCommand[i].speed = cmdspeed;
            break;
        }
    };

    Py_INCREF(Py_None);
    return Py_None;
}

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
    PyObject *m, *d;
    int i;
    gethook = gethooksptr;
    printf("CFPython 2.0a init\n");

    Py_Initialize();
    Crossfire_ObjectType.tp_new = PyType_GenericNew;
    PyType_Ready(&Crossfire_ObjectType);

    m = Py_InitModule("Crossfire", CFPythonMethods);
    d = PyModule_GetDict(m);
    Py_INCREF(&Crossfire_ObjectType);
    PyModule_AddObject(m, "Object", (PyObject*)&Crossfire_ObjectType);
    CFPythonError = PyErr_NewException("Crossfire.error",NULL,NULL);
    PyDict_SetItemString(d,"error",CFPythonError);
    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        CustomCommand[i].name   = NULL;
        CustomCommand[i].script = NULL;
        CustomCommand[i].speed  = 0.0;
    }
    return 0;
}

CF_PLUGIN void* getPluginProperty(int* type, ...)
{
    va_list args;
    char* propname;
    int i;
    static CommArray_s rtn_cmd;

    va_start(args, type);
    propname = va_arg(args, char *);

    if(!strcmp(propname,"command?"))
    {
        char* cmdname;
        cmdname = va_arg(args, char *);
        va_end(args);

        for (i=0;i<NR_CUSTOM_CMD;i++)
        {
            if (CustomCommand[i].name != NULL)
            {
                if (!strcmp(CustomCommand[i].name,cmdname))
                {
                    rtn_cmd.name = CustomCommand[i].name;
                    rtn_cmd.time = (float)CustomCommand[i].speed;
                    rtn_cmd.func = runPluginCommand;
                    current_command = i;
                    return &rtn_cmd;
                }
            }
        }
        return NULL;
    }
    else if (!strcmp(propname, "Identification"))
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
    FILE*        scriptfile;
    char         buf[1024];
    CFPContext*  context;
    static int rv = 0;
    int i;

    if (current_command < -999)
    {
        printf("Illegal call of runPluginCommand, call find_plugin_command first.\n");
        return 1;
    }
    strcpy(buf, cf_get_maps_directory(CustomCommand[i].name));
    strcat(buf, ".py");

    context = malloc(sizeof(CFPContext));
    context->message[0]=0;

    context->who         = Crossfire_Object_wrap(op);
    context->fix         = 0;
    strcpy(context->script,buf);
    strcpy(context->options,params);
    context->returnvalue = 1; /* Default is "command successful" */

    Py_XINCREF(context->who);

    scriptfile = fopen(context->script,"r");
    if (scriptfile == NULL)
    {
        printf( "cfpython - The Script file %s can't be opened\n",context->script);
        return rv;
    }
    pushContext(context);
    PyRun_SimpleFile(scriptfile, context->script);
    fclose(scriptfile);
    context = popContext();
    rv = context->returnvalue;
    /*Py_XDECREF(context->who);
    Py_XDECREF(context->activator);
    Py_XDECREF(context->third);*/
    free(context);
    printf("Execution complete");
    return rv;
}

CF_PLUGIN int postInitPlugin()
{
    int hooktype = 1;
    int rtype = 0;
    FILE*   scriptfile;

    printf("CFPython 2.0a post init\n");
    registerGlobalEvent =   gethook(&rtype,hooktype,"cfapi_system_register_global_event");
    unregisterGlobalEvent = gethook(&rtype,hooktype,"cfapi_system_unregister_global_event");
    systemDirectory       = gethook(&rtype,hooktype,"cfapi_system_directory");
    reCmp                 = gethook(&rtype,hooktype,"cfapi_system_re_cmp");
    cf_init_plugin( gethook );
    initContextStack();
    registerGlobalEvent(NULL,EVENT_BORN,PLUGIN_NAME,globalEventListener);
    /*registerGlobalEvent(NULL,EVENT_CLOCK,PLUGIN_NAME,globalEventListener);*/
    /*registerGlobalEvent(NULL,EVENT_CRASH,PLUGIN_NAME,globalEventListener);*/
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
    registerGlobalEvent(NULL,EVENT_KICK,PLUGIN_NAME,globalEventListener);

    scriptfile = fopen(cf_get_maps_directory("python/events/python_init.py"),"r");
    if (scriptfile != NULL)
    {
        PyRun_SimpleFile(scriptfile, cf_get_maps_directory("python/events/python_init.py"));
        fclose(scriptfile);
    }

    return 0;
}

CF_PLUGIN void* globalEventListener(int* type, ...)
{
    va_list args;
    static int rv=0;
    FILE*   scriptfile;
    CFPContext* context;
    Crossfire_Player* cfpl;
    Crossfire_Object* cfob;
    char* buf;
    player* pl;
    object* op;
    context = malloc(sizeof(CFPContext));

    va_start(args, type);
    context->event_code = va_arg(args, int);

    context->message[0]=0;

    context->who         = NULL;
    context->activator   = NULL;
    context->third       = NULL;
    rv = context->returnvalue = 0;
    switch(context->event_code)
    {
        case EVENT_CRASH:
            printf( "Unimplemented for now\n");
            break;
        case EVENT_BORN:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            strcpy(context->script,cf_get_maps_directory("python/events/python_born.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_PLAYER_DEATH:
            op = va_arg(args, object*);
            context->who = Crossfire_Object_wrap(op);
            strcpy(context->script,cf_get_maps_directory("python/events/python_player_death.py"));
            cfob = (Crossfire_Object*)context->who;
            break;
        case EVENT_GKILL:
            op = va_arg(args, object*);
            context->who = Crossfire_Object_wrap(op);
            context->activator = Crossfire_Object_wrap(op);
            strcpy(context->script,cf_get_maps_directory("python/events/python_gkill.py"));
            cfob = (Crossfire_Object*)context->who;
            break;
        case EVENT_LOGIN:
            pl = va_arg(args, player*);
            context->activator = Crossfire_Object_wrap(pl->ob);
            buf = va_arg(args, char*);
            if (buf !=NULL)
                strcpy(context->message,buf);
            strcpy(context->script,cf_get_maps_directory("python/events/python_login.py"));
            cfpl = (Crossfire_Player*)context->activator;
            break;
        case EVENT_LOGOUT:
            pl = va_arg(args, player*);
            context->activator = Crossfire_Object_wrap(pl->ob);
            buf = va_arg(args, char*);
            if (buf !=NULL)
                strcpy(context->message,buf);
            strcpy(context->script,cf_get_maps_directory("python/events/python_logout.py"));
            cfpl = (Crossfire_Player*)context->activator;
            break;
        case EVENT_REMOVE:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            strcpy(context->script,cf_get_maps_directory("python/events/python_remove.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_SHOUT:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            buf = va_arg(args, char*);
            if (buf !=NULL)
                strcpy(context->message,buf);
            strcpy(context->script,cf_get_maps_directory("python/events/python_shout.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_MUZZLE:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            buf = va_arg(args, char*);
            if (buf !=NULL)
                strcpy(context->message,buf);
            strcpy(context->script,cf_get_maps_directory("python/events/python_muzzle.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_KICK:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            buf = va_arg(args, char*);
            if (buf !=NULL)
                strcpy(context->message,buf);
            strcpy(context->script,cf_get_maps_directory("python/events/python_kick.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_MAPENTER:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            strcpy(context->script,cf_get_maps_directory("python/events/python_mapenter.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_MAPLEAVE:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            strcpy(context->script,cf_get_maps_directory("python/events/python_mapleave.py"));
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_CLOCK:
            strcpy(context->script,cf_get_maps_directory("python/events/python_clock.py"));
            break;
        case EVENT_MAPRESET:
            buf = va_arg(args, char*);
            if (buf !=NULL)
                strcpy(context->message,buf);
            strcpy(context->script,cf_get_maps_directory("python/events/python_mapreset.py"));
            break;
        case EVENT_TELL:
            strcpy(context->script,cf_get_maps_directory("python/events/python_tell.py"));
            break;
    }
    va_end(args);
    context->returnvalue = 0;

    Py_XINCREF(context->who);
    Py_XINCREF(context->activator);
    Py_XINCREF(context->third);

    scriptfile = fopen(context->script,"r");
    if (scriptfile == NULL)
    {
        printf( "cfpython - The Script file %s can't be opened\n",context->script);
        return &rv;
    }
    pushContext(context);
    PyRun_SimpleFile(scriptfile, context->script);
    fclose(scriptfile);
    context = popContext();
    rv = context->returnvalue;
    /*Py_XDECREF(context->who);
    Py_XDECREF(context->activator);
    Py_XDECREF(context->third);*/
    free(context);

    return &rv;
}

CF_PLUGIN void* eventListener(int* type, ...)
{
    static int rv=0;
    va_list args;
    char* buf;
    FILE*   scriptfile;
    CFPContext* context;

    context = malloc(sizeof(CFPContext));

    context->message[0]=0;

    va_start(args,type);

    context->who         = Crossfire_Object_wrap(va_arg(args, object*));
    context->event_code  = va_arg(args,int);
    context->activator   = Crossfire_Object_wrap(va_arg(args, object*));
    context->third       = Crossfire_Object_wrap(va_arg(args, object*));
    buf = va_arg(args, char*);
    if (buf !=0)
        strcpy(context->message,buf);
    context->fix         = va_arg(args, int);
    strcpy(context->script,cf_get_maps_directory(va_arg(args, char*)));
    strcpy(context->options,va_arg(args, char*));
    context->returnvalue = 0;

    Py_XINCREF(context->who);
    Py_XINCREF(context->activator);
    Py_XINCREF(context->third);

    va_end(args);
    scriptfile = fopen(context->script,"r");
    if (scriptfile == NULL)
    {
        printf( "cfpython - The Script file %s can't be opened\n",context->script);
        return &rv;
    }
    pushContext(context);
    PyRun_SimpleFile(scriptfile, context->script);
    fclose(scriptfile);
    context = popContext();
    rv = context->returnvalue;
    /*Py_XDECREF(context->who);
    Py_XDECREF(context->activator);
    Py_XDECREF(context->third);*/
    free(context);
    return &rv;
}

CF_PLUGIN int   closePlugin()
{
    printf("CFPython 2.0a closing\n");
    return 0;
}


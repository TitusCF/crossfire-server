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
/*                                                                           */
/*****************************************************************************/

/* First let's include the header file needed                                */

#include <cfpython.h>
#include <stdarg.h>

#define PYTHON_DEBUG   /* give us some general infos out */
#define PYTHON_CACHE_SIZE 16    /* number of python scripts to store the bytecode of at a time */

typedef struct {
    char *file;
    PyCodeObject *code;
    time_t cached_time, used_time;
} pycode_cache_entry;

static pycode_cache_entry pycode_cache[PYTHON_CACHE_SIZE];

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
static PyObject* getPrivateDictionary(PyObject* self, PyObject* args);
static PyObject* getSharedDictionary(PyObject* self, PyObject* args);
static PyObject* getArchetypes(PyObject* self, PyObject* args);
static PyObject* getMaps(PyObject* self, PyObject* args);
static PyObject* getParties(PyObject* self, PyObject* args);
static PyObject* getRegions(PyObject* self, PyObject* args);
static PyObject* registerCommand(PyObject* self, PyObject* args);
static PyObject* registerGEvent(PyObject* self, PyObject* args);
static PyObject* unregisterGEvent(PyObject* self, PyObject* args);
static PyObject* CFPythonError;
static PyObject* getTime(PyObject* self, PyObject* args);
static PyObject* destroyTimer(PyObject* self, PyObject* args);

/** Set up an Python exception object. */
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
    {"GetReturnValue",      getReturnValue,         METH_VARARGS},
    {"SetReturnValue",      setReturnValue,         METH_VARARGS},
    {"PluginVersion",       getCFPythonVersion,     METH_VARARGS},
    {"CreateObject",        createCFObject,         METH_VARARGS},
    {"CreateObjectByName",  createCFObjectByName,   METH_VARARGS},
    {"GetPrivateDictionary", getPrivateDictionary,  METH_VARARGS},
    {"GetSharedDictionary",  getSharedDictionary,   METH_VARARGS},
    {"GetArchetypes",       getArchetypes,          METH_VARARGS},
    {"GetMaps",             getMaps,                METH_VARARGS},
    {"GetParties",          getParties,             METH_VARARGS},
    {"GetRegions",          getRegions,             METH_VARARGS},
    {"RegisterCommand",     registerCommand,        METH_VARARGS},
    {"RegisterGlobalEvent", registerGEvent,         METH_VARARGS},
    {"UnregisterGlobalEvent",unregisterGEvent,      METH_VARARGS},
    {"GetTime",             getTime,                METH_VARARGS},
    {"DestroyTimer",        destroyTimer,           METH_VARARGS},
    {NULL, NULL, 0}
};

CFPContext* context_stack;
CFPContext* current_context;
static int current_command = -999;
static PyObject* shared_data = NULL;
static PyObject* private_data = NULL;

static PyObject* registerGEvent(PyObject* self, PyObject* args)
{
    int eventcode;
    if (!PyArg_ParseTuple(args, "i", &eventcode))
        return NULL;

    registerGlobalEvent(NULL, eventcode, PLUGIN_NAME, globalEventListener);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject* unregisterGEvent(PyObject* self, PyObject* args)
{
    int eventcode;
    if (!PyArg_ParseTuple(args, "i", &eventcode))
        return NULL;

    unregisterGlobalEvent(NULL, EVENT_TELL, PLUGIN_NAME);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* createCFObject(PyObject* self, PyObject* args)
{
    object* op;
    op = cf_create_object();

    return Crossfire_Object_wrap(op);
}
static PyObject* createCFObjectByName(PyObject* self, PyObject* args)
{
    char* obname;
    object* op;

    if (!PyArg_ParseTuple(args, "s", &obname))
        return NULL;

    op = cf_create_object_by_name(obname);

    return Crossfire_Object_wrap(op);
}
static PyObject* getCFPythonVersion(PyObject* self, PyObject* args)
{
    int i = 2044;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getReturnValue(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", current_context->returnvalue);
}
static PyObject* setReturnValue(PyObject* self, PyObject* args)
{
    int i;
    if (!PyArg_ParseTuple(args, "i", &i))
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
    if (!PyArg_ParseTuple(args, "ss", &premiere, &seconde))
        return NULL;

    result = reCmp( &val, premiere, seconde );
    if (result != NULL)
        return Py_BuildValue("i", 1);
    else
        return Py_BuildValue("i", 0);
}
static PyObject* findPlayer(PyObject* self, PyObject* args)
{
    player *foundpl;
    char* txt;

    if (!PyArg_ParseTuple(args, "s", &txt))
        return NULL;

    foundpl = cf_player_find(txt);

    if (foundpl != NULL)
        return Py_BuildValue("O", Crossfire_Object_wrap(foundpl->ob));
    else {
        Py_INCREF(Py_None);
        return Py_None;
    }
}
static PyObject* readyMap(PyObject* self, PyObject* args)
{
    char* mapname;
    mapstruct* map;

    if (!PyArg_ParseTuple(args, "s", &mapname))
        return NULL;

    map = cf_map_get_map(mapname);

    return Crossfire_Map_wrap(map);
}
static PyObject* getCostFlagTrue(PyObject* self, PyObject* args)
{
    int i = F_TRUE;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getCostFlagBuy(PyObject* self, PyObject* args)
{
    int i = F_BUY;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getCostFlagSell(PyObject* self, PyObject* args)
{
    int i = F_SELL;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getCostFlagNoBargain(PyObject* self, PyObject* args)
{
    int i = F_NO_BARGAIN;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getCostFlagIdentified(PyObject* self, PyObject* args)
{
    int i = F_IDENTIFIED;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getCostFlagNotCursed(PyObject* self, PyObject* args)
{
    int i = F_NOT_CURSED;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionNorthEast(PyObject* self, PyObject* args)
{
    int i = 2;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionEast(PyObject* self, PyObject* args)
{
    int i = 3;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionSouthEast(PyObject* self, PyObject* args)
{
    int i = 4;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionSouth(PyObject* self, PyObject* args)
{
    int i = 5;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionSouthWest(PyObject* self, PyObject* args)
{
    int i = 6;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionWest(PyObject* self, PyObject* args)
{
    int i = 7;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionNorthWest(PyObject* self, PyObject* args)
{
    int i = 8;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getDirectionNorth(PyObject* self, PyObject* args)
{
    int i = 1;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("i", i);
}
static PyObject* getMapDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 0));
}
static PyObject* getUniqueDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 1));
}
static PyObject* getTempDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 2));
}
static PyObject* getConfigDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 3));
}
static PyObject* getLocalDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 4));
}
static PyObject* getPlayerDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 5));
}
static PyObject* getDataDirectory(PyObject* self, PyObject* args)
{
    int rv;
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", systemDirectory(&rv, 6));
}
static PyObject* getWhoAmI(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    if (!current_context->who) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    Py_INCREF(current_context->who);
    return current_context->who;
}
static PyObject* getWhoIsActivator(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    if (!current_context->activator) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    Py_INCREF(current_context->activator);
    return current_context->activator;
}
static PyObject* getWhoIsThird(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    if (!current_context->third) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    Py_INCREF(current_context->third);
    return current_context->third;
}
static PyObject* getWhatIsMessage(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;

    if (current_context->message == NULL)
        return Py_BuildValue("");
    else
        return Py_BuildValue("s", current_context->message);
}
static PyObject* getScriptName(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", current_context->script);
}
static PyObject* getScriptParameters(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;
    return Py_BuildValue("s", current_context->options);
}

static PyObject* getPrivateDictionary(PyObject* self, PyObject* args)
{
    PyObject* data;

    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;

    data = PyDict_GetItemString(private_data, current_context->script);
    if (!data) {
        data = PyDict_New();
        PyDict_SetItemString(private_data, current_context->script, data);
        Py_DECREF(data);
    }
    Py_INCREF(data);
    return data;
}

static PyObject* getSharedDictionary(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;

    Py_INCREF(shared_data);
    return shared_data;
}

static PyObject* getArchetypes(PyObject* self, PyObject* args)
{
    PyObject* list;
    archetype* arch;

    list = PyList_New(0);
    arch = cf_archetype_get_first();
    while (arch) {
        PyList_Append(list, Crossfire_Archetype_wrap(arch));
        arch = cf_archetype_get_next(arch);
    }
    return list;
}

static PyObject* getMaps(PyObject* self, PyObject* args)
{
    PyObject* list;
    mapstruct* map;

    list = PyList_New(0);
    map = cf_map_get_first();
    while (map) {
        PyList_Append(list, Crossfire_Map_wrap(map));
        map = cf_map_get_property(map, CFAPI_MAP_PROP_NEXT);
    }
    return list;
}

static PyObject* getParties(PyObject* self, PyObject* args)
{
    PyObject* list;
    partylist* party;

    list = PyList_New(0);
    party = cf_party_get_first();
    while (party) {
        PyList_Append(list, Crossfire_Party_wrap(party));
        party = cf_party_get_next(party);
    }
    return list;
}

static PyObject* getRegions(PyObject* self, PyObject* args)
{
    PyObject *list;
    region *reg;

    list = PyList_New(0);
    reg = cf_region_get_first();
    while (reg) {
        PyList_Append(list, Crossfire_Region_wrap(reg));
        reg = cf_region_get_next(reg);
   }
   return list;
}

static PyObject* registerCommand(PyObject* self, PyObject* args)
{
    char *cmdname;
    char *scriptname;
    double cmdspeed;
    int i;

    if (!PyArg_ParseTuple(args, "ssd", &cmdname, &scriptname, &cmdspeed))
        return NULL;

    if (cmdspeed < 0) {
        set_exception("speed must not be negative");
        return NULL;
    }

    for (i = 0; i < NR_CUSTOM_CMD; i++) {
        if (CustomCommand[i].name != NULL) {
            if (!strcmp(CustomCommand[i].name, cmdname)) {
                set_exception("command '%s' is already registered", cmdname);
                return NULL;
            }
        }
    }
    for (i = 0; i < NR_CUSTOM_CMD; i++) {
        if (CustomCommand[i].name == NULL) {
            CustomCommand[i].name = cf_strdup_local(cmdname);
            CustomCommand[i].script = cf_strdup_local(scriptname);
            CustomCommand[i].speed = cmdspeed;
            break;
        }
    };

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* getTime(PyObject* self, PyObject* args)
{
    PyObject* list;
    partylist* party;
    timeofday_t tod;

    if (!PyArg_ParseTuple(args, "", NULL))
        return NULL;

    cf_get_time(&tod);

    list = PyList_New(0);
    PyList_Append(list, Py_BuildValue("i",tod.year));
    PyList_Append(list, Py_BuildValue("i",tod.month));
    PyList_Append(list, Py_BuildValue("i",tod.day));
    PyList_Append(list, Py_BuildValue("i",tod.hour));
    PyList_Append(list, Py_BuildValue("i",tod.minute));
    PyList_Append(list, Py_BuildValue("i",tod.dayofweek));
    PyList_Append(list, Py_BuildValue("i",tod.weekofmonth));
    PyList_Append(list, Py_BuildValue("i",tod.season));

    return list;
    }

static PyObject* destroyTimer(PyObject* self, PyObject* args)
{
    int id;
    if (!PyArg_ParseTuple(args, "i", &id))
        return NULL;
    return Py_BuildValue("i", cf_timer_destroy(id));
}

void initContextStack()
{
    current_context = NULL;
    context_stack = NULL;
}

void pushContext(CFPContext* context)
{
    if (current_context == NULL) {
        context_stack = context;
        context->down = NULL;
    } else {
        context->down = current_context;
    }
    current_context = context;
}

CFPContext* popContext()
{
    CFPContext* oldcontext;
    if (current_context != NULL) {
        oldcontext = current_context;
        current_context = current_context->down;
        return oldcontext;
    }
    else
        return NULL;
}

void freeContext(CFPContext* context)
{
    Py_XDECREF(context->who);
    Py_XDECREF(context->activator);
    Py_XDECREF(context->third);
    free(context);
}

/* Outputs the compiled bytecode for a given python file, using in-memory caching of bytecode */
static PyCodeObject *compilePython(char *filename) {
    FILE*   scriptfile;
    char  *sh_path;
    struct  stat stat_buf;
    struct _node *n;
    int i;
    pycode_cache_entry *replace = NULL, *run = NULL;

    if (!(scriptfile = fopen(filename, "r"))) {
        cf_log(llevDebug, "cfpython - The Script file %s can't be opened\n", filename);
        return NULL;
    }
    if(fstat(fileno(scriptfile), &stat_buf)) {
        cf_log(llevDebug, "cfpython - The Script file %s can't be stat:ed\n", filename);
        if(scriptfile)
            fclose(scriptfile);
        return NULL;
    }

    sh_path = cf_add_string(filename);

    /* Search through cache. Three cases:
     * 1) script in cache, but older than file  -> replace cached
     * 2) script in cache and up to date        -> use cached
     * 3) script not in cache, cache not full   -> add to end of cache
     * 4) script not in cache, cache full       -> replace least recently used
     */
    for(i=0; i < PYTHON_CACHE_SIZE; i++) {
        if(pycode_cache[i].file == NULL) {  /* script not in cache, cache not full */
            replace = &pycode_cache[i];     /* add to end of cache */
            break;
        } else if(pycode_cache[i].file == sh_path) {
            /* script in cache */
            if(pycode_cache[i].code == NULL || (pycode_cache[i].cached_time<stat_buf.st_mtime)) {
                /* cache older than file, replace cached */
                replace = &pycode_cache[i];
            } else {
                /* cache uptodate, use cached*/
                replace = NULL;
                run = &pycode_cache[i];
            }
            break;
        } else if(replace == NULL || pycode_cache[i].used_time < replace->used_time)
            /* if we haven't found it yet, set replace to the oldest cache */
            replace = &pycode_cache[i];
    }

    /* replace a specific cache index with the file */
    if(replace) {
        Py_XDECREF(replace->code); /* safe to call on NULL */
        replace->code = NULL;

        /* Need to replace path string? */
        if (replace->file != sh_path) {
            if(replace->file) {
                cf_free_string(replace->file);
            }
            replace->file = cf_add_string(sh_path);
        }

        /* Load, parse and compile */
        if (!scriptfile && !(scriptfile = fopen(filename, "r"))) {
            cf_log(llevDebug, "cfpython - The Script file %s can't be opened\n", filename);
            replace->code = NULL;
            return NULL;
        } else {
            if((n = PyParser_SimpleParseFile (scriptfile, filename, Py_file_input))) {
                replace->code = PyNode_Compile(n, filename);
                PyNode_Free (n);
            }

            if(PyErr_Occurred())
                PyErr_Print();
            else
                replace->cached_time = stat_buf.st_mtime;
            run = replace;
        }
    }

    cf_free_string(sh_path);

    if(scriptfile)
        fclose(scriptfile);

    if (run)
        return run->code;
    else
        return NULL;
}


static int do_script(CFPContext* context, int silent)
{
    PyCodeObject *pycode;
    PyObject* dict;
    PyObject* ret;
#if 0
    PyObject* list;
    int item;
#endif

    pycode = compilePython(context->script);
    if (pycode) {
        pushContext(context);
        dict = PyDict_New();
        PyDict_SetItemString(dict, "__builtins__", PyEval_GetBuiltins());
        ret = PyEval_EvalCode(pycode, dict, NULL);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        Py_XDECREF(ret);
#if 0
        printf( "cfpython - %d items in heap\n", PyDict_Size(dict));
        list = PyDict_Values(dict);
        for (item = PyList_Size(list) - 1; item >= 0; item--) {
            dict = PyList_GET_ITEM(list, item);
            ret = PyObject_Str(dict);
            printf(" ref %s = %d\n", PyString_AsString(ret), dict->ob_refcnt);
            Py_XDECREF(ret);
        }
        Py_DECREF(list);
#endif
        Py_DECREF(dict);
        return 1;
    } else
        return 0;
}

typedef struct
{
    char* name;
    int value;
} CFConstant;

static void addConstants(PyObject* module, const char* name, CFConstant* constants)
{
    int i = 0;
    char tmp[1024];
    PyObject* new;
    PyObject* dict;

    strncpy(tmp, "Crossfire_", 1024);
    strncat(tmp, name, 1024 - strlen(tmp));

    new = Py_InitModule(tmp, NULL);
    dict = PyDict_New();

    while ( constants[i].name != NULL)
    {
        PyModule_AddIntConstant(new, constants[i].name, constants[i].value);
        PyDict_SetItem(dict, PyInt_FromLong(constants[i].value), PyString_FromString(constants[i].name));
        i++;
    }
    PyDict_SetItemString(PyModule_GetDict(module), name, new);
    Py_DECREF(new);

    strncpy(tmp, name, 1024);
    strncat(tmp, "Name", 1024 - strlen(tmp));
    PyDict_SetItemString(PyModule_GetDict(module), tmp, dict);
    Py_DECREF(dict);
}

static void initConstants(PyObject* module)
{
    static CFConstant cstDirection[] = {
        { "NORTH", 1 },
        { "NORTHEAST", 2 },
        { "EAST", 3 },
        { "SOUTHEAST", 4 },
        { "SOUTH", 5 },
        { "SOUTHWEST", 6 },
        { "WEST", 7 },
        { "NORTHWEST", 8 },
        { NULL, 0 } };

    static CFConstant cstType[] = {
        { "PLAYER", PLAYER },
        { "TRANSPORT", TRANSPORT },
        { "ROD", ROD },
        { "TREASURE", TREASURE },
        { "POTION", POTION },
        { "FOOD", FOOD },
        { "POISON", POISON },
        { "BOOK", BOOK },
        { "CLOCK", CLOCK },
        { "LIGHTNING", LIGHTNING },
        { "ARROW", ARROW },
        { "BOW", BOW },
        { "WEAPON", WEAPON },
        { "ARMOUR", ARMOUR },
        { "PEDESTAL", PEDESTAL },
        { "ALTAR", ALTAR },
        { "LOCKED_DOOR", LOCKED_DOOR },
        { "SPECIAL_KEY", SPECIAL_KEY },
        { "MAP", MAP },
        { "DOOR", DOOR },
        { "KEY", KEY },
        { "TIMED_GATE", TIMED_GATE },
        { "TRIGGER", TRIGGER },
        { "GRIMREAPER", GRIMREAPER },
        { "MAGIC_EAR", MAGIC_EAR },
        { "TRIGGER_BUTTON", TRIGGER_BUTTON },
        { "TRIGGER_ALTAR", TRIGGER_ALTAR },
        { "TRIGGER_PEDESTAL", TRIGGER_PEDESTAL },
        { "SHIELD", SHIELD },
        { "HELMET", HELMET },
        { "HORN", HORN },
        { "MONEY", MONEY },
        { "CLASS", CLASS },
        { "GRAVESTONE", GRAVESTONE },
        { "AMULET", AMULET },
        { "PLAYERMOVER", PLAYERMOVER },
        { "TELEPORTER", TELEPORTER },
        { "CREATOR", CREATOR },
        { "SKILL", SKILL },
        { "EXPERIENCE", EXPERIENCE },
        { "EARTHWALL", EARTHWALL },
        { "GOLEM", GOLEM },
        { "THROWN_OBJ", THROWN_OBJ },
        { "BLINDNESS", BLINDNESS },
        { "GOD", GOD },
        { "DETECTOR", DETECTOR },
        { "TRIGGER_MARKER", TRIGGER_MARKER },
        { "DEAD_OBJECT", DEAD_OBJECT },
        { "DRINK", DRINK },
        { "MARKER", MARKER },
        { "HOLY_ALTAR", HOLY_ALTAR },
        { "PLAYER_CHANGER", PLAYER_CHANGER },
        { "BATTLEGROUND", BATTLEGROUND },
        { "PEACEMAKER", PEACEMAKER },
        { "GEM", GEM },
        { "FIREWALL", FIREWALL },
        { "ANVIL", ANVIL },
        { "CHECK_INV", CHECK_INV },
        { "MOOD_FLOOR", MOOD_FLOOR },
        { "EXIT", EXIT },
        { "ENCOUNTER", ENCOUNTER },
        { "SHOP_FLOOR", SHOP_FLOOR },
        { "SHOP_MAT", SHOP_MAT },
        { "RING", RING },
        { "FLOOR", FLOOR },
        { "FLESH", FLESH },
        { "INORGANIC", INORGANIC },
        { "SKILL_TOOL", SKILL_TOOL },
        { "LIGHTER", LIGHTER },
        { "TRAP_PART", TRAP_PART },
        { "WALL", WALL },
        { "LIGHT_SOURCE", LIGHT_SOURCE },
        { "MISC_OBJECT", MISC_OBJECT },
        { "MONSTER", MONSTER },
        { "SPAWN_GENERATOR", SPAWN_GENERATOR },
        { "LAMP", LAMP },
        { "DUPLICATOR", DUPLICATOR },
        { "TOOL", TOOL },
        { "SPELLBOOK", SPELLBOOK },
        { "BUILDFAC", BUILDFAC },
        { "CLOAK", CLOAK },
        { "SPINNER", SPINNER },
        { "GATE", GATE },
        { "BUTTON", BUTTON },
        { "CF_HANDLE", CF_HANDLE },
        { "HOLE", HOLE },
        { "TRAPDOOR", TRAPDOOR },
        { "SIGN", SIGN },
        { "BOOTS", BOOTS },
        { "GLOVES", GLOVES },
        { "SPELL", SPELL },
        { "SPELL_EFFECT", SPELL_EFFECT },
        { "CONVERTER", CONVERTER },
        { "BRACERS", BRACERS },
        { "POISONING", POISONING },
        { "SAVEBED", SAVEBED },
        { "POISONCLOUD", POISONCLOUD },
        { "FIREHOLES", FIREHOLES },
        { "WAND", WAND },
        { "SCROLL", SCROLL },
        { "DIRECTOR", DIRECTOR },
        { "GIRDLE", GIRDLE },
        { "FORCE", FORCE },
        { "POTION_EFFECT", POTION_EFFECT },
        { "EVENT_CONNECTOR", EVENT_CONNECTOR },
        { "CLOSE_CON", CLOSE_CON },
        { "CONTAINER", CONTAINER },
        { "ARMOUR_IMPROVER", ARMOUR_IMPROVER },
        { "WEAPON_IMPROVER", WEAPON_IMPROVER },
        { "SKILLSCROLL", SKILLSCROLL },
        { "DEEP_SWAMP", DEEP_SWAMP },
        { "IDENTIFY_ALTAR", IDENTIFY_ALTAR },
        { "MENU", MENU },
        { "RUNE", RUNE },
        { "TRAP", TRAP },
        { "POWER_CRYSTAL", POWER_CRYSTAL },
        { "CORPSE", CORPSE },
        { "DISEASE", DISEASE },
        { "SYMPTOM", SYMPTOM },
        { "BUILDER", BUILDER },
        { "MATERIAL", MATERIAL },
        { "ITEM_TRANSFORMER", ITEM_TRANSFORMER },
        { "QUEST", QUEST },
        { NULL, 0 } };

    static CFConstant cstMove[] = {
        { "WALK", MOVE_WALK },
        { "FLY_LOW", MOVE_FLY_LOW },
        { "FLY_HIGH", MOVE_FLY_HIGH },
        { "FLYING", MOVE_FLYING },
        { "SWIM", MOVE_SWIM },
        { "BOAT", MOVE_BOAT },
        { "ALL", MOVE_ALL },
        { NULL, 0 } };

    addConstants(module, "Direction", cstDirection);
    addConstants(module, "Type", cstType);
    addConstants(module, "Move", cstMove);
}

CF_PLUGIN int initPlugin(const char* iversion, f_plug_api gethooksptr)
{
    PyObject *m, *d;
    int i;
    gethook = gethooksptr;
    cf_init_plugin( gethook );
    cf_log(llevDebug, "CFPython 2.0a init\n");

    init_object_assoc_table();
    init_map_assoc_table();

    Py_Initialize();
    Crossfire_ObjectType.tp_new = PyType_GenericNew;
    Crossfire_MapType.tp_new    = PyType_GenericNew;
    Crossfire_PlayerType.tp_new = PyType_GenericNew;
    Crossfire_ArchetypeType.tp_new = PyType_GenericNew;
    Crossfire_PartyType.tp_new = PyType_GenericNew;
    Crossfire_RegionType.tp_new = PyType_GenericNew;
    PyType_Ready(&Crossfire_ObjectType);
    PyType_Ready(&Crossfire_MapType);
    PyType_Ready(&Crossfire_PlayerType);
    PyType_Ready(&Crossfire_ArchetypeType);
    PyType_Ready(&Crossfire_PartyType);
    PyType_Ready(&Crossfire_RegionType);

    m = Py_InitModule("Crossfire", CFPythonMethods);
    d = PyModule_GetDict(m);
    Py_INCREF(&Crossfire_ObjectType);
    Py_INCREF(&Crossfire_MapType);
    Py_INCREF(&Crossfire_PlayerType);
    Py_INCREF(&Crossfire_ArchetypeType);
    Py_INCREF(&Crossfire_PartyType);
    Py_INCREF(&Crossfire_RegionType);

    PyModule_AddObject(m, "Object", (PyObject*)&Crossfire_ObjectType);
    PyModule_AddObject(m, "Map", (PyObject*)&Crossfire_MapType);
    PyModule_AddObject(m, "Player", (PyObject*)&Crossfire_PlayerType);
    PyModule_AddObject(m, "Archetype", (PyObject*)&Crossfire_ArchetypeType);
    PyModule_AddObject(m, "Party", (PyObject*)&Crossfire_PartyType);
    PyModule_AddObject(m, "Region", (PyObject*)&Crossfire_RegionType);

    CFPythonError = PyErr_NewException("Crossfire.error", NULL, NULL);
    PyDict_SetItemString(d, "error", CFPythonError);
    for (i = 0; i < NR_CUSTOM_CMD; i++) {
        CustomCommand[i].name   = NULL;
        CustomCommand[i].script = NULL;
        CustomCommand[i].speed  = 0.0;
    }
    initConstants(m);
    private_data = PyDict_New();
    shared_data = PyDict_New();
    return 0;
}

CF_PLUGIN void* getPluginProperty(int* type, ...)
{
    va_list args;
    const char* propname;
    int i;
    static command_array_struct rtn_cmd;

    va_start(args, type);
    propname = va_arg(args, const char *);

    if (!strcmp(propname, "command?")) {
        const char* cmdname;
        cmdname = va_arg(args, const char *);
        va_end(args);

        for (i = 0; i < NR_CUSTOM_CMD; i++) {
            if (CustomCommand[i].name != NULL) {
                if (!strcmp(CustomCommand[i].name, cmdname)) {
                    rtn_cmd.name = CustomCommand[i].name;
                    rtn_cmd.time = (float)CustomCommand[i].speed;
                    rtn_cmd.func = runPluginCommand;
                    current_command = i;
                    return &rtn_cmd;
                }
            }
        }
        return NULL;
    } else if (!strcmp(propname, "Identification")) {
        va_end(args);
        return PLUGIN_NAME;
    } else if (!strcmp(propname, "FullName")) {
        va_end(args);
        return PLUGIN_VERSION;
    }
    return NULL;
}

CF_PLUGIN int runPluginCommand(object* op, char* params)
{
    char         buf[1024];
    CFPContext*  context;
    static int rv = 0;

    rv = 0;

    if (current_command < 0) {
        cf_log(llevError, "Illegal call of runPluginCommand, call find_plugin_command first.\n");
        return 1;
    }
    snprintf(buf, sizeof(buf), "%s.py", cf_get_maps_directory(CustomCommand[current_command].script));

    context = malloc(sizeof(CFPContext));
    context->message[0] = 0;

    context->who         = Crossfire_Object_wrap(op);
    context->activator   = NULL;
    context->third       = NULL;
    context->fix         = 0;
    snprintf(context->script, sizeof(context->script), "%s", buf);
    snprintf(context->options, sizeof(context->options), "%s", params);
    context->returnvalue = 1; /* Default is "command successful" */

    current_command = -999;

    if (!do_script(context, 0)) {
        freeContext(context);
        return rv;
    }

    context = popContext();
    rv = context->returnvalue;
    freeContext(context);
/*    printf("Execution complete"); */
    return rv;
}

CF_PLUGIN int postInitPlugin()
{
    int hooktype = 1;
    int rtype = 0;
    FILE*   scriptfile;

    cf_log(llevDebug, "CFPython 2.0a post init\n");
    registerGlobalEvent =   gethook(&rtype, hooktype, "cfapi_system_register_global_event");
    unregisterGlobalEvent = gethook(&rtype, hooktype, "cfapi_system_unregister_global_event");
    systemDirectory       = gethook(&rtype, hooktype, "cfapi_system_directory");
    reCmp                 = gethook(&rtype, hooktype, "cfapi_system_re_cmp");
    initContextStack();
    registerGlobalEvent(NULL, EVENT_BORN, PLUGIN_NAME, globalEventListener);
    /*registerGlobalEvent(NULL, EVENT_CLOCK, PLUGIN_NAME, globalEventListener);*/
    /*registerGlobalEvent(NULL, EVENT_CRASH, PLUGIN_NAME, globalEventListener);*/
    registerGlobalEvent(NULL, EVENT_PLAYER_DEATH, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_GKILL, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_LOGIN, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_LOGOUT, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_MAPENTER, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_MAPLEAVE, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_MAPRESET, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_REMOVE, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_SHOUT, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_TELL, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_MUZZLE, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_KICK, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_MAPUNLOAD, PLUGIN_NAME, globalEventListener);
    registerGlobalEvent(NULL, EVENT_MAPLOAD, PLUGIN_NAME, globalEventListener);

    scriptfile = fopen(cf_get_maps_directory("python/events/python_init.py"), "r");
    if (scriptfile != NULL) {
        PyRun_SimpleFile(scriptfile, cf_get_maps_directory("python/events/python_init.py"));
        fclose(scriptfile);
    }

    return 0;
}

CF_PLUGIN void* globalEventListener(int* type, ...)
{
    va_list args;
    static int rv = 0;
    CFPContext* context;
    Crossfire_Player* cfpl;
    Crossfire_Object* cfob;
    char* buf;
    player* pl;
    object* op;
    context = malloc(sizeof(CFPContext));

    rv = 0;

    va_start(args, type);
    context->event_code = va_arg(args, int);

    context->message[0] = 0;

    context->who         = NULL;
    context->activator   = NULL;
    context->third       = NULL;
    rv = context->returnvalue = 0;
    snprintf(context->script, sizeof(context->script), "%s", cf_get_maps_directory("python/events/python_event.py"));
    strcpy(context->options, "");
    switch(context->event_code) {
        case EVENT_CRASH:
            cf_log(llevDebug, "Unimplemented for now\n");
            break;
        case EVENT_BORN:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            snprintf(context->options, sizeof(context->options), "born");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_PLAYER_DEATH:
            op = va_arg(args, object*);
            context->who = Crossfire_Object_wrap(op);
            snprintf(context->options, sizeof(context->options), "death");
            cfob = (Crossfire_Object*)context->who;
            break;
        case EVENT_GKILL:
            op = va_arg(args, object*);
            context->who = Crossfire_Object_wrap(op);
            context->activator = Crossfire_Object_wrap(op);
            snprintf(context->options, sizeof(context->options), "gkill");
            cfob = (Crossfire_Object*)context->who;
            break;
        case EVENT_LOGIN:
            pl = va_arg(args, player*);
            context->activator = Crossfire_Object_wrap(pl->ob);
            buf = va_arg(args, char*);
            if (buf != NULL)
                snprintf(context->message, sizeof(context->message), "%s", buf);
            snprintf(context->options, sizeof(context->options), "login");
            cfpl = (Crossfire_Player*)context->activator;
            break;
        case EVENT_LOGOUT:
            pl = va_arg(args, player*);
            context->activator = Crossfire_Object_wrap(pl->ob);
            buf = va_arg(args, char*);
            if (buf != NULL)
                snprintf(context->message, sizeof(context->message), "%s", buf);
            snprintf(context->options, sizeof(context->options), "logout");
            cfpl = (Crossfire_Player*)context->activator;
            break;
        case EVENT_REMOVE:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            snprintf(context->options, sizeof(context->options), "remove");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_SHOUT:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            buf = va_arg(args, char*);
            if (buf != NULL)
                snprintf(context->message, sizeof(context->message), "%s", buf);
            snprintf(context->options, sizeof(context->options), "shout");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_MUZZLE:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            buf = va_arg(args, char*);
            if (buf != NULL)
                snprintf(context->message, sizeof(context->message), "%s", buf);
            snprintf(context->options, sizeof(context->options), "muzzle");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_KICK:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            buf = va_arg(args, char*);
            if (buf != NULL)
                snprintf(context->message, sizeof(context->message), "%s", buf);
            snprintf(context->options, sizeof(context->options), "kick");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_MAPENTER:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            context->who = Crossfire_Map_wrap(va_arg(args, mapstruct*));
            snprintf(context->options, sizeof(context->options), "mapenter");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_MAPLEAVE:
            op = va_arg(args, object*);
            context->activator = Crossfire_Object_wrap(op);
            context->who = Crossfire_Map_wrap(va_arg(args, mapstruct*));
            snprintf(context->options, sizeof(context->options), "mapleave");
            cfob = (Crossfire_Object*)context->activator;
            break;
        case EVENT_CLOCK:
            snprintf(context->options, sizeof(context->options), "clock");
            break;
        case EVENT_MAPRESET:
            context->who = Crossfire_Map_wrap(va_arg(args, mapstruct*));
            snprintf(context->options, sizeof(context->options), "mapreset");
            break;
        case EVENT_TELL:
            snprintf(context->options, sizeof(context->options), "tell");
            break;
        case EVENT_MAPUNLOAD:
            context->who = Crossfire_Map_wrap(va_arg(args, mapstruct*));
            snprintf(context->options, sizeof(context->options), "mapunload");
            break;
        case EVENT_MAPLOAD:
            context->who = Crossfire_Map_wrap(va_arg(args, mapstruct*));
            snprintf(context->options, sizeof(context->options), "mapload");
            break;
    }
    va_end(args);
    context->returnvalue = 0;

    if (!do_script(context, 1)) {
        freeContext(context);
        return &rv;
    }

    context = popContext();
    rv = context->returnvalue;
    
    /* Invalidate freed map wrapper. */
    if (context->event_code == EVENT_MAPUNLOAD)
        Handle_Map_Unload_Hook(context->who);
    
    freeContext(context);
    
    return &rv;
}

CF_PLUGIN void* eventListener(int* type, ...)
{
    static int rv = 0;
    va_list args;
    char* buf;
    char* script_tmp;
    CFPContext* context;

    rv = 0;

    context = malloc(sizeof(CFPContext));

    context->message[0] = 0;

    va_start(args, type);

    context->who         = Crossfire_Object_wrap(va_arg(args, object*));
    context->event_code  = va_arg(args, int);
    context->activator   = Crossfire_Object_wrap(va_arg(args, object*));
    context->third       = Crossfire_Object_wrap(va_arg(args, object*));
    buf = va_arg(args, char*);
    if (buf != NULL)
        snprintf(context->message, sizeof(context->message), "%s", buf);
    context->fix         = va_arg(args, int);
    script_tmp = va_arg(args, char*);
    snprintf(context->script, sizeof(context->script), "%s", cf_get_maps_directory(script_tmp));
    snprintf(context->options, sizeof(context->options), "%s", va_arg(args, char*));
    context->returnvalue = 0;

    va_end(args);
    
    if ((context->event_code == EVENT_DESTROY) && !strcmp(script_tmp, "cfpython_auto_hook")) {
        Handle_Destroy_Hook(context->who);
        freeContext(context);
        return &rv;
    }

    if (!do_script(context, 0)) {
        freeContext(context);
        return &rv;
    }

    context = popContext();
    rv = context->returnvalue;
    freeContext(context);
    return &rv;
}

CF_PLUGIN int   closePlugin()
{
    cf_log(llevDebug, "CFPython 2.0a closing\n");
    Py_Finalize();
    return 0;
}

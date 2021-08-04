/*****************************************************************************/
/* CFPython - A Python module for Crossfire RPG.                             */
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
#ifndef PLUGIN_PYTHON_H
#define PLUGIN_PYTHON_H

/* First the required header files - only the CF module interface and Python */
#ifdef WIN32
    #ifdef _DEBUG
        #undef _DEBUG
        #include <Python.h>
        #define _DEBUG
    #else
        #include <Python.h>
    #endif
#else /* WIN32 */
    #include <Python.h>
#endif

/* This is for allowing specific features of Python 3
 * For example, Python 3.8 changes the typecasting on some stuff we use,
 * so make a check for that.
 */
#if PY_MAJOR_VERSION >= 3
#    if PY_MINOR_VERSION >= 3
#        define IS_PY3K3
#    endif
#    if PY_MINOR_VERSION >= 8
#        define IS_PY3K8
#    endif
#endif

/* Python 2.5 or older doesn't define these. */
#ifndef Py_SIZE
#    define Py_SIZE(ob)         (((PyVarObject*)(ob))->ob_size)
#endif
#ifndef Py_TYPE
#    define Py_TYPE(ob)         (((PyObject*)(ob))->ob_type)
#endif

/* Handle Bytes vs. String */
#define CF_IS_PYSTR(cfpy_obj) (PyUnicode_Check(cfpy_obj))

/* include compile.h from python. Python.h doesn't pull it in with versions
 * 2.3 and older, and it does have protection from double-imports.
 */
#include <compile.h>
#include <plugin.h>

#undef MODULEAPI
#ifdef WIN32
# ifdef PYTHON_PLUGIN_EXPORTS
#  define MODULEAPI __declspec(dllexport)
# else
#  define MODULEAPI __declspec(dllimport)
# endif
#else
#ifdef HAVE_VISIBILITY
# define MODULEAPI __attribute__((visibility("default")))
#else
# define MODULEAPI
#endif
#endif

#define PLUGIN_NAME    "Python"
#define PLUGIN_VERSION "CFPython Plugin 2.0a13 (Fido)"

#include <plugin_common.h>
#include <cfpython_object.h>
#include <cfpython_map.h>
#include <cfpython_archetype.h>
#include <cfpython_party.h>
#include <cfpython_region.h>

typedef struct _cfpcontext {
    struct _cfpcontext *down;
    PyObject   *who;
    PyObject   *activator;
    PyObject   *third;
    PyObject   *event;
    char        message[1024];
    int         fix;
    int         event_code;
    char        script[1024];
    char        options[1024];
    int         returnvalue;
    int         parms[5];
    struct talk_info    *talk;
} CFPContext;

extern f_plug_api gethook;

extern CFPContext *context_stack;

extern CFPContext *current_context;

#include <cfpython_proto.h>

#endif /* PLUGIN_PYTHON_H */

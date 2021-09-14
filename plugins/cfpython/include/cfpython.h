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

/* Handle Bytes vs. String */
#define CF_IS_PYSTR(cfpy_obj) (PyUnicode_Check(cfpy_obj))

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

/**
 * Macro to define a PyTypeObject.
 * It should make it simpler to adjust to various Python versions, one macro to change only.
 * Arguments are:
 * - NAME: object's name, like "Object" or "Map", without quotes
 * - DEALLOC: deallocation function, of type Python::destructor
 * - CONVERT: pointer to a PyNumberMethods variable
 * - HASH: hash, PyObject_HashNotImplemented or NULL (for Player only, to inherit)
 * - FLAGS: Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE or Py_TPFLAGS_DEFAULT (for Player)
 * - DOC: Base description, with quotes
 * - CMP: Python::richcmpfunc to compare 2 objects
 * - METHODS: array of PyMethodDef to declare functions
 * - GETSET: array of PyGetSetDef to declare attributes
 * - BASE: base class, only used for Player
 * - OBNEW: Python::newfunc to create a new object
 */
#if PY_VERSION_HEX == 0x030503F0
#define CF_PYTHON_OBJECT(NAME, DEALLOC, CONVERT, HASH, FLAGS, DOC, CMP, METHODS, GETSET, BASE, OBNEW) \
PyTypeObject Crossfire_ ## NAME ## Type = { \
    /* See http://bugs.python.org/issue4385 */ \
    PyVarObject_HEAD_INIT(NULL, 0) \
    "Crossfire." #NAME,           /* tp_name*/ \
    sizeof(Crossfire_ ## NAME),   /* tp_basicsize*/ \
    0,                            /* tp_itemsize*/ \
    DEALLOC,                      /* tp_dealloc*/ \
    (printfunc)NULL,              /* tp_print*/ \
    NULL,                         /* tp_getattr*/ \
    NULL,                         /* tp_setattr*/ \
    NULL,                         /* tp_reserved */ \
    NULL,                         /* tp_repr*/ \
    CONVERT,                      /* tp_as_number*/ \
    NULL,                         /* tp_as_sequence*/ \
    NULL,                         /* tp_as_mapping*/ \
    HASH,                         /* tp_hash */ \
    NULL,                         /* tp_call*/ \
    NULL,                         /* tp_str*/ \
    PyObject_GenericGetAttr,      /* tp_getattro*/ \
    PyObject_GenericSetAttr,      /* tp_setattro*/ \
    NULL,                         /* tp_as_buffer*/ \
    FLAGS,                        /* tp_flags*/ \
    DOC,                          /* tp_doc */ \
    NULL,                         /* tp_traverse */ \
    NULL,                         /* tp_clear */ \
    CMP,                          /* tp_richcompare */ \
    0,                            /* tp_weaklistoffset */ \
    NULL,                         /* tp_iter */ \
    NULL,                         /* tp_iternext */ \
    METHODS,                      /* tp_methods */ \
    NULL,                         /* tp_members */ \
    GETSET,                       /* tp_getset */ \
    BASE,                         /* tp_base */ \
    NULL,                         /* tp_dict */ \
    NULL,                         /* tp_descr_get */ \
    NULL,                         /* tp_descr_set */ \
    0,                            /* tp_dictoffset */ \
    NULL,                         /* tp_init */ \
    NULL,                         /* tp_alloc */ \
    OBNEW,                        /* tp_new */ \
    NULL,                         /* tp_free */ \
    NULL,                         /* tp_is_gc */ \
    NULL,                         /* tp_bases */ \
    NULL,                         /* tp_mro */ \
    NULL,                         /* tp_cache */ \
    NULL,                         /* tp_subclasses */ \
    NULL,                         /* tp_weaklist */ \
    NULL,                         /* tp_del */ \
    0,                            /* tp_version_tag */ \
    NULL                          /* tp_finalize */ \
}
#else
#define CF_PYTHON_OBJECT(NAME, DEALLOC, CONVERT, HASH, FLAGS, DOC, CMP, METHODS, GETSET, BASE, OBNEW) \
PyTypeObject Crossfire_ ## NAME ## Type = { \
    /* See http://bugs.python.org/issue4385 */ \
    PyVarObject_HEAD_INIT(NULL, 0) \
    "Crossfire." #NAME,           /* tp_name*/ \
    sizeof(Crossfire_ ## NAME),   /* tp_basicsize*/ \
    0,                            /* tp_itemsize*/ \
    DEALLOC,                      /* tp_dealloc*/ \
    (printfunc)NULL,              /* tp_print*/ \
    NULL,                         /* tp_getattr*/ \
    NULL,                         /* tp_setattr*/ \
    NULL,                         /* tp_reserved */ \
    NULL,                         /* tp_repr*/ \
    CONVERT,                      /* tp_as_number*/ \
    NULL,                         /* tp_as_sequence*/ \
    NULL,                         /* tp_as_mapping*/ \
    HASH,                         /* tp_hash */ \
    NULL,                         /* tp_call*/ \
    NULL,                         /* tp_str*/ \
    PyObject_GenericGetAttr,      /* tp_getattro*/ \
    PyObject_GenericSetAttr,      /* tp_setattro*/ \
    NULL,                         /* tp_as_buffer*/ \
    FLAGS,                        /* tp_flags*/ \
    DOC,                          /* tp_doc */ \
    NULL,                         /* tp_traverse */ \
    NULL,                         /* tp_clear */ \
    CMP,                          /* tp_richcompare */ \
    0,                            /* tp_weaklistoffset */ \
    NULL,                         /* tp_iter */ \
    NULL,                         /* tp_iternext */ \
    METHODS,                      /* tp_methods */ \
    NULL,                         /* tp_members */ \
    GETSET,                       /* tp_getset */ \
    BASE,                         /* tp_base */ \
    NULL,                         /* tp_dict */ \
    NULL,                         /* tp_descr_get */ \
    NULL,                         /* tp_descr_set */ \
    0,                            /* tp_dictoffset */ \
    NULL,                         /* tp_init */ \
    NULL,                         /* tp_alloc */ \
    OBNEW,                        /* tp_new */ \
    NULL,                         /* tp_free */ \
    NULL,                         /* tp_is_gc */ \
    NULL,                         /* tp_bases */ \
    NULL,                         /* tp_mro */ \
    NULL,                         /* tp_cache */ \
    NULL,                         /* tp_subclasses */ \
    NULL,                         /* tp_weaklist */ \
    NULL,                         /* tp_del */ \
}
#endif

#endif /* PLUGIN_PYTHON_H */

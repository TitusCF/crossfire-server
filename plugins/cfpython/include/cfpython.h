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
/*                                                                           */ /*****************************************************************************/
#ifndef PLUGIN_PYTHON_H
#define PLUGIN_PYTHON_H

/* First the required header files - only the CF module interface and Python */
#include <Python.h>

/* include compile.h from python. Python.h doesn't pull it in with versions
 * 2.3 and older, and it does have protection from double-imports.
 */
#include <compile.h>
#include <plugin.h>

#undef MODULEAPI
#ifdef WIN32
#ifdef PYTHON_PLUGIN_EXPORTS
#define MODULEAPI __declspec(dllexport)
#else
#define MODULEAPI __declspec(dllimport)
#endif

#else
#define MODULEAPI
#endif

#define PLUGIN_NAME    "Python"
#define PLUGIN_VERSION "CFPython Plugin 2.0a13 (Fido)"

#include <plugin_common.h>
#include <cfpython_object.h>
#include <cfpython_map.h>
#include <cfpython_archetype.h>
#include <cfpython_party.h>
#include <cfpython_region.h>

typedef struct _cfpcontext
{
    struct _cfpcontext* down;
    PyObject*   who;
    PyObject*   activator;
    PyObject*   third;
    char        message[1024];
    int         fix;
    int         event_code;
    char        script[1024];
    char        options[1024];
    int         returnvalue;
    int         parms[5];
} CFPContext;

extern f_plug_api gethook;
extern CFPContext* context_stack;
extern CFPContext* current_context;

/* This structure is used to define one python-implemented crossfire command.*/
typedef struct PythonCmdStruct
{
    char *name;    /* The name of the command, as known in the game.         */
    char *script;  /* The name of the script file to bind.                   */
    double speed;  /* The speed of the command execution.                    */
} PythonCmd;

/* This plugin allows up to 1024 custom commands.                            */
#define NR_CUSTOM_CMD 1024
PythonCmd CustomCommand[NR_CUSTOM_CMD];
#include <cfpython_proto.h>

#endif /* PLUGIN_PYTHON_H */

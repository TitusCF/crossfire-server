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
#ifndef CFPYTHON_OBJECT_H
#define CFPYTHON_OBJECT_H

typedef struct {
    PyObject_HEAD
    object *obj;
    tag_t count;
} Crossfire_Object;

extern PyTypeObject Crossfire_ObjectType;

typedef struct {
    PyObject_HEAD
    object *obj;
    tag_t count;
} Crossfire_Player;

extern PyTypeObject Crossfire_PlayerType;

#define EXISTCHECK(ob) { \
    if (!ob || !ob->obj || (object_was_destroyed(ob->obj, ob->obj->count))) { \
        PyErr_SetString(PyExc_ReferenceError, "Crossfire object no longer exists"); \
        return NULL; \
    } }

/**
 * This is meant to be used for parameters where you don't know if the type of
 * the object is correct. It should NOT be used for the self pointer, since that
 * will always be a compatible type.
 */
#define TYPEEXISTCHECK(ob) { \
    if (!ob || !PyObject_TypeCheck((PyObject*)ob, &Crossfire_ObjectType) || !ob->obj || (object_was_destroyed(ob->obj, ob->obj->count))) { \
        PyErr_SetString(PyExc_ReferenceError, "Not a Crossfire object or Crossfire object no longer exists"); \
        return NULL; \
    } }

#define EXISTCHECK_INT(ob) { \
    if (!ob || !ob->obj || (object_was_destroyed(ob->obj, ob->obj->count))) { \
        PyErr_SetString(PyExc_ReferenceError, "Crossfire object no longer exists"); \
        return -1; \
    } }

#endif /* CFPYTHON_OBJECT_H */

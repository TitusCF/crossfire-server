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

#include <cfpython.h>
#include <cfpython_region_private.h>

static PyObject *Crossfire_Region_GetName(Crossfire_Region *regionptr, void *closure) {
    return Py_BuildValue("s", cf_region_get_name(regionptr->reg));
}

static PyObject *Crossfire_Region_GetLongname(Crossfire_Region *regionptr, void *closure) {
    return Py_BuildValue("s", cf_region_get_longname(regionptr->reg));
}

static PyObject *Crossfire_Region_GetMessage(Crossfire_Region *regionptr, void *closure) {
    return Py_BuildValue("s", cf_region_get_message(regionptr->reg));
}

static PyObject *Crossfire_Region_GetNext(Crossfire_Region *party, void *closure) {
    return Crossfire_Region_wrap(cf_region_get_next(party->reg));
}

static PyObject *Crossfire_Region_GetParent(Crossfire_Region *party, PyObject *args) {
    return Crossfire_Region_wrap(cf_region_get_parent(party->reg));
}

PyObject *Crossfire_Region_wrap(region *what) {
    Crossfire_Region *wrapper;

    /* return None if no object was to be wrapped */
    if (what == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    wrapper = PyObject_NEW(Crossfire_Region, &Crossfire_RegionType);
    if (wrapper != NULL)
        wrapper->reg = what;
    return (PyObject *)wrapper;
}

static int Crossfire_Region_InternalCompare(Crossfire_Region *left, Crossfire_Region *right) {
    return (left->reg < right->reg ? -1 : (left->reg == right->reg ? 0 : 1));
}

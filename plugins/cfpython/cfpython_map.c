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
#include <cfpython.h>
#include <cfpython_map_private.h>

static PyObject* Map_GetDifficulty(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_difficulty(whoptr->map));
}
static PyObject* Map_GetPath(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("s", cf_map_get_property(whoptr->map, CFAPI_MAP_PROP_PATH));
}
static PyObject* Map_GetTempName(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("s", cf_map_get_property(whoptr->map, CFAPI_MAP_PROP_TMPNAME));
}
static PyObject* Map_GetName(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("s", cf_map_get_property(whoptr->map, CFAPI_MAP_PROP_NAME));
}
static PyObject* Map_GetResetTime(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_reset_time(whoptr->map));
}
static PyObject* Map_GetResetTimeout(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_reset_timeout(whoptr->map));
}
static PyObject* Map_GetPlayers(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_players(whoptr->map));
}
static PyObject* Map_GetLight(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", *(int*)cf_map_get_property(whoptr->map, CFAPI_MAP_PROP_LIGHT));
}
static PyObject* Map_GetDarkness(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_darkness(whoptr->map));
}
static PyObject* Map_GetWidth(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_width(whoptr->map));
}
static PyObject* Map_GetHeight(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_height(whoptr->map));
}
static PyObject* Map_GetEnterX(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_property(whoptr->map, CFAPI_MAP_PROP_ENTER_X));
}
static PyObject* Map_GetEnterY(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_enter_x(whoptr->map));
}
static PyObject* Map_GetTemperature(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_temperature(whoptr->map));
}
static PyObject* Map_GetPressure(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_pressure(whoptr->map));
}
static PyObject* Map_GetHumidity(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_humidity(whoptr->map));
}
static PyObject* Map_GetWindSpeed(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_windspeed(whoptr->map));
}
static PyObject* Map_GetWindDir(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_winddir(whoptr->map));
}
static PyObject* Map_GetSky(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_sky(whoptr->map));
}
static PyObject* Map_GetWPartX(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_wpartx(whoptr->map));
}
static PyObject* Map_GetWPartY(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("i", cf_map_get_wparty(whoptr->map));
}
static PyObject* Map_GetMessage(Crossfire_Map* whoptr, void* closure)
{
    return Py_BuildValue("s", cf_map_get_property(whoptr->map, CFAPI_MAP_PROP_MESSAGE));
}

static PyObject* Map_GetRegion(Crossfire_Map* whoptr, void* closure)
{
    return Crossfire_Region_wrap(cf_map_get_property(whoptr->map,CFAPI_MAP_PROP_REGION));
}

static PyObject* Map_Message(Crossfire_Map* map, PyObject* args)
{
    int   color = NDI_BLUE|NDI_UNIQUE;
    char *message;

    if (!PyArg_ParseTuple(args,"s|i",&message,&color))
        return NULL;

    cf_map_message(map->map, message, color);

    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject* Map_GetFirstObjectAt(Crossfire_Map* map, PyObject* args)
{
    int x, y;
    object* val;

    if (!PyArg_ParseTuple(args,"ii",&x,&y))
        return NULL;

    val = cf_map_get_object_at(map->map,x,y);
    return Crossfire_Object_wrap(val);
}
static PyObject* Map_CreateObject(Crossfire_Map* map, PyObject* args)
{
    char* txt;
    int x,y;
    object* op;
    if (!PyArg_ParseTuple(args,"sii",&txt,&x,&y))
        return NULL;
    op = cf_create_object_by_name(txt);
    if (op == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    cf_map_insert_object(map->map,op,x,y);
    return Crossfire_Object_wrap(op);
}
static PyObject* Map_Check(Crossfire_Map* map, PyObject* args)
{
    char *what;
    int x, y;
    object* foundob;
    sint16 nx, ny;
    int mflags;

    if (!PyArg_ParseTuple(args,"s(ii)",&what,&x,&y))
        return NULL;

    /* make sure the map is swapped in */
    if (map->map->in_memory != MAP_IN_MEMORY)
    {
        cf_log(llevError, "MAP AIN'T READY !\n");
    }

    mflags = cf_map_get_flags(map->map, &(map->map), (sint16)x, (sint16)y, &nx, &ny);
    if (mflags & P_OUT_OF_MAP)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }
    foundob = cf_map_present_arch_by_name(what, map->map, nx, ny);
    return Crossfire_Object_wrap(foundob);
}
static PyObject* Map_Next(Crossfire_Map* map, PyObject* args)
{
	return Crossfire_Map_wrap(cf_map_get_property(map->map,CFAPI_MAP_PROP_NEXT));
}

static PyObject* Map_Insert(Crossfire_Map* map, PyObject* args)
{
    int x, y;
    Crossfire_Object* what;
    
    if (!PyArg_ParseTuple(args,"O!ii", &Crossfire_ObjectType, &what, &x, &y))
        return NULL;

    return Crossfire_Object_wrap(cf_map_insert_object(map->map, what->obj, x, y));
}

static int Map_InternalCompare(Crossfire_Map* left, Crossfire_Map* right)
{
	return left->map < right->map ? -1 : ( left->map == right->map ? 0 : 1 );
}

/* Legacy code: convert to long so that non-object functions work correctly */
static PyObject* Crossfire_Map_Long( PyObject* obj )
{
    return Py_BuildValue("l", ((Crossfire_Object*)obj)->obj);
}

static PyObject* Crossfire_Map_Int( PyObject* obj )
{
    return Py_BuildValue("i", ((Crossfire_Object*)obj)->obj);
}

/**
 * Python initialized.
 **/
static PyObject *
        Crossfire_Map_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Crossfire_Map *self;

    self = (Crossfire_Map *)type->tp_alloc(type, 0);
    if(self)
        self->map = NULL;

    return (PyObject *)self;
}

PyObject *Crossfire_Map_wrap(mapstruct *what)
{
    Crossfire_Map *wrapper;

    /* return None if no object was to be wrapped */
    if(what == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    wrapper = PyObject_NEW(Crossfire_Map, &Crossfire_MapType);
    if(wrapper != NULL)
        wrapper->map = what;
    return (PyObject *)wrapper;
}


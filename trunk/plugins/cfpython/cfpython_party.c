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
#include <cfpython_party_private.h>

static PyObject* Crossfire_Party_GetName( Crossfire_Party* partyptr, void* closure)
{
    return Py_BuildValue("s",cf_party_get_name(partyptr->party));
}

static PyObject* Crossfire_Party_GetPassword( Crossfire_Party* partyptr, void* closure)
{
    return Py_BuildValue("s",cf_party_get_password(partyptr->party));
}

static PyObject* Crossfire_Party_GetNext( Crossfire_Party* party, void* closure )
{
    return Crossfire_Party_wrap(cf_party_get_next(party->party));
}

static PyObject* Crossfire_Party_GetPlayers( Crossfire_Party* party, PyObject* args )
{
    PyObject* list;
    player* pl;

    list = PyList_New(0);
    pl = cf_party_get_first_player(party->party);
    while (pl)
    {
        PyList_Append(list,Crossfire_Object_wrap(pl->ob));
        pl = cf_party_get_next_player(party->party,pl);
    }
    return list;
}

PyObject *Crossfire_Party_wrap(partylist *what)
{
    Crossfire_Party *wrapper;

    /* return None if no object was to be wrapped */
    if(what == NULL) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    wrapper = PyObject_NEW(Crossfire_Party, &Crossfire_PartyType);
    if(wrapper != NULL)
        wrapper->party = what;
    return (PyObject *)wrapper;
}

static int Crossfire_Party_InternalCompare(Crossfire_Party* left, Crossfire_Party* right)
{
    return (left->party < right->party ? -1 : ( left->party == right->party ? 0 : 1 ) );
}

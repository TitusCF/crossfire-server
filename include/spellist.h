/*
 * static char *rcsid_spellist_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to mark@pyramid.com
*/

/**
 * @file
 * Path name definition.
 * @todo
 * only used in the documentation, the variable is also defined in init.c. So fix documentation and trash.
 */

#ifndef SPELLIST_H
#define SPELLIST_H

#include "spells.h"

const char *spellpathnames[NRSPELLPATHS] = {
    "Protection",
    "Fire",
    "Frost",
    "Electricity",
    "Missiles",
    "Self",
    "Summoning",
    "Abjuration",
    "Restoration",
    "Detonation",
    "Mind",
    "Creation",
    "Teleportation",
    "Information",
    "Transmutation",
    "Transferrence",
    "Turning",
    "Wounding",
    "Death",
    "Light"
};

#endif /* SPELLIST_H */

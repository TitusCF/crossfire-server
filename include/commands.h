/*
 * static char *rcsid_commands_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
    Copyright (C) 1992 Frank Tore Johansen

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
 * Defines and structures related to commands the player can send.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

/*
 * Crossfire commands
 *      ++Jam
 *
 * ''', run and fire-keys are parsed separately (cannot be overrided).
 */


/* The initialized arrays were removed from this file and are now
 * in commands.c.  Initializing the arrays in any header file
 * is stupid, as it means that header file can only be included
 * in one source file (so what is the point of putting them in a header
 * file then?).  Header files should be used like this one - to declare
 * the structures externally - they actual structures should resided/
 * be initialized in one of the source files.
 */

/**
 * One command function.
 * @param op
 * the player executing the command
 * @param params
 * the command parameters; empty string if no commands are given; leading and
 * trailing spaces have been removed
 */
typedef void (*command_function)(object *op, const char *params);

/** Represents one command. */
typedef struct {
    const char *name;         /**< Command name. */
    command_function func;    /**< Pointer to command function. */
    float time;               /**< How long it takes to execute this command. */
} command_array_struct;

extern command_array_struct Commands[], WizCommands[], CommunicationCommands[];

extern const int CommandsSize, WizCommandsSize, CommunicationCommandSize;

#endif /* COMMANDS_H */

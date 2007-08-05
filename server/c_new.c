/*
 * static char *rcsid_c_new_c =
 *   "$Id$";
 */

/*
  CrossFire, A Multiplayer game for X-windows

  Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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

  The author can be reached via e-mail to crossfire-devel@real-time.com
*/

/**
 * @file
 * This file deals with administrative commands from the client.
 */

#include <global.h>
#include <commands.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#ifndef tolower
/** Simple macro to convert a letter to lowercase. */
#define tolower(C)      (((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))
#endif

/**
 * Compare function for commands.
 *
 * @param a
 * @param b
 * commands to compare.
 * @retval -1
 * a is less then b.
 * @retval 0
 * a and b are equals.
 * @retval 1
 * a is greater than b.
 */
static int compare_A(const void *a, const void *b)
{
    return strcmp(((const command_array_struct *)a)->name,
                  ((const command_array_struct *)b)->name);
}

/**
 * Finds the specified command in the command array. Utility function.
 *
 * @param cmd
 * command to find. Will be put to lowercase.
 * @param commarray
 * commands to search into.
 * @param commsize
 * length of commarray.
 * @return
 * matching command, NULL for no match.
 * @todo move the lowercase conversion to execute_newserver_command() for performance reasons.
 */
static command_array_struct *find_command_element(char *cmd,
                                                  command_array_struct *commarray,
                                                  int commsize)
{
    command_array_struct *asp, dummy;
    char *cp;

    for (cp=cmd; *cp; cp++)
        *cp =tolower(*cp);

    dummy.name =cmd;
    asp =(command_array_struct *)bsearch((void *)&dummy,
                                         (void *)commarray, commsize,
                                         sizeof(command_array_struct),
                                         compare_A);
    return asp;
}

/**
 * Player issued a command, let's handle it.
 *
 * This function is called from the new client/server code.
 *
 * @param pl
 * player who is issuing the command
 * @param command
 * the actual command with its arguments.
 * @return
 * 0 if invalid command, else actual command's return value (which may be zero).
 */
int execute_newserver_command(object *pl, char *command)
{
    command_array_struct *csp;
    char *cp;

    pl->contr->has_hit=0;

        /*
         * remove trailing spaces from commant
         */
    cp=command+strlen(command)-1;
    while ( (cp>=command) && (*cp==' ')){
        *cp='\0';
        cp--;
    }
    cp=strchr(command, ' ');
    if (cp) {
        *(cp++) ='\0';
        while (*cp==' ') cp++;
    }

    csp = find_plugin_command(command,pl);

    if (!csp)
        csp = find_command_element(command, Commands, CommandsSize);
    if (!csp)
        csp = find_command_element(command, CommunicationCommands,
                                   CommunicationCommandSize);
    if (!csp && QUERY_FLAG(pl, FLAG_WIZ))
        csp = find_command_element(command, WizCommands, WizCommandsSize);

    if (csp==NULL) {
        draw_ext_info_format(NDI_UNIQUE, 0,pl,
                             MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "'%s' is not a valid command.",
                             "'%s' is not a valid command.",
                             command);
        return 0;
    }

    pl->speed_left -= csp->time;

        /* A character time can never exceed his speed (which in many cases,
         * if wearing armor, is less than one.)  Thus, in most cases, if
         * the command takes 1.0, the player's speed will be less than zero.
         * it is only really an issue if time goes below -1
         * Due to various reasons that are too long to go into here, we will
         * actually still execute player even if his time is less than 0,
         * but greater than -1.  This is to improve the performance of the
         * new client/server.  In theory, it shouldn't make much difference.
         */

    if (csp->time && pl->speed_left<-2.0) {
        LOG(llevDebug,"execute_newclient_command: Player issued command that takes more time than he has left.\n");
    }
    return csp->func(pl, cp);
}

/**
 * Player wants to start running.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 * @return
 * 0.
 */
int command_run(object *op, char *params)
{
    int dir;
    dir = params?atoi(params):0;
    if ( dir<0 || dir>=9 ){
        draw_ext_info(NDI_UNIQUE, 0,op,
                      MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Can't run into a non adjacent square.", NULL);
        return 0;
    }
    op->contr->run_on=1;
    return move_player(op, dir);
}

/**
 * Player wants to stop running.
 *
 * @param op
 * player.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_run_stop(object *op, char *params)
{
    op->contr->run_on=0;
    return 1;
}

/**
 * Player wants to start furing.
 *
 * @param op
 * player.
 * @param params
 * additional parameters.
 * @return
 * 0.
 */
int command_fire(object *op, char *params)
{
    int dir;
    dir = params?atoi(params):0;
    if ( dir<0 || dir>=9 ){
        draw_ext_info(NDI_UNIQUE, 0,op,
                      MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                      "Can't fire to a non adjacent square.", NULL);
        return 0;
    };
    op->contr->fire_on=1;
    return move_player(op, dir);
}

/**
 * Player wants to stop firing.
 *
 * @param op
 * player.
 * @param params
 * ignored.
 * @return
 * 0.
 */
int command_fire_stop(object *op, char *params)
{
    op->contr->fire_on=0;
    return 1;
}

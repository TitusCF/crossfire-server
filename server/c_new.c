/*
 * static char *rcsid_c_new_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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

/* This file deals with administrative commands from the client. */
#include <global.h>
#include <commands.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#ifndef tolower
#define tolower(C)      (((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))
#endif


static int compare_A(const void *a, const void *b)
{
  return strcmp(((CommArray_s *)a)->name, ((CommArray_s *)b)->name);
}

static CommArray_s *find_command_element(char *cmd, CommArray_s *commarray,
    int commsize)
{
  CommArray_s *asp, dummy;
  char *cp;

  for (cp=cmd; *cp; cp++)
    *cp =tolower(*cp);

  dummy.name =cmd;
  asp =(CommArray_s *)bsearch((void *)&dummy,
			      (void *)commarray, commsize,
			      sizeof(CommArray_s), compare_A);
  return asp;
}

/* This function is called from the new client/server code.
 * pl is the player who is issuing the command, command is the
 * command.
 */
int execute_newserver_command(object *pl, char *command)
{
    CommArray_s *csp;
    char *cp;

    pl->contr->has_hit=0;

    cp=strchr(command, ' ');
    if (cp) {
	*(cp++) ='\0';
	while (*cp==' ') cp++;
    }

    csp = find_plugin_command(command,pl);

    if (!csp)
    csp = find_command_element(command, NewServerCommands,
		NewServerCommandSize);
    if (!csp)
	csp = find_command_element(command, Commands, CommandsSize);
    if (!csp)
        csp = find_command_element(command, CommunicationCommands, CommunicationCommandSize);
    if (!csp && QUERY_FLAG(pl, FLAG_WIZ))
	csp = find_command_element(command, WizCommands, WizCommandsSize);

    if (csp==NULL) {
	    new_draw_info_format(NDI_UNIQUE, 0,pl,
		"'%s' is not a valid command.", command);
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

int command_run(object *op, char *params)
{
    int dir;
    dir = params?atoi(params):0;
    if ( dir<0 || dir>=9 ){
        new_draw_info(NDI_UNIQUE, 0,op,"Can't run into a non adjacent square.");
        return 0;
    }
    op->contr->run_on=1;
    return move_player(op, dir);
}

int command_run_stop(object *op, char *params)
{
    op->contr->run_on=0;
    return 1;
}

int command_fire(object *op, char *params)
{
    int dir;
    dir = params?atoi(params):0;
    if ( dir<0 || dir>=9 ){
        new_draw_info(NDI_UNIQUE, 0,op,"Can't fire to a non adjacent square.");
        return 0;
    };
    op->contr->fire_on=1;
    return move_player(op, dir);
}

int command_fire_stop(object *op, char *params)
{
    op->contr->fire_on=0;
    return 1;
}

int bad_command(object *op, char *params)
{
    new_draw_info(NDI_UNIQUE, 0,op,"bind and unbind are no longer handled on the server");
    return 1;
}

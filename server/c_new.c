/*
 * This file contains various commands that are used by the new client/server
 *	++Jam
 */

#include <global.h>
#include <commands.h>
#include <sproto.h>

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

    csp = find_command_element(command, NewServerCommands,
		NewServerCommandSize);
    if (!csp)
	csp = find_command_element(command, Commands, CommandsSize);

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
    op->contr->run_on=1;
    return (move_player(op, params?atoi(params):0));
}

int command_run_stop(object *op, char *params)
{
    op->contr->run_on=0;
    return 1;
}

int command_fire(object *op, char *params)
{
    op->contr->fire_on=1;
    return move_player(op, params?atoi(params):0);
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

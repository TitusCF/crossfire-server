/*
 * static char *rcsid_commands_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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

/*
 * Command parser
 */

#include <global.h>
#include <commands.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <ctype.h>

/* Added times to all the commands.  However, this was quickly done,
 * and probably needs more refinements.  All socket and DM commands
 * take 0 time.
 */

/*
 * Normal game commands
 */
command_array_struct Commands[] = {
  {"save", command_save,	0.0},

  {"sound", command_sound,	0.0},
  {"party", command_party,	0.0},
  {"gsay", command_gsay,	1.0},

#ifdef DEBUG
  {"sstable", command_sstable,	0.0},
#endif
#ifdef DEBUG_MALLOC_LEVEL
  {"verify", command_malloc_verify,0.0},
#endif
  {"apply", command_apply,	1.0},	/* should be variable */
  {"applymode", command_applymode,	1.0},	/* should be variable */
  {"archs", command_archs,	0.0},
  {"body", command_body,	0.0},
  {"brace", command_brace,	0.0},
  {"build", command_build,  0.0},
  {"cast", command_cast,	0.2},	/* Is this right? */
  {"disarm", command_disarm,	1.0},
  {"dm", command_dm,		0.0},
  {"dmhide", command_dmhide,		0.0}, /* Like dm, but don't tell a dm arrived, hide player */
  {"drop", command_drop,	1.0},
  {"dropall", command_dropall,	1.0},
  {"examine", command_examine,	0.5},
  {"explore", command_explore,  0.0},
  {"fix_me", command_fix_me,	0.0},
  {"get", command_take,		1.0},
  {"help", command_help,	0.0},
  {"hiscore", command_hiscore,	0.0},
  {"inventory", command_inventory,0.0},
  {"invoke", command_invoke,	1.0},
  {"killpets", command_kill_pets,0.0},
  {"listen", command_listen,	0.0},
  {"logs", command_logs,	0.0},
  {"malloc", command_malloc,	0.0},
  {"maps", command_maps,	0.0},
  {"mapinfo", command_mapinfo,	0.0},
  {"mark", command_mark,	0.0},
  {"motd", command_motd,	0.0},
  {"rules", command_rules,	0.0},
  {"news", command_news,	0.0},
  {"output-sync", command_output_sync,	0.0},
  {"output-count", command_output_count,0.0},
  {"passwd", command_passwd, 0.0},
  {"peaceful", command_peaceful,0.0},
  {"pickup", command_pickup,	1.0},
  {"players", command_players,	0.0},
  {"prepare", command_prepare,	1.0},
  {"quit", command_quit,	0.0},
  {"rename", command_rename_item,  0.0},
  {"resistances", command_resistances,	0.0},
  {"rotateshoottype", command_rotateshoottype,	0.0},
  {"shutdown", command_shutdown, 0.0},
  {"skills", command_skills,	0.0},	/* shows player list of skills */
  {"use_skill", command_uskill, 1.0},
  {"quests", command_quests, 0.0},
  {"ready_skill", command_rskill, 1.0},
  {"search",command_search,	1.0},
  {"search-items", command_search_items,	0.0},
  {"showpets", command_showpets,	1.0},
  {"statistics", command_statistics,	0.0},
  {"strings", command_strings,	0.0},
  {"take", command_take,	1.0},
  {"throw", command_throw,	1.0},
  {"time", command_time,	0.0},
  {"weather", command_weather,	0.0},
  {"whereabouts", command_whereabouts, 0.0},
  {"whereami", command_whereami, 0.0},
  {"title", command_title,	0.0},
  {"usekeys", command_usekeys,	0.0},
  {"bowmode", command_bowmode, 0.0},
  {"petmode", command_petmode, 0.0},
  {"version", command_version,	0.0},
  {"wimpy", command_wimpy,	0.0},
  {"who", command_who,		0.0},
  {"afk", command_afk,          0.0},

  {"stay", command_stay,	1.0}, /* 1.0 because it is used when using a
				       *  skill on yourself */
  {"north", command_north,	1.0},
  {"east", command_east,	1.0},
  {"south", command_south,	1.0},
  {"west", command_west,	1.0},
  {"northeast", command_northeast,	1.0},
  {"southeast", command_southeast,	1.0},
  {"southwest", command_southwest,	1.0},
  {"northwest", command_northwest,	1.0},
};

const int CommandsSize =sizeof(Commands) / sizeof(command_array_struct);

command_array_struct CommunicationCommands [] = {
  /* begin emotions */
  {"tell", command_tell,		0.1},
  {"reply", command_reply,		0.0},
  {"say", command_say,			0.1},
  {"shout", command_shout,		0.1},
  {"chat", command_chat,		0.1},
  {"me", command_me,                   0.1},
  {"nod", command_nod,			0.0},
  {"dance", command_dance,		0.0},
  {"kiss", command_kiss,		0.0},
  {"bounce", command_bounce,		0.0},
  {"smile", command_smile,		0.0},
  {"cackle", command_cackle,		0.0},
  {"laugh", command_laugh,		0.0},
  {"giggle", command_giggle,		0.0},
  {"shake", command_shake,		0.0},
  {"puke", command_puke,		0.0},
  {"growl", command_growl,		0.0},
  {"scream", command_scream,		0.0},
  {"sigh", command_sigh,		0.0},
  {"sulk", command_sulk,		0.0},
  {"hug", command_hug,			0.0},
  {"cry", command_cry,			0.0},
  {"poke", command_poke,		0.0},
  {"accuse", command_accuse,		0.0},
  {"grin", command_grin,		0.0},
  {"bow", command_bow,			0.0},
  {"clap", command_clap,		0.0},
  {"blush", command_blush,		0.0},
  {"burp", command_burp,		0.0},
  {"chuckle", command_chuckle,		0.0},
  {"cough", command_cough,		0.0},
  {"flip", command_flip,		0.0},
  {"frown", command_frown,		0.0},
  {"gasp", command_gasp,		0.0},
  {"glare", command_glare,		0.0},
  {"groan", command_groan,		0.0},
  {"hiccup", command_hiccup,		0.0},
  {"lick", command_lick,		0.0},
  {"pout", command_pout,		0.0},
  {"shiver", command_shiver,		0.0},
  {"shrug", command_shrug,		0.0},
  {"slap", command_slap,		0.0},
  {"smirk", command_smirk,		0.0},
  {"snap", command_snap,		0.0},
  {"sneeze", command_sneeze,		0.0},
  {"snicker", command_snicker,		0.0},
  {"sniff", command_sniff,		0.0},
  {"snore", command_snore,		0.0},
  {"spit", command_spit,		0.0},
  {"strut", command_strut,		0.0},
  {"thank", command_thank,		0.0},
  {"twiddle", command_twiddle,		0.0},
  {"wave", command_wave,		0.0},
  {"whistle", command_whistle,		0.0},
  {"wink", command_wink,		0.0},
  {"yawn", command_yawn,		0.0},
  {"beg", command_beg,			0.0},
  {"bleed", command_bleed,		0.0},
  {"cringe", command_cringe,		0.0},
  {"think", command_think,		0.0},
  {"cointoss", command_cointoss, 0.0},
  {"orcknuckle", command_orcknuckle, 0.0},
  {"printlos", command_printlos,0.0},
};

const int CommunicationCommandSize = sizeof(CommunicationCommands)/ sizeof(command_array_struct);

command_array_struct NewServerCommands [] = {
  {"run", command_run, 1.0},
  {"run_stop", command_run_stop, 0.0},
  {"fire", command_fire, 1.0},
  {"fire_stop", command_fire_stop, 0.0}
};

const int NewServerCommandSize = sizeof(NewServerCommands)/ sizeof(command_array_struct);

/*
 * Wizard commands (for both)
 */
command_array_struct WizCommands [] = {
  {"abil", command_abil,0.0},
  {"addexp", command_addexp,0.0},
  {"arrest", command_arrest,0.0},
  {"banish", command_banish,0.0},
  {"create", command_create,0.0},
  {"debug", command_debug,0.0},
  {"diff", command_diff, 0.0 },
  {"dump", command_dump,0.0},
  {"dumpbelow", command_dumpbelow,0.0},
  {"dumpfriendlyobjects", command_dumpfriendlyobjects,0.0},
  {"dumpallarchetypes", command_dumpallarchetypes,0.0},
  {"dumpallmaps", command_dumpallmaps,0.0},
  {"dumpallobjects", command_dumpallobjects,0.0},
  {"dumpmap", command_dumpmap,0.0},
  {"forget_spell", command_forget_spell, 0.0},
  {"free", command_free,0.0},
  {"freeze", command_freeze,0.0},
  {"goto", command_goto,0.0},
  {"hide", command_hide,0.0},
  {"insert_into", command_insert_into,0.0},
  {"invisible", command_invisible,0.0},
  {"kick", (command_function)command_kick, 0.0},
  {"learn_special_prayer", command_learn_special_prayer, 0.0},
  {"learn_spell", command_learn_spell, 0.0},
  {"plugin",command_loadplugin,0.0},
  {"pluglist",command_listplugins,0.0},
  {"plugout",command_unloadplugin,0.0},
  {"nodm", command_nowiz,0.0},
  {"nowiz", command_nowiz,0.0},
  {"patch", command_patch,0.0},
  {"remove", command_remove,0.0},
  {"reset", command_reset,0.0},
  {"set_god", command_setgod, 0.0},
  {"server_speed", command_speed,0.0},
  {"ssdumptable", command_ssdumptable,0.0},
  {"stack_clear", command_stack_clear, 0.0 },
  {"stack_list", command_stack_list, 0.0},
  {"stack_pop", command_stack_pop, 0.0 },
  {"stack_push", command_stack_push, 0.0 },
  {"stats", command_stats,0.0},
  {"style_info", command_style_map_info, 0.0},	/* Costly command, so make it wiz only */
  {"summon", command_summon,0.0},
  {"teleport", command_teleport,0.0},
  {"toggle_shout", command_toggle_shout,0.0},
  {"wizpass", command_wizpass,0.0},
  {"wizcast", command_wizcast,0.0},
  {"overlay_save", command_overlay_save, 0.0},
  {"overlay_reset", command_overlay_reset, 0.0},
  /*  {"possess", command_possess, 0.0}, */
  {"mon_aggr", command_mon_aggr, 0.0},
  {"loadtest", command_loadtest, 0.0},
};
const int WizCommandsSize =sizeof(WizCommands) / sizeof(command_array_struct);

/* Socket commands - these should really do nothing more than output things
 * to the various players/sockets.
 */
command_array_struct Socket_Commands[] = {
  {"hiscore", command_hiscore,	0.0},
  {"logs", command_logs,	0.0},
  {"maps", command_maps,	0.0},
  {"motd", command_motd,	0.0},
  {"rules", command_rules,	0.0},
  {"news", command_news,	0.0},
  {"players", command_players,	0.0},
  {"version", command_version,	0.0},
  {"who", command_who,		0.0},
};

const int Socket_CommandsSize =sizeof(Socket_Commands) / sizeof(command_array_struct);


/* Socket commands - these should really do nothing more than output things
 * to the various players/sockets.
 */
command_array_struct Socket2_Commands[] = {
  {"shout", command_shout,	0.1},
  {"chat", command_chat,	0.1},
  {"tell", command_tell,	0.1},
};

const int Socket2_CommandsSize =sizeof(Socket2_Commands) / sizeof(command_array_struct);



static int compare_A(const void *a, const void *b)
{
    return strcmp(((const command_array_struct *)a)->name, ((const command_array_struct *)b)->name);
}

void init_commands(void)
{
    qsort(Commands, CommandsSize, sizeof(command_array_struct), compare_A);
    qsort(CommunicationCommands, CommunicationCommandSize, sizeof(command_array_struct), compare_A);
    qsort(NewServerCommands, NewServerCommandSize, sizeof(command_array_struct), compare_A);
    qsort(WizCommands, WizCommandsSize, sizeof(command_array_struct), compare_A);
    qsort(Socket_Commands, Socket_CommandsSize, sizeof(command_array_struct), compare_A);
    qsort(Socket2_Commands, Socket2_CommandsSize, sizeof(command_array_struct), compare_A);
}

#ifndef tolower
#define tolower(C)	(((C) >= 'A' && (C) <= 'Z')? (C) - 'A' + 'a': (C))
#endif


command_function find_oldsocket_command(char *cmd)
{
  command_array_struct *asp, dummy;
  char *cp;

  for (cp=cmd; *cp; cp++) {
    *cp =tolower(*cp);
  }

  dummy.name =cmd;
  asp =(command_array_struct *)bsearch((void *)&dummy,
			      (void *)Socket_Commands, Socket_CommandsSize,
			      sizeof(command_array_struct), compare_A);
  if (asp)
    return asp->func;
  return NULL;
}

command_function find_oldsocket_command2(char *cmd)
{
  command_array_struct *asp, dummy;
  char *cp;

  for (cp=cmd; *cp; cp++) {
    *cp =tolower(*cp);
  }

  dummy.name =cmd;
  asp =(command_array_struct *)bsearch((void *)&dummy,
			      (void *)Socket2_Commands, Socket2_CommandsSize,
			      sizeof(command_array_struct), compare_A);
  if (asp)
    return asp->func;
  return NULL;
}

#if 0
/* find_command is only used by parse_string, so if parse_string
 * isn't needed, neither is find_command.  But parse_string
 * is only used by parse_command.
 * MSW 2006-06-02
 */

static command_function find_command(char *cmd)
{
  command_array_struct *asp, dummy;
  char *cp;

  for (cp=cmd; *cp; cp++)
    *cp =tolower(*cp);

  dummy.name =cmd;
  asp =(command_array_struct *)bsearch((void *)&dummy,
			      (void *)Commands, CommandsSize,
			      sizeof(command_array_struct), compare_A);
  LOG(llevDebug, "Getting asp for command string %s\n", cmd);
  if (asp)
    return asp->func;
  else
  {
    LOG(llevDebug, "Now we are here\n");
    asp =(command_array_struct *)bsearch((void *)&dummy,
      (void *)CommunicationCommands, CommunicationCommandSize,
      sizeof(command_array_struct), compare_A);
    if (asp)
      return asp->func;
    else
      return NULL;
  };
}

static command_function find_wizcommand(char *cmd)
{
  command_array_struct *asp, dummy;
  char *cp;

  for (cp=cmd; *cp; cp++)
    *cp =tolower(*cp);

  dummy.name =cmd;
  asp =(command_array_struct *)bsearch((void *)&dummy,
			      (void *)WizCommands, WizCommandsSize,
			      sizeof(command_array_struct), compare_A);
  if (asp)
    return asp->func;
  return NULL;
}

/* parse_string is only used by parse_command, so if parse_command
 * isn't needed, neither is parse_string
 * MSW 2006-06-02
 */

/**
 * parse_string may be called from a player in the game or from a socket
 * (op is NULL if it's a socket).
 * It returnes 1 if it recognized the command, otherwise 0.
 * Actually return value is used as can-repeat -flag
 */

static int parse_string(object *op, char *str)
{
    command_function f;
    char *cp;
    command_array_struct *asp;

#ifdef INPUT_DEBUG
    LOG(llevDebug, "Command: '%s'\n", str);
#endif
    /*
     * remove trailing spaces
     */
    cp = str+strlen(str)-1;
    while ( (cp>=str) && (*cp==' ')){
        *cp='\0';
	cp--;
    }
    /*
     * No arguments?
     */
    if (!(cp=strchr(str, ' '))) {
	/* GROS - If we are here, then maybe this is a plugin-provided command ? */
	asp = find_plugin_command(str, op);
	if (asp)
	    return asp->func(op, NULL);

	if ((f=find_command(str)))
	    return f(op, NULL);
	if (QUERY_FLAG(op,FLAG_WIZ) && (f=find_wizcommand(str)))
	    return f(op, NULL);

	if(op) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "Unknown command.  Try help.", NULL);
	}
	return 0;
    }

    /*
     * Command with some arguments
     */

    *(cp++) ='\0';
    /* Clear all spaces from the start of the optional argument */
    while (*cp==' ') cp++;

    asp = find_plugin_command(str,op);
    if (asp) return asp->func(op,cp);

    if ((f=find_command(str)))
	return f(op, cp);
    if (QUERY_FLAG(op, FLAG_WIZ) && (f=find_wizcommand(str)))
	return f(op, cp);

    if(op) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Unknown command.  Try help.", NULL);
    }
    return 0;
}


/* Parse command is no longer used - should probably be removed.
 * MSW 2006-06-02
 */

/**  this function handles splitting up a ; separated
 *  compound command into sub-commands:  it is recursive.
 */
static int parse_command(object *op, char *str) {
  char *tmp,*tmp2;
  int i;
  /* if it's a keybinding command, ignore semicolons */
  if(strstr(str,"bind")) return parse_string(op,str);
  LOG(llevDebug, "parsin command '%s'\n", str);
  /* If on a socket, you can not do complex commands. */
  if(op && (tmp=strchr(str,';'))!=NULL)  /* we've found a ';' do the 1st and recurse */
	 {
	    char buf[MAX_BUF];
		/* copy 1st command into buf */
		/* Even if tmp2 points the the input_buf, this should still
		 * be safe operation.
		 */
		for(i=0,tmp2=str;tmp2!=tmp;i++,tmp2++) 
		    buf[i]= (*tmp2);
		buf[i]='\0'; /* null terminate the copy*/
		strncpy(op->contr->input_buf,tmp2+1, MAX_BUF);
		op->contr->input_buf[MAX_BUF-1]=0;
		parse_string(op,buf);
	 }
  else {
	/* We need to set the input_buf to 0 so clear any complex keybinding
	 * there might be.  However, str can be a pointer to input_buf in
	 * the case of a complex keybinding.  So first we process the command,
	 * clear the buffer, and then return the value.
	 */
	i=parse_string(op,str);
	if (op) op->contr->input_buf[0]=0;
	return i;
  }
  return 0;
}
#endif

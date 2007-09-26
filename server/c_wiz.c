/*
 * static char *rcsid_c_wiz_c =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * @file
 * Those functions are used by DMs.
 * @todo explain item stack, item specifier for commands.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <treasure.h>
#include <skills.h>

/* Defines for DM item stack **/
#define STACK_SIZE         50   /**< Stack size, static */
/** Values for 'from' field of get_dm_object() */
enum {
    STACK_FROM_NONE    = 0,   /**< Item was not found */
    STACK_FROM_TOP     = 1,   /**< Item is stack top */
    STACK_FROM_STACK   = 2,   /**< Item is somewhere in stack */
    STACK_FROM_NUMBER  = 3    /**< Item is a number (may be top) */
};

/**
 * Enough of the DM functions seem to need this that I broke
 * it out to a seperate function.  name is the person
 * being saught, op is who is looking for them.  This
 * prints diagnostics messages, and returns the
 * other player, or NULL otherwise.
 *
 * @param op
 * player searching someone.
 * @param name
 * name to search for.
 * @return
 * player, or NULL if player can't be found.
 * @todo change name to const char*.
 */
static player *get_other_player_from_name(object *op, char *name) {
    player *pl;

    if (!name)
        return NULL;

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (!strncmp(pl->ob->name, name, MAX_NAME))
            break;

    if (pl == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "No such player.", NULL);
        return NULL;
    }

    if (pl->ob == op) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You can't do that to yourself.", NULL);
        return NULL;
    }
    if (pl->state != ST_PLAYING) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "That player is in no state for that right now.", NULL);
        return NULL;
    }
    return pl;
}

/**
 * This command will stress server.
 *
 * It will basically load all world maps (so 900 maps).
 *
 * @param op
 * DM wanting to test the server.
 * @param params
 * option, must be "TRUE" for the test to happen.
 * @return
 * 0.
 */
int command_loadtest(object *op, char *params) {
    uint32 x, y;
    char buf[1024];

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
		  "loadtest will stress server through teleporting at different map places. "
		  "Use at your own risk.  Very long loop used so server may have to be reset. "
		  "type loadtest TRUE to run",
		  NULL);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
			 "{%s}",
			 "{%s}",
			 params);
    if (!params)
        return 0;
    if (strncmp (params, "TRUE", 4))
        return 0;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
			 "gogogo", NULL);

    for (x = 0; x < settings.worldmaptilesx; x++) {
        for (y = 0; y < settings.worldmaptilesy; y++) {
            sprintf(buf, "/world/world_%d_%d", x+settings.worldmapstartx, y+settings.worldmapstarty);
            command_goto(op, buf);
        }
    }

    return 0;
}

/**
 * Actually hides or unhides specified player (obviously a DM).
 *
 * @param op
 * DM hiding.
 * @param silent_dm
 * if non zero, other players are informed of DM entering/leaving, else they just think someone left/entered.
 */
void do_wizard_hide(object *op, int silent_dm) {
    if (op->contr->hidden) {
        op->contr->hidden = 0;
        op->invisible = 1;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		      "You are no longer hidden from other players", NULL);
        op->map->players++;
        draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
			     MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
			     "%s has entered the game.",
			     "%s has entered the game.",
			     op->name);
        if (!silent_dm) {
            draw_ext_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL,
		  MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
		  "The Dungeon Master has arrived!", NULL);
        }
    } else {
        op->contr->hidden = 1;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
		      "Other players will no longer see you.", NULL);
        op->map->players--;
        if (!silent_dm) {
            draw_ext_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL,
			  MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
			  "The Dungeon Master is gone..", NULL);
        }
        draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
			     MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
			     "%s leaves the game.",
			     "%s leaves the game.",
			     op->name);
        draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
			     MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
			     "%s left the game.",
			     "%s left the game.",
			     op->name);
    }
}

/**
 * Wizard 'hide' command.
 *
 * @param op
 * DM wanting to hide.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_hide(object *op, char *params)
{
    do_wizard_hide(op, 0);
    return 1;
}

/**
 * This finds and returns the object which matches the name or
 * object number (specified via num \#whatever).
 *
 * @param params
 * object to find.
 * @return
 * suitable object, or NULL if none found.
 */
static object *find_object_both(char *params) {
    if (!params)
        return NULL;
    if (params[0] == '#')
        return find_object(atol(params+1));
    else
        return find_object_name(params);
}

/**
 * Sets the god for some objects.
 *
 * @param op
 * DM wanting to change an object.
 * @param params
 * command options. Should contain two values, first the object to change, followed by the god to change it to.
 * @retval 0
 * syntax error.
 * @retval 1
 * correct syntax.
 */
int command_setgod(object *op, char *params) {
    object *ob, *god;
    char *str;

    if (!params || !(str = strchr(params, ' '))) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Usage: setgod object god", NULL);
        return 0;
    }

    /* kill the space, and set string to the next param */
    *str++ = '\0';
    if (!(ob = find_object_both(params))) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "Set whose god - can not find object %s?",
			     "Set whose god - can not find object %s?",
			     params);
        return 1;
    }

    /*
     * Perhaps this is overly restrictive?  Should we perhaps be able
     * to rebless altars and the like?
     */
    if (ob->type != PLAYER) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "%s is not a player - can not change its god",
			     "%s is not a player - can not change its god",
			     ob->name);
        return 1;
    }

    god = find_god(str);
    if (god==NULL) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "No such god %s.",
			     "No such god %s.",
			     str);
        return 1;
    }

    become_follower(ob, god);
    return 1;
}

/**
 * Add player's IP to ban_file and kick them off the server.
 *
 * I know most people have dynamic IPs but this is more of a short term
 * solution if they have to get a new IP to play maybe they'll calm down.
 * This uses the banish_file in the local directory *not* the ban_file
 * The action is logged with a ! for easy searching. -tm
 *
 * @param op
 * DM banishing.
 * @param params
 * player to banish. Must be a complete name match.
 * @return
 * 1.
 */
int command_banish(object *op, char *params) {
    player *pl;
    FILE *banishfile;
    char buf[MAX_BUF];
    time_t now;

    if (!params) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Usage: banish <player>.", NULL);
        return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    sprintf(buf, "%s/%s", settings.localdir, BANISHFILE);

    if ((banishfile = fopen(buf, "a")) == NULL) {
        LOG (llevDebug, "Could not find file banish_file.\n");
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Could not find banish_file.", NULL);
        return 0;
    }

    now = time(NULL);
    /*
     * Record this as a comment - then we don't have to worry about changing
     * the parsing code.
     */
    fprintf(banishfile, "# %s (%s) banned by %s at %s\n", pl->ob->name,
            pl->socket.host, op->name, ctime(&now));
    fprintf(banishfile, "*@%s\n", pl->socket.host);
    fclose(banishfile);

    LOG(llevDebug, "! %s banned %s from IP: %s.\n", op->name, pl->ob->name, pl->socket.host);

    draw_ext_info_format(NDI_UNIQUE|NDI_RED, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "You banish %s",
			 "You banish %s",
			 pl->ob->name);

    draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_RED, 5, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
                 "%s banishes %s from the land!",
                 "%s banishes %s from the land!",
		 op->name, pl->ob->name);
    command_kick(op, pl->ob->name);
    return 1;
}

/**
 * Kicks a player from the server.
 *
 * @param op
 * DM kicking.
 * @param params
 * player to kick. Must be a full name match.
 * @return
 * 1.
 */
int command_kick(object *op, const char *params) {
    struct pl *pl;

    for (pl = first_player; pl != NULL; pl = pl->next) {
        if ((params == NULL || !strcmp(pl->ob->name, params)) && pl->ob != op) {
            object *op;
            int removed = 0;

            op = pl->ob;
            if (!QUERY_FLAG(op, FLAG_REMOVED)) {
                /* Avion : Here we handle the KICK global event */
                execute_global_event(EVENT_KICK, op, params);
                remove_ob(op);
                removed = 1;
            }
            op->direction = 0;
            draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_RED, 5, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
				 "%s is kicked out of the game.",
				 "%s is kicked out of the game.",
				 op->name);
            strcpy(op->contr->killer, "left");
            check_score(op,0); /* Always check score */

            /*
             * not sure how the player would be freed, but did see
             * a crash here - if that is the case, don't save the
             * the player.
             */
            if (!removed && !QUERY_FLAG(op, FLAG_FREED)) {
                (void)save_player(op, 0);
                if (op->map)
                    op->map->players--;
            }
#if MAP_MAXTIMEOUT
            if (op->map)
                op->map->timeout = MAP_TIMEOUT(op->map);
#endif
            pl->socket.status = Ns_Dead;
        }
    }

    return 1;
}

/**
 * Saves the op's map as an overlay - objects are persisted.
 *
 * @param op
 * DM wanting to save.
 * @param params
 * ignored.
 * @return
 * 1 unless op is NULL.
 */
int command_overlay_save(object *op, char *params) {
    if (!op)
        return 0;

    if (save_map(op->map, SAVE_MODE_OVERLAY) < 0)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
          "Overlay save error!", NULL);
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_SUCCESS,
          "Current map has been saved as an overlay.", NULL);

    return 1;
}

/**
 * Removes the overlay for op's current map.
 *
 * @param op
 * DM acting.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_overlay_reset(object *op, char* params) {
    char filename[MAX_BUF];
    struct stat stats;
    create_overlay_pathname(op->map->path, filename, MAX_BUF);
    if (!stat(filename, &stats))
        if (!unlink(filename))
            draw_ext_info(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND,  MSG_TYPE_COMMAND_DM,
			  "Overlay successfully removed.", NULL);
        else
            draw_ext_info(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND,  MSG_TYPE_COMMAND_DM,
			  "Overlay couldn't be removed.", NULL);
    else
        draw_ext_info(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND,  MSG_TYPE_COMMAND_DM,
		      "No overlay for current map.", NULL);

    return 1;
}

/**
 * A simple toggle for the no_shout field. AKA the MUZZLE command.
 *
 * @param op
 * wizard toggling.
 * @param params
 * player to mute/unmute.
 * @return
 * 1.
 */
int command_toggle_shout(object *op, char *params) {
    player *pl;

    if (!params) {
         draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		       "Usage: toggle_shout <player>.", NULL);
         return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    if (pl->ob->contr->no_shout == 0) {
        pl->ob->contr->no_shout = 1;

        draw_ext_info(NDI_UNIQUE|NDI_RED, 0, pl->ob, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
		      "You have been muzzled by the DM!", NULL);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			     "You muzzle %s.",
			     "You muzzle %s.",
			     pl->ob->name);

        /* Avion : Here we handle the MUZZLE global event */
        execute_global_event(EVENT_MUZZLE, pl->ob, params);

        return 1;
    } else {
        pl->ob->contr->no_shout = 0;
        draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl->ob, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
            "You are allowed to shout and chat again.", NULL);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			     "You remove %s's muzzle.",
			     "You remove %s's muzzle.",
			     pl->ob->name);
        return 1;
    }
}

/**
 * Totally shutdowns the server.
 *
 * @param op
 * wizard shutting down the server.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_shutdown(object *op, char *params) {
    /*
     * We need to give op - command_kick expects it.  however, this means
     * the op won't get kicked off, so we do it ourselves
     */
    command_kick(op, NULL);
    check_score(op,0); /* Always check score */
    (void)save_player(op, 0);
    play_again(op);
    cleanup();
    /* not reached */
    return 1;
}

/**
 * Wizard teleports to a map.
 *
 * @param op
 * wizard teleporting.
 * @param params
 * map to teleport to. Can be absolute or relative path.
 * @return
 * 1 unless op is NULL.
 */
int command_goto(object *op, char *params)
{
    char *name;
    object *dummy;

    if (!op)
        return 0;

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Go to what level?", NULL);
        return 1;
    }

    name = params;
    dummy=get_object();
    dummy->map = op->map;
    EXIT_PATH(dummy) = add_string (name);
    dummy->name = add_string(name);

    enter_exit(op, dummy);
    free_object(dummy);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
        "Difficulty: %d.",
        "Difficulty: %d.",
        op->map->difficulty);

    return 1;
}

/**
 * Freezes a player for a specified tick count, 100 by default.
 *
 * @param op
 * wizard freezing the player.
 * @param params
 * optional tick count, followed by player name.
 * @return
 * 1.
 */
int command_freeze(object *op, char *params) {
    int ticks;
    player *pl;

    if (!params) {
         draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		       "Usage: freeze [ticks] <player>.", NULL);
         return 1;
    }

    ticks = atoi(params);
    if (ticks) {
        while ((isdigit(*params) || isspace(*params)) && *params != 0)
            params++;
        if (*params == 0) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			  "Usage: freeze [ticks] <player>.", NULL);
            return 1;
        }
    } else
        ticks = 100;

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    draw_ext_info(NDI_UNIQUE|NDI_RED, 0, pl->ob, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
		  "You have been frozen by the DM!", NULL);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "You freeze %s for %d ticks",
			 "You freeze %s for %d ticks",
			 pl->ob->name, ticks);

    pl->ob->speed_left = -(pl->ob->speed*ticks);
    return 0;
}

/**
 * Wizard jails player.
 *
 * @param op
 * wizard.
 * @param params
 * player to jail.
 * @return
 * 1.
 */
int command_arrest(object *op, char *params) {
    object *dummy;
    player *pl;
    if (!op) return 0;
    if(params==NULL) {
         draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		       "Usage: arrest <player>.", NULL);
         return 1;
    }
    pl = get_other_player_from_name(op, params);
    if (!pl) return 1;
    dummy=get_jail_exit(pl->ob);
    if (!dummy) {
	/* we have nowhere to send the prisoner....*/
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Can't jail player, there is no map to hold them", NULL);
	return 0;
    }
    enter_exit(pl->ob, dummy);
    free_object(dummy);
    draw_ext_info(NDI_UNIQUE, 0,pl->ob,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
		  "You have been arrested.", NULL);
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "Jailed %s",
			 "Jailed %s",
			 pl->ob->name);
    LOG(llevInfo, "Player %s arrested by %s\n", pl->ob->name, op->name);
    return 1;
}

/**
 * Summons player near DM.
 * @param op
 * DM.
 * @param params
 * player to summon.
 * @return
 * 1 unless op is NULL.
 */
int command_summon(object *op, char *params) {
    int i;
    object *dummy;
    player *pl;

    if (!op)
        return 0;

    if (params == NULL) {
         draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		       "Usage: summon <player>.", NULL);
         return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    i = find_free_spot(op, op->map, op->x, op->y, 1, 9);
    if (i == -1) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Can not find a free spot to place summoned player.", NULL);
        return 1;
    }

    dummy = get_object();
    EXIT_PATH(dummy) = add_string(op->map->path);
    EXIT_X(dummy) = op->x+freearr_x[i];
    EXIT_Y(dummy) = op->y+freearr_y[i];
    enter_exit(pl->ob, dummy);
    free_object(dummy);
    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
		  "You are summoned.", NULL);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "You summon %s",
			 "You summon %s",
			 pl->ob->name);
    return 1;
}

/**
 * Teleport next to target player.
 *
 * @param op
 * DM teleporting.
 * @param params
 * options sent by player.
 * @return
 * 0 if couldn't teleport, 1 if teleport successful.
 */
/* mids 01/16/2002 */
int command_teleport(object *op, char *params) {
    int i;
    object *dummy;
    player *pl;

    if (!op)
        return 0;

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Usage: teleport <player>.", NULL);
        return 0;
    }

    pl = find_player_partial_name(params);
    if (!pl) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "No such player or ambiguous name.", NULL);
        return 0;
    }

   i = find_free_spot(pl->ob, pl->ob->map, pl->ob->x, pl->ob->y, 1, 9);
   if (i == -1) {
      draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "Can not find a free spot to teleport to.", NULL);
      return 0;
   }

   dummy = get_object();
   EXIT_PATH(dummy) = add_string(pl->ob->map->path);
   EXIT_X(dummy) = pl->ob->x + freearr_x[i];
   EXIT_Y(dummy) = pl->ob->y + freearr_y[i];
   enter_exit(op, dummy);
   free_object(dummy);
   if (!op->contr->hidden)
      draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
		    "You see a portal open.", NULL);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "You teleport to %s",
			 "You teleport to %s",
			 pl->ob->name);

   return 1;
}

/**
 * Wizard wants to create an object.
 *
 * This function is a real mess, because we're stucking getting
 * the entire item description in one block of text, so we just
 * can't simply parse it - we need to look for double quotes
 * for example.  This could actually get much simpler with just a
 * little help from the client - if we could get line breaks, it
 * makes parsing much easier, eg, something like:
 * - arch dragon
 * - name big nasty creature
 * - hp 5
 * - sp 30
 *
 * which is much easier to parse than
 * dragon name "big nasty creature" hp 5 sp 30
 * for example.
 *
 * @param op
 * wizard.
 * @param params
 * object description.
 * @return
 * 1 unless op is NULL.
 * @todo enable line breaks in command.
 */
int command_create(object *op, char *params) {
    object *tmp = NULL;
    int nrof, i, magic, set_magic = 0, set_nrof = 0, gotquote, gotspace;
    char buf[MAX_BUF], *cp, *bp = buf, *bp2, *bp3, *bp4, *endline;
    archetype *at, *at_spell = NULL;
    artifact *art = NULL;

    if (!op)
        return 0;

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
	      "Usage: create [nr] [magic] <archetype> [ of <artifact>] [variable_to_patch setting]",
	      NULL);
        return 1;
    }
    bp = params;

    /* We need to know where the line ends */
    endline = bp+strlen(bp);

    if (sscanf(bp, "%d ", &nrof)) {
        if ((bp = strchr(params, ' ')) == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "Usage: create [nr] [magic] <archetype> [ of <artifact>] [variable_to_patch setting]",
		  NULL);
            return 1;
        }
        bp++;
        set_nrof = 1;
        LOG(llevDebug, "%s creates: (%d) %s\n", op->name, nrof, bp);
    }
    if (sscanf(bp, "%d ", &magic)) {
        if ((bp = strchr(bp, ' ')) == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "Usage: create [nr] [magic] <archetype> [ of <artifact>] [variable_to_patch setting]",
		  NULL);
            return 1;
        }
        bp++;
        set_magic = 1;
        LOG(llevDebug, "%s creates: (%d) (%d) %s\n", op->name, nrof, magic, bp);
    }
    if ((cp = strstr(bp, " of ")) != NULL) {
        *cp = '\0';
        cp += 4;
    }
    for (bp2 = bp; *bp2; bp2++) {
        if (*bp2 == ' ') {
            *bp2 = '\0';
            bp2++;
            break;
        }
    }

    if ((at = find_archetype(bp)) == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "No such archetype.", NULL);
        return 1;
    }

    if (cp) {
        char spell_name[MAX_BUF], *fsp = NULL;

        /*
         * Try to find a spell object for this. Note that
         * we also set up spell_name which is only
         * the first word.
         */

        at_spell = find_archetype(cp);
        if (!at_spell || at_spell->clone.type != SPELL)
            at_spell = find_archetype_by_object_name(cp);
        if (!at_spell || at_spell->clone.type != SPELL) {
            strcpy(spell_name, cp);
            fsp = strchr(spell_name, ' ');
            if (fsp) {
                *fsp = 0;
                fsp++;
                at_spell = find_archetype(spell_name);

                /* Got a spell, update the first string pointer */
                if (at_spell && at_spell->clone.type == SPELL)
                    bp2 = cp+strlen(spell_name)+1;
                else
                    at_spell = NULL;
            } else
                at_spell = NULL;
        }

        /* OK - we didn't find a spell - presume the 'of'
         * in this case means its an artifact.
         */
        if (!at_spell) {
            if (find_artifactlist(at->clone.type) == NULL) {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                     "No artifact list for type %d\n",
                     "No artifact list for type %d\n",
		     at->clone.type);
            } else {
                art = find_artifactlist(at->clone.type)->items;

                do {
                    if (!strcmp(art->item->name, cp))
                        break;
                    art = art->next;
                } while (art != NULL);
                if (!art) {
                    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                        "No such artifact ([%d] of %s)",
                        "No such artifact ([%d] of %s)",
			 at->clone.type, cp);
                }
            }
            LOG(llevDebug, "%s creates: (%d) (%d) (%s) of (%s)\n", op->name,
                set_nrof ? nrof : 0, set_magic ? magic : 0, bp, cp);
        }
    } /* if cp */

    if ((at->clone.type == ROD || at->clone.type == WAND || at->clone.type == SCROLL ||
        at->clone.type == HORN || at->clone.type == SPELLBOOK) && !at_spell) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Unable to find spell %s for object that needs it, or it is of wrong type",
            "Unable to find spell %s for object that needs it, or it is of wrong type",
            cp);
        return 1;
    }

    /*
     * Rather than have two different blocks with a lot of similar code,
     * just create one object, do all the processing, and then determine
     * if that one object should be inserted or if we need to make copies.
     */
    tmp = object_create_arch(at);
    if (settings.real_wiz == FALSE)
        SET_FLAG(tmp, FLAG_WAS_WIZ);
    if (set_magic)
        set_abs_magic(tmp, magic);
    if (art)
        give_artifact_abilities(tmp, art->item);
    if (need_identify(tmp)) {
        SET_FLAG(tmp, FLAG_IDENTIFIED);
        CLEAR_FLAG(tmp, FLAG_KNOWN_MAGICAL);
    }

    /*
     * This entire block here tries to find variable pairings,
     * eg, 'hp 4' or the like.  The mess here is that values
     * can be quoted (eg "my cool sword");  So the basic logic
     * is we want to find two spaces, but if we got a quote,
     * any spaces there don't count.
     */
    while (*bp2 && bp2 <= endline) {
        bp4 = NULL;
        gotspace = 0;
        gotquote = 0;
        /* find the first quote */
        for (bp3 = bp2; *bp3 && gotspace < 2 && gotquote < 2; bp3++) {

            /* Found a quote - now lets find the second one */
            if (*bp3 == '"') {
                *bp3 = ' ';
                bp2 = bp3+1;    /* Update start of string */
                bp3++;
                gotquote++;
                while (*bp3) {
                    if (*bp3 == '"') {
                        *bp3 = '\0';
                        gotquote++;
                    } else
                        bp3++;
                }
            } else if (*bp3==' ') {
                gotspace++;
            }
        }

        /*
         * If we got two spaces, send the second one to null.
         * if we've reached the end of the line, increase gotspace -
         * this is perfectly valid for the list entry listed.
         */
        if (gotspace == 2 || gotquote == 2) {
            bp3--;      /* Undo the extra increment */
            *bp3 = '\0';
        } else if (*bp3=='\0')
            gotspace++;

        if ((gotquote && gotquote != 2) || (gotspace != 2 && gotquote != 2)) {
            /*
             * Unfortunately, we've clobbered lots of values, so printing
             * out what we have probably isn't useful.  Break out, because
             * trying to recover is probably won't get anything useful
             * anyways, and we'd be confused about end of line pointers
             * anyways.
             */
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "Malformed create line: %s",
				 "Malformed create line: %s",
				 bp2);
            break;
        }
        /* bp2 should still point to the start of this line,
         * with bp3 pointing to the end
         */
        if (set_variable(tmp, bp2) == -1)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "Unknown variable %s",
				 "Unknown variable %s",
				 bp2);
        else
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
				 "(%s#%d)->%s",
				 "(%s#%d)->%s",
				 tmp->name, tmp->count, bp2);
        bp2 = bp3+1;
    }

    if (at->clone.nrof) {
        if (at_spell)
            insert_ob_in_ob(arch_to_object(at_spell), tmp);

        tmp->x = op->x;
        tmp->y = op->y;
        if (set_nrof)
            tmp->nrof = nrof;
        tmp->map = op->map;

        if (at->clone.randomitems != NULL && !at_spell)
            create_treasure(at->clone.randomitems, tmp, GT_APPLY,
                op->map->difficulty, 0);

        /* Multipart objects can't be in inventory, put'em on floor. */
        if (!tmp->more) {
            tmp = insert_ob_in_ob(tmp, op);
        }
        else {
            insert_ob_in_map_at(tmp, op->map, op, 0, op->x, op->y);
        }

        /* Let's put this created item on stack so dm can access it easily. */
        dm_stack_push(op->contr, tmp->count);

        return 1;
    } else {
        for (i = 0 ; i < (set_nrof ? nrof : 1); i++) {
            archetype *atmp;
            object *prev = NULL, *head = NULL, *dup;

            for (atmp = at; atmp != NULL; atmp = atmp->more) {
                dup = arch_to_object(atmp);

                if (at_spell)
                    insert_ob_in_ob(arch_to_object(at_spell), dup);

                /*
                 * The head is what contains all the important bits,
                 * so just copying it over should be fine.
                 */
                if (head == NULL) {
                    head=dup;
                    copy_object(tmp, dup);
                }
                if (settings.real_wiz == FALSE)
                    SET_FLAG(dup, FLAG_WAS_WIZ);
                dup->x = op->x+dup->arch->clone.x;
                dup->y = op->y+dup->arch->clone.y;
                dup->map = op->map;

                if (head != dup) {
                    dup->head = head;
                    prev->more = dup;
                }
                prev = dup;
            }

            if (QUERY_FLAG(head, FLAG_ALIVE)) {
                object *check = head;
                int size_x = 0;
                int size_y = 0;

                while (check) {
                    size_x = MAX(size_x, check->arch->clone.x);
                    size_y = MAX(size_y, check->arch->clone.y);
                    check = check->more;
                }

                if (out_of_map(op->map, head->x+size_x, head->y+size_y)) {
                    if (head->x < size_x || head->y < size_y) {
                        dm_stack_pop(op->contr);
                        free_object(head);
                        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				      "Object too big to insert in map, or wrong position.", NULL);
                        free_object(tmp);
                        return 1;
                    }

                    check = head;
                    while (check) {
                        check->x -= size_x;
                        check->y -= size_y;
                        check = check->more;
                    }
                }

                insert_ob_in_map(head, op->map, op, 0);
            } else
                head = insert_ob_in_ob(head, op);

            /* Let's put this created item on stack so dm can access it easily. */
            /* Wonder if we really want to push all of these, but since
             * things like rods have nrof 0, we want to cover those.
             */
            dm_stack_push(op->contr, head->count);

            if (at->clone.randomitems != NULL && !at_spell)
                create_treasure(at->clone.randomitems, head, GT_APPLY,
                          op->map->difficulty, 0);
        }

        /* free the one we used to copy */
        free_object(tmp);
    }

    return 1;
}

/*
 * Now follows dm-commands which are also acceptable from sockets
 */

/**
 * Shows the inventory or some item.
 *
 * @param op
 * player.
 * @param params
 * object count to get the inventory of. If NULL then defaults to op.
 * @return
 * 1 unless params is NULL.
 */
int command_inventory(object *op, char *params) {
    object *tmp;
    int i;

    if (!params) {
        inventory(op, NULL);
        return 0;
    }

    if (!sscanf(params, "%d", &i) || (tmp = find_object(i)) == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Inventory of what object (nr)?", NULL);
        return 1;
    }

    inventory(op, tmp);
    return 1;
}

/**
 * Player is asking for her skills.
 *
 * Just show player's their skills for now. Dm's can
 * already see skills w/ inventory command - b.t.
 *
 * @param op
 * player.
 * @param params
 * optional skill restriction.
 * @return
 * 0.
 * @todo move out of this file as it is used by all players.
 */
int command_skills (object *op, char *params) {
    show_skills(op, params);
    return 0;
}

/**
 * Dumps the difference between an object and its archetype.
 *
 * @param op
 * wiard.
 * @param params
 * object to dump.
 * @return
 * 1.
 */
int command_dump (object *op, char *params) {
    object *tmp;
    char buf[HUGE_BUF];

    tmp = get_dm_object(op->contr, &params, NULL);
    if (!tmp)
        return 1;

    dump_object(tmp, buf, sizeof(buf));
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM, buf, buf);
    if (QUERY_FLAG(tmp, FLAG_OBJ_ORIGINAL))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Object is marked original", NULL);
    return 1;
}

/**
 *  When DM is possessing a monster, flip aggression on and off, to allow
 * better motion.
 *
 * @param op
 * wiard.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_mon_aggr(object *op, char *params) {
    if (op->enemy || !QUERY_FLAG(op, FLAG_UNAGGRESSIVE)) {
        op->enemy = NULL;
        SET_FLAG(op, FLAG_UNAGGRESSIVE);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Aggression turned OFF", NULL);
    } else {
        CLEAR_FLAG(op, FLAG_FRIENDLY);
        CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Aggression turned ON", NULL);
    }

    return 1;
}

/**
 * DM can possess a monster.  Basically, this tricks the client into thinking
 * a given monster, is actually the player it controls.  This allows a DM
 * to inhabit a monster's body, and run around the game with it.
 * This function is severely broken - it has tons of hardcoded values,
 *
 * @param op
 * wizard wanting to possess something.
 * @param params
 * monster to possess.
 * @return
 * 1.
 * @todo fix and reactivate the function, or totally trash.
 */
int command_possess(object *op, char *params) {
    object *victim;
    player *pl;
    int i;
    char buf[MAX_BUF];

    victim = NULL;
    if (params != NULL) {
        if (sscanf(params, "%d", &i))
            victim = find_object(i);
        else if (sscanf(params, "%s", buf))
            victim = find_object_name(buf);
    }
    if (victim == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Patch what object (nr)?", NULL);
        return 1;
    }

    if (victim == op) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "As insane as you are, I cannot allow you to possess yourself.", NULL);
        return 1;
    }

    /* make the switch */
    pl = op->contr;
    victim->contr = pl;
    pl->ob = victim;
    victim->type = PLAYER;
    SET_FLAG(victim, FLAG_WIZ);

    /* basic patchup */
    /* The use of hard coded values is terrible.  Note
     * that really, to be fair, this shouldn't get changed at
     * all - if you are possessing a kobold, you should have the
     * same limitations.  As it is, as more body locations are added,
     * this will give this player more locations than perhaps
     * they should be allowed.
     */
    for (i = 0; i < NUM_BODY_LOCATIONS; i++)
        if (i == 1 || i == 6 || i == 8 || i == 9)
            victim->body_info[i] = 2;
        else
            victim->body_info[i] = 1;

    esrv_new_player(pl, 80); /* just pick a weight, we don't care */
    esrv_send_inventory(victim, victim);

    fix_object(victim);

    do_some_living(victim);
    return 1;
}

/**
 * Wizard wants to altar an object.
 * @param op
 * wizard.
 * @param params
 * object and what to patch.
 * @return
 * 1.
 */
int command_patch(object *op, char *params) {
    char *arg, *arg2;
    object *tmp;

    tmp = get_dm_object(op->contr, &params, NULL);
    if (!tmp)
      /* Player already informed of failure */
      return 1;

    /* params set to first value by get_dm_default */
    arg = params;
    if (arg == NULL) {
      draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		    "Patch what values?", NULL);
      return 1;
    }

    if ((arg2 = strchr(arg, ' ')))
        arg2++;
    if (settings.real_wiz == FALSE)
        SET_FLAG(tmp, FLAG_WAS_WIZ); /* To avoid cheating */
    if (set_variable(tmp, arg) == -1)
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "Unknown variable %s",
			     "Unknown variable %s",
			     arg);
    else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			     "(%s#%d)->%s=%s",
			     "(%s#%d)->%s=%s",
			     tmp->name, tmp->count, arg, arg2);
    }

    return 1;
  }

/**
 * Remove an object from its position.
 *
 * @param op
 * wizard.
 * @param params
 * object to remove.
 * @return
 * 1.
 */
int command_remove (object *op, char *params) {
    object *tmp;
    int from;

    tmp = get_dm_object(op->contr, &params, &from);
    if (!tmp) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Remove what object (nr)?", NULL);
        return 1;
    }

    if (tmp->type == PLAYER) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Unable to remove a player!", NULL);
        return 1;
    }

    if (QUERY_FLAG(tmp, FLAG_REMOVED)) {
        char name[MAX_BUF];
        query_name(tmp, name, MAX_BUF);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "%s is already removed!",
			     "%s is already removed!",
			     name);
        return 1;
    }

    if (from != STACK_FROM_STACK)
        /* Item is either stack top, or is a number thus is now stack top, let's remove it  */
        dm_stack_pop(op->contr);

    /* Always work on the head - otherwise object will get in odd state */
    if (tmp->head)
        tmp = tmp->head;
    if (tmp->speed != 0) {
        tmp->speed = 0;
        update_ob_speed(tmp);
    }
    remove_ob(tmp);
    return 1;
}

/**
 * Totally free an object.
 * @param op
 * wizard.
 * @param params
 * object to free.
 * @return
 * 1.
 */
int command_free(object *op, char *params) {
    object *tmp;
    int from;

    tmp = get_dm_object(op->contr, &params, &from);

    if (!tmp) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Free what object (nr)?", NULL);
        return 1;
    }

    if (from != STACK_FROM_STACK)
        /* Item is either stack top, or is a number thus is now stack top, let's remove it  */
        dm_stack_pop(op->contr);

    if (tmp->head)
        tmp = tmp->head;

    if (!QUERY_FLAG(tmp, FLAG_REMOVED)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Warning: item was not removed, will do so now.", NULL);
        remove_ob(tmp);
    }

    free_object(tmp);
    return 1;
}

/**
 * This adds exp to a player.  We now allow adding to a specific skill.
 *
 * @param op
 * wizard.
 * @param params
 * should be "player quantity [skill]".
 * @return
 * 1.
 */
int command_addexp(object *op, char *params) {
    char buf[MAX_BUF], skill[MAX_BUF];
    int i, q;
    object *skillob = NULL;
    player *pl;

    skill[0] = '\0';
    if ((params == NULL) || (strlen(params) > MAX_BUF) || ((q = sscanf(params, "%s %d %s", buf, &i, skill)) < 2)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Usage: addexp player quantity [skill].", NULL);
        return 1;
    }

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (!strncmp(pl->ob->name, buf, MAX_NAME))
            break;

    if (pl == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "No such player.", NULL);
        return 1;
    }

    if (q >= 3) {
        skillob = find_skill_by_name(pl->ob, skill);
        if (!skillob) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "Unable to find skill %s in %s",
				 "Unable to find skill %s in %s",
				 skill, buf);
            return 1;
        }

        i = check_exp_adjust(skillob, i);
        skillob->stats.exp += i;
        calc_perm_exp(skillob);
        player_lvl_adj(pl->ob, skillob);
    }

    pl->ob->stats.exp += i;
    calc_perm_exp(pl->ob);
    player_lvl_adj(pl->ob, NULL);

    if (settings.real_wiz == FALSE)
        SET_FLAG(pl->ob, FLAG_WAS_WIZ);
    return 1;
}

/**
 * Changes the server speed.
 *
 * @param op
 * wizard.
 * @param params
 * new speed, or NULL to see the speed.
 * @return
 * 1.
 */
int command_speed(object *op, char *params) {
    int i;

    if (params == NULL || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			     "Current speed is %d",
			     "Current speed is %d",
			     max_time);
        return 1;
    }

    set_max_time(i);
    reset_sleep();
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		  "The speed is changed to %d.",
		  "The speed is changed to %d.",
		  i);
    return 1;
}

/**************************************************************************/
/* Mods made by Tyler Van Gorder, May 10-13, 1992.                        */
/* CSUChico : tvangod@cscihp.ecst.csuchico.edu                            */
/**************************************************************************/

/**
 * Displays the statistics of a player.
 *
 * @param op
 * wizard.
 * @param params
 * player's name.
 * @return
 * 1.
 * @todo use the get_other_player_from_name() function.
 */
int command_stats(object *op, char *params) {
    char thing[20];
    player *pl;

    thing[0] = '\0';
    if (params == NULL || !sscanf(params, "%s", thing) || thing == NULL) {
       draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		     "Who?", NULL);
       return 1;
    }

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (!strcmp(pl->ob->name, thing)) {
	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Str : %-2d      H.P. : %-4d  MAX : %d",
			 "Str : %-2d      H.P. : %-4d  MAX : %d",
			 pl->ob->stats.Str, pl->ob->stats.hp, pl->ob->stats.maxhp);
	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Dex : %-2d      S.P. : %-4d  MAX : %d",
			 "Dex : %-2d      S.P. : %-4d  MAX : %d",
			 pl->ob->stats.Dex, pl->ob->stats.sp, pl->ob->stats.maxsp);

	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Con : %-2d        AC : %-4d  WC  : %d",
			 "Con : %-2d        AC : %-4d  WC  : %d",
			 pl->ob->stats.Con, pl->ob->stats.ac, pl->ob->stats.wc) ;

	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Int : %-2d    Damage : %d",
			 "Int : %-2d    Damage : %d",
			 pl->ob->stats.Int, pl->ob->stats.dam);

	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Wis : %-2d       EXP : %" FMT64,
			 "Wis : %-2d       EXP : %" FMT64,
			 pl->ob->stats.Wis, pl->ob->stats.exp);

	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Pow : %-2d    Grace : %d",
			 "Pow : %-2d    Grace : %d",
			 pl->ob->stats.Pow, pl->ob->stats.grace);

	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "[fixed]Cha : %-2d      Food : %d",
			 "Cha : %-2d      Food : %d",
			 pl->ob->stats.Cha, pl->ob->stats.food);
            break;
        }
    if (pl == NULL)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "No such player.", NULL);
    return 1;
}

/**
 * Changes an object's statistics.
 *
 * @param op
 * wizard.
 * @param params
 * parameters, should be "player statistic new_value".
 * @return
 * 1.
 * @todo use get_other_player_from_name(). Isn't this useless with the command_patch()?
 */
int command_abil(object *op, char *params) {
    char thing[20], thing2[20];
    int iii;
    player *pl;

    iii = 0;
    thing[0] = '\0';
    thing2[0] = '\0';
    if (params == NULL || !sscanf(params, "%s %s %d", thing, thing2, &iii) ||
        thing==NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Who?", NULL);
        return 1;
    }

    if (thing2 == NULL){
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You can't change that.", NULL);
        return 1;
    }

    if (iii < MIN_STAT || iii > MAX_STAT) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Illegal range of stat.\n", NULL);
        return 1;
    }

    for (pl = first_player; pl != NULL; pl = pl->next) {
        if (!strcmp(pl->ob->name, thing)) {
            if (settings.real_wiz == FALSE)
                SET_FLAG(pl->ob, FLAG_WAS_WIZ);
            if (!strcmp("str", thing2))
                pl->ob->stats.Str = iii, pl->orig_stats.Str = iii;
            if (!strcmp("dex", thing2))
                pl->ob->stats.Dex = iii, pl->orig_stats.Dex = iii;
            if (!strcmp("con", thing2))
                pl->ob->stats.Con = iii, pl->orig_stats.Con = iii;
            if (!strcmp("wis", thing2))
                pl->ob->stats.Wis = iii, pl->orig_stats.Wis = iii;
            if (!strcmp("cha", thing2))
                pl->ob->stats.Cha = iii, pl->orig_stats.Cha = iii;
            if (!strcmp("int", thing2))
                pl->ob->stats.Int = iii, pl->orig_stats.Int = iii;
            if (!strcmp("pow", thing2))
                pl->ob->stats.Pow = iii, pl->orig_stats.Pow = iii;
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "%s has been altered.",
			 "%s has been altered.",
			 pl->ob->name);
            fix_object(pl->ob);
            return 1;
        }
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		  "No such player.", NULL);
    return 1;
}

/**
 * Resets a map.
 *
 * @param op
 * wizard.
 * @param params
 * map to reset. Can be "." for current op's map, or a map path.
 * @return
 * 1.
 */
int command_reset (object *op, char *params) {
    mapstruct *m;
    object *dummy = NULL, *tmp = NULL;
    char path[HUGE_BUF];
    int res = 0;

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Reset what map [name]?", NULL);
        return 1;
    }

    if (strcmp(params, ".") == 0)
        snprintf(path, sizeof(path), op->map->path);
    else
        path_combine_and_normalize(op->map->path, params, path, sizeof(path));
    m = has_been_loaded(path);
    if (m == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "No such map.", NULL);
        return 1;
    }

    /* Forbid using reset on our own map when we're in a transport, as
     * it has the displeasant effect of crashing the server.
     * - gros, July 25th 2006 */
    if ((op->contr && op->contr->transport)&&(op->map == m))
    {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You need to disembark first.", NULL);
        return 1;
    }

	snprintf(path, sizeof(path), m->path);

    if (m->in_memory != MAP_SWAPPED) {
        if (m->in_memory != MAP_IN_MEMORY) {
            LOG(llevError, "Tried to swap out map which was not in memory.\n");
            return 0;
        }

        /*
         * Only attempt to remove the player that is doing the reset, and not other
         * players or wiz's.
         */
        if (op->map == m) {
            if (strncmp(m->path, "/random/", 8)==0) {
            /* This is not a very satisfying solution - it would be much better
             * to recreate a random map with the same seed value as the old one.
             * Unfortunately, I think recreating the map would require some
             * knowledge about its 'parent', which appears very non-trivial to
             * me.
             * On the other hand, this should prevent the freeze that this
             * situation caused. - gros, 26th July 2006.
             */
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                    "You cannot reset a random map when inside it.", NULL);
                return 1;
            }

            dummy = get_object();
            dummy->map = NULL;
            EXIT_X(dummy) = op->x;
            EXIT_Y(dummy) = op->y;
            EXIT_PATH(dummy) = add_string(op->map->path);
            remove_ob(op);
            op->map = NULL;
            tmp = op;
        }
        res = swap_map(m);
    }

    if (res < 0 || m->in_memory != MAP_SWAPPED) {
        player *pl;
        int playercount = 0;

        /* Need to re-insert player if swap failed for some reason */
        if (tmp) {
            insert_ob_in_map(op, m, NULL, 0);
            free_object(dummy);
        }

        if (res < 0 && res != SAVE_ERROR_PLAYER)
            /* no need to warn if player on map, code below checks that. */
            draw_ext_info_format(NDI_UNIQUE | NDI_RED, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                "Reset failed, error code: %d.", NULL, res);
        else {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                "Reset failed, couldn't swap map, the following players are on it:",
                NULL);
            for (pl = first_player; pl != NULL; pl = pl->next) {
                if (pl->ob->map == m && pl->ob != op) {
                    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                        pl->ob->name, NULL);
                    playercount++;
                }
            }
            if (!playercount)
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                    "hmm, I don't see any other players on this map, something else is the problem.",
                    NULL);
            return 1;
        }
    }

    /* Here, map reset succeeded. */

    if (m && m->in_memory == MAP_SWAPPED) {
        LOG(llevDebug, "DM %s Resetting map %s.\n", op->name, m->path);

        /* setting this effectively causes an immediate reload */
        m->reset_time = 1;
        flush_old_maps();
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
        "Resetting map %s.",
        "Resetting map %s.",
        path);

    if (tmp) {
        enter_exit(tmp, dummy);
        free_object(dummy);
    }

    return 1;
}

/**
 * Steps down from wizard mode.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_nowiz(object *op, char *params) { /* 'noadm' is alias */
    CLEAR_FLAG(op, FLAG_WIZ);
    CLEAR_FLAG(op, FLAG_WIZPASS);
    CLEAR_FLAG(op, FLAG_WIZCAST);
    if (op->contr->followed_player)
        FREE_AND_CLEAR_STR(op->contr->followed_player);

    if (settings.real_wiz == TRUE)
        CLEAR_FLAG(op, FLAG_WAS_WIZ);
    if (op->contr->hidden) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
            "You are no longer hidden from other players", NULL);
        op->map->players++;
        draw_ext_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
            MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
            "%s has entered the game.",
            "%s has entered the game.",
            op->name);
        op->contr->hidden = 0;
        op->invisible = 1;
    } else
        draw_ext_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
            "The Dungeon Master is gone..", NULL);

    update_los(op);

    return 1;
}

/**
 * object *op is trying to become dm.
 * pl_name is name supplied by player.  Restrictive DM will make it harder
 * for socket users to become DM - in that case, it will check for the players
 * character name.
 *
 * @param op
 * player wishing to become DM.
 * @param pl_name
 * player's name.
 * @param pl_passwd
 * entered password.
 * @param pl_host
 * player's host.
 * @retval 0
 * invalid credentials.
 * @retval 1
 * op can become DM.
 * @todo can't name/host be found from op? What is RESTRICTIVE_DM?
 */
static int checkdm(object *op, const char *pl_name, const char *pl_passwd, const char *pl_host)
{
    FILE *dmfile;
    char buf[MAX_BUF];
    char line_buf[160], name[160], passwd[160], host[160];

#ifdef RESTRICTIVE_DM
    *pl_name = op->name ? op->name : "*";
#endif

    sprintf(buf, "%s/%s", settings.confdir, DMFILE);
    if ((dmfile = fopen(buf, "r")) == NULL) {
        LOG(llevDebug, "Could not find DM file.\n");
        return 0;
    }

    while (fgets(line_buf, 160, dmfile) != NULL) {
        if (line_buf[0] == '#')
            continue;
        if (sscanf(line_buf, "%[^:]:%[^:]:%s\n", name, passwd, host) != 3) {
            LOG(llevError, "Warning - malformed dm file entry: %s\n", line_buf);
        } else if ((!strcmp(name, "*") || (pl_name && !strcmp(pl_name, name)))
            && (!strcmp(passwd, "*") || !strcmp(passwd, pl_passwd)) &&
            (!strcmp(host, "*") || !strcmp(host, pl_host))) {
            fclose(dmfile);
            return (1);
        }
    }
    fclose(dmfile);
    return (0);
}

/**
 * Actually changes a player to wizard.
 *
 * @param op
 * player.
 * @param params
 * password.
 * @param silent
 * if zero, don't inform players of the mode change.
 * @retval 0
 * no mode change.
 * @retval 1
 * op is now a wizard.
 */
int do_wizard_dm(object *op, char *params, int silent) {
    if (!op->contr)
        return 0;

    if (QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "You are already the Dungeon Master!", NULL);
        return 0;
    }

    if (checkdm(op, op->name,
            (params ? params : "*"), op->contr->socket.host)) {
        SET_FLAG(op, FLAG_WIZ);
        SET_FLAG(op, FLAG_WAS_WIZ);
        SET_FLAG(op, FLAG_WIZPASS);
        SET_FLAG(op, FLAG_WIZCAST);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Ok, you are the Dungeon Master!", NULL);
        /*
         * Remove setting flying here - that won't work, because next
        * fix_object() is called that will get cleared - proper solution
         * is probably something like a wiz_force which gives that and any
         * other desired abilities.
         */
        clear_los(op);
        op->contr->write_buf[0] ='\0';

        if (!silent)
            draw_ext_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL,
			  MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM,
			  "The Dungeon Master has arrived!", NULL);

        return 1;
    } else {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Sorry Pal, I don't think so.", NULL);
        op->contr->write_buf[0] ='\0';
        return 0;
    }
}

/**
 * Actual command to perhaps become dm.  Changed around a bit in version 0.92.2
 * to allow people on sockets to become dm, and allow better dm file
 *
 * @param op
 * player wishing to become wizard.
 * @param params
 * password.
 * @return
 * 0 unless op isn't a player.
 */
int command_dm(object *op, char *params) {

    do_wizard_dm(op, params, 0);

    return 1;
}

/**
 * Wizard wants to become invisible.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 0.
 */
int command_invisible(object *op, char *params) {
    if (op) {
        op->invisible += 100;
        update_object(op, UP_OBJ_FACE);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "You turn invisible.", NULL);
    }

    return 0;
}

/**
 * Returns spell object (from archetypes) by name.
 * Used for wizard's learn spell/prayer.
 *
 * Ignores archetypes "spelldirect_xxx" since these archetypes are not used
 * anymore (but may still be present in some player's inventories and thus
 * cannot be removed). We have to ignore them here since they have the same
 * name than other "spell_xxx" archetypes and would always conflict.
 **
 * @param op
 * player issuing the command.
 * @param spell_name
 * spell to find.
 * @return
 * NULL if 0 or more than one spell matches, spell object else.
 * @todo remove the spelldirect_xxx test?
 */
static object *get_spell_by_name(object *op, const char *spell_name) {
    archetype *ar;
    archetype *found;
    int conflict_found;
    size_t spell_name_length;

    /* First check for full name matches. */
    conflict_found = 0;
    found = NULL;
    for (ar = first_archetype; ar != NULL; ar = ar->next) {
        if (ar->clone.type != SPELL)
            continue;

        if (strncmp(ar->name, "spelldirect_", 12) == 0)
            continue;

        if (strcmp(ar->clone.name, spell_name) != 0)
            continue;

        if (found != NULL) {
            if (!conflict_found) {
                conflict_found = 1;
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "More than one archetype matches the spell name %s:",
				     "More than one archetype matches the spell name %s:",
				     spell_name);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "- %s",
				     "- %s",
				     found->name);
            }
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "- %s",
				 "- %s",
				 ar->name);
            continue;
        }

        found = ar;
    }

    /* No match if more more than one archetype matches. */
    if (conflict_found)
        return NULL;

    /* Return if exactly one archetype matches. */
    if (found != NULL)
        return arch_to_object(found);

    /* No full match found: now check for partial matches. */
    spell_name_length = strlen(spell_name);
    conflict_found = 0;
    found = NULL;
    for (ar = first_archetype; ar != NULL; ar = ar->next) {
        if (ar->clone.type != SPELL)
            continue;

        if (strncmp(ar->name, "spelldirect_", 12) == 0)
            continue;

        if (strncmp(ar->clone.name, spell_name, spell_name_length) != 0)
            continue;

        if (found != NULL) {
            if (!conflict_found) {
                conflict_found = 1;
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "More than one spell matches %s:",
				     "More than one spell matches %s:",
				     spell_name);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				     "- %s",
				     "- %s",
				     found->clone.name);
            }
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "- %s",
				 "- %s",
				 ar->clone.name);
            continue;
        }

        found = ar;
    }

    /* No match if more more than one archetype matches. */
    if (conflict_found)
        return NULL;

    /* Return if exactly one archetype matches. */
    if (found != NULL)
        return arch_to_object(found);

    /* No spell found: just print an error message. */
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			 "The spell %s does not exist.",
			 "The spell %s does not exist.",
			 spell_name);
    return NULL;
}

/**
 * Wizards wants to learn a spell.
 *
 * @param op
 * wizard.
 * @param params
 * spell name to learn.
 * @param special_prayer
 * if set, special (god-given) prayer.
 * @retval 0
 * spell wasn't learned, or was already learnt.
 * @retval 1
 * spell learned.
 */
static int command_learn_spell_or_prayer(object *op, char *params,
                                         int special_prayer) {
    object *tmp;

    if (op->contr == NULL || params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Which spell do you want to learn?", NULL);
        return 0;
    }

    tmp = get_spell_by_name(op, params);
    if (tmp == NULL) {
        return 0;
    }

    if (check_spell_known(op, tmp->name)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "You already know the spell %s.",
			     "You already know the spell %s.",
			     tmp->name);
        return 0;
    }

    do_learn_spell(op, tmp, special_prayer);
    free_object(tmp);
    return 1;
}

/**
 * Wizard wants to learn a regular spell.
 *
 * @param op
 * wizard.
 * @param params
 * spell name.
 * @retval 0
 * failure.
 * @retval 1
 * success.
 */
int command_learn_spell(object *op, char *params) {
    return command_learn_spell_or_prayer(op, params, 0);
}

/**
 * Wizard wants to learn a god-given spell.
 *
 * @param op
 * wizard.
 * @param params
 * spell name.
 * @retval 0
 * failure.
 * @retval 1
 * success.
 */
int command_learn_special_prayer(object *op, char *params)
{
    return command_learn_spell_or_prayer(op, params, 1);
}

/**
 * Wizard wishes to forget a spell.
 *
 * @param op
 * wizard.
 * @param params
 * spell name to forget.
 * @return
 * 0 if no spell was forgotten, 1 else.
 */
int command_forget_spell(object *op, char *params)
{
    object *spell;

    if (op->contr == NULL || params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Which spell do you want to forget?", NULL);
        return 0;
    }

    spell = lookup_spell_by_name(op, params);
    if (spell == NULL) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
			     "You do not know the spell %s.",
			     "You do not know the spell %s.",
			     params);
        return 0;
    }

    do_forget_spell(op, spell->name);
    return 1;
}

/**
 * Lists all plugins currently loaded with their IDs and full names.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 1.
 */
int command_listplugins(object *op, char *params)
{
    plugins_display_list(op);
    return 1;
}

/**
 * Loads the given plugin. The DM specifies the name of the library to load (no
 * pathname is needed). Do not ever attempt to load the same plugin more than
 * once at a time, or bad things could happen.
 *
 * @param op
 * DM loading a plugin.
 * @param params
 * should be the plugin's name, eg cfpython.so
 * @return
 * 1.
 */
int command_loadplugin(object *op, char *params) {
    char buf[MAX_BUF];

    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Load which plugin?", NULL);
        return 1;
    }

    strcpy(buf, LIBDIR);
    strcat(buf, "/plugins/");
    strcat(buf, params);
    LOG(llevDebug, "Requested plugin file is %s\n", buf);
    if (plugins_init_plugin(buf) == 0) {
        LOG(llevInfo, "DM %s loaded plugin %s\n", op->name, params);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
            "Plugin %s successfully loaded.",
            "Plugin %s successfully loaded.",
            params);
    }
    else
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Could not load plugin %s.",
            "Could not load plugin %s.",
            params);
    return 1;
}

/**
 * Unloads the given plugin. The DM specified the ID of the library to unload.
 * Note that some things may behave strangely if the correct plugins are not
 * loaded.
 *
 * @param op
 * DM unloading a plugin.
 * @param params
 * should be the plugin's internal name, eg Python
 * @return
 * 1.
 */
int command_unloadplugin(object *op, char *params)
{
    if (params == NULL) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Remove which plugin?", NULL);
        return 1;
    }

    if (plugins_remove_plugin(params) == 0) {
        LOG(llevInfo, "DM %s unloaded plugin %s\n", op->name, params);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
            "Plugin %s successfully removed.",
            "Plugin %s successfully removed.",
            params);
    }
    else
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
            "Could not remove plugin %s.",
            "Could not remove plugin %s.",
            params);
    return 1;
}

/**
 * A players wants to become DM and hide.
 * Let's see if that's authorized.
 * Make sure to not tell anything to anyone.
 *
 * @param op
 * wizard.
 * @param params
 * password.
 * @retval 0
 * failure.
 * @retval 1
 * success.
 */
int command_dmhide(object *op, char *params) {
    if (!do_wizard_dm(op, params, 1))
        return 0;

    do_wizard_hide(op, 1);

    return 1;
}

/**
 * Remove an item from the wizard's item stack.
 *
 * @param pl
 * wizard.
 */
void dm_stack_pop(player *pl) {
    if (!pl->stack_items || !pl->stack_position) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Empty stack!", NULL);
        return;
    }

    pl->stack_position--;
    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "Popped item from stack, %d left.",
			 "Popped item from stack, %d left.",
			 pl->stack_position);
}

/**
 * Get current stack top item for player.
 * Returns NULL if no stacked item.
 * If stacked item disappeared (freed), remove it.
 *
 * Ryo, august 2004
 *
 * @param pl
 * wizard.
 * @return
 * item on top of stack, or NULL if deleted/stack empty.
 */
object *dm_stack_peek(player *pl) {
    object* ob;

    if (!pl->stack_position) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Empty stack!", NULL);
        return NULL;
    }

    ob = find_object(pl->stack_items[pl->stack_position-1]);
    if (!ob) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Stacked item was removed!", NULL);
        dm_stack_pop(pl);
        return NULL;
    }

    return ob;
}

/**
 * Push specified item on player stack.
 * Inform player of position.
 * Initializes variables if needed.
 *
 * @param pl
 * wizard.
 * @param item
 * item to put on stack.
 */
void dm_stack_push(player *pl, tag_t item) {
    if (!pl->stack_items) {
        pl->stack_items = (tag_t *)malloc(sizeof(tag_t)*STACK_SIZE);
        memset(pl->stack_items, 0, sizeof(tag_t)*STACK_SIZE);
    }

    if (pl->stack_position == STACK_SIZE) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Item stack full!", NULL);
        return;
    }

    pl->stack_items[pl->stack_position] = item;
    draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "Item stacked as %d.",
			 "Item stacked as %d.",
			 pl->stack_position);
    pl->stack_position++;
}

/**
 * Checks 'params' for object code.
 *
 * Can be:
 *  - empty => get current object stack top for player
 *  - number => get item with that tag, stack it for future use
 *  - $number => get specified stack item
 *  - "me" => player himself
 *
 * At function exit, params points to first non-object char
 *
 * 'from', if not NULL, contains at exit:
 *  - ::STACK_FROM_NONE => object not found
 *  - ::STACK_FROM_TOP => top item stack, may be NULL if stack was empty
 *  - ::STACK_FROM_STACK => item from somewhere in the stack
 *  - ::STACK_FROM_NUMBER => item by number, pushed on stack
 *
 * Ryo, august 2004
 *
 * @param pl
 * wizard.
 * @param params
 * object specified.
 * @param from 
 * @return
 * pointed object, or NULL if nothing suitable was found.
 */
object *get_dm_object(player *pl, char **params, int *from) {
    int item_tag, item_position;
    object *ob;

    if (!pl)
        return NULL;

    if (!params || !*params || **params == '\0') {
        if (from)
            *from = STACK_FROM_TOP;
        /* No parameter => get stack item */
        return dm_stack_peek(pl);
    }

    /* Let's clean white spaces */
    while (**params == ' ')
        (*params)++;

    /* Next case: number => item tag */
    if (sscanf(*params, "%d", &item_tag)) {
        /* Move parameter to next item */
        while (isdigit(**params))
            (*params)++;

        /* And skip blanks, too */
        while (**params == ' ')
            (*params)++;

        /* Get item */
        ob = find_object(item_tag);
        if (!ob) {
            if (from)
                *from = STACK_FROM_NONE;
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "No such item %d!",
				 "No such item %d!",
				 item_tag);
            return NULL;
        }

        /* Got one, let's push it on stack */
        dm_stack_push(pl, item_tag);
        if (from)
            *from = STACK_FROM_NUMBER;
        return ob;
    }

    /* Next case: $number => stack item */
    if (sscanf(*params, "$%d", &item_position)) {
        /* Move parameter to next item */
        (*params)++;

        while (isdigit(**params))
            (*params)++;
        while (**params == ' ')
            (*params)++;

        if (item_position >= pl->stack_position) {
            if (from)
                *from = STACK_FROM_NONE;
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
				 "No such stack item %d!",
				 "No such stack item %d!",
				 item_position);
            return NULL;
        }

        ob = find_object(pl->stack_items[item_position]);
        if (!ob) {
            if (from)
                *from = STACK_FROM_NONE;
            draw_ext_info_format(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
				 "Stack item %d was removed.",
				 "Stack item %d was removed.",
				 item_position);
            return NULL;
        }

        if (from)
            *from = item_position < pl->stack_position-1 ? STACK_FROM_STACK : STACK_FROM_TOP;
        return ob;
    }

    /* Next case: 'me' => return pl->ob */
    if (!strncmp(*params, "me", 2)) {
        if (from)
            *from = STACK_FROM_NUMBER;
        dm_stack_push(pl, pl->ob->count);

        /* Skip to next token */
        (*params) += 2;
        while (**params == ' ')
            (*params)++;

        return pl->ob;
    }

    /* Last case: get stack top */
    if (from)
        *from = STACK_FROM_TOP;
    return dm_stack_peek(pl);
}

/**
 * Pop the stack top.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 0.
 */
int command_stack_pop(object *op, char *params) {
    dm_stack_pop(op->contr);
    return 0;
}

/**
 * Push specified item on stack.
 *
 * @param op
 * wizard.
 * @param params
 * object specifier.
 * @return
 * 0.
 */
int command_stack_push(object *op, char *params) {
    object *ob;
    int from;
    ob = get_dm_object(op->contr, &params, &from);

    if (ob && from != STACK_FROM_NUMBER)
        /* Object was from stack, need to push it again */
        dm_stack_push(op->contr, ob->count);

    return 0;
}

/**
 * Displays stack contents.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 0.
 */
int command_stack_list(object *op, char *params) {
    int item;
    object *display;
    player *pl = op->contr;

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		  "Item stack contents:", NULL);

    for (item = 0; item < pl->stack_position; item++) {
        display = find_object(pl->stack_items[item]);
        if (display)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
				 " %d : %s [%d]",
				 " %d : %s [%d]",
				 item, display->name, display->count);
        else
            /* Item was freed */
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
				 " %d : (lost item: %d)",
				 " %d : (lost item: %d)",
				 item, pl->stack_items[item]);
    }

    return 0;
}

/**
 * Empty DM item stack.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 0.
 */
int command_stack_clear(object *op, char *params) {
    op->contr->stack_position = 0;
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		  "Item stack cleared.", NULL);
    return 0;
}

/**
 * Get a diff of specified items.
 * Second item is compared to first, and differences displayed.
 *
 * @note
 * get_ob_diff() works the opposite way (first compared to 2nd),
 * but it's easier with stack functions to do it this way, so you can do:
 * - stack_push \<base\>
 * - stack_push \<object to be compared\>
 * - diff
 * - patch xxx <---- applies to object compared to base, easier :)
 *
 * Ryo, august 2004
 *
 * @param op
 * wizard.
 * @param params
 * object specifier.
 * @return
 * 0.
 */
int command_diff(object *op, char *params) {
    object *left, *right, *top;
    char diff[HUGE_BUF];
    int left_from, right_from;

    top = NULL;

    left = get_dm_object(op->contr, &params, &left_from);
    if (!left) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Compare to what item?", NULL);
        return 0;
    }

    if (left_from == STACK_FROM_NUMBER)
        /* Item was stacked, remove it else right will be the same... */
        dm_stack_pop(op->contr);

    right = get_dm_object(op->contr, &params, &right_from);

    if (!right) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Compare what item?", NULL);
        return 0;
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		  "Item difference:", NULL);

    if (left_from == STACK_FROM_TOP && right_from == STACK_FROM_TOP) {
        /*
         * Special case: both items were taken from stack top.
         * Override the behaviour, taking left as item just below top, if exists.
         * See function description for why.
         * Besides, if we don't do anything, compare an item to itself, not really useful.
         */
        if (op->contr->stack_position > 1) {
            left = find_object(op->contr->stack_items[op->contr->stack_position-2]);
            if (left)
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			      "(Note: first item taken from undertop)", NULL);
            else
                /* Stupid case: item under top was freed, fallback to stack top */
                left = right;
        }
    }

    get_ob_diff(left, right, diff, HUGE_BUF);

    if (diff[0]=='\0') {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		      "Objects are the same.", NULL);
        return 0;
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
		  diff, NULL);
    return 0;
}

/**
 * Puts an object into another.
 * @param op
 * wizard.
 * @param params
 * object specifier.
 * @return
 * 0.
 */
int command_insert_into(object* op, char *params)
{
    object *left, *right, *inserted;
    int left_from, right_from;
    char what[MAX_BUF], where[MAX_BUF];

    left = get_dm_object(op->contr, &params, &left_from);
    if (!left) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Insert into what object?", NULL);
        return 0;
    }

    if (left_from == STACK_FROM_NUMBER)
        /* Item was stacked, remove it else right will be the same... */
        dm_stack_pop(op->contr);

    right = get_dm_object(op->contr, &params, &right_from);

    if (!right) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Insert what item?", NULL);
        return 0;
    }

    if (left_from == STACK_FROM_TOP && right_from == STACK_FROM_TOP) {
        /*
        * Special case: both items were taken from stack top.
        * Override the behaviour, taking left as item just below top, if exists.
        * See function description for why.
        * Besides, can't insert an item into itself.
        */
        if (op->contr->stack_position > 1) {
            left = find_object(op->contr->stack_items[op->contr->stack_position-2]);
            if (left)
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			      "(Note: item to insert into taken from undertop)", NULL);
            else
                /* Stupid case: item under top was freed, fallback to stack top */
                left = right;
        }
    }

    if (left == right)
    {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Can't insert an object into itself!", NULL);
        return 0;
    }

    if (right->type == PLAYER)
    {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
		      "Can't insert a player into something!", NULL);
        return 0;
    }

    if (!QUERY_FLAG(right,FLAG_REMOVED))
        remove_ob(right);
    inserted = insert_ob_in_ob(right,left);
    if (left->type == PLAYER) {
        if (inserted != right)
            /* item was merged, so updating name and such. */
            esrv_update_item(UPD_WEIGHT|UPD_NAME|UPD_NROF,left,inserted);
    }
    query_name(inserted, what, MAX_BUF);
    query_name(left, where, MAX_BUF);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DM,
			 "Inserted %s in %s",
			 "Inserted %s in %s",
			 what,where);
    return 0;

}

/**
 * Displays information about styles loaded for random maps.
 *
 * @param op
 * wizard.
 * @param params
 * ignored.
 * @return
 * 0.
 */
int command_style_map_info(object *op, char *params)
{
    extern mapstruct *styles;
    mapstruct	*mp;
    int	    maps_used=0, mapmem=0, objects_used=0, x,y;
    object  *tmp;

    for (mp = styles; mp!=NULL; mp=mp->next) {
	maps_used++;
	mapmem += MAP_WIDTH(mp)*MAP_HEIGHT(mp)*(sizeof(object *)+sizeof(MapSpace)) + sizeof(mapstruct);
	for (x=0; x<MAP_WIDTH(mp); x++) {
	    for (y=0; y<MAP_HEIGHT(mp); y++) {
		for (tmp=get_map_ob(mp, x, y); tmp!=NULL; tmp=tmp->above)
		    objects_used++;
	    }
	}
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
		 "[fixed]Style maps loaded:    %d",
		 "Style maps loaded:    %d",
		 maps_used);
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
		  "[fixed]Memory used, not",
		  "Memory used, not");
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
		 "[fixed]including objects:    %d",
		 "including objects:    %d",
		 mapmem);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
		 "Style objects:        %d",
		 "Style objects:        %d",
		 objects_used);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
		 "Mem for objects:      %d",
		 "Mem for objects:      %d",
		 objects_used * sizeof(object));
    return 0;
}

/**
 * DM wants to follow a player, or stop following a player.
 *
 * @param op
 * wizard.
 * @param params
 * player to follow. If NULL, stop following player.
 * @return
 * 0.
 */
int command_follow(object* op, char* params) {
    player* other;
    if (!params) {
        if (op->contr->followed_player != NULL) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM, "You stop following %s.", NULL, op->contr->followed_player);
            FREE_AND_CLEAR_STR(op->contr->followed_player);
        }
        return 0;
    }

    other = find_player_partial_name(params);
    if (!other) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM, "No such player or ambiguous name.", NULL);
        return 0;
    }
    if (other == op->contr) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM, "You can't follow yourself.", NULL);
        return 0;
    }

    if (op->contr->followed_player)
        FREE_AND_CLEAR_STR(op->contr->followed_player);

    op->contr->followed_player = add_string(other->ob->name);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_DM, "Following %s.", NULL, op->contr->followed_player);
    return 0;
}

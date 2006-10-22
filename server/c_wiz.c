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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <treasure.h>
#include <skills.h>

/** Defines for DM item stack **/
#define STACK_SIZE         50   /* Stack size, static */
/* Values for 'from' field of get_dm_object */
#define STACK_FROM_NONE     0   /* Item was not found */
#define STACK_FROM_TOP      1   /* Item is stack top */
#define STACK_FROM_STACK    2   /* Item is somewhere in stack */
#define STACK_FROM_NUMBER   3   /* Item is a number (may be top) */


/**
 * Enough of the DM functions seem to need this that I broke
 * it out to a seperate function.  name is the person
 * being saught, op is who is looking for them.  This
 * prints diagnostics messages, and returns the
 * other player, or NULL otherwise.
 */
static player *get_other_player_from_name(object *op, char *name) {
    player *pl;

    if (!name)
        return NULL;

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (!strncmp(pl->ob->name, name, MAX_NAME))
            break;

    if (pl == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "No such player.");
        return NULL;
    }

    if (pl->ob == op) {
        new_draw_info(NDI_UNIQUE, 0, op, "You can't do that to yourself.");
        return NULL;
    }
    if (pl->state != ST_PLAYING) {
        new_draw_info(NDI_UNIQUE, 0, op, "That player is in no state for that right now.");
        return NULL;
    }
    return pl;
}

/**
 * This command will stress server.
 */
int command_loadtest(object *op, char *params) {
    uint32 x, y;
    char buf[1024];

    new_draw_info(NDI_UNIQUE, 0, op, "loadtest will stress server through teleporting");
    new_draw_info(NDI_UNIQUE, 0, op, "at different map places.");
    new_draw_info(NDI_UNIQUE, 0, op, "use at your own risks.");
    new_draw_info(NDI_UNIQUE, 0, op, "Very long loop used so server may have to be reset.");
    new_draw_info(NDI_UNIQUE, 0, op, "type loadtest TRUE to run");
    new_draw_info_format(NDI_UNIQUE, 0, op, "{%s}", params);
    if (!params)
        return 0;
    if (strncmp (params, "TRUE", 4))
        return 0;

    new_draw_info_format(NDI_UNIQUE, 0, op, "gogogo");
    for (x = 0; x < settings.worldmaptilesx; x++) {
        for (y = 0; y < settings.worldmaptilesy; y++) {
            sprintf(buf, "/world/world_%d_%d", x+settings.worldmapstartx, y+settings.worldmapstarty);
            command_goto(op, buf);
        }
    }

    return 0;
}

/**
 * Actually hides specified player (obviously a DM).
 * If 'silent_dm' is non zero, other players are informed of DM entering/leaving,
 * else they just think someone left/entered.
 */
void do_wizard_hide(object *op, int silent_dm) {
    if (op->contr->hidden) {
        op->contr->hidden = 0;
        op->invisible = 1;
        new_draw_info(NDI_UNIQUE, 0, op, "You are no longer hidden from other players");
        op->map->players++;
        new_draw_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
            "%s has entered the game.", op->name);
        if (!silent_dm) {
            new_draw_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL,
                "The Dungeon Master has arrived!");
        }
    } else {
        op->contr->hidden = 1;
        new_draw_info(NDI_UNIQUE, 0, op, "Other players will no longer see you.");
        op->map->players--;
        if (!silent_dm) {
            new_draw_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL,
                    "The Dungeon Master is gone..");
        }
        new_draw_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
            "%s leaves the game.", op->name);
        new_draw_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
            "%s left the game.", op->name);
    }
}

int command_hide(object *op, char *params)
{
    do_wizard_hide(op, 0);
    return 1;
}

/**
 * This finds and returns the object which matches the name or
 * object nubmer (specified via num #whatever).
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
 * Sets the god for some objects.  params should contain two values -
 * first the object to change, followed by the god to change it to.
 */
int command_setgod(object *op, char *params) {
    object *ob, *god;
    char *str;

    if (!params || !(str = strchr(params, ' '))) {
        new_draw_info(NDI_UNIQUE, 0, op, "Usage: setgod object god");
        return 0;
    }

    /* kill the space, and set string to the next param */
    *str++ = '\0';
    if (!(ob = find_object_both(params))) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "Set whose god - can not find object %s?", params);
        return 1;
    }

    /*
     * Perhaps this is overly restrictive?  Should we perhaps be able
     * to rebless altars and the like?
     */
    if (ob->type != PLAYER) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "%s is not a player - can not change its god", ob->name);
        return 1;
    }

    god = find_god(str);
    if (god==NULL) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "No such god %s.", str);
        return 1;
    }

    become_follower(ob, god);
    return 1;
}

/**
 * Add player's IP to ban_file and kick them off the server
 * I know most people have dynamic IPs but this is more of a short term
 * solution if they have to get a new IP to play maybe they'll calm down.
 * This uses the banish_file in the local directory *not* the ban_file
 * The action is logged with a ! for easy searching. -tm
 */
int command_banish(object *op, char *params) {
    player *pl;
    FILE *banishfile;
    char buf[MAX_BUF];
    time_t now;

    if (!params) {
        new_draw_info(NDI_UNIQUE, 0, op, "Usage: banish <player>.");
        return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    sprintf(buf, "%s/%s", settings.localdir, BANISHFILE);

    if ((banishfile = fopen(buf, "a")) == NULL) {
        LOG (llevDebug, "Could not find file banish_file.\n");
                new_draw_info(NDI_UNIQUE, 0, op, "Could not find banish_file.");
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
    new_draw_info_format(NDI_UNIQUE|NDI_RED, 0, op, "You banish %s", pl->ob->name);
    new_draw_info_format(NDI_UNIQUE|NDI_ALL|NDI_RED, 5, op,
                 "%s banishes %s from the land!", op->name, pl->ob->name);
    command_kick(op, pl->ob->name);
    return 1;
}

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
            new_draw_info_format(NDI_UNIQUE|NDI_ALL|NDI_RED, 5, op,
                             "%s is kicked out of the game.", op->name);
            strcpy(op->contr->killer, "left");
            check_score(op); /* Always check score */

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

int command_overlay_save(object *op, char *params) {
    if (!op)
        return 0;

    if (op != NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
        new_draw_info(NDI_UNIQUE, 0, op,
            "Sorry, you can't force an overlay save.");
        return 1;
    }

    new_save_map(op->map, 2);
    // No need to save again the map and reset it. OVerlay saving is non destructive.
    // new_save_map(op->map, 0);
    // This fixes bug #1553636 (Crashbug: reset/swaped map after use of "overlay_save")
    // Ryo 2006-10-22
    new_draw_info(NDI_UNIQUE, 0, op, "Current map has been saved as an"
        " overlay.");

    //ready_map_name(op->map->path, 0);

    return 1;
}

int command_overlay_reset(object *op, char* params) {
    char filename[MAX_BUF];
    struct stat stats;
    strcpy(filename, create_overlay_pathname(op->map->path));
    if (!stat(filename, &stats))
        if (!unlink(filename))
            new_draw_info(NDI_UNIQUE, 0, op, "Overlay successfully removed.");
        else
            new_draw_info(NDI_UNIQUE, 0, op, "Overlay couldn't be removed.");
    else
        new_draw_info(NDI_UNIQUE, 0, op, "No overlay for current map.");
    
    return 1;
}

/*
 * A simple toggle for the no_shout field.
 * AKA the MUZZLE command
 */
int command_toggle_shout(object *op, char *params) {
    player *pl;

    if (!params) {
         new_draw_info(NDI_UNIQUE, 0, op, "Usage: toggle_shout <player>.");
         return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    if (pl->ob->contr->no_shout == 0) {
        pl->ob->contr->no_shout = 1;

        new_draw_info(NDI_UNIQUE|NDI_RED, 0, pl->ob, "You have been muzzled by the DM!");
        new_draw_info_format(NDI_UNIQUE, 0, op, "You muzzle %s.", pl->ob->name);

        /* Avion : Here we handle the MUZZLE global event */
        execute_global_event(EVENT_MUZZLE, pl->ob, params);

        return 1;
    } else {
        pl->ob->contr->no_shout = 0;
        new_draw_info(NDI_UNIQUE|NDI_ORANGE, 0, pl->ob,
            "You are allowed to shout and chat again.");
        new_draw_info_format(NDI_UNIQUE, 0, op,
            "You remove %s's muzzle.", pl->ob->name);
        return 1;
    }
}

int command_shutdown(object *op, char *params) {
    if (op!=NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
        new_draw_info(NDI_UNIQUE, 0, op, "Sorry, you can't shutdown the server.");
        return 1;
    }

    /*
     * We need to give op - command_kick expects it.  however, this means
     * the op won't get kicked off, so we do it ourselves
     */
    command_kick(op, NULL);
    check_score(op); /* Always check score */
    (void)save_player(op, 0);
    play_again(op);
    cleanup();
    /* not reached */
    return 1;
}

int command_goto(object *op, char *params)
{
    char *name;
    object *dummy;

    if (!op)
        return 0;

    if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Go to what level?");
        return 1;
    }

    name = params;
    dummy=get_object();
    dummy->map = op->map;
    EXIT_PATH(dummy) = add_string (name);
    dummy->name = add_string(name);

    enter_exit(op, dummy);
    free_object(dummy);
    if (op->contr->loading == NULL) {
        new_draw_info_format(NDI_UNIQUE, 0, op,
            "Difficulty: %d.", op->map->difficulty);
    }

    return 1;
}

/* is this function called from somewhere ? -Tero */
int command_generate (object *op, char *params)
{
    object *tmp;
    int nr = 1, i, retry;

    if (!op)
        return 0;

    if (params != NULL)
        sscanf(params, "%d", &nr);
    for (i = 0; i < nr; i++) {
        retry = 50;
        while ((tmp=generate_treasure(0, op->map->difficulty)) == NULL && --retry)
            ;
        if (tmp != NULL) {
            tmp = insert_ob_in_ob(tmp, op);
            if (op->type == PLAYER)
                esrv_send_item(op, tmp);
        }
    }

    return 1;
}

int command_freeze(object *op, char *params) {
    int ticks;
    player *pl;

    if (!params) {
         new_draw_info(NDI_UNIQUE, 0, op, "Usage: freeze [ticks] <player>.");
         return 1;
    }

    ticks = atoi(params);
    if (ticks) {
        while ((isdigit(*params) || isspace(*params)) && *params != 0)
            params++;
        if (*params == 0) {
            new_draw_info(NDI_UNIQUE, 0, op, "Usage: freeze [ticks] <player>.");
            return 1;
        }
    } else
        ticks = 100;

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    new_draw_info(NDI_UNIQUE|NDI_RED, 0, pl->ob, "You have been frozen by the DM!");
    new_draw_info_format(NDI_UNIQUE, 0, op,
        "You freeze %s for %d ticks", pl->ob->name, ticks);
    pl->ob->speed_left = -(pl->ob->speed*ticks);
    return 0;
}

int command_arrest(object *op, char *params) {
    object *dummy;
    player *pl;
    if (!op) return 0;
    if(params==NULL) {
         new_draw_info(NDI_UNIQUE, 0,op,"Usage: arrest <player>.");
         return 1;
    }
    pl = get_other_player_from_name(op, params);
    if (!pl) return 1;
    dummy=get_jail_exit(pl->ob);
    if (!dummy) {
	/* we have nowhere to send the prisoner....*/
	new_draw_info(NDI_UNIQUE, 0,op,"can't jail player, there is no map to hold them");
	return 0;
    }
    enter_exit(pl->ob, dummy);
    free_object(dummy);
    new_draw_info(NDI_UNIQUE, 0,pl->ob,"You have been arrested.");
    new_draw_info(NDI_UNIQUE, 0,op,"OK.");
    LOG(llevInfo, "Player %s arrested by %s\n", pl->ob->name, op->name);
    return 1;
}

int command_summon(object *op, char *params) {
    int i;
    object *dummy;
    player *pl;

    if (!op)
        return 0;

    if (params == NULL) {
         new_draw_info(NDI_UNIQUE, 0, op, "Usage: summon <player>.");
         return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

    i = find_free_spot(op, op->map, op->x, op->y, 1, 9);
    if (i == -1) {
        new_draw_info(NDI_UNIQUE, 0, op, "Can not find a free spot to place summoned player.");
        return 1;
    }

    dummy = get_object();
    EXIT_PATH(dummy) = add_string(op->map->path);
    EXIT_X(dummy) = op->x+freearr_x[i];
    EXIT_Y(dummy) = op->y+freearr_y[i];
    enter_exit(pl->ob, dummy);
    free_object(dummy);
    new_draw_info(NDI_UNIQUE, 0, pl->ob, "You are summoned.");
    new_draw_info(NDI_UNIQUE, 0, op, "OK.");

    return 1;
}

/**
 * Teleport next to target player.
 */
/* mids 01/16/2002 */
int command_teleport(object *op, char *params) {
    int i;
    object *dummy;
    player *pl;

    if (!op)
        return 0;

    if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Usage: teleport <player>.");
        return 1;
    }

    pl = get_other_player_from_name(op, params);
    if (!pl)
        return 1;

   i = find_free_spot(pl->ob, pl->ob->map, pl->ob->x, pl->ob->y, 1, 9);
   if (i == -1) {
      new_draw_info(NDI_UNIQUE, 0, op, "Can not find a free spot to teleport to.");
      return 1;
   }

   dummy = get_object();
   EXIT_PATH(dummy) = add_string(pl->ob->map->path);
   EXIT_X(dummy) = pl->ob->x + freearr_x[i];
   EXIT_Y(dummy) = pl->ob->y + freearr_y[i];
   enter_exit(op, dummy);
   free_object(dummy);
   if (!op->contr->hidden)
      new_draw_info(NDI_UNIQUE, 0, pl->ob, "You see a portal open.");
   new_draw_info(NDI_UNIQUE, 0, op, "OK.");
   return 1;
}

/**
 * This function is a real mess, because we're stucking getting
 * the entire item description in one block of text, so we just
 * can't simply parse it - we need to look for double quotes
 * for example.  This could actually get much simpler with just a
 * little help from the client - if we could get line breaks, it
 * makes parsing much easier, eg, something like:
 * arch dragon
 * name big nasty creature
 * hp 5
 * sp 30
 * Is much easier to parse than
 * dragon name "big nasty creature" hp 5 sp 30
 * for example.
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
        new_draw_info(NDI_UNIQUE, 0, op,
            "Usage: create [nr] [magic] <archetype> [ of <artifact>]"
            " [variable_to_patch setting]");
        return 1;
    }
    bp = params;

    /* We need to know where the line ends */
    endline = bp+strlen(bp);

    if (sscanf(bp, "%d ", &nrof)) {
        if ((bp = strchr(params, ' ')) == NULL) {
            new_draw_info(NDI_UNIQUE, 0, op,
                "Usage: create [nr] [magic] <archetype> [ of <artifact>]"
            " [variable_to_patch setting]");
            return 1;
        }
        bp++;
        set_nrof = 1;
        LOG(llevDebug, "%s creates: (%d) %s\n", op->name, nrof, bp);
    }
    if (sscanf(bp, "%d ", &magic)) {
        if ((bp = strchr(bp, ' ')) == NULL) {
            new_draw_info(NDI_UNIQUE, 0, op,
                "Usage: create [nr] [magic] <archetype> [ of <artifact>]"
            " [variable_to_patch setting]");
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
        new_draw_info(NDI_UNIQUE, 0, op, "No such archetype.");
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
                new_draw_info_format(NDI_UNIQUE, 0, op,
                     "No artifact list for type %d\n", at->clone.type);
            } else {
                art = find_artifactlist(at->clone.type)->items;

                do {
                    if (!strcmp(art->item->name, cp))
                        break;
                    art = art->next;
                } while (art != NULL);
                if (!art) {
                    new_draw_info_format(NDI_UNIQUE, 0, op,
                        "No such artifact ([%d] of %s)", at->clone.type, cp);
                }
            }
            LOG(llevDebug, "%s creates: (%d) (%d) (%s) of (%s)\n", op->name,
                set_nrof ? nrof : 0, set_magic ? magic : 0, bp, cp);
        }
    } /* if cp */

    if ((at->clone.type == ROD || at->clone.type == WAND || at->clone.type == SCROLL ||
        at->clone.type == HORN || at->clone.type == SPELLBOOK) && !at_spell) {
        new_draw_info_format(NDI_UNIQUE, 0, op,
            "Unable to find spell %s for object that needs it, or it is of wrong type",
            cp);
        return 1;
    }

    /*
     * Rather than have two different blocks with a lot of similar code,
     * just create one object, do all the processing, and then determine
     * if that one object should be inserted or if we need to make copies.
     */
    tmp = arch_to_object(at);
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
            new_draw_info_format(NDI_UNIQUE, 0, op,
                 "Malformed create line: %s", bp2);
            break;
        }
        /* bp2 should still point to the start of this line,
         * with bp3 pointing to the end
         */
        if (set_variable(tmp, bp2) == -1)
            new_draw_info_format(NDI_UNIQUE, 0, op,
                 "Unknown variable %s", bp2);
        else
            new_draw_info_format(NDI_UNIQUE, 0, op,
                 "(%s#%d)->%s", tmp->name, tmp->count, bp2);
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

        tmp = insert_ob_in_ob(tmp, op);
        esrv_send_item(op, tmp);

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
                        new_draw_info(NDI_UNIQUE, 0, op, "Object too big to insert in map, or wrong position.");
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
            esrv_send_item(op, head);
        }

        /* free the one we used to copy */
        free_object(tmp);
    }

    return 1;
}

/*
 * Now follows dm-commands which are also acceptable from sockets
 */

int command_inventory(object *op, char *params) {
    object *tmp;
    int i;

    if (!params) {
        inventory(op, NULL);
        return 0;
    }

    if (!sscanf(params, "%d", &i) || (tmp = find_object(i)) == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Inventory of what object (nr)?");
        return 1;
    }

    inventory(op, tmp);
    return 1;
}

/* just show player's their skills for now. Dm's can
 * already see skills w/ inventory command - b.t.
 */

int command_skills (object *op, char *params) {
    show_skills(op, params);
    return 0;
}

int command_dump (object *op, char *params) {
    object *tmp;

    tmp = get_dm_object(op->contr, &params, NULL);
    if (!tmp)
        return 1;

    dump_object(tmp);
    new_draw_info(NDI_UNIQUE, 0, op, errmsg);
    if (QUERY_FLAG(tmp, FLAG_OBJ_ORIGINAL))
        new_draw_info(NDI_UNIQUE, 0, op, "Object is marked original");
    return 1;
}

/**
 *  When DM is possessing a monster, flip aggression on and off, to allow
 * better motion.
 */
int command_mon_aggr(object *op, char *params) {
    if (op->enemy || !QUERY_FLAG(op, FLAG_UNAGGRESSIVE)) {
        op->enemy = NULL;
        SET_FLAG(op, FLAG_UNAGGRESSIVE);
        new_draw_info(NDI_UNIQUE, 0, op, "Aggression turned OFF");
    } else {
        CLEAR_FLAG(op, FLAG_FRIENDLY);
        CLEAR_FLAG(op, FLAG_UNAGGRESSIVE);
        new_draw_info(NDI_UNIQUE, 0, op, "Aggression turned ON");
    }

    return 1;
}

/** DM can possess a monster.  Basically, this tricks the client into thinking
 * a given monster, is actually the player it controls.  This allows a DM
 * to inhabit a monster's body, and run around the game with it.
 * This function is severely broken - it has tons of hardcoded values,
 */
int command_possess(object *op, char *params) {
    object *victim, *curinv, *nextinv;
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
        new_draw_info(NDI_UNIQUE, 0, op, "Patch what object (nr)?");
        return 1;
    }

    if (victim == op) {
        new_draw_info(NDI_UNIQUE, 0, op, "As insane as you are, I cannot "
                      "allow you to possess yourself.");
        return 1;
    }

    /* clear out the old inventory */
    curinv = op->inv;
    while (curinv != NULL) {
        nextinv = curinv->below;
        esrv_del_item(op->contr, curinv->count);
        curinv = nextinv;
    }

    /* make the switch */
    pl = op->contr;
    victim->contr = pl;
    pl->ob = victim;
    victim->type = PLAYER;
    SET_FLAG(victim, FLAG_WIZ);

    /* send the inventory to the client */
    curinv = victim->inv;
    while (curinv != NULL) {
        nextinv = curinv->below;
        esrv_send_item(victim, curinv);
        curinv = nextinv;
    }
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

    esrv_new_player(pl, 80); /* just pick a wieght, we don't care */
    esrv_send_inventory(victim, victim);

    fix_player(victim);

    do_some_living(victim);
    return 1;
}

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
      new_draw_info(NDI_UNIQUE, 0, op, "Patch what values?");
      return 1;
    }

    if ((arg2 = strchr(arg, ' ')))
        arg2++;
    if (settings.real_wiz == FALSE)
        SET_FLAG(tmp, FLAG_WAS_WIZ); /* To avoid cheating */
    if (set_variable(tmp, arg) == -1)
        new_draw_info_format(NDI_UNIQUE, 0, op, "Unknown variable %s", arg);
    else {
        new_draw_info_format(NDI_UNIQUE, 0, op,
            "(%s#%d)->%s=%s", tmp->name, tmp->count, arg, arg2);
    }

    return 1;
  }

int command_remove (object *op, char *params) {
    object *tmp;
    int from;

    tmp = get_dm_object(op->contr, &params, &from);
    if (!tmp) {
        new_draw_info(NDI_UNIQUE, 0, op, "Remove what object (nr)?");
        return 1;
    }

    if (tmp->type == PLAYER) {
        new_draw_info(NDI_UNIQUE, 0, op, "Unable to remove a player!");
        return 1;
    }

    if (QUERY_FLAG(tmp, FLAG_REMOVED)) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "%s is already removed!",
            query_name(tmp));
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
    if (!tmp->invisible && tmp->env && tmp->env->type == PLAYER)
        esrv_del_item(tmp->env->contr, tmp->count);
    remove_ob(tmp);
    return 1;
}

int command_free(object *op, char *params) {
    object *tmp;
    int from;

    tmp = get_dm_object(op->contr, &params, &from);

    if (!tmp) {
        new_draw_info(NDI_UNIQUE, 0, op, "Free what object (nr)?");
        return 1;
    }

    if (from != STACK_FROM_STACK)
        /* Item is either stack top, or is a number thus is now stack top, let's remove it  */
        dm_stack_pop(op->contr);

    if (tmp->head)
        tmp = tmp->head;

    if (!QUERY_FLAG(tmp, FLAG_REMOVED)) {
        new_draw_info(NDI_UNIQUE, 0, op, "Warning, item wasn't removed.");
        remove_ob(tmp);
    }

    free_object(tmp);
    return 1;
}

/**
 * This adds exp to a player.  We now allow adding to a specific skill.
 */
int command_addexp(object *op, char *params) {
    char buf[MAX_BUF], skill[MAX_BUF];
    int i, q;
    object *skillob = NULL;
    player *pl;

    skill[0] = '\0';
    if ((params == NULL) || (strlen(params) > MAX_BUF) || ((q = sscanf(params, "%s %d %s", buf, &i, skill)) < 2)) {
        new_draw_info(NDI_UNIQUE, 0, op, "Usage: addexp player quantity [skill].");
        return 1;
    }

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (!strncmp(pl->ob->name, buf, MAX_NAME))
            break;

    if (pl == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "No such player.");
        return 1;
    }

    if (q >= 3) {
        skillob = find_skill_by_name(pl->ob, skill);
        if (!skillob) {
            new_draw_info_format(NDI_UNIQUE, 0, op, "Unable to find skill %s in %s", skill, buf);
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

int command_speed(object *op, char *params) {
    int i;

    if (params == NULL || !sscanf(params, "%d", &i)) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "Current speed is %d", max_time);
        return 1;
    }

    set_max_time(i);
    reset_sleep();
    new_draw_info(NDI_UNIQUE, 0, op, "The speed is changed.");
    return 1;
}

/**************************************************************************/
/* Mods made by Tyler Van Gorder, May 10-13, 1992.                        */
/* CSUChico : tvangod@cscihp.ecst.csuchico.edu                            */
/**************************************************************************/

int command_stats(object *op, char *params) {
    char thing[20];
    player *pl;
    char buf[MAX_BUF];

    thing[0] = '\0';
    if (params == NULL || !sscanf(params, "%s", thing) || thing == NULL) {
       new_draw_info(NDI_UNIQUE, 0, op, "Who?");
       return 1;
    }

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (!strcmp(pl->ob->name, thing)) {
            sprintf(buf, "Str : %-2d      H.P. : %-4d  MAX : %d",
                pl->ob->stats.Str, pl->ob->stats.hp, pl->ob->stats.maxhp);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            sprintf(buf, "Dex : %-2d      S.P. : %-4d  MAX : %d",
                pl->ob->stats.Dex, pl->ob->stats.sp, pl->ob->stats.maxsp);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            sprintf(buf, "Con : %-2d        AC : %-4d  WC  : %d",
                pl->ob->stats.Con, pl->ob->stats.ac, pl->ob->stats.wc) ;
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            sprintf(buf, "Int : %-2d    Damage : %d",
                pl->ob->stats.Int, pl->ob->stats.dam);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            sprintf(buf, "Wis : %-2d       EXP : %" FMT64,
                pl->ob->stats.Wis, pl->ob->stats.exp);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            sprintf(buf, "Pow : %-2d    Grace : %d",
                pl->ob->stats.Pow, pl->ob->stats.grace);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            sprintf(buf, "Cha : %-2d      Food : %d",
                pl->ob->stats.Cha, pl->ob->stats.food);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            break;
        }
    if (pl == NULL)
        new_draw_info(NDI_UNIQUE, 0, op, "No such player.");
    return 1;
}

int command_abil(object *op, char *params) {
    char thing[20], thing2[20];
    int iii;
    player *pl;
    char buf[MAX_BUF];

    iii = 0;
    thing[0] = '\0';
    thing2[0] = '\0';
    if (params == NULL || !sscanf(params, "%s %s %d", thing, thing2, &iii) ||
        thing==NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Who?");
        return 1;
    }

    if (thing2 == NULL){
        new_draw_info(NDI_UNIQUE, 0, op, "You can't change that.");
        return 1;
    }

    if (iii < MIN_STAT || iii > MAX_STAT) {
        new_draw_info(NDI_UNIQUE, 0, op, "Illegal range of stat.\n");
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
            sprintf(buf, "%s has been altered.", pl->ob->name);
            new_draw_info(NDI_UNIQUE, 0, op, buf);
            fix_player(pl->ob);
            return 1;
        }
    }

    new_draw_info(NDI_UNIQUE, 0, op, "No such player.");
    return 1;
}

int command_reset (object *op, char *params) {
    mapstruct *m;
    object *dummy = NULL, *tmp = NULL;

    if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Reset what map [name]?");
        return 1;
    }

    if (strcmp(params, ".") == 0)
        params = op->map->path;
    m = has_been_loaded(params);
    if (m == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "No such map.");
        return 1;
    }

    /* Forbid using reset on our own map when we're in a transport, as
     * it has the displeasant effect of crashing the server.
     * - gros, July 25th 2006 */
    if ((op->contr && op->contr->transport)&&(op->map == m))
    {
        new_draw_info(NDI_UNIQUE, 0, op, "You need to disembark first.");
        return 1;
    }

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
                new_draw_info(NDI_UNIQUE, 0, op,
                    "You cannot reset a random map when inside it.");
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
        swap_map(m);
    }

    if (m->in_memory == MAP_SWAPPED) {
        LOG(llevDebug, "Resetting map %s.\n", m->path);

        /* setting this effectively causes an immediate reload */
        m->reset_time = 1;
        flush_old_maps();
        new_draw_info(NDI_UNIQUE, 0, op, "OK.");
        if (tmp) {
            enter_exit(tmp, dummy);
            free_object(dummy);
        }
        return 1;
    } else {
        player *pl;
        int playercount = 0;

        /* Need to re-insert player if swap failed for some reason */
        if (tmp) {
            insert_ob_in_map(op, m, NULL, 0);
            free_object(dummy);
        }

        new_draw_info(NDI_UNIQUE, 0, op,
            "Reset failed, couldn't swap map, the following players are on it:");
        for (pl = first_player; pl != NULL; pl = pl->next) {
            if (pl->ob->map == m && pl->ob != op) {
                new_draw_info_format(NDI_UNIQUE, 0, op, "%s", pl->ob->name);
                playercount++;
            }
        }
        if (!playercount)
            new_draw_info(NDI_UNIQUE, 0, op,
                "hmm, I don't see any other players on this map, something else is the problem.");
        return 1;
    }
}

int command_nowiz(object *op, char *params) { /* 'noadm' is alias */
    CLEAR_FLAG(op, FLAG_WIZ);
    CLEAR_FLAG(op, FLAG_WIZPASS);
    CLEAR_FLAG(op, FLAG_WIZCAST);

    if (settings.real_wiz == TRUE)
        CLEAR_FLAG(op, FLAG_WAS_WIZ);
    if (op->contr->hidden) {
        new_draw_info(NDI_UNIQUE, 0, op, "You are no longer hidden from other players");
        op->map->players++;
        new_draw_info_format(NDI_UNIQUE|NDI_ALL|NDI_DK_ORANGE, 5, NULL,
            "%s has entered the game.", op->name);
        op->contr->hidden = 0;
        op->invisible = 1;
    } else
        new_draw_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL, "The Dungeon Master is gone..");
    return 1;
}

/**
 * object *op is trying to become dm.
 * pl_name is name supplied by player.  Restrictive DM will make it harder
 * for socket users to become DM - in that case, it will check for the players
 * character name.
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

int do_wizard_dm(object *op, char *params, int silent) {
    if (!op->contr)
        return 0;

    if (QUERY_FLAG(op, FLAG_WIZ)) {
        new_draw_info(NDI_UNIQUE, 0, op, "You are already the Dungeon Master!");
        return 0;
    }

    if (checkdm(op, op->name,
            (params ? params : "*"), op->contr->socket.host)) {
        SET_FLAG(op, FLAG_WIZ);
        SET_FLAG(op, FLAG_WAS_WIZ);
        SET_FLAG(op, FLAG_WIZPASS);
        SET_FLAG(op, FLAG_WIZCAST);
        new_draw_info(NDI_UNIQUE, 0, op, "Ok, you are the Dungeon Master!");
        /*
         * Remove setting flying here - that won't work, because next
         * fix_player() is called that will get cleared - proper solution
         * is probably something like a wiz_force which gives that and any
         * other desired abilities.
         */
        clear_los(op);
        op->contr->write_buf[0] ='\0';

        if (!silent)
            new_draw_info(NDI_UNIQUE|NDI_ALL|NDI_LT_GREEN, 1, NULL,
                "The Dungeon Master has arrived!");

        return 1;
    } else {
        new_draw_info(NDI_UNIQUE, 0, op, "Sorry Pal, I don't think so.");
        op->contr->write_buf[0] ='\0';
        return 0;
    }
}

/*
 * Actual command to perhaps become dm.  Changed aroun a bit in version 0.92.2
 * - allow people on sockets to become dm, and allow better dm file
 */
int command_dm(object *op, char *params) {
    if (!op->contr)
        return 0;

    do_wizard_dm(op, params, 0);

    return 1;
}

int command_invisible(object *op, char *params) {
    if (op) {
        op->invisible += 100;
        update_object(op, UP_OBJ_FACE);
        new_draw_info(NDI_UNIQUE, 0, op, "You turn invisible.");
    }

    return 0;
}

/**
 * Returns spell object (from archetypes) by name.
 * Returns NULL if 0 or more than one spell matches.
 * Used for wizard's learn spell/prayer.
 *
 * op is the player issuing the command.
 *
 * Ignores archetypes "spelldirect_xxx" since these archetypes are not used
 * anymore (but may still be present in some player's inventories and thus
 * cannot be removed). We have to ignore them here since they have the same
 * name than other "spell_xxx" archetypes and would always conflict.
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
                new_draw_info_format(NDI_UNIQUE, 0, op, "More than one archetype matches the spell name %s:", spell_name);
                new_draw_info_format(NDI_UNIQUE, 0, op, "- %s", found->name);
            }
            new_draw_info_format(NDI_UNIQUE, 0, op, "- %s", ar->name);
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
                new_draw_info_format(NDI_UNIQUE, 0, op, "More than one spell matches %s:", spell_name);
                new_draw_info_format(NDI_UNIQUE, 0, op, "- %s", found->clone.name);
            }
            new_draw_info_format(NDI_UNIQUE, 0, op, "- %s", ar->clone.name);
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
    new_draw_info_format(NDI_UNIQUE, 0, op, "The spell %s does not exist.", spell_name);
    return NULL;
}

static int command_learn_spell_or_prayer(object *op, char *params,
                                         int special_prayer) {
    object *tmp;

    if (op->contr == NULL || params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Which spell do you want to learn?");
        return 0;
    }

    tmp = get_spell_by_name(op, params);
    if (tmp == NULL) {
        return 0;
    }

    if (check_spell_known(op, tmp->name)) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "You already know the spell %s.", tmp->name);
        return 0;
    }

    do_learn_spell(op, tmp, special_prayer);
    free_object(tmp);
    return 1;
}

int command_learn_spell(object *op, char *params) {
    return command_learn_spell_or_prayer(op, params, 0);
}

int command_learn_special_prayer(object *op, char *params)
{
    return command_learn_spell_or_prayer(op, params, 1);
}

int command_forget_spell(object *op, char *params)
{
    object *spell;

    if (op->contr == NULL || params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Which spell do you want to forget?");
        return 0;
    }

    spell = lookup_spell_by_name(op, params);
    if (spell == NULL) {
        new_draw_info_format(NDI_UNIQUE, 0, op, "You do not know the spell %s.", params);
        return 0;
    }

    do_forget_spell(op, spell->name);
    return 1;
}

/**
 * Lists all plugins currently loaded with their IDs and full names.
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
 */
int command_loadplugin(object *op, char *params) {
    char buf[MAX_BUF];

    if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Load which plugin?");
        return 1;
    }

    strcpy(buf, LIBDIR);
    strcat(buf, "/plugins/");
    strcat(buf, params);
    LOG(llevDebug, "Requested plugin file is %s\n", buf);
    if (plugins_init_plugin(buf) == 0)
        new_draw_info(NDI_UNIQUE, 0, op, "Plugin successfully loaded.");
    else
        new_draw_info(NDI_UNIQUE, 0, op, "Could not load plugin.");
    return 1;
}

/**
 * Unloads the given plugin. The DM specified the ID of the library to unload.
 * Note that some things may behave strangely if the correct plugins are not
 * loaded.
 */
int command_unloadplugin(object *op, char *params)
{
    if (params == NULL) {
        new_draw_info(NDI_UNIQUE, 0, op, "Remove which plugin?");
        return 1;
    }

    if (plugins_remove_plugin(params) == 0)
        new_draw_info(NDI_UNIQUE, 0, op, "Plugin successfully removed.");
    else
        new_draw_info(NDI_UNIQUE, 0, op, "Could not remove plugin.");
    return 1;
}

/**
 * A players wants to become DM and hide.
 * Let's see if that's authorized.
 * Make sure to not tell anything to anyone.
 */
int command_dmhide(object *op, char *params) {
    if (!do_wizard_dm(op, params, 1))
        return 0;

    do_wizard_hide(op, 1);

    return 1;
}

void dm_stack_pop(player *pl) {
    if (!pl->stack_items || !pl->stack_position) {
        new_draw_info(NDI_UNIQUE, 0, pl->ob, "Empty stack!");
        return;
    }

    pl->stack_position--;
    new_draw_info_format(NDI_UNIQUE, 0, pl->ob, "Popped item from stack, %d left.", pl->stack_position);
}

/**
 * Get current stack top item for player.
 * Returns NULL if no stacked item.
 * If stacked item disappeared (freed), remove it.
 *
 * Ryo, august 2004
 */
object *dm_stack_peek(player *pl) {
    object* ob;

    if (!pl->stack_position) {
        new_draw_info(NDI_UNIQUE, 0, pl->ob, "Empty stack!");
        return NULL;
    }

    ob = find_object(pl->stack_items[pl->stack_position-1]);
    if (!ob) {
        new_draw_info(NDI_UNIQUE, 0, pl->ob, "Stacked item was removed!");
        dm_stack_pop(pl);
        return NULL;
    }

    return ob;
}

/**
 * Push specified item on player stack.
 * Inform player of position.
 * Initializes variables if needed.
 */
void dm_stack_push(player *pl, tag_t item) {
    if (!pl->stack_items) {
        pl->stack_items = (tag_t *)malloc(sizeof(tag_t)*STACK_SIZE);
        memset(pl->stack_items, 0, sizeof(tag_t)*STACK_SIZE);
    }

    if (pl->stack_position == STACK_SIZE) {
        new_draw_info(NDI_UNIQUE, 0, pl->ob, "Item stack full!");
        return;
    }

    pl->stack_items[pl->stack_position] = item;
    new_draw_info_format(NDI_UNIQUE, 0, pl->ob, "Item stacked as %d.", pl->stack_position);
    pl->stack_position++;
}

/**
 * Checks 'params' for object code.
 *
 * Can be:
 *  * empty => get current object stack top for player
 *  * number => get item with that tag, stack it for future use
 *  * $number => get specified stack item
 *  * "me" => player himself
 *
 * At function exit, params points to first non-object char
 *
 * 'from', if not NULL, contains at exit:
 *  * STACK_FROM_NONE => object not found
 *  * STACK_FROM_TOP => top item stack, may be NULL if stack was empty
 *  * STACK_FROM_STACK => item from somewhere in the stack
 *  * STACK_FROM_NUMBER => item by number, pushed on stack
 *
 * Ryo, august 2004
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
            new_draw_info_format(NDI_UNIQUE, 0, pl->ob, "No such item %d!", item_tag);
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
            new_draw_info_format(NDI_UNIQUE, 0, pl->ob, "No such stack item %d!", item_position);
            return NULL;
        }

        ob = find_object(pl->stack_items[item_position]);
        if (!ob) {
            if (from)
                *from = STACK_FROM_NONE;
            new_draw_info_format(NDI_UNIQUE, 0, pl->ob, "Stack item %d was removed.", item_position);
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
 */
int command_stack_pop(object *op, char *params) {
    dm_stack_pop(op->contr);
    return 0;
}

/**
 * Push specified item on stack.
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
 */
int command_stack_list(object *op, char *params) {
    int item;
    object *display;
    player *pl = op->contr;

    new_draw_info(NDI_UNIQUE, 0, op, "Item stack contents:");

    for (item = 0; item < pl->stack_position; item++) {
        display = find_object(pl->stack_items[item]);
        if (display)
            new_draw_info_format(NDI_UNIQUE, 0, op, " %d : %s [%d]", item, display->name, display->count);
        else
            /* Item was freed */
            new_draw_info_format(NDI_UNIQUE, 0, op, " %d : (lost item: %d)", item, pl->stack_items[item]);
    }

    return 0;
}

/**
 * Empty DM item stack.
 */
int command_stack_clear(object *op, char *params) {
    op->contr->stack_position = 0;
    new_draw_info(NDI_UNIQUE, 0, op, "Item stack cleared.");
    return 0;
}

/**
 * Get a diff of specified items.
 * Second item is compared to first, and differences displayed.
 * Note: get_ob_diff works the opposite way (first compared to 2nd),
 * but it's easier with stack functions to do it this way, so you can do:
 *  * stack_push <base>
 *  * stack_push <object to be compared>
 *  * diff
 *  * patch xxx <---- applies to object compared to base, easier :)
 *
 * Ryo, august 2004
 */
int command_diff(object *op, char *params) {
    object *left, *right, *top;
    const char *diff;
    int left_from, right_from;

    top = NULL;

    left = get_dm_object(op->contr, &params, &left_from);
    if (!left) {
        new_draw_info(NDI_UNIQUE, 0, op, "Compare to what item?");
        return 0;
    }

    if (left_from == STACK_FROM_NUMBER)
        /* Item was stacked, remove it else right will be the same... */
        dm_stack_pop(op->contr);

    right = get_dm_object(op->contr, &params, &right_from);

    if (!right) {
        new_draw_info(NDI_UNIQUE, 0, op, "Compare what item?");
        return 0;
    }

    new_draw_info(NDI_UNIQUE, 0, op, "Item difference:");

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
                new_draw_info(NDI_UNIQUE, 0, op, "(Note: first item taken from undertop)");
            else
                /* Stupid case: item under top was freed, fallback to stack top */
                left = right;
        }
    }

    diff = get_ob_diff(left, right);

    if (!diff) {
        new_draw_info(NDI_UNIQUE, 0, op, "Objects are the same.");
        return 0;
    }

    new_draw_info(NDI_UNIQUE, 0, op, diff);
    return 0;
}

int command_insert_into(object* op, char *params)
{
    object *left, *right, *inserted;
    int left_from, right_from;

    left = get_dm_object(op->contr, &params, &left_from);
    if (!left) {
        new_draw_info(NDI_UNIQUE, 0, op, "Insert into what object?");
        return 0;
    }

    if (left_from == STACK_FROM_NUMBER)
        /* Item was stacked, remove it else right will be the same... */
        dm_stack_pop(op->contr);

    right = get_dm_object(op->contr, &params, &right_from);

    if (!right) {
        new_draw_info(NDI_UNIQUE, 0, op, "Insert what item?");
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
                new_draw_info(NDI_UNIQUE, 0, op, "(Note: item to insert into taken from undertop)");
            else
                /* Stupid case: item under top was freed, fallback to stack top */
                left = right;
        }
    }

    if (left == right)
    {
        new_draw_info(NDI_UNIQUE, 0, op, "Can't insert an object into itself!");
        return 0;
    }

    if (right->type == PLAYER)
    {
        new_draw_info(NDI_UNIQUE, 0, op, "Can't insert a player into something!");
        return 0;
    }

    if (!QUERY_FLAG(right,FLAG_REMOVED))
        remove_ob(right);
    inserted = insert_ob_in_ob(right,left);
    if (left->type == PLAYER) {
        if (inserted == right)
            esrv_send_item(left,right);
        else
            esrv_update_item(UPD_WEIGHT|UPD_NAME|UPD_NROF,left,inserted);
    }
    new_draw_info_format(NDI_UNIQUE, 0, op, "Inserted %s in %s", query_name(inserted),query_name(left));
    return 0;

}

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


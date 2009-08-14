/*
 * static char *rcsid_sock_info_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002,2006 Mark Wedel & Crossfire Development Team
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
 * \file
 * Basic client output functions.
 *
 * \date 2003-12-02
 *
 * This file implements some of the simpler output functions to the
 * client.  Basically, things like sending text strings along
 */

#include <global.h>
#include <sproto.h>
#include <stdarg.h>
#include <spells.h>
#include <skills.h>

/**
 * Draws a normal message on the client.  It is pretty
 * much the same thing as the draw_info above, but takes a color
 * parameter.  the esrv_drawinfo functions should probably be
 * replaced with this, just using black as the color.
 */
static void esrv_print_msg(socket_struct *ns, int color, const char *str) {
    SockList sl;

    SockList_Init(&sl);
    SockList_AddPrintf(&sl, "drawinfo %d %s", color, str);
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * Draws   an extended message on the client.
 * ns      the socket to send message to
 * color   color informations (used mainly if client does not support message type)
 * type,
 * subtype type and subtype of text message
 * intro   Intro message to send with main message if client does not support the message type
 * message The main message
 */
static void esrv_print_ext_msg(socket_struct *ns, int color, uint8 type, uint8 subtype, const char *message) {
    SockList sl;

    SockList_Init(&sl);
    SockList_AddPrintf(&sl, "drawextinfo %d %hhu %hhu %s", color, type, subtype, message);
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * Frontend for esrv_print_msg
 * \param colr message color
 * \param pl player to send to. Can be NULL
 * \param tmp message to send. Can be NULL
 *
 * If pl is NULL or without contr set, writes message to log.
 *
 * Else sends message to player via esrv_print_msg
 */

static void print_message(int colr, const object *pl, const char *tmp) {
    if (tmp == NULL) {
        tmp = "[NULL]";
    }

    if (!pl || (pl->type == PLAYER && pl->contr == NULL)) {
        fprintf(logfile, "%s\n", tmp);
        return;
    }
    if (pl->type == PLAYER) {
        esrv_print_msg(&pl->contr->socket, colr, tmp);
        return;
    }
}

/**
 * Sends message to player(s).
 *
 * @param flags Various flags - mostly color, plus a few specials.
 *
 * @param pri Priority.  It is a little odd - the lower the value, the more
 * important it is.  Thus, 0 gets sent no matter what.  Otherwise, the
 * value must be less than the listening level that the player has set.
 * Unfortunately, there is no clear guideline on what each level does what.
 *
 * @param pl Can be passed as NULL - in fact, this will be done if NDI_ALL is
 * set in the flags.
 *
 * @param type The type MSG_TYPE for the type of message.
 *
 * @param subtype The type MSG_TYPE for the type of message.
 *
 * @param message The message to send for clients that support draw_ext_info.
 *
 * @param oldmessage is for clients that do not support it.  oldmessage can be
 * NULL, in which case this function will strip out the tags of message.
 */
void draw_ext_info(
    int flags, int pri, const object *pl, uint8 type,
    uint8 subtype, const char *message, const char *oldmessage) {
    if ((flags&NDI_ALL) || (flags&NDI_ALL_DMS)) {
        player *tmppl;

        for (tmppl = first_player; tmppl != NULL; tmppl = tmppl->next) {
            if ((flags&NDI_ALL_DMS) && !QUERY_FLAG(tmppl->ob, FLAG_WIZ))
                continue;
            draw_ext_info((flags&~NDI_ALL&~NDI_ALL_DMS), pri, tmppl->ob, type, subtype, message, oldmessage);
        }

        return;
    }

    if (!pl || (pl->type == PLAYER && pl->contr == NULL)) {
        /* Write to the socket? */
        print_message(0, NULL, oldmessage);
        return;
    }
    if (pl->type != PLAYER)
        return;
    if (pri >= pl->contr->listening)
        return;

    /* If the client doesn't support the readables, need to convert
     * it to old format.  If oldmessage is specified, it is presumed
     * that no conversion is needed (if the caller isn't sure, it
     * should pass NULL for oldmessage).
     */
    if (!CLIENT_SUPPORT_READABLES(&pl->contr->socket, type)) {
        char *buf;

        if (oldmessage) {
            buf = (char *)oldmessage;
        } else {
            buf = strdup_local(message);
            if (buf == NULL) {
                LOG(llevError, "info::draw_ext_info -> Out of memory!\n");
                return;
            }
            strip_media_tag(buf);
        }
        print_message(flags&NDI_COLOR_MASK, pl, buf);
        if (!oldmessage)
            free(buf);
    } else {
        esrv_print_ext_msg(&pl->contr->socket, flags&NDI_COLOR_MASK, type, subtype, message);
    }
}

/**
 * Sends message to player(s).
 *
 *  This function is the same as draw_ext_info, but takes varargs format.
 *  Otherwise, the meaning of all the fields is the same.  This is perhaps not
 *  the most efficient as we do vsnprintf on both the old and newbuf, but it
 *  simplifies the code greatly since we can just call draw_ext_info.  Also,
 *  hopefully at some point, need for old_format will go away.
 *
 * @param flags Various flags - mostly color, plus a few specials.
 *
 * @param pri Priority.  It is a little odd - the lower the value, the more
 * important it is.  Thus, 0 gets sent no matter what.  Otherwise, the
 * value must be less than the listening level that the player has set.
 * Unfortunately, there is no clear guideline on what each level does what.
 *
 * @param pl Can be passed as NULL - in fact, this will be done if NDI_ALL is
 * set in the flags.
 *
 * @param type The type MSG_TYPE for the type of message.
 *
 * @param subtype The type MSG_TYPE for the type of message.
 *
 * @param new_format
 *
 * @param old_format
 */
void draw_ext_info_format(int flags, int pri, const object *pl, uint8 type, uint8 subtype, const char *new_format, const char *old_format, ...) {
    char newbuf[HUGE_BUF], oldbuf[HUGE_BUF];
    va_list ap;

    if (!old_format)
        old_format = new_format;

    va_start(ap, old_format);
    vsnprintf(oldbuf, HUGE_BUF, old_format, ap);
    va_end(ap);
    va_start(ap, old_format);
    vsnprintf(newbuf, HUGE_BUF, new_format, ap);
    va_end(ap);

    draw_ext_info(flags, pri, pl, type, subtype, newbuf, oldbuf);
}

/**
 * Writes to everyone on the specified map
 */
void ext_info_map(int color, const mapstruct *map, uint8 type, uint8 subtype, const char *str1, const char *str2) {
    player *pl;

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (pl->ob != NULL && pl->ob->map == map) {
            draw_ext_info(color, 0, pl->ob, type, subtype, str1, str2);
        }
}

/**
 * Writes to everyone on the map *except *op.  This is useful for emotions.
 */
void ext_info_map_except(int color, const mapstruct *map, const object *op, uint8 type, uint8 subtype, const char *str1, const char *str2) {
    player *pl;

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (pl->ob != NULL && pl->ob->map == map && pl->ob != op) {
            draw_ext_info(color, 0, pl->ob, type, subtype, str1, str2);
        }
}

/**
 * Writes to everyone on the map except op1 and op2
 */
void ext_info_map_except2(int color, const mapstruct *map, const object *op1, const object *op2, int type, int subtype, const char *str1, const char *str2) {
    player *pl;

    for (pl = first_player; pl != NULL; pl = pl->next)
        if (pl->ob != NULL && pl->ob->map == map
                && pl->ob != op1 && pl->ob != op2) {
            draw_ext_info(color, 0, pl->ob, type, subtype, str1, str2);
        }
}

/**
 * Get player's current range attack in obuf.
 */
void rangetostring(const object *pl, char *obuf, size_t len) {
    char name[MAX_BUF];

    switch (pl->contr->shoottype) {
    case range_none:
        strncpy(obuf, "Range: nothing", len);
        break;

    case range_bow: {
            object *op;

            for (op = pl->inv; op; op = op->below)
                if (op->type == BOW && QUERY_FLAG(op, FLAG_APPLIED))
                    break;
            if (op == NULL)
                break;

            query_base_name(op, 0, name, MAX_BUF);
            snprintf(obuf, len, "Range: %s (%s)", name, op->race ? op->race : "nothing");
        }
        break;

    case range_magic:
        if (settings.casting_time == TRUE) {
            if (pl->casting_time > -1) {
                if (pl->casting_time == 0)
                    snprintf(obuf, len, "Range: Holding spell (%s)", pl->spell->name);
                else
                    snprintf(obuf, len, "Range: Casting spell (%s)", pl->spell->name);
            } else
                snprintf(obuf, len, "Range: spell (%s)", pl->contr->ranges[range_magic]->name);
        } else
            snprintf(obuf, len, "Range: spell (%s)", pl->contr->ranges[range_magic]->name);
        break;

    case range_misc:
        if (pl->contr->ranges[range_misc])
            query_base_name(pl->contr->ranges[range_misc], 0, name, MAX_BUF);
        else
            strncpy(name, "none", MAX_BUF);
        snprintf(obuf, len, "Range: %s", name);
        break;

        /* range_scroll is only used for controlling golems.  If the
         * the player does not have a golem, reset some things.
         */
    case range_golem:
        if (pl->contr->ranges[range_golem] != NULL)
            snprintf(obuf, len, "Range: golem (%s)", pl->contr->ranges[range_golem]->name);
        else {
            pl->contr->shoottype = range_none;
            strncpy(obuf, "Range: nothing", len);
        }
        break;

    case range_skill:
        snprintf(obuf, len, "Skill: %s", pl->chosen_skill != NULL ? pl->chosen_skill->name : "none");
        break;

    case range_builder:
        query_base_name(pl->contr->ranges[range_builder], 0, name, MAX_BUF);
        snprintf(obuf, len, "Builder: %s", name);
        break;

    default:
        strncpy(obuf, "Range: illegal", len);
    }
}

/**
 * Sets player title.
 */
void set_title(const object *pl, char *buf, size_t len) {
    char *p;

    snprintf(buf, len, "Player: %s ", pl->name);
    p = strchr(buf, '\0');
    player_get_title(pl->contr, p, (buf+len)-p);
}

/**
 * Helper for magic map creation.
 *
 * Takes a player, the map_mark array and an x and y starting position.
 * pl is the player.
 * px, py are offsets from the player.
 *
 * This function examines all the adjacant spaces next to px, py.
 * It updates the map_mark arrow with the color and high bits set
 * for various code values.
 */
static void magic_mapping_mark_recursive(object *pl, char *map_mark, int px, int py) {
    int x, y, dx, dy, mflags, l;
    sint16 nx, ny;
    mapstruct *mp;
    New_Face *f;
    object *ob;

    for (dx = -1; dx <= 1; dx++) {
        for (dy = -1; dy <= 1; dy++) {
            x = px+dx;
            y = py+dy;

            if (FABS(x) >= MAGIC_MAP_HALF || FABS(y) >= MAGIC_MAP_HALF)
                continue;

            mp = pl->map;
            nx = pl->x+x;
            ny = pl->y+y;

            mflags = get_map_flags(pl->map, &mp, nx, ny, &nx, &ny);
            if (mflags&P_OUT_OF_MAP)
                continue;

            if (map_mark[MAGIC_MAP_HALF+x+MAGIC_MAP_SIZE*(MAGIC_MAP_HALF+y)] == 0) {
                for (l = 0; l < MAP_LAYERS; l++) {
                    ob = GET_MAP_FACE_OBJ(mp, nx, ny, l);
                    if (ob && !ob->invisible && ob->face != blank_face)
                        break;
                }
                if (ob)
                    f = ob->face;
                else
                    f = blank_face;

                /* Should probably have P_NO_MAGIC here also, but then shops don't
                 * work.
                 */
                if (mflags&P_BLOCKSVIEW)
                    map_mark[MAGIC_MAP_HALF+x+MAGIC_MAP_SIZE*(MAGIC_MAP_HALF+y)] = FACE_WALL|(f ? f->magicmap : 0);
                else {
                    map_mark[MAGIC_MAP_HALF+x+MAGIC_MAP_SIZE*(MAGIC_MAP_HALF+y)] = FACE_FLOOR|(f ? f->magicmap : 0);
                    magic_mapping_mark_recursive(pl, map_mark, x, y);
                }
            }
        }
    }
}

/**
 * Creates magic map for player.
 *
 * Note:  For improved magic mapping display, the space that blocks
 * the view is now marked with value 2.  Any dependencies of map_mark
 * being nonzero have been changed to check for 1.  Also, since
 * map_mark is a char value, putting 2 in should cause no problems.
 *
 * This function examines the map the player is on, and determines what
 * is visible.  2 is set for walls or objects that blocks view.  1
 * is for open spaces.  map_mark should already have been initialized
 * to zero before this is called.
 * strength is an initial strength*2 rectangular area that we automatically
 * see in/penetrate through.
 */
void magic_mapping_mark(object *pl, char *map_mark, int strength) {
    int x, y, mflags, l;
    sint16 nx, ny;
    mapstruct *mp;
    New_Face *f;
    object *ob;

    for (x = -strength; x < strength; x++) {
        for (y = -strength; y < strength; y++) {
            mp = pl->map;
            nx = pl->x+x;
            ny = pl->y+y;
            mflags = get_map_flags(pl->map, &mp, nx, ny, &nx, &ny);
            if (mflags&P_OUT_OF_MAP)
                continue;
            else {
                for (l = 0; l < MAP_LAYERS; l++) {
                    ob = GET_MAP_FACE_OBJ(mp, nx, ny, l);
                    if (ob && !ob->invisible && ob->face != blank_face)
                        break;
                }
                if (ob)
                    f = ob->face;
                else
                    f = blank_face;
            }

            if (mflags&P_BLOCKSVIEW)
                map_mark[MAGIC_MAP_HALF+x+MAGIC_MAP_SIZE*(MAGIC_MAP_HALF+y)] = FACE_WALL|(f ? f->magicmap : 0);
            else {
                map_mark[MAGIC_MAP_HALF+x+MAGIC_MAP_SIZE*(MAGIC_MAP_HALF+y)] = FACE_FLOOR|(f ? f->magicmap : 0);
                magic_mapping_mark_recursive(pl, map_mark, x, y);
            }
        }
    }
}

/**
 * Creates and sends magic map to player.
 *
 * The following function is a lot messier than it really should be,
 * but there is no real easy solution.
 *
 * Mark Wedel
 */
void draw_magic_map(object *pl) {
    int x, y;
    char map_mark[MAGIC_MAP_SIZE*MAGIC_MAP_SIZE];
    int xmin, xmax, ymin, ymax;
    /* This is to prevent stack smashing below. */
    int max_width;
    SockList sl;

    if (pl->type != PLAYER) {
        LOG(llevError, "Non player object called draw_map.\n");
        return;
    }

    /* First, we figure out what spaces are 'reachable' by the player */
    memset(map_mark, 0, MAGIC_MAP_SIZE*MAGIC_MAP_SIZE);
    magic_mapping_mark(pl, map_mark, 3);

    /* We now go through and figure out what spaces have been
     * marked, and thus figure out rectangular region we send
     * to the client (eg, if only a 10x10 area is visible, we only
     * want to send those 100 spaces.)
     */
    xmin = MAGIC_MAP_SIZE;
    ymin = MAGIC_MAP_SIZE;
    xmax = 0;
    ymax = 0;
    max_width = (MAP_WIDTH(pl->map) > MAGIC_MAP_SIZE) ? MAGIC_MAP_SIZE : MAP_WIDTH(pl->map);
    for (x = 0; x < MAGIC_MAP_SIZE; x++) {
        for (y = 0; y < MAGIC_MAP_SIZE; y++) {
            if (map_mark[x+max_width*y]&~FACE_FLOOR) {
                xmin = MIN(x, xmin);
                xmax = MAX(x, xmax);
                ymin = MIN(y, ymin);
                ymax = MAX(y, ymax);
            }
        }
    }

    SockList_Init(&sl);
    SockList_AddPrintf(&sl, "magicmap %d %d %d %d ", (xmax-xmin+1), (ymax-ymin+1), MAGIC_MAP_HALF-xmin, MAGIC_MAP_HALF-ymin);

    for (y = ymin; y <= ymax; y++) {
        for (x = xmin; x <= xmax; x++) {
            SockList_AddChar(&sl, map_mark[x+MAGIC_MAP_SIZE*y]&~FACE_FLOOR);
        } /* x loop */
    } /* y loop */

    Send_With_Handling(&pl->contr->socket, &sl);
    SockList_Term(&sl);
}

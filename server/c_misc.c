/*
 * static char *rcsid_c_misc_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2010 Mark Wedel & Crossfire Development Team
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
 * Various functions. Handles misc. input request - things like hash table, malloc, maps,
 * who, etc.
 */

#define WANT_UNARMED_SKILLS

#include <global.h>
#include <loader.h>

#undef SS_STATISTICS
#include <shstr.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <assert.h>
#include <sounds.h>

/**
 * This is the 'maps' command.
 *
 * @param op
 * player requesting the information.
 * @param search
 * optional substring to search for.
 */
void map_info(object *op, char *search) {
    mapstruct *m;
    char map_path[MAX_BUF];
    long sec = seconds();

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_000),
                         (sec%86400)/3600, (sec%3600)/60, sec%60);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                  i18n_translate(get_language(op), I18N_MSG_CMISC_001));

    for (m = first_map; m != NULL; m = m->next) {
        if (*search != '\0' && strstr(m->path, search) == NULL)
            continue;   /* Skip unwanted maps */

        /* Print out the last 18 characters of the map name... */
        if (strlen(m->path) <= 18)
            strcpy(map_path, m->path);
        else
            strcpy(map_path, m->path+strlen(m->path)-18);

        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_003),
                             map_path, m->players, players_on_map(m, FALSE),
                             m->in_memory, m->timeout, m->difficulty,
                             (MAP_WHEN_RESET(m)%86400)/3600, (MAP_WHEN_RESET(m)%3600)/60,
                             MAP_WHEN_RESET(m)%60);
    }
}

/**
 * This is the 'language' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * optional language code ("en", "fr", etc.)
 */
int command_language(object *op, char *params) {
    const char *language_str;
    int language = -1;
    int i;

    if (!op->contr)
        return 0;

    language_str = language_names[get_language(op)];

    if (*params == '\0' || (!strcmp(params, ""))) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_005),
                             language_str);
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_051),
                             language_str);
        for (i = 0; i < NUM_LANGUAGES; i++) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                 "[fixed]%s: %s",
                                 language_codes[i],
                                 language_names[i]);
        }
        return 0;
    }

    for (i = 0; i < NUM_LANGUAGES; i++) {
        if (!strcmp(language_codes[i], params)) {
            language = i;
            i = NUM_LANGUAGES;
        }
    }
    /* Error out if unknown language. */
    if (language == -1) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Unknown language");
        return 0;
    }
    op->contr->language = language;
    language_str = language_names[language];

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         i18n_translate(language, I18N_MSG_CMISC_006),
                         language_str);
    return 0;
}

/**
 * This command dumps the body information for object *op.
 * it doesn't care what the params are.
 *
 * This is mostly meant as a debug command.
 *
 * This is the 'body' command.
 *
 * @param op
 * player to display body info for.
 * @param params
 * unused
 * @retval
 * 1.
 */
int command_body(object *op, char *params) {
    int i;

    /* Too hard to try and make a header that lines everything up, so just
     * give a description.
     */
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                  i18n_translate(get_language(op), I18N_MSG_CMISC_007));

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                  i18n_translate(get_language(op), I18N_MSG_CMISC_008));

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                  i18n_translate(get_language(op), I18N_MSG_CMISC_009));

    for (i = 0; i < NUM_BODY_LOCATIONS; i++) {
        /* really debugging - normally body_used should not be set to anything
         * if body_info isn't also set.
         */
        if (op->body_info[i] || op->body_used[i]) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                                 i18n_translate(get_language(op), I18N_MSG_CMISC_010),
                                 body_locations[i].use_name, op->body_info[i], op->body_used[i]);
        }
    }
    if (!QUERY_FLAG(op, FLAG_USE_ARMOUR))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_012));
    if (!QUERY_FLAG(op, FLAG_USE_WEAPON))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_013));

    return 1;
}

/**
 * Display the message of the day.
 *
 * @param op
 * player requesting the motd.
 * @param params
 * unused.
 * @retval
 * 1.
 */
int command_motd(object *op, char *params) {
    display_motd(op);
    return 1;
}

/**
 * Display the server rules.
 *
 * @param op
 * player requesting the rules.
 * @param params
 * unused.
 * @retval
 * 1.
 */
int command_rules(object *op, char *params) {
    send_rules(op);
    return 1;
}

/**
 * Display the server news.
 *
 * @param op
 * player requesting the news.
 * @param params
 * unused.
 * @retval
 * 1.
 */
int command_news(object *op, char *params) {
    send_news(op);
    return 1;
}

/**
 * Sends various memory-related statistics.
 *
 * @param op
 * player requesting the information.
 */
void malloc_info(object *op) {
    int ob_used = object_count_used(), ob_free = object_count_free(), players, nrofmaps;
    int nrm = 0, mapmem = 0, anr, anims, sum_alloc = 0, sum_used = 0, i, tlnr, alnr;
    treasurelist *tl;
    player *pl;
    mapstruct *m;
    archetype *at;
    artifactlist *al;

    for (tl = first_treasurelist, tlnr = 0; tl != NULL; tl = tl->next, tlnr++)
        ;
    for (al = first_artifactlist, alnr = 0; al != NULL; al = al->next, alnr++)
        ;

    for (at = first_archetype, anr = 0, anims = 0; at != NULL; at = at->more == NULL ? at->next : at->more, anr++)
        ;

    for (i = 1; i < num_animations; i++)
        anims += animations[i].num_animations;

    for (pl = first_player, players = 0; pl != NULL; pl = pl->next, players++)
        ;

    for (m = first_map, nrofmaps = 0; m != NULL; m = m->next, nrofmaps++)
        if (m->in_memory == MAP_IN_MEMORY) {
            mapmem += MAP_WIDTH(m)*MAP_HEIGHT(m)*(sizeof(object *)+sizeof(MapSpace));
            nrm++;
        }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_014),
                         sizeof(object), sizeof(player), sizeof(mapstruct));

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_015),
                         ob_used, i = (ob_used*sizeof(object)));

    sum_used += i;
    sum_alloc += i;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_017),
                         ob_free, i = (ob_free*sizeof(object)));

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_019),
                         object_count_active(), 0);

    sum_alloc += i;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_021),
                         players, i = (players*sizeof(player)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_023),
                         nrofmaps, i = (nrofmaps*sizeof(mapstruct)));

    sum_alloc += i;
    sum_used += nrm*sizeof(mapstruct);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_025),
                         nrm, mapmem);

    sum_alloc += mapmem;
    sum_used += mapmem;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_027),
                         anr, i = (anr*sizeof(archetype)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_029),
                         anims, i = (anims*sizeof(Fontindex)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_031),
                         tlnr, i = (tlnr*sizeof(treasurelist)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_033),
                         nroftreasures, i = (nroftreasures*sizeof(treasure)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_035),
                         nrofartifacts, i = (nrofartifacts*sizeof(artifact)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_037),
                         nrofallowedstr, i = (nrofallowedstr*sizeof(linked_char)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_039),
                         alnr, i = (alnr*sizeof(artifactlist)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_041),
                         sum_alloc);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_043),
                         sum_used);
}

/**
 * 'whereami' command.
 *
 * Pretty much identical to current map_info(), but on a bigger scale
 *
 * This function returns the name of the players current region, and
 * a description of it. It is there merely for flavour text.
 *
 * @param op
 * player wanting information.
 */
void current_region_info(object *op) {
    /*
     * Ok I /suppose/ I should write a seperate function for this, but it isn't
     * going to be /that/ slow, and won't get called much
     */
    region *r = get_region_by_name(get_name_of_region_for_map(op->map));

    /* This should only be possible if regions are not operating on this server. */
    if (!r)
        return;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_045),
                         get_region_longname(r), get_region_msg(r));
}

/**
 * 'mapinfo' command.
 *
 * @param op
 * player requesting the information.
 */
void current_map_info(object *op) {
    mapstruct *m = op->map;

    if (!m)
        return;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         "%s (%s) in %s",
                         m->name, m->path, get_region_longname(get_region_by_map(m)));

    if (QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_046),
                             m->players, m->difficulty,
                             MAP_WIDTH(m), MAP_HEIGHT(m),
                             MAP_ENTER_X(m), MAP_ENTER_Y(m),
                             MAP_TIMEOUT(m));
    }
    if (m->msg)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, m->msg);
}

#ifdef DEBUG_MALLOC_LEVEL
/**
 * Checks the server heap.
 *
 * @param op
 * player checking.
 * @param parms
 * ignored.
 * @retval 1
 */
int command_malloc_verify(object *op, char *parms) {
    extern int malloc_verify(void);

    if (!malloc_verify())
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_047));
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_048));

    return 1;
}
#endif

/**
 * 'whereabouts' command.
 *
 * Displays how many players are in which regions.
 *
 * @param op
 * player requesting information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_whereabouts(object *op, char *params) {
    region *reg;
    player *pl;

    /*
     * reset the counter on the region, then use it to store the number of
     * players there.
     * I don't know how thread-safe this would be, I suspect not very....
     */
    for (reg = first_region; reg != NULL; reg = reg->next) {
        reg->counter = 0;
    }
    for (pl = first_player; pl != NULL; pl = pl->next)
        if (pl->ob->map != NULL)
            get_region_by_map(pl->ob->map)->counter++;

    /* we only want to print out by places with a 'longname' field...*/
    for (reg = first_region; reg != NULL; reg = reg->next) {
        if (reg->longname == NULL && reg->counter > 0) {
            if (reg->parent != NULL) {
                reg->parent->counter += reg->counter;
                reg->counter = 0;
            } else /*uh oh, we shouldn't be here. */
                LOG(llevError, "command_whereabouts() Region %s with no longname has no parent\n", reg->name);
        }
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                  i18n_translate(get_language(op), I18N_MSG_CMISC_049));

    for (reg = first_region; reg != NULL; reg = reg->next)
        if (reg->counter > 0) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                 i18n_translate(get_language(op), I18N_MSG_CMISC_050),
                                 reg->counter, get_region_longname(reg));
        }
    return 1;
}

/** Utility structure for the 'who' command. */
typedef struct {
    char namebuf[MAX_BUF];
    int login_order;
} chars_names;

/**
 * Local function for qsort comparison.
 *
 * @param c1
 * @param c2
 * players to compare.
 * @return
 * -1, 0 or 1 depending on c1 and c2's order.
 */
static int name_cmp(const chars_names *c1, const chars_names *c2) {
    return strcasecmp(c1->namebuf, c2->namebuf);
}

/**
 * Displays the players in a region or party. If both are NULL, all players are listed.
 *
 * @param op
 * who is asking for player list.
 * @param reg
 * region to display players of.
 * @param party
 * party to list.
 */
void list_players(object *op, region *reg, partylist *party) {
    player *pl;
    uint16 i;
    char *format;
    int num_players = 0, num_wiz = 0, num_afk = 0, num_bot = 0;
    chars_names *chars = NULL;

    if (op == NULL || QUERY_FLAG(op, FLAG_WIZ))
        format = settings.who_wiz_format;
    else
        format = settings.who_format;

    for (pl = first_player; pl != NULL; pl = pl->next) {
        if (pl->ob->map == NULL)
            continue;
        if (pl->hidden && !QUERY_FLAG(op, FLAG_WIZ))
            continue;

        if (reg && !region_is_child_of_region(get_region_by_map(pl->ob->map), reg))
            continue;
        if (party && pl->party != party)
            continue;

        if (pl->state == ST_PLAYING || pl->state == ST_GET_PARTY_PASSWORD) {
            num_players++;
            chars = (chars_names *)realloc(chars, num_players*sizeof(chars_names));
            if (chars == NULL) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                              i18n_translate(get_language(op), I18N_MSG_CMISC_052));
                return;
            }
            sprintf(chars[num_players-1].namebuf, "%s", pl->ob->name);
            chars[num_players-1].login_order = num_players;

            /* Check for WIZ's & AFK's*/
            if (QUERY_FLAG(pl->ob, FLAG_WIZ))
                num_wiz++;

            if (QUERY_FLAG(pl->ob, FLAG_AFK))
                num_afk++;

            if (pl->socket.is_bot)
                num_bot++;
        }
    }
    if (first_player != (player *)NULL) {
        if (reg == NULL && party == NULL)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                                 i18n_translate(get_language(op), I18N_MSG_CMISC_053),
                                 num_players, num_wiz, num_afk, num_bot);
        else if (party == NULL)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                                 i18n_translate(get_language(op), I18N_MSG_CMISC_054),
                                 reg->longname ? reg->longname : reg->name, num_players, num_wiz, num_afk, num_bot);
        else
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                                 i18n_translate(get_language(op), I18N_MSG_CMISC_055),
                                 party->partyname, num_players, num_wiz, num_afk, num_bot);
    }
    qsort(chars, num_players, sizeof(chars_names), (int (*)(const void *, const void *))name_cmp);
    for (i = 0; i < num_players; i++)
        display_who_entry(op, find_player(chars[i].namebuf), format);
    free(chars);
}

/**
 * 'who' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * optional region to limit the information to.
 * @return
 * 1.
 */
int command_who(object *op, char *params) {
    region *reg;

    reg = get_region_from_string(params);
    list_players(op, reg, NULL);

    return 1;
}

/**
 * Display a line of 'who' to op, about pl, using the formatting specified by format.
 *
 * @param op
 * player getting the information.
 * @param pl
 * player to display information for.
 * @param format
 * format to display.
 */
void display_who_entry(object *op, player *pl, const char *format) {
    char tmpbuf[MAX_BUF];
    char outbuf[MAX_BUF];
    size_t i;

    strcpy(outbuf, "[fixed]");

    if (pl == NULL) {
        LOG(llevError, "display_who_entry(): I was passed a null player\n");
        return;
    }
    for (i = 0; i <= strlen(format); i++) {
        if (format[i] == '%') {
            i++;
            get_who_escape_code_value(tmpbuf, sizeof(tmpbuf), format[i], pl);
            strcat(outbuf, tmpbuf);
        } else if (format[i] == '_') {
            strcat(outbuf, " "); /* allow '_' to be used in place of spaces */
        } else {
            snprintf(tmpbuf, sizeof(tmpbuf), "%c", format[i]);
            strcat(outbuf, tmpbuf);
        }
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO, outbuf);
}

/**
 * Returns the value of the escape code used in the who format specifier.
 *
 * Specifier values are:
 * - N  Name of character
 * - t  title of character
 * - c  count
 * - n  newline
 * - h  \<Hostile\> if character is hostile, nothing otherwise
 * - d  \<WIZ\> if character is a dm, nothing otherwise
 * - a  \<AFK\> if character is afk, nothing otherwise
 * - b  \<BOT\> if character is a bot, nothing otherwise
 * - l  the level of the character
 * - m  the map path the character is currently on
 * - M  the map name of the map the character is currently on
 * - r  the region name (eg scorn, wolfsburg)
 * - R  the regional title (eg The Kingdom of Scorn, The Port of Wolfsburg)
 * - i  player's ip address
 * - %  a literal %
 * - _  a literal underscore
 *
 * @param[out] return_val
 * buffer that will contain the information.
 * @param size
 * length of return_val.
 * @param letter
 * format specifier.
 * @param pl
 * player to get information for.
 */
void get_who_escape_code_value(char *return_val, int size, const char letter, player *pl) {
    switch (letter) {
    case 'N':
        snprintf(return_val, size, "%s", pl->ob->name);
        break;

    case 't':
        player_get_title(pl, return_val, size);
        break;

    case 'c':
        snprintf(return_val, size, "%u", pl->ob->count);
        break;

    case 'n':
        snprintf(return_val, size, "\n");
        break;

    case 'h':
        snprintf(return_val, size, "%s", pl->peaceful ? "" : " <Hostile>");
        break;

    case 'l':
        snprintf(return_val, size, "%d", pl->ob->level);
        break;

    case 'd':
        snprintf(return_val, size, "%s", (QUERY_FLAG(pl->ob, FLAG_WIZ) ? " <WIZ>" : ""));
        break;

    case 'a':
        snprintf(return_val, size, "%s", (QUERY_FLAG(pl->ob, FLAG_AFK) ? " <AFK>" : ""));
        break;

    case 'b':
        snprintf(return_val, size, "%s", (pl->socket.is_bot == 1) ? " <BOT>" : "");
        break;

    case 'm':
        snprintf(return_val, size, "%s", pl->ob->map->path);
        break;

    case 'M':
        snprintf(return_val, size, "%s", pl->ob->map->name ? pl->ob->map->name : "Untitled");
        break;

    case 'r':
        snprintf(return_val, size, "%s", get_name_of_region_for_map(pl->ob->map));
            break;

    case 'R':
        snprintf(return_val, size, "%s", get_region_longname(get_region_by_map(pl->ob->map)));
        break;

    case 'i':
        snprintf(return_val, size, "%s", pl->socket.host);
        break;

    case '%':
        snprintf(return_val, size, "%%");
        break;

    case '_':
        snprintf(return_val, size, "_");
        break;
    }
}

/**
 * Toggles the afk status of a player. 'afk' command.
 *
 * @param op
 * player to toggle status for.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_afk(object *op, char *params) {
    if QUERY_FLAG(op, FLAG_AFK) {
        CLEAR_FLAG(op, FLAG_AFK);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_056));
    } else {
        SET_FLAG(op, FLAG_AFK);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_057));
    }
    return 1;
}

/**
 * Display memory information.
 *
 * @param op
 * player requesting information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_malloc(object *op, char *params) {
    malloc_info(op);
    return 1;
}

/**
 * 'mapinfo' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_mapinfo(object *op, char *params) {
    current_map_info(op);
    return 1;
}

/**
 * 'whereami' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_whereami(object *op, char *params) {
    current_region_info(op);
    return 1;
}

/**
 * 'maps' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * region to restrict to.
 */
int command_maps(object *op, char *params) {
    map_info(op, params);
    return 1;
}

/**
 * Various string-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_strings(object *op, char *params) {
    char stats[HUGE_BUF];

    ss_dump_statistics(stats, sizeof(stats));
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_LAST,
                         "[fixed]%s\n",
                         stats);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_LAST,
                  ss_dump_table(SS_DUMP_TOTALS, stats, sizeof(stats)));
    return 1;
}

/**
 * Players asks for the time.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_time(object *op, char *params) {
    time_info(op);
    return 1;
}

/**
 * Archetype-related statistics. Wizard 'archs' command.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_archs(object *op, char *params) {
    arch_info(op);
    return 1;
}

/**
 * Player is asking for the hiscore.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_hiscore(object *op, char *params) {
    hiscore_display(op, op == NULL ? 9999 : 50, params);
    return 1;
}

/**
 * Player wants to see/change the debug level.
 *
 * @param op
 * player asking for information.
 * @param params
 * new debug value.
 * @return
 * 1.
 */
int command_debug(object *op, char *params) {
    int i;

    if (*params == '\0' || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_092),
                             settings.debug);
        return 1;
    }
    if (op != NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_093));
        return 1;
    }
    settings.debug = (enum LogLevel)FABS(i);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_094),
                         i);
    return 1;
}


/**
 * Player wants to dump object below her.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_dumpbelow(object *op, char *params) {
    if (op && op->below) {
        StringBuffer *sb;
        char *diff;

        sb = stringbuffer_new();
        object_dump(op->below, sb);
        diff = stringbuffer_finish(sb);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, diff);
        free(diff);

        /* Let's push that item on the dm's stack */
        dm_stack_push(op->contr, op->below->count);
    }
    return 0;
}

/**
 * Wizard toggling wall-crossing.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_wizpass(object *op, char *params) {
    int i;

    if (!op)
        return 0;

    if (*params == '\0')
        i = (QUERY_FLAG(op, FLAG_WIZPASS)) ? 0 : 1;
    else
        i = onoff_value(params);

    if (i) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_095));
        SET_FLAG(op, FLAG_WIZPASS);
    } else {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_096));
        CLEAR_FLAG(op, FLAG_WIZPASS);
    }
    return 0;
}

/**
 * Wizard toggling "cast everywhere" ability.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_wizcast(object *op, char *params) {
    int i;

    if (!op)
        return 0;

    if (*params == '\0')
        i = (QUERY_FLAG(op, FLAG_WIZCAST)) ? 0 : 1;
    else
        i = onoff_value(params);

    if (i) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_097));
        SET_FLAG(op, FLAG_WIZCAST);
    } else {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_098));
        CLEAR_FLAG(op, FLAG_WIZCAST);
    }
    return 0;
}

/**
 * Various object-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_dumpallobjects(object *op, char *params) {
    object_dump_all();
    return 0;
}

/**
 * Various friendly object-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_dumpfriendlyobjects(object *op, char *params) {
    dump_friendly_objects();
    return 0;
}

/**
 * Various archetypes-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_dumpallarchetypes(object *op, char *params) {
    dump_all_archetypes();
    return 0;
}

/**
 * Various string-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_ssdumptable(object *op, char *params) {
    ss_dump_table(SS_DUMP_TABLE, NULL, 0);
    return 0;
}

/**
 * Various map-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_dumpmap(object *op, char *params) {
    if (op)
        dump_map(op->map);
    return 0;
}

/**
 * Various map-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_dumpallmaps(object *op, char *params) {
    dump_all_maps();
    return 0;
}

/**
 * Various LOS-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_printlos(object *op, char *params) {
    if (op)
        print_los(op);
    return 0;
}


/**
 * Server version.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_version(object *op, char *params) {
    version(op);
    return 0;
}

/**
 * Change the player's listen level.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_listen(object *op, char *params) {
    int i;

    if (*params == '\0' || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_105),
                             op->contr->listening);
        return 1;
    }
    if (i < 0) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_106));
        return 1;
    }
    op->contr->listening = (char)i;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_107),
                         i);
    return 1;
}

/**
 * Prints out some useful information for the character.  Everything we print
 * out can be determined by the docs, so we aren't revealing anything extra -
 * rather, we are making it convenient to find the values.  params have
 * no meaning here.
 *
 * @param pl
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_statistics(object *pl, char *params) {
    char buf[MAX_BUF];
    uint32 hours, minutes;
    uint64 seconds; /* 64 bit to prevent overflows an intermediate results */

    if (!pl->contr)
        return 1;
    strcpy(buf, i18n_translate(get_language(pl), I18N_MSG_CMISC_108));
    strcat(buf, FMT64);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         buf,
                         pl->stats.exp);
    strcpy(buf, i18n_translate(get_language(pl), I18N_MSG_CMISC_110));
    strcat(buf, FMT64);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         buf,
                         level_exp(pl->level+1, pl->expmul));

    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                  i18n_translate(get_language(pl), I18N_MSG_CMISC_112));

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_114),
                         pl->contr->orig_stats.Str, pl->stats.Str, 20+pl->arch->clone.stats.Str);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_116),
                         pl->contr->orig_stats.Dex, pl->stats.Dex, 20+pl->arch->clone.stats.Dex);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_118),
                         pl->contr->orig_stats.Con, pl->stats.Con, 20+pl->arch->clone.stats.Con);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_120),
                         pl->contr->orig_stats.Int, pl->stats.Int, 20+pl->arch->clone.stats.Int);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_122),
                         pl->contr->orig_stats.Wis, pl->stats.Wis, 20+pl->arch->clone.stats.Wis);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_124),
                         pl->contr->orig_stats.Pow, pl->stats.Pow, 20+pl->arch->clone.stats.Pow);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_126),
                         pl->contr->orig_stats.Cha, pl->stats.Cha, 20+pl->arch->clone.stats.Cha);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         i18n_translate(get_language(pl), I18N_MSG_CMISC_128),
                         pl->contr->peaceful ?  "Peaceful" : "Hostile");

    /* max_time is in microseconds - thus divide by 1000000.
     * Need 64 bit values, as otherwise ticks_played * max_time
     * can easily overflow.
     * Note the message displayed here isn't really
     * perfect, since if max_time has been changed since the player started,
     * the time estimates use the current value.  But I'm presuming that
     * max_time won't change very often.  MSW 2009-12-01
     */
    seconds = (uint64)pl->contr->ticks_played * (uint64)max_time / 1000000;
    minutes = (uint32)seconds / 60;
    hours = minutes / 60;
    minutes = minutes % 60;

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "You have played this character for %u ticks, which amounts "
                         "to %d hours and %d minutes.",
                         pl->contr->ticks_played, hours, minutes);


    /* Can't think of anything else to print right now */
    return 0;
}

/**
 * Wrapper to fix a player.
 *
 * @param op
 * player asking to be fixed.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_fix_me(object *op, char *params) {
    object_sum_weight(op);
    fix_object(op);
    return 1;
}

/**
 * Display all known players.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_players(object *op, char *params) {
    char buf[MAX_BUF];
    char *t;
    DIR *dir;

    snprintf(buf, sizeof(buf), "%s/%s/", settings.localdir, settings.playerdir);
    t = buf+strlen(buf);
    if ((dir = opendir(buf)) != NULL) {
        const struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            /* skip '.' , '..' */
            if (!((entry->d_name[0] == '.' && entry->d_name[1] == '\0')
                || (entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
                struct stat st;

                strcpy(t, entry->d_name);
                if (stat(buf, &st) == 0) {
                    /* This was not posix compatible
                     * if ((st.st_mode & S_IFMT)==S_IFDIR) {
                     */
                    if (S_ISDIR(st.st_mode)) {
                        struct tm *tm = localtime(&st.st_mtime);

                        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                             i18n_translate(get_language(op), I18N_MSG_CMISC_130),
                                             entry->d_name,
                                             1900+tm->tm_year,
                                             1+tm->tm_mon,
                                             tm->tm_mday,
                                             tm->tm_hour,
                                             tm->tm_min,
                                             tm->tm_sec);
                    }
                }
            }
        }
    }
    closedir(dir);
    return 0;
}

/**
 * Players wants to change the apply mode, ie how to handle applying an item when no body slot available.
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 * @return
 * 1.
 */
int command_applymode(object *op, char *params) {
    unapplymode unapply = op->contr->unapply;
    static const char *const types[] = {
        "nochoice",
        "never",
        "always"
    };

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_132),
                             types[op->contr->unapply]);
        return 1;
    }

    if (!strcmp(params, "nochoice"))
        op->contr->unapply = unapply_nochoice;
    else if (!strcmp(params, "never"))
        op->contr->unapply = unapply_never;
    else if (!strcmp(params, "always"))
        op->contr->unapply = unapply_always;
    else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_134),
                             params);
        return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_135),
                         (unapply == op->contr->unapply ? "" : " now"),
                         types[op->contr->unapply]);

    return 1;
}

/**
 * Player wants to change the bowmode, how arrows are fired.
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 * @return
 * 1.
 */
int command_bowmode(object *op, char *params) {
    bowtype_t oldtype = op->contr->bowtype;
    static const char *const types[] = {
        "normal",
        "threewide",
        "spreadshot",
        "firenorth",
        "firene",
        "fireeast",
        "firese",
        "firesouth",
        "firesw",
        "firewest",
        "firenw",
        "bestarrow"
    };
    char buf[MAX_BUF];
    int i, found;

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_136),
                             types[op->contr->bowtype]);
        return 1;
    }

    for (i = 0, found = 0; i <= bow_bestarrow; i++) {
        if (!strcmp(params, types[i])) {
            found++;
            op->contr->bowtype = i;
            break;
        }
    }
    if (!found) {
        snprintf(buf, sizeof(buf), "bowmode: Unknown options %s, valid options are:", params);
        for (i = 0; i <= bow_bestarrow; i++) {
            strcat(buf, " ");
            strcat(buf, types[i]);
            if (i < bow_nw)
                strcat(buf, ",");
            else
                strcat(buf, ".");
        }
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG, buf);
        return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_137),
                         (oldtype == op->contr->bowtype ? "" : "now"),
                         types[op->contr->bowtype]);
    return 1;
}

/**
 * Player wants to change prefered unarmed skill
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 * @return
 * 1.
 */
int command_unarmed_skill(object *op, char *params) {
    object *skill;
    size_t i;

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "unarmed skill is set to %s",
                             op->contr->unarmed_skill ? op->contr->unarmed_skill: "nothing");
        return 1;
    }

    /* find_skill_by_name() will ready any skill tools - which
     * is OK for us because no unarmed skills require skill tools,
     * but this could be an issue if you reuse this code for other skills.
     */
    skill = find_skill_by_name(op, params);

    if (!skill) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You do not know any such skill called %s",
                             params);
        return 1;
    }
    for (i = 0; i < sizeof(unarmed_skills); i++)
        if (skill->subtype == unarmed_skills[i])
            break;
    if (i == sizeof(unarmed_skills)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "%s is not an unarmed skill!",
                             skill->name);
        return 1;

    }

    if (op->contr->unarmed_skill)
        free_string(op->contr->unarmed_skill);

    /* Taking actual skill name is better than taking params,
     * as params could be something more than an exact skill name.
     */
    op->contr->unarmed_skill = add_string(skill->name);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         "unarmed skill is now set to %s",
                         op->contr->unarmed_skill);
    return 1;
}


/**
 * Player wants to change how her pets behave.
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 * @return
 * 1.
 */
int command_petmode(object *op, char *params) {
    petmode_t oldtype = op->contr->petmode;
    static const char *const types[] = {
        "normal",
        "sad",
        "defend",
        "arena"
    };

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_138),
                             types[op->contr->petmode]);
        return 1;
    }

    if (!strcmp(params, "normal"))
        op->contr->petmode = pet_normal;
    else if (!strcmp(params, "sad"))
        op->contr->petmode = pet_sad;
    else if (!strcmp(params, "defend"))
        op->contr->petmode = pet_defend;
    else if (!strcmp(params, "arena"))
        op->contr->petmode = pet_arena;
    else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_139),
                             params);
        return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_140),
                         (oldtype == op->contr->petmode ? "" : "now"),
                         types[op->contr->petmode]);
    return 1;
}

/**
 * Players wants to know her pets.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_showpets(object *op, char *params) {
    objectlink *obl, *next;
    int counter = 0, target = 0;
    int have_shown_pet = 0;
    if (*params != '\0')
        target = atoi(params);

    for (obl = first_friendly_object; obl != NULL; obl = next) {
        object *ob = obl->ob;

        next = obl->next;
        if (object_get_owner(ob) == op) {
            if (target == 0) {
                if (counter == 0)
                    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                  i18n_translate(get_language(op), I18N_MSG_CMISC_141));
                draw_ext_info_format(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_142),
                                     ++counter, ob->name, ob->level);
            } else if (!have_shown_pet && ++counter == target) {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_143),
                                     ob->level, ob->name);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_145),
                                     ob->stats.hp, ob->stats.maxhp, ob->stats.sp, ob->stats.maxsp);

                /* this is not a nice way to do this, it should be made to be more like the statistics command */
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_147),
                                     ob->stats.Str);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_149),
                                     ob->stats.Dex);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_151),
                                     ob->stats.Con);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_153),
                                     ob->stats.Int);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_155),
                                     ob->stats.Wis);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_157),
                                     ob->stats.Cha);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_159),
                                     ob->stats.Pow);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     i18n_translate(get_language(op), I18N_MSG_CMISC_161),
                                     ob->stats.wc, ob->stats.dam, ob->stats.ac);
                have_shown_pet = 1;
            }
        }
    }
    if (counter == 0)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_163));
    else if (target != 0 && have_shown_pet == 0)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_164));
    return 0;
}

/**
 * Player wants to change how keys are used.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_usekeys(object *op, char *params) {
    usekeytype oldtype = op->contr->usekeys;
    static const char *const types[] = {
        "inventory",
        "keyrings",
        "containers"
    };

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_165),
                             types[op->contr->usekeys]);
        return 1;
    }

    if (!strcmp(params, "inventory"))
        op->contr->usekeys = key_inventory;
    else if (!strcmp(params, "keyrings"))
        op->contr->usekeys = keyrings;
    else if (!strcmp(params, "containers"))
        op->contr->usekeys = containers;
    else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_166),
                             params);
        return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_167),
                         (oldtype == op->contr->usekeys ? "" : "now"),
                         types[op->contr->usekeys]);

    return 1;
}

/**
 * Players wants to know her resistances.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_resistances(object *op, char *params) {
    int i;
    if (!op)
        return 0;

    for (i = 0; i < NROFATTACKS; i++) {
        if (i == ATNR_INTERNAL)
            continue;

        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_168),
                             attacktype_desc[i], op->resist[i]);
    }

    /* If dragon player, let's display natural resistances */
    if (is_dragon_pl(op)) {
        int attack;
        object *tmp;

        tmp = object_find_by_type_and_arch_name(op, FORCE, "dragon_skin_force");
        if (tmp != NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_170));

            for (attack = 0; attack < NROFATTACKS; attack++) {
                if (atnr_is_dragon_enabled(attack)) {
                    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                                         i18n_translate(get_language(op), I18N_MSG_CMISC_171),
                                         change_resist_msg[attack], tmp->resist[attack]);
                }
            }
        }
    }

    return 0;
}

/**
 * Player wants to know available help topics.
 *
 * @param op
 * player asking for information.
 * @param what
 * - 1: wizard topics.
 * - 3: misc topics.
 * - other: regular commands.
 */
static void help_topics(object *op, int what) {
    DIR *dirp;
    struct dirent *de;
    char filename[MAX_BUF], line[HUGE_BUF];
    char suffix[MAX_BUF];
    int namelen;
    const char *language;

    language = language_codes[get_language(op)];
    snprintf(suffix, sizeof(suffix), ".%s", language);

    switch (what) {
    case 1:
        snprintf(filename, sizeof(filename), "%s/wizhelp", settings.datadir);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_172));
        break;

    case 3:
        snprintf(filename, sizeof(filename), "%s/mischelp", settings.datadir);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_173));
        break;

    default:
        snprintf(filename, sizeof(filename), "%s/help", settings.datadir);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_174));
        break;
    }
    if (!(dirp = opendir(filename)))
        return;

    line[0] = '\0';
    for (de = readdir(dirp); de; de = readdir(dirp)) {
        namelen = NAMLEN(de);

        if (namelen <= 2
        && *de->d_name == '.'
        && (namelen == 1 || de->d_name[1] == '.'))
            continue;
        if (strstr(de->d_name, suffix)) {
            strcat(line, strtok(de->d_name, "."));
            strcat(line, " ");
        }
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                  line);
    closedir(dirp);
}

/**
 * Helper function to display commands.
 *
 * @param op
 * player asking for information.
 * @param what
 * - 1: display wizard commands.
 * - 2: display communication commands.
 * - other: display regular commands.
 */
static void show_commands(object *op, int what) {
    char line[HUGE_BUF];
    int i, size, namelen;
    command_array_struct *ap;

    switch (what) {
    case 1:
        ap = WizCommands;
        size = WizCommandsSize;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_175));
        break;

    case 2:
        ap = CommunicationCommands;
        size = CommunicationCommandSize;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_176));
        break;

    default:
        ap = Commands;
        size = CommandsSize;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_177));
        break;
    }

    line[0] = '\0';
    for (i = 0; i < size; i++) {
        namelen = strlen(ap[i].name);

        strcat(line, ap[i].name);
        strcat(line, " ");
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line);
}

/**
 * Player is asking for some help.
 *
 * @param op
 * player asking for information.
 * @param params
 * what kind of help to ask for.
 * @return
 * 0.
 */
int command_help(object *op, char *params) {
    struct stat st;
    FILE *fp;
    char filename[MAX_BUF], line[MAX_BUF];
    int len;
    const char *language;

    language = language_codes[get_language(op)];

    /*
     * Main help page?
     */
    if (*params == '\0') {
        snprintf(filename, sizeof(filename), "%s/def_help", settings.datadir);
        if ((fp = fopen(filename, "r")) == NULL) {
            LOG(llevError, "Cannot open help file %s: %s\n", filename, strerror_local(errno, line, sizeof(line)));
            return 0;
        }
        while (fgets(line, MAX_BUF, fp)) {
            line[MAX_BUF-1] = '\0';
            len = strlen(line)-1;
            if (line[len] == '\n')
                line[len] = '\0';
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line);
        }
        fclose(fp);
        return 0;
    }

    /*
     * Topics list
     */
    if (!strcmp(params, "topics")) {
        help_topics(op, 3);
        help_topics(op, 0);
        if (QUERY_FLAG(op, FLAG_WIZ))
            help_topics(op, 1);
        return 0;
    }

    /*
     * Commands list
     */
    if (!strcmp(params, "commands")) {
        show_commands(op, 0);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "\n");
        show_commands(op, 2); /* show comm commands */
        if (QUERY_FLAG(op, FLAG_WIZ)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "\n");
            show_commands(op, 1);
        }
        return 0;
    }

    /*
     * User wants info about command
     */
    if (strchr(params, '.') || strchr(params, ' ') || strchr(params, '/')) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_178),
                             params);
        return 0;
    }

    snprintf(filename, sizeof(filename), "%s/mischelp/%s.%s", settings.datadir, params, language);
    if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
        if (op) {
            snprintf(filename, sizeof(filename), "%s/help/%s.%s", settings.datadir, params, language);
            if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
                if (QUERY_FLAG(op, FLAG_WIZ)) {
                    snprintf(filename, sizeof(filename), "%s/wizhelp/%s.%s", settings.datadir, params, language);
                    if (stat(filename, &st) || !S_ISREG(st.st_mode))
                        goto nohelp;
                } else
                    goto nohelp;
            }
        }
    }

    /*
     * Found that. Just cat it to screen.
     */
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open help file %s: %s\n", filename, strerror_local(errno, line, sizeof(line)));
        return 0;
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_179),
                         params);

    while (fgets(line, MAX_BUF, fp)) {
        line[MAX_BUF-1] = '\0';
        len = strlen(line)-1;
        if (line[len] == '\n')
            line[len] = '\0';
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line);
    }
    fclose(fp);
    return 0;

    /*
     * No_help -escape
     */

nohelp:
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_180),
                         params);

    return 0;
}

/**
 * Utility function to convert a reply to a yes/no or on/off value.
 *
 * @param line
 * string to check.
 * @retval 1
 * line is one of on y k s d.
 * @retval 0
 * other value.
 */
int onoff_value(const char *line) {
    int i;

    if (sscanf(line, "%d", &i))
        return (i != 0);

    switch (line[0]) {
    case 'o':
        switch (line[1]) {
        case 'n':
            return 1;  /* on */
        default:
            return 0;  /* o[ff] */
        }

    case 'y':   /* y[es] */
    case 'k':   /* k[ylla] */
    case 's':
    case 'd':
        return 1;

    case 'n':   /* n[o] */
    case 'e':   /* e[i] */
    case 'u':
    default:
        return 0;
    }
}

/**
 * Player wants to totally delete her character.
 *
 * @param op
 * player wanting to delete her character.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_quit(object *op, char *params) {
    if (QUERY_FLAG(op, FLAG_WAS_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_ADMIN_DM, "Can't quit when in DM mode.");
        return 1;
    }

    send_query(&op->contr->socket, CS_QUERY_SINGLECHAR,
               i18n_translate(get_language(op), I18N_MSG_CMISC_181));

    op->contr->state = ST_CONFIRM_QUIT;
    return 1;
}

/**
 * Player wants to change sound status.
 *
 * @param op
 * player asking for change.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_sound(object *op, char *params) {
    if (!(op->contr->socket.sound&SND_MUTE)) {
        op->contr->socket.sound = op->contr->socket.sound|SND_MUTE;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_186));
    } else {
        op->contr->socket.sound = op->contr->socket.sound&~SND_MUTE;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_187));
    }
    return 1;
}

/**
 * A player just entered her name.
 *
 * Perhaps these should be in player.c, but that file is
 * already a bit big.
 *
 * @param op
 * player we're getting the name of.
 */
void receive_player_name(object *op) {
    if (!check_name(op->contr, op->contr->write_buf+1)) {
        get_name(op);
        return;
    }
    FREE_AND_COPY(op->name, op->contr->write_buf+1);
    FREE_AND_COPY(op->name_pl, op->contr->write_buf+1);
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, op->contr->write_buf);
    op->contr->name_changed = 1;
    get_password(op);
}

/**
 * A player just entered her password, including for changing it.
 *
 * @param op
 * player.
 */
void receive_player_password(object *op) {
    unsigned int pwd_len = strlen(op->contr->write_buf);

    if (pwd_len <= 1 || pwd_len > 17) {
        if (op->contr->state == ST_CHANGE_PASSWORD_OLD
        || op->contr->state == ST_CHANGE_PASSWORD_NEW
        || op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_188));
            op->contr->state = ST_PLAYING;
        } else
            get_name(op);
        return;
    }
    /* To hide the password better */
    /* With currently clients, not sure if this is really the case - MSW */
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, "          ");

    if (checkbanned(op->name, op->contr->socket.host)) {
        LOG(llevInfo, "Banned player tried to add: [%s@%s]\n", op->name, op->contr->socket.host);
        draw_ext_info(NDI_UNIQUE|NDI_RED, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_189));
        get_name(op);
        return;
    }

    if (op->contr->state == ST_CONFIRM_PASSWORD) {
        if (!check_password(op->contr->write_buf+1, op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_190));
            get_name(op);
            return;
        }
        LOG(llevInfo, "LOGIN: New player named %s from ip %s\n", op->name, op->contr->socket.host);
        display_motd(op);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_191));
        roll_again(op);
        op->contr->state = ST_ROLL_STAT;
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_OLD) {
        if (!check_password(op->contr->write_buf+1, op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_192));
            op->contr->state = ST_PLAYING;
        } else {
            send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, i18n_translate(get_language(op), I18N_MSG_CMISC_193));
            op->contr->state = ST_CHANGE_PASSWORD_NEW;
        }
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_NEW) {
        strcpy(op->contr->new_password, crypt_string(op->contr->write_buf+1, NULL));
        send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, i18n_translate(get_language(op), I18N_MSG_CMISC_194));
        op->contr->state = ST_CHANGE_PASSWORD_CONFIRM;
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
        if (strcmp(crypt_string(op->contr->write_buf+1, op->contr->new_password), op->contr->new_password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_195));
        } else {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_196));
            strncpy(op->contr->password, op->contr->new_password, 13);
        }
        op->contr->state = ST_PLAYING;
        return;
    }

    strcpy(op->contr->password, crypt_string(op->contr->write_buf+1, NULL));
    op->contr->state = ST_ROLL_STAT;
    check_login(op, TRUE);
}

/**
 * Player wishes to change her title.
 *
 * @param op
 * player asking for change.
 * @param params
 * new title.
 * @return
 * 1.
 */
int command_title(object *op, char *params) {
    char buf[MAX_BUF];

    if (settings.set_title == FALSE) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_197));
        return 1;
    }

    /* dragon players cannot change titles */
    if (is_dragon_pl(op)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_198));
        return 1;
    }

    if (*params == '\0') {
        char tmp[MAX_BUF];

        player_get_title(op->contr, tmp, sizeof(tmp));
        snprintf(buf, sizeof(buf), i18n_translate(get_language(op), I18N_MSG_CMISC_199), tmp);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG, buf);
        return 1;
    }
    if (strcmp(params, "clear") == 0 || strcmp(params, "default") == 0) {
        if (!player_has_own_title(op->contr))
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_201));
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_202));
        player_set_own_title(op->contr, "");
        return 1;
    }

    if ((int)strlen(params) >= MAX_NAME) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_203));
        return 1;
    }
    player_set_own_title(op->contr, params);
    return 1;
}

/**
 * Player wants to get saved.
 *
 * @param op
 * player.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_save(object *op, char *params) {
    if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL)&P_NO_CLERIC) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_204));
    } else if (!op->stats.exp) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_205));
    } else {
        if (save_player(op, 1))
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_206));
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_207));
    }
    return 1;
}

/**
 * Player toggles her peaceful status.
 *
 * @param op
 * player.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_peaceful(object *op, char *params) {
    if ((op->contr->peaceful = !op->contr->peaceful))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_208));
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_209));
    return 1;
}

/**
 * Player wants to change how soon she'll flee.
 *
 * @param op
 * player.
 * @param params
 * new value.
 * @return
 * 1.
 */
int command_wimpy(object *op, char *params) {
    int i;

    if (*params == '\0' || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             i18n_translate(get_language(op), I18N_MSG_CMISC_210),
                             op->run_away);
        return 1;
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         i18n_translate(get_language(op), I18N_MSG_CMISC_211),
                         i);
    op->run_away = i;
    return 1;
}

/**
 * Player toggles her braced status.
 *
 * @param op
 * player.
 * @param params
 * brace status (on/off).
 * @return
 * 1.
 */
int command_brace(object *op, char *params) {
    if (*params == '\0')
        op->contr->braced = !op->contr->braced;
    else
        op->contr->braced = onoff_value(params);

    if (op->contr->braced)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_212));
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_213));

    fix_object(op);

    return 0;
}

/**
 * Player wants to get rid of pets.
 *
 * @param op
 * player.
 * @param params
 * unused.
 * @return
 * 0.
 */
int command_kill_pets(object *op, char *params) {
    objectlink *obl, *next;
    int counter = 0, removecount = 0;

    if (*params == '\0') {
        pets_terminate_all(op);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      i18n_translate(get_language(op), I18N_MSG_CMISC_214));
    } else {
        int target = atoi(params);
        for (obl = first_friendly_object; obl != NULL; obl = next) {
            object *ob = obl->ob;
            next = obl->next;
            if (object_get_owner(ob) == op)
                if (++counter == target || (target == 0 && !strcasecmp(ob->name, params)))  {
                    if (!QUERY_FLAG(ob, FLAG_REMOVED))
                        object_remove(ob);
                    remove_friendly_object(ob);
                    object_free_drop_inventory(ob);
                    removecount++;
                }
        }
        if (removecount != 0)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                 i18n_translate(get_language(op), I18N_MSG_CMISC_215),
                                 removecount);
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          i18n_translate(get_language(op), I18N_MSG_CMISC_216));
    }
    return 0;
}

/**
 * Player is asking to change password.
 *
 * @param pl
 * player.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_passwd(object *pl, char *params) {
    send_query(&pl->contr->socket, CS_QUERY_HIDEINPUT, i18n_translate(get_language(pl), I18N_MSG_CMISC_217));

    pl->contr->state = ST_CHANGE_PASSWORD_OLD;
    return 1;
}

/**
 * Player is trying to harvest something.
 * @param pl
 * player trying to harvest.
 * @param dir
 * direction.
 * @param skill
 * skill being used.
 * @return
 * 0
 */
int do_harvest(object *pl, int dir, object *skill) {
    sint16 x, y;
    int count = 0, proba; /* Probability to get the item, 100 based. */
    int level, exp;
    object *found[10]; /* Found items that can be harvested. */
    mapstruct *map;
    object *item, *inv;
    sstring trace, ttool, tspeed, race, tool, slevel, sexp;
    float speed;

    x = pl->x+freearr_x[dir];
    y = pl->y+freearr_y[dir];
    map = pl->map;

    if (!pl->type == PLAYER)
        return 0;

    if (!map)
        return 0;

    if (get_map_flags(map, &map, x, y, &x, &y)&P_OUT_OF_MAP) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You can't %s anything here.", skill->slaying);
        return 0;
    }

    if (!pl->chosen_skill || pl->chosen_skill->skill != skill->skill)
        return 0;

    trace = object_get_value(pl->chosen_skill, "harvest_race");
    ttool = object_get_value(pl->chosen_skill, "harvest_tool");
    tspeed = object_get_value(pl->chosen_skill, "harvest_speed");
    if (!trace || strcmp(trace, "") == 0 || !ttool || strcmp(ttool, "") == 0 || !tspeed || strcmp(tspeed, "") == 0) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You can't %s anything here.", skill->slaying);
        LOG(llevError, "do_harvest: tool %s without harvest_[race|tool|speed]\n", pl->chosen_skill->name);
        return 0;
    }

    item = GET_MAP_OB(map, x, y);
    while (item && count < 10) {
        FOR_INV_PREPARE(item, inv) {
            if (object_get_value(inv, "harvestable") == NULL)
                continue;
            race = object_get_value(inv, "harvest_race");
            tool = object_get_value(inv, "harvest_tool");
            slevel = object_get_value(inv, "harvest_level");
            sexp = object_get_value(inv, "harvest_exp");
            if (race && (!slevel || !sexp)) {
                LOG(llevError, "do_harvest: item %s without harvest_[level|exp]\n", inv->name);
                continue;
            }
            if (race == trace && (!tool || tool == ttool))
                found[count++] = inv;
        } FOR_INV_FINISH();
        item = item->above;
    }
    if (count == 0) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You can't %s anything here.", skill->slaying);
        return 0;
    }

    inv = found[rndm(0, count-1)];
    assert(inv);

    slevel = object_get_value(inv, "harvest_level");
    sexp = object_get_value(inv, "harvest_exp");
    level = atoi(slevel);
    exp = atoi(sexp);

    speed = atof(tspeed);
    if (speed < 0)
        speed = -speed*pl->speed;
    pl->speed_left -= speed;


    /* Now we found something to harvest, randomly try to get it. */
    if (level > skill->level+10) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", skill->slaying);
        return 0;
    }

    if (level >= skill->level)
        /* Up to 10 more levels, 1 to 11 percent probability. */
        proba = 10+skill->level-level;
    else if (skill->level <= level+10)
        proba = 10+(skill->level-level)*2;
    else
        proba = 30;

    if (proba <= random_roll(0, 100, pl, 1)) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", skill->slaying);
        return 0;
    }

    /* Ok, got it. */
    item = object_new();
    object_copy_with_inv(inv, item);
    object_set_value(item, "harvestable", NULL, 0);
    if (QUERY_FLAG(item, FLAG_MONSTER)) {
        int spot = object_find_free_spot(item, pl->map, pl->x, pl->y, 0, SIZEOFFREE);
        if (spot == -1) {
            /* Better luck next time...*/
            object_remove(item);
            draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", skill->slaying);
            return 0;
        }
        object_insert_in_map_at(item, pl->map, NULL, 0, pl->x+freearr_x[spot], pl->y+freearr_y[spot]);
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You %s a %s!", skill->slaying, item->name);
    } else {
        item = object_insert_in_ob(item, pl);
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You %s some %s", skill->slaying, item->name);
    }

    /* Get exp */
    change_exp(pl, exp, skill->name, SK_EXP_ADD_SKILL);

    return 0;
}

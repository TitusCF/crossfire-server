/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Various functions. Handles misc. input request - things like hash table, malloc, maps,
 * who, etc.
 */

#define WANT_UNARMED_SKILLS

#include "global.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#undef SS_STATISTICS
#include "loader.h"
#include "shstr.h"
#include "sounds.h"
#include "sproto.h"
#include "version.h"

#include "assets.h"

/* Static declarations where needed (when ordering would be an issue) */
static void display_who_entry(object *op, player *pl, const char *format);
static void get_who_escape_code_value(char *return_val, int size, const char letter, player *pl);
static int onoff_value(const char *line);

/**
 * This is the 'maps' command.
 *
 * @param op
 * player requesting the information.
 * @param search
 * optional substring to search for.
 */
static void map_info(object *op, const char *search) {
    if ( *search != 0 ) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                      "Maps matching search: '%s'", search);
    }
    if (QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                      "[fixed]Path             Reset In (HH:MM:SS) Pl IM   TO");
    } else {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                      "[fixed]Path             Reset In (HH:MM)");
    }

    /* Allow a comma-separate list of search strings; more complicated because of the const */
    char *to_be_freed = NULL;
    char *search_array[64];
    int search_array_count = 0;
    if ( search[0] ) {
        to_be_freed = strdup(search);
        if ( !to_be_freed ) {
            search_array[0] = to_be_freed;
            search_array_count = 1;
        } else {
            search_array_count = split_string(to_be_freed,search_array,64,',');
        }
    }

    for (mapstruct *m = first_map; m != NULL; m = m->next) {
        bool match = TRUE;
        if ( search_array_count ) {
            match = FALSE;
            for (int i=0; i<search_array_count; ++i) {
                if ( strstr(m->path,search_array[i]) ) {
                    match=TRUE;
                    break;
                }
            }
        }
        if ( !match ) continue;   /* Skip unwanted maps */

        /* Print out the last 26 characters of the map name... */
        char map_path[MAX_BUF];
        if (strlen(m->path) <= 26) {
            strcpy(map_path, m->path);
        } else {
            safe_strncpy(map_path, m->path + strlen(m->path) - 26, sizeof(map_path));
        }

        uint32_t ttr = MAP_WHEN_RESET(m) - seconds() > 0 ? MAP_WHEN_RESET(m) - seconds() : 0;
        if ( !m->reset_timeout && !m->unique ) set_map_reset_time(m);
        if (m->players) {
            ttr = (m->unique ? 0 : m->reset_timeout) + 300;
        }
        else if (m->timeout) {
            ttr = (m->unique ? 0 : m->reset_timeout) + m->timeout;
        }
        const uint32_t hh = ttr/3600, mm = (ttr%3600)/60, ss = ttr%60;
        if (QUERY_FLAG(op, FLAG_WIZ)) {
            draw_ext_info_format(
                NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                "[fixed]%-26.26s %2d:%02d:%02d  %2d %2d %4d",
                map_path, hh, mm, ss, m->players, m->in_memory, m->timeout);
        } else {
            draw_ext_info_format(
                NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
                "[fixed]%-26.26s %2d:%02d%s", map_path, hh, mm,
                m->players ? " (in use)" : "");
        }
    }
    if ( to_be_freed ) free( to_be_freed );
}

/**
 * This is the 'language' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * optional language code ("en", "fr", etc.)
 */
void command_language(object *op, const char *params) {
    int language = -1;

    if (!op->contr)
        return;

    if (*params == '\0' || (!strcmp(params, ""))) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             "Your current language is set to: English.");
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             "Available languages:");
        i18n_list_languages(op);
        return;
    }

    language = i18n_find_language_by_code(params);

    /* Error out if unknown language. */
    if (language == -1) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Unknown language.");
        return;
    }

    op->contr->language = language;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         "Your current language is set to English.");
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
 */
void command_body(object *op, const char *params) {
    int i;
    (void)params;

    /* Too hard to try and make a header that lines everything up, so just
     * give a description.
     */
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                  "The first column is the name of the body location.");

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                  "The second column is how many of those locations your body has.");

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                  "The third column is how many slots in that location are available.");

    for (i = 0; i < NUM_BODY_LOCATIONS; i++) {
        /* really debugging - normally body_used should not be set to anything
         * if body_info isn't also set.
         */
        if (op->body_info[i] || op->body_used[i]) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                                 "[fixed]%-30s %5d %5d",
                                 i18n(op, body_locations[i].use_name), op->body_info[i], op->body_used[i]);
        }
    }
    if (!QUERY_FLAG(op, FLAG_USE_ARMOUR))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                      "You are not allowed to wear armor.");
    if (!QUERY_FLAG(op, FLAG_USE_WEAPON))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
                      "You are not allowed to use weapons.");
}

/**
 * Display the message of the day.
 *
 * @param op
 * player requesting the motd.
 * @param params
 * unused.
 */
void command_motd(object *op, const char *params) {
    (void)params;
    display_motd(op);
}

/**
 * Display the server rules.
 *
 * @param op
 * player requesting the rules.
 * @param params
 * unused.
 */
void command_rules(object *op, const char *params) {
    (void)params;
    send_rules(op);
}

/**
 * Display the server news.
 *
 * @param op
 * player requesting the news.
 * @param params
 * unused.
 */
void command_news(object *op, const char *params) {
    (void)params;
    send_news(op);
}

/**
 * Sends various memory-related statistics.
 *
 * @param op
 * player requesting the information.
 */
static void malloc_info(object *op) {
    int ob_used = object_count_used(), ob_free = object_count_free(), players, nrofmaps;
    int nrm = 0, mapmem = 0, anr, anims, sum_alloc = 0, sum_used = 0, i, alnr;
    player *pl;
    mapstruct *m;
    artifactlist *al;

    for (al = first_artifactlist, alnr = 0; al != NULL; al = al->next, alnr++)
        ;

    anr = assets_number_of_archetypes();

    anims = assets_number_of_animations();

    for (pl = first_player, players = 0; pl != NULL; pl = pl->next, players++)
        ;

    for (m = first_map, nrofmaps = 0; m != NULL; m = m->next, nrofmaps++)
        if (m->in_memory == MAP_IN_MEMORY) {
            mapmem += map_size(m) * (sizeof(object *) + sizeof(MapSpace));
            nrm++;
        }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "Sizeof: object=%d  player=%d  map=%d",
                         sizeof(object), sizeof(player), sizeof(mapstruct));

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]Objects:");

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%6d used", ob_used);

    if (ob_used != nrofallocobjects - nroffreeobjects) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                             "[fixed]      (used list mismatch: %d)",
                             nrofallocobjects - nroffreeobjects);
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%6d free (%.2f%% of %d allocated)",
                         ob_free, (float)ob_free / nrofallocobjects * 100, nrofallocobjects);

    if (ob_free != nroffreeobjects) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                             "[fixed]      (free list mismatch: %d)",
                             nroffreeobjects);
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%6d on active list",
                         object_count_active());

    i = (ob_used*sizeof(object));
    sum_used += i;
    sum_alloc += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]     object total: %11d", i);

    i = (ob_free*sizeof(object));
    sum_alloc += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d players:         %8d",
                         players, i = (players*sizeof(player)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d maps allocated:  %8d",
                         nrofmaps, i = (nrofmaps*sizeof(mapstruct)));

    sum_alloc += i;
    sum_used += nrm*sizeof(mapstruct);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d maps in memory:  %8d",
                         nrm, mapmem);

    sum_alloc += mapmem;
    sum_used += mapmem;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d archetypes:      %8d",
                         anr, i = (anr*sizeof(archetype)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d animations:      %8d",
                         anims, i = (anims*sizeof(uint16_t)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d treasurelists    %8d",
                         assets_number_of_treasurelists(), i = (assets_number_of_treasurelists() * sizeof(treasurelist)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4ld treasures        %8d",
                         assets_number_of_treasures(), i = (assets_number_of_treasures() *sizeof(treasure)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4ld artifacts        %8d",
                         nrofartifacts, i = (nrofartifacts*sizeof(artifact)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4ld artifacts strngs %8d",
                         nrofallowedstr, i = (nrofallowedstr*sizeof(linked_char)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]%4d artifactlists    %8d",
                         alnr, i = (alnr*sizeof(artifactlist)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]Total space allocated:%8d",
                         sum_alloc);

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
                         "[fixed]Total space used:     %8d",
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
static void current_region_info(object *op) {
    /*
     * Ok I /suppose/ I should write a seperate function for this, but it isn't
     * going to be /that/ slow, and won't get called much
     */
    region *r = get_region_by_name(get_name_of_region_for_map(op->map));

    /* This should only be possible if regions are not operating on this server. */
    if (!r)
        return;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         "You are in %s.\n%s",
                         get_region_longname(r), get_region_msg(r));
}

/**
 * 'mapinfo' command.
 *
 * @param op
 * player requesting the information.
 */
static void current_map_info(object *op) {
    mapstruct *m = op->map;

    if (!m)
        return;

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         "%s (%s) in %s",
                         m->name, m->path, get_region_longname(get_region_by_map(m)));

    if (QUERY_FLAG(op, FLAG_WIZ)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             "players:%d difficulty:%d size:%dx%d start:%dx%d timeout %d",
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
 */
void command_malloc_verify(object *op, char *parms) {
    extern int malloc_verify(void);

    if (!malloc_verify())
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Heap is corrupted.");
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Heap checks out OK.");

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
 */
void command_whereabouts(object *op, const char *params) {
    region *reg;
    player *pl;
    (void)params;

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
                  "In the world currently there are:");

    for (reg = first_region; reg != NULL; reg = reg->next)
        if (reg->counter > 0) {
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                 "%u players in %s",
                                 reg->counter, get_region_longname(reg));
        }
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
    uint16_t i;
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
                              "who failed - out of memory!");
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
                                 "Total Players (%d) -- WIZ(%d) AFK(%d) BOT(%d)",
                                 num_players, num_wiz, num_afk, num_bot);
        else if (party == NULL)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                                 "Total Players in %s (%d) -- WIZ(%d) AFK(%d) BOT(%d)",
                                 reg->longname ? reg->longname : reg->name, num_players, num_wiz, num_afk, num_bot);
        else
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                                 "Total Players in party %s (%d) -- WIZ(%d) AFK(%d) BOT(%d)",
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
 */
void command_who(object *op, const char *params) {
    region *reg;

    reg = get_region_from_string(params);
    list_players(op, reg, NULL);
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
static void display_who_entry(object *op, player *pl, const char *format) {
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
static void get_who_escape_code_value(char *return_val, int size, const char letter, player *pl) {
    switch (letter) {
    case 'N':
        strlcpy(return_val, pl->ob->name, size);
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
        strlcpy(return_val, pl->peaceful ? "" : " <Hostile>", size);
        break;

    case 'l':
        snprintf(return_val, size, "%d", pl->ob->level);
        break;

    case 'd':
        strlcpy(return_val, (QUERY_FLAG(pl->ob, FLAG_WIZ) ? " <WIZ>" : ""), size);
        break;

    case 'a':
        strlcpy(return_val, (QUERY_FLAG(pl->ob, FLAG_AFK) ? " <AFK>" : ""), size);
        break;

    case 'b':
        strlcpy(return_val, (pl->socket.is_bot == 1) ? " <BOT>" : "", size);
        break;

    case 'm':
        strlcpy(return_val, pl->ob->map->path, size);
        break;

    case 'M':
        strlcpy(return_val, pl->ob->map->name ? pl->ob->map->name : "Untitled", size);
        break;

    case 'r':
        strlcpy(return_val, get_name_of_region_for_map(pl->ob->map), size);
            break;

    case 'R':
        strlcpy(return_val, get_region_longname(get_region_by_map(pl->ob->map)), size);
        break;

    case 'i':
        strlcpy(return_val, pl->socket.host, size);
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
 */
void command_afk(object *op, const char *params) {
    (void)params;
    if (QUERY_FLAG(op, FLAG_AFK)) {
        CLEAR_FLAG(op, FLAG_AFK);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You are no longer AFK");
    } else {
        SET_FLAG(op, FLAG_AFK);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You are now AFK");
    }
}

/**
 * Display memory information.
 *
 * @param op
 * player requesting information.
 * @param params
 * unused.
 */
void command_malloc(object *op, const char *params) {
    (void)params;
    malloc_info(op);
}

/**
 * 'mapinfo' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * unused.
 */
void command_mapinfo(object *op, const char *params) {
    (void)params;
    current_map_info(op);
}

/**
 * 'whereami' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * unused.
 */
void command_whereami(object *op, const char *params) {
    (void)params;
    current_region_info(op);
}

/**
 * 'maps' command.
 *
 * @param op
 * player requesting the information.
 * @param params
 * region to restrict to.
 */
void command_maps(object *op, const char *params) {
    map_info(op, params);
}

/**
 * Various string-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_strings(object *op, const char *params) {
    char stats[HUGE_BUF];
    (void)params;

    ss_dump_statistics(stats, sizeof(stats));
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_LAST,
                         "[fixed]%s\n",
                         stats);

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_LAST,
                  ss_dump_table(SS_DUMP_TOTALS, stats, sizeof(stats)));
}

/**
 * Players asks for the time.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_time(object *op, const char *params) {
    (void)params;
    time_info(op);
}

/**
 * Player is asking for the hiscore.
 *
 * @param op
 * player asking for information.
 * @param params
 * options for the hiscore command (typically a match for the names)
 */
void command_hiscore(object *op, const char *params) {
    hiscore_display(op, op == NULL ? 9999 : 50, params);
}

/**
 * Player wants to see/change the debug level.
 *
 * @param op
 * player asking for information.
 * @param params
 * new debug value.
 */
void command_debug(object *op, const char *params) {
    int i;

    if (*params == '\0' || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             "Global debug level is %d.",
                             settings.debug);
        return;
    }
    settings.debug = (enum LogLevel)FABS(i);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         "Debug level set to %d.",
                         i);
}


/**
 * Wizard toggling wall-crossing.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_wizpass(object *op, const char *params) {
    int i;

    if (!op)
        return;

    if (*params == '\0')
        i = (QUERY_FLAG(op, FLAG_WIZPASS)) ? 0 : 1;
    else
        i = onoff_value(params);

    if (i) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You will now walk through walls.");
        SET_FLAG(op, FLAG_WIZPASS);
    } else {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You will now be stopped by walls.");
        CLEAR_FLAG(op, FLAG_WIZPASS);
    }
}

/**
 * Wizard toggling "cast everywhere" ability.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_wizcast(object *op, const char *params) {
    int i;

    if (!op)
        return;

    if (*params == '\0')
        i = (QUERY_FLAG(op, FLAG_WIZCAST)) ? 0 : 1;
    else
        i = onoff_value(params);

    if (i) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You can now cast spells anywhere.");
        SET_FLAG(op, FLAG_WIZCAST);
    } else {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You now cannot cast spells in no-magic areas.");
        CLEAR_FLAG(op, FLAG_WIZCAST);
    }
}

/**
 * Various object-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_dumpallobjects(object *op, const char *params) {
    (void)op;
    (void)params;
    object_dump_all();
}

/**
 * Various friendly object-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_dumpfriendlyobjects(object *op, const char *params) {
    (void)op;
    (void)params;
    dump_friendly_objects();
}

/**
 * Various archetypes-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_dumpallarchetypes(object *op, const char *params) {
    (void)op;
    (void)params;
    dump_all_archetypes();
}

/**
 * Various string-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_ssdumptable(object *op, const char *params) {
    (void)op;
    (void)params;
    ss_dump_table(SS_DUMP_TABLE, NULL, 0);
}

/**
 * Various map-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_dumpmap(object *op, const char *params) {
    (void)params;
    if (op)
        dump_map(op->map);
}

/**
 * Various map-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_dumpallmaps(object *op, const char *params) {
    (void)op;
    (void)params;
    dump_all_maps();
}

/**
 * Various LOS-related statistics.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_printlos(object *op, const char *params) {
    (void)params;
    if (op)
        print_los(op);
}


/**
 * Server version.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_version(object *op, const char *params) {
    (void)params;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_ADMIN,
            MSG_TYPE_ADMIN_VERSION, "Crossfire "FULL_VERSION);
}

/**
 * Change the player's listen level.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_listen(object *op, const char *params) {
    int i;

    if (*params == '\0' || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             "Set listen to what (presently %d)?",
                             op->contr->listening);
        return;
    }
    if (i < 0) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                             "Verbose level should be positive.");
        return;
    }
    op->contr->listening = (char)i;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                         "Your verbose level is now %d.",
                         i);
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
 */
void command_statistics(object *pl, const char *params) {
    char buf[MAX_BUF];
    uint32_t hours, minutes;
    uint64_t seconds; /* 64 bit to prevent overflows an intermediate results */
    (void)params;

    if (!pl->contr)
        return;
    safe_strncpy(buf, i18n(pl, "[fixed]  Experience: %"), sizeof(buf));
    strcat(buf, FMT64);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         buf,
                         pl->stats.exp);
    safe_strncpy(buf, i18n(pl, "[fixed]  Next Level: %"), sizeof(buf));
    strcat(buf, FMT64);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         buf,
                         level_exp(pl->level+1, pl->expmul));

    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                  "[fixed]\nStat       Nat/Real/Max");

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Str         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Str, pl->stats.Str, 20+pl->arch->clone.stats.Str);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Dex         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Dex, pl->stats.Dex, 20+pl->arch->clone.stats.Dex);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Con         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Con, pl->stats.Con, 20+pl->arch->clone.stats.Con);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Int         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Int, pl->stats.Int, 20+pl->arch->clone.stats.Int);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Wis         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Wis, pl->stats.Wis, 20+pl->arch->clone.stats.Wis);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Pow         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Pow, pl->stats.Pow, 20+pl->arch->clone.stats.Pow);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "[fixed]Cha         %2d/ %3d/%3d",
                         pl->contr->orig_stats.Cha, pl->stats.Cha, 20+pl->arch->clone.stats.Cha);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "\nAttack Mode: %s",
                         i18n(pl, pl->contr->peaceful ?  "Peaceful" : "Hostile"));
    /**
     * Add preliminary damage per second calculations here
     */
    float weap_speed = pl->weapon_speed; // This is the number of attacks per tick.
    if (weap_speed < 0.0f)
        weap_speed = 0.0f;
    if (weap_speed > 1.0f)
        weap_speed = 1.0f;
    // We will initially calculate the damage if every attack you perform hits.
    // This will serve as a baseline for future calculations
    float dps = (1000000.0f / max_time) * weap_speed * pl->stats.dam;
    // TODO: Account for opposing AC in calculations, make some sort of table/chart.
    // Then we round the floating-point.
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                        "\n\nDam/Sec: %4d", (int)(dps + 0.5f));

    /* max_time is in microseconds - thus divide by 1000000.
     * Need 64 bit values, as otherwise ticks_played * max_time
     * can easily overflow.
     * Note the message displayed here isn't really
     * perfect, since if max_time has been changed since the player started,
     * the time estimates use the current value.  But I'm presuming that
     * max_time won't change very often.  MSW 2009-12-01
     */
    seconds = (uint64_t)pl->contr->ticks_played * (uint64_t)max_time / 1000000;
    minutes = (uint32_t)seconds / 60;
    hours = minutes / 60;
    minutes = minutes % 60;

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
                         "You have played this character for %u ticks, which amounts "
                         "to %d hours and %d minutes.",
                         pl->contr->ticks_played, hours, minutes);


    /* Can't think of anything else to print right now */
}

/**
 * Wrapper to fix a player.
 *
 * @param op
 * player asking to be fixed.
 * @param params
 * unused.
 */
void command_fix_me(object *op, const char *params) {
    (void)params;
    object_sum_weight(op);
    fix_object(op);
}

/**
 * Display all known players.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_players(object *op, const char *params) {
    char buf[MAX_BUF];
    char *t;
    DIR *dir;
    (void)params;

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
                                             "[fixed]%s\t%04d %02d %02d %02d %02d %02d",
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
}

/**
 * Players wants to change the apply mode, ie how to handle applying an item when no body slot available.
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 */
void command_applymode(object *op, const char *params) {
    unapplymode unapply = op->contr->unapply;
    static const char *const types[] = {
        "nochoice",
        "never",
        "always"
    };

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "applymode is set to %s",
                             types[op->contr->unapply]);
        return;
    }

    if (!strcmp(params, "nochoice"))
        op->contr->unapply = unapply_nochoice;
    else if (!strcmp(params, "never"))
        op->contr->unapply = unapply_never;
    else if (!strcmp(params, "always"))
        op->contr->unapply = unapply_always;
    else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "applymode: Unknown options %s, valid options are nochoice, never, always",
                             params);
        return;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         "applymode%s set to %s",
                         (unapply == op->contr->unapply ? "" : " now"),
                         types[op->contr->unapply]);
}

/**
 * Player wants to change the bowmode, how arrows are fired.
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 */
void command_bowmode(object *op, const char *params) {
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
    int i, found;

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "bowmode is set to %s",
                             types[op->contr->bowtype]);
        return;
    }

    for (i = 0, found = 0; i <= bow_bestarrow; i++) {
        if (!strcmp(params, types[i])) {
            found++;
            op->contr->bowtype = i;
            break;
        }
    }
    if (!found) {
        StringBuffer *buf = stringbuffer_new();
        stringbuffer_append_printf(buf, "bowmode: Unknown options %s, valid options are:", params);
        for (i = 0; i <= bow_bestarrow; i++) {
            stringbuffer_append_string(buf, " ");
            stringbuffer_append_string(buf, types[i]);
            if (i < bow_nw)
                stringbuffer_append_string(buf, ",");
            else
                stringbuffer_append_string(buf, ".");
        }
        char *result = stringbuffer_finish(buf);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG, result);
        free(result);
        return;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         "bowmode%s set to %s",
                         (oldtype == op->contr->bowtype ? "" : " now"),
                         types[op->contr->bowtype]);
    return;
}

/**
 * Player wants to change prefered unarmed skill
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 */
void command_unarmed_skill(object *op, const char *params) {
    object *skill;
    size_t i;

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "unarmed skill is set to %s",
                             op->contr->unarmed_skill ? op->contr->unarmed_skill: "nothing");
        return;
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
        return;
    }
    for (i = 0; i < sizeof(unarmed_skills); i++)
        if (skill->subtype == unarmed_skills[i])
            break;
    if (i == sizeof(unarmed_skills)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "%s is not an unarmed skill!",
                             skill->name);
        return;

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
}


/**
 * Player wants to change how her pets behave.
 *
 * @param op
 * player asking for change.
 * @param params
 * new mode.
 */
void command_petmode(object *op, const char *params) {
    petmode_t oldtype = op->contr->petmode;
    static const char *const types[] = {
        "normal",
        "sad",
        "defend",
        "arena"
    };

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "petmode is set to %s",
                             types[op->contr->petmode]);
        return;
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
                             "petmode: Unknown options %s, valid options are normal, sad (seek and destroy), defend, arena",
                             params);
        return;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         "petmode%s set to %s",
                         (oldtype == op->contr->petmode ? "" : " now"),
                         types[op->contr->petmode]);
}

/**
 * Players wants to know her pets.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_showpets(object *op, const char *params) {
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
                                  "Pets:");
                draw_ext_info_format(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "%d  %s - level %d",
                                     ++counter, ob->name, ob->level);
            } else if (!have_shown_pet && ++counter == target) {
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]level %d %s",
                                     ob->level, ob->name);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]%d/%d HP, %d/%d SP",
                                     ob->stats.hp, ob->stats.maxhp, ob->stats.sp, ob->stats.maxsp);

                /* this is not a nice way to do this, it should be made to be more like the statistics command */
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Str %d",
                                     ob->stats.Str);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Dex %d",
                                     ob->stats.Dex);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Con %d",
                                     ob->stats.Con);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Int %d",
                                     ob->stats.Int);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Wis %d",
                                     ob->stats.Wis);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Cha %d",
                                     ob->stats.Cha);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]Pow %d",
                                     ob->stats.Pow);
                draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                                     "[fixed]wc %d  damage %d ac %d",
                                     ob->stats.wc, ob->stats.dam, ob->stats.ac);
                have_shown_pet = 1;
            }
        }
    }
    if (counter == 0)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You have no pets.");
    else if (target != 0 && have_shown_pet == 0)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "No such pet.");
}

/**
 * Player wants to change how keys are used.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_usekeys(object *op, const char *params) {
    usekeytype oldtype = op->contr->usekeys;
    static const char *const types[] = {
        "inventory",
        "keyrings",
        "containers"
    };

    if (*params == '\0') {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "usekeys is set to %s",
                             types[op->contr->usekeys]);
        return;
    }

    if (!strcmp(params, "inventory"))
        op->contr->usekeys = key_inventory;
    else if (!strcmp(params, "keyrings"))
        op->contr->usekeys = keyrings;
    else if (!strcmp(params, "containers"))
        op->contr->usekeys = containers;
    else {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "usekeys: Unknown option %s, valid options are inventory, keyrings, containers",
                             params);
        return;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         "usekeys%s set to %s",
                         (oldtype == op->contr->usekeys ? "" : " now"),
                         types[op->contr->usekeys]);
}

/**
 * Players wants to know her resistances.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 */
void command_resistances(object *op, const char *params) {
    int i;
    (void)params;
    if (!op)
        return;

    for (i = 0; i < NROFATTACKS; i++) {
        if (i == ATNR_INTERNAL)
            continue;

        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                             "[fixed]%-20s %+5d",
                             attacktype_desc[i], op->resist[i]);
    }

    /* If dragon player, let's display natural resistances */
    if (is_dragon_pl(op)) {
        int attack;
        object *tmp;

        tmp = object_find_by_type_and_arch_name(op, FORCE, "dragon_skin_force");
        if (tmp != NULL) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                          "\nNatural skin resistances:");

            for (attack = 0; attack < NROFATTACKS; attack++) {
                if (atnr_is_dragon_enabled(attack)) {
                    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                                         "%s: %d",
                                         change_resist_msg[attack], tmp->resist[attack]);
                }
            }
        }
    }
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

    language = i18n_get_language_code(op->contr->language);
    snprintf(suffix, sizeof(suffix), ".%s", language);

    switch (what) {
    case 1:
        snprintf(filename, sizeof(filename), "%s/wizhelp", settings.datadir);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      "      Wiz commands:");
        break;

    case 3:
        snprintf(filename, sizeof(filename), "%s/mischelp", settings.datadir);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      "      Misc help:");
        break;

    default:
        snprintf(filename, sizeof(filename), "%s/help", settings.datadir);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                      "      Commands:");
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
 * Find a (help) file in the specified subdirectory of data.
 * @param dir subdirectory to search, 'help' or 'wizhelp'.
 * @param name file name to look for.
 * @param language language prefix.
 * @param[out] path modified to contain the full path of the tested file.
 * @param length maximum length of path.
 * @return 1 if file was found, 0 else.
 */
static int find_help_file_in(const char *dir, const char *name, const char *language, char *path, int length) {
    struct stat st;

    snprintf(path, length, "%s/%s/%s.%s", settings.datadir, dir, name, language);
    if (stat(path, &st) == 0 && S_ISREG(st.st_mode)) {
        return 1;
    }
    return 0;
}

/**
 * Find an appropriate help file. Will search regular commands, and wizard ones
 * if asked for. Specified language is tried, as well as English.
 * 'path' is altered whatever the return value.
 * Wizard help is searched first, to allow a different syntax of a regular command
 * when run in DM.
 * @param name command name.
 * @param language player language.
 * @param wiz if 1 the wizard-related files are searched, else no.
 * @param[out] path buffer to contain the found help file, modified.
 * @param length length of path.
 * @return 1 if a file was found in which case path contains the path, else 0.
 */
static int find_help_file(const char *name, const char *language, int wiz, char *path, int length) {
    if (wiz) {
        if (find_help_file_in("wizhelp", name, language, path, length))
            return 1;
        if (strcmp(language, "en") && find_help_file_in("wizhelp", name, "en", path, length))
            return 1;
    }

    if (find_help_file_in("help", name, language, path, length))
        return 1;
    if (strcmp(language, "en") && find_help_file_in("help", name, "en", path, length))
        return 1;

    return 0;
}

/**
 * Attempt to send the contents of the specified file to the player.
 * If the file does not exist, then an error is logged to the server, and
 * nothing is sent to the player.
 * @param op who to send the file to.
 * @param filename full help filename.
 */
static void display_help_file(object *op, const char *filename) {
    BufferReader *br;
    const char *line;

    if ((br = bufferreader_init_from_file(NULL, filename, "Cannot open help file %s: %s\n", llevError)) == NULL) {
        return;
    }

    while ((line = bufferreader_next_line(br))) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line);
    }

    bufferreader_destroy(br);
}

/**
 * Player is asking for some help.
 *
 * @param op
 * player asking for information.
 * @param params
 * what kind of help to ask for.
 */
void command_help(object *op, const char *params) {
    char filename[MAX_BUF];
    const char *language;

    /*
     * Main help page?
     */
    if (*params == '\0') {
        snprintf(filename, sizeof(filename), "%s/def_help", settings.datadir);
        display_help_file(op, filename);
        return;
    }

    /*
     * Topics list
     */
    if (!strcmp(params, "topics")) {
        help_topics(op, 3);
        help_topics(op, 0);
        if (QUERY_FLAG(op, FLAG_WIZ))
            help_topics(op, 1);
        return;
    }

    /*
     * Commands list
     */
    if (!strcmp(params, "commands")) {
        command_list(op, QUERY_FLAG(op, FLAG_WIZ));
        return;
    }

    /*
     * User wants info about command
     */
    if (strchr(params, '.') || strchr(params, ' ') || strchr(params, '/')) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                             "Illegal characters in '%s'",
                             params);
        return;
    }

    language = i18n_get_language_code(op->contr->language);

    if (!find_help_file(params, language, QUERY_FLAG(op, FLAG_WIZ), filename, sizeof(filename))) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                            "No help available on '%s'",
                            params);
        return;
    }

    /*
     * Found that. Just cat it to screen.
     */
    display_help_file(op, filename);
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
static int onoff_value(const char *line) {
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
 * Tell players to use the 'delete' command. Too many new players have
 * accidentally deleted their characters because they didn't carefully read
 * the warning that the original 'quit' command gave.
 */
void command_quit(object* op, const char* params) {
    (void)params;
    draw_ext_info(
        NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
             "To leave the game, sleep in (apply) a bed to reality. To "
             "permenantly delete your character, use the 'delete' command.");
}

/**
 * Player wants to totally delete her character.
 *
 * @param op
 * player wanting to delete her character.
 * @param params
 * unused.
 */
void command_delete(object *op, const char *params) {
    (void)params;
    if (QUERY_FLAG(op, FLAG_WAS_WIZ)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_ADMIN_DM, "Can't quit when in DM mode.");
        return;
    }

    send_query(&op->contr->socket, CS_QUERY_SINGLECHAR,
               i18n(op, "Quitting will delete your character.\nAre you sure you want to delete your character (y/n):"));

    player_set_state(op->contr, ST_CONFIRM_QUIT);
}

/**
 * Player wants to change sound status.
 *
 * @param op
 * player asking for change.
 * @param params
 * unused.
 */
void command_sound(object *op, const char *params) {
    (void)params;
    if (!(op->contr->socket.sound&SND_MUTE)) {
        op->contr->socket.sound = op->contr->socket.sound|SND_MUTE;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Sounds are turned off.");
    } else {
        op->contr->socket.sound = op->contr->socket.sound&~SND_MUTE;
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "The sounds are enabled.");
    }
    return;
}

/**
 * A player just entered her name.
 *
 * Perhaps these should be in player.c, but that file is
 * already a bit big.
 *
 * @param op
 * player we're getting the name of.
 * @param name
 * name the player entered.
 */
void receive_player_name(object *op, const char *name) {
    if (!check_name(op->contr, name)) {
        get_name(op);
        return;
    }
    FREE_AND_COPY(op->name, name);
    FREE_AND_COPY(op->name_pl, name);
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, name);
    op->contr->name_changed = 1;
    get_password(op);
}

/**
 * A player just entered her password, including for changing it.
 *
 * @param op
 * player.
 * @param password
 * password used.
 */
void receive_player_password(object *op, const char *password) {
    unsigned int pwd_len = strlen(password);

    if (pwd_len == 0 || pwd_len > 16) {
        if (op->contr->state == ST_CHANGE_PASSWORD_OLD
        || op->contr->state == ST_CHANGE_PASSWORD_NEW
        || op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "Password changed cancelled.");
            player_set_state(op->contr, ST_PLAYING);
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
                      "You are not allowed to play.");
        get_name(op);
        return;
    }

    if (op->contr->state == ST_CONFIRM_PASSWORD) {
        if (!check_password(password, op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "The passwords did not match.");
            get_name(op);
            return;
        }
        LOG(llevInfo, "LOGIN: New player named %s from ip %s\n", op->name, op->contr->socket.host);
        display_motd(op);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "\nWelcome, Brave New Warrior!\n");
        roll_again(op);
        player_set_state(op->contr, ST_ROLL_STAT);
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_OLD) {
        if (!check_password(password, op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "You entered the wrong current password.");
            player_set_state(op->contr, ST_PLAYING);
        } else {
            send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, i18n(op, "Please enter your new password, or blank to cancel:"));
            player_set_state(op->contr, ST_CHANGE_PASSWORD_NEW);
        }
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_NEW) {
        safe_strncpy(op->contr->new_password, newhash(password),
                sizeof(op->contr->new_password));
        send_query(&op->contr->socket, CS_QUERY_HIDEINPUT,
                i18n(op, "Please confirm your new password, or blank to cancel:"));
        player_set_state(op->contr, ST_CHANGE_PASSWORD_CONFIRM);
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
        if (!check_password(password, op->contr->new_password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "The new passwords don't match!");
        } else {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "Password changed.");
            strncpy(op->contr->password, op->contr->new_password, 13);
        }
        player_set_state(op->contr, ST_PLAYING);
        return;
    }

    safe_strncpy(op->contr->password, newhash(password),
            sizeof(op->contr->password));
    player_set_state(op->contr, ST_ROLL_STAT);
    check_login(op, password);
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
void command_title(object *op, const char *params) {
    char buf[MAX_BUF];

    if (settings.set_title == FALSE) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      "You cannot change your title.");
        return;
    }

    /* dragon players cannot change titles */
    if (is_dragon_pl(op)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      "Dragons cannot change titles.");
        return;
    }

    if (*params == '\0') {
        char tmp[MAX_BUF];

        player_get_title(op->contr, tmp, sizeof(tmp));
        snprintf(buf, sizeof(buf), "Your title is '%s'.", tmp);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG, buf);
        return;
    }
    if (strcmp(params, "clear") == 0 || strcmp(params, "default") == 0) {
        if (!player_has_own_title(op->contr))
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                          "Your title is the default title.");
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                          "Title set to default.");
        player_set_own_title(op->contr, "");
        return;
    }

    if ((int)strlen(params) >= MAX_NAME) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      "Title too long.");
        return;
    }
    player_set_own_title(op->contr, params);
}

/**
 * Player wants to get saved.
 *
 * @param op
 * player.
 * @param params
 * unused.
 */
void command_save(object *op, const char *params) {
    (void)params;
    if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL)&P_NO_CLERIC) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You can not save on unholy ground.");
    } else if (!op->stats.exp) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You don't deserve to save yet.");
    } else {
        if (save_player(op, 1))
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "You have been saved.");
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "SAVE FAILED!");
    }
}

/**
 * Player toggles her peaceful status.
 *
 * @param op
 * player.
 * @param params
 * unused.
 */
void command_peaceful(object *op, const char *params) {
    (void)params;
    if ((op->contr->peaceful = !op->contr->peaceful))
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      "You will not attack other players.");
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                      "You will attack other players.");
}

/**
 * Player wants to change how soon she'll flee.
 *
 * @param op
 * player.
 * @param params
 * new value.
 */
void command_wimpy(object *op, const char *params) {
    int i;

    if (*params == '\0' || !sscanf(params, "%d", &i)) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                             "Your current wimpy level is %d.",
                             op->run_away);
        return;
    }

    if (i < 0 || i > 100) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                            "Wimpy level should be between 1 and 100.",
                            i);
        return;
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
                         "Your new wimpy level is %d.",
                         i);
    op->run_away = i;
}

/**
 * Player toggles her braced status.
 *
 * @param op
 * player.
 * @param params
 * brace status (on/off).
 */
void command_brace(object *op, const char *params) {
    if (*params == '\0')
        op->contr->braced = !op->contr->braced;
    else
        op->contr->braced = onoff_value(params);

    if (op->contr->braced)
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "You are braced.");
    else
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Not braced.");

    fix_object(op);
}

/**
 * Player wants to get rid of pets.
 *
 * @param op
 * player.
 * @param params
 * unused.
 */
void command_kill_pets(object *op, const char *params) {
    objectlink *obl, *next;
    int counter = 0, removecount = 0;

    if (*params == '\0') {
        pets_terminate_all(op);
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                      "Your pets have been killed.");
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
                                 "Killed %d pets.",
                                 removecount);
        else
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "Couldn't find any suitable pets to kill.");
    }
}

/**
 * Player is asking to change password.
 *
 * @param pl
 * player.
 * @param params
 * unused.
 */
void command_passwd(object *pl, const char *params) {
    (void)params;
    /* If old client, this is the way you change your password. */
    if (pl->contr->socket.login_method < 1){
        send_query(&pl->contr->socket, CS_QUERY_HIDEINPUT, i18n(pl, "Password change.\nPlease enter your current password, or empty string to cancel."));

        player_set_state(pl->contr, ST_CHANGE_PASSWORD_OLD);
    }
    /* If new client (login_method = 2) or jxclient (login_method = 1), changing the password does nothing anyway, so error out */
    else{
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                          "passwd is maintained for older clients that do not support the account system. Please use the 'Password' button in your character selection screen to change your password.");
    }
}

/**
 * Player is trying to harvest something.
 * @param pl
 * player trying to harvest.
 * @param dir
 * direction.
 * @param skill
 * skill being used.
 */
void do_harvest(object *pl, int dir, object *skill) {
    int16_t x, y;
    int count = 0, proba; /* Probability to get the item, 100 based. */
    int level, exp, check_exhaust = 0;
    object *found[10]; /* Found items that can be harvested. */
    mapstruct *map;
    object *item, *inv, *harvested;
    sstring trace, ttool, tspeed, race, tool, slevel, sexp;
    float speed;

    x = pl->x+freearr_x[dir];
    y = pl->y+freearr_y[dir];
    map = pl->map;

    if (!IS_PLAYER(pl))
        return;

    if (!map)
        return;

    if (get_map_flags(map, &map, x, y, &x, &y)&P_OUT_OF_MAP) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, i18n(pl, "You cannot %s here."), skill->slaying);
        return;
    }

    if (!pl->chosen_skill || pl->chosen_skill->skill != skill->skill)
        return;

    trace = object_get_value(pl->chosen_skill, "harvest_race");
    ttool = object_get_value(pl->chosen_skill, "harvest_tool");
    tspeed = object_get_value(pl->chosen_skill, "harvest_speed");
    if (!trace || strcmp(trace, "") == 0 || !ttool || strcmp(ttool, "") == 0 || !tspeed || strcmp(tspeed, "") == 0) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, i18n(pl, "You start to %s, but change your mind."), skill->slaying);
        LOG(llevError, "do_harvest: tool %s without harvest_[race|tool|speed]\n", pl->chosen_skill->name);
        return;
    }

    item = GET_MAP_OB(map, x, y);
    while (item && count < 10) {
        FOR_INV_PREPARE(item, inv) {
            if (object_value_set(inv, "harvestable") == false)
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
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, i18n(pl, "You find nothing to %s here."), skill->slaying);
        return;
    }

    inv = found[rndm(0, count-1)];
    assert(inv);
    item = inv->env;
    assert(item);

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
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, i18n(pl, "You find something, but it is too difficult for you to %s."), skill->slaying);
        return;
    }

    if (level >= skill->level)
        /* Up to 10 more levels, 1 to 11 percent probability. */
        proba = 10+skill->level-level;
    else if (skill->level <= level+10)
        proba = 10+(skill->level-level)*2;
    else
        proba = 30;

    if (proba <= random_roll(0, 100, pl, 1)) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, i18n(pl, "You find something, but fail to %s it."), skill->slaying);
        return;
    }

    /* Check the new item can fit into inventory.
     * Fixes bug #3060474: fishing puts more fishes into inventory than you can carry. */
    if (((uint32_t)(pl->weight + pl->carrying + inv->weight)) > get_weight_limit(pl->stats.Str)) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You are carrying too much to %s a %s.", skill->slaying, inv->name);
        return;
    }

    /* Ok, got it. */
    if (inv->nrof == 0) {
        harvested = object_new();
        object_copy_with_inv(inv, harvested);
    } else {
        if (count == 1 && inv->nrof == 1) {
            check_exhaust = 1;
        }
        harvested = object_split(inv, 1, NULL, 0);
    }
    object_set_value(harvested, "harvestable", NULL, 0);
    if (QUERY_FLAG(harvested, FLAG_MONSTER)) {
        int spot = object_find_free_spot(harvested, pl->map, pl->x, pl->y, 0, SIZEOFFREE);
        if (spot == -1) {
            /* Better luck next time...*/
            object_remove(harvested);
            draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", skill->slaying);
            return;
        }
        object_insert_in_map_at(harvested, pl->map, NULL, 0, pl->x+freearr_x[spot], pl->y+freearr_y[spot]);
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You %s a %s!", skill->slaying, harvested->name);
    } else {
        harvested = object_insert_in_ob(harvested, pl);
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You %s some %s.", skill->slaying, harvested->name);
    }

    /* Get exp */
    change_exp(pl, exp, skill->name, SK_EXP_ADD_SKILL);

    if (check_exhaust) {
        sstring replacement = object_get_value(item, "harvest_exhaust_replacement");
        if (replacement) {
            if (replacement[0] != '-') {
                archetype *other = try_find_archetype(replacement);
                if (other) {
                    object *final = object_create_arch(other);
                    object_insert_in_map_at(final, map, item, INS_BELOW_ORIGINATOR, item->x, item->y);
                }
            }
            object_remove(item);
            object_free(item, FREE_OBJ_FREE_INVENTORY);
        }
    }

    return;
}

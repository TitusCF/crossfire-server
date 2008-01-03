/*
 * static char *rcsid_c_misc_c =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * @file
 * Various functions. Handles misc. input request - things like hash table, malloc, maps,
 * who, etc.
 */

#include <global.h>
#include <loader.h>

#undef SS_STATISTICS
#include <shstr.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <assert.h>
#include <sounds.h>

extern weathermap_t **weathermap;

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

    draw_ext_info_format(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
	i18n_translate(get_language(op),I18N_MSG_CMISC_000),
	i18n_translate(get_language(op),I18N_MSG_CMISC_000),
	  (sec%86400)/3600,(sec%3600)/60,sec%60);

    draw_ext_info(NDI_UNIQUE, 0,op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
	i18n_translate(get_language(op),I18N_MSG_CMISC_001),
	i18n_translate(get_language(op),I18N_MSG_CMISC_002));

    for(m=first_map;m!=NULL;m=m->next) {

	if ( search && strstr(m->path,search)==NULL ) continue; /* Skip unwanted maps */

	/* Print out the last 18 characters of the map name... */
	if (strlen(m->path)<=18) strcpy(map_path, m->path);
	else strcpy(map_path, m->path + strlen(m->path) - 18);

	draw_ext_info_format(NDI_UNIQUE,0,op,
	    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
	    i18n_translate(get_language(op),I18N_MSG_CMISC_003),
	    i18n_translate(get_language(op),I18N_MSG_CMISC_004),
              map_path, m->players,players_on_map(m,FALSE),
              m->in_memory,m->timeout,m->difficulty,
	      (MAP_WHEN_RESET(m)%86400)/3600,(MAP_WHEN_RESET(m)%3600)/60,
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
int command_language(object* op, char* params)
{
    const char* language_str;
    int language;
    int i;
    if (!op->contr)
        return 0;

    language_str = language_names[get_language(op)];

    if (!params||(!strcmp(params, "")))
    {
        draw_ext_info_format(NDI_UNIQUE,0,op,
            MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_005),
            i18n_translate(get_language(op),I18N_MSG_CMISC_005),
            language_str);
        draw_ext_info_format(NDI_UNIQUE,0,op,
            MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_051),
            i18n_translate(get_language(op),I18N_MSG_CMISC_051),
            language_str);
        for(i=0;i<NUM_LANGUAGES; i++)
        {
            draw_ext_info_format(NDI_UNIQUE,0,op,
                MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
                "[fixed]%s: %s",
                "%s: %s",
                language_codes[i],
                language_names[i]);
        }
        return 0;
    }
    else
    {
        char buf[MAX_BUF];
        for(i=0;i<NUM_LANGUAGES; i++)
        {
            if (!strcmp(language_codes[i], params))
            {
                language = i;
                i = NUM_LANGUAGES;
            }
        }
        op->contr->language = language;
        language_str = language_names[language];

        draw_ext_info_format(NDI_UNIQUE,0,op,
            MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(language,I18N_MSG_CMISC_006),
            i18n_translate(language,I18N_MSG_CMISC_006),
            language_str);
        return 0;
    }
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
int command_body(object *op, char *params)
{
    int i;

    /* Too hard to try and make a header that lines everything up, so just
     * give a description.
     */
    draw_ext_info(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	i18n_translate(get_language(op),I18N_MSG_CMISC_007), NULL);

    draw_ext_info(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	i18n_translate(get_language(op),I18N_MSG_CMISC_008), NULL);

    draw_ext_info(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	i18n_translate(get_language(op),I18N_MSG_CMISC_009), NULL);

    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
	/* really debugging - normally body_used should not be set to anything
	 * if body_info isn't also set.
	 */
	if (op->body_info[i] || op->body_used[i]) {
	    draw_ext_info_format(NDI_UNIQUE, 0, op,
		MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
		i18n_translate(get_language(op),I18N_MSG_CMISC_010),
		i18n_translate(get_language(op),I18N_MSG_CMISC_011),
		body_locations[i].use_name, op->body_info[i], op->body_used[i]);
	}
    }
    if (!QUERY_FLAG(op, FLAG_USE_ARMOUR))
	draw_ext_info(NDI_UNIQUE, 0, op,
	    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	    i18n_translate(get_language(op),I18N_MSG_CMISC_012), NULL);
    if (!QUERY_FLAG(op, FLAG_USE_WEAPON))
	draw_ext_info(NDI_UNIQUE, 0, op,
	    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	    i18n_translate(get_language(op),I18N_MSG_CMISC_013), NULL);

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
int command_motd(object *op, char *params)
{
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
int command_rules(object *op, char *params)
{
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
int command_news(object *op, char *params)
{
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
    int ob_used=count_used(),ob_free=count_free(),players,nrofmaps;
    int nrm=0,mapmem=0,anr,anims,sum_alloc=0,sum_used=0,i,tlnr, alnr;
    treasurelist *tl;
    player *pl;
    mapstruct *m;
    archetype *at;
    artifactlist *al;

    for(tl=first_treasurelist,tlnr=0;tl!=NULL;tl=tl->next,tlnr++);
    for(al=first_artifactlist, alnr=0; al!=NULL; al=al->next, alnr++);

    for(at=first_archetype,anr=0,anims=0;at!=NULL;
        at=at->more==NULL?at->next:at->more,anr++);

    for (i=1; i<num_animations; i++)
        anims += animations[i].num_animations;

    for(pl=first_player,players=0;pl!=NULL;pl=pl->next,players++);

    for(m=first_map,nrofmaps=0;m!=NULL;m=m->next,nrofmaps++)
	if(m->in_memory == MAP_IN_MEMORY) {
	    mapmem+=MAP_WIDTH(m)*MAP_HEIGHT(m)*(sizeof(object *)+sizeof(MapSpace));
	    nrm++;
	}

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	i18n_translate(get_language(op),I18N_MSG_CMISC_014),
	i18n_translate(get_language(op),I18N_MSG_CMISC_014),
	sizeof(object),sizeof(player),sizeof(mapstruct));

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
        i18n_translate(get_language(op),I18N_MSG_CMISC_015),
        i18n_translate(get_language(op),I18N_MSG_CMISC_016),
        ob_used,i=(ob_used*sizeof(object)));

    sum_used+=i;
    sum_alloc+=i;
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
        i18n_translate(get_language(op),I18N_MSG_CMISC_017),
        i18n_translate(get_language(op),I18N_MSG_CMISC_018),
        ob_free,i=(ob_free*sizeof(object)));

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
        i18n_translate(get_language(op),I18N_MSG_CMISC_019),
        i18n_translate(get_language(op),I18N_MSG_CMISC_020),
        count_active(), 0);

    sum_alloc+=i;
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
        i18n_translate(get_language(op),I18N_MSG_CMISC_021),
        i18n_translate(get_language(op),I18N_MSG_CMISC_022),
        players,i=(players*sizeof(player)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
        i18n_translate(get_language(op),I18N_MSG_CMISC_023),
        i18n_translate(get_language(op),I18N_MSG_CMISC_024),
        nrofmaps, i=(nrofmaps*sizeof(mapstruct)));

    sum_alloc+=i;
    sum_used+=nrm*sizeof(mapstruct);

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
        i18n_translate(get_language(op),I18N_MSG_CMISC_025),
        i18n_translate(get_language(op),I18N_MSG_CMISC_026),
        nrm,mapmem);

    sum_alloc+=mapmem;
    sum_used+=mapmem;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_027),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_028),
	 anr,i=(anr*sizeof(archetype)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_029),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_030),
	  anims,i=(anims*sizeof(Fontindex)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_031),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_032),
	 tlnr,i=(tlnr*sizeof(treasurelist)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  i18n_translate(get_language(op),I18N_MSG_CMISC_033),
	  i18n_translate(get_language(op),I18N_MSG_CMISC_034),
	  nroftreasures, i=(nroftreasures*sizeof(treasure)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	i18n_translate(get_language(op),I18N_MSG_CMISC_035),
	i18n_translate(get_language(op),I18N_MSG_CMISC_036),
	nrofartifacts, i=(nrofartifacts*sizeof(artifact)));

    sum_alloc+=i;
    sum_used +=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  i18n_translate(get_language(op),I18N_MSG_CMISC_037),
	  i18n_translate(get_language(op),I18N_MSG_CMISC_038),
	  nrofallowedstr, i=(nrofallowedstr*sizeof(linked_char)));

    sum_alloc += i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  i18n_translate(get_language(op),I18N_MSG_CMISC_039),
	  i18n_translate(get_language(op),I18N_MSG_CMISC_040),
	  alnr,i=(alnr*sizeof(artifactlist)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  i18n_translate(get_language(op),I18N_MSG_CMISC_041),
	  i18n_translate(get_language(op),I18N_MSG_CMISC_042),
	  sum_alloc);

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  i18n_translate(get_language(op),I18N_MSG_CMISC_043),
	  i18n_translate(get_language(op),I18N_MSG_CMISC_044),
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

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	i18n_translate(get_language(op),I18N_MSG_CMISC_045),
	i18n_translate(get_language(op),I18N_MSG_CMISC_045),
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

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	"%s (%s) in %s",
	"%s (%s) in %s",
	 m->name, m->path, get_region_longname(get_region_by_map(m)));

    if (QUERY_FLAG(op,FLAG_WIZ)) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		i18n_translate(get_language(op),I18N_MSG_CMISC_046),
		i18n_translate(get_language(op),I18N_MSG_CMISC_046),
		 m->players, m->difficulty,
		 MAP_WIDTH(m), MAP_HEIGHT(m),
		 MAP_ENTER_X(m), MAP_ENTER_Y(m),
		 MAP_TIMEOUT(m));

    }
    if (m->msg)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, m->msg, NULL);
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
int command_malloc_verify(object *op, char *parms)
{
    extern int malloc_verify(void);

    if (!malloc_verify())
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_047), NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		i18n_translate(get_language(op),I18N_MSG_CMISC_048), NULL);

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
    for (reg=first_region;reg!=NULL;reg=reg->next) {
        reg->counter=0;
    }
    for (pl=first_player;pl!=NULL;pl=pl->next)
        if (pl->ob->map!=NULL)
            get_region_by_map(pl->ob->map)->counter++;

     /* we only want to print out by places with a 'longname' field...*/
    for (reg=first_region;reg!=NULL;reg=reg->next) {
	if (reg->longname==NULL && reg->counter>0) {
	    if(reg->parent !=NULL) {
		reg->parent->counter+=reg->counter;
		reg->counter=0;
	    }
	    else /*uh oh, we shouldn't be here. */
		LOG(llevError,"command_whereabouts() Region %s with no longname has no parent\n", reg->name);
	}
    }
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	i18n_translate(get_language(op),I18N_MSG_CMISC_049), NULL);

    for (reg=first_region;reg!=NULL;reg=reg->next)
	if(reg->counter>0) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		 i18n_translate(get_language(op),I18N_MSG_CMISC_050),
		 i18n_translate(get_language(op),I18N_MSG_CMISC_050),
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
static int name_cmp (const chars_names *c1, const chars_names *c2) {
    return strcasecmp (c1->namebuf, c2->namebuf);
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
void list_players(object* op, region* reg, partylist* party) {
    player *pl;
    uint16 i;
    char* format;
    int num_players = 0, num_wiz = 0, num_afk = 0, num_bot = 0;
    chars_names *chars = NULL;

    if (op == NULL || QUERY_FLAG(op, FLAG_WIZ))
        format=settings.who_wiz_format;
    else
        format=settings.who_format;

    for (pl=first_player;pl!=NULL;pl=pl->next) {
        if (pl->ob->map == NULL)
            continue;
        if (pl->hidden && !QUERY_FLAG(op, FLAG_WIZ))
            continue;

        if(reg && !region_is_child_of_region(get_region_by_map(pl->ob->map),reg))
            continue;
        if (party && pl->party != party)
            continue;

        if (pl->state==ST_PLAYING || pl->state==ST_GET_PARTY_PASSWORD) {

            num_players++;
            chars = (chars_names *) realloc(chars, num_players*sizeof(chars_names));
            if (chars == NULL) {
                draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                              i18n_translate(get_language(op),I18N_MSG_CMISC_052), NULL);
                return;
            }
            sprintf(chars[num_players-1].namebuf, "%s", pl->ob->name);
            chars[num_players-1].login_order = num_players;

            /* Check for WIZ's & AFK's*/
            if (QUERY_FLAG(pl->ob,FLAG_WIZ))
                num_wiz++;

            if (QUERY_FLAG(pl->ob,FLAG_AFK))
                num_afk++;

            if (pl->socket.is_bot)
                num_bot++;
        }
    }
    if (first_player != (player *) NULL) {
        if (reg == NULL && party == NULL)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                i18n_translate(get_language(op),I18N_MSG_CMISC_053),
                i18n_translate(get_language(op),I18N_MSG_CMISC_053),
                num_players, num_wiz, num_afk, num_bot);
        else if (party == NULL)
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                i18n_translate(get_language(op),I18N_MSG_CMISC_054),
                i18n_translate(get_language(op),I18N_MSG_CMISC_054),
                reg->longname?reg->longname:reg->name, num_players, num_wiz, num_afk, num_bot);
        else
            draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
                i18n_translate(get_language(op),I18N_MSG_CMISC_055),
                i18n_translate(get_language(op),I18N_MSG_CMISC_055),
                party->partyname, num_players, num_wiz, num_afk, num_bot);
    }
    qsort (chars, num_players, sizeof(chars_names), (int (*)(const void *, const void *))name_cmp);
    for (i=0;i<num_players;i++)
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
int command_who (object *op, char *params) {
    region *reg;
    reg=get_region_from_string(params);

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
    char outbuf[MAX_BUF], outbuf1[MAX_BUF];
    size_t i;

    strcpy(outbuf, "[fixed]");
    outbuf1[0] = '\0';

    if (pl==NULL) {
    	LOG(llevError,"display_who_entry(): I was passed a null player\n");
	return;
    }
    for (i=0;i<=strlen(format);i++) {
        if (format[i]=='%') {
            i++;
            get_who_escape_code_value(tmpbuf,sizeof(tmpbuf), format[i],pl);
            strcat(outbuf, tmpbuf);
            strcat(outbuf1, tmpbuf);
        }
        else if (format[i]=='_') {
            strcat(outbuf," "); /* allow '_' to be used in place of spaces */
            strcat(outbuf1," "); /* allow '_' to be used in place of spaces */
	}
        else {
            sprintf(tmpbuf,"%c",format[i]);
            strcat(outbuf,tmpbuf);
            strcat(outbuf1,tmpbuf);
        }
    }
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO, outbuf, outbuf1);
}

/**
 * Returns the value of the escape code used in the who format specifier.
 *
 * Specifier values are:
 * - N	Name of character
 * - t	title of character
 * - T	the optional "the " sequence value (depend if player has own_title or not)
 * - c	count
 * - n	newline
 * - h	\<Hostile\> if character is hostile, nothing otherwise
 * - d	\<WIZ\> if character is a dm, nothing otherwise
 * - a	\<AFK\> if character is afk, nothing otherwise
 * - b	\<BOT\> if character is a bot, nothing otherwise
 * - l	the level of the character
 * - m	the map path the character is currently on
 * - M	the map name of the map the character is currently on
 * - r	the region name (eg scorn, wolfsburg)
 * - R	the regional title (eg The Kingdom of Scorn, The Port of Wolfsburg)
 * - i	player's ip address
 * - %	a literal %
 * - _	a literal underscore
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
	case 'N' :	snprintf(return_val, size, pl->ob->name);
			break;

	case 't' :	snprintf(return_val,size, (pl->own_title[0]=='\0'?pl->title:pl->own_title));
			break;

	case 'T' :	if (pl->own_title[0]=='\0')
			    snprintf(return_val, size, "the ");
			else
			    *return_val='\0';
			break;

	case 'c' :	snprintf(return_val, size, "%d",pl->ob->count);
			break;

	case 'n' :	snprintf(return_val, size, "\n");
			break;

	case 'h' :	snprintf(return_val, size, pl->peaceful?"":" <Hostile>");
			break;

	case 'l' :	snprintf(return_val, size, "%d",pl->ob->level);
			break;

	case 'd' :	snprintf(return_val, size, (QUERY_FLAG(pl->ob,FLAG_WIZ)?" <WIZ>":""));
			break;

	case 'a' :	snprintf(return_val, size, (QUERY_FLAG(pl->ob,FLAG_AFK)?" <AFK>":""));
			break;

	case 'b' :	snprintf(return_val, size, (pl->socket.is_bot == 1)?" <BOT>":"");
			break;

	case 'm' :	snprintf(return_val, size, pl->ob->map->path);
			break;

	case 'M' :	snprintf(return_val, size, pl->ob->map->name?pl->ob->map->name:"Untitled");
			break;

	case 'r' :	snprintf(return_val, size, get_name_of_region_for_map(pl->ob->map));
			break;

	case 'R' :	snprintf(return_val, size, get_region_longname(get_region_by_map(pl->ob->map)));
			break;

	case 'i' :	snprintf(return_val, size, pl->socket.host);
			break;

	case '%' :	snprintf(return_val, size, "%%");
			break;

	case '_' :	snprintf(return_val, size, "_");
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
int command_afk (object *op, char *params)
{
    if QUERY_FLAG(op,FLAG_AFK) {
	CLEAR_FLAG(op,FLAG_AFK);
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_056), NULL);
    }
    else
    {
	SET_FLAG(op,FLAG_AFK);
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_057), NULL);
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
int command_malloc (object *op, char *params)
{
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
int command_mapinfo (object *op, char *params)
{
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
int command_whereami (object *op, char *params)
{
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
int command_maps (object *op, char *params)
{
    map_info(op,params);
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
int command_strings (object *op, char *params)
{
    char stats[HUGE_BUF];
    ss_dump_statistics(stats, sizeof(stats));
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_LAST,
		"[fixed]%s\n",
		"%s",
		  stats);

    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_LAST,
		  ss_dump_table(SS_DUMP_TOTALS, stats, sizeof(stats)), NULL);
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
int command_time (object *op, char *params)
{
    time_info(op);
    return 1;
}

/**
 * Player is wondering about the weather.
 *
 * @param op
 * player asking for information.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_weather (object *op, char *params)
{
    int wx, wy, temp, sky;
    char buf[MAX_BUF];

    if (settings.dynamiclevel < 1) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
            i18n_translate(get_language(op),I18N_MSG_CMISC_058), NULL);
        return 1;
    }

    if (op->map == NULL)
	return 1;

    if (worldmap_to_weathermap(op->x, op->y, &wx, &wy, op->map) != 0) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
            i18n_translate(get_language(op),I18N_MSG_CMISC_059), NULL);
        return 1;
    }

    if (QUERY_FLAG(op, FLAG_WIZ)) {
	/* dump the weather, Dm style! Yo! */

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_060),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_060),
	     real_world_temperature(op->x, op->y, op->map));

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_061),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_061),
	     weathermap[wx][wy].temp);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_062),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_062),
	     weathermap[wx][wy].humid);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_063),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_063),
	     weathermap[wx][wy].winddir, weathermap[wx][wy].windspeed);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_064),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_064),
	     weathermap[wx][wy].pressure);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_065),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_065),
	     weathermap[wx][wy].avgelev);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_066),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_066),
	     weathermap[wx][wy].rainfall, weathermap[wx][wy].water);
    }

    temp = real_world_temperature(op->x, op->y, op->map);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_067),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_067),
	 temp);

    /* humid */
    if (weathermap[wx][wy].humid < 20)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_068), NULL);
    else if (weathermap[wx][wy].humid < 40)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_069), NULL);
    else if (weathermap[wx][wy].humid < 60)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_070), NULL);
    else if (weathermap[wx][wy].humid < 80)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_071), NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_072), NULL);

    /* wind */
    switch (weathermap[wx][wy].winddir) {
	case 1: strcpy(buf, "north");	    break;
	case 2: strcpy(buf, "northeast");   break;
	case 3: strcpy(buf, "east");	    break;
	case 4: strcpy(buf, "southeast");   break;
	case 5: strcpy(buf, "south");	    break;
	case 6: strcpy(buf, "southwest");   break;
	case 7: strcpy(buf, "west");	    break;
	case 8: strcpy(buf, "northwest");   break;
    }
    if (weathermap[wx][wy].windspeed < 5)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_073),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_073),
	     buf);
    else if (weathermap[wx][wy].windspeed < 10)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	      i18n_translate(get_language(op),I18N_MSG_CMISC_074),
	      i18n_translate(get_language(op),I18N_MSG_CMISC_074),
	      buf);
    else if (weathermap[wx][wy].windspeed < 15)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_075),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_075),
	     buf);
    else if (weathermap[wx][wy].windspeed < 25)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_076),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_076),
	     buf);
    else if (weathermap[wx][wy].windspeed < 35)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_077),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_077),
	     buf);
    else
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_078),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_078),
	     buf);

    sky = weathermap[wx][wy].sky;
    if (temp <= 0 && sky > SKY_OVERCAST && sky < SKY_FOG)
	sky += 10; /*let it snow*/

    switch (sky) {
	case SKY_CLEAR:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_079), NULL);
	    break;
	case SKY_LIGHTCLOUD:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_080), NULL);
	    break;
	case SKY_OVERCAST:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			   i18n_translate(get_language(op),I18N_MSG_CMISC_081), NULL);
	    break;
	case SKY_LIGHT_RAIN:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_082), NULL);
	    break;
	case SKY_RAIN:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_083), NULL);
	    break;
	case SKY_HEAVY_RAIN:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_084), NULL);
	    break;
	case SKY_HURRICANE:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_085), NULL);
	    break;
	case SKY_FOG:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			   i18n_translate(get_language(op),I18N_MSG_CMISC_086), NULL);
	    break;
	case SKY_HAIL:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_087), NULL);
	    break;
	case SKY_LIGHT_SNOW:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_088), NULL);
	    break;
	case SKY_SNOW:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_089), NULL);
	    break;
	case SKY_HEAVY_SNOW:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_090), NULL);
	    break;
	case SKY_BLIZZARD:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_091), NULL);
	    break;
    }
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
int command_archs (object *op, char *params)
{
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
int command_hiscore (object *op, char *params)
{
    display_high_score(op,op==NULL?9999:50, params);
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
int command_debug (object *op, char *params)
{
    int i;

    if(params==NULL || !sscanf(params, "%d", &i)) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		 i18n_translate(get_language(op),I18N_MSG_CMISC_092),
		 i18n_translate(get_language(op),I18N_MSG_CMISC_092),
		 settings.debug);
	return 1;
    }
    if(op != NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_093), NULL);
	return 1;
    }
    settings.debug = (enum LogLevel) FABS(i);
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		 i18n_translate(get_language(op),I18N_MSG_CMISC_094),
		 i18n_translate(get_language(op),I18N_MSG_CMISC_094),
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
int command_dumpbelow (object *op, char *params)
{
    if (op && op->below) {
        char buf[HUGE_BUF];
	dump_object(op->below, buf, sizeof(buf));
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, buf, NULL);

	/* Let's push that item on the dm's stack */
	dm_stack_push( op->contr, op->below->count );
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
int command_wizpass (object *op, char *params)
{
    int i;

    if (!op)
	return 0;

    if (!params)
	i = (QUERY_FLAG(op, FLAG_WIZPASS)) ? 0 : 1;
    else
	i =onoff_value(params);

    if (i) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_095), NULL);
	SET_FLAG(op, FLAG_WIZPASS);
    } else {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_096), NULL);
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
int command_wizcast (object *op, char *params)
{
    int i;

    if (!op)
	return 0;

    if (!params)
	i = (QUERY_FLAG(op, FLAG_WIZCAST)) ? 0 : 1;
    else
	i = onoff_value(params);

    if (i) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_097), NULL);
	SET_FLAG(op, FLAG_WIZCAST);
    } else {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_098), NULL);
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
int command_dumpallobjects (object *op, char *params)
{
    dump_all_objects();
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
int command_dumpfriendlyobjects (object *op, char *params)
{
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
int command_dumpallarchetypes (object *op, char *params)
{
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
int command_ssdumptable (object *op, char *params)
{
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
int command_dumpmap (object *op, char *params)
{
    if(op)
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
int command_dumpallmaps (object *op, char *params)
{
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
int command_printlos (object *op, char *params)
{
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
int command_version (object *op, char *params)
{
    version(op);
    return 0;
}


/**
 * Output-sync command.
 *
 * @param op
 * player asking for information.
 * @param params
 * new value.
 * @return
 * 1.
 */
int command_output_sync(object *op, char *params)
{
    int val;

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_099),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_099),
	     op->contr->outputs_sync);
	return 1;
    }
    val=atoi(params);
    if (val>0) {
	op->contr->outputs_sync = val;
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_100),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_100),
	     op->contr->outputs_sync);
    }
    else
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_101), NULL);

    return 1;
}

/**
 * output-count command.
 *
 * @param op
 * player asking for information.
 * @param params
 * new value.
 * @return
 * 1.
 */
int command_output_count(object *op, char *params)
{
    int val;

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_102),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_102),
	     op->contr->outputs_count);
	return 1;
    }
    val=atoi(params);
    if (val>0) {
	op->contr->outputs_count = val;
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_103),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_103),
	     op->contr->outputs_count);
    }
    else
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_104), NULL);

    return 1;
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
int command_listen (object *op, char *params)
{
    int i;

    if(params==NULL || !sscanf(params, "%d", &i)) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_105),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_105),
	     op->contr->listening);
	return 1;
    }
    if (i < 0) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_106), NULL);
        return 1;
    }
    op->contr->listening=(char) i;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_107),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_107),
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
int command_statistics(object *pl, char *params)
{
    char buf[MAX_BUF];
    char buf2[MAX_BUF];

    if (!pl->contr) return 1;
    strcpy(buf, i18n_translate(get_language(pl),I18N_MSG_CMISC_108));
    strcpy(buf2, i18n_translate(get_language(pl),I18N_MSG_CMISC_109));
    strcat(buf, FMT64);
    strcat(buf2, FMT64);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 buf,
			 buf2,
			 pl->stats.exp);
    strcpy(buf, i18n_translate(get_language(pl),I18N_MSG_CMISC_110));
    strcpy(buf2, i18n_translate(get_language(pl),I18N_MSG_CMISC_111));
    strcat(buf, FMT64);
    strcat(buf2, FMT64);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 buf,
			 buf2,
			 level_exp(pl->level+1, pl->expmul));

    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
		  i18n_translate(get_language(pl),I18N_MSG_CMISC_112),
		  i18n_translate(get_language(pl),I18N_MSG_CMISC_113));

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_114),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_115),
			 pl->contr->orig_stats.Str, pl->stats.Str, 20+pl->arch->clone.stats.Str);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_116),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_117),
			 pl->contr->orig_stats.Dex, pl->stats.Dex, 20+pl->arch->clone.stats.Dex);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_118),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_119),
			 pl->contr->orig_stats.Con, pl->stats.Con, 20+pl->arch->clone.stats.Con);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_120),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_121),
			 pl->contr->orig_stats.Int, pl->stats.Int, 20+pl->arch->clone.stats.Int);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_122),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_123),
			 pl->contr->orig_stats.Wis, pl->stats.Wis, 20+pl->arch->clone.stats.Wis);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_124),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_125),
			 pl->contr->orig_stats.Pow, pl->stats.Pow, 20+pl->arch->clone.stats.Pow);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_126),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_127),
			 pl->contr->orig_stats.Cha, pl->stats.Cha, 20+pl->arch->clone.stats.Cha);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_128),
			 i18n_translate(get_language(pl),I18N_MSG_CMISC_129),
			 pl->contr->peaceful? "Peaceful":"Hostile");

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
int command_fix_me(object *op, char *params)
{
    sum_weight(op);
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
int command_players(object *op, char *params)
{
    char buf[MAX_BUF];
    char *t;
    DIR *Dir;

    sprintf(buf,"%s/%s/",settings.localdir,settings.playerdir);
    t=buf+strlen(buf);
    if ((Dir=opendir(buf))!=NULL) {
	const struct dirent *Entry;

	while ((Entry=readdir(Dir))!=NULL) {
	    /* skip '.' , '..' */
	    if (!((Entry->d_name[0]=='.' && Entry->d_name[1]=='\0') ||
		(Entry->d_name[0]=='.' && Entry->d_name[1]=='.' && Entry->d_name[2]=='\0')))
	    {
		struct stat Stat;

		strcpy(t,Entry->d_name);
		if (stat(buf,&Stat)==0) {
		    /* This was not posix compatible
             * if ((Stat.st_mode & S_IFMT)==S_IFDIR) {
             */
            if (S_ISDIR(Stat.st_mode)){
			struct tm *tm=localtime(&Stat.st_mtime);

			draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			          i18n_translate(get_language(op),I18N_MSG_CMISC_130),
			          i18n_translate(get_language(op),I18N_MSG_CMISC_131),
				  Entry->d_name,
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
    closedir(Dir);
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
int command_applymode(object *op, char *params)
{
    unapplymode unapply = op->contr->unapply;
    static const char* const types[]={"nochoice", "never", "always"};

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_132),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_133),
	     types[op->contr->unapply]);
	return 1;
    }

    if (!strcmp(params,"nochoice"))
	op->contr->unapply=unapply_nochoice;
    else if (!strcmp(params,"never"))
	op->contr->unapply=unapply_never;
    else if (!strcmp(params,"always"))
	op->contr->unapply=unapply_always;
    else {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	     i18n_translate(get_language(op),I18N_MSG_CMISC_134),
	     i18n_translate(get_language(op),I18N_MSG_CMISC_134),
	     params);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_135),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_135),
	 (unapply==op->contr->unapply?"":" now"),
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
int command_bowmode(object *op, char *params)
{
    bowtype_t oldtype=op->contr->bowtype;
    static const char* const types[] =
	{"normal", "threewide", "spreadshot", "firenorth",
	 "firene", "fireeast", "firese", "firesouth",
	 "firesw", "firewest", "firenw", "bestarrow"};
    char buf[MAX_BUF];
    int i, found;

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			     i18n_translate(get_language(op),I18N_MSG_CMISC_136),
			     i18n_translate(get_language(op),I18N_MSG_CMISC_136),
			     types[op->contr->bowtype]);
	return 1;
    }

    for (i=0,found=0; i<=bow_bestarrow; i++) {
	if (!strcmp(params, types[i])) {
	    found++;
	    op->contr->bowtype=i;
	    break;
	}
    }
    if (!found) {
	sprintf(buf, "bowmode: Unknown options %s, valid options are:", params);
	for (i=0; i<=bow_bestarrow; i++) {
	    strcat(buf, " ");
	    strcat(buf, types[i]);
	    if (i < bow_nw)
		strcat(buf, ",");
	    else
		strcat(buf, ".");
	}
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG, buf, NULL);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		 i18n_translate(get_language(op),I18N_MSG_CMISC_137),
		 i18n_translate(get_language(op),I18N_MSG_CMISC_137),
		 (oldtype==op->contr->bowtype?"":"now"),
		 types[op->contr->bowtype]);
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
int command_petmode(object *op, char *params)
{
    petmode_t oldtype=op->contr->petmode;
    static const char* const types[]={"normal", "sad", "defend", "arena"};

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		     i18n_translate(get_language(op),I18N_MSG_CMISC_138),
		     i18n_translate(get_language(op),I18N_MSG_CMISC_138),
		     types[op->contr->petmode]);
	return 1;
    }

    if (!strcmp(params,"normal"))
	op->contr->petmode=pet_normal;
    else if (!strcmp(params,"sad"))
	op->contr->petmode=pet_sad;
    else if (!strcmp(params,"defend"))
	op->contr->petmode=pet_defend;
    else if (!strcmp(params,"arena"))
	op->contr->petmode=pet_arena;
    else {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	    i18n_translate(get_language(op),I18N_MSG_CMISC_139),
	    i18n_translate(get_language(op),I18N_MSG_CMISC_139),
	     params);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_140),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_140),
	(oldtype==op->contr->petmode?"":"now"),
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
int command_showpets(object *op, char *params)
{
    objectlink *obl, *next;
    int counter=0, target=0;
    int have_shown_pet=0;
    if (params !=NULL) target= atoi(params);

    for (obl = first_friendly_object; obl != NULL; obl = next) {
    	object *ob = obl->ob;
    	next = obl->next;
    	if (get_owner(ob) == op) {
	    if (target ==0) {
	    	if (counter==0)
		    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				  i18n_translate(get_language(op),I18N_MSG_CMISC_141), NULL);
		draw_ext_info_format(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			     i18n_translate(get_language(op),I18N_MSG_CMISC_142),
			     i18n_translate(get_language(op),I18N_MSG_CMISC_142),
			     ++counter, ob->name, ob->level );
	    }
	    else if (!have_shown_pet && ++counter==target) {
	    	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_143),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_144),
				     ob->level, ob->name);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_145),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_146),
				     ob->stats.hp, ob->stats.maxhp, ob->stats.sp, ob->stats.maxsp);

		/* this is not a nice way to do this, it should be made to be more like the statistics command */
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_147),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_148),
				     ob->stats.Str);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_149),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_150),
				     ob->stats.Dex);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_151),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_152),
				     ob->stats.Con);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_153),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_154),
				     ob->stats.Int);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_155),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_156),
				     ob->stats.Wis);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_157),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_158),
				     ob->stats.Cha);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_159),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_160),
				     ob->stats.Pow);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     i18n_translate(get_language(op),I18N_MSG_CMISC_161),
				     i18n_translate(get_language(op),I18N_MSG_CMISC_162),
				     ob->stats.wc, ob->stats.dam, ob->stats.ac);
		have_shown_pet=1;
	    }
	}
    }
    if (counter == 0)
    	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_163), NULL);
    else if (target !=0 && have_shown_pet==0)
    	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_164), NULL);
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
int command_usekeys(object *op, char *params)
{
    usekeytype oldtype=op->contr->usekeys;
    static const char* const types[]={"inventory", "keyrings", "containers"};

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		     i18n_translate(get_language(op),I18N_MSG_CMISC_165),
		     i18n_translate(get_language(op),I18N_MSG_CMISC_165),
		     types[op->contr->usekeys]);
	return 1;
    }

    if (!strcmp(params,"inventory"))
	op->contr->usekeys=key_inventory;
    else if (!strcmp(params,"keyrings"))
	op->contr->usekeys=keyrings;
    else if (!strcmp(params,"containers"))
	op->contr->usekeys=containers;
    else {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	    i18n_translate(get_language(op),I18N_MSG_CMISC_166),
	    i18n_translate(get_language(op),I18N_MSG_CMISC_166),
	     params);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_167),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_167),
	(oldtype==op->contr->usekeys?"":"now"),
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
int command_resistances(object *op, char *params)
{
    int i;
    if (!op)
	return 0;

    for (i=0; i<NROFATTACKS; i++) {
	if (i==ATNR_INTERNAL) continue;

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			     i18n_translate(get_language(op),I18N_MSG_CMISC_168),
			     i18n_translate(get_language(op),I18N_MSG_CMISC_169),
			     attacktype_desc[i], op->resist[i]);
    }

    /* If dragon player, let's display natural resistances */
    if ( is_dragon_pl( op ) ) {
        int attack;
        object* tmp;

        for ( tmp = op->inv; tmp != NULL; tmp = tmp->below ) {
            if ( ( tmp->type == FORCE ) && ( strcmp( tmp->arch->name, "dragon_skin_force" )== 0 ) ) {
                draw_ext_info( NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			      i18n_translate(get_language(op),I18N_MSG_CMISC_170), NULL);

                for ( attack = 0; attack < NROFATTACKS; attack++ ) {
                    if ( atnr_is_dragon_enabled( attack ) ) {
                        draw_ext_info_format( NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
					     i18n_translate(get_language(op),I18N_MSG_CMISC_171),
					     i18n_translate(get_language(op),I18N_MSG_CMISC_171),
					     change_resist_msg[ attack ], tmp->resist[ attack ] );
		    }
		}
		break;
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
static void help_topics(object *op, int what)
{
    DIR *dirp;
    struct dirent *de;
    char filename[MAX_BUF], line[HUGE_BUF];
    char suffix[MAX_BUF];
    int namelen;
    const char* language;

    language = language_codes[get_language(op)];
    sprintf(suffix, ".%s", language);

    switch (what) {
        case 1:
            sprintf(filename, "%s/wizhelp", settings.datadir);
            draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                i18n_translate(get_language(op),I18N_MSG_CMISC_172), NULL);
            break;
        case 3:
            sprintf(filename, "%s/mischelp", settings.datadir);
            draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                    i18n_translate(get_language(op),I18N_MSG_CMISC_173), NULL);
            break;
        default:
            sprintf(filename, "%s/help", settings.datadir);
            draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
                    i18n_translate(get_language(op),I18N_MSG_CMISC_174), NULL);
            break;
    }
    if (!(dirp=opendir(filename)))
        return;

    line[0] ='\0';
    for (de = readdir(dirp); de; de = readdir(dirp)) {
        namelen = NAMLEN(de);

        if (namelen <= 2 && *de->d_name == '.' &&
            (namelen == 1 || de->d_name[1] == '.' ) )
            continue;
        if (strstr(de->d_name,suffix))
        {
            strcat(line, strtok(de->d_name,"."));
            strcat(line, " ");
        }
    }
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
        line, line);
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
static void show_commands(object *op, int what)
{
    char line[HUGE_BUF];
    int i, size, namelen;
    command_array_struct *ap;
    extern command_array_struct Commands[], WizCommands[];
    extern const int CommandsSize, WizCommandsSize;

    switch (what) {
	case 1:
	    ap =WizCommands;
	    size =WizCommandsSize;
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_175), NULL);
	    break;

	case 2:
	    ap= CommunicationCommands;
	    size= CommunicationCommandSize;
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_176), NULL);
	    break;

	default:
	    ap =Commands;
	    size =CommandsSize;
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_177), NULL);
	    break;
    }

    line[0] ='\0';
    for (i=0; i<size; i++) {
	namelen = strlen(ap[i].name);

	strcat(line, ap[i].name);
	strcat(line, " ");
    }
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line, line);
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
int command_help (object *op, char *params)
{
    struct stat st;
    FILE *fp;
    char filename[MAX_BUF], line[MAX_BUF];
    int len;
    const char* language;

    language = language_codes[get_language(op)];

    /*
     * Main help page?
     */

    if (!params) {
	sprintf(filename, "%s/def_help", settings.datadir);
	if ((fp=fopen(filename, "r")) == NULL) {
	    LOG(llevError, "Cannot open help file %s: %s\n", filename, strerror_local(errno, line, sizeof(line)));
	    return 0;
	}
	while (fgets(line, MAX_BUF, fp)) {
	    line[MAX_BUF-1] ='\0';
	    len =strlen(line)-1;
	    if (line[len] == '\n')
		line[len] ='\0';
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line, NULL);
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
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "\n", NULL);
	show_commands(op, 2); /* show comm commands */
	if (QUERY_FLAG(op, FLAG_WIZ)) {
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "\n", NULL);
	    show_commands(op, 1);
	}
	return 0;
    }

    /*
     * User wants info about command
     */
    if (strchr(params, '.') || strchr(params, ' ') || strchr(params, '/')) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			     i18n_translate(get_language(op),I18N_MSG_CMISC_178),
			     i18n_translate(get_language(op),I18N_MSG_CMISC_178),
			     params);
	return 0;
    }

    sprintf(filename, "%s/mischelp/%s.%s", settings.datadir, params, language);
    if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
	if (op) {
	    sprintf(filename, "%s/help/%s.%s", settings.datadir, params, language);
	    if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
		if (QUERY_FLAG(op, FLAG_WIZ)) {
		    sprintf(filename, "%s/wizhelp/%s.%s", settings.datadir, params, language);
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
    if ((fp=fopen(filename, "r")) == NULL) {
	LOG(llevError, "Cannot open help file %s: %s\n", filename, strerror_local(errno, line, sizeof(line)));
	return 0;
    }

  draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
		       i18n_translate(get_language(op),I18N_MSG_CMISC_179),
		       i18n_translate(get_language(op),I18N_MSG_CMISC_179),
		       params);

    while (fgets(line, MAX_BUF, fp)) {
	line[MAX_BUF-1] ='\0';
	len =strlen(line)-1;
	if (line[len] == '\n')
	    line[len] ='\0';
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line, NULL);
    }
    fclose(fp);
    return 0;

    /*
     * No_help -escape
     */

    nohelp:

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
	 i18n_translate(get_language(op),I18N_MSG_CMISC_180),
	 i18n_translate(get_language(op),I18N_MSG_CMISC_180),
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
int onoff_value(const char *line)
{
    int i;

    if (sscanf(line, "%d", &i))
	return (i != 0);

    switch (line[0]) {
	case 'o':
	    switch (line[1]) {
		case 'n': return 1;		/* on */
		default:  return 0;		/* o[ff] */
	    }
	case 'y':			/* y[es] */
	case 'k':			/* k[ylla] */
	case 's':
	case 'd':
	    return 1;

	case 'n':			/* n[o] */
	case 'e':			/* e[i] */
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
int command_quit (object *op, char *params)
{
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,
	       i18n_translate(get_language(op),I18N_MSG_CMISC_181));

    op->contr->state = ST_CONFIRM_QUIT;
    return 1;
}

/**
 * Player wants to enter explore mode, that is never-dying mode.
 *
 * Don't allow people to exit explore mode.  It otherwise becomes
 * really easy to abuse this.
 *
 * @param op
 * player asking for explore mode.
 * @param params
 * unused.
 * @return
 * 1.
 */
int command_explore (object *op, char *params)
{
    if (settings.explore_mode == FALSE) {
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_182), NULL);
        return 1;
    }
    /*
     * I guess this is the best way to see if we are solo or not.  Actually,
     * are there any cases when first_player->next==NULL and we are not solo?
     */
    if ((first_player!=op->contr) || (first_player->next!=NULL)) {
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_183), NULL);
    } else if (op->contr->explore)
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_184), NULL);
    else {
        op->contr->explore=1;
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            i18n_translate(get_language(op),I18N_MSG_CMISC_185), NULL);
    }
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
int command_sound (object *op, char *params)
{
    if (!(op->contr->socket.sound & SND_MUTE)) {
        op->contr->socket.sound=op->contr->socket.sound | SND_MUTE;
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_186), NULL);
    }
    else {
        op->contr->socket.sound=op->contr->socket.sound & ~SND_MUTE;
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_187), NULL);
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

    if(!check_name(op->contr,op->contr->write_buf+1)) {
	get_name(op);
	return;
    }
    FREE_AND_COPY(op->name, op->contr->write_buf+1);
    FREE_AND_COPY(op->name_pl, op->contr->write_buf+1);
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, op->contr->write_buf, NULL);
    op->contr->name_changed=1;
    get_password(op);
}

/**
 * A player just entered her password, including for changing it.
 *
 * @param op
 * player.
 */
void receive_player_password(object *op) {

    unsigned int pwd_len=strlen(op->contr->write_buf);

    if(pwd_len<=1||pwd_len>17) {
        if (op->contr->state == ST_CHANGE_PASSWORD_OLD || op->contr->state == ST_CHANGE_PASSWORD_NEW || op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_188), NULL);
            op->contr->state = ST_PLAYING;
        }
        else
            get_name(op);
        return;
    }
    /* To hide the password better */
    /* With currently clients, not sure if this is really the case - MSW */
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,"          ", NULL);

    if (checkbanned(op->name, op->contr->socket.host)) {
        LOG(llevInfo, "Banned player tried to add: [%s@%s]\n", op->name, op->contr->socket.host);
        draw_ext_info(NDI_UNIQUE|NDI_RED, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_189), NULL);
        get_name(op);
        return;
    }

    if(op->contr->state==ST_CONFIRM_PASSWORD) {
        if(!check_password(op->contr->write_buf+1,op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0,op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_190), NULL);
            get_name(op);
            return;
        }
        LOG(llevInfo,"LOGIN: New player named %s from ip %s\n", op->name, op->contr->socket.host);
        display_motd(op);
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_191), NULL);
        roll_again(op);
        op->contr->state=ST_ROLL_STAT;
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_OLD) {
        if (!check_password(op->contr->write_buf + 1, op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_192), NULL);
            op->contr->state = ST_PLAYING;
        } else {
            send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, i18n_translate(get_language(op),I18N_MSG_CMISC_193));
            op->contr->state = ST_CHANGE_PASSWORD_NEW;
        }
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_NEW) {
        strcpy(op->contr->new_password, crypt_string(op->contr->write_buf + 1, NULL));
        send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, i18n_translate(get_language(op),I18N_MSG_CMISC_194));
        op->contr->state = ST_CHANGE_PASSWORD_CONFIRM;
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
        if (strcmp(crypt_string(op->contr->write_buf + 1, op->contr->new_password), op->contr->new_password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_195), NULL);
        } else {
        	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			      i18n_translate(get_language(op),I18N_MSG_CMISC_196), NULL);
            strncpy(op->contr->password, op->contr->new_password, 13);
        }
        op->contr->state = ST_PLAYING;
        return;
    }

    strcpy(op->contr->password,crypt_string(op->contr->write_buf+1,NULL));
    op->contr->state=ST_ROLL_STAT;
    check_login(op);
    return;
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
int command_title (object *op, char *params)
{
    char buf[MAX_BUF];

    if (settings.set_title == FALSE) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_197), NULL);
	return 1;
    }

    /* dragon players cannot change titles */
    if (is_dragon_pl(op)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_198), NULL);
        return 1;
    }

    if(params == NULL) {
	if(op->contr->own_title[0]=='\0')
	    sprintf(buf,i18n_translate(get_language(op),I18N_MSG_CMISC_199), op->contr->title);
	else
	    sprintf(buf,i18n_translate(get_language(op),I18N_MSG_CMISC_200), op->contr->own_title);
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,buf, NULL);
	return 1;
    }
    if(strcmp(params, "clear")==0 || strcmp(params, "default")==0) {
	if(op->contr->own_title[0]=='\0')
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_201), NULL);
	else
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_202), NULL);
	op->contr->own_title[0]='\0';
	return 1;
    }

    if((int)strlen(params) >= MAX_NAME) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_203), NULL);
	return 1;
    }
    strcpy(op->contr->own_title, params);
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
int command_save (object *op, char *params)
{
    if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL) & P_NO_CLERIC) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_204), NULL);
    } else if (!op->stats.exp) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_205), NULL);
    } else {
	if(save_player(op,1))
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_206), NULL);
	else
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_207), NULL);
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
int command_peaceful (object *op, char *params)
{
    if((op->contr->peaceful=!op->contr->peaceful))
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_208), NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_209), NULL);
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
int command_wimpy (object *op, char *params)
{
    int i;

    if (params==NULL || !sscanf(params, "%d", &i)) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			     i18n_translate(get_language(op),I18N_MSG_CMISC_210),
			     i18n_translate(get_language(op),I18N_MSG_CMISC_210),
			     op->run_away);
	return 1;
    }
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		 i18n_translate(get_language(op),I18N_MSG_CMISC_211),
		 i18n_translate(get_language(op),I18N_MSG_CMISC_211),
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
int command_brace (object *op, char *params)
{
    if (!params)
	op->contr->braced =!op->contr->braced;
    else
	op->contr->braced =onoff_value(params);

    if(op->contr->braced)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_212), NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_213), NULL);

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
int command_kill_pets(object *op, char *params)
{
    objectlink *obl, *next;
    int counter=0, removecount=0;
    if (params == NULL) {
    	terminate_all_pets(op);
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      i18n_translate(get_language(op),I18N_MSG_CMISC_214), NULL);
    }
    else {
	int target = atoi(params);
	for(obl = first_friendly_object; obl != NULL; obl = next) {
	    object *ob = obl->ob;
	    next = obl->next;
	    if (get_owner(ob) == op)
	    	if (++counter==target || (target==0 && !strcasecmp(ob->name, params)))  {
		if (!QUERY_FLAG(ob, FLAG_REMOVED))
		    remove_ob(ob);
		remove_friendly_object(ob);
		free_object(ob);
		removecount++;
            }
	}
	if (removecount!=0)
	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			 i18n_translate(get_language(op),I18N_MSG_CMISC_215),
			 i18n_translate(get_language(op),I18N_MSG_CMISC_215),
			 removecount);
	else
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  i18n_translate(get_language(op),I18N_MSG_CMISC_216),NULL);
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
int command_passwd(object *pl, char *params)
{
    send_query(&pl->contr->socket,CS_QUERY_HIDEINPUT,
        i18n_translate(get_language(pl),I18N_MSG_CMISC_217));

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
int do_harvest(object* pl, int dir, object* skill) {
    sint16 x, y;
    int count = 0, proba; /* Probability to get the item, 100 based. */
    int level, exp;
    object* found[10]; /* Found items that can be harvested. */
    mapstruct* map;
    object* item, *inv;
    sstring trace, ttool, tspeed, race, tool, slevel, sexp;
    float speed;

    x = pl->x + freearr_x[dir];
    y = pl->y + freearr_y[dir];
    map = pl->map;

    if (!pl->type == PLAYER)
        return 0;

    if (!map)
        return 0;

    if (get_map_flags(map, &map, x, y, &x, &y) & P_OUT_OF_MAP) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You can't %s anything here.", NULL, skill->slaying);
        return 0;
    }

    if (!pl->chosen_skill || pl->chosen_skill->skill != skill->skill)
        return 0;

    trace = get_ob_key_value(pl->chosen_skill, "harvest_race");
    ttool = get_ob_key_value(pl->chosen_skill, "harvest_tool");
    tspeed = get_ob_key_value(pl->chosen_skill, "harvest_speed");
    if (!trace || strcmp(trace, "") == 0 || !ttool || strcmp(ttool, "") == 0 || !tspeed || strcmp(tspeed, "") == 0) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You can't %s anything here.", NULL, skill->slaying);
        LOG(llevError, "do_harvest: tool %s without harvest_[race|tool|speed]\n", pl->chosen_skill->name);
        return 0;
    }

    item = GET_MAP_OB(map, x, y);
    while (item && count < 10) {
        for (inv = item->inv; inv; inv = inv->below) {
            if (get_ob_key_value(inv, "harvestable") == NULL)
                continue;
            race = get_ob_key_value(inv, "harvest_race");
            tool = get_ob_key_value(inv, "harvest_tool");
            slevel = get_ob_key_value(inv, "harvest_level");
            sexp = get_ob_key_value(inv, "harvest_exp");
            if (race && (!slevel || !sexp)) {
                LOG(llevError, "do_harvest: item %s without harvest_[level|exp]\n", inv->name);
                continue;
            }
            if (race == trace && (!tool || tool == ttool))
                found[count++] = inv;
        }
        item = item->above;
    }
    if (count == 0) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You can't %s anything here.", NULL, skill->slaying);
        return 0;
    }

    inv = found[rndm(0, count - 1)];
    assert(inv);

    slevel = get_ob_key_value(inv, "harvest_level");
    sexp = get_ob_key_value(inv, "harvest_exp");
    level = atoi(slevel);
    exp = atoi(sexp);

    speed = atof(tspeed);
    if (speed < 0)
        speed = - speed * pl->speed;
    pl->speed_left -= speed;


    /* Now we found something to harvest, randomly try to get it. */
    if (level > skill->level + 10) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", NULL, skill->slaying);
        return 0;
    }

    if (level >= skill->level)
        /* Up to 10 more levels, 1 to 11 percent probability. */
        proba = 10 + skill->level - level;
    else if (skill->level <= level + 10)
        proba = 10 + (skill->level - level) * 2;
    else
        proba = 30;

    if (proba <= random_roll(0, 100, pl, 1)) {
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", NULL, skill->slaying);
        return 0;
    }

    /* Ok, got it. */
    item = get_object();
    copy_object_with_inv(inv, item);
    set_ob_key_value(item, "harvestable", NULL, 0);
    if (QUERY_FLAG(item, FLAG_MONSTER)) {
        int spot = find_free_spot(item, pl->map, pl->x, pl->y, 0, SIZEOFFREE);
        if (spot == -1) {
            /* Better luck next time...*/
            remove_ob(item);
            draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You fail to %s anything.", NULL, skill->slaying);
        return 0;
        }
        item->x = pl->x + freearr_x[spot];
        item->y = pl->y + freearr_y[spot];
        insert_ob_in_map(item, pl->map, NULL, 0);
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You %s a %s!", NULL, skill->slaying, item->name);
    }
    else {
        item = insert_ob_in_ob(item, pl);
        draw_ext_info_format(NDI_WHITE, 0, pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE, "You %s some %s", NULL, skill->slaying, item->name);
    }

    /* Get exp */
    change_exp(pl, exp, skill->name, SK_EXP_ADD_SKILL);

    return 0;
}

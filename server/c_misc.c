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

#include <global.h>
#include <loader.h>
#include <shstr.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

extern weathermap_t **weathermap;

/**
 * Handles misc. input request - things like hash table, malloc, maps,
 * who, etc.
 */

void map_info(object *op, char *search) {
    mapstruct *m;
    char map_path[MAX_BUF];
    long sec = seconds();

    draw_ext_info_format(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
	"Current time is: %02ld:%02ld:%02ld.",
	"Current time is: %02ld:%02ld:%02ld.",
	  (sec%86400)/3600,(sec%3600)/60,sec%60);

    draw_ext_info(NDI_UNIQUE, 0,op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
	"[fixed]Path               Pl PlM IM   TO Dif Reset",
	"Path               Pl PlM IM   TO Dif Reset");

    for(m=first_map;m!=NULL;m=m->next) {

	if ( search && strstr(m->path,search)==NULL ) continue; /* Skip unwanted maps */

	/* Print out the last 18 characters of the map name... */
	if (strlen(m->path)<=18) strcpy(map_path, m->path);
	else strcpy(map_path, m->path + strlen(m->path) - 18);

	draw_ext_info_format(NDI_UNIQUE,0,op,
	    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MAPS,
	    "[fixed]%-18.18s %2d %2d   %1d %4d %2d  %02d:%02d:%02d",
	    "%-18.18s %2d %2d   %1d %4d %2d  %02d:%02d:%02d",
              map_path, m->players,players_on_map(m,FALSE),
              m->in_memory,m->timeout,m->difficulty,
	      (MAP_WHEN_RESET(m)%86400)/3600,(MAP_WHEN_RESET(m)%3600)/60,
              MAP_WHEN_RESET(m)%60);
    }
}

/**
 * This command dumps the body information for object *op.
 * it doesn't care what the params are.
 * This is mostly meant as a debug command.
 */
int command_body(object *op, char *params)
{
    int i;

    /* Too hard to try and make a header that lines everything up, so just 
     * give a description.
     */
    draw_ext_info(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	"The first column is the name of the body location.", NULL);

    draw_ext_info(NDI_UNIQUE, 0, op,
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	"The second column is how many of those locations your body has.", NULL);

    draw_ext_info(NDI_UNIQUE, 0, op, 
	MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	"The third column is how many slots in that location are available.", NULL);

    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
	/* really debugging - normally body_used should not be set to anything
	 * if body_info isn't also set.
	 */
	if (op->body_info[i] || op->body_used[i]) {
	    draw_ext_info_format(NDI_UNIQUE, 0, op, 
		MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
		"[fixed]%-30s %5d %5d",
		"%-30s %5d %5d",
		body_locations[i].use_name, op->body_info[i], op->body_used[i]);
	}
    }
    if (!QUERY_FLAG(op, FLAG_USE_ARMOUR))
	draw_ext_info(NDI_UNIQUE, 0, op,
	    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	    "You are not allowed to wear armor", NULL);
    if (!QUERY_FLAG(op, FLAG_USE_WEAPON))
	draw_ext_info(NDI_UNIQUE, 0, op,
	    MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_BODY,
	    "You are not allowed to use weapons", NULL);

    return 1;
}


int command_motd(object *op, char *params)
{
	display_motd(op);
	return 1;
}

int command_rules(object *op, char *params)
{
	send_rules(op);
	return 1;
}

int command_news(object *op, char *params)
{
	send_news(op);
	return 1;
}

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
	"Sizeof: object=%d  player=%d  map=%d",
	"Sizeof: object=%d  player=%d  map=%d",
	sizeof(object),sizeof(player),sizeof(mapstruct));

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	"[fixed]%4d used objects:    %8d",
	"%4d used objects:    %8d",
	ob_used,i=(ob_used*sizeof(object)));

    sum_used+=i;
    sum_alloc+=i;
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	"[fixed]%4d free objects:    %8d",
	"%4d free objects:    %8d",
	ob_free,i=(ob_free*sizeof(object)));

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 "[fixed]%4d active objects:  %8d",
	 "%4d active objects:  %8d",
	 count_active(), 0);

    sum_alloc+=i;
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 "[fixed]%4d players:         %8d",
	 "%4d players:         %8d",
	 players,i=(players*sizeof(player)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  "[fixed]%4d maps allocated:  %8d",
	  "%4d maps allocated:  %8d",
	  nrofmaps, i=(nrofmaps*sizeof(mapstruct)));

    sum_alloc+=i;
    sum_used+=nrm*sizeof(mapstruct);

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 "[fixed]%4d maps in memory:  %8d",
	 "%4d maps in memory:  %8d",
	 nrm,mapmem);

    sum_alloc+=mapmem;
    sum_used+=mapmem;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 "[fixed]%4d archetypes:      %8d",
	 "%4d archetypes:      %8d",
	 anr,i=(anr*sizeof(archetype)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 "[fixed]%4d animations:      %8d",
	 "%4d animations:      %8d",
	  anims,i=(anims*sizeof(Fontindex)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	 "[fixed]%4d treasurelists    %8d",
	 "%4d treasurelists    %8d",
	 tlnr,i=(tlnr*sizeof(treasurelist)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  "[fixed]%4ld treasures        %8d",
	  "%4ld treasures        %8d",
	  nroftreasures, i=(nroftreasures*sizeof(treasure)));

    sum_alloc+=i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	"[fixed]%4ld artifacts        %8d", 
	"%4ld artifacts        %8d", 
	nrofartifacts, i=(nrofartifacts*sizeof(artifact)));

    sum_alloc+=i;
    sum_used +=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  "[fixed]%4ld artifacts strngs %8d", 
	  "%4ld artifacts strngs %8d", 
	  nrofallowedstr, i=(nrofallowedstr*sizeof(linked_char)));

    sum_alloc += i;
    sum_used+=i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  "[fixed]%4d artifactlists    %8d",
	  "%4d artifactlists    %8d",
	  alnr,i=(alnr*sizeof(artifactlist)));

    sum_alloc += i;
    sum_used += i;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  "[fixed]Total space allocated:%8d",
	  "Total space allocated:%8d",
	  sum_alloc);

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_MALLOC,
	  "[fixed]Total space used:     %8d",
	  "Total space used:     %8d",
	  sum_used);

}

/**
 * Pretty much identical to current map_info, but on a bigger scale 
 * This function returns the name of the players current region, and
 * a description of it. It is there merely for flavour text.
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
	"You are in %s. \n %s",
	"You are in %s. \n %s",
	 get_region_longname(r), get_region_msg(r));
}

void current_map_info(object *op) {
    mapstruct *m = op->map;

    if (!m)
	return;

    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
	"%s (%s) in %s", 
	"%s (%s) in %s", 
	 m->name, m->path, get_name_of_region_for_map(m));

    if (QUERY_FLAG(op,FLAG_WIZ)) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		"players:%d difficulty:%d size:%dx%d start:%dx%d timeout %ld",
		"players:%d difficulty:%d size:%dx%d start:%dx%d timeout %ld",
		 m->players, m->difficulty, 
		 MAP_WIDTH(m), MAP_HEIGHT(m), 
		 MAP_ENTER_X(m), MAP_ENTER_Y(m),
		 MAP_TIMEOUT(m));

    }
    if (m->msg)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, m->msg, NULL);
}

#ifdef DEBUG_MALLOC_LEVEL
int command_malloc_verify(object *op, char *parms)
{
    extern int malloc_verify(void);

    if (!malloc_verify()) 
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "Heap is corrupted.", NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		"Heap checks out OK.", NULL);

    return 1;
}
#endif

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
	"In the world currently there are:", NULL);

    for (reg=first_region;reg!=NULL;reg=reg->next)
	if(reg->counter>0) {
	    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		 "%u players in %s",
		 "%u players in %s", 
		 reg->counter, get_region_longname(reg));
	}
    return 1;
}

typedef struct {
	char namebuf[MAX_BUF];
	int login_order;
} chars_names;

/** local functon for qsort comparison*/
static int name_cmp (const chars_names *c1, const chars_names *c2) {
    return strcasecmp (c1->namebuf, c2->namebuf);
}

int command_who (object *op, char *params) {
    player *pl;
    uint16 i;
    region *reg;
    char* format;
    int num_players = 0, num_wiz = 0, num_afk = 0, num_bot = 0;
    chars_names *chars = NULL;
    
    /* 
     * The who formats are defined in config to be blank. They should have been
     * overridden by the settings file, if there are no entries however, it will
     * have stayed blank. Since this probably isn't what is wanted, we will check if
     * new formats have been specified, and if not we will use the old defaults.
     */
    if (!strcmp(settings.who_format,"")) 
    	strcpy(settings.who_format, "%N_%T%t%h%d%b%n<%m>");
    if (!strcmp(settings.who_wiz_format,"")) 
    	strcpy(settings.who_wiz_format, "%N_%T%t%h%d%b%nLevel %l <%m>(@%i)(%c)");
    if (op == NULL || QUERY_FLAG(op, FLAG_WIZ))
    	format=settings.who_wiz_format;
    else
    	format=settings.who_format;

    reg=get_region_from_string(params);
    
    for (pl=first_player;pl!=NULL;pl=pl->next) {
	if (pl->ob->map == NULL)
	    continue;
	if (pl->hidden && !QUERY_FLAG(op, FLAG_WIZ)) continue;
	
	if(!region_is_child_of_region(get_region_by_map(pl->ob->map),reg)) continue;

	if (pl->state==ST_PLAYING || pl->state==ST_GET_PARTY_PASSWORD) {

	    num_players++;
	    chars = (chars_names *) realloc(chars, num_players*sizeof(chars_names));
            if (chars == NULL) {
	        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
			      "who failed - out of memory!", NULL);
                return 0;
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
    	if (reg == NULL) 
	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
		     "Total Players (%d) -- WIZ(%d) AFK(%d) BOT(%d)", 
		     "Total Players (%d) -- WIZ(%d) AFK(%d) BOT(%d)", 
		     num_players, num_wiz, num_afk, num_bot);
	    else
     		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WHO,
		     "Total Players in %s (%d) -- WIZ(%d) AFK(%d) BOT(%d)", 
		     "Total Players in %s (%d) -- WIZ(%d) AFK(%d) BOT(%d)", 
		     reg->longname?reg->longname:reg->name, num_players, num_wiz, num_afk, num_bot);
    }
    qsort (chars, num_players, sizeof(chars_names), (int (*)(const void *, const void *))name_cmp);
    for (i=0;i<num_players;i++)
	display_who_entry(op, find_player(chars[i].namebuf), format);
    free(chars);    
    return 1;
}

/** Display a line of 'who' to op, about pl, using the formatting specified by format */
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
            get_who_escape_code_value(tmpbuf,format[i],pl);
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
 * Returns the value of the escape code used in the who format specifier
 * the values are:
 * N	Name of character
 * t	title of character
 * T    the optional "the " sequence value (depend if player has own_title or not)
 * c	count
 * n	newline
 * h	<Hostile> if character is hostile, nothing otherwise
 * d	<WIZ> if character is a dm, nothing otherwise
 * a	<AFK> if character is afk, nothing otherwise
 * b	<BOT> if character is a bot, nothing otherwise
 * l	the level of the character
 * m	the map path the character is currently on
 * M	the map name of the map the character is currently on
 * r	the region name (eg scorn, wolfsburg)
 * R	the regional title (eg The Kingdom of Scorn, The Port of Wolfsburg)
 * i	player's ip address
 * %	a literal %
 * _	a literal underscore
 */
  
void get_who_escape_code_value(char *return_val, const char letter, player *pl) {
    
    switch (letter) {
	case 'N' :	strcpy(return_val, pl->ob->name);
			break;

	case 't' :	strcpy(return_val,(pl->own_title[0]=='\0'?pl->title:pl->own_title));
			break;

	case 'T' :	if (pl->own_title[0]=='\0')
			    strcpy(return_val,"the ");
			else
			    *return_val='\0';
			break;

	case 'c' :	sprintf(return_val,"%d",pl->ob->count);
			break;

	case 'n' :	strcpy(return_val, "\n"); 
			break;

	case 'h' :	strcpy(return_val,pl->peaceful?"":" <Hostile>");
			break;

	case 'l' :	sprintf(return_val,"%d",pl->ob->level);
			break;

	case 'd' :	strcpy(return_val,(QUERY_FLAG(pl->ob,FLAG_WIZ)?" <WIZ>":""));
			break;

	case 'a' :	strcpy(return_val,(QUERY_FLAG(pl->ob,FLAG_AFK)?" <AFK>":""));
			break;

	case 'b' :	strcpy(return_val,(pl->socket.is_bot == 1)?" <BOT>":"");
			break;

	case 'm' :	strcpy(return_val,pl->ob->map->path);
			break;

	case 'M' :	strcpy(return_val,pl->ob->map->name?pl->ob->map->name:"Untitled");
			break;		

	case 'r' :	strcpy(return_val,get_name_of_region_for_map(pl->ob->map));
			break;

	case 'R' :	strcpy(return_val,get_region_longname(get_region_by_map(pl->ob->map)));
			break;

	case 'i' :	strcpy(return_val,pl->socket.host);
			break;

	case '%' :	strcpy(return_val, "%"); 
			break;

	case '_' :	strcpy(return_val, "_"); 
			break;
    }
}


int command_afk (object *op, char *params)
{
    if QUERY_FLAG(op,FLAG_AFK) { 
	CLEAR_FLAG(op,FLAG_AFK);
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
		      "You are no longer AFK", NULL);
    }    
    else 
    {
	SET_FLAG(op,FLAG_AFK);
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
		      "You are now AFK", NULL);
    }
    return 1;
}

int command_malloc (object *op, char *params)
{
    malloc_info(op);
    return 1;
}

int command_mapinfo (object *op, char *params)
{
    current_map_info(op);
    return 1;
}

int command_whereami (object *op, char *params)
{
    current_region_info(op);
    return 1;
}

 int command_maps (object *op, char *params)
{
    map_info(op,params);
    return 1;
}

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

int command_time (object *op, char *params)
{
    time_info(op);
    return 1;
}

int command_weather (object *op, char *params)
{
    int wx, wy, temp, sky;
    char buf[MAX_BUF];

    if (settings.dynamiclevel < 1)
	return 1;

    if (op->map == NULL)
	return 1;

    if (worldmap_to_weathermap(op->x, op->y, &wx, &wy, op->map) != 0)
	return 1;

    if (QUERY_FLAG(op, FLAG_WIZ)) {
	/* dump the weather, Dm style! Yo! */

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Real temp: %d",
	     "Real temp: %d",
	     real_world_temperature(op->x, op->y, op->map));

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Base temp: %d",
	     "Base temp: %d",
	     weathermap[wx][wy].temp);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Humid: %d",
	     "Humid: %d",
	     weathermap[wx][wy].humid);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Wind: dir=%d speed=%d",
	     "Wind: dir=%d speed=%d",
	     weathermap[wx][wy].winddir, weathermap[wx][wy].windspeed);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Pressure: %d",
	     "Pressure: %d",
	     weathermap[wx][wy].pressure);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Avg Elevation: %d",
	     "Avg Elevation: %d",
	     weathermap[wx][wy].avgelev);

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "Rainfall: %d  Water: %d",
	     "Rainfall: %d  Water: %d",
	     weathermap[wx][wy].rainfall, weathermap[wx][wy].water);
    }

    temp = real_world_temperature(op->x, op->y, op->map);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	 "It's currently %d degrees Centigrade out.",
	 "It's currently %d degrees Centigrade out.",
	 temp);

    /* humid */
    if (weathermap[wx][wy].humid < 20)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      "It is very dry.", NULL);
    else if (weathermap[wx][wy].humid < 40)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      "It is very comfortable today.", NULL);
    else if (weathermap[wx][wy].humid < 60)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      "It is a bit muggy.", NULL);
    else if (weathermap[wx][wy].humid < 80)
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      "It is muggy.", NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
		      "It is uncomfortably muggy.", NULL);

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
	     "There is a mild breeze coming from the %s.", 
	     "There is a mild breeze coming from the %s.", 
	     buf);
    else if (weathermap[wx][wy].windspeed < 10)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	      "There is a strong breeze coming from the %s.", 
	      "There is a strong breeze coming from the %s.", 
	      buf);
    else if (weathermap[wx][wy].windspeed < 15)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "There is a light wind coming from the %s.", 
	     "There is a light wind coming from the %s.", 
	     buf);
    else if (weathermap[wx][wy].windspeed < 25)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "There is a strong wind coming from the %s.",
	     "There is a strong wind coming from the %s.",
	     buf);
    else if (weathermap[wx][wy].windspeed < 35)
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "There is a heavy wind coming from the %s.", 
	     "There is a heavy wind coming from the %s.", 
	     buf);
    else
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
	     "The wind from the %s is incredibly strong!", 
	     "The wind from the %s is incredibly strong!", 
	     buf);

    sky = weathermap[wx][wy].sky;
    if (temp <= 0 && sky > SKY_OVERCAST && sky < SKY_FOG)
	sky += 10; /*let it snow*/

    switch (sky) {
	case SKY_CLEAR:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "There isn't a cloud in the sky.", NULL);
	    break;
	case SKY_LIGHTCLOUD:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "There are a few light clouds in the sky", NULL);
	    break;
	case SKY_OVERCAST:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			   "The sky is cloudy and dreary.", NULL);
	    break;
	case SKY_LIGHT_RAIN:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "It is raining softly.", NULL);
	    break;
	case SKY_RAIN:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "It is raining.", NULL);
	    break;
	case SKY_HEAVY_RAIN:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "It is raining heavily.", NULL);
	    break;
	case SKY_HURRICANE:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "There is a heavy storm!  You should go inside!", NULL);
	    break;
	case SKY_FOG:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			   "It's foggy and miserable.", NULL);
	    break;
	case SKY_HAIL:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "It's hailing out!  Take cover!", NULL);
	    break;
	case SKY_LIGHT_SNOW:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "Snow is gently falling from the sky.", NULL);
	    break;
	case SKY_SNOW:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "It is snowing out.", NULL);
	    break;
	case SKY_HEAVY_SNOW:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "Snow is falling very heavily.", NULL);
	    break;
	case SKY_BLIZZARD:
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_WEATHER,
			  "A full blown blizzard is in effect.  You might want to take cover!", NULL);
	    break;
    }
    return 1;
}

int command_archs (object *op, char *params)
{
    arch_info(op);
    return 1;
}

int command_hiscore (object *op, char *params)
{
    display_high_score(op,op==NULL?9999:50, params);
    return 1;
}

int command_debug (object *op, char *params)
{
    int i;

    if(params==NULL || !sscanf(params, "%d", &i)) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		 "Global debug level is %d.",
		 "Global debug level is %d.",
		 settings.debug);
	return 1;
    }
    if(op != NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Debug is a privileged command.", NULL);
	return 1;
    }
    settings.debug = (enum LogLevel) FABS(i);
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		 "Debug level set to %d.", 
		 "Debug level set to %d.", 
		 i);
    return 1;
}


/**
 * Those dumps should be just one dump with good parser
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
		      "You will now walk through walls.\n", NULL);
	SET_FLAG(op, FLAG_WIZPASS);
    } else {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
		      "You will now be stopped by walls.\n", NULL);
	CLEAR_FLAG(op, FLAG_WIZPASS);
    }
    return 0;
}

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
		      "You can now cast spells anywhere.", NULL);
	SET_FLAG(op, FLAG_WIZCAST);
    } else {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
		      "You now cannot cast spells in no-magic areas.", NULL);
	CLEAR_FLAG(op, FLAG_WIZCAST);
    }
    return 0;
}

int command_dumpallobjects (object *op, char *params)
{
    dump_all_objects();
    return 0;
}

int command_dumpfriendlyobjects (object *op, char *params)
{
    dump_friendly_objects();
    return 0;
}

int command_dumpallarchetypes (object *op, char *params)
{
    dump_all_archetypes();
    return 0;
}

int command_ssdumptable (object *op, char *params)
{
    ss_dump_table(SS_DUMP_TABLE, NULL, 0);
    return 0;
}

int command_dumpmap (object *op, char *params)
{
    if(op)
        dump_map(op->map);
    return 0;
}

int command_dumpallmaps (object *op, char *params)
{
    dump_all_maps();
    return 0;
}

int command_printlos (object *op, char *params)
{
    if (op)
        print_los(op);
    return 0;
}


int command_version (object *op, char *params)
{
    version(op);
    return 0;
}


int command_output_sync(object *op, char *params)
{
    int val;

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
	     "Output sync time is presently %d", 
	     "Output sync time is presently %d", 
	     op->contr->outputs_sync);
	return 1;
    }
    val=atoi(params);
    if (val>0) {
	op->contr->outputs_sync = val;
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
	     "Output sync time now set to %d", 
	     "Output sync time now set to %d", 
	     op->contr->outputs_sync);
    }
    else
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Invalid value for output_sync.", NULL);

    return 1;
}

int command_output_count(object *op, char *params)
{
    int val;

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
	     "Output count is presently %d",
	     "Output count is presently %d",
	     op->contr->outputs_count);
	return 1;
    }
    val=atoi(params);
    if (val>0) {
	op->contr->outputs_count = val;
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
	     "Output count now set to %d", 
	     "Output count now set to %d", 
	     op->contr->outputs_count);
    }
    else
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Invalid value for output_count.", NULL);

    return 1;
}

int command_listen (object *op, char *params)
{
    int i;

    if(params==NULL || !sscanf(params, "%d", &i)) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
	     "Set listen to what (presently %d)?", 
	     "Set listen to what (presently %d)?", 
	     op->contr->listening);
	return 1;
    }
    op->contr->listening=(char) i;
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE, 
	 "Your verbose level is now %d.",
	 "Your verbose level is now %d.",
	  i);
    return 1;
}

/**
 * Prints out some useful information for the character.  Everything we print
 * out can be determined by the docs, so we aren't revealing anything extra -
 * rather, we are making it convenient to find the values.  params have
 * no meaning here.
 */
int command_statistics(object *pl, char *params)
{
    if (!pl->contr) return 1;

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]  Experience: %" FMT64,
			 "  Experience: %" FMT64,
			 pl->stats.exp);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]  Next Level: %" FMT64,
			 "  Next Level: %" FMT64,
			 level_exp(pl->level+1, pl->expmul));

    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
		  "[fixed]\nStat       Nat/Real/Max",
		  "\nStat       Nat/Real/Max");

    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Str         %2d/ %3d/%3d",
			 "Str         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Str, pl->stats.Str, 20+pl->arch->clone.stats.Str);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Dex         %2d/ %3d/%3d",
			 "Dex         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Dex, pl->stats.Dex, 20+pl->arch->clone.stats.Dex);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Con         %2d/ %3d/%3d",
			 "Con         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Con, pl->stats.Con, 20+pl->arch->clone.stats.Con);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Int         %2d/ %3d/%3d",
			 "Int         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Int, pl->stats.Int, 20+pl->arch->clone.stats.Int);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Wis         %2d/ %3d/%3d",
			 "Wis         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Wis, pl->stats.Wis, 20+pl->arch->clone.stats.Wis);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Pow         %2d/ %3d/%3d",
			 "Pow         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Pow, pl->stats.Pow, 20+pl->arch->clone.stats.Pow);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "[fixed]Cha         %2d/ %3d/%3d",
			 "Cha         %2d/ %3d/%3d",
			 pl->contr->orig_stats.Cha, pl->stats.Cha, 20+pl->arch->clone.stats.Cha);
    draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_STATISTICS,
			 "\nAttack Mode: %s",
			 "\nAttack Mode: %s",
			 pl->contr->peaceful? "Peaceful":"Hostile");

   /* Can't think of anything else to print right now */
   return 0;
}

int command_fix_me(object *op, char *params)
{
    sum_weight(op);
    fix_object(op);
    return 1;
}

int command_players(object *op, char *paramss)
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
			          "[fixed]%s\t%04d %02d %02d %02d %02d %02d",
			          "%s\t%04d %02d %02d %02d %02d %02d",
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



int command_logs (object *op, char *params)
{
    int i;
    int first;

    first=1;
    for(i=2; i<socket_info.allocated_sockets; i++) {
	if (init_sockets[i].old_mode == Old_Listen) {
	    if (first) {
		draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			      "Kill-logs are sent to:", NULL);
		first=0;
	    }
	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				 "%s: %s",
				 "%s: %s",
				 init_sockets[i].host,init_sockets[i].comment);
	}
    }
    if (first) {
	draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Nobody is currently logging kills.", NULL);
    }
    return 1;
}

int command_applymode(object *op, char *params)
{
    unapplymode unapply = op->contr->unapply;
    static const char* const types[]={"nochoice", "never", "always"};

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	     "applymode is set to %s",
	     "applymode is set to %s",
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
	     "applymode: Unknown options %s, valid options are nochoice, never, always",
	     "applymode: Unknown options %s, valid options are nochoice, never, always",
	     params);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	 "Applymode %s set to %s",
	 "Applymode %s set to %s",
	 (unapply==op->contr->unapply?"":" now"),
	 types[op->contr->unapply]);

    return 1;
}

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
			     "bowmode is set to %s",
			     "bowmode is set to %s",
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
		 "bowmode %s set to %s",
		 "bowmode %s set to %s",
		 (oldtype==op->contr->bowtype?"":"now"),
		 types[op->contr->bowtype]);
    return 1;
}

int command_petmode(object *op, char *params)
{
    petmode_t oldtype=op->contr->petmode;
    static const char* const types[]={"normal", "sad", "defend", "arena"};

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		     "petmode is set to %s",
		     "petmode is set to %s",
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
	    "petmode: Unknown options %s, valid options are normal, sad (seek and destroy), defend, arena", 
	    "petmode: Unknown options %s, valid options are normal, sad (seek and destroy), defend, arena", 
	     params);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	 "petmode %s set to %s",
	 "petmode %s set to %s",
	(oldtype==op->contr->petmode?"":"now"),
	types[op->contr->petmode]);
    return 1;
}

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
				  "Pets:", NULL);
		draw_ext_info_format(NDI_UNIQUE, 0, op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			     "%d  %s - level %d", 
			     "%d  %s - level %d", 
			     ++counter, ob->name, ob->level );
	    }	
	    else if (!have_shown_pet && ++counter==target) {
	    	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]level %d %s",
				     "level %d %s",
				     ob->level, ob->name);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]%d/%d HP, %d/%d SP",
				     "%d/%d HP, %d/%d SP",
				     ob->stats.hp, ob->stats.maxhp, ob->stats.sp, ob->stats.maxsp);

		/* this is not a nice way to do this, it should be made to be more like the statistics command */
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Str %d",
				     "Str %d",
				     ob->stats.Str);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Dex %d", 
				     "Dex %d",
				     ob->stats.Dex);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Con %d",
				     "Con %d",
				     ob->stats.Con);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Int %d",
				     "Int %d",
				     ob->stats.Int);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Wis %d",
				     "Wis %d",
				     ob->stats.Wis);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Cha %d",
				     "Cha %d",
				     ob->stats.Cha);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]Pow %d",
				     "Pow %d",
				     ob->stats.Pow);
		draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
				     "[fixed]wc %d  damage %d ac %d ", 
				     "wc %d  damage %d ac %d ", 
				     ob->stats.wc, ob->stats.dam, ob->stats.ac);
		have_shown_pet=1;
	    }
	}
    }
    if (counter == 0) 
    	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "you have no pets.", NULL);
    else if (target !=0 && have_shown_pet==0)
    	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "no such pet.", NULL);
    return 0;
}

int command_usekeys(object *op, char *params)
{
    usekeytype oldtype=op->contr->usekeys;
    static const char* const types[]={"inventory", "keyrings", "containers"};

    if (!params) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		     "usekeys is set to %s",
		     "usekeys is set to %s",
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
	    "usekeys: Unknown options %s, valid options are inventory, keyrings, containers",
	    "usekeys: Unknown options %s, valid options are inventory, keyrings, containers",
	     params);
	return 0;
    }
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
	 "usekeys %s set to %s",
	 "usekeys %s set to %s",
	(oldtype==op->contr->usekeys?"":"now"),
	types[op->contr->usekeys]);

    return 1;
}

int command_resistances(object *op, char *params)
{
    int i;
    if (!op)
	return 0;

    for (i=0; i<NROFATTACKS; i++) {
	if (i==ATNR_INTERNAL) continue;

	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			     "[fixed]%-20s %+5d", 
			     "%-20s %+5d", 
			     attacktype_desc[i], op->resist[i]);
    }

    /* If dragon player, let's display natural resistances */
    if ( is_dragon_pl( op ) ) {
        int attack;
        object* tmp;

        for ( tmp = op->inv; tmp != NULL; tmp = tmp->below ) {
            if ( ( tmp->type == FORCE ) && ( strcmp( tmp->arch->name, "dragon_skin_force" )== 0 ) ) {
                draw_ext_info( NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			      "\nNatural skin resistances:", NULL);

                for ( attack = 0; attack < NROFATTACKS; attack++ ) {
                    if ( atnr_is_dragon_enabled( attack ) ) {
                        draw_ext_info_format( NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
					     "%s: %d", 
					     "%s: %d", 
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
 * Actual commands.
 * Those should be in small separate files (c_object.c, c_wiz.c, cmove.c,...)
 */
static void help_topics(object *op, int what)
{
    DIR *dirp;
    struct dirent *de;
    char filename[MAX_BUF], line[HUGE_BUF];
    int namelen;
  
    switch (what) {
	case 1:
	    sprintf(filename, "%s/wizhelp", settings.datadir);
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  "      Wiz commands:", NULL);
	    break;
	case 3:
	    sprintf(filename, "%s/mischelp", settings.datadir);
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  "      Misc help:", NULL);
	    break;
	default:
	    sprintf(filename, "%s/help", settings.datadir);
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  "      Commands:", NULL);
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

	strcat(line, de->d_name);
	strcat(line, " ");
    }
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
		  line, line);
    closedir(dirp);
}

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
			  "      Wiz commands:", NULL);
	    break;

	case 2:
	    ap= CommunicationCommands;
	    size= CommunicationCommandSize;
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  "      Communication commands:", NULL);
	    break;

	default:
	    ap =Commands;
	    size =CommandsSize;
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
			  "      Commands:", NULL);
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


int command_help (object *op, char *params)
{
    struct stat st;
    FILE *fp;
    char filename[MAX_BUF], line[MAX_BUF];
    int len;

    /*
     * Main help page?
     */

    if (!params) {
	sprintf(filename, "%s/def_help", settings.datadir);
	if ((fp=fopen(filename, "r")) == NULL) {
	    LOG(llevError, "Cannot open help file %s: %s\n", filename, strerror_local(errno));
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
			     "Illegal characters in '%s'", 
			     "Illegal characters in '%s'", 
			     params);
	return 0;
    }

    sprintf(filename, "%s/mischelp/%s", settings.datadir, params);
    if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
	if (op) {
	    sprintf(filename, "%s/help/%s", settings.datadir, params);
	    if (stat(filename, &st) || !S_ISREG(st.st_mode)) {
		if (QUERY_FLAG(op, FLAG_WIZ)) {
		    sprintf(filename, "%s/wizhelp/%s", settings.datadir, params);
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
	LOG(llevError, "Cannot open help file %s: %s\n", filename, strerror_local(errno));
	return 0;
    }

  draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO,
		       "Help about '%s'",
		       "Help about '%s'",
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
	 "No help available on '%s'", 
	 "No help available on '%s'", 
	 params);

    return 0;
}


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

int command_quit (object *op, char *params)
{
    send_query(&op->contr->socket,CS_QUERY_SINGLECHAR,
	       "Quitting will delete your character.\nAre you sure you want to quit (y/n):");

    op->contr->state = ST_CONFIRM_QUIT;
    return 1;
}

/**
 * don't allow people to exit explore mode.  It otherwise becomes
 * really easy to abuse this.
 */
int command_explore (object *op, char *params)
{
    if (settings.explore_mode == FALSE)
	return 1;
    /*
     * I guess this is the best way to see if we are solo or not.  Actually,
     * are there any cases when first_player->next==NULL and we are not solo?
     */
    if ((first_player!=op->contr) || (first_player->next!=NULL)) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "You can not enter explore mode if there are other players", NULL);
    } else if (op->contr->explore)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "There is no return from explore mode", NULL);
    else {
	op->contr->explore=1;
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "You are now in explore mode", NULL);
    }
    return 1;
}

int command_sound (object *op, char *params)
{
    if (op->contr->socket.sound) {
        op->contr->socket.sound=0;
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Sounds are turned off", NULL);
    }
    else {
        op->contr->socket.sound=1;
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "The sounds are enabled.", NULL);
    }
    return 1;
}

/**
 * Perhaps these should be in player.c, but that file is
 * already a bit big.
 */

void receive_player_name(object *op,char k) {

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

void receive_player_password(object *op,char k) {

    unsigned int pwd_len=strlen(op->contr->write_buf);

    if(pwd_len<=1||pwd_len>17) {
        if (op->contr->state == ST_CHANGE_PASSWORD_OLD || op->contr->state == ST_CHANGE_PASSWORD_NEW || op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "Password changed cancelled.", NULL);
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
		      "You are not allowed to play.", NULL);
        get_name(op);
        return;
    }

    if(op->contr->state==ST_CONFIRM_PASSWORD) {
        if(!check_password(op->contr->write_buf+1,op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0,op,  MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "The passwords did not match.", NULL);
            get_name(op);
            return;
        }
        display_motd(op);
        draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "\nWelcome, Brave New Warrior!\n", NULL);
        roll_again(op);
        op->contr->state=ST_ROLL_STAT;
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_OLD) {
        if (!check_password(op->contr->write_buf + 1, op->contr->password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "You entered the wrong current password.", NULL);
            op->contr->state = ST_PLAYING;
        } else {
            send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, "Please enter your new password, or blank to cancel:");
            op->contr->state = ST_CHANGE_PASSWORD_NEW;
        }
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_NEW) {
        strcpy(op->contr->new_password, crypt_string(op->contr->write_buf + 1, NULL));
        send_query(&op->contr->socket, CS_QUERY_HIDEINPUT, "Please confirm your new password, or blank to cancel:");
        op->contr->state = ST_CHANGE_PASSWORD_CONFIRM;
        return;
    }

    if (op->contr->state == ST_CHANGE_PASSWORD_CONFIRM) {
        if (strcmp(crypt_string(op->contr->write_buf + 1, op->contr->new_password), op->contr->new_password)) {
            draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "The new passwords don't match!", NULL);
        } else {
        	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			      "Password changed.", NULL);
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


int command_title (object *op, char *params)
{
    char buf[MAX_BUF];
    
    if (settings.set_title == FALSE) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      "You cannot change your title.", NULL);
	return 1;
    }

    /* dragon players cannot change titles */
    if (is_dragon_pl(op)) {
        draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      "Dragons cannot change titles.", NULL);
        return 1;
    }
    
    if(params == NULL) {
	if(op->contr->own_title[0]=='\0')
	    sprintf(buf,"Your title is '%s'.", op->contr->title);
	else
	    sprintf(buf,"Your title is '%s'.", op->contr->own_title);
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,buf, NULL);
	return 1;
    }
    if(strcmp(params, "clear")==0 || strcmp(params, "default")==0) {
	if(op->contr->own_title[0]=='\0')
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			  "Your title is the default title.", NULL);
	else
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			  "Title set to default.", NULL);
	op->contr->own_title[0]='\0';
	return 1;
    }

    if((int)strlen(params) >= MAX_NAME) {
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      "Title too long.", NULL);
	return 1;
    }
    strcpy(op->contr->own_title, params);
    return 1;
}

int command_save (object *op, char *params)
{
    if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL) & P_NO_CLERIC) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "You can not save on unholy ground", NULL);
    } else if (!op->stats.exp) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "You don't deserve to save yet.", NULL);
    } else {
	if(save_player(op,1))
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "You have been saved.", NULL);
	else
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "SAVE FAILED!", NULL);
    }
    return 1;
}


int command_peaceful (object *op, char *params)
{
    if((op->contr->peaceful=!op->contr->peaceful))
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      "You will not attack other players.", NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		      "You will attack other players.", NULL);
    return 1;
}



int command_wimpy (object *op, char *params)
{
    int i;

    if (params==NULL || !sscanf(params, "%d", &i)) {
	draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
			     "Your current wimpy level is %d.",
			     "Your current wimpy level is %d.",
			     op->run_away);
	return 1;
    }
    draw_ext_info_format(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_CONFIG,
		 "Your new wimpy level is %d.", 
		 "Your new wimpy level is %d.", 
		 i);
    op->run_away = i;
    return 1;
}


int command_brace (object *op, char *params)
{
    if (!params)
	op->contr->braced =!op->contr->braced;
    else
	op->contr->braced =onoff_value(params);

    if(op->contr->braced)
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "You are braced.", NULL);
    else
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Not braced.", NULL);

    fix_object(op);

    return 0;
}

int command_kill_pets(object *op, char *params)
{
    objectlink *obl, *next;
    int counter=0, removecount=0;
    if (params == NULL) { 
    	terminate_all_pets(op);
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
		      "Your pets have been killed.", NULL);
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
			 "killed %d pets.", 
			 "killed %d pets.", 
			 removecount);
	else
	    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
			  "Couldn't find any suitable pets to kill.",NULL);
    }
    return 0;
}

/**
 * Displays all non start/end tags for specified quest.
 **/
static void display_quest_details( object* pl, object* quest )
{

    draw_ext_info_format( NDI_WHITE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
		 "Quest: %s\n%s\n",
		 "Quest: %s\n%s\n",
		 quest_get_name( quest ), quest->lore ? quest->lore : "(no description available)" );
}

/**
 * Displays quest informations to player.
 * Acceptable parameters:
 *  * finished => finished quests only
 *  * <name> => only this particular quest, finished or not, with details
 *  * nothing => all current quests
 *
 * For current quests, will display either the lore of the non start tags,
 *  or the lore of start tag if no other tag.
 **/
int command_quests( object *pl, char *params )
{
    object* item;

    if ( params && !strcmp( params, "finished" ) ) {
        draw_ext_info( NDI_WHITE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
		      "Completed quests:\n", NULL);

        for ( item = pl->inv; item; item = item->below ) {
            if ( quest_is_quest_marker( item, 0 ) ) {
                draw_ext_info( NDI_WHITE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
			      quest_get_name( item ), NULL );
	    }
	}
	return 1;
    }

    if ( params ) {
        for ( item = pl->inv; item; item = item->below ) {
            if ( quest_is_quest_marker( item, 0 )
                && !strcmp( quest_get_name( item ), params ) ) {
		    display_quest_details( pl, item );
	    }
	}
	return 1;
    }

    /*Display current quests */
    draw_ext_info( NDI_WHITE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
		  "Current quests:\n",NULL );

    for ( item = pl->inv; item; item = item->below ) {
        if ( quest_is_quest_marker( item, 0 ) && quest_is_in_progress( item, 0 ) ) {
            draw_ext_info( NDI_WHITE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS,
			  quest_get_name( item ), NULL );
	}
    }
    return 1;
}

/**
 * Player is asking to change password.
 **/
int command_passwd(object *pl, char *params)
{
    send_query(&pl->contr->socket,CS_QUERY_HIDEINPUT,
        "Password change.\nPlease enter your current password, or empty string to cancel.");

    pl->contr->state = ST_CHANGE_PASSWORD_OLD;
    return 1;
}

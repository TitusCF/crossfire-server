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
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

extern weathermap_t **weathermap;

/* Handles misc. input request - things like hash table, malloc, maps,
 * who, etc.
 */

void map_info(object *op, char *search) {
  mapstruct *m;
  char buf[MAX_BUF], map_path[MAX_BUF];
  long sec = seconds();
  new_draw_info_format(NDI_UNIQUE, 0, op,
	"Current time is: %02ld:%02ld:%02ld.",
	  (sec%86400)/3600,(sec%3600)/60,sec%60);
  new_draw_info(NDI_UNIQUE, 0,op,"Path               Pl PlM IM   TO Dif Reset");
  for(m=first_map;m!=NULL;m=m->next) {

    if ( search && strstr(m->path,search)==NULL ) continue; /* Skip unwanted maps */
    /* Print out the last 18 characters of the map name... */
    if (strlen(m->path)<=18) strcpy(map_path, m->path);
    else strcpy(map_path, m->path + strlen(m->path) - 18);
      sprintf(buf,"%-18.18s %2d %2d   %1d %4d %2d  %02d:%02d:%02d",
              map_path, m->players,players_on_map(m,FALSE),
              m->in_memory,m->timeout,m->difficulty,
	      (MAP_WHEN_RESET(m)%86400)/3600,(MAP_WHEN_RESET(m)%3600)/60,
              MAP_WHEN_RESET(m)%60);
    new_draw_info(NDI_UNIQUE, 0,op,buf);
  }
}

/* This command dumps the body information for object *op.
 * it doesn't care what the params are.
 * This is mostly meant as a debug command.
 */
int command_body(object *op, char *params)
{
    int i;

    /* Too hard to try and make a header that lines everything up, so just 
     * give a description.
     */
    new_draw_info(NDI_UNIQUE, 0, op, "The first column is the name of the body location.");
    new_draw_info(NDI_UNIQUE, 0, op, "The second column is how many of those locations your body has.");
    new_draw_info(NDI_UNIQUE, 0, op, "The third column is how many slots in that location are available.");
    for (i=0; i<NUM_BODY_LOCATIONS; i++) {
	/* really debugging - normally body_used should not be set to anything
	 * if body_info isn't also set.
	 */
	if (op->body_info[i] || op->body_used[i]) {
	    new_draw_info_format(NDI_UNIQUE, 0, op, 
		"%-30s %5d %5d", body_locations[i].use_name, op->body_info[i], op->body_used[i]);
	}
    }
    if (!QUERY_FLAG(op, FLAG_USE_ARMOUR))
	new_draw_info(NDI_UNIQUE, 0, op, "You are not allowed to wear armor");
    if (!QUERY_FLAG(op, FLAG_USE_WEAPON))
	new_draw_info(NDI_UNIQUE, 0, op, "You are not allowed to use weapons");

    return 1;
}


int command_motd(object *op, char *params)
{
	display_motd(op);
	return 1;
}

int command_bug(object *op, char *params)
{
    char buf[MAX_BUF];

    if (params == NULL) {
      new_draw_info(NDI_UNIQUE, 0,op,"what bugs?");
      return 1;
    }
    strcpy(buf,op->name);
    strcat(buf," bug-reports: ");
    strncat(buf,++params,MAX_BUF - strlen(buf) );
    buf[MAX_BUF - 1] = '\0';
    bug_report(buf);
    LOG(llevError,"%s\n",buf);
    new_draw_info(NDI_ALL | NDI_UNIQUE, 1, NULL, buf);
    new_draw_info(NDI_UNIQUE, 0,op, "OK, thanks!");
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
  sprintf(errmsg,"Sizeof: object=%ld  player=%ld  map=%ld",
          (long)sizeof(object),(long)sizeof(player),(long)sizeof(mapstruct));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sprintf(errmsg,"%4d used objects:    %8d",ob_used,i=(ob_used*sizeof(object)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_used+=i;  sum_alloc+=i;
  sprintf(errmsg,"%4d free objects:    %8d",ob_free,i=(ob_free*sizeof(object)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sprintf(errmsg,"%4d active objects:  %8d",count_active(), 0);
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i;
  sprintf(errmsg,"%4d players:         %8d",players,i=(players*sizeof(player)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i; sum_used+=i;


  sprintf(errmsg,"%4d maps allocated:  %8d",nrofmaps,
          i=(nrofmaps*sizeof(mapstruct)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i;  sum_used+=nrm*sizeof(mapstruct);
  sprintf(errmsg,"%4d maps in memory:  %8d",nrm,mapmem);
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=mapmem; sum_used+=mapmem;
  sprintf(errmsg,"%4d archetypes:      %8d",anr,i=(anr*sizeof(archetype)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i; sum_used+=i;
  sprintf(errmsg,"%4d animations:      %8d",anims,i=(anims*sizeof(Fontindex)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i; sum_used+=i;

  sprintf(errmsg,"%4d treasurelists    %8d",tlnr,i=(tlnr*sizeof(treasurelist)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i; sum_used+=i;
  sprintf(errmsg,"%4ld treasures        %8d",nroftreasures,
          i=(nroftreasures*sizeof(treasure)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc+=i; sum_used+=i;
  sprintf(errmsg,"%4ld artifacts        %8d", nrofartifacts,
          i=(nrofartifacts*sizeof(artifact)));
  new_draw_info(NDI_UNIQUE, 0,op, errmsg);
  sum_alloc+=i; sum_used +=i;
  sprintf(errmsg,"%4ld artifacts strngs %8d", nrofallowedstr,
          i=(nrofallowedstr*sizeof(linked_char)));
  new_draw_info(NDI_UNIQUE, 0,op, errmsg);
  sum_alloc += i;sum_used+=i;
  sprintf(errmsg,"%4d artifactlists    %8d",alnr,i=(alnr*sizeof(artifactlist)));
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sum_alloc += i; sum_used += i;

  sprintf(errmsg,"Total space allocated:%8d",sum_alloc);
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
  sprintf(errmsg,"Total space used:     %8d",sum_used);
  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
}

/* 
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

    if (!r)
	return;
	/* This should only be possible if regions are not operating on this server. */

    new_draw_info_format(NDI_UNIQUE, 0,op,   
	"You are in %s. \n %s", get_region_longname(r), get_region_msg(r));
}

void current_map_info(object *op) {
    mapstruct *m = op->map;

    if (!m)
	return;

    new_draw_info_format(NDI_UNIQUE, 0,op,   
	"%s (%s) in %s", m->name, m->path, get_name_of_region_for_map(m));

    if (QUERY_FLAG(op,FLAG_WIZ)) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"players:%d difficulty:%d size:%dx%d start:%dx%d timeout %ld",
		 m->players, m->difficulty, 
		 MAP_WIDTH(m), MAP_HEIGHT(m), 
		 MAP_ENTER_X(m), MAP_ENTER_Y(m),
		 MAP_TIMEOUT(m));

    }
    if (m->msg)
	new_draw_info(NDI_UNIQUE, NDI_NAVY, op, m->msg);
}

#ifdef DEBUG_MALLOC_LEVEL
int command_malloc_verify(object *op, char *parms)
{
	extern int malloc_verify(void);

	if (!malloc_verify()) 
		new_draw_info(NDI_UNIQUE, 0,op,"Heap is corrupted.");
	else
		new_draw_info(NDI_UNIQUE, 0,op,"Heap checks out OK.");
	return 1;
	}
#endif

typedef struct
    {
      char namebuf[MAX_BUF];
      int login_order;
    } chars_names;

/*local functon for qsort comparison*/
int name_cmp (chars_names *c1, chars_names *c2)
    {
      return strcasecmp (c1->namebuf, c2->namebuf);
    }

int command_who (object *op, char *params) {
    player *pl;
    uint16 i;
    region *reg;
    char* format;
    int num_players = 0;
    int num_wiz = 0;
    int num_afk = 0;
    char players_str[MAX_BUF];
    chars_names *chars = NULL;
    
    /* 
     * The who formats are defined in config to be blank. They should have been
     * overridden by the settings file, if there are no entries however, it will
     * have stayed blank. Since this probably isn't what is wanted, we will check if
     * new formats have been specified, and if not we will use the old defaults.
     */
    if (!strcmp(settings.who_format,"")) 
    	strcpy(settings.who_format, "%N_the_%t%h%d%n[%m]");
    if (!strcmp(settings.who_wiz_format,"")) 
    	strcpy(settings.who_wiz_format, "%N_the_%t%h%d%nLevel %l [%m](@%i)(%c)");
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
	        new_draw_info(NDI_UNIQUE, 0, op, "who failed - out of memory!");
                return 0;
            } 
	    sprintf(chars[num_players-1].namebuf, "%s", pl->ob->name);
	    chars[num_players-1].login_order = num_players;
	    /*Check for WIZ's & AFK's*/
	    if (QUERY_FLAG(pl->ob,FLAG_WIZ))
	      num_wiz++;
	    if (QUERY_FLAG(pl->ob,FLAG_AFK))
	      num_afk++;
	}
    }
    if (first_player != (player *) NULL) {
    	if (reg == NULL) 
            new_draw_info_format(NDI_UNIQUE, 0, op, "Total Players (%d) -- WIZ(%d) AFK(%d)", 
      	    	num_players, num_wiz, num_afk);
	else if (reg->longname == NULL)
	    new_draw_info_format(NDI_UNIQUE, 0, op, "Total Players in %s (%d) -- WIZ(%d) AFK(%d)", 
      	    	reg->name, num_players, num_wiz, num_afk);
	else
	    new_draw_info_format(NDI_UNIQUE, 0, op, "Total Players in %s (%d) -- WIZ(%d) AFK(%d)", 
      	    	reg->longname, num_players, num_wiz, num_afk);
    }
    qsort (chars, num_players, sizeof(chars_names), name_cmp);
    for (i=0;i<num_players;i++)
	display_who_entry(op, find_player(chars[i].namebuf), format);
    free(chars);    
    return 1;
}

/* Display a line of 'who' to op, about pl, using the formatting specified by format */
void display_who_entry(object *op, player *pl, char *format) {
    char tmpbuf[MAX_BUF];
    char outbuf[MAX_BUF];
    sint16 i;
    outbuf[0]='\0'; /* we strcat to this, so reset it here. */
    if (pl==NULL) {
    	LOG(llevError,"display_who_entry(): I was passed a null player");
	return;
    }
    for (i=0;i<=strlen(format);i++) {
        if (format[i]=='%') {
            i++;
            get_who_escape_code_value(tmpbuf,format[i],pl);
            strcat(outbuf, tmpbuf);
        }
        else if (format[i]=='_')
            strcat(outbuf," "); /* allow '_' to be used in place of spaces */
        else {
            sprintf(tmpbuf,"%c",format[i]);
            strcat(outbuf,tmpbuf);
        }
    }
    new_draw_info(NDI_UNIQUE, 0, op, outbuf);
}

/* Returns the value of the escape code used in the who format specifier
 * the values are:
 * N	Name of character
 * t	title of character
 * c	count
 * n	newline
 * h	[Hostile] if character is hostile, nothing otherwise
 * d	[WIZ] if character is a dm, nothing otherwise
 * a	[AFK] if character is afk, nothing otherwise
 * l	the level of the character
 * m	the map path the character is currently on
 * M	the map name of the map the character is currently on
 * r	the region name (eg scorn, wolfsburg)
 * R	the regional title (eg The Kingdom of Scorn, The Port of Wolfsburg)
 * i	player's ip adress
 * %	a literal %
 * _	a literal underscore
 */
  
void get_who_escape_code_value(char *return_val, const char letter, player *pl) {
    
    switch (letter) {
	case 'N' :    strcpy(return_val, pl->ob->name);
		          break;
	case 't' :    strcpy(return_val,(pl->own_title[0]=='\0'?pl->title:pl->own_title));
			  break;
	case 'c' :    sprintf(return_val,"%d",pl->ob->count);
			  break;
	case 'n' :    strcpy(return_val, "\n\0"); 
		          break;
	case 'h' :    strcpy(return_val,pl->peaceful?"":" [Hostile]");
			  break;
	case 'l' :    sprintf(return_val,"%d",pl->ob->level);
			  break;
	case 'd' :    strcpy(return_val,(QUERY_FLAG(pl->ob,FLAG_WIZ)?" [WIZ]":""));
			  break;
	case 'a' :    strcpy(return_val,(QUERY_FLAG(pl->ob,FLAG_AFK)?" [AFK]":""));
			  break;
	case 'm' :    strcpy(return_val,pl->ob->map->path);
		 	  break;
	case 'M' :    strcpy(return_val,pl->ob->map->name);
			  break;		
	case 'r' :    strcpy(return_val,get_name_of_region_for_map(pl->ob->map));
			  break;
	case 'R' :    strcpy(return_val,get_region_longname(get_region_by_map(pl->ob->map)));
			  break;
	case 'i' :    strcpy(return_val,pl->socket.host);
			  break;
	case '%' :    strcpy(return_val, "%\0"); 
		          break;
	case '_' :    strcpy(return_val, "_\0"); 
		          break;
    }

}


int command_afk (object *op, char *params)
{
    if QUERY_FLAG(op,FLAG_AFK) { 
       CLEAR_FLAG(op,FLAG_AFK);
       new_draw_info(NDI_UNIQUE, 0, op, "You are no longer AFK");
    }    
    else 
    {
       SET_FLAG(op,FLAG_AFK);
       new_draw_info(NDI_UNIQUE, 0, op, "You are now AFK");
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
    ss_dump_statistics();
    new_draw_info(NDI_UNIQUE, 0,op,errmsg);
    new_draw_info(NDI_UNIQUE, 0,op,ss_dump_table(2));
    return 1;
  }

#ifdef DEBUG
int command_sstable (object *op, char *params)
{
    ss_dump_table(1);
    return 1;
  }
#endif

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
	new_draw_info_format(NDI_UNIQUE, 0, op, "Real temp: %d",
	    real_world_temperature(op->x, op->y, op->map));
	new_draw_info_format(NDI_UNIQUE, 0, op, "Base temp: %d",
	    weathermap[wx][wy].temp);
	new_draw_info_format(NDI_UNIQUE, 0, op, "Humid: %d",
	    weathermap[wx][wy].humid);
	new_draw_info_format(NDI_UNIQUE, 0, op, "Wind: dir=%d speed=%d",
	    weathermap[wx][wy].winddir, weathermap[wx][wy].windspeed);
	new_draw_info_format(NDI_UNIQUE, 0, op, "Pressure: %d",
	    weathermap[wx][wy].pressure);
	new_draw_info_format(NDI_UNIQUE, 0, op, "Avg Elevation: %d",
	    weathermap[wx][wy].avgelev);
	new_draw_info_format(NDI_UNIQUE, 0, op, "Rainfall: %d  Water: %d",
	    weathermap[wx][wy].rainfall, weathermap[wx][wy].water);
    }

    temp = real_world_temperature(op->x, op->y, op->map);
    new_draw_info_format(NDI_UNIQUE, 0, op, "It's currently %d degrees "
	"Centigrade out.", temp);

    /* humid */
    if (weathermap[wx][wy].humid < 20)
	new_draw_info(NDI_UNIQUE, 0, op, "It is very dry.");
    else if (weathermap[wx][wy].humid < 40)
	new_draw_info(NDI_UNIQUE, 0, op, "It is very comfortable today.");
    else if (weathermap[wx][wy].humid < 60)
	new_draw_info(NDI_UNIQUE, 0, op, "It is a bit muggy.");
    else if (weathermap[wx][wy].humid < 80)
	new_draw_info(NDI_UNIQUE, 0, op, "It is muggy.");
    else
	new_draw_info(NDI_UNIQUE, 0, op, "It is uncomfortably muggy.");

    /* wind */
    switch (weathermap[wx][wy].winddir) {
    case 1: sprintf(buf, "north"); break;
    case 2: sprintf(buf, "northeast"); break;
    case 3: sprintf(buf, "east"); break;
    case 4: sprintf(buf, "southeast"); break;
    case 5: sprintf(buf, "south"); break;
    case 6: sprintf(buf, "southwest"); break;
    case 7: sprintf(buf, "west"); break;
    case 8: sprintf(buf, "northwest"); break;
    }
    if (weathermap[wx][wy].windspeed < 5)
	new_draw_info_format(NDI_UNIQUE, 0, op, "There is a mild breeze "
	    "coming from the %s.", buf);
    else if (weathermap[wx][wy].windspeed < 10)
	new_draw_info_format(NDI_UNIQUE, 0, op, "There is a strong breeze "
	    "coming from the %s.", buf);
    else if (weathermap[wx][wy].windspeed < 15)
	new_draw_info_format(NDI_UNIQUE, 0, op, "There is a light wind "
	    "coming from the %s.", buf);
    else if (weathermap[wx][wy].windspeed < 25)
	new_draw_info_format(NDI_UNIQUE, 0, op, "There is a strong wind "
	    "coming from the %s.", buf);
    else if (weathermap[wx][wy].windspeed < 35)
	new_draw_info_format(NDI_UNIQUE, 0, op, "There is a heavy wind "
	    "coming from the %s.", buf);
    else
	new_draw_info_format(NDI_UNIQUE, 0, op, "The wind from the %s is "
	    "incredibly strong!", buf);

    sky = weathermap[wx][wy].sky;
    if (temp <= 0 && sky > SKY_OVERCAST && sky < SKY_FOG)
	sky += 10; /*let it snow*/
    switch (sky) {
    case SKY_CLEAR: new_draw_info(NDI_UNIQUE, 0, op, "There isn''t a cloud in the sky."); break;
    case SKY_LIGHTCLOUD: new_draw_info(NDI_UNIQUE, 0, op, "There are a few light clouds in the sky."); break;
    case SKY_OVERCAST: new_draw_info(NDI_UNIQUE, 0, op, "The sky is cloudy and dreary."); break;
    case SKY_LIGHT_RAIN: new_draw_info(NDI_UNIQUE, 0, op, "It is raining softly."); break;
    case SKY_RAIN: new_draw_info(NDI_UNIQUE, 0, op, "It is raining."); break;
    case SKY_HEAVY_RAIN: new_draw_info(NDI_UNIQUE, 0, op, "It is raining heavily."); break;
    case SKY_HURRICANE: new_draw_info(NDI_UNIQUE, 0, op, "There is a heavy storm!  You should go inside!"); break;
    case SKY_FOG: new_draw_info(NDI_UNIQUE, 0, op, "It''s foggy and miserable."); break;
    case SKY_HAIL: new_draw_info(NDI_UNIQUE, 0, op, "It''s hailing out!  Take cover!"); break;
    case SKY_LIGHT_SNOW: new_draw_info(NDI_UNIQUE, 0, op, "Snow is gently falling from the sky."); break;
    case SKY_SNOW: new_draw_info(NDI_UNIQUE, 0, op, "It''s snowing out."); break;
    case SKY_HEAVY_SNOW: new_draw_info(NDI_UNIQUE, 0, op, "The snow is falling very heavily now."); break;
    case SKY_BLIZZARD: new_draw_info(NDI_UNIQUE, 0, op, "A full blown blizzard is in effect.  You might want to take cover!"); break;
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
    char buf[MAX_BUF];
  if(params==NULL || !sscanf(params, "%d", &i)) {
      sprintf(buf,"Global debug level is %d.",settings.debug);
      new_draw_info(NDI_UNIQUE, 0,op,buf);
      return 1;
    }
  if(op != NULL && !QUERY_FLAG(op, FLAG_WIZ)) {
      new_draw_info(NDI_UNIQUE, 0,op,"Privileged command.");
      return 1;
    }
    settings.debug = (enum LogLevel) FABS(i);
    sprintf(buf,"Set debug level to %d.", i);
    new_draw_info(NDI_UNIQUE, 0,op,buf);
    return 1;
  }


/*
 * Those dumps should be just one dump with good parser
 */

int command_dumpbelow (object *op, char *params)
{
  if (op && op->below) {
	  dump_object(op->below);
	  new_draw_info(NDI_UNIQUE, 0,op,errmsg);
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
          new_draw_info(NDI_UNIQUE, 0,op, "You will now walk through walls.\n");
	  SET_FLAG(op, FLAG_WIZPASS);
  } else {
    new_draw_info(NDI_UNIQUE, 0,op, "You will now be stopped by walls.\n");
    CLEAR_FLAG(op, FLAG_WIZPASS);
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
      (void) ss_dump_table(1);
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


#ifndef BUG_LOG
#define BUG_LOG "bug_log"
#endif
void bug_report(char * reportstring){
  FILE * fp;
  if((fp = fopen( BUG_LOG , "a")) != NULL){
      fprintf(fp,"%s\n", reportstring);
      fclose(fp);
  } else {
      perror(BUG_LOG);
  }
}

int command_output_sync(object *op, char *params)
{
    int val;

    if (!params) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"Output sync time is presently %d", op->contr->outputs_sync);
	return 1;
    }
    val=atoi(params);
    if (val>0) {
	op->contr->outputs_sync = val;
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"Output sync time now set to %d", op->contr->outputs_sync);
    }
    else
	new_draw_info(NDI_UNIQUE, 0, op,"Invalid value for output_sync.");

    return 1;
}

int command_output_count(object *op, char *params)
{
    int val;

    if (!params) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"Output count is presently %d", op->contr->outputs_count);
	return 1;
    }
    val=atoi(params);
    if (val>0) {
	op->contr->outputs_count = val;
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"Output count now set to %d", op->contr->outputs_count);
    }
    else
	new_draw_info(NDI_UNIQUE, 0, op,"Invalid value for output_count.");

    return 1;
}

int command_listen (object *op, char *params)
{
  int i;

  if(params==NULL || !sscanf(params, "%d", &i)) {
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"Set listen to what (presently %d)?", op->contr->listening);
      return 1;
    }
    op->contr->listening=(char) i;
    new_draw_info_format(NDI_UNIQUE, 0, op,
	"Your verbose level is now %d.",i);
    return 1;
}

/* Prints out some useful information for the character.  Everything we print
 * out can be determined by the docs, so we aren't revealing anything extra -
 * rather, we are making it convenient to find the values.  params have
 * no meaning here.
 */
int command_statistics(object *pl, char *params)
{
    if (!pl->contr) return 1;
#ifndef WIN32
    new_draw_info_format(NDI_UNIQUE, 0, pl,"  Experience: %lld",pl->stats.exp);
    new_draw_info_format(NDI_UNIQUE, 0, pl,"  Next Level: %lld",level_exp(pl->level+1, pl->expmul));
#else
    new_draw_info_format(NDI_UNIQUE, 0, pl,"  Experience: %I64d",pl->stats.exp);
    new_draw_info_format(NDI_UNIQUE, 0, pl,"  Next Level: %I64d",level_exp(pl->level+1, pl->expmul));
#endif
    new_draw_info(NDI_UNIQUE, 0, pl,       "\nStat       Nat/Real/Max");

    new_draw_info_format(NDI_UNIQUE, 0, pl, "Str         %2d/ %3d/%3d",
	pl->contr->orig_stats.Str, pl->stats.Str, 20+pl->arch->clone.stats.Str);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "Dex         %2d/ %3d/%3d",
	pl->contr->orig_stats.Dex, pl->stats.Dex, 20+pl->arch->clone.stats.Dex);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "Con         %2d/ %3d/%3d",
	pl->contr->orig_stats.Con, pl->stats.Con, 20+pl->arch->clone.stats.Con);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "Int         %2d/ %3d/%3d",
	pl->contr->orig_stats.Int, pl->stats.Int, 20+pl->arch->clone.stats.Int);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "Wis         %2d/ %3d/%3d",
	pl->contr->orig_stats.Wis, pl->stats.Wis, 20+pl->arch->clone.stats.Wis);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "Pow         %2d/ %3d/%3d",
	pl->contr->orig_stats.Pow, pl->stats.Pow, 20+pl->arch->clone.stats.Pow);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "Cha         %2d/ %3d/%3d",
	pl->contr->orig_stats.Cha, pl->stats.Cha, 20+pl->arch->clone.stats.Cha);
    new_draw_info_format(NDI_UNIQUE, 0, pl, "\nAttack Mode: %s",pl->contr->peaceful? "Peaceful":"Hostile");

    /* If dragon player, let's display natural resistances */
    if ( is_dragon_pl( pl ) )
        {
        int attack;
        object* tmp;
        for ( tmp = pl->inv; tmp != NULL; tmp = tmp->below )
            {
            if ( ( tmp->type == FORCE ) && ( strcmp( tmp->arch->name, "dragon_skin_force" )== 0 ) )
                {
                new_draw_info( NDI_UNIQUE, 0, pl, "\nNatural skin resistances:" );
                for ( attack = 0; attack < NROFATTACKS; attack++ )
                    {
                    if ( atnr_is_dragon_enabled( attack ) )
                        {
                        new_draw_info_format( NDI_UNIQUE, 0, pl, "%s: %d", change_resist_msg[ attack ], tmp->resist[ attack ] );
                        }
                    }
                break;
                }
            }
        }

   /* Can't think of anything else to print right now */
   return 0;
}

int command_fix_me(object *op, char *params)
{
    sum_weight(op);
    fix_player(op);
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
		    if ((Stat.st_mode & S_IFMT)==S_IFDIR) {
			char buf2[MAX_BUF];
			struct tm *tm=localtime(&Stat.st_mtime);
			sprintf(buf2,"%s\t%04d %02d %02d %02d %02d %02d",
				  Entry->d_name,
				  1900+tm->tm_year,
				  1+tm->tm_mon,
				  tm->tm_mday,
				  tm->tm_hour,
				  tm->tm_min,
				  tm->tm_sec);
			new_draw_info(NDI_UNIQUE, 0, op, buf2);
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
		new_draw_info(NDI_UNIQUE,0,op,"Kill-logs are sent to:");
		first=0;
	    }
	    new_draw_info_format(NDI_UNIQUE, 0, op, "%s: %s",
				 init_sockets[i].host,init_sockets[i].comment);
	}
    }
    if (first) {
	new_draw_info(NDI_UNIQUE,0,op,"Nobody is currently logging kills.");
    }
    return 1;
}

int command_applymode(object *op, char *params)
{
    unapplymode unapply = op->contr->unapply;
    static char *types[]={"nochoice", "never", "always"};

    if (!params) {
	new_draw_info_format(NDI_UNIQUE, 0, op, "applymode is set to %s",
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
	new_draw_info_format(NDI_UNIQUE, 0, op,
	    "applymode: Unknown options %s, valid options are nochoice, never, always",
			     params);
	return 0;
    }
    new_draw_info_format(NDI_UNIQUE, 0, op, "Applymode %s set to %s",
	(unapply==op->contr->unapply?"":" now"),
	types[op->contr->unapply]);
    return 1;
}

int command_bowmode(object *op, char *params)
{
    bowtype_t oldtype=op->contr->bowtype;
    static char *types[] =
	{"normal", "threewide", "spreadshot", "firenorth",
	 "firene", "fireeast", "firese", "firesouth",
	 "firesw", "firewest", "firenw", "bestarrow"};
    char buf[MAX_BUF];
    int i, found;

    if (!params) {
	new_draw_info_format(NDI_UNIQUE, 0, op, "bowmode is set to %s",
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
	new_draw_info_format(NDI_UNIQUE, 0, op, buf);
	return 0;
    }
    new_draw_info_format(NDI_UNIQUE, 0, op, "bowmode %s set to %s",
	(oldtype==op->contr->bowtype?"":"now"),
	types[op->contr->bowtype]);
    return 1;
}

int command_petmode(object *op, char *params)
{
    petmode_t oldtype=op->contr->petmode;
    static char *types[]={"normal", "sad", "defend"};

    if (!params) {
	new_draw_info_format(NDI_UNIQUE, 0, op, "petmode is set to %s",
	types[op->contr->petmode]);
	return 1;
    }

    if (!strcmp(params,"normal")) 
	op->contr->petmode=pet_normal;
    else if (!strcmp(params,"sad")) 
	op->contr->petmode=pet_sad;
    else if (!strcmp(params,"defend")) 
	op->contr->petmode=pet_defend;
    else {
	new_draw_info_format(NDI_UNIQUE, 0, op,
	    "petmode: Unknown options %s, valid options are normal,"
	    "sad (seek and destroy), defend", params);
	return 0;
    }
    new_draw_info_format(NDI_UNIQUE, 0, op, "petmode %s set to %s",
	(oldtype==op->contr->petmode?"":"now"),
	types[op->contr->petmode]);
    return 1;
}

int command_usekeys(object *op, char *params)
{
    usekeytype oldtype=op->contr->usekeys;
    static char *types[]={"inventory", "keyrings", "containers"};

    if (!params) {
	new_draw_info_format(NDI_UNIQUE, 0, op, "usekeys is set to %s",
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
	new_draw_info_format(NDI_UNIQUE, 0, op,
	    "usekeys: Unknown options %s, valid options are inventory, keyrings, containers",
			     params);
	return 0;
    }
    new_draw_info_format(NDI_UNIQUE, 0, op, "usekeys %s set to %s",
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

	new_draw_info_format(NDI_UNIQUE, 0, op, "%-20s %+5d", 
		attacktype_desc[i], op->resist[i]);
    }
  return 0;
}
/*
 * Actual commands.
 * Those should be in small separate files (c_object.c, c_wiz.c, cmove.c,...)
 */


static void help_topics(object *op, int what)
{
    DIR *dirp;
    struct dirent *de;
    char filename[MAX_BUF], line[80];
    int namelen, linelen=0;
  
    switch (what) {
	case 1:
	    sprintf(filename, "%s/wizhelp", settings.datadir);
	    new_draw_info(NDI_UNIQUE, 0,op, "      Wiz commands:");
	    break;
	case 3:
	    sprintf(filename, "%s/mischelp", settings.datadir);
	    new_draw_info(NDI_UNIQUE, 0,op, "      Misc help:");
	    break;
	default:
	    sprintf(filename, "%s/help", settings.datadir);
	    new_draw_info(NDI_UNIQUE, 0,op, "      Commands:");
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
	linelen +=namelen+1;
	if (linelen > 42) {
	    new_draw_info(NDI_UNIQUE, 0,op, line);
	    sprintf(line, " %s", de->d_name);
	    linelen =namelen+1;
	    continue;
	}
	strcat(line, " ");
	strcat(line, de->d_name);
    }
    new_draw_info(NDI_UNIQUE, 0,op, line);
    closedir(dirp);
}

static void show_commands(object *op, int what)
{
  char line[80];
  int i, size, namelen, linelen=0;
  CommArray_s *ap;
  extern CommArray_s Commands[], WizCommands[];
  extern const int CommandsSize, WizCommandsSize;
  
  switch (what) {
  case 1:
    ap =WizCommands;
    size =WizCommandsSize;
    new_draw_info(NDI_UNIQUE, 0,op, "      Wiz commands:");
    break;
  case 2:
    ap= CommunicationCommands;
    size= CommunicationCommandSize;
    new_draw_info(NDI_UNIQUE, 0, op, "      Communication commands:");
    break;
  default:
    ap =Commands;
    size =CommandsSize;
    new_draw_info(NDI_UNIQUE, 0,op, "      Commands:");
    break;
  }

  line[0] ='\0';
  for (i=0; i<size; i++) {
    namelen = strlen(ap[i].name);
    linelen +=namelen+1;
    if (linelen > 42) {
      new_draw_info(NDI_UNIQUE, 0,op, line);
      sprintf(line, " %s", ap[i].name);
      linelen =namelen+1;
      continue;
    }
    strcat(line, " ");
    strcat(line, ap[i].name);
  }	       
  new_draw_info(NDI_UNIQUE, 0,op, line);
}


int command_help (object *op, char *params)
{
  struct stat st;
  FILE *fp;
  char filename[MAX_BUF], line[MAX_BUF];
  int len;

  if(op != NULL)
    clear_win_info(op);

/*
   * Main help page?
 */
  if (!params) {
    sprintf(filename, "%s/def_help", settings.datadir);
    if ((fp=fopen(filename, "r")) == NULL) {
      LOG(llevError, "Can't open %s\n", filename);
      perror("Can't read default help");
      return 0;
    }
    while (fgets(line, MAX_BUF, fp)) {
      line[MAX_BUF-1] ='\0';
      len =strlen(line)-1;
      if (line[len] == '\n')
	line[len] ='\0';
      new_draw_info(NDI_UNIQUE, 0,op, line);
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
    show_commands(op, 2); /* show comm commands */
    if (QUERY_FLAG(op, FLAG_WIZ))
      show_commands(op, 1);
    return 0;
  }

  /*
   * User wants info about command
   */
  if (strchr(params, '.') || strchr(params, ' ') || strchr(params, '/')) {
    sprintf(line, "Illegal characters in '%s'", params);
    new_draw_info(NDI_UNIQUE, 0,op, line);
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
    LOG(llevError, "Can't open %s\n", filename);
    perror("Can't read helpfile");
    return 0;
      }
  sprintf(line, "Help about '%s'", params);
  new_draw_info(NDI_UNIQUE, 0,op, line);
  while (fgets(line, MAX_BUF, fp)) {
    line[MAX_BUF-1] ='\0';
    len =strlen(line)-1;
    if (line[len] == '\n')
      line[len] ='\0';
    new_draw_info(NDI_UNIQUE, 0,op, line);
    }
  fclose(fp);
  return 0;

  /*
   * No_help -escape
   */
 nohelp:
  sprintf(line, "No help availble on '%s'", params);
  new_draw_info(NDI_UNIQUE, 0,op, line);
  return 0;
}


int onoff_value(char *line)
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

/*
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
	new_draw_info(NDI_UNIQUE, 0,op,"You can not enter explore mode if you are in a party");
    } else if (op->contr->explore)
	new_draw_info(NDI_UNIQUE, 0,op, "There is no return from explore mode");
    else {
	op->contr->explore=1;
	new_draw_info(NDI_UNIQUE, 0,op, "You are now in explore mode");
    }
    return 1;
}

int command_sound (object *op, char *params)
{
    if (op->contr->socket.sound) {
        op->contr->socket.sound=0;
        new_draw_info(NDI_UNIQUE, 0,op, "Silence is golden...");
    }
    else {
        op->contr->socket.sound=1;
        new_draw_info(NDI_UNIQUE, 0,op, "The sounds are enabled.");
    }
    return 1;
}

/* Perhaps these should be in player.c, but that file is
 * already a bit big.
 */

void receive_player_name(object *op,char k) {

    if(!check_name(op->contr,op->contr->write_buf+1)) {
	get_name(op);
	return;
    }
    FREE_AND_COPY(op->name, op->contr->write_buf+1);
    FREE_AND_COPY(op->name_pl, op->contr->write_buf+1);
    new_draw_info(NDI_UNIQUE, 0,op,op->contr->write_buf);
    op->contr->name_changed=1;
    get_password(op);
}

void receive_player_password(object *op,char k) {

  unsigned int pwd_len=strlen(op->contr->write_buf);
  if(pwd_len<=1||pwd_len>17) {
    get_name(op);
    return;
  }
  new_draw_info(NDI_UNIQUE, 0,op,"          "); /* To hide the password better */
  if(op->contr->state==ST_CONFIRM_PASSWORD) {
    if(!check_password(op->contr->write_buf+1,op->contr->password)) {
      new_draw_info(NDI_UNIQUE, 0,op,"The passwords did not match.");
      get_name(op);
      return;
    }
    clear_win_info(op);
    display_motd(op);
    new_draw_info(NDI_UNIQUE, 0,op," ");
    new_draw_info(NDI_UNIQUE, 0,op,"Welcome, Brave New Warrior!");
    new_draw_info(NDI_UNIQUE, 0,op," ");
    Roll_Again(op);
    op->contr->state=ST_ROLL_STAT;
    return;
  }
  strcpy(op->contr->password,crypt_string(op->contr->write_buf+1,NULL));
  op->contr->state=ST_ROLL_STAT;
  check_login(op);
  return;
}


int explore_mode() {
    player *pl;

    if (settings.explore_mode == TRUE) {
	for (pl = first_player; pl != (player *) NULL; pl = pl->next)
	    if (pl->explore)
		return 1;
    }
    return 0;
}


int command_title (object *op, char *params)
{
    char buf[MAX_BUF];
    
    if (settings.set_title == FALSE) {
	new_draw_info(NDI_UNIQUE, 0, op, "You cannot change your title.");
	return 1;
    }

    /* dragon players cannot change titles */
    if (is_dragon_pl(op)) {
        new_draw_info(NDI_UNIQUE, 0, op, "Dragons cannot change titles.");
        return 1;
    }
    
    if(params == NULL) {
	if(op->contr->own_title[0]=='\0')
	    sprintf(buf,"Your title is '%s'.", op->contr->title);
	else
	    sprintf(buf,"Your title is '%s'.", op->contr->own_title);
	new_draw_info(NDI_UNIQUE, 0,op,buf);
	return 1;
    }
    if(strcmp(params, "clear")==0 || strcmp(params, "default")==0) {
	if(op->contr->own_title[0]=='\0')
	    new_draw_info(NDI_UNIQUE, 0,op,"Your title is the default title.");
	else
	    new_draw_info(NDI_UNIQUE, 0,op,"Title set to default.");
	op->contr->own_title[0]='\0';
	return 1;
    }

    if((int)strlen(params) >= MAX_NAME) {
	new_draw_info(NDI_UNIQUE, 0,op,"Title too long.");
	return 1;
    }
    strcpy(op->contr->own_title, params);
    return 1;
}

int command_save (object *op, char *params)
{
    if (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL) & P_NO_CLERIC) {
	new_draw_info(NDI_UNIQUE, 0, op, "You can not save on unholy ground");
    } else if (!op->stats.exp) {
	new_draw_info(NDI_UNIQUE, 0, op, "You don't deserve to save yet.");
    } else {
	if(save_player(op,1))
	    new_draw_info(NDI_UNIQUE, 0,op,"You have been saved.");
	else
	    new_draw_info(NDI_UNIQUE, 0,op,"SAVE FAILED!");
    }
    return 1;
}


int command_peaceful (object *op, char *params)
{
    if((op->contr->peaceful=!op->contr->peaceful))
	new_draw_info(NDI_UNIQUE, 0,op,"You will not attack other players.");
    else
	new_draw_info(NDI_UNIQUE, 0,op,"You will attack other players.");
    return 1;
}



int command_wimpy (object *op, char *params)
{
    int i;
    char buf[MAX_BUF];

    if (params==NULL || !sscanf(params, "%d", &i)) {
	sprintf(buf, "Your current wimpy level is %d.", op->run_away);
	new_draw_info(NDI_UNIQUE, 0,op, buf);
	return 1;
    }
    sprintf(buf, "Your new wimpy level is %d.", i);
    new_draw_info(NDI_UNIQUE, 0,op, buf);
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
    new_draw_info(NDI_UNIQUE, 0,op, "You are braced.");
  else
    new_draw_info(NDI_UNIQUE, 0,op, "Not braced.");

      fix_player(op);
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
    new_draw_info_format(NDI_UNIQUE, 0, op, "Style maps loaded:    %d", maps_used);
    new_draw_info(NDI_UNIQUE, 0, op, "Memory used, not");
    new_draw_info_format(NDI_UNIQUE, 0, op, "including objects:    %d", mapmem);
    new_draw_info_format(NDI_UNIQUE, 0, op, "Style objects:        %d", objects_used);
    new_draw_info_format(NDI_UNIQUE, 0, op, "Mem for objects:      %d", objects_used * sizeof(object));
    return 0;
}

int command_kill_pets(object *op, char *params)
{
    terminate_all_pets(op);
    new_draw_info(NDI_UNIQUE, 0, op, "Your pets have been killed.");
    return 0;
}

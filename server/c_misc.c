/*
 * static char *rcsid_c_misc_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1992 Mark Wedel
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

    The author can be reached via e-mail to master@rahul.net
*/
#include <global.h>
#include <loader.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif


/* Handles misc. input request - things like hash table, malloc, maps,
 * who, etc.
 */

void map_info(object *op) {
  mapstruct *m;
  char buf[MAX_BUF], map_path[MAX_BUF];
  long sec = seconds();
#ifdef MAP_RESET
  new_draw_info_format(NDI_UNIQUE, 0, op,
	"Current time is: %02ld:%02ld:%02ld.",
	  (sec%86400)/3600,(sec%3600)/60,sec%60);
  new_draw_info(NDI_UNIQUE, 0,op,"Path               Pl PlM IM   TO Dif Pen Reset");
#else
  new_draw_info(NDI_UNIQUE, 0,op,"Pl Pl-M IM   TO Dif Pen");
#endif
  for(m=first_map;m!=NULL;m=m->next) {
#ifndef MAP_RESET
    if (m->in_memory == MAP_SWAPPED)
      continue;
#endif
    /* Print out the last 18 characters of the map name... */
    if (strlen(m->path)<=18) strcpy(map_path, m->path);
    else strcpy(map_path, m->path + strlen(m->path) - 18);
#ifndef MAP_RESET
      sprintf(buf,"%-18.18s %2ld %2d   %1ld %4ld %2ld  %2d",
              map_path, m->players,players_on_map(m),m->in_memory,m->timeout,
              m->difficulty, count_pending(m));
#else
      sprintf(buf,"%-18.18s %2d %2d   %1d %4d %2d  %2d %02ld:%02ld:%02ld",
              map_path, m->players,players_on_map(m),
              m->in_memory,m->timeout,m->difficulty, count_pending(m),
              (m->reset_time%86400)/3600,(m->reset_time%3600)/60,
              m->reset_time%60);
#endif
    new_draw_info(NDI_UNIQUE, 0,op,buf);
  }
}

int command_spell_reset(object *op, char *params)
{
	init_spell_param();
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
    if(m->in_memory == MAP_IN_MEMORY)
      mapmem+=m->mapx*m->mapy*(sizeof(object *)+sizeof(unsigned char *)*2),
      nrm++;
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
  sprintf(errmsg,"%4d spells:          %8d",NROFREALSPELLS,
          i=(NROFREALSPELLS*sizeof(spell)));
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

int count_pending(mapstruct *map) {
  objectlink *obl;
  int i;

  for(i=0, obl = map->pending; obl != NULL; obl = obl->next)
    i++;
  return i;
}

void current_map_info(object *op) {
    mapstruct *m = op->map;

    if (!m)
	return;

    new_draw_info_format(NDI_UNIQUE, 0,op,   
	"%s (%s)", m->map_object->name, m->path);

    if (QUERY_FLAG(op,FLAG_WIZ)) {
	new_draw_info_format(NDI_UNIQUE, 0, op,
		"players:%d difficulty:%d size:%dx%d start:%dx%d timeout %ld", 
		 m->players, m->difficulty, 
		 m->mapx, m->mapy, 
		 EXIT_X(m->map_object), EXIT_Y(m->map_object),
		 MAP_TIMEOUT(m));

    }
    if (m->map_object->msg)
	new_draw_info(NDI_UNIQUE, NDI_NAVY, op, m->map_object->msg);
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

int command_who (object *op, char *params)
{
    player *pl;
    char buf[MAX_BUF];

    if (first_player != (player *) NULL)
	new_draw_info(NDI_UNIQUE, 0,op,"Players:");

    for(pl=first_player;pl!=NULL;pl=pl->next) {
	if(pl->ob->map == NULL)
	    continue;
	if (pl->state==ST_PLAYING || pl->state==ST_GET_PARTY_PASSWORD) {

	    /* Any reason one sprintf can't be used?  The are displaying all
	     * the same informaitn, except one display pl->ob->count.
	     */

	    if(op == NULL || QUERY_FLAG(op, FLAG_WIZ))
		(void) sprintf(buf,"%s the %s (@%s) [%s]%s%s%s (%d)",pl->ob->name,
		       (pl->own_title[0]=='\0'?pl->title:pl->own_title),
		       pl->socket.host,
		       pl->ob->map->path,
		       QUERY_FLAG(pl->ob,FLAG_WIZ)?" [WIZ]":"",pl->idle?" I":"",
		       pl->peaceful?"P":"W",pl->ob->count);
	    else
		(void) sprintf(buf,"%s the %s (@%s) [%s]%s%s%s",pl->ob->name,
		       (pl->own_title[0]=='\0'?pl->title:pl->own_title),
		       pl->socket.host,
		       pl->ob->map->path,
		       QUERY_FLAG(pl->ob,FLAG_WIZ)?" [WIZ]":"",pl->idle?" I":"",
		       pl->peaceful?"P":"W");
	    new_draw_info(NDI_UNIQUE, 0,op,buf);
	}
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

int command_maps (object *op, char *params)
{
    map_info(op);
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

int command_dumplights (object *op, char *params)
{
	if(op) dump_map_lights(op->map);
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
    new_draw_info_format(NDI_UNIQUE, 0, pl,"  Experience: %d",pl->stats.exp);
    new_draw_info_format(NDI_UNIQUE, 0, pl,"  Next Level: %d",level_exp(pl->level+1, pl->expmul));
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

   /* Can't think of anything else to print right now */
   return 0;
}

int command_fix_me(object *op, char *params)
{
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

/*
 * static char *rcsid_standalone_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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

#define LO_NEWFILE 2

/* the main routine for making a standalone version. */

#include <time.h>
#include <stdio.h>
#include <global.h>
#include <maze_gen.h>
#include <room_gen.h>
#include <random_map.h>
#include <rproto.h>

int main(int argc, char *argv[]) {
  char InFileName[1024],OutFileName[1024];
  mapstruct *newMap;
  RMParms rp;
  FILE *fp;

  if(argc < 3) {
    printf("\nUsage:  %s inputfile outputfile\n",argv[0]);
    exit(0);
  }
  strcpy(InFileName,argv[1]);
  strcpy(OutFileName,argv[2]);

  init_globals();
  init_library();
  init_archetypes();
  init_artifacts();
  init_formulae();
  init_readable();

  init_gods();
  memset(&rp, 0, sizeof(RMParms));
  rp.Xsize=-1;
  rp.Ysize=-1;
  if ((fp=fopen(InFileName, "r"))==NULL) {
    fprintf(stderr,"\nError: can not open %s\n", InFileName);
    exit(1);
  }
  load_parameters(fp, LO_NEWFILE, &rp);
  fclose(fp);
  newMap = generate_random_map(OutFileName, &rp, NULL);
  save_map(newMap,1); 
  exit(0);
}

void set_map_timeout(void) {}   /* doesn't need to do anything */

#include <global.h>


/* some plagarized code from apply.c--I needed just these two functions
without all the rest of the junk, so.... */
int auto_apply (object *op) {
  object *tmp = NULL;
  int i;

  switch(op->type) {
  case SHOP_FLOOR:
    if (!HAS_RANDOM_ITEMS(op)) return 0;
    do {
      i=10; /* let's give it 10 tries */
      while((tmp=generate_treasure(op->randomitems,op->stats.exp?
		op->stats.exp:5))==NULL&&--i);
      if(tmp==NULL)
	  return 0;
      if(QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED))
      {
        free_object(tmp);
        tmp = NULL;
      }
    } while(!tmp);

    tmp->x=op->x,tmp->y=op->y;
    SET_FLAG(tmp,FLAG_UNPAID);
    insert_ob_in_map(tmp,op->map,NULL,0);
    CLEAR_FLAG(op,FLAG_AUTO_APPLY);
    identify(tmp);
    break;

  case TREASURE:
    if (HAS_RANDOM_ITEMS(op))
      while ((op->stats.hp--)>0)
        create_treasure(op->randomitems, op, GT_ENVIRONMENT,
    op->stats.exp ? op->stats.exp :
    op->map == NULL ?  14: op->map->difficulty,0);
    remove_ob(op);
    free_object(op);
    break;
  }

  return tmp ? 1 : 0;
}

/* fix_auto_apply goes through the entire map (only the first time
 * when an original map is loaded) and performs special actions for
 * certain objects (most initialization of chests and creation of
 * treasures and stuff).  Calls auto_apply if appropriate.
 */

void fix_auto_apply(mapstruct *m) {
  object *tmp,*above=NULL;
  int x,y;

  for(x=0;x<MAP_WIDTH(m);x++)
    for(y=0;y<MAP_HEIGHT(m);y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=above) {
        above=tmp->above;

        if(QUERY_FLAG(tmp,FLAG_AUTO_APPLY))
          auto_apply(tmp);
        else if(tmp->type==TREASURE) {
          if (HAS_RANDOM_ITEMS(tmp))
            while ((tmp->stats.hp--)>0)
              create_treasure(tmp->randomitems, tmp, 0,
                              m->difficulty,0);
        }
        if(tmp && tmp->arch && tmp->type!=PLAYER && tmp->type!=TREASURE &&
           tmp->randomitems){
          if(tmp->type==CONTAINER) {
            if (HAS_RANDOM_ITEMS(tmp))
              while ((tmp->stats.hp--)>0)
                create_treasure(tmp->randomitems, tmp, 0,
                                m->difficulty,0);
          }
          else if (HAS_RANDOM_ITEMS(tmp))
            create_treasure(tmp->randomitems, tmp, GT_APPLY,
                            m->difficulty,0);
        }
      }
  for(x=0;x<MAP_WIDTH(m);x++)
    for(y=0;y<MAP_HEIGHT(m);y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above)
        if (tmp->above
            && (tmp->type == TRIGGER_BUTTON || tmp->type == TRIGGER_PEDESTAL))
          check_trigger(tmp,tmp->above);
}

/**
 * Those are dummy functions defined to resolve all symboles.
 * Added as part of glue cleaning.
 * Ryo 2005-07-15
 **/
 

void draw_ext_info(int flags, int pri, const object *pl, uint8 type, uint8 subtype, const char *txt, const char *txt2)
    {
    fprintf(logfile, "%s\n", txt);
    }

void draw_ext_info_format(
        int flags, int pri, const object *pl, uint8 type, 
        uint8 subtype, 
        const char* new_format, 
        const char* old_format, 
        ...)
{
    va_list ap;
    va_start(ap, old_format);
    vfprintf(logfile, old_format, ap);
    va_end(ap);
}


void ext_info_map(int color, const mapstruct *map, uint8 type, uint8 subtype, const char *str1, const char *str2)
{
    fprintf(logfile, "ext_info_map: %s\n", str2);
}

void move_teleporter( object* ob)
    {
    }
 
void move_firewall( object* ob)
    {
    }
 
void move_duplicator( object* ob)
    {
    }
 
void move_marker( object* ob)
    {
    }
 
void move_creator( object* ob)
    {
    }
 
void emergency_save( int x )
    {
    }
 
void clean_tmp_files( void )
    {
    }
 
void esrv_send_item( object* ob, object* obx )
    {
    }
 
void dragon_ability_gain( object* ob, int x, int y )
    {
    }
 
void weather_effect(mapstruct *m)
    {
    }
 
void set_darkness_map( mapstruct* m)
    {
    }
 
int ob_move_on(object* op, object* victim, object* originator)
    {
    }
 
object* find_skill_by_number( object* ob, int x )
    {
    return NULL;
    }
 
void esrv_del_item(player *pl, int tag)
    {
    }
 
void esrv_update_spells(player *pl)
    {
    }

void monster_check_apply( object* ob, object* obt )
    {
    }
 
void trap_adjust( object* ob, int x )
    {
    }

int execute_event(object* op, int eventcode, object* activator, object* third, const char* message, int fix)
    {
    return 0;
    }

int execute_global_event(int eventcode, ...)
    {
    return 0;
    }

/*
 * static char *rcsid_standalone_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to mark@pyramid.com
*/


/* the main routine for making a standalone version. */

#include <time.h>
#include <stdio.h>
#include <global.h>
#include <maze_gen.h>
#include <room_gen.h>
#include <random_map.h>

int main(int argc, char *argv[]) {
  char InFileName[1024],OutFileName[1024];
  mapstruct *newMap;
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
  newMap = generate_random_map(InFileName,OutFileName);
  new_save_map(newMap,1); 
  exit(0);
}


#include <global.h>


/* some plagarized code from apply.c--I needed just these two functions
without all the rest of the junk, so.... */
int auto_apply (object *op) {
  object *tmp = NULL;
  int i;

  switch(op->type) {
  case SHOP_FLOOR:
    if (op->randomitems==NULL) return 0;
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
    insert_ob_in_map(tmp,op->map);
    CLEAR_FLAG(op,FLAG_AUTO_APPLY);
    identify(tmp);
    break;

  case TREASURE:
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

  for(x=0;x<m->mapx;x++)
    for(y=0;y<m->mapy;y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=above) {
        above=tmp->above;

	if(QUERY_FLAG(tmp,FLAG_AUTO_APPLY))
          auto_apply(tmp);
        else if(tmp->type==TREASURE) {
	  while ((tmp->stats.hp--)>0)
            create_treasure(tmp->randomitems, tmp, GT_INVENTORY,
                            m->difficulty,0);
	}
        if(tmp && tmp->arch && tmp->type!=PLAYER && tmp->type!=TREASURE &&
	   tmp->randomitems){
	  if(tmp->type==CONTAINER) {
	    while ((tmp->stats.hp--)>0)
	      create_treasure(tmp->randomitems, tmp, GT_INVENTORY,
			      m->difficulty,0);
	  }
	  else create_treasure(tmp->randomitems, tmp, GT_INVENTORY,
			      m->difficulty,0);
	}
      }
  for(x=0;x<m->mapx;x++)
    for(y=0;y<m->mapy;y++)
      for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above)
	if (tmp->type == TRIGGER || tmp->type == TRIGGER_BUTTON ||
            tmp->type == TRIGGER_PEDESTAL || tmp->type == TRIGGER_ALTAR)
	  check_trigger(tmp);
}



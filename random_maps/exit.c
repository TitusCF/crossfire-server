/*
 * static char *rcsid_exit_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

#include <global.h>
#include <random_map.h>
#include <sproto.h>
#include <rproto.h>

/* orientation:  0 means random,
  1 means descending dungeon
  2 means ascending dungeon
  3 means rightward
  4 means leftward
  5 means northward
  6 means southward
*/

void place_exits(mapstruct *map, char **maze,char *exitstyle,int orientation,RMParms *RP) {
  char styledirname[256];
  mapstruct *style_map_down=0;  /* harder maze */
  mapstruct *style_map_up=0;    /* easier maze */
  object *the_exit_down;        /* harder maze */
  object *the_exit_up;          /* easier maze */
  object *random_sign;          /* magic mouth saying this is a random map. */
  int cx=-1,cy=-1;  /* location of a map center */

  if(orientation == 0) orientation = RANDOM() % 6 + 1;

  switch(orientation) {
  case 1:
    {
      sprintf(styledirname,"/styles/exitstyles/up");
      style_map_up = find_style(styledirname,exitstyle,-1);
      sprintf(styledirname,"/styles/exitstyles/down");
      style_map_down = find_style(styledirname,exitstyle,-1);
      break;
    }
  case 2:
    {
      sprintf(styledirname,"/styles/exitstyles/down");
      style_map_up = find_style(styledirname,exitstyle,-1);
      sprintf(styledirname,"/styles/exitstyles/up");
      style_map_down = find_style(styledirname,exitstyle,-1);
      break;
    }
  default:
    {
      sprintf(styledirname,"/styles/exitstyles/generic");
      style_map_up = find_style(styledirname,exitstyle,-1);
      style_map_down = style_map_up;
      break;
    }
  }
  if(style_map_up == 0) the_exit_up =arch_to_object(find_archetype("exit"));
  else {
    object *tmp;
    tmp = pick_random_object(style_map_up);
    the_exit_up = arch_to_object(tmp->arch);
  }

  /* we need a down exit only if we're recursing. */
  if(RP->dungeon_level < RP->dungeon_depth || RP->final_map[0]!=0)
    if(style_map_down == 0) the_exit_down = arch_to_object(find_archetype("exit"));
    else {
      object *tmp;
      tmp = pick_random_object(style_map_down);
      the_exit_down = arch_to_object(tmp->arch);
    }
  else the_exit_down = 0;

  /* set up the down exit */
  the_exit_up->stats.hp = RP->origin_x;
  the_exit_up->stats.sp = RP->origin_y;
  the_exit_up->slaying = add_string(RP->origin_map);

  /* figure out where to put the entrance */
  /* begin a logical block */
  {
    int ex=-1,ey=-1;
    int i,j;
    /* first, look for a C, the map center.  */
    for(i=0;i<RP->Xsize&&cx==-1;i++)
      for(j=0;j<RP->Ysize;j++) {
		  if(maze[i][j]=='C') {
			 cx = i; cy=j; break;
		  }
      }
    if(cx!=-1) {
      if(cx > RP->Xsize/2) ex = 1;
      else ex = RP->Xsize -2;
      if(cy > RP->Ysize/2) ey = 1;
      else ey = RP->Ysize -2;
    }
    else {
		ex = RANDOM() % (RP->Xsize-2) +1;
		ey = RANDOM() % (RP->Ysize-2) +1;
    }
	 i = find_first_free_spot(the_exit_up->arch,map,ex,ey);
	 the_exit_up->x = ex + freearr_x[i];
	 the_exit_up->y = ey + freearr_y[i];

	 /* surround the exits with notices that this is a random map. */
	 for(j=1;j<9;j++) {
		if(!wall_blocked(map,the_exit_up->x+freearr_x[j],the_exit_up->y+freearr_y[j])) {
		  random_sign = get_archetype("sign");
		  random_sign->x = the_exit_up->x+freearr_x[j];
		  random_sign->y = the_exit_up->y+freearr_y[j];

		  random_sign->msg = add_string("This is a random map.\n");
		  insert_ob_in_map(random_sign,map,NULL);
		}
	 }
	 insert_ob_in_map(the_exit_up,map,NULL);
	 maze[the_exit_up->x][the_exit_up->y]='<';
	 /* set the starting x,y for this map */
	 map->map_object->stats.hp=the_exit_up->x;
	 map->map_object->stats.sp=the_exit_up->y;
   
	 /* make the other exit far away from this one if
		 there's no center. */
	 if(cx==-1) {
		if(ex > RP->Xsize/2) cx = 1;
		else cx = RP->Xsize -2;
		if(ey > RP->Ysize/2) cy = 1;
		else cy = RP->Ysize -2;
	 };

	 if(the_exit_down) {
		char buf[2048];
		i = find_first_free_spot(the_exit_down->arch,map,cx,cy);
		the_exit_down->x = cx + freearr_x[i];
		the_exit_down->y = cy + freearr_y[i];
		RP->origin_x = the_exit_down->x;
		RP->origin_y = the_exit_down->y;
		write_map_parameters_to_string(buf,RP);
		the_exit_down->msg = add_string(buf);
		/* the identifier for making a random map. */
		if(RP->dungeon_level >= RP->dungeon_depth && RP->final_map[0]!=0) {
		  mapstruct *new_map;
		  char new_map_name[MAX_BUF];
		  object *the_exit_back = arch_to_object(the_exit_up->arch);
		  /* give the final map a name */
		  sprintf(new_map_name,"%sfinal_map",RP->final_map);
		  /* set the exit down. */
		  the_exit_down->slaying = add_string(new_map_name);
		  /* load it */
		  if( (new_map=has_been_loaded(RP->final_map)) == NULL)
		    new_map = load_original_map(RP->final_map,0);
		  /* leave if we couldn't find it. */
		  if(new_map==NULL) return;
		  /* fix the treasures in the map. */
		  fix_auto_apply(new_map);
		  strcpy(new_map->path,new_map_name);
		  /* setup the exit back */
		  the_exit_back->slaying = add_string(map->path);
		  the_exit_back->stats.hp = the_exit_down->x;
		  the_exit_back->stats.sp = the_exit_down->y;
		  the_exit_back->x = EXIT_X(new_map->map_object);
		  the_exit_back->y = EXIT_Y(new_map->map_object);
		  insert_ob_in_map(the_exit_back,new_map,NULL);
		}
		else
		  the_exit_down->slaying = add_string("/!");
		insert_ob_in_map(the_exit_down,map,NULL);
		maze[the_exit_down->x][the_exit_down->y]='>';
	 }
  }     
    
}

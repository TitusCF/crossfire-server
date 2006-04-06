/*
 * static char *rcsid_exit_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
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
#include <random_map.h>
#include <sproto.h>
#include <rproto.h>


/*  find a character in the layout.  fx and fy are pointers to
    where to find the char.  fx,fy = -1 if not found. */
void find_in_layout(int mode, char target,int *fx,int *fy,char **layout,RMParms *RP) {
  int M;
  int i,j;
  *fx=-1;
  *fy=-1;

  /* if a starting point isn't given, pick one */
  if(mode < 1 || mode > 4) M=RANDOM() % 4 + 1 ;
  else M = mode;

  /* four different search starting points and methods so that
     we can do something different for symmetrical maps instead of
     the same damned thing every time. */
  switch(M) {
  case 1: {  /* search from top left down/right */
    for(i=1;i<RP->Xsize;i++)
      for(j=1;j<RP->Ysize;j++) {
        if(layout[i][j]==target) {
          *fx = i; *fy = j;
          return;
        }
      }
    break;
  }
  case 2: { /* Search from top right down/left */
    for(i=RP->Xsize-2;i>0;i--)
      for(j=1;j<RP->Ysize-1;j++) {
        if(layout[i][j]==target) {
          *fx = i; *fy = j;
          return;
        }
      }
    break;
  }
  case 3: { /* search from bottom-left up-right */
    for(i=1;i<RP->Xsize-1;i++)
      for(j=RP->Ysize-2;j>0;j--) {
        if(layout[i][j]==target) {
          *fx = i; *fy = j;
          return;
        }
      }
    break;
  }
  case 4: { /* search from bottom-right up-left */
    for(i=RP->Xsize-2;i>0;i--)
      for(j=RP->Ysize-2;j>0;j--) {
        if(layout[i][j]==target) {
          *fx = i; *fy = j;
          return;
        }
      }
    break;
  }
  }
}
        
    
    

  
  
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
  char buf[512];
  int cx=-1,cy=-1;  /* location of a map center */
  int upx=-1,upy=-1;  /* location of up exit */
  int downx=-1,downy=-1;
  int final_map_exit=1;
 
  if(RP->exit_on_final_map){
	  if(strstr(RP->exit_on_final_map,"no")) final_map_exit=0;
    }

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

  /* set up the up exit */
  the_exit_up->stats.hp = RP->origin_x;
  the_exit_up->stats.sp = RP->origin_y;
  the_exit_up->slaying = add_string(RP->origin_map);

  /* figure out where to put the entrance */
  /* begin a logical block */
  {
    int i,j;
    /* First, look for a '<' char */
    find_in_layout(0,'<',&upx,&upy,maze,RP);

    /* next, look for a C, the map center.  */
    find_in_layout(0,'C',&cx,&cy,maze,RP);

    
    /* if we didn't find an up, find an empty place far from the center */
    if(upx==-1 && cx!=-1) {
      if(cx > RP->Xsize/2) upx = 1;
      else upx = RP->Xsize -2;
      if(cy > RP->Ysize/2) upy = 1;
      else upy = RP->Ysize -2;
      /* find an empty place far from the center */
      if(upx==1 && upy==1) find_in_layout(1,0,&upx,&upy,maze,RP);
      else
        if(upx==1 && upy>1) find_in_layout(3,0,&upx,&upy,maze,RP);
        else
          if(upx>1 && upy==1) find_in_layout(2,0,&upx,&upy,maze,RP);
          else
            if(upx>1 && upy>1) find_in_layout(4,0,&upx,&upy,maze,RP);
    }
    
    /* no indication of where to place the exit, so just place it. */
    if(upx==-1) find_in_layout(0,0,&upx,&upy,maze,RP);

    the_exit_up->x = upx;
    the_exit_up->y = upy;

    /* surround the exits with notices that this is a random map. */
    for(j=1;j<9;j++) {
      if(!wall_blocked(map,the_exit_up->x+freearr_x[j],the_exit_up->y+freearr_y[j])) {
        random_sign = create_archetype("sign");
        random_sign->x = the_exit_up->x+freearr_x[j];
        random_sign->y = the_exit_up->y+freearr_y[j];
		  
		sprintf(buf,"This is a random map.\nLevel: %d\n", (RP->dungeon_level)-1);
        
		random_sign->msg = add_string(buf);
        insert_ob_in_map(random_sign,map,NULL,0);
      }
    }
    /* Block the exit so things don't get dumped on top of it. */
    the_exit_up->move_block = MOVE_ALL;

    insert_ob_in_map(the_exit_up,map,NULL,0);
    maze[the_exit_up->x][the_exit_up->y]='<';

    /* set the starting x,y for this map */
    MAP_ENTER_X(map) = the_exit_up->x;
    MAP_ENTER_Y(map) = the_exit_up->y;

    /* first, look for a '>' character */
    find_in_layout(0,'>',&downx,&downy,maze,RP);
    /* if no > is found use C */
    if(downx==-1) { downx = cx; downy=cy;};
        
    /* make the other exit far away from this one if
       there's no center. */
    if(downx==-1) {
      if(upx > RP->Xsize/2) downx = 1;
      else downx = RP->Xsize -2;
      if(upy > RP->Ysize/2) downy = 1;
      else downy = RP->Ysize -2;
      /* find an empty place far from the entrance */
      if(downx==1 && downy==1) find_in_layout(1,0,&downx,&downy,maze,RP);
      else
        if(downx==1 && downy>1) find_in_layout(3,0,&downx,&downy,maze,RP);
        else
          if(downx>1 && downy==1) find_in_layout(2,0,&downx,&downy,maze,RP);
          else
            if(downx>1 && downy>1) find_in_layout(4,0,&downx,&downy,maze,RP);
      
    }
    /* no indication of where to place the down exit, so just place it */
    if(downx==-1) find_in_layout(0,0,&downx,&downy,maze,RP);
    if(the_exit_down) {
      char buf[2048];
      i = find_first_free_spot(the_exit_down, map, downx, downy);
      the_exit_down->x = downx + freearr_x[i];
      the_exit_down->y = downy + freearr_y[i];
      RP->origin_x = the_exit_down->x;
      RP->origin_y = the_exit_down->y;
      write_map_parameters_to_string(buf,RP);
      the_exit_down->msg = add_string(buf);
      /* the identifier for making a random map. */
      if(RP->dungeon_level >= RP->dungeon_depth && RP->final_map[0]!=0) {
        mapstruct *new_map;
        object *the_exit_back = arch_to_object(the_exit_up->arch), *tmp;
#if 0
	/* I'm not sure if there was any reason to change the path to the
	 * map other than to maybe make it more descriptive in the 'maps'
	 * command.  But changing the map name makes life more complicated,
	 * (has_been_loaded needs to use the new name)
	 */

        char new_map_name[MAX_BUF];
        /* give the final map a name */
        sprintf(new_map_name,"%sfinal_map",RP->final_map);
        /* set the exit down. */
#endif
        /* load it */
        if((new_map=ready_map_name(RP->final_map,0)) == NULL) 
	    return;

        the_exit_down->slaying = add_string(RP->final_map);
        strcpy(new_map->path,RP->final_map);

		for (tmp=GET_MAP_OB(new_map,  MAP_ENTER_X(new_map), MAP_ENTER_Y(new_map)); tmp; tmp=tmp->above)
	    /* Remove exit back to previous random map.  There should only be one
	     * which is why we break out.  To try to process more than one
	     * would require keeping a 'next' pointer, ad free_object kills tmp, which
	     * breaks the for loop.
	     */
	    if (tmp->type == EXIT && EXIT_PATH(tmp) && !strncmp(EXIT_PATH(tmp),"/random/", 8)) {
		remove_ob(tmp);
		free_object(tmp);
		break;
	    }

		if (final_map_exit == 1)
			{
        /* setup the exit back */
        the_exit_back->slaying = add_string(map->path);
        the_exit_back->stats.hp = the_exit_down->x;
        the_exit_back->stats.sp = the_exit_down->y;
        the_exit_back->x = MAP_ENTER_X(new_map);
        the_exit_back->y = MAP_ENTER_Y(new_map);

        insert_ob_in_map(the_exit_back,new_map,NULL,0);
			}
			
		set_map_timeout(new_map);   /* So it gets swapped out */
      }
      else 
        the_exit_down->slaying = add_string("/!");
      /* Block the exit so things don't get dumped on top of it. */
      the_exit_down->move_block = MOVE_ALL;
      insert_ob_in_map(the_exit_down,map,NULL,0);
      maze[the_exit_down->x][the_exit_down->y]='>';
    }
  }
    
}



/* this function unblocks the exits.  We blocked them to
   keep things from being dumped on them during the other
   phases of random map generation. */
void unblock_exits(mapstruct *map, char **maze, RMParms *RP) {
  int i=0,j=0;
  object *walk;

  for(i=0;i<RP->Xsize;i++)
    for(j=0;j<RP->Ysize;j++)
      if(maze[i][j]=='>' || maze[i][j]=='<') {
        for(walk=get_map_ob(map,i,j);walk!=NULL;walk=walk->above) {
          if(walk->move_block == MOVE_ALL && walk->type != LOCKED_DOOR) {
	    walk->move_block = MOVE_BLOCK_DEFAULT;
            update_object(walk,UP_OBJ_CHANGE);
          }
        }
      }
}

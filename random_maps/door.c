/*
 * static char *rcsid_door_c =
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
#include <rproto.h>

void put_doors(mapstruct *the_map,char **maze , char *doorstyle, RMParms *RP) {
  int i,j;
  mapstruct *vdoors;
  mapstruct *hdoors;
  char doorpath[128];

  if(!strcmp(doorstyle,"none")) return;
  vdoors = find_style("/styles/doorstyles/vdoors",doorstyle,-1);
  if(!vdoors) return;
  sprintf(doorpath,"/styles/doorstyles/hdoors%s",strrchr(vdoors->path,'/'));
  hdoors = find_style(doorpath,0,-1);
  for(i=0;i<RP->Xsize;i++) 
    for(j=0;j<RP->Ysize;j++) {
      if(maze[i][j]=='D') {
		  int sindex;
		  object *this_door,*new_door;
		  sindex = surround_flag(maze,i,j,RP);
		  if(sindex==3) 
			 this_door=pick_random_object(hdoors);
		  else
			 this_door=pick_random_object(vdoors);
		  new_door = arch_to_object(this_door->arch);
		  copy_object(this_door,new_door);
		  new_door->x = i;
		  new_door->y = j;
		  insert_ob_in_map(new_door,the_map,NULL);
      }
    }
}

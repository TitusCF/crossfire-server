/*
 * static char *rcsid_los_c =
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

    The author can be reached via e-mail to frankj@ifi.uio.no.

    This file was made based on an idea by vidarl@ifi.uio.no
*/

/* Nov 95 - inserted USE_LIGHTING code stuff in here - b.t. */

#include <global.h>
#include <funcpoint.h>


typedef struct blstr {
  int x[4],y[4],b[4];
  int index;
} blocks;

blocks block[11][11];

/*
 * Used to initialise the array used by the LOS routines.
 */

void set_block(int x,int y,int bx, int by) {
  int index=block[x][y].index;
  block[x][y].x[index]=bx;
  block[x][y].y[index]=by;
  block[x][y].index++;
}

/*
 * initialises the array used by the LOS routines.
 */

void init_block() {
  int x,y;
  for(x=0;x<11;x++)
    for(y=0;y<11;y++) {
    block[x][y].index=0;
    }

/* region 1 */

  set_block(5,4,4,3);
  set_block(5,4,5,3);
  set_block(5,4,6,3);
  set_block(4,3,4,2);
  set_block(5,3,5,2);
  set_block(6,3,6,2);
  set_block(4,2,4,1);
  set_block(4,2,3,1);
  set_block(5,2,5,1);
  set_block(6,2,6,1);
  set_block(6,2,7,1);
  set_block(3,1,3,0);
  set_block(4,1,4,0);
  set_block(5,1,5,0);
  set_block(6,1,6,0);
  set_block(7,1,7,0);

/* region 2  */

  set_block(6,4,7,3);
  set_block(7,3,7,2);
  set_block(7,3,8,2);
  set_block(7,3,8,3);
  set_block(7,2,8,1);
  set_block(8,2,9,1);
  set_block(8,3,9,2);
  set_block(8,1,8,0);
  set_block(8,1,9,0);
  set_block(9,1,10,0);
  set_block(9,2,10,1);
  set_block(9,2,10,2);

/* region 3  */

  set_block(6,5,7,4);
  set_block(6,5,7,5);
  set_block(6,5,7,6);
  set_block(7,4,8,4);
  set_block(7,5,8,5);
  set_block(7,6,8,6);
  set_block(8,4,9,3);
  set_block(8,4,9,4);
  set_block(8,5,9,5);
  set_block(8,6,9,6);
  set_block(8,6,9,7);
  set_block(9,3,10,3);
  set_block(9,4,10,4);
  set_block(9,5,10,5);
  set_block(9,6,10,6);
  set_block(9,7,10,7);

/* region 4  */

  set_block(6,6,7,7);
  set_block(7,7,8,7);
  set_block(7,7,8,8);
  set_block(7,7,7,8);
  set_block(8,7,9,8);
  set_block(8,8,9,9);
  set_block(7,8,8,9);
  set_block(9,8,10,8);
  set_block(9,8,10,9);
  set_block(9,9,10,10);
  set_block(8,9,9,10);
  set_block(8,9,8,10);

/* region 5  */

  set_block(5,6,4,7);
  set_block(5,6,5,7);
  set_block(5,6,6,7);
  set_block(4,7,4,8);
  set_block(5,7,5,8);
  set_block(6,7,6,8);
  set_block(4,8,3,9);
  set_block(4,8,4,9);
  set_block(5,8,5,9);
  set_block(6,8,6,9);
  set_block(6,8,7,9);
  set_block(3,9,3,10);
  set_block(4,9,4,10);
  set_block(5,9,5,10);
  set_block(6,9,6,10);
  set_block(7,9,7,10);

/* region 6 */

  set_block(4,6,3,7);
  set_block(3,7,2,7);
  set_block(3,7,2,8);
  set_block(3,7,3,8);
  set_block(2,7,1,8);
  set_block(2,8,1,9);
  set_block(3,8,2,9);
  set_block(1,8,0,8);
  set_block(1,8,0,9);
  set_block(1,9,0,10);
  set_block(2,9,1,10);
  set_block(2,9,2,10);

/* region 7 */

  set_block(4,5,3,4);
  set_block(4,5,3,5);
  set_block(4,5,3,6);
  set_block(3,4,2,4);
  set_block(3,5,2,5);
  set_block(3,6,2,6);
  set_block(2,4,1,3);
  set_block(2,4,1,4);
  set_block(2,5,1,5);
  set_block(2,6,1,6);
  set_block(2,6,1,7);
  set_block(1,3,0,3);
  set_block(1,4,0,4);
  set_block(1,5,0,5);
  set_block(1,6,0,6);
  set_block(1,7,0,7);

/* region 8 */

  set_block(4,4,3,3);
  set_block(3,3,3,2);
  set_block(3,3,2,2);
  set_block(3,3,2,3);
  set_block(3,2,2,1);
  set_block(2,2,1,1);
  set_block(2,3,1,2);
  set_block(2,1,1,0);
  set_block(2,1,2,0);
  set_block(1,1,0,0);
  set_block(1,2,0,1);
  set_block(1,2,0,2);
}

/*
 * Used to initialise the array used by the LOS routines.
 */

void set_wall(object *op,int x,int y) {
  int i;
  for(i=0;i<block[x][y].index;i++) {
    int dx=block[x][y].x[i],dy=block[x][y].y[i];
    op->contr->blocked_los[dx][dy]=100;
    set_wall(op,dx,dy);
  }
}

/*
 * Used to initialise the array used by the LOS routines.
 */

void check_wall(object *op,int x,int y) {
  if(!block[x][y].index)
    return;
  if(blocks_view(op->map,op->x-5+x,op->y-5+y))
    set_wall(op,x,y);
  else {
    int i;
    for(i=0;i<block[x][y].index;i++)
      check_wall(op,block[x][y].x[i],block[x][y].y[i]);
  }
}

/*
 * Clears/initialises the los-array associated to the player
 * controlling the object.
 */

void clear_los(object *op) {
  (void)memset((void *) op->contr->blocked_los,0,
               (WINRIGHT-WINLEFT+1)*(WINLOWER-WINUPPER+1));
}

/* change_map_light() - used to change map light level (darkness)
 * up or down by *1*. This fctn is not designed to change by
 * more than that!  Returns true if successful. -b.t. */
 
int change_map_light(mapstruct *m, int change) {
  int new_level = m->darkness + change;
 
  if(new_level<=0) {
        m->darkness = 0;
        return 0;
  }
 
  if(new_level>MAX_DARKNESS) return 0;
 
  if(change) {
     char buf[MAX_BUF];
 
     if(change>0) sprintf(buf,"It becomes darker.");
     else sprintf(buf,"It becomes brighter.");

     /* inform all players on the map */
     (info_map_func)(NDI_BLACK, m,buf);

     m->darkness=m->map_object->invisible=new_level;
     m->do_los=0; /* to insure that los is updated */
     update_all_los(m);
     return 1;
  }

  return 0;
}

/*
 * expand_sight goes through the array of what the given player is
 * able to see, and expands the visible area a bit, so the player will,
 * to a certain degree, be able to see into corners.
 * This is somewhat suboptimal, would be better to improve the formula.
 * There are two versions of this function, based on whether or not
 * CD_LINE_OF_SIGHT is defined.  If it is defined, the function
 * becomes a bit more time-consuming, but covers a few more "corners"...
 */


void expand_sight(object *op) 
{
  int i,x,y, dx, dy;

  for(x=1;x<10;x++)	/* loop over inner squares */
    for(y=1;y<10;y++)
      if(!op->contr->blocked_los[x][y]&&
         !blocks_view(op->map,op->x-5+x,op->y-5+y))
        for(i=1;i<=8;i+=1) {	/* mark all directions */
          dx = x + freearr_x[i];
			 dy = y + freearr_y[i];
          if(op->contr->blocked_los[dx][dy] > 0) /* for any square blocked */
            op->contr->blocked_los[dx][dy]= -1;
		  }

#ifdef USE_LIGHTING
  if(op->map->do_los) update_map_lights(op->map);
  if(op->map->darkness>0)  /* player is on a dark map */
      expand_lighted_sight(op);
#endif

  /* clear mark squares */
  for (x = 0; x < 11; x++)
    for (y = 0; y < 11; y++)
      if (op->contr->blocked_los[x][y] < 0)
            op->contr->blocked_los[x][y] = 0;
}

void add_light_to_list (object *light, object *op) {
  objectlink *obl=get_objectlink();

  if(!light||light->glow_radius<1||QUERY_FLAG(light, FLAG_FREED)) return;

  obl = (objectlink *) malloc(sizeof(objectlink));
  obl->ob = light;
  obl->id = light->count;
  obl->next = op->lights;
  op->lights = obl;
#ifdef DEBUG_LIGHTS
  LOG(llevDebug,"Adding light %s (%d) to op %s light list\n"
	,light->name,light->count,op->name);
#endif

}


void remove_light_from_list(object *light, object *op) {
  objectlink *obl=op->lights,*first,*prev=NULL,*nxt;

  if(!light||QUERY_FLAG(light, FLAG_FREED)) return;

  prev=first=obl;
  while(obl) {
     nxt = obl->next ? obl->next: NULL;
     if(obl->id==light->count) {
          if(obl==first) {
              op->lights = nxt;
              nxt = (nxt&&nxt->next) ? nxt->next: NULL;
          }
          if(prev) prev->next = nxt;
#ifdef DEBUG_LIGHTS
          LOG(llevDebug,"Removing light from op list: %s (%d)\n",
		light->name,light->count);
#endif
          CFREE(obl);
	  obl = NULL;
          return;
     } else
          prev = obl;
     obl=nxt;
  }

  /* light not found! */
  LOG(llevError,"Couldnt remove requested light %s (%d) from op list\n",
        light->name, light->count);
  if(first) op->lights = first;
}

/* returns true if op carries one or more lights */

int has_carried_lights(object *op) {
  objectlink *obl;
  object *tmp=NULL;

  /* op may glow! */
  if(op->glow_radius>0) return 1;

  /* carried items */
  for(obl=op->lights;obl;obl=obl->next)
     if((tmp=obl->ob)&&!QUERY_FLAG(tmp,FLAG_FREED)&&obl->id>0) 
	   return 1;

  return 0;
}
 
/* called when a player/pet monster changes maps, and has lights */

void add_carried_lights(object *pl) {
  objectlink *obl;
  object *tmp=NULL;
 
  /* pl may glow! */
  if(pl->glow_radius>0) add_light_to_map(pl,pl->map);

  /* carried items */
  for(obl=pl->lights;obl;obl=obl->next) {    
        if((tmp=obl->ob)&&!QUERY_FLAG(tmp,FLAG_FREED)&&obl->id>0) { 
#ifdef DEBUG_LIGHTS
           LOG(llevDebug,"got carried light: %s (%d)\n",tmp->name,tmp->count); 
#endif
           add_light_to_map(tmp,pl->map);
	}
  } 
} 

void remove_carried_lights(object *pl, mapstruct *map) {
  objectlink *obl=get_objectlink(),*maplight;

  for(obl=pl->lights;obl;obl=obl->next)
     for(maplight=map->light;maplight;maplight=maplight->next) { 
	if(maplight->id==obl->id) maplight->id = -1;
     }
  if(pl->lights) update_map_lights(map);
}


void add_light_to_map(object *ob, mapstruct *map) {
  objectlink *obl;
 
  if(ob->arch == NULL) { 
    LOG(llevError,"Can't add light %s (%d) without arch\n",ob->name,ob->count);
    return;
  } 
#ifdef DEBUG_LIGHTS
  LOG(llevDebug,"Adding light source: %s (%d) to maplist %s\n"
        ,ob->name,ob->count,map->path);
#endif   
  obl=get_objectlink();   
  obl->ob = ob;  
  obl->id = ob->count;   
  obl->next = map->light;
  map->light=obl;

  update_all_los(map);
}

/* light_not_listed()- check to see if the light is already on the map list 
 * returns true if the light is not listed */ 

int light_not_listed(object *op) {
   objectlink *obl=NULL;

   if(!op->map)
	if(op->env&&op->env->map) 
	    obl=op->env->map->light;
	else { 
	    LOG(llevError,"Error: can't find map light list in light_not_listed()\n");
	    return 0; 
	}
   else 
     obl=op->map->light;
   while(obl) {
     if(obl->id==op->count) return 0;
     obl=obl->next;
   } 
   return 1;
}

/* update_map_lights() - traverse linked list of lights, and 
 * remove those objects which were freed. -b.t.
 */

void update_map_lights(mapstruct *m) {
  objectlink *obl=m->light,*prev,*first,*nxt=NULL;
  object *tmp=NULL;

  prev=first=obl;
  while(obl) {
     nxt = obl->next ? obl->next: NULL;
     if(!(tmp=obl->ob)||(obl->id!=tmp->count)||QUERY_FLAG(tmp,FLAG_FREED)
	||tmp->glow_radius<1) {
	  if(obl==first) { 
	      first = prev = nxt;
     	      nxt = (nxt&&nxt->next) ? nxt->next: NULL;
	  }
	  if(prev) prev->next = nxt;
#ifdef DEBUG_LIGHTS
  	  if(tmp) LOG(llevDebug,"Removing light from map list: %s (%d)\n"
		,tmp->name,tmp->count);
  	  else LOG(llevDebug,"Removing null light (%d) from map list.\n",
		obl->id);
#endif
          CFREE(obl);
	  obl = NULL;
     } else
	  prev = obl;
     obl=nxt;
  }
  if(first)
     m->light = first;
  else 	
     m->light = NULL;

  m->do_los = 0;
}

void expand_lighted_sight(object *op)
{
  int x,y,dx,dy,radius=0,darklevel;
  object *tmp=NULL;
  mapstruct *m=op->map;
  objectlink *light=m->light;
 
  darklevel = MAP_DARKNESS(m);


/* If the player can see in the dark, lower the darklevel for him */ 
  if(QUERY_FLAG(op,FLAG_SEE_IN_DARK)) darklevel -= 2;

/* add light, by finding all (non-null) nearby light sources, then 
 * mark those squares specially. If the darklevel<1, there is no
 * reason to do this, so we skip this function */

  if(darklevel<1) return;
  /* First, limit player furthest (unlighted) vision */
  for (x = 0; x < 11; x++)
    for (y = 0; y < 11; y++)
		if(!(op->contr->blocked_los[x][y]==100))
		  op->contr->blocked_los[x][y]= 4;

  for(light=m->light;light!=NULL;light=light->next) { 
	 if(!(tmp=light->ob)||tmp->count!=light->id
		 ||tmp->glow_radius<1||QUERY_FLAG(tmp,FLAG_FREED)) continue;

	 /* if the light is not carried by a live creature *or* if its 
	  * on the map but its not the top object we ignore it (unless 
	  * its invisible). This helps to speed up the game. */

	 if(!tmp->env&&tmp->above&&!tmp->invisible) continue;

	 /* which coordinates to use for the light */ 
	 if(!tmp->env) /* use map coord */
		dx=abs(tmp->x-op->x),dy=abs(tmp->y-op->y);
	 else /* light is held, use env coord */
		dx=abs(tmp->env->x-op->x),dy=abs(tmp->env->y-op->y);

	 radius = BRIGHTNESS(tmp);

	/* ok, lets do it if in viewable range */
	 if(dx<=(4+radius)&&dy<=(4+radius)) {
		int basex,basey;
		if(!tmp->env) { /* get right coord for the light */
		  basex=tmp->x-op->x+5,basey=tmp->y-op->y+5;
		} else { 
		  basex=tmp->env->x-op->x+5,basey=tmp->env->y-op->y+5;
		}
		radius-=1;  /* so we get right size illumination */ 
		
		for(x=-radius;x<=radius;x++) 
		  for(y=-radius;y<=radius;y++) {
			 /* round corners */
			 if(radius>1&&(abs(x)==radius)&&(abs(y)==radius)) continue; 
			 dx=basex+x,dy=basey+y;
			 if(dx>=0&&dx<11&&dy>=0&&dy<11
				 &&!(op->contr->blocked_los[dx][dy]==100))
				op->contr->blocked_los[dx][dy]-= radius - MAX(abs(x),abs(y))+1;
		  }
	 }
  }
  /*  grant some vision to the player, based on the darklevel */
  for(x=darklevel-5; x<6-darklevel; x++)
	 for(y=darklevel-5; y<6-darklevel; y++)
		if(!(op->contr->blocked_los[x+5][y+5]==100))
		  op->contr->blocked_los[x+5][y+5]-= MAX(0,6 -darklevel - MAX(abs(x),abs(y))); 
}

/* blinded_sight() - sets all veiwable squares to blocked except 
 * for the one the central one that the player occupies 
 */

void blinded_sight (object *op) {
  int x,y;

  for (x = 0; x < 11; x++)
    for (y = 0; y < 11; y++)
	op->contr->blocked_los[x][y] = 100;

  op->contr->blocked_los[5][5] = 0;
}

/*
 * update_los() recalculates the array which specifies what is
 * visible for the given player-object.
 */

void update_los(object *op) {
  int i;
  if(QUERY_FLAG(op,FLAG_REMOVED))
    return;
  clear_los(op);
  if(QUERY_FLAG(op,FLAG_WIZ) /* ||XRAYS(op) */)
    return;
  for(i=1;i<9;i++)
    check_wall(op,5+freearr_x[i],5+freearr_y[i]);

  /* do the los of the player. 3 (potential) cases */
  if(QUERY_FLAG(op,FLAG_BLIND)) /* player is blind */ 
    blinded_sight(op);
  else				
    expand_sight(op);

  if (QUERY_FLAG(op,FLAG_XRAYS)) {
    int x, y;
    for (x = -2; x <= 2; x++)
      for (y = -2; y <= 2; y++)
        op->contr->blocked_los[5 + x][5 + y] = 0;
  }
}

/*
 * This function makes sure that update_los() will be called for all
 * players on the given map within the next frame.
 * It is triggered by removal or inserting of objects which blocks
 * the sight in the map.
 */

void update_all_los(mapstruct *map) {
  player *pl;

#ifdef USE_LIGHTING
  if(map->do_los) return; /* we already did this */
  map->do_los = 1;
#endif
  for(pl=first_player;pl!=NULL;pl=pl->next)
    if(pl->ob->map==map)
      pl->do_los=1;
}

/*
 * Debug-routine which dumps the array which specifies the visible
 * area of a player.  Triggered by the z key in DM mode.
 */

void print_los(object *op) {
  int x,y;
  char buf[50], buf2[10];

  strcpy(buf,"   ");
  for(x=0;x<11;x++)
  {
    sprintf(buf2,"%2d",x);
    strcat(buf,buf2);
  }
  (*draw_info_func)(NDI_UNIQUE, 0, op, buf);
  for(y=0;y<11;y++) {
    sprintf(buf,"%2d:",y);
    for(x=0;x<11;x++)
    {
      sprintf(buf2," %1d",op->contr->blocked_los[x][y]);
      strcat(buf,buf2);
    }
    (*draw_info_func)(NDI_UNIQUE, 0, op, buf);
  }
}

/*
 * make_sure_seen: The object is supposed to be visible through walls, thus
 * check if any players are nearby, and edit their LOS array.
 */

void make_sure_seen(object *op) {
  player *pl;

  for (pl = first_player; pl; pl = pl->next)
    if (pl->ob->map == op->map &&
        pl->ob->y + WINUPPER <= op->y && pl->ob->y + WINLOWER >= op->y &&
        pl->ob->x + WINLEFT  <= op->x && pl->ob->x + WINRIGHT >= op->x)
      pl->blocked_los[5 + op->x - pl->ob->x][5 + op->y - pl->ob->y] = 0;
}

/*
 * make_sure_not_seen: The object which is supposed to be visible through
 * walls has just been removed from the map, so update the los of any
 * players within its range
 */

void make_sure_not_seen(object *op) { 
  player *pl;
  for (pl = first_player; pl; pl = pl->next)
    if (pl->ob->map == op->map &&
        pl->ob->y + WINUPPER <= op->y && pl->ob->y + WINLOWER >= op->y &&
        pl->ob->x + WINLEFT  <= op->x && pl->ob->x + WINRIGHT >= op->x)
      pl->do_los = 1;
}

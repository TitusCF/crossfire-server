/*
 * static char *rcsid_los_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net

    This file was made based on an idea by vidarl@ifi.uio.no
*/

/* Nov 95 - inserted USE_LIGHTING code stuff in here - b.t. */

#include <global.h>
#include <funcpoint.h>
#include <math.h>


/* Distance must be less than this for the object to be blocked.
 * An object is 1.0 wide, so if set to 0.5, it means the object
 * that blocks half the view (0.0 is complete block) will
 * block view in our tables.
 * .4 or less lets you see through walls.  .5 is about right.
 */

#define SPACE_BLOCK	0.5

typedef struct blstr {
  int x[4],y[4];
  int index;
} blocks;

blocks block[MAP_CLIENT_X][MAP_CLIENT_Y];

/*
 * Used to initialise the array used by the LOS routines.
 * What this sets if that x,y blocks the view of bx,by
 * This then sets up a relation - for example, something
 * at 5,4 blocks view at 5,3 which blocks view at 5,2
 * etc.  So when we check 5,4 and find it block, we have
 * the data to know that 5,3 and 5,2 and 5,1 should also
 * be blocked.
 */

void set_block(int x,int y,int bx, int by) {
    int index=block[x][y].index,i;

    /* Due to flipping, we may get duplicates - better safe than sorry.
     */
    for (i=0; i<index; i++) {
	if (block[x][y].x[i] == bx && block[x][y].y[i] == by) return;
    }

    block[x][y].x[index]=bx;
    block[x][y].y[index]=by;
    block[x][y].index++;
#ifdef LOS_DEBUG
    fprintf(stderr,"setblock: added %d %d -> %d %d (%d)\n", x, y, bx, by,
	    block[x][y].index);
#endif
}

/*
 * initialises the array used by the LOS routines.
 */

/* since we are only doing the upper left quadrant, only
 * these spaces could possibly get blocked, since these
 * are the only ones further out that are still possibly in the
 * sightline.
 */

void init_block() {
    int x,y, dx, dy, i;
    static int block_x[3] = {-1, -1, 0}, block_y[3] = {-1, 0, -1};

    for(x=0;x<MAP_CLIENT_X;x++)
	for(y=0;y<MAP_CLIENT_Y;y++) {
	    block[x][y].index=0;
	}


    /* The table should be symmetric, so only do the upper left
     * quadrant - makes the processing easier.
     */
    for (x=1; x<=MAP_CLIENT_X/2; x++) {
	for (y=1; y<=MAP_CLIENT_Y/2; y++) {
	    for (i=0; i< 3; i++) {
		dx = x + block_x[i];
		dy = y + block_y[i];

		/* center space never blocks */
		if (x == MAP_CLIENT_X/2 && y == MAP_CLIENT_Y/2) continue;

		/* If its a straight line, its blocked */
		if ((dx == x && x == MAP_CLIENT_X/2) || 
		    (dy==y && y == MAP_CLIENT_Y/2)) {
			/* For simplicity, we mirror the coordinates to block the other
			 * quadrants.
			 */
			set_block(x, y, dx, dy);
			if (x == MAP_CLIENT_X/2) {
			    set_block(x, MAP_CLIENT_Y - y -1, dx, MAP_CLIENT_Y - dy-1);
			} else if (y == MAP_CLIENT_Y/2) {
			    set_block(MAP_CLIENT_X - x -1, y, MAP_CLIENT_X - dx - 1, dy);
			}
		} else {
		    float d1, r, s,l;

		    /* We use the algorihm that found out how close the point
		     * (x,y) is to the line from dx,dy to the center of the viewable
		     * area.  l is the distance from x,y to the line.
		     * r is more a curiosity - it lets us know what direction (left/right)
		     * the line is off
		     */

		    d1 = (float) (pow(MAP_CLIENT_X/2 - dx, 2) + pow(MAP_CLIENT_Y/2 - dy,2));
		    r = (float)((dy-y)*(dy - MAP_CLIENT_Y/2) - (dx-x)*(MAP_CLIENT_X/2-dx))/d1;
		    s = (float)((dy-y)*(MAP_CLIENT_X/2 - dx ) - (dx-x)*(MAP_CLIENT_Y/2-dy))/d1;
		    l = FABS(sqrt(d1) * s);

		    if (l <= SPACE_BLOCK) {
			/* For simplicity, we mirror the coordinates to block the other
			 * quadrants.
			 */
			set_block(x,y,dx,dy);
			set_block(MAP_CLIENT_X - x -1, y, MAP_CLIENT_X - dx - 1, dy);
			set_block(x, MAP_CLIENT_Y - y -1, dx, MAP_CLIENT_Y - dy - 1);
			set_block(MAP_CLIENT_X -x-1, MAP_CLIENT_Y -y-1, MAP_CLIENT_X - dx-1, MAP_CLIENT_Y - dy-1);
		    }
		}
	    }
	}
    }
}

/*
 * Used to initialise the array used by the LOS routines.
 * x,y are indexes into the blocked[][] array.
 */

static void set_wall(object *op,int x,int y) {
    int i;

    for(i=0;i<block[x][y].index;i++) {
	int dx=block[x][y].x[i],dy=block[x][y].y[i],ax,ay;

	/* ax, ay are the values as adjusted to be in the
	 * socket look structure.
	 */
	ax = dx - (MAP_CLIENT_X - op->contr->socket.mapx)/2;
	ay = dy - (MAP_CLIENT_Y - op->contr->socket.mapy)/2;

	if (ax < 0 || ax>=op->contr->socket.mapx ||
	    ay < 0 || ay>=op->contr->socket.mapy) continue;
#ifdef LOS_DEBUG
	fprintf(stderr,"blocked %d %d -> %d %d\n",
#endif		dx, dy, ax, ay);

	/* we need to adjust to the fact that the socket
	 * code wants the los to start from the 0,0
	 * and not be relative to middle of los array.
	 */
	op->contr->blocked_los[ax][ay]=100;
	set_wall(op,dx,dy);
    }
}

/*
 * Used to initialise the array used by the LOS routines.
 * op is the object, x and y values based on MAP_CLIENT_X and Y.
 * this is because they index the blocked[][] arrays.
 */

static void check_wall(object *op,int x,int y) {
    int ax, ay;
    if(!block[x][y].index)
	return;

    /* ax, ay are coordinates as indexed into the look window */
    ax = x - (MAP_CLIENT_X - op->contr->socket.mapx)/2;
    ay = y - (MAP_CLIENT_Y - op->contr->socket.mapy)/2;

    /* If the converted coordinates are outside the viewable
     * area for the client, return now.
     */
    if (ax < 0 || ay < 0 || ax >= op->contr->socket.mapx || ay >= op->contr->socket.mapy)
	return;

    if(blocks_view(op->map,op->x-op->contr->socket.mapx/2 + ax,
		 op->y-op->contr->socket.mapy/2 + ay))
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
    /* This is safer than using the socket->mapx, mapy because
     * we index the blocked_los as a 2 way array, so clearing
     * the first z spaces may not not cover the spaces we are
     * actually going to use
     */
    (void)memset((void *) op->contr->blocked_los,0,
	       MAP_CLIENT_X * MAP_CLIENT_Y);
}

/* change_map_light() - used to change map light level (darkness)
 * up or down by *1*. This fctn is not designed to change by
 * more than that!  Returns true if successful. -b.t. 
 */
 
int change_map_light(mapstruct *m, int change) {
    int new_level = m->darkness + change;
 
    if(new_level<=0) {
	m->darkness = 0;
        return 0;
    }
 
    if(new_level>MAX_DARKNESS) return 0;
 
    if(change) {
	/* inform all players on the map */
	if (change>0) 
	    (info_map_func)(NDI_BLACK, m,"It becomes darker.");
	else
	    (info_map_func)(NDI_BLACK, m,"It becomes brighter.");

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
 */

void expand_sight(object *op) 
{
    int i,x,y, dx, dy;

    for(x=1;x<op->contr->socket.mapx-1;x++)	/* loop over inner squares */
	for(y=1;y<op->contr->socket.mapy-1;y++) {
	    if(!op->contr->blocked_los[x][y] &&
	       !blocks_view(op->map,op->x-op->contr->socket.mapx/2+x,
		      op->y-op->contr->socket.mapy/2+y))

	    for(i=1;i<=8;i+=1) {	/* mark all directions */
		dx = x + freearr_x[i];
		dy = y + freearr_y[i];
		if(op->contr->blocked_los[dx][dy] > 0) /* for any square blocked */
		    op->contr->blocked_los[dx][dy]= -1;
	    }
	}

#ifdef USE_LIGHTING
    if(op->map->do_los) update_map_lights(op->map);
    if(op->map->darkness>0)  /* player is on a dark map */
	expand_lighted_sight(op);
#endif

    /* clear mark squares */
    for (x = 0; x < op->contr->socket.mapx; x++)
	for (y = 0; y < op->contr->socket.mapy; y++)
	    if (op->contr->blocked_los[x][y] < 0)
		op->contr->blocked_los[x][y] = 0;
}

void add_light_to_list (object *light, object *op) {
    objectlink *obl;

    if(!light||light->glow_radius<1||QUERY_FLAG(light, FLAG_FREED)) return;

    obl = get_objectlink();
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
	nxt = obl->next;
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
     * reason to do this, so we skip this function 
     */

    if(darklevel<1) return;
    /* First, limit player furthest (unlighted) vision */
    for (x = 0; x < op->contr->socket.mapx; x++)
	for (y = 0; y < op->contr->socket.mapy; y++)
	    if(!(op->contr->blocked_los[x][y]==100))
		  op->contr->blocked_los[x][y]= 4;

    for(light=m->light;light!=NULL;light=light->next) { 
	if(!(tmp=light->ob)||tmp->count!=light->id
		 ||tmp->glow_radius<1||QUERY_FLAG(tmp,FLAG_FREED)) continue;

	/* if the light is not carried by a live creature *or* if its 
	 * on the map but its not the top object we ignore it (unless 
	 * its invisible). This helps to speed up the game. 
	 *
	 * Remove that check (MSW 2001-05-29).  Best I can see, this
	 * means we will process lights in piles of loot - I don't see
	 * a problem with that - I personally find it odd if something
	 * gets dropped onto a space with a torch that it suddenly gets
	 * darker - the ordering of objects in crossfire is somewhat suspect.
	 */

#if 0
	 if(!tmp->env&&tmp->above&&!tmp->invisible) continue;
#endif

	/* which coordinates to use for the light */ 
	if(!tmp->env) /* use map coord */
	    dx=abs(tmp->x-op->x),dy=abs(tmp->y-op->y);
	else /* light is held, use env coord */
	    dx=abs(tmp->env->x-op->x),dy=abs(tmp->env->y-op->y);

	radius = BRIGHTNESS(tmp);

	/* dx and dy are number of spaces it is away from player */
	if(dx<=(op->contr->socket.mapx/2+radius)&&dy<=(op->contr->socket.mapy/2+radius)) {
	    /* Its within range to do some good */
	    int basex,basey;

	    if(!tmp->env) { /* get right coord for the light */
		basex=tmp->x-op->x+op->contr->socket.mapx/2;
		basey=tmp->y-op->y+op->contr->socket.mapy/2;
	    } else { 
		basex=tmp->env->x-op->x+op->contr->socket.mapx/2;
		basey=tmp->env->y-op->y+op->contr->socket.mapy/2;
	    }
	    radius-=1;  /* so we get right size illumination */ 
		
	    for(x=-radius;x<=radius;x++) 
		for(y=-radius;y<=radius;y++) {
		    /* round corners */
		    if(radius>1&&(abs(x)==radius)&&(abs(y)==radius)) continue; 

		    dx=basex+x,dy=basey+y;
		    if(dx>=0 && dx<op->contr->socket.mapx &&
		       dy>=0 && dy<op->contr->socket.mapy &&
		       !(op->contr->blocked_los[dx][dy]==100))
			    op->contr->blocked_los[dx][dy]-= radius - MAX(abs(x),abs(y))+1;
		}
	}
    }
    /*  grant some vision to the player, based on the darklevel */
    for(x=darklevel-5; x<6-darklevel; x++)
	for(y=darklevel-5; y<6-darklevel; y++)
	    if(!(op->contr->blocked_los[x+op->contr->socket.mapx/2][y+op->contr->socket.mapy/2]==100))
		op->contr->blocked_los[x+op->contr->socket.mapx/2][y+op->contr->socket.mapy/2]-= 
		    MAX(0,6 -darklevel - MAX(abs(x),abs(y))); 
}

/* blinded_sight() - sets all veiwable squares to blocked except 
 * for the one the central one that the player occupies 
 */

void blinded_sight (object *op) {
    int x,y;

    for (x = 0; x < op->contr->socket.mapx; x++)
	for (y = 0; y <  op->contr->socket.mapy; y++)
	    op->contr->blocked_los[x][y] = 100;

    op->contr->blocked_los[ op->contr->socket.mapx/2][ op->contr->socket.mapy/2] = 0;
}

/*
 * update_los() recalculates the array which specifies what is
 * visible for the given player-object.
 */

void update_los(object *op) {
    int i, dx = op->contr->socket.mapx/2, dy = op->contr->socket.mapy/2;
  
    if(QUERY_FLAG(op,FLAG_REMOVED))
	return;

    clear_los(op);
    if(QUERY_FLAG(op,FLAG_WIZ) /* ||XRAYS(op) */)
	return;


    for(i=1;i<9;i++)
	/* The block[][] array presumes player always in center relative to
	 * full size, and not client size, as that is a global structure.
	 */
	check_wall(op,MAP_CLIENT_X/2+freearr_x[i],MAP_CLIENT_Y/2+freearr_y[i]);

    /* do the los of the player. 3 (potential) cases */
    if(QUERY_FLAG(op,FLAG_BLIND)) /* player is blind */ 
	blinded_sight(op);
    else				
	expand_sight(op);

    if (QUERY_FLAG(op,FLAG_XRAYS)) {
	int x, y;
	for (x = -2; x <= 2; x++)
	    for (y = -2; y <= 2; y++)
		op->contr->blocked_los[dx + x][dy + y] = 0;
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
    for(x=0;x<op->contr->socket.mapx;x++) {
	sprintf(buf2,"%2d",x);
	strcat(buf,buf2);
    }
    (*draw_info_func)(NDI_UNIQUE, 0, op, buf);
    for(y=0;y<op->contr->socket.mapy;y++) {
	sprintf(buf,"%2d:",y);
	for(x=0;x<op->contr->socket.mapx;x++) {
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
	    pl->ob->y - pl->socket.mapy/2 <= op->y && 
	    pl->ob->y + pl->socket.mapy/2 >= op->y &&
	    pl->ob->x - pl->socket.mapx/2 <= op->x && 
	    pl->ob->x + pl->socket.mapx/2 >= op->x)
		pl->blocked_los[pl->socket.mapx/2 + op->x - pl->ob->x]
		    [pl->socket.mapy/2 + op->y - pl->ob->y] = 0;
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
	    pl->ob->y - pl->socket.mapy/2 <= op->y && 
	    pl->ob->y + pl->socket.mapy/2 >= op->y &&
	    pl->ob->x - pl->socket.mapx/2 <= op->x && 
	    pl->ob->x + pl->socket.mapx/2 >= op->x)
		pl->do_los = 1;
}

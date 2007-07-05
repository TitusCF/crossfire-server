/*
 * static char *rcsid_special_c =
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

/**
 * @file
 * Those functions handle placement of fountains, submaps, and so on.
 * @todo
 * replace defines by enums for linking purposes.
 */

#include <global.h>
#include <random_map.h>
#include <rproto.h>

#define NUM_OF_SPECIAL_TYPES 4
#define NO_SPECIAL 0
#define SPECIAL_SUBMAP 1
#define SPECIAL_FOUNTAIN 2
#define SPECIAL_EXIT 3

#define GLORY_HOLE 1
#define ORC_ZONE 2
#define MINING_ZONE 3
#define NR_OF_HOLE_TYPES 3

/**
 * Erases all objects (except floor) in the given rectangle.
 * @param map
 * map to process.
 * @param xstart
 * @param ystart
 * top-left coordinates of zone.
 * @param xsize
 * @param ysize
 * size of zone.
 */
void nuke_map_region(mapstruct *map,int xstart,int ystart, int xsize, int ysize) {
    int i,j;
    object *tmp;
    for(i=xstart;i<xstart + xsize;i++)
        for(j=ystart;j<ystart +ysize;j++) {
            for(tmp=get_map_ob(map,i,j);tmp!=NULL;tmp=tmp->above) {
                if(!QUERY_FLAG(tmp,FLAG_IS_FLOOR)) {
                if(tmp->head) tmp=tmp->head;
                remove_ob(tmp);
                free_object(tmp);
                tmp=get_map_ob(map,i,j);
                }
                if(tmp==NULL) break;
            }
        }
}

/**
 * Copy in_map into dest_map at point x,y
 * @param dest_map
 * map where to copy to.
 * @param in_map
 * map to copy from.
 * @param x
 * @param y
 * coordinates to put in_map to.
 */
void include_map_in_map(mapstruct *dest_map, mapstruct *in_map,int x, int y) {
    int i,j;
    object *tmp;
    object *new_ob;

    /* First, splatter everything in the dest map at the location */
    nuke_map_region(dest_map,x,y,MAP_WIDTH(in_map),MAP_HEIGHT(in_map));

    for(i=0;i<MAP_WIDTH(in_map);i++)
        for(j=0;j<MAP_HEIGHT(in_map);j++) {
            for(tmp=get_map_ob(in_map,i,j);tmp!=NULL;tmp=tmp->above) {
                /* don't copy things with multiple squares:  must be dealt with
                   specially. */
                if(tmp->head!=NULL) continue;
                new_ob = arch_to_object(tmp->arch);
                copy_object_with_inv(tmp,new_ob);
                if(QUERY_FLAG(tmp,FLAG_IS_LINKED))
                    add_button_link(new_ob,dest_map,tmp->path_attuned);
                new_ob->x = i + x;
                new_ob->y = j + y;
                insert_multisquare_ob_in_map(new_ob,dest_map);
            }
        }
}

/**
 * Finds a place to put a submap into.
 * @param map
 * map to insert.
 * @param layout
 * where to insert into.
 * @param ix
 * @param iy
 * coordinates of suitable location if return is 1.
 * @param xsize
 * @param ysize
 * size of layout.
 * @return
 * 0 if no space found, 1 else.
 */
int find_spot_for_submap(mapstruct *map,char **layout,int *ix, int *iy,int xsize, int ysize) {
    int tries;
    int i=0,j=0;	/* initialization may not be needed but prevents compiler warnings */
    int is_occupied=0;
    int l,m;

    /* don't even try to place a submap into a map if the big map isn't
       sufficiently large. */
    if(2*xsize > MAP_WIDTH(map) || 2*ysize > MAP_HEIGHT(map)) return 0;

    /* search a bit for a completely free spot. */
    for(tries=0;tries<20;tries++) {
        /* pick a random location in the layout */
        i = RANDOM() % (MAP_WIDTH(map) - xsize-2)+1;
        j = RANDOM() % (MAP_HEIGHT(map) - ysize-2)+1;
        is_occupied=0;
        for(l=i;l<i + xsize;l++)
            for(m=j;m<j + ysize;m++)
                is_occupied|=layout[l][m];
        if(!is_occupied) break;
    }

    /* if we failed, relax the restrictions */
    if(is_occupied) { /* failure, try a relaxed placer. */
        /* pick a random location in the layout */
        for(tries=0;tries<10;tries++) {
        i = RANDOM() % (MAP_WIDTH(map) - xsize-2)+1;
        j = RANDOM() % (MAP_HEIGHT(map) - ysize-2)+1;
        is_occupied=0;
        for(l=i;l<i + xsize;l++)
            for(m=j;m<j + ysize;m++)
                if(layout[l][m]=='C' || layout[l][m]=='>' || layout[l][m]=='<')
                    is_occupied|=1;
        }
    }
    if(is_occupied) return 0;
    *ix=i;
    *iy=j;
    return 1;
}

/**
 * Places a special fountain on the map.
 * @param map
 * map to place to.
 * @todo
 * change logic to allocate potion only if success?
 */
void place_fountain_with_specials(mapstruct *map) {
    int ix,iy,i=-1,tries=0;
    mapstruct *fountain_style=find_style("/styles/misc","fountains",-1);
    object *fountain=create_archetype("fountain");
    object *potion=get_object();
    copy_object(pick_random_object(fountain_style),potion);
    while(i<0 && tries<10) {
        ix = RANDOM() % (MAP_WIDTH(map) -2) +1;
        iy = RANDOM() % (MAP_HEIGHT(map) -2) +1;
        i = find_first_free_spot(fountain,map,ix,iy);
        tries++;
    };
    if(i==-1) {  /* can't place fountain */
        free_object(fountain);
        free_object(potion);
        return;
    }
    ix += freearr_x[i];
    iy += freearr_y[i];
    potion->face=fountain->face;
    SET_FLAG(potion,FLAG_NO_PICK);
    SET_FLAG(potion,FLAG_IDENTIFIED);
    potion->name=add_string("fountain");
    potion->name_pl=add_string("fountain");
    potion->x = ix;
    potion->y = iy;
    potion->material=M_ADAMANT;
    fountain->x = ix;
    fountain->y = iy;
    insert_ob_in_map(fountain,map,NULL,0);
    insert_ob_in_map(potion,map,NULL,0);
}

/**
 * Place an exit with a resource map.
 * @param map
 * where to put the exit.
 * @param hole_type
 * type of random map to link to.
 * @param RP
 * parameters from which map was generated.
 * @todo
 * link to holy types when transformed to enum.
 */
void place_special_exit(mapstruct * map, int hole_type,RMParms *RP) {
    int ix,iy,i=-1;
    char buf[HUGE_BUF], *style, *decor, *mon;
    mapstruct *exit_style=find_style("/styles/misc","obscure_exits",-1);
    int g_xsize,g_ysize;

    object *the_exit=get_object();
    if(!exit_style) return;

    copy_object(pick_random_object(exit_style),the_exit);

    while(i<0) {
        ix = RANDOM() % (MAP_WIDTH(map) -2) +1;
        iy = RANDOM() % (MAP_HEIGHT(map) -2) +1;
        i = find_first_free_spot(the_exit,map,ix,iy);
    }

    ix += freearr_x[i];
    iy += freearr_y[i];
    the_exit->x = ix;
    the_exit->y = iy;

    if(!hole_type) hole_type = RANDOM() % NR_OF_HOLE_TYPES + 1 ;

    switch(hole_type) {
        case GLORY_HOLE:   /* treasures */
        {
            g_xsize = RANDOM() %3 + 4 + RP->difficulty/4;
            g_ysize = RANDOM() %3 + 4 + RP->difficulty/4;
            style = "onion";
            decor = "wealth2";
            mon = "none";
            break;
        }

        case ORC_ZONE:  /* hole with orcs in it. */
        {
            g_xsize = RANDOM() %3 + 4 + RP->difficulty/4;
            g_ysize = RANDOM() %3 + 4 + RP->difficulty/4;
            style = "onion";
            decor = "wealth2";
            mon = "orc";
            break;
        }

        case MINING_ZONE:  /* hole with orcs in it. */
        {
            g_xsize = RANDOM() %9 + 4 + RP->difficulty/4;
            g_ysize = RANDOM() %9 + 4 + RP->difficulty/4;
            style = "maze";
            decor = "minerals2";
            mon = "none";
            break;
        }

        default:  /* undefined */
            LOG(llevError, "place_special_exit: undefined hole type %d\n", hole_type);
            return;
            break;
    }

    /* Need to be at least this size, otherwise the load
     * code will generate new size values which are too large.
     */
    if (g_xsize < MIN_RANDOM_MAP_SIZE) g_xsize = MIN_RANDOM_MAP_SIZE;
    if (g_ysize < MIN_RANDOM_MAP_SIZE) g_ysize = MIN_RANDOM_MAP_SIZE;

    write_parameters_to_string(buf, g_xsize, g_ysize,RP->wallstyle,RP->floorstyle,mon,
                                 "none",style,decor,"none",RP->exitstyle,0,0,0,
                                 OPT_WALLS_ONLY,0,0,1,RP->dungeon_level,RP->dungeon_level,
                                 RP->difficulty,RP->difficulty,-1,1,0,0,0,0, RP->difficulty_increase);
    the_exit->slaying = add_string("/!");
    the_exit->msg = add_string(buf);

    insert_ob_in_map(the_exit,map,NULL,0);
}

/**
 * Main function for specials.
 * @param map
 * map to alter.
 * @param layout
 * layout the map was generated from.
 * @param RP
 * parameters the map was generated from.
 */
void place_specials_in_map(mapstruct *map, char **layout,RMParms *RP) {
    mapstruct *special_map;
    int ix,iy;  /* map insertion locatons */
    int special_type; /* type of special to make */

    special_type = RANDOM() % NUM_OF_SPECIAL_TYPES;
    switch(special_type) {

        /* includes a special map into the random map being made. */
        case SPECIAL_SUBMAP: {
            special_map = find_style("/styles/specialmaps",0,RP->difficulty);
            if(special_map==NULL) return;

            if(find_spot_for_submap(map,layout,&ix,&iy,MAP_WIDTH(special_map),MAP_HEIGHT(special_map)))
                include_map_in_map(map,special_map,ix,iy);
            break;
        }

        /* Make a special fountain:  an unpickable potion disguised as
           a fountain, or rather, colocated with a fountain. */
        case SPECIAL_FOUNTAIN: {
            place_fountain_with_specials(map);
            break;
        }

        /* Make an exit to another random map, e.g. a gloryhole. */
        case SPECIAL_EXIT: {
            place_special_exit(map,0,RP);
            break;
        }
    }
}

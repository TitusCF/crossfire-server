/*
 * static char *rcsid_build_map =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001-2006 Mark Wedel & Crossfire Development Team
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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/

/**
 * @file
 * This file handles all ingame construction functions: builders, destroyers, building materials.
 *
 * Basically, those enable a player to alter in real-time a map.
 *
 * @todo document building, forces used to store connection values, ... Make functions static.
 */

#include <global.h>
#include <living.h>
#include <spells.h>
#include <skills.h>
#include <tod.h>
#include <sproto.h>

/**
 * Check if objects on a square interfere with building.
 *
 * @param map
 * map we're building on.
 * @param tmp
 * item the player is trying to build.
 * @param x
 * @param y
 * coordinates where to build.
 * @return
 * 0 if tmp can't be built on the spot, 1 if it can be built.
 */
int can_build_over(struct mapdef* map, object* tmp, short x, short y) {
    object* ob;

    for ( ob = GET_MAP_OB( map, x, y ); ob; ob = ob->above) {

        if (strcmp( ob->arch->name, "rune_mark" ) == 0)
            /* you can always build on marking runes, used for connected building things. */
            continue;

        if ((ob->head && QUERY_FLAG(ob->head, FLAG_IS_BUILDABLE)) || (!ob->head && QUERY_FLAG(ob, FLAG_IS_BUILDABLE)))
            /* Check for the flag is required, as this function
             * can be called recursively on different spots.
             */
          continue;

        switch ( tmp->type ) {
            case SIGN:
            case MAGIC_EAR:
                /* Allow signs and magic ears to be built on books */
                if ( ob->type != BOOK )
                    return 0;

                break;
            case BUTTON:
            case DETECTOR:
            case PEDESTAL:
            case CF_HANDLE:
                /* Allow buttons and levers to be built under gates */
                if ( ob->type != GATE && ob->type != DOOR )
                    return 0;

                break;
            default:
                return 0;
        }
    }

    /* If builded item is multi-tile, need to check other parts too. */
    if (tmp->more)
        return can_build_over(map, tmp->more, x + tmp->more->arch->clone.x - tmp->arch->clone.x, y + tmp->more->arch->clone.y - tmp->arch->clone.y);

    return 1;
    }

/**
 *  Erases all marking runes at specified location
 *
 * @param map
 * @param x
 * @param y
 * coordinates to erase runes at.
 */
void remove_marking_runes( struct mapdef* map, short x, short y )
    {
    object* rune;
    object* next;

    rune = GET_MAP_OB( map, x, y );
    while ( rune )
        {
        next = rune->above;
        if ( ( rune->type == SIGN ) && ( !strcmp( rune->arch->name, "rune_mark" ) ) )
            {
            remove_ob( rune );
            free_object( rune );
            }
        rune = next;
        }
    }

/**
 * Returns an unused value for 'connected'.
 *
 * Tries 1000 random values, then returns -1.
 *
 * @param map
 * map for which to find a value
 * @return
 * 'connected' value with no item, or -1 if failure.
 */
int find_unused_connected_value( struct mapdef* map )
    {
    int connected = 0;
    int itest = 0;
    oblinkpt* obp;

    while ( itest++ < 1000 )
        {
        connected = 1 + rand( ) % 20000;
        for ( obp = map->buttons; obp && ( obp->value != connected ); obp = obp->next );

        if ( !obp )
            return connected;
        }

    return -1;
    }


/**
 * Helper function for door/button/connected item building.
 *
 * Will search the specified spot for a marking rune.
 * If not found, returns -1
 * Else, searches a force in op's inventory matching the map's name
 * and the rune's text.
 * If found, returns the connection value associated
 * else searches a new connection value, and adds the force to the player.
 *
 * @param pl
 * player building.
 * @param x
 * @param y
 * coordinates where to build.
 * @param rune
 * rune used to indicate the connection value. If NULL, building is searched for one.
 * @return
 * -1 for failure, else connection value.
 */
int find_or_create_connection_for_map( object* pl, short x, short y, object* rune )
    {
    object* force;
    int connected;

    if ( !rune )
        rune = get_connection_rune( pl, x, y );

    if ( !rune || !rune->msg )
        {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
		      "You need to put a marking rune with the group name.", NULL);
        return -1;
        }

    /* Now, find force in player's inventory */
    force = pl->inv;
    while ( force && ( ( force->type != FORCE ) || ( !force->slaying ) || ( strcmp( force->slaying, pl->map->path ) ) || ( !force->msg ) || ( strcmp( force->msg, rune->msg ) ) ) )
        force = force->below;

    if ( !force )
        /* No force, need to create & insert one */
        {
        /* Find unused value */
        connected = find_unused_connected_value( pl->map );
        if ( connected == -1 )
            {
            draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                "Could not create more groups.", NULL);
            return -1;
            }

        force = create_archetype( FORCE_NAME );
        force->speed = 0;
        update_ob_speed( force );
        force->slaying = add_string( pl->map->path );
        force->msg = add_string( rune->msg );
        force->path_attuned = connected;
        insert_ob_in_ob( force, pl );

        return connected;
        }

    /* Found the force, everything's easy. */
    return force->path_attuned;
    }

/**
 * Returns the marking rune on the square, for purposes of building connections.
 *
 * @param pl
 * player trying to build.
 * @param x
 * @param y
 * coordinates to search.
 * @return
 * marking rune, NULL if none found.
 */
object* get_connection_rune( object* pl, short x, short y )
    {
    object* rune;

    rune = GET_MAP_OB( pl->map, x, y );
    while ( rune && ( ( rune->type != SIGN ) || ( strcmp( rune->arch->name, "rune_mark" ) ) ) )
        rune = rune->above;
    return rune;
    }

/**
 * Returns the book/scroll on the current square, for purposes of building
 *
 * @param pl
 * player trying to build.
 * @param x
 * @param y
 * coordinates to search.
 * @return
 * book, NULL if none found.
 */
object* get_msg_book( object* pl, short x, short y )
    {
    object* book;

    book = GET_MAP_OB( pl->map, x, y );
    while ( book && ( book->type != BOOK ) )
        book = book->above;
    return book;
    }

/**
 * Returns first item of type WALL.
 *
 * @param map
 * @param x
 * @param y
 * where to search.
 * @return
 * wall, or NULL if none found.
 * @todo isn't there a similar function somewhere? put this in a common library?
 */
object* get_wall( struct mapdef* map, int x, int y )
    {
    object* wall;

    wall = GET_MAP_OB( map, x, y );
    while ( wall && ( WALL != wall->type ) )
        wall = wall->above;

    return wall;
    }

/**
 * Fixes walls around specified spot
 *
 * Basically it ensures the correct wall is put where needed.
 *
 * @note
 * x & y must be valid map coordinates.
 *
 * @param map
 * @param x
 * @param y
 * position to fix.
 * @todo use safe string functions.
 */
void fix_walls( struct mapdef* map, int x, int y )
    {
    int connect;
    object* wall;
    char archetype[ MAX_BUF ];
    char* underscore;
    uint32 old_flags[ 4 ];
    struct archt* new_arch;
    int flag;

    /* First, find the wall on that spot */
    wall = get_wall( map, x, y );
    if ( !wall )
        /* Nothing -> bail out */
        return;

    /* Find base name */
    strcpy( archetype, wall->arch->name );
    underscore = strchr( archetype, '_' );
    if ( !underscore || ( !isdigit( *( underscore + 1 ) ) ) )
        /* Not in a format we can change, bail out */
        return;

    underscore++;
    *underscore = '\0';

    connect = 0;

    if ( ( x > 0 ) && get_wall( map, x - 1, y ) )
        connect |= 1;
    if ( ( x < MAP_WIDTH( map ) - 1 ) && get_wall( map, x + 1, y ) )
        connect |= 2;

    if ( ( y > 0 ) && get_wall( map, x, y - 1 ) )
        connect |= 4;

    if ( ( y < MAP_HEIGHT( map ) - 1 ) && get_wall( map, x, y + 1 ) )
        connect |= 8;

    switch( connect )
        {
        case 0:
            strcat( archetype, "0");
            break;
        case 1:
            strcat( archetype, "1_3");
            break;
        case 2:
            strcat( archetype, "1_4");
            break;
        case 3:
            strcat( archetype, "2_1_2");
            break;
        case 4:
            strcat( archetype, "1_2");
            break;
        case 5:
            strcat( archetype, "2_2_4");
            break;
        case 6:
            strcat( archetype, "2_2_1");
            break;
        case 7:
            strcat( archetype, "3_1");
            break;
        case 8:
            strcat( archetype, "1_1");
            break;
        case 9:
            strcat( archetype, "2_2_3");
            break;
        case 10:
            strcat( archetype, "2_2_2");
            break;
        case 11:
            strcat( archetype, "3_3");
            break;
        case 12:
            strcat( archetype, "2_1_1");
            break;
        case 13:
            strcat( archetype, "3_4");
            break;
        case 14:
            strcat( archetype, "3_2");
            break;
        case 15:
            strcat( archetype, "4");
            break;
        }

    /*
     * Before anything, make sure the archetype does exist...
     * If not, prolly an error...
     */
    new_arch = find_archetype( archetype );
    if ( !new_arch )
        return;

    /* Now delete current wall, and insert new one
     * We save flags to avoid any trouble with buildable/non buildable, and so on
     */
    for ( flag = 0; flag < 4; flag++ )
        old_flags[ flag ] = wall->flags[ flag ];
    remove_ob( wall );
    free_object( wall );

    wall = arch_to_object( new_arch );
    wall->type = WALL;
    insert_ob_in_map_at( wall, map, NULL, INS_ABOVE_FLOOR_ONLY, x, y );
    for ( flag = 0; flag < 4; flag++ )
        wall->flags[ flag ] = old_flags[ flag ];
    }

/**
 * Floor building function.
 *
 * Floors can be build:
 *  - on existing floors, with or without a detector/button
 *  - on an existing wall, with or without a floor under it
 *
 * @note
 * this function will inconditionally change squares around (x, y)
 * so don't call it with x == 0 for instance!
 *
 * @param pl
 * player building.
 * @param material
 * floor being built.
 * @param x
 * @param y
 * where to build.
 * @todo use safe string functions
 */
void apply_builder_floor(object* pl, object* material, short x, short y )
    {
    object* tmp, *above;
    object* above_floor; /* Item above floor, if any */
    object* floor;       /* Floor which would be removed if required */
    struct archt* new_floor;
    struct archt* new_wall;
    int i, xt, yt, wall_removed;
    char message[ MAX_BUF ];

    sprintf( message, "You change the floor to better suit your tastes." );

    /*
     * Now the building part...
     * First, remove wall(s) and floor(s) at position x, y
     */
    above_floor = NULL;
    floor = NULL;
    new_wall = NULL;
    wall_removed = 0;
    tmp = GET_MAP_OB( pl->map, x, y );
    while ( tmp )
        {
        above = tmp->above;
        if ( WALL == tmp->type )
            {
            /* There was a wall, remove it & keep its archetype to make new walls */
            new_wall = tmp->arch;
            remove_ob( tmp );
            free_object( tmp );
            sprintf( message, "You destroy the wall and redo the floor." );
            wall_removed = 1;
            if ( floor != NULL ) {
                remove_ob(floor);
                free_object(floor);
            }
            }
        else if ( ( FLOOR == tmp->type ) || ( QUERY_FLAG(tmp, FLAG_IS_FLOOR ) ) )
            {
            floor = tmp;
            }
        else
            {
            if ( floor != NULL )
                above_floor = tmp;
            }

        tmp = above;
        }

    if ( wall_removed == 0 && floor != NULL ) {
        if ( floor->arch->name == material->slaying ) {
            draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD, "You feel too lazy to redo the exact same floor.", NULL);
            return;
        }
    }

    /* Now insert our floor */
    new_floor = find_archetype( material->slaying );
    if ( !new_floor )
        {
        /* Not found, log & bail out */
        LOG( llevError, "apply_builder_floor: unable to find archetype %s.\n", material->slaying );
        return;
        }

    tmp = arch_to_object( new_floor );
    SET_FLAG( tmp, FLAG_IS_BUILDABLE );
    SET_FLAG( tmp, FLAG_UNIQUE );
    SET_FLAG( tmp, FLAG_IS_FLOOR );
    tmp->type = FLOOR;
    insert_ob_in_map_at( tmp, pl->map, above_floor, above_floor ? INS_BELOW_ORIGINATOR : INS_ON_TOP, x, y );

    /*
     * Next step: make sure there are either walls or floors around the new square
     * Since building, you can have: blocking view / floor / wall / nothing
     */
    for ( i = 1; i <= 8; i++ )
        {
        xt = x + freearr_x[ i ];
        yt = y + freearr_y[ i ];
        tmp = GET_MAP_OB( pl->map, xt, yt );
        if ( !tmp )
            {
            /* Must insert floor & wall */
            tmp = arch_to_object( new_floor );
            /* Better make the floor unique */
            SET_FLAG( tmp, FLAG_UNIQUE );
            SET_FLAG( tmp, FLAG_IS_BUILDABLE );
            tmp->type = FLOOR;
            insert_ob_in_map_at( tmp, pl->map, 0, 0, xt, yt );
            /* Insert wall if exists. Note: if it doesn't, the map is weird... */
            if ( new_wall )
                {
                tmp = arch_to_object( new_wall );
                SET_FLAG( tmp, FLAG_IS_BUILDABLE );
                tmp->type = WALL;
                insert_ob_in_map_at( tmp, pl->map, 0, 0, xt, yt );
                }
            }
        }

    /* Finally fixing walls to ensure nice continuous walls
     * Note: 2 squares around are checked, because potentially we added walls around the building
     * spot, so need to check that those new walls connect correctly
     */
    for ( xt = x - 2; xt <= x + 2; xt++ )
        for ( yt = y - 2; yt <= y + 2; yt++ )
            {
            if ( !OUT_OF_REAL_MAP( pl->map, xt, yt ) )
                fix_walls( pl->map, xt, yt );
            }

    /* Now remove raw item from inventory */
    decrease_ob( material );

    /* And tell player about the fix */
    draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD, message, NULL);
    }

/**
 * Wall building function
 *
 * Walls can be build:
 *  - on a floor without anything else
 *  - on an existing wall, with or without a floor
 *
 * @param pl
 * player building.
 * @param material
 * wall being built.
 * @param x
 * @param y
 * where to build.
 * @todo use safe string functions
 */
void apply_builder_wall( object* pl, object* material, short x, short y )
    {
    object* current_wall;
    object* tmp;
    int xt, yt;
    struct archt* new_wall;
    char message[ MAX_BUF ];

    remove_marking_runes( pl->map, x, y );

    /* Grab existing wall, if any */
    current_wall = NULL;
    tmp = GET_MAP_OB( pl->map, x, y );
    while ( tmp && !current_wall )
        {
        if ( WALL == tmp->type )
            current_wall = tmp;

        tmp = tmp->above;
        }

    /* Find the raw wall in inventory */
    sprintf( message, "You build a wall." );

    /* Now we can actually insert the wall */
    new_wall = find_archetype( material->slaying );
    if ( !new_wall )
        {
	    LOG( llevError, "apply_builder_wall: unable to find archetype %s\n", material->slaying );
        return;
        }

    tmp = arch_to_object( new_wall );
    tmp->type = WALL;
    SET_FLAG( tmp, FLAG_IS_BUILDABLE );
    insert_ob_in_map_at( tmp, pl->map, 0, INS_ABOVE_FLOOR_ONLY, x, y );

    /* If existing wall, remove it, no need to fix other walls */
    if ( current_wall )
        {
        remove_ob( current_wall );
        free_object( current_wall );
        fix_walls( pl->map, x, y );
        sprintf( message, "You redecorate the wall to better suit your tastes." );
        }
    else
        {
        /* Else fix all walls around */
        for ( xt = x - 1; xt <= x + 1; xt++ )
            for ( yt = y - 1; yt <= y + 1; yt++ )
                {
                if ( OUT_OF_REAL_MAP( pl->map, xt, yt ) )
                    continue;

                fix_walls( pl->map, xt, yt );
                }
        }

    /* Now remove item from inventory */
    decrease_ob( material );

    /* And tell player what happened */
    draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD, message, NULL);
    }

/**
 * Generic item builder.
 *
 * Item must be put on a square with a floor, you can have something under.
 *
 * Archetype of created object is item->slaying (raw material).
 *
 * Type of inserted item is tested for specific cases (doors & such).
 *
 * Item is inserted above the floor.
 *
 * Note that it is the responsability of the caller to check whether the space is buildable or not.
 *
 * @param pl
 * player building.
 * @param item
 * item being built.
 * @param x
 * @param y
 * where to build.
 */
void apply_builder_item( object* pl, object* item, short x, short y )
    {
    object* tmp;
    struct archt* arch;
    int insert_flag;
    object* floor;
    object* con_rune;
    int connected;
    char name[MAX_BUF];

    /* Find floor */
    floor = GET_MAP_OB( pl->map, x, y );
    if ( !floor )
        {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,"Invalid square.", NULL);
        return;
        }

    while ( floor && ( floor->type != FLOOR ) && ( !QUERY_FLAG( floor, FLAG_IS_FLOOR ) ) )
        floor = floor->above;

    if ( !floor )
        {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "This square has no floor, you can't build here.", NULL);
        return;
        }
    /* Create item, set flag, insert in map */
    arch = find_archetype( item->slaying );
    if ( !arch )
        return;

    tmp = object_create_arch(arch);
    SET_FLAG( tmp, FLAG_IS_BUILDABLE );
    SET_FLAG( tmp, FLAG_NO_PICK );

    /*
     * This doesn't work on non unique maps. pedestals under floor will not be saved...
    insert_flag = ( item->stats.Str == 1 ) ? INS_BELOW_ORIGINATOR : INS_ABOVE_FLOOR_ONLY;
    */
    insert_flag = INS_ABOVE_FLOOR_ONLY;

    connected = 0;
    switch( tmp->type )
        {
        case DOOR:
        case GATE:
        case BUTTON:
        case DETECTOR:
        case TIMED_GATE:
        case PEDESTAL:
        case CF_HANDLE:
        case MAGIC_EAR:
        case SIGN:
            /* Signs don't need a connection, but but magic mouths do. */
            if (tmp->type == SIGN && strcmp( tmp->arch->name, "magic_mouth" ))
                break;
            con_rune = get_connection_rune( pl, x, y );
            connected = find_or_create_connection_for_map( pl, x, y, con_rune );
            if ( connected == -1 )
                {
                /* Player already informed of failure by the previous function */
                free_object( tmp );
                return;
                }
            /* Remove marking rune */
            remove_ob( con_rune );
            free_object( con_rune );
        }

    /* For magic mouths/ears, and signs, take the msg from a book of scroll */
    if ((tmp->type == SIGN) || (tmp->type == MAGIC_EAR)) {
        if (adjust_sign_msg( pl, x, y, tmp ) == -1) {
            free_object( tmp );
            return;
        }
    }

    insert_ob_in_map_at( tmp, pl->map, floor, insert_flag, x, y );
    if ( connected != 0 )
        add_button_link( tmp, pl->map, connected );

    query_name( tmp, name, MAX_BUF );
    draw_ext_info_format( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
        "You build the %s",
        "You build the %s",
        name);
    decrease_ob_nr( item, 1 );
    }

/**
 * Item remover.
 *
 * Removes first buildable item, either under or above the floor
 *
 * @param pl
 * player removing an item.
 * @param dir
 * direction the player is trying to remove.
 */
void apply_builder_remove( object* pl, int dir )
    {
    object* item;
    short x, y;
    char name[MAX_BUF];

    x = pl->x + freearr_x[ dir ];
    y = pl->y + freearr_y[ dir ];

    /* Check square */
    item = GET_MAP_OB( pl->map, x, y );
    if ( !item ) {
        /* Should not happen with previous tests, but we never know */
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "Invalid square.", NULL);
        LOG( llevError, "apply_builder_remove: (null) square at (%d, %d, %s)\n", x, y, pl->map->path );
        return;
    }

    if ( item->type == FLOOR || QUERY_FLAG(item,FLAG_IS_FLOOR) )
        item = item->above;

    if ( !item ) {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "Nothing to remove.", NULL);
        return;
    }

    /* Now remove object, with special cases (buttons & such) */
    switch ( item->type )
        {
        case WALL:
            draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                "Can't remove a wall with that, build a floor.", NULL);
            return;

        case DOOR:
        case BUTTON:
        case GATE:
        case TIMED_GATE:
        case DETECTOR:
        case PEDESTAL:
        case CF_HANDLE:
        case MAGIC_EAR:
        case SIGN:
            /* Special case: must unconnect */
	    if (QUERY_FLAG(item,FLAG_IS_LINKED))
                remove_button_link( item );

            /* Fall through */

        default:
            /* Remove generic item */
            query_name( item, name, MAX_BUF );
            draw_ext_info_format( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                "You remove the %s",
                "You remove the %s",
                name );
            remove_ob( item );
            free_object( item );
        }
    }

/**
 * Global building function
 *
 * This is the general map building function. Called when the player 'fires' a builder
 * or remover object.
 *
 * @param pl
 * player building or removing.
 * @param dir
 * building direction.
 */
void apply_map_builder( object* pl, int dir ) {
    object* builder;
    object* tmp;
    short x, y;

    if ( !pl->type == PLAYER )
        return;

    /*if ( !player->map->unique )
        {
	    draw_ext_info( NDI_UNIQUE, 0, player, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
			  "You can't build outside a unique map.", NULL);
	    return;
        }*/

    if ( dir == 0 ) {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "You can't build or destroy under yourself.", NULL);
        return;
    }

    x = pl->x + freearr_x[ dir ];
    y = pl->y + freearr_y[ dir ];

    if ( ( 1 > x ) || ( 1 > y ) || ( ( MAP_WIDTH( pl->map ) - 2 ) < x ) || ( ( MAP_HEIGHT( pl->map ) - 2 ) < y ) )
        {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "Can't build on map edge.", NULL);
        return;
        }

    /*
     * Check specified square
     * The square must have only buildable items
     * Exception: marking runes are all right,
     * since they are used for special things like connecting doors / buttons
     */

    tmp = GET_MAP_OB( pl->map, x, y );
    if ( !tmp )
        {
        /* Nothing, meaning player is standing next to an undefined square... */
        LOG( llevError, "apply_map_builder: undefined square at (%d, %d, %s)\n", x, y, pl->map->path );
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "You'd better not build here, it looks weird.", NULL);
        return;
        }

    builder = pl->contr->ranges[ range_builder ];

    if (builder->subtype != ST_BD_BUILD) {
        while ( tmp ) {
            if ( !QUERY_FLAG( tmp, FLAG_IS_BUILDABLE ) && ( ( tmp->type != SIGN )
                || ( strcmp( tmp->arch->name, "rune_mark" ) ) ) ) {
                draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                    "You can't build here.", NULL);
                return;
            }
            tmp = tmp->above;
        }
    }

    /* Now we know the square is ok */

    if ( builder->subtype == ST_BD_REMOVE )
        /* Remover -> call specific function and bail out */
        {
        apply_builder_remove( pl, dir );
        return;
        }

    if ( builder->subtype == ST_BD_BUILD )
        /*
         * Builder.
         * Find marked item to build, call specific function
         */
        {
        tmp = find_marked_object(pl);
        if ( !tmp ) {
            draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                "You need to mark raw materials to use.", NULL);
            return;
        }

        if ( tmp->type != MATERIAL ) {
            draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                "You can't use the marked item to build.", NULL);
	        return;
        }

        if (!can_build_over(pl->map, tmp, x, y)) {
            draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                "You can't build here.", NULL);
            return;
        }

        switch( tmp->subtype )
            {
            case ST_MAT_FLOOR:
                apply_builder_floor( pl, tmp, x, y );
                return;

            case ST_MAT_WALL:
                apply_builder_wall( pl, tmp, x, y );
                return;

            case ST_MAT_ITEM:
                apply_builder_item( pl, tmp, x, y );
                return;

            default:
                draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
                    "Don't know how to apply this material, sorry.", NULL);
                LOG( llevError, "apply_map_builder: invalid material subtype %d\n", tmp->subtype );
                return;
            }
        }

    /* Here, it means the builder has an invalid type */
    draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
        "Don't know how to apply this tool, sorry.", NULL);
    LOG( llevError, "apply_map_builder: invalid builder subtype %d\n", builder->subtype );
    }

/**
 * Make the built object inherit the msg of books that are used with it.
 * For objects already invisible (i.e. magic mouths & ears), also make it
 * it inherit the face and the name with "talking " prepended.
 *
 * The book is removed during the operation.
 *
 * @param pl
 * player building.
 * @param x
 * @param y
 * building coordinates.
 * @param tmp
 * object that is being built.
 * @return
 * -1 if no text found, 0 if ok to build.
 */
int adjust_sign_msg( object* pl, short x, short y, object* tmp )
    {
    object* book;
    char buf[MAX_BUF];
    char buf2[MAX_BUF];

    book = get_msg_book( pl, x, y );
    if ( !book ) {
        draw_ext_info( NDI_UNIQUE, 0, pl, MSG_TYPE_APPLY, MSG_TYPE_APPLY_BUILD,
            "You need to put a book or scroll with the message.", NULL);
        return -1;
    }

    tmp->msg = book->msg;
    add_refcount( tmp->msg );

    if (tmp->invisible) {
        if(book->custom_name != NULL) {
            snprintf(buf, sizeof(buf), "talking %s", book->custom_name);
        } else {
            snprintf(buf, sizeof(buf), "talking %s", book->name);
        }
        if ( tmp->name )
            free_string( tmp->name );
        tmp->name = add_string( buf );

        if(book->name_pl != NULL) {
            snprintf(buf2, sizeof(buf2), "talking %s", book->name_pl);
            if ( tmp->name_pl )
                free_string( tmp->name_pl );
            tmp->name_pl = add_string( buf2 );
        }

        tmp->face = book->face;
        tmp->invisible = 0;
    }
    remove_ob( book );
    free_object( book );
    return 0;
}

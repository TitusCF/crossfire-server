/*
 * static char *rcsid_build_map =
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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/

#include <global.h>
#include <living.h>
#include <spells.h>
#include <skills.h>
#include <tod.h>
#include <sproto.h>

/**
 *  Erases marking runes at specified location
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
 * \param map: map for which to find a value
 * \return 'connected' value with no item, or -1 if failure.
 *
 * Tries 1000 random values, then returns -1.
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
 */
int find_or_create_connection_for_map( object* player, short x, short y )
    {
    object* rune;
    object* force;
    int connected;

    rune = GET_MAP_OB( player->map, x, y );
    while ( rune && ( ( rune->type != SIGN ) || ( strcmp( rune->arch->name, "rune_mark" ) ) ) )
        rune = rune->above;

    if ( !rune )
        {
        new_draw_info( NDI_UNIQUE, 0, player, "You need to put a marking rune with the group name." );
        return -1;
        }

    /* Now, find force in player's inventory */
    force = player->inv;
    while ( force && ( ( force->type != FORCE ) || ( !force->slaying ) || ( strcmp( force->slaying, player->map->path ) ) || ( !force->msg ) || ( strcmp( force->msg, rune->msg ) ) ) )
        force = force->below;

    if ( !force )
        /* No force, need to create & insert one */
        {
        /* Find unused value */
        connected = find_unused_connected_value( player->map );
        if ( connected == -1 )
            {
            new_draw_info( NDI_UNIQUE, 0, player, "Could not create more groups." );
            return -1;
            }

        force = get_archetype( "force" );
        force->speed = 0;
        update_ob_speed( force );
        force->slaying = add_string( player->map->path );
        force->msg = add_string( rune->msg );
        force->path_attuned = connected;
        insert_ob_in_ob( force, player );

        return connected;
        }

    /* Found the force, everything's easy. */
    return force->path_attuned;
    }

/**
 * Returns first item of type WALL.
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
 *   Fixes walls around specified spot
 *
 *   \param map is the map
 *   \param x
 *   \param y are the position to fix
 *
 *   Basically it ensures the correct wall is put where needed.
 *
 *   Note: x & y must be valid map coordinates.
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
 *   \brief Floor building function
 *
 *   Floors can be build:
 *    - on existing floors, with or without a detector/button
 *    - on an existing wall, with or without a floor under it
 *
 *   Note: this function will inconditionally change squares around (x, y)
 *    so don't call it with x == 0 for instance!
 */
void apply_builder_floor(object* player, object* material, short x, short y )
    {
    object* tmp, *above;
    object* above_floor; /* Item above floor, if any */
    char has_floor = 0;
    struct archt* new_floor;
    struct archt* new_wall;
    int i, xt, yt, floor_removed;
    char message[ MAX_BUF ];

    sprintf( message, "You change the floor to better suit your tastes." );

    /*
     * Now the building part... 
     * First, remove wall(s) and floor(s) at position x, y
     */
    above_floor = NULL;
    new_wall = NULL;
    floor_removed = 0;
    tmp = GET_MAP_OB( player->map, x, y );
    if ( tmp )
        {
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
                }
            else if ( FLOOR == tmp->type )
                {
                remove_ob( tmp );
                free_object( tmp );
                floor_removed = 1;
                }
            else
                {
                if ( floor_removed )
                    above_floor = tmp;
                }

            tmp = above;
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
    tmp->type = FLOOR;
    insert_ob_in_map_at( tmp, player->map, above_floor, above_floor ? INS_BELOW_ORIGINATOR : INS_ON_TOP, x, y );

    /*
     * Next step: make sure there are either walls or floors around the new square
     * Since building, you can have: blocking view / floor / wall / nothing
     */
    for ( i = 1; i <= 8; i++ )
        {
        xt = x + freearr_x[ i ];
        yt = y + freearr_y[ i ];
        tmp = GET_MAP_OB( player->map, xt, yt );
        if ( !tmp )
            {
            /* Must insert floor & wall */
            tmp = arch_to_object( new_floor );
            /* Better make the floor unique */
            SET_FLAG( tmp, FLAG_UNIQUE );
            SET_FLAG( tmp, FLAG_IS_BUILDABLE );
            tmp->type = FLOOR;
            insert_ob_in_map_at( tmp, player->map, 0, 0, xt, yt );
            /* Insert wall if exists. Note: if it doesn't, the map is weird... */
            if ( new_wall )
                {
                tmp = arch_to_object( new_wall );
                SET_FLAG( tmp, FLAG_IS_BUILDABLE );
                tmp->type = WALL;
                insert_ob_in_map_at( tmp, player->map, 0, 0, xt, yt );
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
            if ( !OUT_OF_REAL_MAP( player->map, xt, yt ) )
                fix_walls( player->map, xt, yt );
            }

    /* Now remove raw item from inventory */
    decrease_ob( material );

    /* And tell player about the fix */
    new_draw_info( NDI_UNIQUE, 0, player, message );
    }

/**
 * Wall building function
 *
 * Walls can be build:
 *  - on a floor without anything else
 *  - on an existing wall, with or without a floor
 */
void apply_builder_wall( object* player, object* material, short x, short y )
    {
    object* current_wall;
    object* tmp;
    int xt, yt;
    struct archt* new_wall;
    char message[ MAX_BUF ];

    remove_marking_runes( player->map, x, y );    

    /* Grab existing wall, if any */
    current_wall = NULL;
    tmp = GET_MAP_OB( player->map, x, y );
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
    insert_ob_in_map_at( tmp, player->map, 0, INS_ABOVE_FLOOR_ONLY, x, y );

    /* If existing wall, remove it, no need to fix other walls */
    if ( current_wall )
        {
        remove_ob( current_wall );
        free_object( current_wall );
        fix_walls( player->map, x, y );
        sprintf( message, "You redecorate the wall to better suit your tastes." );
        }
    else
        {
        /* Else fix all walls around */
        for ( xt = x - 1; xt <= x + 1; xt++ )
            for ( yt = y - 1; yt <= y + 1; yt++ )
                {
                if ( OUT_OF_REAL_MAP( player->map, xt, yt ) )
                    continue;

                fix_walls( player->map, xt, yt );
                }
        }

    /* Now remove item from inventory */
    decrease_ob( material );

    /* And tell player what happened */
    new_draw_info( NDI_UNIQUE, 0, player, message );
    }

/**
 * Generic item builder.
 *
 * Item must be put on a square with a floor, you can have something under.
 * Archetype of created object is item->slaying (raw material).
 * Type of inserted item is tested for specific cases (doors & such).
 * Item is inserted above the floor, unless Str == 1 (only for detectors i guess)
 */
void apply_builder_item( object* player, object* item, short x, short y )
    {
    object* tmp;
    struct archt* arch;
    int insert_flag;
    object* floor;
    int connected;

    /* Find floor */
    floor = GET_MAP_OB( player->map, x, y );
    if ( !floor )
        {
        new_draw_info( NDI_UNIQUE, 0, player, "Invalid square." );
        return;
        }

    while ( floor && ( floor->type != FLOOR ) )
        floor = floor->above;

    if ( !floor )
        {
        new_draw_info( NDI_UNIQUE, 0, player, "This square has no floor, you can't build here." );
        return;
        }

    if ( ( floor->above ) && ( strcmp( floor->above->arch->name, "rune_mark" ) ) )
        /* Floor has something on top, and it isn't a marking rune */
        {
        new_draw_info( NDI_UNIQUE, 0, player, "You can't build here." );
        return;
        }

    /* Create item, set flag, insert in map */
    arch = find_archetype( item->slaying );
    if ( !arch )
        return;

    tmp = arch_to_object( arch );
    SET_FLAG( tmp, FLAG_IS_BUILDABLE );
    SET_FLAG( tmp, FLAG_NO_PICK );

    insert_flag = ( item->stats.Str == 1 ) ? INS_BELOW_ORIGINATOR : INS_ABOVE_FLOOR_ONLY;

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
            /* Those items require a valid connection */
            connected = find_or_create_connection_for_map( player, x, y );
            if ( connected == -1 )
                {
                /* Player already informed of failure by the previous function */
                free_object( tmp );
                return;
                }
            /* Remove marking rune */
            remove_marking_runes( player->map, x, y );
        }

    insert_ob_in_map_at( tmp, player->map, floor, insert_flag, x, y );
    if ( connected != 0 )
        add_button_link( tmp, player->map, connected );

    new_draw_info_format( NDI_UNIQUE, 0, player, "You build the %s", query_name( tmp ) );
    decrease_ob_nr( item, 1 );
    }

/**
 * Item remover.
 *
 * Removes first buildable item, either under or above the floor
 */
void apply_builder_remove( object* player, int dir )
    {
    object* item;
    short x, y;

    x = player->x + freearr_x[ dir ];
    y = player->y + freearr_y[ dir ];

    /* Check square */
    item = GET_MAP_OB( player->map, x, y );
    if ( !item )
        {
        /* Should not happen with previous tests, but we never know */
        new_draw_info( NDI_UNIQUE, 0, player, "Invalid square." );
	    LOG( llevError, "apply_builder_remove: (null) square at (%d, %d, %s)\n", x, y, player->map->path );
        return;
        }

    if ( item->type == FLOOR )
        item = item->above;

    if ( !item )
        {
        new_draw_info( NDI_UNIQUE, 0, player, "Nothing to remove." );
        return;
        }

    /* Now remove object, with special cases (buttons & such) */
    switch ( item->type )
        {
        case WALL:
            new_draw_info( NDI_UNIQUE, 0, player, "Can't remove a wall with that, build a floor." );
            return;

        case DOOR:
        case BUTTON:
        case GATE:
        case TIMED_GATE:
        case DETECTOR:
        case PEDESTAL:
        case CF_HANDLE:
            /* Special case: must unconnect */
            remove_button_link( item );

            /* Fall through */

        default:
            /* Remove generic item */
            new_draw_info_format( NDI_UNIQUE, 0, player, "You remove the %s", query_name( item ) );
            remove_ob( item );
            free_object( item );
        }
    }

/**
 * Global building function
 *
 * This is the general map building function. Called when the player 'fires' a builder
 * or remover object.
 */
void apply_map_builder( object* player, int dir )
    {
    object* builder;
    object* tmp;
    short x, y;

    if ( !player->type == PLAYER )
        return;

    if ( !player->map->unique )
        {
	    new_draw_info( NDI_UNIQUE, 0, player, "You can't build outside a unique map." );
	    return;
        }

    if ( dir == 0 )
        {
	    new_draw_info( NDI_UNIQUE, 0, player, "You can't build or destroy under yourself." );
	    return;
        }

    x = player->x + freearr_x[ dir ];
    y = player->y + freearr_y[ dir ];

    if ( ( 1 > x ) || ( 1 > y ) || ( ( MAP_WIDTH( player->map ) - 2 ) < x ) || ( ( MAP_HEIGHT( player->map ) - 2 ) < y ) )
        {
        new_draw_info( NDI_UNIQUE, 0, player, "Can't build on map edge..." );
        return;
        }

    /*
     * Check specified square
     * The square must have only buildable items
     * Exception: marking runes are all right,
     * since they are used for special things like connecting doors / buttons
     */

    tmp = GET_MAP_OB( player->map, x, y );
    if ( !tmp )
        {
        /* Nothing, meaning player is standing next to an undefined square... */
	    LOG( llevError, "apply_map_builder: undefined square at (%d, %d, %s)\n", x, y, player->map->path );
        new_draw_info( NDI_UNIQUE, 0, player, "You'd better not build here, it looks weird." );
        return;
        }

    while ( tmp )
        {
        if ( !QUERY_FLAG( tmp, FLAG_IS_BUILDABLE ) && ( ( tmp->type != SIGN ) || ( strcmp( tmp->arch->name, "rune_mark" ) ) ) )
            {
	        new_draw_info( NDI_UNIQUE, 0, player, "You can't build here." );
	        return;
            }
        tmp = tmp->above;
        }

    /* Now we know the square is ok */
    builder = player->contr->ranges[ range_builder ];

    if ( builder->subtype == ST_BD_REMOVE )
        /* Remover -> call specific function and bail out */
        {
        apply_builder_remove( player, dir );
        return;
        }

    if ( builder->subtype == ST_BD_BUILD )
        /*
         * Builder.
         * Find marked item to build, call specific function
         */
        {
        tmp = find_marked_object( player );
        if ( !tmp )
            {
	        new_draw_info( NDI_UNIQUE, 0, player, "You need to mark raw materials to use." );
	        return;
            }

        if ( tmp->type != MATERIAL )
            {
	        new_draw_info( NDI_UNIQUE, 0, player, "You can't use the marked item to build." );
	        return;
            }
	    
        switch( tmp->subtype )
            {
            case ST_MAT_FLOOR:
                apply_builder_floor( player, tmp, x, y );
                return;
        
            case ST_MAT_WALL:
                apply_builder_wall( player, tmp, x, y );
                return;

            case ST_MAT_ITEM:
                apply_builder_item( player, tmp, x, y );
                return;

            default:
                new_draw_info( NDI_UNIQUE, 0, player, "Don't know how to apply this material, sorry." );
	            LOG( llevError, "apply_map_builder: invalid material subtype %d\n", tmp->subtype );
                return;
            }
        }

    /* Here, it means the builder has an invalid type */
    new_draw_info( NDI_UNIQUE, 0, player, "Don't know how to apply this tool, sorry." );
    LOG( llevError, "apply_map_builder: invalid builder subtype %d\n", builder->subtype );
    }

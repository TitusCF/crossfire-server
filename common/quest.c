/*
 * static char *rcsid_quest_c =
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

/*
 This file handles quest-related functions.
 Quests are stored in marker items.
*/

#include <global.h>
#include <commands.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <ctype.h>
#include "quest.h"

int quest_is_same_quest( const char* slaying1, const char* slaying2 )
    {
    char *start1, *start2, *end1, *end2;

    /* Sanity check */
    if ( !slaying1 )
        {
        LOG( llevError, "quest_is_same_quest: slaying1 invalid.\n" );
        return 0;
        }
    if ( !slaying2 )
        {
        LOG( llevError, "quest_is_same_quest: slaying2 invalid.\n" );
        return 0;
        }

    start1 = strchr( slaying1, ' ' );
    start2 = strchr( slaying2, ' ' );
    end1 = strrchr( slaying1, ' ' );
    end2 = strrchr( slaying2, ' ' );
    if ( end1 - start1 != end2 - start2 )
        return 0;
    if ( strncmp( start1, start2, end1 - start1 ) )
        return 0;
    return 1;
    }

int quest_is_quest_marker( object* marker )
    {
    if ( marker->type != MARKER && marker->type != FORCE )
        return 0;
    if ( !marker->slaying || strncmp( marker->slaying, QUEST_MARKER, strlen( QUEST_MARKER ) ) )
        return 0;
    return 1;
    }

int quest_is_start( const char* slaying )
    {
    const char* str;
    if ( !slaying || ( strlen( slaying ) < ( strlen( QUEST_MARKER ) + strlen( QUEST_START ) ) ) )
        return 0;
    str = strrstr( slaying, QUEST_START );
    if ( !str || ( ( size_t )( str - slaying ) < strlen( slaying ) - strlen( QUEST_START ) ) )
        return 0;
    return 1;
    }

int quest_is_end( const char* slaying )
    {
    const char* str;
    if ( !slaying || ( strlen( slaying ) < ( strlen( QUEST_MARKER ) + strlen( QUEST_END ) ) ) )
        return 0;
    str = strrstr( slaying, QUEST_END );
    if ( !str || ( ( size_t )( str - slaying ) < strlen( slaying ) - strlen( QUEST_END ) ) )
        return 0;
    return 1;
    }

static int quest_has_start( object* marker, object* pl )
    {
    const char* start;
    const char* end;
    object* item;
    start = marker->slaying + strlen( QUEST_START );
    end = strrchr( start, ' ' );
    if ( !end )
        {
        LOG( llevError, "quest_has_start: invalid marker slaying %s\n", marker->slaying );
        return 0;
        }

    for ( item = pl->inv; item; item = item->below )
        {
        if ( item->type == FORCE
            && item->slaying
            && ( quest_is_quest_marker( item ) )
            && ( quest_is_same_quest( marker->slaying, item->slaying ) )
            && ( quest_is_start( item->slaying ) ) )
            return 1;
        }

    return 0;
    }

static int quest_has_end( object* marker, object* pl )
    {
    const char* start;
    const char* end;
    object* item;
    start = marker->slaying + strlen( QUEST_START );
    end = strrchr( start, ' ' );
    if ( !end )
        {
        LOG( llevError, "quest_has_end: invalid marker slaying %s\n", marker->slaying );
        return 0;
        }

    for ( item = pl->inv; item; item = item->below )
        {
        if ( item->type == FORCE
            && item->slaying
            && ( quest_is_quest_marker( item ) )
            && ( quest_is_same_quest( marker->slaying, item->slaying ) )
            && ( quest_is_end( item->slaying ) ) )
            return 1;
        }

    return 0;
    }

/**
 * Checks if the marker is a quest one, and if so if compatible with the player's current quests.
 **/
int quest_marker_compatible( object* marker, object* pl )
    {
    if ( !quest_is_quest_marker( marker ) )
        return 1;

    if ( quest_is_start( marker->slaying ) )
        {
        if ( quest_has_end( marker, pl ) )
            /* Can't restart a quest */
            return 0;
        if ( quest_has_start( marker, pl ) )
            /* Can't restart a quest already started */
            return 0;
        return 1;
        }
    else if ( quest_is_end( marker->slaying ) )
        {
        if ( !quest_has_start( marker, pl ) )
            return 0;
        if ( quest_has_end( marker, pl ) )
            return 0;
        return 1;
        }

    /* Neither a start or end, seek a start */
    if ( quest_has_start( marker, pl ) )
        return 1;

    /* No start, or an end => can't */
    return 0;
    }

/**
 * Clears other quest markers if required.
 **/
void quest_clear_markers( object* marker, object* pl )
    {
    object *item;
    object *next;
    if ( !quest_is_end( marker->slaying ) )
        return;

    next = NULL;
    for ( item = pl->inv; item; item = next )
        {
        next = item->below;
        if ( quest_is_quest_marker( item ) && quest_is_same_quest( marker->slaying, item->slaying ) )
            remove_ob( item );
        }
    }

/**
 * Checks message against quest marker. Returns:
 *  * message if message not linked to quest
 *  * NULL if message linked to quest and player isn't at right step.
 *  * first char of real message, line after the 'quest xxx xxx' line.
 **/
const char* quest_message_check( const char* message, object* pl )
    {
    const char *end1, *end2, *nl;
    char buf[ MAX_BUF ];
    object *item;
    if ( !message || strncmp( message, QUEST_MARKER, strlen( QUEST_MARKER ) ) )
        return message;

    if ( nl = strchr( message, '\n' ) )
        {
        strncpy( buf, message, MIN( nl - message, MAX_BUF ) );
        buf[ MIN( nl - message, MAX_BUF ) ] = '\0';
        nl++;
        }
    else
        {
        strncpy( buf, message, MAX_BUF );
        buf[ strlen( message ) ] = '\0';
        nl = "";
        }

    for ( item = pl->inv; item; item = item->below )
        {
        if ( quest_is_quest_marker( item )
            && quest_is_same_quest( item->slaying, buf ) )
            {
            end1 = strrchr( item->slaying, ' ' );
            end2 = strrchr( buf, ' ' );
            if ( !strcmp( end1, end2 ) )
                return nl;
            }
        }
    return NULL;
    }

const char* quest_get_name( object* marker )
    {
    static char buf[ 2 ][ MAX_BUF ];
    static int index_buf = 0;
    const char *start, *end;

    start = strchr( marker->slaying, ' ' );
    end = strrchr( marker->slaying, ' ' );

    index_buf = 1 - index_buf;

    strncpy( buf[ index_buf ], start + 1, end - start - 1 );

    return buf[ index_buf ];
    }

/*
 * static char *rcsid_quest_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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
 Quests are stored in specific items of type QUEST.
*/

#include <global.h>
#include <commands.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <ctype.h>
#include "quest.h"

#define QUEST_NAME( op ) op->name_pl
#define TASK_NAME( op ) op->custom_name

#if 0
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
#endif

int quest_is_quest_marker( const object* marker, int task )
    {
    if ( marker->type != QUEST )
        return 0;
    if ( task && !TASK_NAME(marker) )
        return 0;
    if ( !task && TASK_NAME(marker) )
        return 0;
    return 1;
    }

int quest_is_in_progress( const object* marker, int task )
    {
    if ( marker->subtype != QUEST_IN_PROGRESS )
        return 0;
    return 1;
    }

int quest_is_completed( const object* marker, int task )
    {
    if ( marker->type != QUEST )
        return 0;
    if ( task && ( !marker->subtype != QUEST_DONE_TASK ) )
        return 0;
    if ( !task && ( marker->subtype != QUEST_DONE_QUEST ) )
        return 0;
    return 1;
    }

#if 0
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
#endif

const char* quest_get_name( const object* marker )
    {
    if ( marker->type != QUEST )
        return NULL;
    if ( TASK_NAME(marker) )
        return TASK_NAME(marker);
    return QUEST_NAME(marker);
    }

object* quest_get_player_quest( const object* pl, const char* name, const char* name_pl )
{
    object* quest;
    for ( quest = pl->inv; quest; quest = quest->below )
        if ( quest->type == QUEST &&
            QUEST_NAME(quest) == name && TASK_NAME(quest) == name_pl )
            return quest;

    return NULL;
}

object* quest_get_override( const object* ob, const object* pl )
{
    object *in_ob, *in_pl;
    if ( !ob->inv )
        return NULL;
    for ( in_ob = ob->inv; in_ob; in_ob = in_ob->below )
    {
        if ( in_ob->type == QUEST && in_ob->subtype == QUEST_OVERRIDE )
        {
            in_pl = quest_get_player_quest( pl, QUEST_NAME(in_ob), TASK_NAME(in_ob) );
            if ( in_pl )
                return in_ob;
        }
    }
    return NULL;
}

const char* quest_get_override_slaying( const object* ob, const object* pl )
{
    object* quest;
    quest = quest_get_override( ob, pl );
    if ( quest )
        return quest->slaying;
    return ob->slaying;
}

const char* quest_get_override_msg( const object* ob, const object* pl )
{
    object* quest;
    quest = quest_get_override( ob, pl );
    if ( quest )
        return quest->msg;
    return ob->msg;
}
/*
static int quest_objects_match( object* first, object* second )
{
    if ( !first || !second )
        return 0;
    if ( first->name != second->name )
        return 0;
    if ( first->name_pl && first->name_pl != second->name_pl )
        return 0;
    return 1;
}
*/
static void quest_remove_marker(object* item, const char* name, const char* name_pl, int type)
{
    object* test;
    for ( test = item->inv; test; test = test->below )
    {
        if ( ( test->type == QUEST ) && ( test->subtype == type ) && ( QUEST_NAME(test) == name ) && ( !name_pl || TASK_NAME(test) == name_pl ) )
        {
            remove_ob(test);
            return;
        }
    }
}

void quest_apply_items( object* wrapper, player* pl )
{
    object* item;
    object* qm;
    for ( item = wrapper->inv; item; item = item->below )
    {
        if ( item->type == QUEST )
        {
            switch ( item->subtype )
            {
            case QUEST_START_QUEST:
                if ( quest_get_player_quest( pl->ob, QUEST_NAME(item), NULL ) )
                    return;
                qm = create_archetype("quest_in_progress");
                FREE_AND_COPY(QUEST_NAME(qm), QUEST_NAME(item));
                if ( item->lore )
                {
                    FREE_AND_COPY(qm->lore, item->lore);
		    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, item->lore, NULL);
                }
                insert_ob_in_ob(qm, pl->ob);
                break;
            case QUEST_END_QUEST:
                if ( !quest_get_player_quest( pl->ob, QUEST_NAME(item), NULL ) )
                    return;
                qm = create_archetype("quest_done_quest");
                FREE_AND_COPY(QUEST_NAME(qm), QUEST_NAME(item));
                if ( item->lore )
                {
                    FREE_AND_COPY(qm->lore, item->lore);
		    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, item->lore, NULL);
                }
                quest_remove_marker(pl->ob, QUEST_NAME(item), NULL, QUEST_IN_PROGRESS);
                insert_ob_in_ob(qm, pl->ob);
                break;
            case QUEST_START_TASK:
                if ( quest_get_player_quest( pl->ob, QUEST_NAME(item), TASK_NAME(item) ) )
                    return;
                qm = create_archetype("quest_in_progress");
                FREE_AND_COPY(QUEST_NAME(qm), QUEST_NAME(item));
                FREE_AND_COPY(TASK_NAME(qm), TASK_NAME(item));
                if ( item->lore )
                {
                    FREE_AND_COPY(qm->lore, item->lore);
		    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, item->lore, NULL);
                }
                insert_ob_in_ob(qm, pl->ob);
                break;
            case QUEST_END_TASK:
                if ( !quest_get_player_quest( pl->ob, QUEST_NAME(item), TASK_NAME(item) ) )
                    return;
                qm = create_archetype("quest_done_task");
                FREE_AND_COPY(QUEST_NAME(qm), QUEST_NAME(item));
                FREE_AND_COPY(TASK_NAME(qm), TASK_NAME(item));
                if ( item->lore )
                {
                    FREE_AND_COPY(qm->lore, item->lore);
		    draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_QUESTS, item->lore, NULL);
                }
                quest_remove_marker(pl->ob, QUEST_NAME(item), TASK_NAME(item), QUEST_IN_PROGRESS);
                insert_ob_in_ob(qm, pl->ob);
                break;
            }
        }
    }
}

/**
 * Returns 0 if no matching quest, 1 if anything matched.
 **/
int quest_on_activate( object* ob, player*pl )
{
    object* in_item;

    for ( in_item = ob->inv; in_item; in_item = in_item->below )
    {
        if ( ( in_item->type == QUEST ) && ( in_item->subtype == QUEST_ON_ACTIVATE ) )
        {
            quest_apply_items( in_item, pl );
            return 1;
        }
    }
    return 0;
}

/**
 * NS :not started
 * S  :started
 * D  :done
 * SD :started or done
 * Q  :quest
 * T  :task
 **/
#define QCT_QNS     1
#define QCT_QS      2
#define QCT_QS_TNS  3
#define QCT_QS_TS   4
#define QCT_QS_TD   5
#define QCT_QS_TSD  6
#define QCT_QSD     7
#define QCT_QD      8

int quest_is_override_compatible(const object *marker, const object* pl)
{
    object* test;
    if ( marker->type != QUEST || marker->subtype != QUEST_OVERRIDE )
        return 0;
    switch ( marker->stats.hp )
    {
        case QCT_QNS:
            if ( quest_get_player_quest( pl, QUEST_NAME(marker), NULL ) == NULL )
                return 1;
            return 0;
        case QCT_QS:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), NULL );
            if ( !test )
                return 0;
            if ( quest_is_completed( test, 0 ) )
                return 0;
            return 1;
        case QCT_QS_TNS:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), TASK_NAME(marker) );
            if ( !test )
                if ( quest_get_player_quest( pl, QUEST_NAME(marker), NULL ) )
                    return 1;
            return 0;
        case QCT_QS_TS:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), TASK_NAME(marker) );
            if ( !test )
                return 0;
            if ( quest_is_completed( test, 1 ) )
                return 0;
            return 1;
        case QCT_QS_TD:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), TASK_NAME(marker) );
            if ( !test )
                return 0;
            if ( quest_is_completed( test, 1 ) )
                return 1;
            return 0;
        case QCT_QS_TSD:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), TASK_NAME(marker) );
            if ( !test )
                return 0;
            return 1;
        case QCT_QSD:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), NULL );
            if ( !test )
                return 0;
            return 1;
        case QCT_QD:
            test = quest_get_player_quest( pl, QUEST_NAME(marker), NULL );
            if ( !test || !quest_is_completed( test, 0 ) )
                return 0;
            return 1;
    };
    return 1;
}

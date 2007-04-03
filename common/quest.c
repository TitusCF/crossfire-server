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

/**
 * @file quest.c
 * This file handles quest-related functions.
 * Quests are stored in specific items of type QUEST.
 *
 * @todo
 * Quest stuff should be totally rewritten :)
*/

#include <global.h>
#include <commands.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <ctype.h>
#include "quest.h"

/** Quest name. */
#define QUEST_NAME( op ) op->name_pl
/** Task (aka subquest) name. */
#define TASK_NAME( op ) op->custom_name

/**
 * Checks if specified item is a quest marker for the task.
 *
 * @param marker
 * object to check.
 * @param task
 * if set, marker should be a subtask, else a quest itself.
 * @return
 * 1 if it's a quest marker, 0 else.
 */
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

/**
 * Checks if object is a marker for a quest in progress.
 * @param marker
 * object to check
 * @param task
 * (ignored)
 * @return
 * 1 if marker is in progress, 0 else.
 */
int quest_is_in_progress( const object* marker, int task )
    {
    if ( marker->subtype != QUEST_IN_PROGRESS )
        return 0;
    return 1;
    }

/**
 * Checks if object is a marker for a completed quest.
 * @param marker
 * object to check
 * @param task
 * if set, marker must be for a task, else for a (main) task.
 * @return
 * 1 if marker is completed, 0 else.
 */
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

/**
 * Returns the name of the quest.
 * @param marker
 * quest we're searching the name of.
 * @return
 * name of task if marker is a task, else quest name. NULL if marker isn't a quest object.
 */
const char* quest_get_name( const object* marker )
    {
    if ( marker->type != QUEST )
        return NULL;
    if ( TASK_NAME(marker) )
        return TASK_NAME(marker);
    return QUEST_NAME(marker);
    }

/**
 * Searches the player's inventory for a quest item.
 *
 * @param pl
 * player to search into.
 * @param name
 * name of the quest to search. Must be NULL or a shared string.
 * @param name_pl
 * name of the task to search. Must be NULL or a shared string.
 * @return
 * matching quest object, NULL if no match found.
 */
object* quest_get_player_quest( const object* pl, const char* name, const char* name_pl )
{
    object* quest;
    for ( quest = pl->inv; quest; quest = quest->below )
        if ( quest->type == QUEST &&
            QUEST_NAME(quest) == name && TASK_NAME(quest) == name_pl )
            return quest;

    return NULL;
}

/**
 * Finds an object in ob that can override behaviour of its owner, for a specific player.
 *
 * @param ob
 * object we're searching into.
 * @param pl
 * player that we're considering.
 * @return
 * overriding object, NULL if none found.
 */
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

/**
 * Finds a slaying field specific to player for a quest.
 * @param ob
 * object we're considering.
 * @param pl
 * player we're considering.
 * @return
 * slaying to use for that player and object.
 */
const char* quest_get_override_slaying( const object* ob, const object* pl )
{
    object* quest;
    quest = quest_get_override( ob, pl );
    if ( quest )
        return quest->slaying;
    return ob->slaying;
}

/**
 * Finds a message field specific to player for a quest.
 * @param ob
 * object we're considering.
 * @param pl
 * player we're considering.
 * @return
 * message to use for that player and object.
 */
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

/**
 * Removes a quest marker from item.
 *
 * @param item
 * item we're altering.
 * @param name
 * name of the quest. Must be a shared string.
 * @param name_pl
 * name of the task. Can be NULL. Must be a shared string.
 * @param type
 * a subtype of the QUEST item type.
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

/**
 * Player applies an object that can contain quest information. Check what to do.
 *
 * @param wrapper
 * object being applied.
 * @param pl
 * player applying the object.
 */
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
 * Player is applying an object, check if special quest behaviour should be done.
 *
 * @param ob
 * object being applied.
 * @param pl
 * player applying the object.
 * @return
 * 0 if no matching quest, 1 if anything matched.
 */
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
/** Quest not started. */
#define QCT_QNS     1
/** Quest started. */
#define QCT_QS      2
/** Quest started, task not started. */
#define QCT_QS_TNS  3
/** Quest started, task started. */
#define QCT_QS_TS   4
/** Quest started, task done. */
#define QCT_QS_TD   5
/** Quest started, task started or done. */
#define QCT_QS_TSD  6
/** Quest started or done. */
#define QCT_QSD     7
/** Quest done. */
#define QCT_QD      8

/**
 * Checks if the marker can be applied to the player.
 * @param marker
 * marker we're trying to apply.
 * @param pl
 * player we're applying to.
 * @return
 * 1 if marker can be applied, 0 else.
 */
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

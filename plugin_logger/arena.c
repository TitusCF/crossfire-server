/*****************************************************************************/
/* The crossfire logging facility.                                           */
/* This plugin is part of a projects who's purpose is to track every         */
/* interesting thing happening in the crossfire server, from conversation    */
/* between player and their deaths to informations about server crashes.     */
/* The datas could then be loaded by php scripts to present info on a player,*/
/* important events of the day, current experience of all players,           */
/* crash-o-meter and so on.                                                  */
/*****************************************************************************/
/* This code is placed under the GNU General Public Licence (GPL) either     */
/* version 2 or higher, at your choice. You can get a copy of the GPL at the */
/* following address: http://www.gnu.org                                     */
/*                                                                           */
/* (C)2001 Tchize. Feel free to report any problem.                          */
/* tchize@mailandnews.com                                                    */
/*****************************************************************************/

#include <plugin_logging.h>
#ifndef __CEXTRACT__
#include <logger_proto.h>
#endif

#include <logger_arena.h>


/*****************************************************************************/
/* Handles the "player comments" in the Arena stuff.                         */
/* Returns 1 on success, -1 on failure.                                      */
/*****************************************************************************/
int arena_player_comment (object* talker, char* saywhat, int playernumber)
{
    printf ("%s -> %s (%d)\n",talker->name,saywhat,playernumber);
    if ((playernumber>1)|| !saywhat)
        return -1;
    if (strcmp(last_arena.players[playernumber],talker->name))
        return -1; /*Bad comment*/
    if ((strlen(saywhat)+strlen(last_arena.comment[playernumber])+3) > COMMENT_LENGTH)
        return -1; /*No space left*/
    printf ("player %d said:\n %s\n-------\n",playernumber,saywhat);
    strcat (last_arena.comment[playernumber],"\n");
    strcat (last_arena.comment[playernumber],saywhat);
    return 1;/*Return 1 to stop sending the say to other npcs!*/
}

/*****************************************************************************/
/* Handles the "looser comments" in the Arena stuff.                         */
/* Basically, just a wrapper around arena_player_comment.                    */
/* Return value: 1 on success, 0 on failure.                                 */
/*****************************************************************************/
int arena_looser_comment (object* talker, char* saywhat)
{
    if (QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_RESULT))
        return arena_player_comment (talker,saywhat,1-last_arena.result);
    else
        return -1; /*No arena ready*/
}

/*****************************************************************************/
/* Handles the "winner comments" in the Arena stuff.                         */
/* Basically, just a wrapper around arena_player_comment.                    */
/* Return value: 1 on success, 0 on failure.                                 */
/*****************************************************************************/
int arena_winner_comment (object* talker, char* saywhat)
{
    if (QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_RESULT))
        return arena_player_comment (talker,saywhat,last_arena.result);
    else
        return -1; /*No arena ready*/
}

/*****************************************************************************/
/* This one may later be used to force a player to comment!                  */
/*****************************************************************************/
int arena_looser_leave(object* who)
{
    if (!strcmp(last_arena.players[1-last_arena.result],who->name))
        return 0;
    else
        last_arena.flags |= (last_arena.result>0)?
            FLAG_ARENA_HAS_COMMENT1:FLAG_ARENA_HAS_COMMENT2;
    arena_fix_last();
    return 0;
}

/*****************************************************************************/
/* This one may later be used to force a player to comment!                  */
/*****************************************************************************/
int arena_winner_leave(object* who)
{
    if (!strcmp(last_arena.players[last_arena.result],who->name))
        return 0;
    else
        last_arena.flags |= (last_arena.result>0)?
            FLAG_ARENA_HAS_COMMENT2:FLAG_ARENA_HAS_COMMENT1;
    arena_fix_last();
    return 0;
}

/*****************************************************************************/
/* Defines the time stamp of the current arena.                              */
/*****************************************************************************/
void set_arena_time (void)
{
    gettimestamp(current_arena.timestamp,sizeof (current_arena.timestamp),
        &current_arena.counter);
}

/*****************************************************************************/
/* Saves the last Arena, and then invalidate it.                             */
/*****************************************************************************/
void arena_fix_last (void)
{
    if (QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_ALL))
    {
        save_last_arena();
        last_arena.flags = FLAG_ARENA_IS_INVALID;
        /* Invalidate arena to be sure it is saved only once!  */
    }
}

/*****************************************************************************/
/* Saves the last Arena datas.                                               */
/*****************************************************************************/
void save_last_arena(void)
{
    char query[HUGE_BUF];
    if (!QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_RESULT)) return;

    printf ("Last arena was %s vs %s\n",last_arena.players[0],
        last_arena.players[1]);
    printf ("%s won it\n",(last_arena.result==0)?
        last_arena.players[0]:last_arena.players[1]);
    printf ("%s said: \n%s\n",last_arena.players[0],last_arena.comment[0]);
    printf ("%s said: \n%s\n",last_arena.players[1],last_arena.comment[1]);
    sprintf (query,"insert into arena (winner,looser,winner_comment,\
        looser_comment,moment,counter,screenshoot) VALUES ('%s', '%s', '%s',\
         '%s', '%s', %d, '%s')",
        last_arena.players[last_arena.result],
        last_arena.players[1-last_arena.result],
        last_arena.comment[last_arena.result],
        last_arena.comment[1-last_arena.result],
        last_arena.timestamp,
        last_arena.counter,
        last_arena.screenshoot);
    database_insert (query);
    return;
}

void flush_arena (void)
{
    if (!QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_IS_INVALID)) save_last_arena ();
    memcpy (&last_arena,&current_arena,sizeof (arena_data));
    memcpy (&current_arena,&empty_arena,sizeof (arena_data));
}


int set_arena_player (object* pl, int playernumber)
{
    if (playernumber>1) return -1;
    if (current_arena.result!=-1)
    {
        flush_arena(); /*New arena*/
        set_arena_time();
    }
    printf ("Player %d is %s\n",playernumber,pl->name);
    strcpy (current_arena.players[playernumber],pl->name);
    current_arena.flags |= (playernumber==0)?
        FLAG_ARENA_HAS_PLAYER1:FLAG_ARENA_HAS_PLAYER2;
    return 0;
}

void set_looser (object* looser)
{
    player* pl;
    object* op;
    CFParm* CFPptr;
    CFParm CFP;
    char* screenshoot;
    printf ("Looser is %s\n",looser->name);

    if (!strcmp(looser->name,current_arena.players[0]))
    {
        current_arena.flags |= FLAG_ARENA_HAS_RESULT;
        current_arena.result = 1;
    }
    else if (!strcmp(looser->name,current_arena.players[1]))
    {
        current_arena.flags |= FLAG_ARENA_HAS_RESULT;
        current_arena.result = 0;
    }
    else
        current_arena.flags=FLAG_ARENA_IS_INVALID;

    set_arena_time();
    CFP.Value[0] = (void*)&current_arena.players[current_arena.result];
    CFPptr = (PlugHooks[HOOK_FINDPLAYER])(&CFP);
    pl = (player*)CFPptr->Value[0];
    free(CFPptr);
    op = pl->ob;
    screenshoot = takescreenshoot (op);
    if (screenshoot) strcpy (current_arena.screenshoot,screenshoot);
    flush_arena();
}

int trigger_arena (object* trigger, object* above, char* options)
/*When we get player infos, we suppose the arenas already got flushed*/
{
    if (!strcmp(options,"SET_PLAYER1"))
        set_arena_player (above,0);
    else if (!strcmp(options,"SET_PLAYER2"))
        set_arena_player (above,1);
    else if (!strcmp(options,"AM_LOOSER"))
        set_looser (above);
    return 0; /*To be sure teleporters still work!*/
}

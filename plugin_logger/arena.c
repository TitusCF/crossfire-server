/*
 * The crossfire logging facility.
 * This plugin is part of a project who's purpose is to track every
 * interesting thing happening in the crossfire server, from conversation
 * between player and their deaths to informations about server crashes.
 * The datas could then be loaded by some php pages to present info on a player,
 * important events of the day, current Xp of all players,
 * crash-o-meter and so on.
 */

/*
 * This code is placed under the GNU General Public Licence (GPL)
 * either version 2 or higher, at your choice. You can get a copy
 * of the GPL at the following address: http://www.gnu.org
 *
 * (C)2001 Tchize. Feel free to log errors.
 * tchize@mailandnews.com
 */

#include <plugin_logging.h>
#ifndef __CEXTRACT__
#include <logger_proto.h>
#endif

#define FLAG_ARENA_IS_INVALID   0x00000000
#define FLAG_ARENA_HAS_PLAYER1  0x00000001
#define FLAG_ARENA_HAS_PLAYER2  0x00000002
/*Not all used yet*/
#define FLAG_ARENA_HAS_COMMENT1 0x00000004
#define FLAG_ARENA_HAS_COMMENT2 0x00000008
#define FLAG_ARENA_HAS_RESULT   0x00000010
#define FLAG_ARENA_HAS_ALL      0x0000001F

#define QUERY_ARENA_FLAG(arena_name,flag) (arena_name.flags & flag)
#define COMMENT_LENGTH 2048

typedef struct {
    char players[2][MAX_BUF];
    int result;
           /*0: player0 won. 1: player1 won. -1: not valid */
    char comment[2][COMMENT_LENGTH];
           /*0: winner comment. 1: looser comment*/
    char timestamp[MAX_BUF];
    int counter;
    char screenshoot[MAX_BUF];
    int flags;
           /*screenshoots for player 0 and 1 at fatal moment*/
    } arena_data;
#define EMPTY_ARENA_DATAS { {"Player1","Player2"},\
                          -1,                     \
                          {"",""},                \
                          "",-1,                  \
                          "",                \
                          FLAG_ARENA_IS_INVALID}
arena_data empty_arena = EMPTY_ARENA_DATAS;
arena_data last_arena=EMPTY_ARENA_DATAS, current_arena=EMPTY_ARENA_DATAS;

int arena_player_comment (object* talker, char* saywhat, int playernumber)
{
    printf ("%s -> %s (%d)\n",talker->name,saywhat,playernumber);
    if ((playernumber>1)|| !saywhat) return -1;
    if (strcmp(last_arena.players[playernumber],talker->name)) return -1; /*Bad commenter*/
    if ((strlen (saywhat)+strlen(last_arena.comment[playernumber])+3) >COMMENT_LENGTH) return -1; /*No space left*/
    printf ("player %d said:\n %s\n-------\n",playernumber,saywhat);
    strcat (last_arena.comment[playernumber],"\n");
    strcat (last_arena.comment[playernumber],saywhat);
    return 1;/*Return 1 to stop sending the say to other npcs!*/
}

int arena_looser_comment (object* talker, char* saywhat)
{
    if (QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_RESULT))
        return arena_player_comment (talker,saywhat,1-last_arena.result);
    else return -1; /*No arena ready*/
}

int arena_winner_comment (object* talker, char* saywhat)
{
    if (QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_RESULT))
        return arena_player_comment (talker,saywhat,last_arena.result);
    else{printf ("error\n"); return -1;}; /*No arena ready*/
}
/*Later may be used to force a player to comment!*/
int arena_looser_leave(object* who)
{
    if (!strcmp(last_arena.players[1-last_arena.result],who->name)) return 0;
    else last_arena.flags|=(last_arena.result>0)?FLAG_ARENA_HAS_COMMENT1:FLAG_ARENA_HAS_COMMENT2;
    arena_fix_last();
    return 0;
}

/*Later may be used to force a player to comment!*/
int arena_winner_leave(object* who)
{
    if (!strcmp(last_arena.players[last_arena.result],who->name)) return 0;
    else last_arena.flags|=(last_arena.result>0)?FLAG_ARENA_HAS_COMMENT2:FLAG_ARENA_HAS_COMMENT1;
    arena_fix_last();
    return 0;
}

void set_arena_time (void)
{
    gettimestamp (current_arena.timestamp,sizeof (current_arena.timestamp),&current_arena.counter);
}

void arena_fix_last (void)
{
    if (QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_ALL)){
        save_last_arena();
        last_arena.flags=FLAG_ARENA_IS_INVALID;
        /* Invalidate arena to be sure it is saved only once!  */
        }
}

void save_last_arena(void)
{
    char query[HUGE_BUF];
    if (!QUERY_ARENA_FLAG(last_arena,FLAG_ARENA_HAS_RESULT)) return;
    printf ("Last arena was %s vs %s\n",last_arena.players[0],last_arena.players[1]);
    printf ("%s won it\n",(last_arena.result==0)?last_arena.players[0]:last_arena.players[1]);
    printf ("%s said: \n%s\n",last_arena.players[0],last_arena.comment[0]);
    printf ("%s said: \n%s\n",last_arena.players[1],last_arena.comment[1]);
    sprintf (query,"insert into arena (winner,looser,winner_comment,looser_comment,moment,counter,screenshoot)\
                    VALUES ('%s', '%s', '%s', '%s', '%s', %d, '%s')",
                    last_arena.players[last_arena.result],
                    last_arena.players[1-last_arena.result],
                    addslashes(last_arena.comment[last_arena.result]),
                    addslashes(last_arena.comment[1-last_arena.result]),
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
    current_arena.flags|=(playernumber==0)?FLAG_ARENA_HAS_PLAYER1:FLAG_ARENA_HAS_PLAYER2;
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
    if (!strcmp(looser->name,current_arena.players[0])){
        current_arena.flags|=FLAG_ARENA_HAS_RESULT;
        current_arena.result=1;
    }else if (!strcmp(looser->name,current_arena.players[1])){
        current_arena.flags|=FLAG_ARENA_HAS_RESULT;
        current_arena.result=0;
    }else current_arena.flags=FLAG_ARENA_IS_INVALID;
    set_arena_time();
    CFP.Value[0]=(void*)&current_arena.players[current_arena.result];
    CFPptr=(PlugHooks[HOOK_FINDPLAYER])(&CFP);
    pl=(player*)CFPptr->Value[0];
    free (CFPptr);
    op=pl->ob;
    screenshoot=takescreenshoot (op);
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

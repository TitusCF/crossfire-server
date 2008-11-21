/*
 * static char *rcsid_request_c =
 *    "$Id$";
 */

/*
  CrossFire, A Multiplayer game for X-windows

  Copyright (C) 2001-2006 Mark Wedel
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

  The author can be reached via e-mail to crossfire-devel@real-time.com
*/

/**
 * \file
 * Client handling.
 *
 * \date 2003-12-02
 *
 * This file implements all of the goo on the server side for handling
 * clients.  It's got a bunch of global variables for keeping track of
 * each of the clients.
 *
 * Note:  All functions that are used to process data from the client
 * have the prototype of (char *data, int datalen, int client_num).  This
 * way, we can use one dispatch table.
 *
 * esrv_map_new starts updating the map
 *
 * esrv_map_setbelow allows filling in all of the faces for the map.
 * if a face has not already been sent to the client, it is sent now.
 *
 * compactstack, perform the map compressing
 * operations
 *
 * esrv_map_scroll tells the client to scroll the map, and does similarily
 * for the locally cached copy.
 *
 * @todo
 * smoothing should be automatic for latest clients. Remove some stuff we can assume is always on.
 * fix comments for this file.
 */

#include <global.h>
#include <sproto.h>

#include <newclient.h>
#include <newserver.h>
#include <living.h>
#include <commands.h>

/* This block is basically taken from socket.c - I assume if it works there,
 * it should work here.
 */
#ifndef WIN32 /* ---win32 exclude unix headers */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#endif /* win32 */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "sounds.h"

/**
 * This table translates the attack numbers as used within the
 * program to the value we use when sending STATS command to the
 * client.  If a value is -1, then we don't send that to the
 * client.
 */
static const short atnr_cs_stat[NROFATTACKS] = {CS_STAT_RES_PHYS, CS_STAT_RES_MAG,
                                                CS_STAT_RES_FIRE, CS_STAT_RES_ELEC,
                                                CS_STAT_RES_COLD, CS_STAT_RES_CONF,
                                                CS_STAT_RES_ACID,
                                                CS_STAT_RES_DRAIN, -1 /* weaponmagic */,
                                                CS_STAT_RES_GHOSTHIT, CS_STAT_RES_POISON,
                                                CS_STAT_RES_SLOW, CS_STAT_RES_PARA,
                                                CS_STAT_TURN_UNDEAD,
                                                CS_STAT_RES_FEAR, -1 /* Cancellation */,
                                                CS_STAT_RES_DEPLETE, CS_STAT_RES_DEATH,
                                                -1 /* Chaos */, -1 /* Counterspell */,
                                                -1 /* Godpower */, CS_STAT_RES_HOLYWORD,
                                                CS_STAT_RES_BLIND,
                                                -1, /* Internal */
                                                -1, /* life stealing */
                                                -1 /* Disease - not fully done yet */
};

/** This is the Setup cmd - easy first implementation */
void set_up_cmd(char *buf, int len, socket_struct *ns) {
    int s;
    char *cmd, *param;
    SockList sl;

    /* run through the cmds of setup
     * syntax is setup <cmdname1> <parameter> <cmdname2> <parameter> ...
     *
     * we send the status of the cmd back, or a FALSE is the cmd
     * is the server unknown
     * The client then must sort this out
     */

    LOG(llevInfo,"Get SetupCmd:: %s\n", buf);
    SockList_Init(&sl);
    SockList_AddString(&sl, "setup");
    for (s=0;s<len;) {

        cmd = &buf[s];

        /* find the next space, and put a null there */
        for (;buf[s] && buf[s] != ' ';s++) ;
        if (s>=len)
            break;
        buf[s++]=0;
        while (buf[s] == ' ') s++;

        if (s>=len)
            break;

        param = &buf[s];

        for (;buf[s] && buf[s] != ' ';s++) ;
        buf[s++]=0;
        while (s<len && buf[s] == ' ') s++;

        SockList_AddPrintf(&sl, " %s ", cmd);

        if (!strcmp(cmd,"sound")) {
            /* this is the old sound command, which means the client doesn't understand our sound => mute. */
            ns->sound = 0;
            SockList_AddString(&sl, "FALSE");
        } else if (!strcmp(cmd,"sound2")) {
            ns->sound = atoi(param)&(SND_EFFECTS|SND_MUSIC|SND_MUTE);
            SockList_AddString(&sl, param);
        } else if (!strcmp(cmd,"exp64")) {
            /* for compatibility, return 1 since older clients can be confused else. */
            SockList_AddString(&sl, "1");
        } else if (!strcmp(cmd, "spellmon")) {
            int monitor_spells;

            monitor_spells = atoi(param);
            if (monitor_spells != 0 && monitor_spells != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->monitor_spells = monitor_spells;
                SockList_AddPrintf(&sl, "%d", monitor_spells);
            }
        } else if (!strcmp(cmd,"darkness")) {
            int darkness;

            darkness = atoi(param);
            if (darkness != 0 && darkness != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->darkness = darkness;
                SockList_AddPrintf(&sl, "%d", darkness);
            }
        } else if (!strcmp(cmd,"map2cmd")) {
            int map2cmd;

            map2cmd = atoi(param);
            if (map2cmd != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->mapmode = Map2Cmd;
                SockList_AddString(&sl, "1");
            }
        } else if (!strcmp(cmd,"newmapcmd")) {
            int newmapcmd;

            newmapcmd = atoi(param);
            if (newmapcmd != 0 && newmapcmd != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->newmapcmd = newmapcmd;
                SockList_AddPrintf(&sl, "%d", newmapcmd);
            }
        } else if (!strcmp(cmd,"facecache")) {
            int facecache;

            facecache = atoi(param);
            if (facecache != 0 && facecache != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->facecache = facecache;
                SockList_AddPrintf(&sl, "%d", facecache);
            }
        } else if (!strcmp(cmd,"faceset")) {
            int q = atoi(param);

            if (is_valid_faceset(q))
                ns->faceset=q;
            SockList_AddPrintf(&sl, "%d", ns->faceset);
        } else if (!strcmp(cmd,"itemcmd")) {
            /* client ignore the value anyway. */
            SockList_AddString(&sl, "2");
        } else if (!strcmp(cmd,"mapsize")) {
            int x, y, n;

            if (sscanf(param, "%dx%d%n", &x, &y, &n) != 2 || n != (int)strlen(param)) {
                x = 0;
                y = 0;
            }
            if (x < 9 || y < 9 || x > MAP_CLIENT_X || y > MAP_CLIENT_Y) {
                SockList_AddPrintf(&sl, "%dx%d", MAP_CLIENT_X, MAP_CLIENT_Y);
            } else {
                ns->mapx = x;
                ns->mapy = y;
                /* better to send back what we are really using and not the
                 * param as given to us in case it gets parsed differently.
                 */
                SockList_AddPrintf(&sl, "%dx%d", x, y);
                /* If beyond this size and still using orig map
                 * command, need to go to map1cmd.
                 */
            }
        } else if (!strcmp(cmd,"extendedMapInfos")) {
            SockList_AddString(&sl, "1");
        } else if (!strcmp(cmd,"extendedTextInfos")) {
            int has_readable_type;

            has_readable_type = atoi(param);
            if (has_readable_type != 0 && has_readable_type != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->has_readable_type = has_readable_type;
                SockList_AddPrintf(&sl, "%d", has_readable_type);
            }
        } else if (!strcmp(cmd,"tick")) {
            int tick;

            tick = atoi(param);
            if (tick != 0 && tick != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->tick = tick;
                SockList_AddPrintf(&sl, "%d", tick);
            }
        } else if (!strcmp(cmd,"bot")) {
            int is_bot;

            is_bot = atoi(param);
            if (is_bot != 0 && is_bot != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->is_bot = is_bot;
                SockList_AddPrintf(&sl, "%d", is_bot);
            }
        } else if (!strcmp(cmd,"want_pickup")) {
            int want_pickup;

            want_pickup = atoi(param);
            if (want_pickup != 0 && want_pickup != 1) {
                SockList_AddString(&sl, "FALSE");
            } else {
                ns->want_pickup = want_pickup;
                SockList_AddPrintf(&sl, "%d", want_pickup);
            }
        } else if (!strcmp(cmd,"inscribe")) {
            SockList_AddString(&sl, "1");
        } else if (!strcmp(cmd,"num_look_objects")) {
            int tmp;

            tmp = atoi(param);
            if (tmp < MIN_NUM_LOOK_OBJECTS) {
                tmp = MIN_NUM_LOOK_OBJECTS;
            } else if (tmp > MAX_NUM_LOOK_OBJECTS) {
                tmp = MAX_NUM_LOOK_OBJECTS;
            }
            ns->num_look_objects = (uint8)tmp;
            SockList_AddPrintf(&sl, "%d", tmp);
        } else {
            /* Didn't get a setup command we understood -
             * report a failure to the client.
             */
            SockList_AddString(&sl, "FALSE");
        }
    } /* for processing all the setup commands */
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * The client has requested to be added to the game.
 * This is what takes care of it.  We tell the client how things worked out.
 * I am not sure if this file is the best place for this function.  however,
 * it either has to be here or init_sockets needs to be exported.
 *
 * @todo can ns->status not be Ns_Add?
 */
void add_me_cmd(char *buf, int len, socket_struct *ns) {
    Settings oldsettings;
    oldsettings=settings;
    if (ns->status != Ns_Add) {
        Write_String_To_Socket(ns, "addme_failed",12);
    } else {
        add_player(ns);
        /* Basically, the add_player copies the socket structure into
         * the player structure, so this one (which is from init_sockets)
         * is not needed anymore.  The write below should still work,
         * as the stuff in ns is still relevant.
         */
        Write_String_To_Socket(ns, "addme_success",13);
        if (ns->sc_version < 1027 || ns->cs_version < 1023) {
            /* The space in the link isn't correct, but in my
             * quick test with client 1.1.0, it didn't print it
             * out correctly when done as a single line.
             */
            const char *buf= "drawinfo 3 Warning: Your client is too old to receive map data.  Please update to a new client at http://sourceforge.net/project/showfiles.php ?group_id=13833";
            Write_String_To_Socket(ns, buf, strlen(buf));
        }

        socket_info.nconns--;
        ns->status = Ns_Avail;
    }
    settings=oldsettings;
}

/** Reply to ExtendedInfos command */
void toggle_extended_infos_cmd(char *buf, int len, socket_struct *ns) {
    char cmdback[MAX_BUF];
    char command[50];
    int info,nextinfo, smooth = 0;
    cmdback[0]='\0';
    nextinfo=0;
    while (1) {
        /* 1. Extract an info*/
        info=nextinfo;
        while ((info<len) && (buf[info]==' ')) info++;
        if (info>=len)
            break;
        nextinfo=info+1;
        while ((nextinfo<len) && (buf[nextinfo]!=' '))
            nextinfo++;
        if (nextinfo-info>=49) /*Erroneous info asked*/
            continue;
        strncpy(command,&(buf[info]),nextinfo-info);
        command[nextinfo-info]='\0';
        /* 2. Interpret info*/
        if (!strcmp("smooth",command)) {
            /* Toggle smoothing*/
            smooth = 1;
        } else {
            /*bad value*/
        }
        /*3. Next info*/
    }
    strcpy(cmdback,"ExtendedInfoSet");
    if (smooth) {
        strcat(cmdback," ");
        strcat(cmdback,"smoothing");
    }
    Write_String_To_Socket(ns, cmdback,strlen(cmdback));
}

/** Reply to ExtendedInfos command */
void toggle_extended_text_cmd(char *buf, int len, socket_struct *ns) {
    char cmdback[MAX_BUF];
    char temp[10];
    char command[50];
    int info,nextinfo,i,flag;
    cmdback[0]='\0';
    nextinfo=0;
    while (1) {
        /* 1. Extract an info*/
        info=nextinfo;
        while ((info<len) && (buf[info]==' ')) info++;
        if (info>=len)
            break;
        nextinfo=info+1;
        while ((nextinfo<len) && (buf[nextinfo]!=' '))
            nextinfo++;
        if (nextinfo-info>=49) /*Erroneous info asked*/
            continue;
        strncpy(command,&(buf[info]),nextinfo-info);
        command[nextinfo-info]='\0';
        /* 2. Interpret info*/
        i = sscanf(command,"%d",&flag);
        if ((i==1) && (flag>0) && (flag<=MSG_TYPE_LAST))
            ns->supported_readables|=(1<<flag);
        /*3. Next info*/
    }
    /* Send resulting state */
    strcpy(cmdback,"ExtendedTextSet");
    for (i=0;i<=MSG_TYPE_LAST;i++)
        if (ns->supported_readables &(1<<i)) {
            strcat(cmdback," ");
            snprintf(temp,sizeof(temp),"%d",i);
            strcat(cmdback,temp);
        }
    Write_String_To_Socket(ns, cmdback,strlen(cmdback));
}

/**
 * A lot like the old AskSmooth (in fact, now called by AskSmooth).
 * Basically, it makes no sense to wait for the client to request a
 * a piece of data from us that we know the client wants.  So
 * if we know the client wants it, might as well push it to the
 * client.
 */
static void send_smooth(socket_struct *ns, uint16 face) {
    uint16 smoothface;
    SockList sl;

    /* If we can't find a face, return and set it so we won't
     * try to send this again.
     */
    if ((!find_smooth(face, &smoothface)) &&
        (!find_smooth(smooth_face->number, &smoothface))) {

        LOG(llevError,"could not findsmooth for %d. Neither default (%s)\n",
            face,smooth_face->name);
        ns->faces_sent[face] |= NS_FACESENT_SMOOTH;
        return;
    }

    if (!(ns->faces_sent[smoothface] & NS_FACESENT_FACE))
        esrv_send_face(ns, smoothface, 0);

    ns->faces_sent[face] |= NS_FACESENT_SMOOTH;

    SockList_Init(&sl);
    SockList_AddString(&sl, "smooth ");
    SockList_AddShort(&sl, face);
    SockList_AddShort(&sl, smoothface);
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * Tells client the picture it has to use
 * to smooth a picture number given as argument.
 */
void ask_smooth_cmd(char *buf, int len, socket_struct *ns) {
    uint16 facenbr;

    facenbr=atoi(buf);
    send_smooth(ns, facenbr);
}

/**
 * This handles the commands issued by the player (ie, north, fire, cast,
 * etc.).  This is called with the 'ncom' method which gives more information back
 * to the client so it can throttle.
 *
 * @param buf
 * data received.
 * @param len
 * length of buf.
 * @param pl
 * player who issued the command. Mustn't be NULL.
 */
void new_player_cmd(uint8 *buf, int len, player *pl) {
    int time,repeat;
    short packet;
    char    command[MAX_BUF];
    SockList sl;

    if (len < 7) {
        LOG(llevDebug,"Corrupt ncom command - not long enough - discarding\n");
        return;
    }

    packet = GetShort_String(buf);
    repeat = GetInt_String(buf+2);
    /* -1 is special - no repeat, but don't update */
    if (repeat!=-1) {
        pl->count=repeat;
    }
    if ((len-4) >= MAX_BUF) len=MAX_BUF-5;

    strncpy(command, (char*)buf+6, len-4);
    command[len-4]='\0';

    /* The following should never happen with a proper or honest client.
     * Therefore, the error message doesn't have to be too clear - if
     * someone is playing with a hacked/non working client, this gives them
     * an idea of the problem, but they deserve what they get
     */
    if (pl->state!=ST_PLAYING) {
        draw_ext_info_format(NDI_UNIQUE, 0,pl->ob,
                             MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "You can not issue commands - state is not ST_PLAYING (%s)",
                             "You can not issue commands - state is not ST_PLAYING (%s)",
                             buf);
        return;
    }

    /* This should not happen anymore.    */
    if (pl->ob->speed_left<-1.0) {
        LOG(llevError,"Player has negative time - shouldn't do command.\n");
    }
    /* In c_new.c */
    execute_newserver_command(pl->ob, command);
    /* Perhaps something better should be done with a left over count.
     * Cleaning up the input should probably be done first - all actions
     * for the command that issued the count should be done before
     * any other commands.
     */
    pl->count=0;

    /* Send confirmation of command execution now */
    SockList_Init(&sl);
    SockList_AddString(&sl, "comc ");
    SockList_AddShort(&sl,packet);
    if (FABS(pl->ob->speed) < 0.001)
        time=MAX_TIME * 100;
    else
        time = (int)(MAX_TIME/ FABS(pl->ob->speed));
    SockList_AddInt(&sl,time);
    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}


/** This is a reply to a previous query. */
void reply_cmd(char *buf, int len, player *pl) {
    /* This is to synthesize how the data would be stored if it
     * was normally entered.  A bit of a hack, and should be cleaned up
     * once all the X11 code is removed from the server.
     *
     * We pass 13 to many of the functions because this way they
     * think it was the carriage return that was entered, and the
     * function then does not try to do additional input.
     */
    snprintf(pl->write_buf, sizeof(pl->write_buf), ":%s", buf);

    /* this avoids any hacking here */

    switch (pl->state) {
        case ST_PLAYING:
            LOG(llevError,"Got reply message with ST_PLAYING input state\n");
            break;

        case ST_PLAY_AGAIN:
            /* We can check this for return value (2==quit).  Maybe we
             * should, and do something appropriate?
             */
            receive_play_again(pl->ob, buf[0]);
            break;

        case ST_ROLL_STAT:
            key_roll_stat(pl->ob,buf[0]);
            break;

        case ST_CHANGE_CLASS:

            key_change_class(pl->ob, buf[0]);
            break;

        case ST_CONFIRM_QUIT:
            key_confirm_quit(pl->ob, buf[0]);
            break;

        case ST_GET_NAME:
            receive_player_name(pl->ob);
            break;

        case ST_GET_PASSWORD:
        case ST_CONFIRM_PASSWORD:
        case ST_CHANGE_PASSWORD_OLD:
        case ST_CHANGE_PASSWORD_NEW:
        case ST_CHANGE_PASSWORD_CONFIRM:
            receive_player_password(pl->ob);
            break;

        case ST_GET_PARTY_PASSWORD:        /* Get password for party */
            receive_party_password(pl->ob);
            break;

        default:
            LOG(llevError,"Unknown input state: %d\n", pl->state);
    }
}

/**
 * Client tells its version.  If there is a mismatch, we close the
 * socket.  In real life, all we should care about is the client having
 * something older than the server.  If we assume the client will be
 * backwards compatible, having it be a later version should not be a
 * problem.
 */
void version_cmd(char *buf, int len,socket_struct *ns) {
    char *cp;

    if (!buf) {
        LOG(llevError, "CS: received corrupted version command\n");
        return;
    }

    ns->cs_version = atoi(buf);
    ns->sc_version =  ns->cs_version;
    if (VERSION_CS !=  ns->cs_version) {
#ifdef ESRV_DEBUG
        LOG(llevDebug, "CS: csversion mismatch (%d,%d)\n",
            VERSION_CS,ns->cs_version);
#endif
    }
    cp = strchr(buf+1,' ');
    if (!cp) return;
    ns->sc_version = atoi(cp);
    if (VERSION_SC != ns->sc_version) {
#ifdef ESRV_DEBUG
        LOG(llevDebug, "CS: scversion mismatch (%d,%d)\n",
            VERSION_SC,ns->sc_version);
#endif
    }
    cp = strchr(cp+1, ' ');
    if (cp) {
        LOG(llevDebug,"CS: connection from client of type <%s>, ip %s\n",
            cp, ns->host);
    }
}

/**
 * Sound related function.
 * @todo remove once clients don't try to use this - server closes connection on invalid client.
 */

void set_sound_cmd(char *buf, int len, socket_struct *ns) {
}

/** client wants the map resent
 * @todo remove
*/

void map_redraw_cmd(char *buf, int len, player *pl) {
    /* This function is currently disabled; just clearing the
     * map state results in display errors. It should clear the
     * cache and send a newmap command.  Unfortunately this
     * solution does not work because some client versions send
     * a mapredraw command after receiving a newmap command.
     */
}

/** Newmap command */
void map_newmap_cmd(player *pl) {
    /* If getting a newmap command, this scroll information
     * is no longer relevant.
     */
    pl->socket.map_scroll_x = 0;
    pl->socket.map_scroll_y = 0;

    if (pl->socket.newmapcmd == 1) {
        memset(&pl->socket.lastmap, 0, sizeof(pl->socket.lastmap));
        Write_String_To_Socket(&pl->socket, "newmap", 6);
    }
}



/**
 * Moves an object (typically, container to inventory).
 * syntax is: move (to) (tag) (nrof)
 */
void move_cmd(char *buf, int len,player *pl) {
    int vals[3], i;

    /* A little funky here.  We only cycle for 2 records, because
     * we obviously am not going to find a space after the third
     * record.  Perhaps we should just replace this with a
     * sscanf?
     */
    for (i=0; i<2; i++) {
        vals[i]=atoi(buf);
        if (!(buf = strchr(buf, ' '))) {
            LOG(llevError,"Incomplete move command: %s\n", buf);
            return;
        }
        buf++;
    }
    vals[2]=atoi(buf);

/*    LOG(llevDebug,"Move item %d (nrof=%d) to %d.\n", vals[1], vals[2], vals[0]);*/
    esrv_move_object(pl->ob,vals[0], vals[1], vals[2]);
}



/***************************************************************************
 *
 * Start of commands the server sends to the client.
 *
 ***************************************************************************
 */

/**
 * Asks the client to query the user.  This way, the client knows
 * it needs to send something back (vs just printing out a message)
 */
void send_query(socket_struct *ns, uint8 flags, const char *text) {
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), "query %d %s", flags, text?text:"");
    Write_String_To_Socket(ns, buf, strlen(buf));
}

#define AddIfInt64(Old,New,Type) if (Old != New) {  \
        Old = New;                                  \
        SockList_AddChar(&sl, Type);                \
        SockList_AddInt64(&sl, New);                \
    }

#define AddIfInt(Old,New,Type) if (Old != New) {    \
        Old = New;                                  \
        SockList_AddChar(&sl, Type);                \
        SockList_AddInt(&sl, New);                  \
    }

#define AddIfShort(Old,New,Type) if (Old != New) {  \
        Old = New;                                  \
        SockList_AddChar(&sl, Type);                \
        SockList_AddShort(&sl, New);                \
    }

#define AddIfFloat(Old,New,Type) if (Old != New) {      \
        Old = New;                                      \
        SockList_AddChar(&sl, Type);                    \
        SockList_AddInt(&sl,(long)(New*FLOAT_MULTI));   \
    }

#define AddIfString(Old,New,Type) if (Old == NULL || strcmp(Old,New)) { \
        if (Old) free(Old);                                             \
        Old = strdup_local(New);                                        \
        SockList_AddChar(&sl, Type);                                    \
        SockList_AddLen8Data(&sl, New, strlen(New));                    \
    }

/**
 * Sends a statistics update.  We look at the old values,
 * and only send what has changed.  Stat mapping values are in newclient.h
 * Since this gets sent a lot, this is actually one of the few binary
 * commands for now.
 */
void esrv_update_stats(player *pl) {
    SockList sl;
    char buf[MAX_BUF];
    uint16 flags;
    uint8 s;

    SockList_Init(&sl);
    SockList_AddString(&sl, "stats ");

    if (pl->ob != NULL) {
        AddIfShort(pl->last_stats.hp, pl->ob->stats.hp, CS_STAT_HP);
        AddIfShort(pl->last_stats.maxhp, pl->ob->stats.maxhp, CS_STAT_MAXHP);
        AddIfShort(pl->last_stats.sp, pl->ob->stats.sp, CS_STAT_SP);
        AddIfShort(pl->last_stats.maxsp, pl->ob->stats.maxsp, CS_STAT_MAXSP);
        AddIfShort(pl->last_stats.grace, pl->ob->stats.grace, CS_STAT_GRACE);
        AddIfShort(pl->last_stats.maxgrace, pl->ob->stats.maxgrace,
                   CS_STAT_MAXGRACE);
        AddIfShort(pl->last_stats.Str, pl->ob->stats.Str, CS_STAT_STR);
        AddIfShort(pl->last_stats.Int, pl->ob->stats.Int, CS_STAT_INT);
        AddIfShort(pl->last_stats.Pow, pl->ob->stats.Pow, CS_STAT_POW);
        AddIfShort(pl->last_stats.Wis, pl->ob->stats.Wis, CS_STAT_WIS);
        AddIfShort(pl->last_stats.Dex, pl->ob->stats.Dex, CS_STAT_DEX);
        AddIfShort(pl->last_stats.Con, pl->ob->stats.Con, CS_STAT_CON);
        AddIfShort(pl->last_stats.Cha, pl->ob->stats.Cha, CS_STAT_CHA);
    }

    for (s=0;s<NUM_SKILLS;s++) {
        if (pl->last_skill_ob[s] &&
            pl->last_skill_exp[s] != pl->last_skill_ob[s]->stats.exp) {

            /* Always send along the level if exp changes.  This
             * is only 1 extra byte, but keeps processing simpler.
             */
            SockList_AddChar(&sl, (char)(s + CS_STAT_SKILLINFO));
            SockList_AddChar(&sl, (char)pl->last_skill_ob[s]->level);
            SockList_AddInt64(&sl, pl->last_skill_ob[s]->stats.exp);
            pl->last_skill_exp[s] =  pl->last_skill_ob[s]->stats.exp;
        }
    }
    AddIfInt64(pl->last_stats.exp, pl->ob->stats.exp, CS_STAT_EXP64);
    AddIfShort(pl->last_level, (char)pl->ob->level, CS_STAT_LEVEL);
    AddIfShort(pl->last_stats.wc, pl->ob->stats.wc, CS_STAT_WC);
    AddIfShort(pl->last_stats.ac, pl->ob->stats.ac, CS_STAT_AC);
    AddIfShort(pl->last_stats.dam, pl->ob->stats.dam, CS_STAT_DAM);
    AddIfFloat(pl->last_speed, pl->ob->speed, CS_STAT_SPEED);
    AddIfShort(pl->last_stats.food, pl->ob->stats.food, CS_STAT_FOOD);
    AddIfFloat(pl->last_weapon_sp, pl->ob->weapon_speed, CS_STAT_WEAP_SP);
    AddIfInt(pl->last_weight_limit, (sint32)weight_limit[pl->ob->stats.Str],
             CS_STAT_WEIGHT_LIM);
    flags=0;
    if (pl->fire_on) flags |=SF_FIREON;
    if (pl->run_on) flags |= SF_RUNON;

    AddIfShort(pl->last_flags, flags, CS_STAT_FLAGS);
    if (pl->socket.sc_version<1025) {
        AddIfShort(pl->last_resist[ATNR_PHYSICAL],
                   pl->ob->resist[ATNR_PHYSICAL], CS_STAT_ARMOUR);
    } else {
        int i;

        for (i=0; i<NROFATTACKS; i++) {
            /* Skip ones we won't send */
            if (atnr_cs_stat[i]==-1) continue;
            AddIfShort(pl->last_resist[i], pl->ob->resist[i],
                       (char)atnr_cs_stat[i]);
        }
    }
    if (pl->socket.monitor_spells) {
        AddIfInt(pl->last_path_attuned, pl->ob->path_attuned,
                 CS_STAT_SPELL_ATTUNE);
        AddIfInt(pl->last_path_repelled, pl->ob->path_repelled,
                 CS_STAT_SPELL_REPEL);
        AddIfInt(pl->last_path_denied, pl->ob->path_denied,
                 CS_STAT_SPELL_DENY);
    }
    /* we want to use the new fire & run system in new client */
    rangetostring(pl->ob, buf, sizeof(buf));
    AddIfString(pl->socket.stats.range, buf, CS_STAT_RANGE);
    set_title(pl->ob, buf, sizeof(buf));
    AddIfString(pl->socket.stats.title, buf, CS_STAT_TITLE);

    /* Only send it away if we have some actual data */
    if (sl.len>6) {
#ifdef ESRV_DEBUG
        LOG(llevDebug,"Sending stats command, %d bytes long.\n", sl.len);
#endif
        Send_With_Handling(&pl->socket, &sl);
    }
    SockList_Term(&sl);
}


/**
 * Tells the client that here is a player it should start using.
 */
void esrv_new_player(player *pl, uint32 weight) {
    SockList    sl;

    pl->last_weight = weight;

    if (!(pl->socket.faces_sent[pl->ob->face->number] & NS_FACESENT_FACE))
        esrv_send_face(&pl->socket, pl->ob->face->number, 0);

    SockList_Init(&sl);
    SockList_AddString(&sl, "player ");
    SockList_AddInt(&sl, pl->ob->count);
    SockList_AddInt(&sl, weight);
    SockList_AddInt(&sl, pl->ob->face->number);
    SockList_AddLen8Data(&sl, pl->ob->name, strlen(pl->ob->name));

    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
    SET_FLAG(pl->ob, FLAG_CLIENT_SENT);
}


/**
 * Need to send an animation sequence to the client.
 * We will send appropriate face commands to the client if we haven't
 * sent them the face yet (this can become quite costly in terms of
 * how much we are sending - on the other hand, this should only happen
 * when the player logs in and picks stuff up.
 */
void esrv_send_animation(socket_struct *ns, short anim_num) {
    SockList sl;
    int i;

    /* Do some checking on the anim_num we got.  Note that the animations
     * are added in contigous order, so if the number is in the valid
     * range, it must be a valid animation.
     */
    if (anim_num < 0 || anim_num > num_animations) {
        LOG(llevError,"esrv_send_anim (%d) out of bounds??\n",anim_num);
        return;
    }

    SockList_Init(&sl);
    SockList_AddString(&sl, "anim ");
    SockList_AddShort(&sl, anim_num);
    SockList_AddShort(&sl, 0);  /* flags - not used right now */
    /* Build up the list of faces.  Also, send any information (ie, the
     * the face itself) down to the client.
     */
    for (i=0; i<animations[anim_num].num_animations; i++) {
        if (!(ns->faces_sent[animations[anim_num].faces[i]] &
              NS_FACESENT_FACE))
            esrv_send_face(ns,animations[anim_num].faces[i],0);
        /* flags - not used right now */
        SockList_AddShort(&sl, animations[anim_num].faces[i]);
    }
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
    ns->anims_sent[anim_num] = 1;
}


/****************************************************************************
 *
 * Start of map related commands.
 *
 ****************************************************************************/


/** Clears a map cell */
static void map_clearcell(struct map_cell_struct *cell, int face, int count) {
    cell->darkness=count;
    memset(cell->faces, face, sizeof(cell->faces));
}

#define MAX_HEAD_POS    MAX(MAX_CLIENT_X, MAX_CLIENT_Y)

/** Using a global really isn't a good approach, but saves the over head of
 * allocating and deallocating such a block of data each time run through,
 * and saves the space of allocating this in the socket object when we only
 * need it for this cycle.  If the serve is ever threaded, this needs to be
 * re-examined.
 */

static object  *heads[MAX_HEAD_POS * MAX_HEAD_POS * MAP_LAYERS];

/****************************************************************************
 * This block is for map2 drawing related commands.
 * Note that the map2 still uses other functions.
 *
 ***************************************************************************/

/**
 * object 'ob' at 'ax,ay' on 'layer' is visible to the client.
 * This function does the following things:
 * If is_head head is set, this means this is from the heads[] array,
 * so don't try to store it away again - just send it and update
 * our look faces.
 *
 * 1) If a multipart object and we are not at the lower right corner,
 *    store this info away for later use.
 * 2) Check to see if this face has been sent to the client.  If not,
 *    we add data to the socklist, update the last map, and send any
 *    other data the client will need (smoothing table, image data, etc)
 * 3) Return 1 if function increased socket.
 * 4) has_obj is increased by one if there are visible objects on this
 *    this space, whether or not we sent them.  Basically, if has_obj
 *    is 0, we can clear info about this space.  It could be set to 1
 *    with the function returning zero - this means there are objects
 *    on the space we have already sent to the client.
 */
static int map2_add_ob(int ax, int ay, int layer, object *ob, SockList *sl,
                       socket_struct *ns, int *has_obj, int is_head) {
    uint16  face_num;
    uint8   nlayer, smoothlevel=0;
    object  *head;

    head = ob->head;
    if (!head) head = ob;
    face_num = ob->face->number;

    /* This is a multipart object, and we are not at the lower
     * right corner. So we need to store away the lower right corner.
     */
    if (!is_head && head && (head->arch->tail_x || head->arch->tail_y) &&
        (head->arch->tail_x != ob->arch->clone.x ||
         (head->arch->tail_y  != ob->arch->clone.y))) {
        int bx, by, l;


        /* Basically figure out where the offset is from where we
         * are right now.  the ob->arch->clone.{x,y} values hold
         * the offset that this current piece is from the head,
         * and the tail is where the tail is from the head.
         * Note that bx and by will equal sx and sy if we are
         * already working on the bottom right corner.  If ob is
         * the head, the clone values will be zero, so the right
         * thing will still happen.
         */
        bx = ax + head->arch->tail_x - ob->arch->clone.x;
        by = ay + head->arch->tail_y - ob->arch->clone.y;

        /* I don't think this can ever happen, but better to check
         * for it just in case.
         */
        if (bx < ax || by < ay) {
            LOG(llevError, "map2_add_ob: bx (%d) or by (%d) is less than ax (%d) or ay (%d)\n",
                bx, by, ax, ay);
            face_num = 0;
        }
        /* the target position must be within +/-1 of our current
         * layer as the layers are defined.  We are basically checking
         * to see if we have already stored this object away.
         */
        for (l = layer-1; l<=layer+1; l++) {
            if (l<0 || l>= MAP_LAYERS) continue;
            if (heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + l] == head)
                break;
        }
        /* Didn't find it.  So we need to store it away.  Try to store it
         * on our original layer, and then move up a layer.
         */
        if (l == (layer+2)) {
            if (!heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + layer])
                heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + layer] = head;
            else if ((layer + 1) <MAP_LAYERS &&
                     !heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + layer+1])
                heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + layer + 1] =head;
        }
        return 0;
        /* Ok - All done storing away the head for future use */

    } else {
        (*has_obj)++;
        if (QUERY_FLAG(ob, FLAG_CLIENT_ANIM_SYNC) ||
            QUERY_FLAG(ob, FLAG_CLIENT_ANIM_RANDOM)) {
            face_num = ob->animation_id | (1 << 15);
            if (QUERY_FLAG(ob, FLAG_CLIENT_ANIM_SYNC))
                face_num |=  ANIM_SYNC;
            else if (QUERY_FLAG(ob, FLAG_CLIENT_ANIM_RANDOM))
                face_num |= ANIM_RANDOM;
        }
        /* Since face_num includes the bits for the animation tag,
         * and we will store that away in the faces[] array, below
         * check works fine _except_ for the case where animation
         * speed chances.
         */
        if (ns->lastmap.cells[ax][ay].faces[layer] != face_num)  {
            uint8   len, anim_speed=0, i;


            /* This block takes care of sending the actual face
             * to the client. */
            ns->lastmap.cells[ax][ay].faces[layer] = face_num;

            /* Now form the data packet */
            nlayer = 0x10 + layer;

            len = 2;

            if (!MAP_NOSMOOTH(ob->map)) {
                smoothlevel = ob->smoothlevel;
                if (smoothlevel) len++;
            }

            if (QUERY_FLAG(ob, FLAG_CLIENT_ANIM_SYNC) ||
                QUERY_FLAG(ob, FLAG_CLIENT_ANIM_RANDOM)) {
                len++;
                /* 1/0.004 == 250, so this is a good cap for an
                 * upper limit */
                if (ob->anim_speed) anim_speed=ob->anim_speed;
                else if (FABS(ob->speed)<0.004) anim_speed=255;
                else if (FABS(ob->speed)>=1.0) anim_speed=1;
                else anim_speed = (int)(1.0/FABS(ob->speed));

                if (!ns->anims_sent[ob->animation_id])
                    esrv_send_animation(ns, ob->animation_id);

                /* If smoothing, need to send smoothing information
                 * for all faces in the animation sequence. Since
                 * smoothlevel is an object attribute,
                 * it applies to all faces.
                 */
                if (smoothlevel) {
                    for (i=0; i < NUM_ANIMATIONS(ob); i++) {
                        if (!(ns->faces_sent[animations[ob->animation_id].faces[i]] &
                              NS_FACESENT_SMOOTH))
                            send_smooth(ns, animations[ob->animation_id].faces[i]);
                    }
                }
            } else if (!(ns->faces_sent[face_num] & NS_FACESENT_FACE)) {
                esrv_send_face(ns, face_num, 0);
            }

            if (smoothlevel &&
                !(ns->faces_sent[ob->face->number] & NS_FACESENT_SMOOTH))
                send_smooth(ns, ob->face->number);

            /* Length of packet */
            nlayer |=  len << 5;

            SockList_AddChar(sl, nlayer);
            SockList_AddShort(sl, face_num);
            if (anim_speed) SockList_AddChar(sl, anim_speed);
            if (smoothlevel) SockList_AddChar(sl, smoothlevel);
            return 1;
        } /* Face is different */
    }
    return 0;
}

/* This function is used see if a layer needs to be cleared.
 * It updates the socklist, and returns 1 if the update is
 * needed, 0 otherwise.
 */
static int map2_delete_layer(int ax, int ay, int layer,
                             SockList *sl, socket_struct *ns) {
    int nlayer;

    if (ns->lastmap.cells[ax][ay].faces[layer] != 0)  {
        /* Now form the data packet */
        nlayer = 0x10 + layer + (2 << 5);
        SockList_AddChar(sl, nlayer);
        SockList_AddShort(sl, 0);
        ns->lastmap.cells[ax][ay].faces[layer] = 0;
        return 1;
    }
    return 0;
}

/*
 * This function is used to check a space (ax, ay) whose only
 * data we may care about are any heads.  Basically, this
 * space is out of direct view.  This is only used with the
 * Map2 protocol
 */
static void check_space_for_heads(int ax, int ay,
                                  SockList *sl, socket_struct *ns) {
    int layer, got_one=0, del_one=0, oldlen, has_obj=0;
    uint16 coord;

    coord = ((ax + MAP2_COORD_OFFSET) & 0x3f) <<
            10 | ((ay + MAP2_COORD_OFFSET)& 0x3f) << 4;
    oldlen = sl->len;
    SockList_AddShort(sl, coord);

    for (layer=0; layer < MAP_LAYERS; layer++) {
        if (heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS + layer]) {
            /* in this context, got_one should always increase
             * because heads should always point to data to really send.
             */
            got_one += map2_add_ob(ax, ay, layer,
                                   heads[(ay * MAX_HEAD_POS + ax) *
                                         MAP_LAYERS + layer],
                                   sl, ns, &has_obj, 1);
        } else {
            del_one += map2_delete_layer(ax, ay, layer, sl, ns);
        }
    }
    /* Note - if/when lighting information is added, some code is
     * needed here - lighting sources that are out of sight may still
     * extend into the viewable area.
     */

    /* If nothing to do for this space, we
     * can erase the coordinate bytes
     */
    if (!del_one && !got_one) {
        sl->len = oldlen;
    } else if (del_one && !has_obj) {
        /* If we're only deleting faces and not adding, and there
         * are not any faces on the space we care about,
         * more efficient
         * to send 0 as the type/len field.
         */
        sl->len = oldlen +2;        /* 2 bytes for coordinate */
        SockList_AddChar(sl, 0);    /* Clear byte */
        SockList_AddChar(sl, 255);  /* Termination byte */
        map_clearcell(&ns->lastmap.cells[ax][ay],0,0);
    } else {
        SockList_AddChar(sl, 255);  /* Termination byte */
    }
}

void draw_client_map2(object *pl) {
    int x,y,ax, ay, d, max_x, max_y, oldlen, layer;
    size_t startlen;
    sint16 nx, ny;
    SockList sl;
    uint16  coord;
    mapstruct *m;
    object *ob;

    SockList_Init(&sl);
    SockList_AddString(&sl, "map2 ");
    startlen = sl.len;

    /* Init data to zero */
    memset(heads, 0,
           sizeof(object *) * MAX_HEAD_POS * MAX_HEAD_POS * MAP_LAYERS);

    /* x,y are the real map locations.  ax, ay are viewport relative
     * locations.
     */
    ay=0;

    /* We could do this logic as conditionals in the if statement,
     * but that started to get a bit messy to look at.
     */
    max_x = pl->x+(pl->contr->socket.mapx+1)/2 + MAX_HEAD_OFFSET;
    max_y = pl->y+(pl->contr->socket.mapy+1)/2 + MAX_HEAD_OFFSET;


    /* Handle map scroll */
    if (pl->contr->socket.map_scroll_x || pl->contr->socket.map_scroll_y) {
        coord = ((pl->contr->socket.map_scroll_x +
                  MAP2_COORD_OFFSET) & 0x3f) << 10 |
                ((pl->contr->socket.map_scroll_y +
                  MAP2_COORD_OFFSET) & 0x3f) << 4 | 1;
        pl->contr->socket.map_scroll_x=0;
        pl->contr->socket.map_scroll_y=0;
        SockList_AddShort(&sl, coord);
    }

    for (y=(pl->y - pl->contr->socket.mapy / 2); y< max_y; y++, ay++) {
        ax=0;
        for (x=(pl->x - pl->contr->socket.mapx / 2); x< max_x ; x++,ax++) {

            /* If this space is out of the normal viewable area,
             * we only check the heads value.  This is used to
             * handle big images - if they extend to a viewable
             * portion, we need to send just the lower right corner.
             */
            if (ax >= pl->contr->socket.mapx || ay >= pl->contr->socket.mapy) {
                check_space_for_heads(ax, ay, &sl, &pl->contr->socket);

            } else {
                /* This space is within the viewport of the client.  Due
                 * to walls or darkness, it may still not be visible.
                 */

                /* Meaning of d:
                 * 0 - object is in plain sight, full brightness.
                 * 1 - MAX_DARKNESS - how dark the space is - higher
                 * value is darker space.  If level is at max darkness,
                 * you can't see the space (too dark)
                 * 100 - space is blocked from sight.
                 */
                d =  pl->contr->blocked_los[ax][ay];

                /* If the coordinates are not valid, or it is too
                 * dark to see, we tell the client as such
                 */
                nx=x;
                ny=y;
                m = get_map_from_coord(pl->map, &nx, &ny);
                coord = ((ax + MAP2_COORD_OFFSET) & 0x3f) << 10 |
                        ((ay + MAP2_COORD_OFFSET) & 0x3f) << 4;

                if (!m) {
                    /* space is out of map.  Update space and clear
                     * values if this hasn't already been done.
                     * If the space is out of the map, it shouldn't
                     * have a head.
                     */
                    if (pl->contr->socket.lastmap.cells[ax][ay].darkness !=0) {
                        SockList_AddShort(&sl, coord);
                        SockList_AddChar(&sl, 0);
                        SockList_AddChar(&sl, 255); /* Termination byte */
                        map_clearcell(&pl->contr->socket.lastmap.cells[ax][ay],
                                      0,0);
                    }
                } else if (d>=MAX_LIGHT_RADII) {
                    /* This block deals with spaces that are not
                     * visible due to darkness or walls.  Still
                     * need to send the heads for this space.
                     */
                    check_space_for_heads(ax, ay, &sl, &pl->contr->socket);
                } else {
                    int have_darkness=0, has_obj=0, got_one=0, del_one=0, g1;

                    /* In this block, the space is visible. */

                    /* Rather than try to figure out what everything
                     * that we might need to send is, then form the
                     * packet after that, we presume that we will in
                     * fact form a packet, and update the bits by what
                     * we do actually send.  If we send nothing, we
                     * just back out sl.len to the old value, and no
                     * harm is done.
                     * I think this is simpler than doing a bunch of
                     * checks to see what if anything we need to send,
                     * setting the bits, then doing those checks again
                     * to add the real data.
                     */

                    oldlen = sl.len;
                    SockList_AddShort(&sl, coord);

                    /* Darkness changed */
                    if (pl->contr->socket.lastmap.cells[ax][ay].darkness != d
                        && pl->contr->socket.darkness) {
                        pl->contr->socket.lastmap.cells[ax][ay].darkness = d;
                        /* Darkness tag & length*/
                        SockList_AddChar(&sl, 0x1 | 1 << 5);
                        SockList_AddChar(&sl, 255 - d * (256/MAX_LIGHT_RADII));
                        have_darkness = 1;
                    }

                    for (layer=0; layer < MAP_LAYERS; layer++) {
                        ob = GET_MAP_FACE_OBJ(m, nx, ny, layer);

                        /* Special case: send player itself if invisible */
                        if (!ob && x == pl->x && y == pl->y &&
                            (pl->invisible & (pl->invisible < 50 ? 4:1)) &&
                            (layer == MAP_LAYER_LIVING1 ||
                             layer == MAP_LAYER_LIVING2))
                            ob = pl;

                        if (ob)  {
                            g1 = has_obj;
                            got_one += map2_add_ob(ax, ay, layer, ob,
                                                   &sl, &pl->contr->socket,
                                                   &has_obj, 0);
                            /* If we are just storing away the head
                             * for future use, then effectively this
                             * space/layer is blank, and we should clear
                             * it if needed.
                             */
                            if (g1 == has_obj)
                                del_one +=
                                    map2_delete_layer(ax, ay, layer, &sl,
                                                      &pl->contr->socket);
                        } else {
                            del_one +=
                                map2_delete_layer(ax, ay, layer, &sl,
                                                  &pl->contr->socket);
                        }
                    }
                    /* If nothing to do for this space, we
                     * can erase the coordinate bytes
                     */
                    if (!del_one && !got_one && !have_darkness) {
                        sl.len = oldlen;
                    } else if (del_one && !has_obj) {
                        /* If we're only deleting faces and don't
                         * have any objs we care about, just clear
                         * space.  Note it is possible we may have
                         * darkness, but if there is nothing on the
                         * space, darkness isn't all that interesting
                         * - we can send it when an object shows up.
                         */
                        sl.len = oldlen +2;     /* 2 bytes for coordinate */
                        SockList_AddChar(&sl, 0);   /* Clear byte */
                        SockList_AddChar(&sl, 255); /* Termination byte */
                        map_clearcell(&pl->contr->socket.lastmap.cells[ax][ay],
                                      0,0);
                    } else {
                        SockList_AddChar(&sl, 255); /* Termination byte */
                    }
                }
            } /* else this is a viewable space */
        } /* for x loop */
    } /* for y loop */

    /* Only send this if there are in fact some differences. */
    if (sl.len > startlen) {
        Send_With_Handling(&pl->contr->socket, &sl);
    }
    SockList_Term(&sl);
}



/**
 * Draws client map.
 */
void draw_client_map(object *pl) {
    int i,j;
    sint16  ax, ay;
    int mflags;
    mapstruct   *m, *pm;

    if (pl->type != PLAYER) {
        LOG(llevError,
            "draw_client_map called with non player/non eric-server\n");
        return;
    }

    if (pl->contr->transport) {
        pm = pl->contr->transport->map;
    } else
        pm = pl->map;

    /* If player is just joining the game, he isn't here yet, so
     * the map can get swapped out.  If so, don't try to send them
     * a map.  All will be OK once they really log in.
     */
    if (pm==NULL || pm->in_memory!=MAP_IN_MEMORY) return;


    /*
     * This block just makes sure all the spaces are properly
     * updated in terms of what they look like.
     */
    for (j = (pl->y - pl->contr->socket.mapy/2) ;
         j < (pl->y + (pl->contr->socket.mapy+1)/2); j++) {
        for (i = (pl->x - pl->contr->socket.mapx/2) ;
             i < (pl->x + (pl->contr->socket.mapx+1)/2); i++) {
            ax=i;
            ay=j;
            m = pm;
            mflags = get_map_flags(m, &m, ax, ay, &ax, &ay);
            if (mflags & P_OUT_OF_MAP)
                continue;
            if (mflags & P_NEED_UPDATE)
                update_position(m, ax, ay);
            /* If a map is visible to the player, we don't want
             * to swap it out just to reload it.  This should
             * really call something like swap_map, but this is
             * much more efficient and 'good enough'
             */
            if (mflags & P_NEW_MAP)
                m->timeout = 50;
        }
    }

    /* do LOS after calls to update_position */
    if (pl->contr->do_los) {
        update_los(pl);
        pl->contr->do_los = 0;
    }

    draw_client_map2(pl);
}


void esrv_map_scroll(socket_struct *ns,int dx,int dy) {
    struct Map newmap;
    int x,y, mx, my;

    ns->map_scroll_x += dx;
    ns->map_scroll_y += dy;

    mx = ns->mapx + MAX_HEAD_OFFSET;
    my = ns->mapy + MAX_HEAD_OFFSET;

    /* the x and y here are coordinates for the new map, i.e. if we moved
     * (dx,dy), newmap[x][y] = oldmap[x-dx][y-dy].  For this reason,
     * if the destination x or y coordinate is outside the viewable
     * area, we clear the values - otherwise, the old values
     * are preserved, and the check_head thinks it needs to clear them.
     */
    for (x=0; x<mx; x++) {
        for (y=0; y<my; y++) {
            if (x >= ns->mapx || y >= ns->mapy) {
                /* clear cells outside the viewable area */
                memset(&newmap.cells[x][y], 0, sizeof(struct map_cell_struct));
            } else if ((x+dx) < 0 || (x+dx) >= ns->mapx ||
                       (y+dy) < 0 || (y + dy) >= ns->mapy) {
                /* clear newly visible tiles within the viewable area */
                memset(&(newmap.cells[x][y]), 0,
                       sizeof(struct map_cell_struct));
            } else {
                memcpy(&(newmap.cells[x][y]),
                       &(ns->lastmap.cells[x+dx][y+dy]),
                       sizeof(struct map_cell_struct));
            }
        }
    }

    memcpy(&(ns->lastmap), &newmap,sizeof(struct Map));
}

/**
 * GROS: The following one is used to allow a plugin to send a generic cmd to
 * a player. Of course, the client need to know the command to be able to
 * manage it !
 */
void send_plugin_custom_message(object *pl, char *buf) {
    Write_String_To_Socket(&pl->contr->socket, buf, strlen(buf));
}

/**
 * This sends the experience table the sever is using
 */
void send_exp_table(socket_struct *ns, char *params) {
    SockList sl;
    int i;
    extern sint64 *levels;

    SockList_Init(&sl);
    SockList_AddString(&sl, "replyinfo exp_table\n");
    SockList_AddShort(&sl, settings.max_level+1);
    for (i=1; i<= settings.max_level; i++) {
        if (SockList_Avail(&sl) < 8) {
            LOG(llevError, "Buffer overflow in send_exp_table, not sending all information\n");
            break;
        }
        SockList_AddInt64(&sl, levels[i]);
    }
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * This sends the skill number to name mapping.  We ignore
 * the params - we always send the same info no matter what.
 */
void send_skill_info(socket_struct *ns, char *params) {
    SockList sl;
    int i;

    SockList_Init(&sl);
    SockList_AddString(&sl, "replyinfo skill_info\n");
    for (i=1; i< NUM_SKILLS; i++) {
        size_t len;

        len = 16+strlen(skill_names[i]); /* upper bound for length */
        if (SockList_Avail(&sl) < len) {
            LOG(llevError, "Buffer overflow in send_skill_info, not sending all skill information\n");
            break;
        }

        SockList_AddPrintf(&sl, "%d:%s\n", i+CS_STAT_SKILLINFO, skill_names[i]);
    }
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * This sends the spell path to name mapping.  We ignore
 * the params - we always send the same info no matter what.
 */
void send_spell_paths(socket_struct *ns, char *params) {
    SockList sl;
    int i;

    SockList_Init(&sl);
    SockList_AddString(&sl, "replyinfo spell_paths\n");
    for (i=0; i<NRSPELLPATHS; i++) {
        size_t len;

        len = 16+strlen(spellpathnames[i]); /* upper bound for length */
        if (SockList_Avail(&sl) < len) {
            LOG(llevError, "Buffer overflow in send_spell_paths, not sending all spell information\n");
            break;
        }

        SockList_AddPrintf(&sl, "%d:%s\n", 1<<i, spellpathnames[i]);
    }
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * Creates the appropriate reply to the 'race_list' request info.
 *
 * @param sl
 * suitable reply.
 */
static void build_race_list_reply(SockList *sl) {
    archetype *race;

    SockList_AddString(sl, "replyinfo race_list ");

    for (race = first_archetype; race; race = race->next) {
        if (race->clone.type == PLAYER) {
            SockList_AddPrintf(sl, "|%s", race->name);
        }
    }
}

/**
 * Send the list of player races to the client.
 * The reply is kept in a static buffer, as it won't change during server run.
 *
 * @param ns
 * where to send.
 * @param params
 * ignored.
 */
void send_race_list(socket_struct *ns, char *params) {
    static SockList sl;
    static int sl_initialized = 0;

    if (!sl_initialized) {
        sl_initialized = 1;
        SockList_Init(&sl);
        build_race_list_reply(&sl);
    }

    Send_With_Handling(ns, &sl);
}

/**
 * Sends information on specified race to the client.
 *
 * @param ns
 * where to send.
 * @param params
 * race name to send.
 * @todo finish writing
 */
void send_race_info(socket_struct *ns, char *params) {
    archetype *race = try_find_archetype(params);
    SockList sl;

    SockList_Init(&sl);
    SockList_AddPrintf(&sl, "replyinfo race_info %s", params);

    if (race) {
    }

    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

/**
 * Creates the appropriate reply to the 'class_list' request info.
 *
 * @param sl
 * reply.
 */
static void build_class_list_reply(SockList *sl) {
    archetype *cl;

    SockList_Reset(sl);
    SockList_AddString(sl, "replyinfo class_list ");

    for (cl = first_archetype; cl; cl = cl->next) {
        if (cl->clone.type == CLASS) {
            SockList_AddPrintf(sl, "|%s", cl->name);
        }
    }
}

/**
 * Sends the list of classes to the client.
 * The reply is kept in a static buffer, as it won't change during server run.
 *
 * @param ns
 * client to send to.
 * @param params
 * ignored.
 */
void send_class_list(socket_struct *ns, char *params) {
    static SockList sl;
    static int sl_initialized = 0;

    if (!sl_initialized) {
        sl_initialized = 1;
        SockList_Init(&sl);
        build_class_list_reply(&sl);
    }

    Send_With_Handling(ns, &sl);
}

/**
 * Send information on the specified class.
 *
 * @param ns
 * where to send.
 * @param params
 * class name to send.
 * @todo finish writing
 */
void send_class_info(socket_struct *ns, char *params) {
}

/**
 * This looks for any spells the player may have that have changed
 * their stats. It then sends an updspell packet for each spell that
 * has changed in this way.
 */
void esrv_update_spells(player *pl) {
    SockList sl;
    int flags=0;
    object *spell;
    client_spell *spell_info;

    if (!pl->socket.monitor_spells) return;

    /* Handles problem at login, where this is called from fix_object
     * before we have had a chance to send spells to the player.  It does seem
     * to me that there should never be a case where update_spells is called
     * before add_spells has been called.  Add_spells() will update the
     * spell_state to non null.
     */
    if (!pl->spell_state) return;

    for (spell=pl->ob->inv; spell!=NULL; spell=spell->below) {
        if (spell->type == SPELL) {
            spell_info = get_client_spell_state(pl, spell);
            /* check if we need to update it*/
            if (spell_info->last_sp !=
                SP_level_spellpoint_cost(pl->ob, spell, SPELL_MANA)) {
                spell_info->last_sp =
                    SP_level_spellpoint_cost(pl->ob, spell, SPELL_MANA);
                flags |= UPD_SP_MANA;
            }
            if (spell_info->last_grace !=
                SP_level_spellpoint_cost(pl->ob, spell, SPELL_GRACE)) {
                spell_info->last_grace =
                    SP_level_spellpoint_cost(pl->ob, spell, SPELL_GRACE);
                flags |= UPD_SP_GRACE;
            }
            if (spell_info->last_dam !=
                spell->stats.dam+SP_level_dam_adjust(pl->ob, spell)) {
                spell_info->last_dam =
                    spell->stats.dam+SP_level_dam_adjust(pl->ob, spell);
                flags |= UPD_SP_DAMAGE;
            }
            if (flags !=0) {
                SockList_Init(&sl);
                SockList_AddString(&sl, "updspell ");
                SockList_AddChar(&sl, flags);
                SockList_AddInt(&sl, spell->count);
                if (flags & UPD_SP_MANA)
                    SockList_AddShort(&sl, spell_info->last_sp);
                if (flags & UPD_SP_GRACE)
                    SockList_AddShort(&sl, spell_info->last_grace);
                if (flags & UPD_SP_DAMAGE)
                    SockList_AddShort(&sl, spell_info->last_dam);
                flags = 0;
                Send_With_Handling(&pl->socket, &sl);
                SockList_Term(&sl);
            }
        }
    }
}

void esrv_remove_spell(player *pl, object *spell) {
    SockList sl;

    if (!pl->socket.monitor_spells) return;
    if (!pl || !spell || spell->env != pl->ob) {
        LOG(llevError, "Invalid call to esrv_remove_spell\n");
        return;
    }
    SockList_Init(&sl);
    SockList_AddString(&sl, "delspell ");
    SockList_AddInt(&sl, spell->count);
    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}

/**
 * Sends the "pickup" state to pl if client wants it requested.
 *
 * @param pl
 * player that just logged in.
 */
void esrv_send_pickup(player *pl) {
    SockList sl;
    if (!pl->socket.want_pickup)
        return;
    SockList_Init(&sl);
    SockList_AddString(&sl, "pickup ");
    SockList_AddInt(&sl, pl->mode);
    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}

/** appends the spell *spell to the Socklist we will send the data to. */
static void append_spell(player *pl, SockList *sl, object *spell) {
    client_spell *spell_info;
    int len, i, skill=0;

    if (!(spell->name)) {
        LOG(llevError, "item number %d is a spell with no name.\n",
            spell->count);
        return;
    }

    if (spell->face && !(pl->socket.faces_sent[spell->face->number] & NS_FACESENT_FACE))
        esrv_send_face(&pl->socket, spell->face->number, 0);

    spell_info = get_client_spell_state(pl, spell);
    SockList_AddInt(sl, spell->count);
    SockList_AddShort(sl, spell->level);
    SockList_AddShort(sl, spell->casting_time);
    /* store costs and damage in the object struct, to compare to later */
    spell_info->last_sp = SP_level_spellpoint_cost(pl->ob, spell, SPELL_MANA);
    spell_info->last_grace =
        SP_level_spellpoint_cost(pl->ob, spell, SPELL_GRACE);
    spell_info->last_dam = spell->stats.dam+SP_level_dam_adjust(pl->ob, spell);
    /* send the current values */
    SockList_AddShort(sl, spell_info->last_sp);
    SockList_AddShort(sl, spell_info->last_grace);
    SockList_AddShort(sl, spell_info->last_dam);

    /* figure out which skill it uses, if it uses one */
    if (spell->skill) {
        for (i=1; i< NUM_SKILLS; i++)
            if (!strcmp(spell->skill, skill_names[i])) {
                skill = i+CS_STAT_SKILLINFO;
                break;
            }
    }
    SockList_AddChar(sl, skill);

    SockList_AddInt(sl, spell->path_attuned);
    SockList_AddInt(sl, (spell->face)?spell->face->number:0);
    SockList_AddLen8Data(sl, spell->name, strlen(spell->name));

    if (!spell->msg) {
        SockList_AddShort(sl, 0);
    } else {
        len = strlen(spell->msg);
        SockList_AddShort(sl, len);
        SockList_AddData(sl, spell->msg, len);
    }
}

/**
 * This tells the client to add the spell *spell, if spell is NULL, then add
 * all spells in the player's inventory.
 */
void esrv_add_spells(player *pl, object *spell) {
    SockList sl;
    if (!pl) {
        LOG(llevError, "esrv_add_spells, tried to add a spell to a NULL player\n");
        return;
    }
    if (!pl->socket.monitor_spells) return;
    SockList_Init(&sl);
    SockList_AddString(&sl, "addspell ");
    if (!spell) {
        for (spell=pl->ob->inv; spell!=NULL; spell=spell->below) {
            if (spell->type != SPELL) continue;
            /* Were we to simply keep appending data here, we could
             * exceed the SockList buffer if the player has enough spells
             * to add.  We know that append_spell will always append
             * 23 data bytes, plus 3 length bytes and 2 strings
             * (because that is the spec) so we need to check that
             * the length of those 2 strings, plus the 26 bytes,
             * won't take us over the length limit for the socket.
             * If it does, we need to send what we already have,
             * and restart packet formation.
             */
            if (SockList_Avail(&sl) < 26 + strlen(spell->name) + (spell->msg ? strlen(spell->msg) : 0)) {
                Send_With_Handling(&pl->socket, &sl);
                SockList_Reset(&sl);
                SockList_AddString(&sl, "addspell ");
            }
            append_spell(pl, &sl, spell);
        }
    } else if (spell->type != SPELL) {
        LOG(llevError, "Asked to send a non-spell object as a spell\n");
        return;
    } else append_spell(pl, &sl, spell);
    /* finally, we can send the packet */
    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}


/* sends a 'tick' information to the client.
 * We also take the opportunity to toggle TCP_NODELAY -
 * this forces the data in the socket to be flushed sooner to the
 * client - otherwise, the OS tries to wait for full packets
 * and will this hold sending the data for some amount of time,
 * which thus adds some additional latency.
 */
void send_tick(player *pl) {
    SockList sl;
    int tmp;

    SockList_Init(&sl);
    SockList_AddString(&sl, "tick ");
    SockList_AddInt(&sl, pticks);
    tmp = 1;
    if (setsockopt(pl->socket.fd, IPPROTO_TCP,TCP_NODELAY, &tmp, sizeof(tmp)))
        LOG(llevError,"send_tick: Unable to turn on TCP_NODELAY\n");

    Send_With_Handling(&pl->socket, &sl);
    tmp = 0;
    if (setsockopt(pl->socket.fd, IPPROTO_TCP,TCP_NODELAY, &tmp, sizeof(tmp)))
        LOG(llevError,"send_tick: Unable to turn off TCP_NODELAY\n");
    SockList_Term(&sl);
}

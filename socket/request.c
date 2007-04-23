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
short atnr_cs_stat[NROFATTACKS] = {CS_STAT_RES_PHYS, CS_STAT_RES_MAG,
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
void set_up_cmd(char *buf, int len, socket_struct *ns)
{
    int s, slen;
    char *cmd, *param, cmdback[HUGE_BUF];

        /* run through the cmds of setup
         * syntax is setup <cmdname1> <parameter> <cmdname2> <parameter> ...
         *
         * we send the status of the cmd back, or a FALSE is the cmd
         * is the server unknown
         * The client then must sort this out
         */

    LOG(llevInfo,"Get SetupCmd:: %s\n", buf);
    strcpy(cmdback,"setup");
    for(s=0;s<len; ) {

        cmd = &buf[s];

            /* find the next space, and put a null there */
        for(;buf[s] && buf[s] != ' ';s++) ;
        buf[s++]=0;
        while (buf[s] == ' ') s++;

        if(s>=len)
            break;

        param = &buf[s];

        for(;buf[s] && buf[s] != ' ';s++) ;
        buf[s++]=0;
        while (buf[s] == ' ') s++;
        
        slen = strlen(cmdback);
        safe_strcat(cmdback, " ", &slen, HUGE_BUF);
        safe_strcat(cmdback, cmd, &slen, HUGE_BUF);
        safe_strcat(cmdback, " ", &slen, HUGE_BUF);
    
        if (!strcmp(cmd,"sound")) {
            ns->sound = atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        }
        else if (!strcmp(cmd,"exp64")) {
            ns->exp64 = atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        } else if (!strcmp(cmd, "spellmon")) {
            ns->monitor_spells = atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        }  else if (!strcmp(cmd,"darkness")) {
            ns->darkness = atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"map1cmd")) {
            if (atoi(param)) ns->mapmode = Map1Cmd;
                /* if beyond this size, need to use map1cmd no matter what */
            if (ns->mapx>11 || ns->mapy>11) ns->mapmode = Map1Cmd;
            safe_strcat(cmdback, ns->mapmode == Map1Cmd?"1":"0",
                        &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"map1acmd")) {
            if (atoi(param)) ns->mapmode = Map1aCmd;
            safe_strcat(cmdback, ns->mapmode == Map1aCmd?"1":"0",
                        &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"map2cmd")) {
            if (atoi(param)) ns->mapmode = Map2Cmd;
            safe_strcat(cmdback, ns->mapmode == Map2Cmd?"1":"0",
                        &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"newmapcmd")) {
            ns->newmapcmd= atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"facecache")) {
            ns->facecache = atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"faceset")) {
            char tmpbuf[20];
            int q = atoi(param);

            if (is_valid_faceset(q))
                ns->faceset=q;
            sprintf(tmpbuf,"%d", ns->faceset);
            safe_strcat(cmdback, tmpbuf, &slen, HUGE_BUF);
                /* if the client is using faceset, it knows about
                 * image2 command
                 */
            ns->image2=1;
        } else if (!strcmp(cmd,"itemcmd")) {
                /* Version of the item protocol command to use.  Currently,
                 * only supported versions are 1 and 2.  Using a numeric
                 * value will make it very easy to extend this in the future.
                 */
            char tmpbuf[20];
            int q = atoi(param);
            if (q<1 || q>2) {
                strcpy(tmpbuf,"FALSE");
            } else  {
                ns->itemcmd = q;
                sprintf(tmpbuf,"%d", ns->itemcmd);
            }
            safe_strcat(cmdback, tmpbuf, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"mapsize")) {
            int x, y=0;
            char tmpbuf[MAX_BUF], *cp;

            x = atoi(param);
            for (cp = param; *cp!=0; cp++)
                if (*cp == 'x' || *cp == 'X') {
                    y = atoi(cp+1);
                    break;
                }
            if (x < 9 || y < 9 || x>MAP_CLIENT_X || y > MAP_CLIENT_Y) {
                sprintf(tmpbuf," %dx%d", MAP_CLIENT_X, MAP_CLIENT_Y);
                safe_strcat(cmdback, tmpbuf, &slen, HUGE_BUF);
            } else {
                ns->mapx = x;
                ns->mapy = y;
                    /* better to send back what we are really using and not the
                     * param as given to us in case it gets parsed differently.
                     */
                sprintf(tmpbuf,"%dx%d", x,y);
                safe_strcat(cmdback, tmpbuf, &slen, HUGE_BUF);
                    /* If beyond this size and still using orig map
                     * command, need to go to map1cmd.
                     */
                if ((x>11 || y>11) && ns->mapmode == Map0Cmd)
                    ns->mapmode = Map1Cmd;
            }
        } else if (!strcmp(cmd,"extendedMapInfos")) {
                /* Added by tchize
                 * prepare to use the mapextended command
                 */
            char tmpbuf[20];
            ns->ext_mapinfos = (atoi(param));
            sprintf(tmpbuf,"%d", ns->ext_mapinfos);
            safe_strcat(cmdback, tmpbuf, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"extendedTextInfos")) {
                /* Added by tchize
                 * prepare to use the extended text commands
                 * Client toggle this to non zero to get exttext
                 */
            char tmpbuf[20];

            ns->has_readable_type = (atoi(param));
            sprintf(tmpbuf,"%d", ns->has_readable_type);
            safe_strcat(cmdback, tmpbuf, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"tick")) {
            ns->tick = atoi(param);
            safe_strcat(cmdback, param, &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"bot")) {
            ns->is_bot = ( atoi(param) != 0 ? 1 : 0 );
            safe_strcat(cmdback, ns->is_bot ? "1" : "0", &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"want_pickup")) {
            ns->want_pickup = ( atoi(param) != 0 ? 1 : 0 );
            safe_strcat(cmdback, ns->want_pickup ? "1" : "0", &slen, HUGE_BUF);
        } else if (!strcmp(cmd,"inscribe")) {
            safe_strcat(cmdback, atoi(param) != 0 ? "1" : "0", &slen, HUGE_BUF);
        } else {
                /* Didn't get a setup command we understood -
                 * report a failure to the client.
                 */
            safe_strcat(cmdback, "FALSE", &slen, HUGE_BUF);
        }
    } /* for processing all the setup commands */
    LOG(llevInfo,"SendBack SetupCmd:: %s\n", cmdback);
    Write_String_To_Socket(ns, cmdback, strlen(cmdback));
}

/**
 * The client has requested to be added to the game.
 * This is what takes care of it.  We tell the client how things worked out.
 * I am not sure if this file is the best place for this function.  however,
 * it either has to be here or init_sockets needs to be exported.
 */
void add_me_cmd(char *buf, int len, socket_struct *ns)
{
    Settings oldsettings;
    oldsettings=settings;
    if (ns->status != Ns_Add || add_player(ns)) {
        Write_String_To_Socket(ns, "addme_failed",12);
    } else {
            /* Basically, the add_player copies the socket structure into
             * the player structure, so this one (which is from init_sockets)
             * is not needed anymore.  The write below should still work,
             * as the stuff in ns is still relevant.
             */
        Write_String_To_Socket(ns, "addme_success",13);
        if (ns->mapmode < Map1Cmd) {
                /* The space in the link isn't correct, but in my
                 * quick test with client 1.1.0, it didn't print it
                 * out correctly when done as a single line.
                 */
            char *buf= "drawinfo 3 Warning: Your client is too old to receive map data.  Please update to a new client at http://sourceforge.net/project/showfiles.php ?group_id=13833";
            Write_String_To_Socket(ns, buf, strlen(buf));
        }

        socket_info.nconns--;
        ns->status = Ns_Avail;
    }
    settings=oldsettings;   
}

/** Reply to ExtendedInfos command */
void toggle_extended_infos_cmd(char *buf, int len, socket_struct *ns)
{
    char cmdback[MAX_BUF];
    char command[50];
    int info,nextinfo;
    cmdback[0]='\0';     
    nextinfo=0;
    while (1){
            /* 1. Extract an info*/
        info=nextinfo;
        while ( (info<len) && (buf[info]==' ') ) info++;
        if (info>=len)
            break;
        nextinfo=info+1;
        while ( (nextinfo<len) && (buf[nextinfo]!=' ') )
            nextinfo++;  
        if (nextinfo-info>=49) /*Erroneous info asked*/
            continue;
        strncpy (command,&(buf[info]),nextinfo-info);
        command[nextinfo-info]='\0';
            /* 2. Interpret info*/
        if (!strcmp("smooth",command)){
                /* Toggle smoothing*/
            ns->EMI_smooth=!ns->EMI_smooth;
        }else{
                /*bad value*/
        }
            /*3. Next info*/
    }
    strcpy (cmdback,"ExtendedInfoSet");
    if (ns->EMI_smooth){
        strcat (cmdback," ");
        strcat (cmdback,"smoothing");
    }
    Write_String_To_Socket(ns, cmdback,strlen(cmdback));
}

/** Reply to ExtendedInfos command */
void toggle_extended_text_cmd(char *buf, int len, socket_struct *ns)
{
    char cmdback[MAX_BUF];
    char temp[10];
    char command[50];
    int info,nextinfo,i,flag;
    cmdback[0]='\0';     
    nextinfo=0;
    while (1){
            /* 1. Extract an info*/
        info=nextinfo;
        while ( (info<len) && (buf[info]==' ') ) info++;
        if (info>=len)
            break;
        nextinfo=info+1;
        while ( (nextinfo<len) && (buf[nextinfo]!=' ') )
            nextinfo++;  
        if (nextinfo-info>=49) /*Erroneous info asked*/
            continue;
        strncpy (command,&(buf[info]),nextinfo-info);
        command[nextinfo-info]='\0';
            /* 2. Interpret info*/
        i = sscanf(command,"%d",&flag);
        if ( (i==1) && (flag>0) && (flag<=MSG_TYPE_LAST))
            ns->supported_readables|=(1<<flag);
            /*3. Next info*/
    }
        /* Send resulting state */
    strcpy (cmdback,"ExtendedTextSet");
    for (i=0;i<=MSG_TYPE_LAST;i++)
        if (ns->supported_readables &(1<<i)){
            strcat (cmdback," ");
            snprintf(temp,sizeof(temp),"%d",i);
            strcat (cmdback,temp);
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
    uint8 reply[MAX_BUF];
    SockList sl;

        /* If we can't find a face, return and set it so we won't
         * try to send this again.
         */
    if ((!find_smooth(face, &smoothface)) &&
        (!find_smooth( smooth_face->number, &smoothface))) {

        LOG(llevError,"could not findsmooth for %d. Neither default (%s)\n",
            face,smooth_face->name);
        ns->faces_sent[face] |= NS_FACESENT_SMOOTH;
        return;
    }

    if (!(ns->faces_sent[smoothface] & NS_FACESENT_FACE))
        esrv_send_face(ns, smoothface, 0);

    ns->faces_sent[face] |= NS_FACESENT_SMOOTH;

    sl.buf=reply;
    strcpy((char*)sl.buf,"smooth ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddShort(&sl, face);
    SockList_AddShort(&sl, smoothface);
    Send_With_Handling(ns, &sl);
}

    /**
     * Tells client the picture it has to use 
     * to smooth a picture number given as argument.
     */
void ask_smooth_cmd(char *buf, int len, socket_struct *ns){
    uint16 facenbr;

    facenbr=atoi (buf);
    send_smooth(ns, facenbr);
}





/**
 * This handles the general commands from the client (ie, north, fire, cast,
 * etc.)
 */
void player_cmd(char *buf, int len, player *pl)
{

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
        /* Check if there is a count.  In theory, a zero count could also be
         * sent, so check for that also.
         */
    if (atoi(buf) || buf[0]=='0') {
        pl->count=atoi((char*)buf);
        buf=strchr(buf,' ');    /* advance beyond the numbers */
        if (!buf) {
#ifdef ESRV_DEBUG
            LOG(llevDebug,"player_cmd: Got count but no command.\n");
#endif
            return;
        }
        buf++;
    }
        /* This should not happen anymore.    */
    if (pl->ob->speed_left<-1.0) {
        LOG(llevError,"Player has negative time - shouldn't do command.\n");
    }
        /* In c_new.c */
    execute_newserver_command(pl->ob, (char*)buf);
        /* Perhaps something better should be done with a left over count.
         * Cleaning up the input should probably be done first - all actions
         * for the command that issued the count should be done before
         * any other commands.
         */

    pl->count=0;

}


/**
 * This handles the general commands from the client (ie, north, fire, cast,
 * etc.).  It is a lot like player_cmd above, but is called with the
 * 'ncom' method which gives more information back to the client so it
 * can throttle.
 */
void new_player_cmd(uint8 *buf, int len, player *pl)
{
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
    sl.buf = (uint8*)command;
    strcpy((char*)sl.buf,"comc ");
    sl.len=5;
    SockList_AddShort(&sl,packet);
    if (FABS(pl->ob->speed) < 0.001)
        time=MAX_TIME * 100;
    else
        time = ( int )( MAX_TIME/ FABS(pl->ob->speed) );
    SockList_AddInt(&sl,time);
    Send_With_Handling(&pl->socket, &sl);
}


/** This is a reply to a previous query. */
void reply_cmd(char *buf, int len, player *pl)
{
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

        case ST_CONFIGURE:
            LOG(llevError,
                "In client input handling, but into configure state\n");
            pl->state = ST_PLAYING;
            break;

        case ST_GET_NAME:
            receive_player_name(pl->ob,13);
            break;

        case ST_GET_PASSWORD:
        case ST_CONFIRM_PASSWORD:
        case ST_CHANGE_PASSWORD_OLD:
        case ST_CHANGE_PASSWORD_NEW:
        case ST_CHANGE_PASSWORD_CONFIRM:
            receive_player_password(pl->ob,13);
            break;

        case ST_GET_PARTY_PASSWORD:        /* Get password for party */
            receive_party_password(pl->ob,13);
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
void version_cmd(char *buf, int len,socket_struct *ns)
{
    char *cp;
    char version_warning[256];
        
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

            /* This is first implementation - i skip all beta DX clients
             * with it 
             * Add later stuff here for other clients 
             */

            /* these are old dxclients */
            /* Version 1024 added support for singular + plural name
             * values - requiing this minimal value reduces complexity
             * of that code, and it has been around for a long time.
             */
        if(!strcmp(" CF DX CLIENT", cp) || ns->sc_version < 1024 )
        {
            sprintf(version_warning,"drawinfo %d %s", NDI_RED,
                    "**** VERSION WARNING ****\n**** CLIENT IS TOO OLD!! UPDATE THE CLIENT!! ****");
            Write_String_To_Socket(ns, version_warning,
                                   strlen(version_warning));
        }

    }
}

/** sound related functions. */
 
void set_sound_cmd(char *buf, int len, socket_struct *ns)
{
    ns->sound = atoi(buf);
}

/** client wants the map resent 
 * @todo remove
*/

void map_redraw_cmd(char *buf, int len, player *pl)
{
/* This function is currently disabled; just clearing the
 * map state results in display errors. It should clear the
 * cache and send a newmap command.  Unfortunately this
 * solution does not work because some client versions send
 * a mapredraw command after receiving a newmap command.
 */
}

/** Newmap command */
void map_newmap_cmd( player *pl)
{
        /* If getting a newmap command, this scroll information
         * is no longer relevant.
         */
    if (pl->socket.mapmode == Map2Cmd) {
        pl->socket.map_scroll_x = 0;
        pl->socket.map_scroll_y = 0;
    }

    if( pl->socket.newmapcmd == 1) {
        memset(&pl->socket.lastmap, 0, sizeof(pl->socket.lastmap));
        Write_String_To_Socket( &pl->socket, "newmap", 6);
    }
}



/**
 * Moves an object (typically, container to inventory).
 * syntax is: move (to) (tag) (nrof)
 */
void move_cmd(char *buf, int len,player *pl)
{
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
void send_query(socket_struct *ns, uint8 flags, const char *text)
{
    char buf[MAX_BUF];

    sprintf(buf,"query %d %s", flags, text?text:"");
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
        SockList_AddChar(&sl, ( char )strlen(New));                     \
        strcpy((char*)sl.buf + sl.len, New);                            \
        sl.len += strlen(New);                                          \
    }

/**
 * Sends a statistics update.  We look at the old values,
 * and only send what has changed.  Stat mapping values are in newclient.h
 * Since this gets sent a lot, this is actually one of the few binary
 * commands for now.
 */
void esrv_update_stats(player *pl)
{
    SockList sl;
    char buf[MAX_BUF];
    uint16 flags;

    sl.buf=malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"stats ");
    sl.len=strlen((char*)sl.buf);
    
    if(pl->ob != NULL)
    {
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
    if(pl->socket.exp64) {
        uint8 s;
        for(s=0;s<NUM_SKILLS;s++) {
            if (pl->last_skill_ob[s] && 
                pl->last_skill_exp[s] != pl->last_skill_ob[s]->stats.exp) {

                    /* Always send along the level if exp changes.  This
                     * is only 1 extra byte, but keeps processing simpler.
                     */
                SockList_AddChar(&sl, ( char )( s + CS_STAT_SKILLINFO ));
                SockList_AddChar(&sl, ( char )pl->last_skill_ob[s]->level);
                SockList_AddInt64(&sl, pl->last_skill_ob[s]->stats.exp);
                pl->last_skill_exp[s] =  pl->last_skill_ob[s]->stats.exp;
            }
        }
    }
    if (pl->socket.exp64) {
        AddIfInt64(pl->last_stats.exp, pl->ob->stats.exp, CS_STAT_EXP64);
    } else {
        AddIfInt(pl->last_stats.exp, ( int )pl->ob->stats.exp, CS_STAT_EXP);
    }
    AddIfShort(pl->last_level, ( char )pl->ob->level, CS_STAT_LEVEL);
    AddIfShort(pl->last_stats.wc, pl->ob->stats.wc, CS_STAT_WC);
    AddIfShort(pl->last_stats.ac, pl->ob->stats.ac, CS_STAT_AC);
    AddIfShort(pl->last_stats.dam, pl->ob->stats.dam, CS_STAT_DAM);
    AddIfFloat(pl->last_speed, pl->ob->speed, CS_STAT_SPEED);
    AddIfShort(pl->last_stats.food, pl->ob->stats.food, CS_STAT_FOOD);
    AddIfFloat(pl->last_weapon_sp, pl->weapon_sp, CS_STAT_WEAP_SP);
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
                       ( char )atnr_cs_stat[i]);
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
    rangetostring(pl->ob, buf); 
    AddIfString(pl->socket.stats.range, buf, CS_STAT_RANGE);
    set_title(pl->ob, buf);
    AddIfString(pl->socket.stats.title, buf, CS_STAT_TITLE);

        /* Only send it away if we have some actual data */
    if (sl.len>6) {
#ifdef ESRV_DEBUG
        LOG(llevDebug,"Sending stats command, %d bytes long.\n", sl.len);
#endif
        Send_With_Handling(&pl->socket, &sl);
    }
    free(sl.buf);
}


/**
 * Tells the client that here is a player it should start using.
 */
void esrv_new_player(player *pl, uint32 weight)
{
    SockList    sl;

    pl->last_weight = weight;

    sl.buf=malloc(MAXSOCKSENDBUF);

    strcpy((char*)sl.buf,"player ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddInt(&sl, pl->ob->count);
    SockList_AddInt(&sl, weight);
    SockList_AddInt(&sl, pl->ob->face->number);

    SockList_AddChar(&sl, ( char )strlen(pl->ob->name));
    strcpy((char*)sl.buf+sl.len, pl->ob->name);
    sl.len += strlen(pl->ob->name);
       
    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
    SET_FLAG(pl->ob, FLAG_CLIENT_SENT);
}


/**
 * Need to send an animation sequence to the client.
 * We will send appropriate face commands to the client if we haven't
 * sent them the face yet (this can become quite costly in terms of
 * how much we are sending - on the other hand, this should only happen
 * when the player logs in and picks stuff up.
 */
void esrv_send_animation(socket_struct *ns, short anim_num)
{
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

    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf, "anim ");
    sl.len=5;
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
    free(sl.buf);
    ns->anims_sent[anim_num] = 1;
}


/****************************************************************************
 *
 * Start of map related commands.
 *
 ****************************************************************************/


/** Clears a map cell */
static void map_clearcell(struct map_cell_struct *cell, int face, int count)
{
    cell->darkness=count;
    memset(cell->faces, face, sizeof(cell->faces));
}

#define MAX_HEAD_POS    MAX(MAX_CLIENT_X, MAX_CLIENT_Y)

/* Using a global really isn't a good approach, but saves the over head of
 * allocating and deallocating such a block of data each time run through,
 * and saves the space of allocating this in the socket object when we only
 * need it for this cycle.  If the serve is ever threaded, this needs to be
 * re-examined.
 */

static object  *heads[MAX_HEAD_POS * MAX_HEAD_POS * MAP_LAYERS];

/**
 * Returns true if any of the heads for this
 * space is set.  Returns false if all are blank - this is used
 * for empty space checking.
 */
static inline int have_head(int ax, int ay) {

    if (heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS] ||
        heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS + 1] ||
        heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS + 2]) return 1;
    return 0;
}

/**
 * check_head is a bit simplistic version of update_space below.
 * basically, it only checks the that the head on space ax,ay at layer
 * needs to get sent - if so, it adds the data, sending the head
 * if needed, and returning 1.  If this no data needs to get
 * sent, it returns zero.
 */
static inline int check_head(SockList *sl, socket_struct *ns,
                             int ax, int ay, int layer)
{
    short face_num;

    if (heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS + layer])
        face_num =
            heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS + layer]->face->number;
    else
        face_num = 0;

    if (face_num != ns->lastmap.cells[ax][ay].faces[layer]) {
        SockList_AddShort(sl, face_num);
        if (face_num && !(ns->faces_sent[face_num] & NS_FACESENT_FACE))
            esrv_send_face(ns, face_num, 0);
        heads[(ay * MAX_HEAD_POS + ax) * MAP_LAYERS + layer] = NULL;
        ns->lastmap.cells[ax][ay].faces[layer] = face_num;
        return 1;
    }

    return 0;   /* No change */
}


/* This adds an object to the heads array.
 * sx are the map coordinates relative to the client/
 * the newmap/heads array.  ob is the object encountered
 * on sx, sy.
 * returns the face number to draw, 0 if nothing to
 * draw for this space.
 */
static uint16 add_head(object *ob, int sx, int sy, int p_layer)
{
    object *head;
    int bx, by, i;
    uint16 face_num=0;

    if (ob->head) head = ob->head;
    else head = ob;

        /* Basically figure out where the offset is from where we
         * are right now.  the ob->arch->clone.{x,y} values hold the
         * offset that this current piece is from the head, and the
         * tail is where the tail is from the head.  Note that bx and
         * by will equal sx and sy if we are already working on the
         * bottom right corner.  If ob is the head, the clone values
         * will be zero, so the right thing will still happen.
         */
    bx = sx + head->arch->tail_x - ob->arch->clone.x;
    by = sy + head->arch->tail_y - ob->arch->clone.y;

        /* I don't think this can ever happen, but better to check for
         * it just in case.
         */
    if (bx < sx || by < sy) {
        LOG(llevError,
            "add_head: bx (%d) or by (%d) is less than sx (%d) or sy (%d)\n",
            bx, by, sx, sy);
        face_num = 0;
    }
        /* single part object, multipart object with non merged faces,
         * of multipart object already at lower right.
         */
    else if (bx == sx && by == sy) {
        face_num = ob->face->number;

            /* if this face matches one stored away, clear that one away.
             * this code relies on the fact that the map1 commands
             * goes from 2 down to 0.
             */
        for (i=0; i<MAP_LAYERS; i++)
            if (heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + i] &&
                heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + i]->face->number == face_num)
                heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + i] = NULL;
    }
    else {
            /* If this head is stored away, clear it - otherwise,
             * there can be cases where a object is on multiple layers - 
             * we only want to send it once.
             */
        face_num = head->face->number;
        for (i=0; i<MAP_LAYERS; i++)
            if (heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + i] &&
                heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + i]->face->number == face_num)
                heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + i] = NULL;

            /* First, try to put the new head on the same layer.
             * If that is used up, then find another layer.
             */
        if (heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + p_layer] == NULL) {
            heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + p_layer] = head;
        } else for (i=0; i<MAP_LAYERS; i++) {
            if (heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + i] == NULL ||
                heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + i] == head) {
                heads[(by * MAX_HEAD_POS + bx) * MAP_LAYERS + i] = head;
            }
        }
        face_num = 0; /* Don't send this object - we'll send the head later */
    }
    return face_num;
}

/**
 * Removes the need to replicate the same code for each layer.
 * this returns true if this space is now in fact different than
 * it was.
 * sl is the socklist this data is going into.
 * ns is the socket we are working on - all the info we care
 * about is in this socket structure, so now need not pass the
 * entire player object.
 * mx and my are map coordinate offsets for map mp
 * sx and sy are the offsets into the socket structure that
 * holds the old values.
 * m_layer/p_layer is the layer to update.
 * m_layer is the layer as stored in the map array, and is what is
 *  used by GET_MAP_FACE_OBJ.
 * p_layer is the protocol layer, 0 by highest, 2 being floor.
 * With the map redo and map2 command, we now have 8 layers, but
 * the map1 protocol only supports 3.  We want to try and send
 * the data if possible, but this difference necessitates this mapping.
 * The caller will figure out what layer to use.
 */

static inline int update_space(SockList *sl, socket_struct *ns,
                               mapstruct  *mp, int mx, int my, 
                               int sx, int sy, int m_layer, int p_layer)
{
    object *ob, *head;
    uint16 face_num;
    int bx, i;

        /* If there is a multipart object stored away, treat that as
         * more important.  If not, then do the normal processing.
         */

    head = heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + p_layer];

        /* Check to see if this head is part of the set of objects
         * we would normally send for this space.  If so, then
         * don't use the head value.  We need to do the check
         * here and not when setting up the heads[] value for two reasons:
         * 1) the heads[] values will get used even if the space is not
         *    visible.
         * 2) its possible the head is not on the same map as a part, and
         *    I'd rather not need to do the map translation overhead.
         * 3) We need to do some extra checking to make sure that we will
         * otherwise send the image as this layer, eg, either it matches
         * the head value, or is not multipart.
         */

    if (head && !head->more) {
        for (i=0; i<MAP_LAYERS; i++) {
            ob = GET_MAP_FACE_OBJ(mp, mx, my, i);
            if (!ob) continue;

            if (ob->head) ob=ob->head;

            if (ob == head) {
                heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + p_layer] = NULL;
                head = NULL;
            }
        }
    }


    ob = head;
    if (!ob) ob = GET_MAP_FACE_OBJ(mp, mx, my, m_layer);

        /* If there is no object for this space, or if the face for the object
         * is the blank face, set the face number to zero.
         * else if we have the stored head object for this space, that takes
         * precedence over the other object for this space.
         * otherwise, we do special head processing
         */
    if (!ob || ob->face == blank_face) face_num=0;
    else if (head){
            /* if this is a head that had previously been stored */
        face_num = ob->face->number;
    } else {

            /* if the faces for the different parts of a multipart object
             * are the same, we only want to send the bottom right most
             * portion of the object.  That info is in the tail_.. values
             * of the head.  Note that for the head itself, ob->head will
             * be null, so we only do this block if we are working on
             * a tail piece.
             */

            /* tail_x and tail_y will only be set in the head object.  If
             * this is the head object and these are set, we proceed
             * with logic to only send bottom right.  Similarly, if
             * this is one of the more parts but the head has those values
             * set, we want to do the processing.  There can be cases where
             * the head is not visible but one of its parts is, so we just
             * can always expect that ob->arch->tail_x will be true for all
             * object we may want to display.
             */
        if ((ob->arch->tail_x || ob->arch->tail_y) ||
            (ob->head && (ob->head->arch->tail_x || ob->head->arch->tail_y))) {

            face_num=add_head(ob, sx, sy, p_layer);
        } else {
                /* In this case, we are already at the lower right or
                 * single part object, so nothing special 
                 */
            face_num = ob->face->number;

                /* clear out any head entries that have the same face
                 * as this one
                 */
            for (bx=0; bx<p_layer; bx++)
                if (heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + bx] &&
                    heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + bx]->face->number == face_num)
                    heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + bx] = NULL;
        }
    } /* else not already head object or blank face */

        /* This is a real hack.  Basically, if we have nothing to send
         * for this layer, but there is a head on the next layer, send
         * that instead.
         * Without this, what happens is you can get the case where the
         * player stands on the same space as the head.  However, if you
         * have overlapping big objects of the same type, what happens
         * then is it doesn't think it needs to send.
         * This tends to make stacking also work/look better.
         */
    if (!face_num && p_layer > 0 &&
        heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + p_layer -1]) {

        face_num = heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + p_layer -1]->face->number;
        heads[(sy * MAX_HEAD_POS + sx) * MAP_LAYERS + p_layer -1] = NULL;
    }

        /* Another hack - because of heads and whatnot, this face may
         * match one we already sent for a lower layer.  In that case,
         * don't send this one.
         */
    if (face_num && p_layer+1<MAP_LAYERS &&
        ns->lastmap.cells[sx][sy].faces[p_layer+1] == face_num) {
        face_num = 0;
    }

        /* We've gotten what face we want to use for the object.  Now see if
         * if it has changed since we last sent it to the client.
         */
    if (ns->lastmap.cells[sx][sy].faces[p_layer] != face_num)  {
        ns->lastmap.cells[sx][sy].faces[p_layer] = face_num;
        if (!(ns->faces_sent[face_num] & NS_FACESENT_FACE))
            esrv_send_face(ns, face_num, 0);
        SockList_AddShort(sl, face_num);
        return 1;
    }
        /* Nothing changed */
    return 0;
}

/**
 * This function is mainly a copy of update_space,
 * except it handles update of the smoothing updates,
 * not the face updates.
 * Removes the need to replicate the same code for each layer.
 * this returns true if this smooth is now in fact different
 * than it was.
 * sl is the socklist this data is going into.
 * ns is the socket we are working on - all the info we care
 * about is in this socket structure, so know need to pass the
 * entire player object.
 * mx and my are map coordinate offsets for map mp
 * sx and sy are the offsets into the socket structure that
 * holds the old values.
 * layer is the layer to update, with 2 being the floor and 0 the
 * top layer (this matches what the GET_MAP_FACE and GET_MAP_FACE_OBJ
 * take.  
 */
static inline int update_smooth(SockList *sl, socket_struct *ns,
                                mapstruct  *mp, int mx, int my, 
                                int sx, int sy, int m_layer, int p_layer)
{
    object *ob;
    int smoothlevel; /* old face_num;*/

    ob = GET_MAP_FACE_OBJ(mp, mx, my, m_layer);

        /* If there is no object for this space, or if the face for the object
         * is the blank face, set the smoothlevel to zero.
         */
    if (!ob || ob->face == blank_face || MAP_NOSMOOTH(mp)) smoothlevel=0;
    else {
        smoothlevel = ob->smoothlevel;
        if (smoothlevel &&
            !(ns->faces_sent[ob->face->number] & NS_FACESENT_SMOOTH))
            send_smooth(ns, ob->face->number);
    } /* else not already head object or blank face */

        /* We've gotten what face we want to use for the object.  Now see if
         * if it has changed since we last sent it to the client.
         */
    if (smoothlevel>255)
        smoothlevel=255;
    else if (smoothlevel<0)
        smoothlevel=0;
    if (ns->lastmap.cells[sx][sy].smooth[p_layer] != smoothlevel)  {
        ns->lastmap.cells[sx][sy].smooth[p_layer] = smoothlevel;
        SockList_AddChar(sl, (uint8) (smoothlevel&0xFF));
        return 1;
    }
        /* Nothing changed */
    return 0;
}

/**
 * Returns the size of a data for a map square as returned by
 * mapextended. There are CLIENTMAPX*CLIENTMAPY*LAYERS entries
 * available.
 */
static int get_extended_mapinfo_size(socket_struct* ns){
    int result=0;
    if (ns->ext_mapinfos){
        if (ns->EMI_smooth)
            result+=1; /*One byte for smoothlevel*/
    }
    return result;
}

/**
 * This function uses the new map1 protocol command to send the map
 * to the client.  It is necessary because the old map command supports
 * a maximum map size of 15x15.
 * This function is much simpler than the old one.  This is because
 * the old function optimized to send as few face identifiers as possible,
 * at the expense of sending more coordinate location (coordinates were
 * only 1 byte, faces 2 bytes, so this was a worthwhile savings).  Since
 * we need 2 bytes for coordinates and 2 bytes for faces, such a trade off
 * maps no sense.  Instead, we actually really only use 12 bits for
 * coordinates, and use the other 4 bits for other informatiion.   For
 * full documentation of what we send, see the doc/Protocol file.
 * I will describe internally what we do:
 * the socket->lastmap shows how the map last looked when sent to the client.
 * in the lastmap structure, there is a cells array, which is set to the
 * maximum viewable size (As set in config.h).
 * in the cells, there are faces and a count value.
 * we use the count value to hold the darkness value.  If -1, then this space
 *   is not viewable.
 * we use faces[0] faces[1] faces[2] to hold what the three layers
 * look like.
 */

void draw_client_map1(object *pl)
{
    int x,y,ax, ay, d, startlen, max_x, max_y, oldlen;
    sint16 nx, ny;
    int estartlen, eoldlen, b_layer, m_layer, t_layer, o_layer,n_layer, face;
    SockList sl, esl; /*For extended Map info*/
    uint16  mask,emask, ewhatstart,ewhatflag;
    uint8 eentrysize, extendedinfos;
    mapstruct *m;
    object *m_ob, *t_ob, *ob;

    sl.buf=malloc(MAXSOCKSENDBUF);
    if (pl->contr->socket.mapmode == Map1Cmd)
        strcpy((char*)sl.buf,"map1 ");
    else
        strcpy((char*)sl.buf,"map1a ");
    sl.len=strlen((char*)sl.buf);
    startlen = sl.len;
        /*Extendedmapinfo structure initialisation*/
    if (pl->contr->socket.ext_mapinfos){
        esl.buf=malloc(MAXSOCKSENDBUF);
        strcpy((char*)esl.buf,"mapextended ");
        esl.len=strlen((char*)esl.buf);
        extendedinfos=EMI_NOREDRAW;
        if (pl->contr->socket.EMI_smooth)
            extendedinfos|=EMI_SMOOTH;
        ewhatstart=esl.len;
        ewhatflag=extendedinfos; /*The EMI_NOREDRAW bit
                                   could need to be taken away*/
        SockList_AddChar(&esl, extendedinfos);
        eentrysize=get_extended_mapinfo_size(&(pl->contr->socket));
        SockList_AddChar(&esl, eentrysize);
        estartlen = esl.len;
    } else {
            /* suppress compiler warnings */
        ewhatstart = 0;
        ewhatflag = 0;
        estartlen = 0;
    }
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
    max_x = pl->x+(pl->contr->socket.mapx+1)/2;
    max_y = pl->y+(pl->contr->socket.mapy+1)/2;
    if (pl->contr->socket.mapmode == Map1aCmd) {
        max_x += MAX_HEAD_OFFSET;
        max_y += MAX_HEAD_OFFSET;
    }

    for(y=pl->y-pl->contr->socket.mapy/2; y<max_y; y++,ay++) {
        ax=0;
        for(x=pl->x-pl->contr->socket.mapx/2;x<max_x;x++,ax++) {

            mask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
            emask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;

                /* If this space is out of the normal viewable area, we
                 * only check the heads value  ax or ay will only be
                 * greater than what the client wants if using the
                 * map1a command - this is because if the map1a command
                 * is not used, max_x and max_y will be set to lower values.
                 */
            if (ax >= pl->contr->socket.mapx || ay >= pl->contr->socket.mapy) {
                int i, got_one;

                oldlen = sl.len;


                SockList_AddShort(&sl, mask);

                if (check_head(&sl, &pl->contr->socket, ax, ay, 2))
                    mask |= 0x4;
                if (check_head(&sl, &pl->contr->socket, ax, ay, 1))
                    mask |= 0x2;
                if (check_head(&sl, &pl->contr->socket, ax, ay, 0))
                    mask |= 0x1;

                    /* If all we are doing is sending 0 (blank) faces, we don't
                     * actually need to send that - just the coordinates
                     * with no faces tells the client to blank out the
                     * space.
                     */
                got_one=0;
                for (i=oldlen+2; i<sl.len; i++) {
                    if (sl.buf[i]) got_one=1;
                }
                if (got_one && (mask & 0xf)) {
                    sl.buf[oldlen+1] = mask & 0xff;
                } else { /*either all faces blank, either no face at all*/
                    /*at least 1 face, we know it's blank, only send
                     * coordinates
                     */
                    if (mask & 0xf)
                        sl.len = oldlen + 2;
                    else
                        sl.len = oldlen;
                }
                    /* What concerns extendinfos, nothing to be done for now
                     * (perhaps effects layer later)
                     */
                continue;   /* don't do processing below */
            }

            d =  pl->contr->blocked_los[ax][ay];

                /* If the coordinates are not valid, or it is too dark to see,
                 * we tell the client as such
                 */
            nx=x;
            ny=y;
            m = get_map_from_coord(pl->map, &nx, &ny);
            if (!m) {
                    /* space is out of map.  Update space and clear values
                     * if this hasn't already been done.  If the space is out
                     * of the map, it shouldn't have a head
                     */
                if (pl->contr->socket.lastmap.cells[ax][ay].darkness != -1) {
                    SockList_AddShort(&sl, mask);
                    map_clearcell(&pl->contr->socket.lastmap.cells[ax][ay],
                                  0,-1);
                }
            } else if (d>3) {
                int need_send=0, count;
                    /* This block deals with spaces that are not visible
                     * for whatever reason.  Still may need to send the
                     * head for this space.
                     */

                oldlen = sl.len;

                SockList_AddShort(&sl, mask);
                if (pl->contr->socket.lastmap.cells[ax][ay].darkness != -1)
                    need_send=1;
                count = -1;

                if (pl->contr->socket.mapmode == Map1aCmd &&
                    have_head(ax, ay)) {
                        /* Now check to see if any heads need to be sent */

                    if (check_head(&sl, &pl->contr->socket, ax, ay, 2))
                        mask |= 0x4;
                    if (check_head(&sl, &pl->contr->socket, ax, ay, 1))
                        mask |= 0x2;
                    if (check_head(&sl, &pl->contr->socket, ax, ay, 0))
                        mask |= 0x1;
                    pl->contr->socket.lastmap.cells[ax][ay].darkness = count;

                } else {
                    struct map_cell_struct *cell =
                        &pl->contr->socket.lastmap.cells[ax][ay];
                        /* properly clear a previously sent big face */
                    if(cell->faces[0] != 0
                       || cell->faces[1] != 0
                       || cell->faces[2] != 0)
                        need_send = 1;
                    map_clearcell(&pl->contr->socket.lastmap.cells[ax][ay],
                                  0, count);
                }

                if ((mask & 0xf) || need_send) {
                    sl.buf[oldlen+1] = mask & 0xff;
                } else {
                    sl.len = oldlen;
                }
            } else {
                    /* In this block, the space is visible or there are
                     * head objects we need to send.
                     */

                    /* Rather than try to figure out what everything
                     * that we might need to send is, then form the
                     * packet after that, we presume that we will in
                     * fact form a packet, and update the bits by what
                     * we do actually send.  If we send nothing, we just
                     * back out sl.len to the old value, and no harm
                     * is done.
                     * * I think this is simpler than doing a bunch of
                     * checks to see what if anything we need to send,
                     * setting the bits, then doing those checks again
                     * to add the real data.
                     */
                oldlen = sl.len;
                mask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
                eoldlen = esl.len;
                emask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
                SockList_AddShort(&sl, mask);

                if (pl->contr->socket.ext_mapinfos)
                    SockList_AddShort(&esl, emask);

                    /* Darkness changed */
                if (pl->contr->socket.lastmap.cells[ax][ay].darkness != d &&
                    pl->contr->socket.darkness) {
                    pl->contr->socket.lastmap.cells[ax][ay].darkness = d;
                    mask |= 0x8;    /* darkness bit */

                        /* Protocol defines 255 full bright, 0 full dark.
                         * We currently don't have that many darkness ranges,
                         * so we current what limited values we do have.
                         */
                    if (d==0) SockList_AddChar(&sl, 255);
                    else if (d==1) SockList_AddChar(&sl, 191);
                    else if (d==2) SockList_AddChar(&sl, 127);
                    else if (d==3) SockList_AddChar(&sl, 63);
                }
                else {
                        /* need to reset from -1 so that if it does become
                         * blocked again, the code that deals with that
                         * can detect that it needs to tell the client
                         * that this space is now blocked.
                         */
                    pl->contr->socket.lastmap.cells[ax][ay].darkness = d;
                }
                b_layer = 0;
                for (o_layer=0; o_layer < MAP_LAYERS; o_layer++) {
                    if (GET_MAP_FACE_OBJ(m, nx, ny, o_layer)) {
                        b_layer = o_layer;
                        break;
                    }
                }
                    /* Floor face */
                if (update_space(&sl, &pl->contr->socket,
                                 m, nx, ny, ax, ay, b_layer, 2))
                    mask |= 0x4;

                if (pl->contr->socket.EMI_smooth)
                    if (update_smooth(&esl, &pl->contr->socket,
                                      m, nx, ny, ax, ay, b_layer, 2)){
                        emask |= 0x4;
                    }

                m_layer=0;
                t_layer=0;
                n_layer=0;
                    /* o_layer is set above */
                for (; o_layer < MAP_LAYERS; o_layer++) {
                    if ((ob=GET_MAP_FACE_OBJ(m, nx, ny, o_layer))!=NULL) {
                        if (((ob->arch->tail_x || ob->arch->tail_y) ||
                             (ob->head &&
                              (ob->head->arch->tail_x ||
                               ob->head->arch->tail_y))) &&
                            ob->more) {

                            face = add_head(ob, ax, ay, 1);
                            if (!face) continue;
                        } else if (!m_layer) {
                            m_layer = o_layer;
                            m_ob = ob;
                        } else if (!t_layer) {
                            t_layer = o_layer;
                            t_ob = ob;
                        } else {    /* Both layers are full */
                                /* We want to take this object if it has
                                 * a higher visibility.  But we also want
                                 * to keep the stacking the same.  We
                                 * replace the object with the lower
                                 * visibility.  If replace what is currently
                                 * the middle object, we push the top object
                                 * to the middle, and new object becomes the
                                 * top.
                                 */
                            if (m_ob->face->visibility >=
                                t_ob->face->visibility) {
                                if (ob->face->visibility >=
                                    t_ob->face->visibility) {
                                    if (t_ob->type == PLAYER) {
                                        m_ob = t_ob;
                                        m_layer = t_layer;  
                                    }
                                    t_ob = ob;
                                    t_layer = o_layer;
                                }
                            } else if (ob->face->visibility >=
                                       m_ob->face->visibility) {
                                if (m_ob->type != PLAYER) {
                                    m_ob = t_ob;
                                    m_layer = t_layer;
                                }
                                t_ob = ob;
                                t_layer = o_layer;
                            }
                        }
                    } else {
                            /* this is a layer that doesn't have any face - 
                             * with the new stacking code, we can't ever be
                             * sure what layer doesn't have a face, yet
                             * we need to pass this in to update_space to
                             * it clears out old entries.
                             */
                        n_layer = o_layer;
                    }
                }
                    /* If the top layer isn't set it, set it to the middle
                     * layer, and clear the middle layer.  This makes drawing
                     * work better relating to stacking of big images and
                     * players & monsters.
                     */
                if (!t_layer) {
                    if (m_layer) {
                        t_layer = m_layer;
                        m_layer = n_layer;
                    } else
                        t_layer = n_layer;
                }
                if (!m_layer) m_layer = n_layer;


                    /* Middle face */
                if (update_space(&sl, &pl->contr->socket,
                                 m, nx, ny, ax, ay, m_layer, 1))
                    mask |= 0x2;

                if (pl->contr->socket.EMI_smooth && 
                    update_smooth(&esl, &pl->contr->socket,
                                  m, nx, ny, ax, ay, m_layer, 1))
                    emask |= 0x2;
        


                    /* Special logic to always send player */
                if(nx == pl->x && ny == pl->y &&
                   pl->invisible & (pl->invisible < 50 ? 4 : 1)) {
                    if (pl->contr->socket.lastmap.cells[ax][ay].faces[0] !=
                        pl->face->number) {
                        pl->contr->socket.lastmap.cells[ax][ay].faces[0] =
                            pl->face->number;
                        mask |= 0x1;
                        if (!(pl->contr->socket.faces_sent[pl->face->number] &
                              NS_FACESENT_FACE))
                            esrv_send_face(&pl->contr->socket,
                                           pl->face->number, 0);
                        SockList_AddShort(&sl, pl->face->number);
                    }
                }
                    /* Top face */
                else {
                    if (update_space(&sl, &pl->contr->socket,
                                     m, nx, ny, ax, ay, t_layer, 0))
                        mask |= 0x1;
                    if (pl->contr->socket.EMI_smooth && 
                        update_smooth(&esl, &pl->contr->socket,
                                      m, nx, ny, ax, ay, t_layer, 0)){
                        emask |= 0x1;
                    }
                }

                    /* Check to see if we are in fact sending anything
                     * for this space by checking the mask.  If so,
                     * update the mask.  If not, reset the len to that
                     * from before adding the mask value, so we don't
                     * send those bits.
                     */
                if (mask & 0xf) {
                    sl.buf[oldlen+1] = mask & 0xff;
                } else {
                    sl.len = oldlen;
                }
                if (emask & 0xf) {
                    esl.buf[eoldlen+1] = emask & 0xff;
                } else {
                    esl.len = eoldlen;
                }
            } /* else this is a viewable space */
        } /* for x loop */
    } /* for y loop */

        /* Verify that we in fact do need to send this */
    if (pl->contr->socket.ext_mapinfos){
        if (!(sl.len>startlen || pl->contr->socket.sent_scroll)){
                /* No map data will follow, so don't say the client
                 * it doesn't need draw!
                 */
            ewhatflag&=(~EMI_NOREDRAW);
            esl.buf[ewhatstart+1] = ewhatflag & 0xff;
        }
        if (esl.len>estartlen) {
            Send_With_Handling(&pl->contr->socket, &esl);
        }
    }
    if (sl.len>startlen || pl->contr->socket.sent_scroll) {
        Send_With_Handling(&pl->contr->socket, &sl);
        pl->contr->socket.sent_scroll = 0;
    }
    if (pl->contr->socket.ext_mapinfos){
        free(esl.buf);
    }
    free(sl.buf);
}

/****************************************************************************
 * This block is for map2 drawing related commands.
 * Note that the map2 still uses other functions.
 *
 ***************************************************************************/

/* object 'ob' at 'ax,ay' on 'layer' is visible to the client.
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
                       socket_struct *ns, int *has_obj, int is_head)
{
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
                if (FABS(ob->speed)<0.004) anim_speed=255;
                else if (FABS(ob->speed)>=1.0) anim_speed=1;
                else anim_speed = (int) (1.0/FABS(ob->speed));
                if (!ns->anims_sent[ob->animation_id])
                    esrv_send_animation(ns, ob->animation_id);

                    /* If smoothing, need to send smoothing information
                     * for all faces in the animation sequence. Since
                     * smoothlevel is an object attribute,
                     * it applies to all faces.
                     */
                if (smoothlevel) {
                    for (i=0; i < NUM_ANIMATIONS(ob); i++) {
                        if(!(ns->faces_sent[
                                 animations[ob->animation_id].faces[i]] &
                             NS_FACESENT_SMOOTH))
                            send_smooth(ns,
                                        animations[ob->animation_id].faces[i]);
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
                             SockList *sl, socket_struct *ns)
{
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
                                  SockList *sl, socket_struct *ns)
{
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
    }
    else if (del_one && !has_obj) {
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

void draw_client_map2(object *pl)
{
    int x,y,ax, ay, d, startlen, max_x, max_y, oldlen, layer;
    sint16 nx, ny;
    SockList sl;
    uint16  coord;
    mapstruct *m;
    object *ob;

    sl.buf=malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"map2 ");
    sl.len=strlen((char*)sl.buf);
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

    for(y=(pl->y - pl->contr->socket.mapy / 2); y< max_y; y++, ay++) {
        ax=0;
        for(x=(pl->x - pl->contr->socket.mapx / 2); x< max_x ; x++,ax++) {

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
                    if (pl->contr->socket.lastmap.cells[ax][ay].darkness !=
                        d && pl->contr->socket.darkness) {
                        pl->contr->socket.lastmap.cells[ax][ay].darkness = d;
                            /* Darkness tag & length*/
                        SockList_AddChar(&sl, 0x1 | 1 << 5);    
                        SockList_AddChar(&sl, 255 - d * (256/MAX_LIGHT_RADII));
                        have_darkness = 1;
                    }

                    for (layer=0; layer < MAP_LAYERS; layer++) {
                        ob = GET_MAP_FACE_OBJ(m, nx, ny, layer);
            
                            /* Special case: send player itself if invisible */
                        if ( !ob && x == pl->x && y == pl->y &&
                             ( pl->invisible & (pl->invisible < 50 ? 4:1) ) &&
                             ( layer == MAP_LAYER_LIVING1 ||
                               layer == MAP_LAYER_LIVING2 ) )
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
                    }
                    else if (del_one && !has_obj) {
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
        pl->contr->socket.sent_scroll = 0;
    }
    free(sl.buf);
}



/**
 * Draws client map.
 */
void draw_client_map(object *pl)
{
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
    }
    else
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
    for(j = (pl->y - pl->contr->socket.mapy/2) ;
        j < (pl->y + (pl->contr->socket.mapy+1)/2); j++) {
        for(i = (pl->x - pl->contr->socket.mapx/2) ;
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
    if(pl->contr->do_los) {
        update_los(pl);
        pl->contr->do_los = 0;
    }

        /* Big maps need a different drawing mechanism to work */
    if (pl->contr->socket.mapmode == Map2Cmd)  {
        draw_client_map2(pl);
        return;
    } else if (pl->contr->socket.mapmode == Map1Cmd ||
               pl->contr->socket.mapmode == Map1aCmd) {
        draw_client_map1(pl);
    }
        /* Original map draw code used to be here */
}


void esrv_map_scroll(socket_struct *ns,int dx,int dy)
{
    struct Map newmap;
    int x,y, mx, my;
    char buf[MAXSOCKSENDBUF];

    if (ns->mapmode == Map2Cmd) {
        ns->map_scroll_x += dx;
        ns->map_scroll_y += dy;
    } else {
        sprintf(buf,"map_scroll %d %d", dx, dy);
        Write_String_To_Socket(ns, buf, strlen(buf));
    }

        /* If we are using the Map1aCmd, we may in fact send
         * head information that is outside the viewable map.
         * So set the mx,my to the max value we want to
         * look for.  Removed code to do so - it caused extra
         * complexities for the client, and probably doesn't make
         * that much difference in bandwidth.
         */
    mx = ns->mapx;
    my = ns->mapy;

    if (ns->mapmode >= Map1aCmd) {
        mx += MAX_HEAD_OFFSET;
        my += MAX_HEAD_OFFSET;
    }

        /* the x and y here are coordinates for the new map, i.e. if we moved
         * (dx,dy), newmap[x][y] = oldmap[x-dx][y-dy].  For this reason,
         * if the destination x or y coordinate is outside the viewable
         * area, we clear the values - otherwise, the old values
         * are preserved, and the check_head thinks it needs to clear them.
         */
    for(x=0; x<mx; x++) {
        for(y=0; y<my; y++) {
            if(x >= ns->mapx || y >= ns->mapy) {
                    /* clear cells outside the viewable area */
                memset(&newmap.cells[x][y], 0, sizeof(struct map_cell_struct));
            }
            else if ((x+dx) < 0 || (x+dx) >= ns->mapx ||
                     (y+dy) < 0 || (y + dy) >= ns->mapy) {
                    /* clear newly visible tiles within the viewable area */
                memset(&(newmap.cells[x][y]), 0,
                       sizeof(struct map_cell_struct));
            }
            else {
                memcpy(&(newmap.cells[x][y]),
                       &(ns->lastmap.cells[x+dx][y+dy]),
                       sizeof(struct map_cell_struct));
            }
        }
    }

    memcpy(&(ns->lastmap), &newmap,sizeof(struct Map));

        /* Make sure that the next "map1" command will be sent (even if
         * it is empty).
         */
    ns->sent_scroll = 1;
}

/**
 * GROS: The following one is used to allow a plugin to send a generic cmd to
 * a player. Of course, the client need to know the command to be able to
 * manage it !
 */
void send_plugin_custom_message(object *pl, char *buf)
{
    cs_write_string(&pl->contr->socket,buf,strlen(buf));
}

/**
 * This sends the experience table the sever is using
 */
void send_exp_table(socket_struct *ns, char *params)
{
    SockList sl;
    int i;
    extern sint64 *levels;

    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"replyinfo exp_table\n");
    sl.len = strlen((char*)sl.buf);
    SockList_AddShort(&sl, settings.max_level+1);
    for (i=1; i<= settings.max_level; i++) {
        if (sl.len+8 > MAXSOCKSENDBUF) {
            LOG(llevError, "Buffer overflow in send_exp_table, not sending all information\n");
            break;
        }
        SockList_AddInt64(&sl, levels[i]);
    }
    Send_With_Handling(ns, &sl);
    free(sl.buf);
}

/**
 * This sends the skill number to name mapping.  We ignore
 * the params - we always send the same info no matter what.
 */
void send_skill_info(socket_struct *ns, char *params)
{
    SockList sl;
    int i;

    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"replyinfo skill_info\n");
    sl.len = strlen((char*)sl.buf);
    for (i=1; i< NUM_SKILLS; i++) {
        int len;

        len = 16+strlen(skill_names[i]); /* upper bound for length */
        if (sl.len+len > MAXSOCKSENDBUF) {
            LOG(llevError, "Buffer overflow in send_skill_info, not sending all skill information\n");
            break;
        }

        sprintf((char*)sl.buf+sl.len, "%d:%s\n",
                i+CS_STAT_SKILLINFO, skill_names[i]);
        sl.len += strlen((char*)sl.buf+sl.len);
    }
    Send_With_Handling(ns, &sl);
    free(sl.buf);
}

/**
 * This sends the spell path to name mapping.  We ignore
 * the params - we always send the same info no matter what.
 */
void send_spell_paths (socket_struct *ns, char *params) {
    SockList sl;
    int i;

    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"replyinfo spell_paths\n");
    sl.len = strlen((char*)sl.buf);
    for(i=0; i<NRSPELLPATHS; i++) {
        int len;

        len = 16+strlen(spellpathnames[i]); /* upper bound for length */
        if (sl.len+len > MAXSOCKSENDBUF) {
            LOG(llevError, "Buffer overflow in send_spell_paths, not sending all spell information\n");
            break;
        }

        sprintf((char*)sl.buf+sl.len, "%d:%s\n", 1<<i, spellpathnames[i]);
        sl.len += strlen((char*)sl.buf+sl.len);
    }
    Send_With_Handling(ns, &sl);
    free(sl.buf);
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
    client_spell* spell_info;
    if (!pl->socket.monitor_spells) return;
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
                sl.buf = malloc(MAXSOCKSENDBUF);
                strcpy((char*)sl.buf,"updspell ");
                sl.len=strlen((char*)sl.buf);
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
                free(sl.buf);
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
    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"delspell ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddInt(&sl, spell->count);
    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
}

/**
 * Sends the "pickup" state to pl if client wants it requested.
 *
 * @param pl
 * player that just logged in.
 */
void esrv_send_pickup(player* pl) {
    SockList sl;
    if (!pl->socket.want_pickup)
        return;
    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"pickup ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddInt(&sl, pl->mode);
    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
}

/** appends the spell *spell to the Socklist we will send the data to. */
static void append_spell (player *pl, SockList *sl, object *spell) {
    client_spell* spell_info;
    int len, i, skill=0; 

    if (!(spell->name)) {
        LOG(llevError, "item number %d is a spell with no name.\n",
            spell->count);
        return;
    }
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

    len = strlen(spell->name);
    SockList_AddChar(sl, (char)len);
    memcpy(sl->buf+sl->len, spell->name, len);
    sl->len+=len;

    if (!spell->msg) {
        SockList_AddShort(sl, 0);
    }
    else {
        len = strlen(spell->msg);
        SockList_AddShort(sl, len);
        memcpy(sl->buf+sl->len, spell->msg, len);
        sl->len+=len;
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
    sl.buf = malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"addspell ");
    sl.len=strlen((char*)sl.buf);
    if (!spell) {
        for (spell=pl->ob->inv; spell!=NULL; spell=spell->below) {
            if (spell->type != SPELL) continue;
                /* Were we to simply keep appending data here, we could
                 * exceed MAXSOCKSENDBUF if the player has enough spells
                 * to add.  We know that append_spell will always append
                 * 23 data bytes, plus 3 length bytes and 2 strings
                 * (because that is the spec) so we need to check that
                 * the length of those 2 strings, plus the 26 bytes,
                 * won't take us over the length limit for the socket.
                 * If it does, we need to send what we already have,
                 * and restart packet formation.
                 */
            if (sl.len >
                (MAXSOCKSENDBUF - (26 + strlen(spell->name) + 
                                   (spell->msg?strlen(spell->msg):0)))) {
                Send_With_Handling(&pl->socket, &sl);
                strcpy((char*)sl.buf,"addspell ");
                sl.len=strlen((char*)sl.buf);
            }
            append_spell(pl, &sl, spell);
        }
    }
    else if (spell->type != SPELL) {
        LOG(llevError, "Asked to send a non-spell object as a spell\n");
        return;
    }
    else append_spell(pl, &sl, spell);
    if (sl.len > MAXSOCKSENDBUF) {
        LOG(llevError,"Buffer overflow in esrv_add_spells!\n");
        fatal(0);
    }
        /* finally, we can send the packet */
    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
}


/* sends a 'tick' information to the client.
 * We also take the opportunity to toggle TCP_NODELAY - 
 * this forces the data in the socket to be flushed sooner to the
 * client - otherwise, the OS tries to wait for full packets
 * and will this hold sending the data for some amount of time,
 * which thus adds some additional latency.
 */
void send_tick(player *pl)
{
    SockList sl;
    int tmp;

    sl.buf=malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf,"tick ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddInt(&sl, pticks);
    tmp = 1;
    if (setsockopt(pl->socket.fd, IPPROTO_TCP,TCP_NODELAY, &tmp, sizeof(tmp))) 
        LOG(llevError,"send_tick: Unable to turn on TCP_NODELAY\n");

    Send_With_Handling(&pl->socket, &sl);
    tmp = 0;
    if (setsockopt(pl->socket.fd, IPPROTO_TCP,TCP_NODELAY, &tmp, sizeof(tmp))) 
        LOG(llevError,"send_tick: Unable to turn off TCP_NODELAY\n");
    free(sl.buf);
}

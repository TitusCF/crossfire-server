/*
 * static char *rcsid_init_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel
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

/*
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
 * mapcellchanged, compactlayer, compactstack, perform the map compressing
 * operations
 *
 * esrv_map_doneredraw finishes the map update, and ships across the
 * map updates. 
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
#include <netdb.h>
#endif /* win32 */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "sounds.h"

/* This table translates the attack numbers as used within the
 * program to the value we use when sending STATS command to the
 * client.  IF a value is -1, then we don't send that to the
 * client.
 */
short atnr_cs_stat[NROFATTACKS] = {CS_STAT_RES_PHYS,
    CS_STAT_RES_MAG,CS_STAT_RES_FIRE, CS_STAT_RES_ELEC,
    CS_STAT_RES_COLD, CS_STAT_RES_CONF, CS_STAT_RES_ACID,
    CS_STAT_RES_DRAIN, -1 /* weaponmagic */,
    CS_STAT_RES_GHOSTHIT, CS_STAT_RES_POISON, 
    CS_STAT_RES_SLOW, CS_STAT_RES_PARA, CS_STAT_TURN_UNDEAD,
    CS_STAT_RES_FEAR, -1 /* Cancellation */,
    CS_STAT_RES_DEPLETE, CS_STAT_RES_DEATH,
    -1 /* Chaos */, -1 /* Counterspell */,
    -1 /* Godpower */, CS_STAT_RES_HOLYWORD,
    CS_STAT_RES_BLIND, 
    -1, /* Internal */
    -1, /* life stealing */
    -1 /* Disease - not fully done yet */
};

/* This is the Setup cmd - easy first implementation */
void SetUp(char *buf, int len, NewSocket *ns)
{
    int s;
    char *cmd, *param, cmdback[HUGE_BUF];

    /* run through the cmds of setup
     * syntax is setup <cmdname1> <parameter> <cmdname2> <parameter> ...
     *
     * we send the status of the cmd back, or a FALSE is the cmd is the server unknown
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
		
	strcat(cmdback, " ");
	strcat(cmdback, cmd);
	strcat(cmdback, " ");

	
	if (!strcmp(cmd,"sound")) {
	    ns->sound = atoi(param);
	    strcat(cmdback, param);
	}
	else if (!strcmp(cmd,"ext2")) {
	    ns->ext2 = atoi(param);
	    strcat(cmdback, param);
	}
	else if (!strcmp(cmd,"exp64")) {
	    ns->exp64 = atoi(param);
	    strcat(cmdback, param);
	} else if (!strcmp(cmd,"darkness")) {
	    ns->darkness = atoi(param);
	    strcat(cmdback, param);
	} else if (!strcmp(cmd,"map1cmd")) {
	    if (atoi(param)) ns->mapmode = Map1Cmd;
	    /* if beyond this size, need to use map1cmd no matter what */
	    if (ns->mapx>11 || ns->mapy>11) ns->mapmode = Map1Cmd;
	    strcat(cmdback, ns->mapmode == Map1Cmd?"1":"0");
	} else if (!strcmp(cmd,"map1acmd")) {
	    if (atoi(param)) ns->mapmode = Map1aCmd;
	    /* if beyond this size, need to use map1acmd no matter what */
	    if (ns->mapx>11 || ns->mapy>11) ns->mapmode = Map1aCmd;
	    strcat(cmdback, ns->mapmode == Map1aCmd?"1":"0");
        } else if (!strcmp(cmd,"newmapcmd")) {
            ns->newmapcmd= atoi(param);
            strcat(cmdback, param);
	} else if (!strcmp(cmd,"facecache")) {
	    ns->facecache = atoi(param);
            strcat(cmdback, param);
	} else if (!strcmp(cmd,"faceset")) {
	    char tmpbuf[20];
	    int q = atoi(param);

	    if (is_valid_faceset(q))
		ns->faceset=q;
	    sprintf(tmpbuf,"%d", ns->faceset);
	    strcat(cmdback, tmpbuf);
	    /* if the client is using faceset, it knows about image2 command */
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
	    strcat(cmdback, tmpbuf);
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
		strcat(cmdback, tmpbuf);
	    } else {
		ns->mapx = x;
		ns->mapy = y;
		/* better to send back what we are really using and not the
		 * param as given to us in case it gets parsed differently.
		 */
		sprintf(tmpbuf,"%dx%d", x,y);
		strcat(cmdback, tmpbuf);
		/* If beyond this size and still using orig map command, need to
		 * go to map1cmd.
		 */
		if ((x>11 || y>11) && ns->mapmode == Map0Cmd) ns->mapmode = Map1Cmd;
	    }
	} else if (!strcmp(cmd,"extendedMapInfos")) {
        /* Added by tchize
         * prepare to use the mapextended command
         */
	    char tmpbuf[20];
        ns->ext_mapinfos = (atoi(param));
	    sprintf(tmpbuf,"%d", ns->ext_mapinfos);
	    strcat(cmdback, tmpbuf);
	} else {
	    /* Didn't get a setup command we understood -
	     * report a failure to the client.
	     */
	    strcat(cmdback, "FALSE");
	}
    } /* for processing all the setup commands */
    LOG(llevInfo,"SendBack SetupCmd:: %s\n", cmdback);
    Write_String_To_Socket(ns, cmdback, strlen(cmdback));
}

/* The client has requested to be added to the game.  This is what
 * takes care of it.  We tell the client how things worked out.
 * I am not sure if this file is the best place for this function.  however,
 * it either has to be here or init_sockets needs to be exported.
 */
void AddMeCmd(char *buf, int len, NewSocket *ns)
{
    Settings oldsettings;
    oldsettings=settings;
    if (ns->status != Ns_Add || add_player(ns)) {		
	Write_String_To_Socket(ns, "addme_failed",12);
    } else {
	/* Basically, the add_player copies the socket structure into
	 * the player structure, so this one (which is from init_sockets)
	 * is not needed anymore.  The write below should still work, as the
	 * stuff in ns is still relevant.
	 */
	Write_String_To_Socket(ns, "addme_success",13);
	socket_info.nconns--;
	ns->status = Ns_Avail;
    }
    settings=oldsettings;	
}

void ToggleExtendedInfos (char *buf, int len, NewSocket *ns){
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

 /**
  * Tells client the picture it has to use 
  * to smooth a picture number given as argument.
  * Also take care of sending those pictures to the client.
  * For information, here is where we get the smoothing inforations:
  * - Take the facename and add '_smoothed' to it.
  * - Look for that archetype and find it's animation.
  * - Use the animation elements from 0 to 8 for smoothing.
  */
void AskSmooth (char *buf, int len, NewSocket *ns){
     char* defaultsmooth="default_smoothed.111";
     char reply[MAX_BUF];
     SockList sl;
     uint16 smoothface;
     int facenbr;
     facenbr=atoi (buf);
     if ((!FindSmooth (facenbr, &smoothface)) &&
         (!FindSmooth (FindFace(defaultsmooth,0), &smoothface))
        )

         LOG(llevError,"could not findsmooth for %d. Neither default (%s)\n",facenbr,defaultsmooth);
     if (ns->faces_sent[smoothface] == 0)
                esrv_send_face(ns, smoothface, 0);
     sl.buf=reply;
     strcpy((char*)sl.buf,"smooth ");
     sl.len=strlen((char*)sl.buf);
     SockList_AddShort(&sl, facenbr);
     SockList_AddShort(&sl, smoothface);
     Send_With_Handling(ns, &sl);
}
/* This handles the general commands from the client (ie, north, fire, cast,
 * etc.)
 */
void PlayerCmd(char *buf, int len, player *pl)
{

    /* The following should never happen with a proper or honest client.
     * Therefore, the error message doesn't have to be too clear - if 
     * someone is playing with a hacked/non working client, this gives them
     * an idea of the problem, but they deserve what they get
     */
    if (pl->state!=ST_PLAYING) {
	new_draw_info_format(NDI_UNIQUE, 0,pl->ob,
	    "You can not issue commands - state is not ST_PLAYING (%s)", buf);
	return;
    }
    /* Check if there is a count.  In theory, a zero count could also be
     * sent, so check for that also.
     */
    if (atoi(buf) || buf[0]=='0') {
	pl->count=atoi((char*)buf);
	buf=strchr(buf,' ');	/* advance beyond the numbers */
	if (!buf) {
#ifdef ESRV_DEBUG
	    LOG(llevDebug,"PlayerCmd: Got count but no command.");
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
     * for the command that issued the count should be done before any other
     * commands.
     */

    pl->count=0;

}


/* This handles the general commands from the client (ie, north, fire, cast,
 * etc.)  It is a lot like PlayerCmd above, but is called with the
 * 'ncom' method which gives more information back to the client so it
 * can throttle.
 */
void NewPlayerCmd(uint8 *buf, int len, player *pl)
{
    int packet, time,repeat;
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
	new_draw_info_format(NDI_UNIQUE, 0,pl->ob,
	    "You can not issue commands - state is not ST_PLAYING (%s)", buf);
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
     * for the command that issued the count should be done before any other
     * commands.
     */
    pl->count=0;

    /* Send confirmation of command execution now */
    sl.buf = (uint8*)command;
    strcpy((char*)sl.buf,"comc ");
    sl.len=5;
    SockList_AddShort(&sl,packet);
    if (FABS(pl->ob->speed) < 0.001) time=MAX_TIME * 100;
    else
	time = MAX_TIME/ FABS(pl->ob->speed);
    SockList_AddInt(&sl,time);
    Send_With_Handling(&pl->socket, &sl);
}


/* This is a reply to a previous query. */
void ReplyCmd(char *buf, int len, player *pl)
{
    /* This is to synthesize how the data would be stored if it
     * was normally entered.  A bit of a hack, and should be cleaned up
     * once all the X11 code is removed from the server.
     *
     * We pass 13 to many of the functions because this way they
     * think it was the carriage return that was entered, and the
     * function then does not try to do additional input.
     */
    sprintf(pl->write_buf,":%s", (char*)buf);

    pl->socket.ext_title_flag = 2; /* funny hack: we want the class info in old title function */

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
	    LOG(llevError,"In client input handling, but into configure state\n");
	    pl->state = ST_PLAYING;
	    break;

	case ST_GET_NAME:
	    receive_player_name(pl->ob,13);
	    break;

	case ST_GET_PASSWORD:
	case ST_CONFIRM_PASSWORD:
	    receive_player_password(pl->ob,13);
	    break;

	case ST_GET_PARTY_PASSWORD:        /* Get password for party */
	receive_party_password(pl->ob,13);
	    break;

	default:
	    LOG(llevError,"Unknown input state: %d\n", pl->state);
    }
}

/* Client tells its version.  If there is a mismatch, we close the
 * socket.  In real life, all we should care about is the client having
 * something older than the server.  If we assume the client will be
 * backwards compatible, having it be a later version should not be a 
 * problem.
 */
void VersionCmd(char *buf, int len,NewSocket *ns)
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
	LOG(llevDebug, "CS: csversion mismatch (%d,%d)\n", VERSION_CS,ns->cs_version);
#endif
    }
    cp = strchr(buf+1,' ');
    if (!cp) return;
    ns->sc_version = atoi(cp);
    if (VERSION_SC != ns->sc_version) {
#ifdef ESRV_DEBUG
	LOG(llevDebug, "CS: scversion mismatch (%d,%d)\n",VERSION_SC,ns->sc_version);
#endif
    }
    cp = strchr(cp+1, ' ');
    if (cp) {
	LOG(llevDebug,"CS: connection from client of type <%s>\n", cp);

	/* This is first implementation - i skip all beta DX clients with it 
	 * Add later stuff here for other clients 
	 */

	/* these are old dxclients */
	/* Version 1024 added support for singular + plural name values - 
	 * requiing this minimal value reduces complexity of that code, and it
	 * has been around for a long time.
	 */
	if(!strcmp(" CF DX CLIENT", cp) || ns->sc_version < 1024 )
	{
	    sprintf(version_warning,"drawinfo %d %s", NDI_RED, "**** VERSION WARNING ****\n**** CLIENT IS TOO OLD!! UPDATE THE CLIENT!! ****");
	    Write_String_To_Socket(ns, version_warning, strlen(version_warning));
	}

    }
}

/* sound related functions. */
 
void SetSound(char *buf, int len, NewSocket *ns)
{
    ns->sound = atoi(buf);
}

/* client wants the map resent */

void MapRedrawCmd(char *buff, int len, player *pl)
{
    /* Okay, this is MAJOR UGLY. but the only way I know how to
     * clear the "cache"
     */
    memset(&pl->socket.lastmap, 0, sizeof(struct Map));
    draw_client_map(pl->ob);
}

void MapNewmapCmd( player *pl)
{
    if( pl->socket.newmapcmd == 1)
        Write_String_To_Socket( &pl->socket, "newmap", 6);
}



/* Moves and object (typically, container to inventory
 * move <to> <tag> <nrof> 
 */
void MoveCmd(char *buf, int len,player *pl)
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



/******************************************************************************
 *
 * Start of commands the server sends to the client.
 *
 ******************************************************************************/

/*
 * send_query asks the client to query the user.  This way, the client knows
 * it needs to send something back (vs just printing out a message
 */
void send_query(NewSocket *ns, uint8 flags, char *text)
{
    char buf[MAX_BUF];

    sprintf(buf,"query %d %s", flags, text?text:"");
    Write_String_To_Socket(ns, buf, strlen(buf));
}


/* Sends the stats to the client - only sends them if they have changed */

#define AddIfInt64(Old,New,Type) if (Old != New) {\
			Old = New; \
			SockList_AddChar(&sl, Type); \
			SockList_AddInt64(&sl, New); \
		       }

#define AddIfInt(Old,New,Type) if (Old != New) {\
			Old = New; \
			SockList_AddChar(&sl, Type); \
			SockList_AddInt(&sl, New); \
		       }

#define AddIfShort(Old,New,Type) if (Old != New) {\
			Old = New; \
			SockList_AddChar(&sl, Type); \
			SockList_AddShort(&sl, New); \
		       }

#define AddIfFloat(Old,New,Type) if (Old != New) {\
			Old = New; \
			SockList_AddChar(&sl, Type); \
			SockList_AddInt(&sl,(long)(New*FLOAT_MULTI));\
			}

#define AddIfString(Old,New,Type) if (Old == NULL || strcmp(Old,New)) {\
			if (Old) free(Old);\
	                Old = strdup_local(New);\
			SockList_AddChar(&sl, Type); \
			SockList_AddChar(&sl, strlen(New)); \
			strcpy((char*)sl.buf + sl.len, New); \
			sl.len += strlen(New); \
			}

/*
 * esrv_update_stats sends a statistics update.  We look at the old values,
 * and only send what has changed.  Stat mapping values are in newclient.h
 * Since this gets sent a lot, this is actually one of the few binary
 * commands for now.
 */
void esrv_update_stats(player *pl)
{
    SockList sl;
    char buf[MAX_BUF];
    uint16 flags;

    sl.buf=malloc(MAXSOCKBUF);
    strcpy((char*)sl.buf,"stats ");
    sl.len=strlen((char*)sl.buf);
    
    if(pl->ob != NULL)
    {
        AddIfShort(pl->last_stats.hp, pl->ob->stats.hp, CS_STAT_HP);
        AddIfShort(pl->last_stats.maxhp, pl->ob->stats.maxhp, CS_STAT_MAXHP);
        AddIfShort(pl->last_stats.sp, pl->ob->stats.sp, CS_STAT_SP);
        AddIfShort(pl->last_stats.maxsp, pl->ob->stats.maxsp, CS_STAT_MAXSP);
        AddIfShort(pl->last_stats.grace, pl->ob->stats.grace, CS_STAT_GRACE);
        AddIfShort(pl->last_stats.maxgrace, pl->ob->stats.maxgrace, CS_STAT_MAXGRACE);
        AddIfShort(pl->last_stats.Str, pl->ob->stats.Str, CS_STAT_STR);
        AddIfShort(pl->last_stats.Int, pl->ob->stats.Int, CS_STAT_INT);
        AddIfShort(pl->last_stats.Pow, pl->ob->stats.Pow, CS_STAT_POW);
        AddIfShort(pl->last_stats.Wis, pl->ob->stats.Wis, CS_STAT_WIS);
        AddIfShort(pl->last_stats.Dex, pl->ob->stats.Dex, CS_STAT_DEX);
        AddIfShort(pl->last_stats.Con, pl->ob->stats.Con, CS_STAT_CON);
        AddIfShort(pl->last_stats.Cha, pl->ob->stats.Cha, CS_STAT_CHA);
    }
    if(pl->socket.exp64) {
	int s;
	for(s=0;s<NUM_SKILLS;s++) {
	    if (pl->last_skill_ob[s] && 
		pl->last_skill_exp[s] != pl->last_skill_ob[s]->stats.exp) {

		/* Always send along the level if exp changes.  This is only
		 * 1 extra byte, but keeps processing simpler.
		 */
		SockList_AddChar(&sl, s + CS_STAT_SKILLINFO);
		SockList_AddChar(&sl, pl->last_skill_ob[s]->level);
		SockList_AddInt64(&sl, pl->last_skill_ob[s]->stats.exp);
		pl->last_skill_exp[s] =  pl->last_skill_ob[s]->stats.exp;
	    }
	}
    }
    if (pl->socket.exp64) {
	AddIfInt64(pl->last_stats.exp, pl->ob->stats.exp, CS_STAT_EXP64);
    } else {
	AddIfInt(pl->last_stats.exp, pl->ob->stats.exp, CS_STAT_EXP);
    }
    AddIfShort(pl->last_level, pl->ob->level, CS_STAT_LEVEL);
    AddIfShort(pl->last_stats.wc, pl->ob->stats.wc, CS_STAT_WC);
    AddIfShort(pl->last_stats.ac, pl->ob->stats.ac, CS_STAT_AC);
    AddIfShort(pl->last_stats.dam, pl->ob->stats.dam, CS_STAT_DAM);
    AddIfFloat(pl->last_speed, pl->ob->speed, CS_STAT_SPEED);
    AddIfShort(pl->last_stats.food, pl->ob->stats.food, CS_STAT_FOOD);
    AddIfFloat(pl->last_weapon_sp, pl->weapon_sp, CS_STAT_WEAP_SP);
    AddIfInt(pl->last_weight_limit, weight_limit[pl->ob->stats.Str], CS_STAT_WEIGHT_LIM);
    flags=0;
    if (pl->fire_on) flags |=SF_FIREON;
    if (pl->run_on) flags |= SF_RUNON;

    AddIfShort(pl->last_flags, flags, CS_STAT_FLAGS);
    if (pl->socket.sc_version<1025) {
	AddIfShort(pl->last_resist[ATNR_PHYSICAL], pl->ob->resist[ATNR_PHYSICAL], CS_STAT_ARMOUR);
    } else {
	int i;

	for (i=0; i<NROFATTACKS; i++) {
	    /* Skip ones we won't send */
	    if (atnr_cs_stat[i]==-1) continue;
	    AddIfShort(pl->last_resist[i], pl->ob->resist[i], atnr_cs_stat[i]);
	}
    }
 
    rangetostring(pl->ob, buf); /* we want use the new fire & run system in new client */
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


/* Tells the client that here is a player it should start using.
 */

void esrv_new_player(player *pl, uint32 weight)
{
    SockList	sl;

    sl.buf=malloc(MAXSOCKBUF);

    strcpy((char*)sl.buf,"player ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddInt(&sl, pl->ob->count);
    SockList_AddInt(&sl, weight);
    SockList_AddInt(&sl, pl->ob->face->number);

    SockList_AddChar(&sl, strlen(pl->ob->name));
    strcpy((char*)sl.buf+sl.len, pl->ob->name);
    sl.len += strlen(pl->ob->name);
       
    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
    SET_FLAG(pl->ob, FLAG_CLIENT_SENT);
}


/* Need to send an animation sequence to the client.
 * We will send appropriate face commands to the client if we haven't
 * sent them the face yet (this can become quite costly in terms of
 * how much we are sending - on the other hand, this should only happen
 * when the player logs in and picks stuff up.
 */
void esrv_send_animation(NewSocket *ns, short anim_num)
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

    sl.buf = malloc(MAXSOCKBUF);
    strcpy((char*)sl.buf, "anim ");
    sl.len=5;
    SockList_AddShort(&sl, anim_num);
    SockList_AddShort(&sl, 0);  /* flags - not used right now */
    /* Build up the list of faces.  Also, send any information (ie, the
     * the face itself) down to the client.
     */
    for (i=0; i<animations[anim_num].num_animations; i++) {
	if (ns->faces_sent[animations[anim_num].faces[i]] == 0)
	    esrv_send_face(ns,animations[anim_num].faces[i],0);
	SockList_AddShort(&sl, animations[anim_num].faces[i]);  /* flags - not used right now */
    }
    Send_With_Handling(ns, &sl);
    free(sl.buf);
    ns->anims_sent[anim_num] = 1;
}


/******************************************************************************
 *
 * Start of map related commands.
 *
 ******************************************************************************/

/* This adds face_num to a map cell at x,y.  If the client doesn't have
 * the face yet, we will also send it.
 */
static void esrv_map_setbelow(NewSocket *ns, int x,int y,
			      short face_num, struct Map *newmap)
{

    if (x<0 || x>ns->mapx-1 || 
	y<0 || y>ns->mapy-1 || 
	face_num < 0 || face_num > MAXFACENUM) {

	LOG(llevError,"bad user x/y/facenum not in 0..10,0..10,0..%d\n",
	    MAXFACENUM-1);
	abort();
    }
    if(newmap->cells[x][y].count >= MAP_LAYERS) {
	LOG(llevError,"Too many faces in map cell %d %d\n",x,y);
	return;
	abort();
    }
    newmap->cells[x][y].faces[newmap->cells[x][y].count] = face_num;
    newmap->cells[x][y].count ++;
    if (ns->faces_sent[face_num] == 0)
	esrv_send_face(ns,face_num,0);
}

struct LayerCell {
  uint16 xy;
  short face;
};

struct MapLayer {
  int count;
  struct LayerCell lcells[MAP_CLIENT_X * MAP_CLIENT_Y];
};

static int mapcellchanged(NewSocket *ns,int i,int j, struct Map *newmap)
{
  int k;

  if (ns->lastmap.cells[i][j].count != newmap->cells[i][j].count)
    return 1;
  for(k=0;k<newmap->cells[i][j].count;k++) {
    if (ns->lastmap.cells[i][j].faces[k] !=
	newmap->cells[i][j].faces[k]) {
      return 1;
    }
  }
  return 0;
}


/* cnum is the client number, cur is the the buffer we put all of
 * this data into.  we return the end of the data.  layers is
 * how many layers of data we should back.
 * Basically, what this does is pack the data into layers.
 */  
static uint8 *compactlayer(NewSocket *ns, unsigned char *cur, int numlayers, 
			   struct Map *newmap)
{
    int x,y,k;
    int face;
    unsigned char *fcur;
    struct MapLayer layers[MAP_LAYERS];
  
    for(k = 0;k<MAP_LAYERS;k++)
	layers[k].count = 0;
    fcur = cur;
    for(x=0;x<ns->mapx;x++) {
	for(y=0;y<ns->mapy;y++) {
	    if (!mapcellchanged(ns,x,y,newmap))
		continue;
	    if (newmap->cells[x][y].count == 0) {
		*cur = x*ns->mapy+y;	    /* mark empty space */
		cur++;
		continue;
	    }
	    for(k=0;k<newmap->cells[x][y].count;k++) {
		layers[k].lcells[layers[k].count].xy = x*ns->mapy+y;
		layers[k].lcells[layers[k].count].face = 
		    newmap->cells[x][y].faces[k];
		layers[k].count++;
	    }
	}
    }
    /* If no data, return now. */
    if (fcur == cur && layers[0].count == 0)
	return cur;
    *cur = 255; /* mark end of explicitly cleared cells */
    cur++;
    /* First pack by layers. */
    for(k=0;k<numlayers;k++) {
	if (layers[k].count == 0)
	    break; /* once a layer is entirely empty, no layer below it can
		have anything in it either */
	/* Pack by entries in thie layer */
	for(x=0;x<layers[k].count;) {
	    fcur = cur;
	    *cur = layers[k].lcells[x].face >> 8;
	    cur++;
	    *cur = layers[k].lcells[x].face & 0xFF;
	    cur++;
	    face = layers[k].lcells[x].face;
	    /* Now, we back the redundant data into 1 byte xy pairings */
	    for(y=x;y<layers[k].count;y++) {
		if (layers[k].lcells[y].face == face) {
		    *cur = layers[k].lcells[y].xy;
		    cur++;
		    layers[k].lcells[y].face = -1;
		}
	    }
	    *(cur-1) = *(cur-1) | 128; /* mark for end of xy's; 11*11 < 128 */
	    /* forward over the now redundant data */
	    while(x < layers[k].count &&
		  layers[k].lcells[x].face == -1)
		x++;
	}
	*fcur = *fcur | 128; /* mark for end of faces at this layer */
    }
    return cur;
}

static void esrv_map_doneredraw(NewSocket *ns, struct Map *newmap)
{
    static long frames,bytes,tbytes,tframes;
    uint8 *cur;
    SockList sl;


    sl.buf=malloc(MAXSOCKBUF);
    strcpy((char*)sl.buf,"map ");
    sl.len=strlen((char*)sl.buf);

    cur = compactlayer(ns,sl.buf+sl.len,MAP_LAYERS,newmap);
    sl.len=cur-sl.buf;

/*    LOG(llevDebug, "Sending map command.\n");*/

    if (sl.len>strlen("map ") || ns->sent_scroll) {
	/* All of this is just accounting stuff */
	if (tframes>100) {
	    tframes = tbytes = 0;
	}
	tframes++;
	frames++;
	tbytes += sl.len;
	bytes += sl.len;
	memcpy(&ns->lastmap,newmap,sizeof(struct Map));
	Send_With_Handling(ns, &sl);
	ns->sent_scroll = 0;
    }
    free(sl.buf);
}


/* Clears a map cell */
static void map_clearcell(struct MapCell *cell, int face0, int face1, int face2, int count)
{
    cell->count=count;
    cell->faces[0] = face0;
    cell->faces[1] = face1;
    cell->faces[2] = face2;
}

#define MAX_HEAD_POS	31
#define MAX_LAYERS	3

/* Using a global really isn't a good approach, but saves the over head of
 * allocating and deallocating such a block of data each time run through,
 * and saves the space of allocating this in the socket object when we only
 * need it for this cycle.  If the serve is ever threaded, this needs to be
 * re-examined.
 */

static object  *heads[MAX_HEAD_POS * MAX_HEAD_POS * MAX_LAYERS];

/* simple function - returns true of any of the heads for this
 * space is set.  Returns 0 if all are blank - this is used
 * for empty space checking.
 */
static inline int have_head(int ax, int ay) {

    if (heads[(ay * MAX_HEAD_POS + ax) * MAX_LAYERS] ||
	heads[(ay * MAX_HEAD_POS + ax) * MAX_LAYERS + 1] ||
	heads[(ay * MAX_HEAD_POS + ax) * MAX_LAYERS + 2]) return 1;
    return 0;
}

/* check_head is a bit simplistic version of update_space below.
 * basically, it only checks the that the head on space ax,ay at layer
 * needs to get sent - if so, it adds the data, sending the head
 * if needed, and returning 1.  If this no data needs to get
 * sent, it returns zero.
 */
static inline int check_head(SockList *sl, NewSocket *ns, int ax, int ay, int layer)
{
    short face_num;

    if (heads[(ay * MAX_HEAD_POS + ax) * MAX_LAYERS + layer])
	face_num = heads[(ay * MAX_HEAD_POS + ax) * MAX_LAYERS + layer]->face->number;
    else
	face_num = 0;

    if (face_num != ns->lastmap.cells[ax][ay].faces[layer]) {
	SockList_AddShort(sl, face_num);
	if (face_num && ns->faces_sent[face_num] == 0)
	    esrv_send_face(ns, face_num, 0);
	heads[(ay * MAX_HEAD_POS + ax) * MAX_LAYERS + layer] = NULL;
	ns->lastmap.cells[ax][ay].faces[layer] = face_num;
	return 1;
    }
    /* We know, for now, that check_head is only called on blocked or otherwise
     * out of view spaces.  So if there is no head object, clear
     * the last look for this layer - this sort of replaces the need to
     * call map_clearcell.
     */
    if (face_num ==0 && ns->lastmap.cells[ax][ay].faces[layer] != 0)
	ns->lastmap.cells[ax][ay].faces[layer] = 0;

    return 0;   /* No change */
}

/* Removes the need to replicate the same code for each layer.
 * this returns true if this space is now in fact different than
 * it was.
 * sl is the socklist this data is going into.
 * ns is the socket we are working on - all the info we care
 * about is in this socket structure, so now need not pass the
 * entire player object.
 * mx and my are map coordinate offsets for map mp
 * sx and sy are the offsets into the socket structure that
 * holds the old values.
 * layer is the layer to update, with 2 being the floor and 0 the
 * top layer (this matches what the GET_MAP_FACE and GET_MAP_FACE_OBJ)
 * take.  Interesting to note that before this function, the map1 function
 * numbers the spaces differently - I think this was a leftover from
 * the map command, where the faces stack up.  Sinces that is no longer
 * the case, it seems to make more sense to have these layer values
 * actually match.
 */

static inline int update_space(SockList *sl, NewSocket *ns, mapstruct  *mp, int mx, int my, int sx, int sy, int layer)
{
    object *ob, *head;
    int face_num;
    int bx, by,i;

    /* If there is a multipart object stored away, treat that as more important.
     * If not, then do the normal processing.
     */

    head = heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + layer];

    /* Check to see if this head is part of the set of objects
     * we would normally send for this space.  If so, then
     * don't use the head value.  We need to do the check
     * here and not when setting up the heads[] value for two reasons -
     * 1) the heads[] values will get used even if the space is not visible.
     * 2) its possible the head is not on the same map as a part, and I'd
     *    rather not need to do the map translation overhead.
     * 3) We need to do some extra checking to make sure that we will
     * otherwise send the image as this layer, eg, either it matches
     * the head value, or is not multipart.
     */
    if (head) {
	for (i=0; i<MAP_LAYERS; i++) {
	    ob = GET_MAP_FACE_OBJ(mp, mx, my, i);
	    if (!ob) continue;

	    if (ob->head) ob=ob->head;

	    if (ob->face == head->face &&
		(ob == head && !ob->more)) {
		    heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + layer] = NULL;
		    head = NULL;
		    break;
	    }
	}
    }

    ob = head;
    if (!ob) ob = GET_MAP_FACE_OBJ(mp, mx, my, layer);

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

	    if (ob->head) head = ob->head;
	    else head = ob;

	    /* Basically figure out where the offset is from where we are right
	     * now.  the ob->arch->clone.{x,y} values hold the offset that this current
	     * piece is from the head, and the tail is where the tail is from the
	     * head.  Note that bx and by will equal sx and sy if we are already working
	     * on the bottom right corner.  If ob is the head, the clone values
	     * will be zero, so the right thing will still happen.
	     */
	    bx = sx + head->arch->tail_x - ob->arch->clone.x;
	    by = sy + head->arch->tail_y - ob->arch->clone.y;

	    /* I don't think this can ever happen, but better to check for it just
	     * in case.
	     */
	    if (bx < sx || by < sy) {
		LOG(llevError,"update_space: bx (%d) or by (%d) is less than sx (%d) or sy (%d)\n",
		    bx, by, sx, sy);
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
		    if (heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + i] &&
			heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + i]->face->number == face_num)
			heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + i] = NULL;
	    }
	    else {
		/* First, try to put the new head on the same layer.  If that is used up,
		 * then find another layer.
		 */
		if (heads[(by * MAX_HEAD_POS + bx) * MAX_LAYERS + layer] == NULL ||
		    heads[(by * MAX_HEAD_POS + bx) * MAX_LAYERS + layer] == head) {
			heads[(by * MAX_HEAD_POS + bx) * MAX_LAYERS + layer] = head;
		} else for (i=0; i<MAX_LAYERS; i++) {
		    if (heads[(by * MAX_HEAD_POS + bx) * MAX_LAYERS + i] == NULL ||
			heads[(by * MAX_HEAD_POS + bx) * MAX_LAYERS + i] == head) {
			    heads[(by * MAX_HEAD_POS + bx) * MAX_LAYERS + i] = head;
		    }
		}
		face_num = 0;	/* Don't send this object - we'll send the head later */
	    }
	} else {
	    /* In this case, we are already at the lower right or single part object,
	     * so nothing special 
	     */
	    face_num = ob->face->number;

	    /* clear out any head entries that have the same face as this one */
	    for (bx=0; bx<layer; bx++)
		if (heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + bx] &&
		    heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + bx]->face->number == face_num)
			heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + bx] = NULL;
	}
    } /* else not already head object or blank face */

    /* This is a real hack.  Basically, if we have nothing to send for this layer,
     * but there is a head on the next layer, send that instead.
     * Without this, what happens is you can get the case where the player stands
     * on the same space as the head.  However, if you have overlapping big objects
     * of the same type, what happens then is it doesn't think it needs to send
     * This tends to make stacking also work/look better.
     */
    if (!face_num && layer > 0 && heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + layer -1]) {
	face_num = heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + layer -1]->face->number;
	heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + layer -1] = NULL;
    }

    /* Another hack - because of heads and whatnot, this face may match one
     * we already sent for a lower layer.  In that case, don't send
     * this one.
     */
    if (face_num && layer<MAP_LAYERS && ns->lastmap.cells[sx][sy].faces[layer+1] == face_num) {
	face_num = 0;
    }

    /* We've gotten what face we want to use for the object.  Now see if
     * if it has changed since we last sent it to the client.
     */
    if (ns->lastmap.cells[sx][sy].faces[layer] != face_num)  {
	ns->lastmap.cells[sx][sy].faces[layer] = face_num;
	if (ns->faces_sent[face_num] == 0)
	    esrv_send_face(ns, face_num, 0);
	SockList_AddShort(sl, face_num);
	return 1;
    }
    /* Nothing changed */
    return 0;
}

/* This function is mainly a copy of update_space, 
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

static inline int update_smooth(SockList *sl, NewSocket *ns, mapstruct  *mp, int mx, int my, int sx, int sy, int layer)
{
    object *ob, *head;
    int smoothlevel; /* old face_num;*/

    /* If there is a multipart object stored away, treat that as more important.
     * If not, then do the normal processing.
     */
    ob=NULL;
    head = heads[(sy * MAX_HEAD_POS + sx) * MAX_LAYERS + layer];


    ob = GET_MAP_FACE_OBJ(mp, mx, my, layer);

    /* If there is no object for this space, or if the face for the object
     * is the blank face, set the smoothlevel to zero.
     */
    if (!ob || ob->face == blank_face) smoothlevel=0;
    else {
	    smoothlevel = ob->smoothlevel;
    } /* else not already head object or blank face */

    /* We've gotten what face we want to use for the object.  Now see if
     * if it has changed since we last sent it to the client.
     */
    if (smoothlevel>255)
         smoothlevel=255;
    else if (smoothlevel<0)
         smoothlevel=0;
    if (ns->lastmap.cells[sx][sy].smooth[layer] != smoothlevel)  {
	ns->lastmap.cells[sx][sy].smooth[layer] = smoothlevel;
	SockList_AddChar(sl, (uint8) (smoothlevel&0xFF));
	return 1;
    }
    /* Nothing changed */
    return 0;
}
/* returns the size of a data for a map square as returned by
 * mapextended. There are CLIENTMAPX*CLIENTMAPY*LAYERS entries
 * available.
 */
int getExtendedMapInfoSize(NewSocket* ns){
    int result=0;
    if (ns->ext_mapinfos){
        if (ns->EMI_smooth)
            result+=1; /*One byte for smoothlevel*/
    }
    return result;
}
/* this function uses the new map1 protocol command to send the map
 * to the client.  It is necessary because the old map command supports
 * a maximum map size of 15x15.
 * This function is much simpler than the old one.  This is because
 * the old function optimized to send as few face identifiers as possible,
 * at the expense of sending more coordinate location (coordinates were
 * only 1 byte, faces 2 bytes, so this was a worthwhile savings).  Since
 * we need 2 bytes for coordinates and 2 bytes for faces, such a trade off
 * maps no sense.  Instead, we actually really only use 12 bits for coordinates,
 * and use the other 4 bits for other informatiion.   For full documentation
 * of what we send, see the doc/Protocol file.
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
    int x,y,ax, ay, d, nx,ny, startlen, max_x, max_y, oldlen;
    int estartlen, eoldlen;
    SockList sl;
    SockList esl; /*For extended Map info*/
    uint16  mask,emask,eentrysize;
    uint16  ewhatstart,ewhatflag;
    uint8   extendedinfos;
    mapstruct *m;

    sl.buf=malloc(MAXSOCKBUF);
    if (pl->contr->socket.mapmode == Map1Cmd)
	strcpy((char*)sl.buf,"map1 ");
    else
	strcpy((char*)sl.buf,"map1a ");
    sl.len=strlen((char*)sl.buf);
    startlen = sl.len;
    /*Extendedmapinfo structure initialisation*/
    if (pl->contr->socket.ext_mapinfos){
        esl.buf=malloc(MAXSOCKBUF);
        strcpy((char*)esl.buf,"mapextended ");
        esl.len=strlen((char*)esl.buf);
        extendedinfos=EMI_NOREDRAW;
        if (pl->contr->socket.EMI_smooth)
            extendedinfos|=EMI_SMOOTH;
        ewhatstart=esl.len;
        ewhatflag=extendedinfos; /*The EMI_NOREDRAW bit
                                   could need to be taken away*/
        SockList_AddChar(&esl, extendedinfos);
        eentrysize=getExtendedMapInfoSize(&(pl->contr->socket));
        SockList_AddChar(&esl, eentrysize);
        estartlen = esl.len;
    }
    /* Init data to zero */
    memset(heads, 0, sizeof(object *) * MAX_HEAD_POS * MAX_HEAD_POS * MAX_LAYERS);

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

	    /* If this space is out of the normal viewable area, we only check
	     * the heads value  ax or ay will only be greater than what
	     * the client wants if using the map1a command - this is because
	     * if the map1a command is not used, max_x and max_y will be
	     * set to lower values.
	     */
	    if (ax >= pl->contr->socket.mapx || ay >= pl->contr->socket.mapy) {
		oldlen = sl.len;

		if (pl->contr->socket.ext_mapinfos){
		    SockList_AddShort(&esl, emask);
		}

		SockList_AddShort(&sl, mask);

		if (check_head(&sl, &pl->contr->socket, ax, ay, 2))
		    mask |= 0x4;
		if (check_head(&sl, &pl->contr->socket, ax, ay, 1))
		    mask |= 0x2;
		if (check_head(&sl, &pl->contr->socket, ax, ay, 0))
		    mask |= 0x1;

		if (mask & 0xf) {
		    sl.buf[oldlen+1] = mask & 0xff;
		} else {
		    sl.len = oldlen;
		}
		/*What concerns extendinfos, nothing to be done for now 
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
		if (pl->contr->socket.lastmap.cells[ax][ay].count != -1) {
		    SockList_AddShort(&sl, mask);
		    map_clearcell(&pl->contr->socket.lastmap.cells[ax][ay],0,0,0,-1);
		}
	    } else if (d>3) {
		int need_send=0, count;
		/* This block deals with spaces that are not visible for whatever
		 * reason.  Still may need to send the head for this space.
		 */

		oldlen = sl.len;
#if 0
		/* First thing we do is blank out this space (clear it)
		 * if not already done.  If the client is using darkness, and
		 * this space is at the edge, we also include the darkness.
		 */
		if (d==4) {
		    if (pl->contr->socket.darkness && pl->contr->socket.lastmap.cells[ax][ay].count != d) {
			mask |= 8;
			SockList_AddShort(&sl, mask);
			SockList_AddChar(&sl, 0);
		    }
		    count = d;
		} else
#endif
		{
		    SockList_AddShort(&sl, mask);
		    if (pl->contr->socket.lastmap.cells[ax][ay].count != -1) need_send=1;
		    count = -1;
		}

		if (pl->contr->socket.mapmode == Map1aCmd && have_head(ax, ay)) {
		    /* Now check to see if any heads need to be sent */

		    if (check_head(&sl, &pl->contr->socket, ax, ay, 2))
			mask |= 0x4;
		    if (check_head(&sl, &pl->contr->socket, ax, ay, 1))
			mask |= 0x2;
		    if (check_head(&sl, &pl->contr->socket, ax, ay, 0))
			mask |= 0x1;
		    pl->contr->socket.lastmap.cells[ax][ay].count = count;

		} else {
		    map_clearcell(&pl->contr->socket.lastmap.cells[ax][ay], 0, 0, 0, count);
		}

		if ((mask & 0xf) || need_send) {
		    sl.buf[oldlen+1] = mask & 0xff;
		} else {
		    sl.len = oldlen;
		}
	    } else {
		/* In this block, the space is visible or there are head objects
		 * we need to send.
		 */

		 /* Rather than try to figure out what everything that we might
		 * need to send is, then form the packet after that,
		 * we presume that we will in fact form a packet, and update
		 * the bits by what we do actually send.  If we send nothing,
		 * we just back out sl.len to the old value, and no harm
		 * is done.
		 * I think this is simpler than doing a bunch of checks to see
		 * what if anything we need to send, setting the bits, then
		 * doing those checks again to add the real data.
		 */
		oldlen = sl.len;
		mask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
		eoldlen = esl.len;
		emask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
		SockList_AddShort(&sl, mask);		

		if (pl->contr->socket.ext_mapinfos)
		    SockList_AddShort(&esl, emask);

		/* Darkness changed */
		if (pl->contr->socket.lastmap.cells[ax][ay].count != d && pl->contr->socket.darkness) {
		    pl->contr->socket.lastmap.cells[ax][ay].count = d;
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
		    /* need to reset from -1 so that if it does become blocked again,
		     * the code that deals with that can detect that it needs to tell
		     * the client that this space is now blocked.
		     */
		    pl->contr->socket.lastmap.cells[ax][ay].count = d;
		}

		/* Floor face */
		if (update_space(&sl, &pl->contr->socket, m, nx, ny, ax, ay, 2))
		    mask |= 0x4;
		
		if (pl->contr->socket.EMI_smooth)
		    if (update_smooth(&esl, &pl->contr->socket, m, nx, ny, ax, ay, 2)){
			emask |= 0x4;
		    }

		/* Middle face */
		if (update_space(&sl, &pl->contr->socket, m, nx, ny, ax, ay, 1))
		    mask |= 0x2;		

		if (pl->contr->socket.EMI_smooth)
		    if (update_smooth(&esl, &pl->contr->socket, m, nx, ny, ax, ay, 1)){
			emask |= 0x2;
		    }


		if(nx == pl->x && ny == pl->y && pl->invisible & (pl->invisible < 50 ? 4 : 1)) {
		    if (pl->contr->socket.lastmap.cells[ax][ay].faces[0] != pl->face->number) {
			pl->contr->socket.lastmap.cells[ax][ay].faces[0] = pl->face->number;
			mask |= 0x1;
			if (pl->contr->socket.faces_sent[pl->face->number] == 0)
			    esrv_send_face(&pl->contr->socket, pl->face->number, 0);
			SockList_AddShort(&sl, pl->face->number);
		    }
		}
		/* Top face */
		else {
		    if (update_space(&sl, &pl->contr->socket, m, nx, ny, ax, ay, 0))
		        mask |= 0x1;            
		    if (pl->contr->socket.EMI_smooth)
			if (update_smooth(&esl, &pl->contr->socket, m, nx, ny, ax, ay, 0)){
			    emask |= 0x1;
			}
		}
		/* Check to see if we are in fact sending anything for this
		 * space by checking the mask.  If so, update the mask.
		 * if not, reset the len to that from before adding the mask
		 * value, so we don't send those bits.
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
        free(esl.buf);
        }
    }
    if (sl.len>startlen || pl->contr->socket.sent_scroll) {
	Send_With_Handling(&pl->contr->socket, &sl);
	pl->contr->socket.sent_scroll = 0;
    }
    free(sl.buf);
}


void draw_client_map(object *pl)
{
    int i,j,nx,ny; /* ax and ay goes from 0 to max-size of arrays */
    sint16  ax, ay;
    New_Face	*face,*floor;
    New_Face	*floor2;
    int d, mflags;
    struct Map	newmap;
    mapstruct	*m;

    if (pl->type != PLAYER) {
	LOG(llevError,"draw_client_map called with non player/non eric-server\n");
	return;
    }

    if(pl->map == NULL || pl->map->in_memory != MAP_IN_MEMORY) {
	LOG(llevError,"draw_client_map called with no map/map out of memory\n");
	return;
    }


    /* IF player is just joining the game, he isn't here yet, so the map
     * can get swapped out.  If so, don't try to send them a map.  All will
     * be OK once they really log in.
     */
    if (pl->map==NULL || pl->map->in_memory!=MAP_IN_MEMORY) return;
    memset(&newmap, 0, sizeof(struct Map));

    for(j = (pl->y - pl->contr->socket.mapy/2) ; j < (pl->y + (pl->contr->socket.mapy+1)/2); j++) {
        for(i = (pl->x - pl->contr->socket.mapx/2) ; i < (pl->x + (pl->contr->socket.mapx+1)/2); i++) {
	    ax=i;
	    ay=j;
	    m = pl->map;
	    mflags = get_map_flags(m, &m, ax, ay, &ax, &ay);
	    if (mflags & P_NEED_UPDATE)
		update_position(m, ax, ay);
	    /* If a map is visible to the player, we don't want to swap it out
	     * just to reload it.  This should really call something like
             * swap_map, but this is much more efficient and 'good enough'
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

    if (pl->contr->socket.mapmode == Map1Cmd || pl->contr->socket.mapmode == Map1aCmd) {
        /* Big maps need a different drawing mechanism to work */
        draw_client_map1(pl);
        return;
    }
    
    if(pl->invisible & (pl->invisible < 50 ? 4 : 1)) {
	esrv_map_setbelow(&pl->contr->socket,pl->contr->socket.mapx/2,
			  pl->contr->socket.mapy/2,pl->face->number,&newmap);
    }

    /* j and i are the y and x coordinates of the real map (which is 
     * basically some number of spaces around the player)
     * ax and ay are values from within the viewport (ie, 0, 0 is upper
     * left corner) and are thus disconnected from the map values.
     * Subtract 1 from the max values so that we properly handle cases where
     * player has specified an even map.  Otherwise, we try to send them too
     * much, ie, if mapx is 10, we would try to send from -5 to 5, which is actually
     * 11 spaces.  Now, we would send from -5 to 4, which is properly.  If mapx is
     * odd, this still works fine.
     */
    ay=0;
    for(j=pl->y-pl->contr->socket.mapy/2; j<=pl->y+(pl->contr->socket.mapy-1)/2;j++, ay++) {
	ax=0;
	for(i=pl->x-pl->contr->socket.mapx/2;i<=pl->x+(pl->contr->socket.mapx-1)/2;i++, ax++) {

	    d =  pl->contr->blocked_los[ax][ay];
	    /* note the out_of_map and d>3 checks are both within the same
	     * negation check.
	     */
	    nx = i;
	    ny = j;
	    m = get_map_from_coord(pl->map, &nx, &ny);
	    if (m && d<4) {
		face = GET_MAP_FACE(m, nx, ny,0);
		floor2 = GET_MAP_FACE(m, nx, ny,1);
		floor = GET_MAP_FACE(m, nx, ny,2);

		/* If all is blank, send a blank face. */
		if ((!face || face == blank_face) &&
		    (!floor2 || floor2 == blank_face) &&
		    (!floor || floor == blank_face)) {
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
				blank_face->number,&newmap);
		} else { /* actually have something interesting */
		  /* send the darkness mask, if any. */
		  if (d && pl->contr->socket.darkness) 
		    esrv_map_setbelow(&pl->contr->socket,ax,ay,
						  dark_faces[d-1]->number,&newmap);

		    if (face && face != blank_face)
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
					  face->number,&newmap);
		    if (floor2 && floor2 != blank_face)
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
				floor2->number,&newmap);
		    if (floor && floor != blank_face)
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
					  floor->number,&newmap);
		} 
	    } /* Is a valid space */
	}
    }
    esrv_map_doneredraw(&pl->contr->socket, &newmap);
}


void esrv_map_scroll(NewSocket *ns,int dx,int dy)
{
    struct Map newmap;
    int x,y, mx, my;
    char buf[MAXSOCKBUF];

    sprintf(buf,"map_scroll %d %d", dx, dy);
    Write_String_To_Socket(ns, buf, strlen(buf));

    /* If we are using the Map1aCmd, we may in fact send
     * head information that is outside the viewable map.
     * So set the mx,my to the max value we want to
     * look for.  Removed code to do so - it caused extra
     * complexities for the client, and probably doesn't make
     * that much difference in bandwidth.
     */
    mx = ns->mapx;
    my = ns->mapy;

    if (ns->mapmode == Map1aCmd) {
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
	    if ((x+dx) < 0 || (x+dx) >= ns->mapx || (y+dy) < 0 || (y + dy) >= ns->mapy) {
		memset(&(newmap.cells[x][y]), 0, sizeof(struct MapCell));
	    }
	    else {
		memcpy(&(newmap.cells[x][y]),
		   &(ns->lastmap.cells[x+dx][y+dy]),sizeof(struct MapCell));
	    }
	}
    }

    memcpy(&(ns->lastmap), &newmap,sizeof(struct Map));
    ns->sent_scroll = 1;
}

/*****************************************************************************/
/* GROS: The following one is used to allow a plugin to send a generic cmd to*/
/* a player. Of course, the client need to know the command to be able to    */
/* manage it !                                                               */
/*****************************************************************************/
void send_plugin_custom_message(object *pl, char *buf)
{
    cs_write_string(&pl->contr->socket,buf,strlen(buf));
}

/* This sends the skill number to name mapping.  We ignore
 * the params - we always send the same info no matter what.
  */
void send_skill_info(NewSocket *ns, char *params)
{
    SockList sl;
    int i;

    sl.buf = malloc(MAXSOCKBUF);
    strcpy(sl.buf,"replyinfo skill_info\n");
    for (i=1; i< NUM_SKILLS; i++) {
	sprintf(sl.buf + strlen(sl.buf), "%d:%s\n", i + CS_STAT_SKILLINFO,
		skill_names[i]);
    }
    sl.len = strlen(sl.buf);
    if (sl.len > MAXSOCKBUF) {
	LOG(llevError,"Buffer overflow in send_skill_info!\n");
	fatal(0);
    }
    Send_With_Handling(ns, &sl);
    free(sl.buf);
}

/*
 * static char *rcsid_init_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net
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
    CS_STAT_RES_BLIND,    -1 /* Blind */
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
    for(s=0;;) {
	if(s>=len)	/* ugly, but for secure...*/
	    break;

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
	else if (!strcmp(cmd,"sexp")) {
	    ns->skillexp = atoi(param);
	    strcat(cmdback, param);
	} else if (!strcmp(cmd,"map1cmd")) {
	    ns->map1cmd = atoi(param);
	    /* if beyond this size, need to use map1cmd no matter what */
	    if (ns->mapx>11 || ns->mapy>11) ns->map1cmd=1;
	    strcat(cmdback, ns->map1cmd?"1":"0");
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
		/* If beyond this size, will use map1cmd */
		if (x>11 || y>11) ns->map1cmd=1;
	    }
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
	pl->count_left=0;
	buf=strchr(buf,' ');	/* advance beyond the numbers */
	if (!buf) {
#ifdef ESRV_DEBUG
	    LOG(llevDebug,"PlayerCmd: Got count but no command.");
#endif
	    return;
	}
	buf++;
    }
    pl->idle=0;
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

    pl->count_left=0;
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
	pl->count_left=0;
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

    pl->idle=0;

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
    pl->count_left=0;
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

/* Client tells its its version.  If there is a mismatch, we close the
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

	if(!strcmp(" CF DX CLIENT", cp)) /* these are old dxclients */
	{
	    sprintf(version_warning,"drawinfo %d %s", NDI_RED, "**** VERSION WARNING ****\n**** CLIENT IS TO OLD!! UPDATE THE CLIENT!! ****");
	    Write_String_To_Socket(ns, version_warning, strlen(version_warning));
	}
    }
}

/*
 * Client tells us what type of faces it wants.  Also sets
 * the caching attribute.
 *
 */

void SetFaceMode(char *buf, int len, NewSocket *ns)
{
    int mask =(atoi(buf) & CF_FACE_CACHE), mode=(atoi(buf) & ~CF_FACE_CACHE);

    if (mode==CF_FACE_NONE) {
	ns->facemode=Send_Face_None;
    }
    else if (mode==CF_FACE_BITMAP) {
	ns->facemode=Send_Face_Bitmap;
    } else if (mode==CF_FACE_XPM) {
	ns->facemode=Send_Face_Pixmap;
    } else if (mode==CF_FACE_PNG) {
	ns->facemode=Send_Face_Png;
#ifdef ESRV_DEBUG
    } else {
	LOG(llevDebug,"SetFaceMode: Invalid mode from client: %d\n", mode);
#endif
    }
    if (mask) {
	ns->facecache=1;
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


/* client has requested pixmap that it somehow missed getting
 * This will be called often if the client is
 * caching images.
 */

void SendFaceCmd(char *buff, int len, player *pl)
{
	long tmpnum = atoi(buff);
	short facenum=tmpnum & 0xffff;

	if(facenum!=0)
		esrv_send_face(&pl->socket, facenum,1);
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

    sl.buf=malloc(MAX_BUF);
    strcpy((char*)sl.buf,"stats ");
    sl.len=strlen((char*)sl.buf);

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
    if(pl->last_stats.exp != pl->ob->stats.exp && pl->socket.skillexp) 
    {
	int s;
	for(s=0;s<pl->last_skill_index;s++)
        {
	    AddIfInt(pl->last_skill_exp[s],pl->last_skill_ob[s]->stats.exp , pl->last_skill_id[s]);
	    AddIfShort(pl->last_skill_level[s],pl->last_skill_ob[s]->level , pl->last_skill_id[s]+1);
        }
    }
    AddIfInt(pl->last_stats.exp, pl->ob->stats.exp, CS_STAT_EXP);
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
    if (anim_num < 0 || anim_num >= num_animations) {
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


/*
 * esrv_send_face sends a face to a client if they are in pixmap mode
 * nothing gets sent in bitmap mode. 
 * If nocache is true (nonzero), ignore the cache setting from the client -
 * this is needed for the askface, in which we really do want to send the
 * face (and askface is the only place that should be setting it).  Otherwise,
 * we look at the facecache, and if set, send the image name.
 */

void esrv_send_face(NewSocket *ns,short face_num, int nocache)
{
    SockList sl;



    if (face_num < 0 || face_num >= MAXFACENUM) {
	LOG(llevError,"esrv_send_face (%d) out of bounds??\n",face_num);
	return;
    }
    if (faces[face_num].data == NULL) {
	LOG(llevError,"esrv_send_face: faces[%d].data == NULL\n",face_num);
	return;
    }

    sl.buf = malloc(MAXSOCKBUF);

    if ((!nocache && ns->facecache) || ns->facemode==Send_Face_None ) {
	if (ns->sc_version >= 1026)
	    strcpy((char*)sl.buf, "face1 ");
	else
	    strcpy((char*)sl.buf, "face ");

	sl.len=strlen(sl.buf);
	SockList_AddShort(&sl, face_num);
	if (ns->sc_version >= 1026)
	    SockList_AddInt(&sl, faces[face_num].checksum);
	strcpy((char*)sl.buf + sl.len, new_faces[face_num].name);
	sl.len += strlen(new_faces[face_num].name);
	Send_With_Handling(ns, &sl);
    }
    else if (ns->facemode == Send_Face_Pixmap) {
	strcpy((char*)sl.buf, "pixmap ");
	sl.len=strlen((char*)sl.buf);
	SockList_AddInt(&sl, face_num);
	SockList_AddInt(&sl, faces[face_num].datalen[1]);
	memcpy(sl.buf+sl.len, faces[face_num].data[1], faces[face_num].datalen[1]);
	sl.len += faces[face_num].datalen[1];
/*	LOG(llevDebug,"sending pixmap %d, len %d\n", face_num, faces[face_num].datalen);*/
	Send_With_Handling(ns, &sl);
    } else if (ns->facemode == Send_Face_Bitmap) {
	strcpy((char*)sl.buf, "bitmap ");
	sl.len=strlen((char*)sl.buf);
	SockList_AddInt(&sl, face_num);
	SockList_AddChar(&sl, new_faces[face_num].fg);
	SockList_AddChar(&sl, new_faces[face_num].bg);
	memcpy(sl.buf+sl.len, faces[face_num].data[0], faces[face_num].datalen[0]);
	memcpy(sl.buf+sl.len, faces[face_num].data[0], 3*24);
	sl.len += faces[face_num].datalen[0];
	Send_With_Handling(ns, &sl);
    }
    else if (ns->facemode == Send_Face_Png) {
	strcpy((char*)sl.buf, "image ");
	sl.len=strlen((char*)sl.buf);
	SockList_AddInt(&sl, face_num);
	SockList_AddInt(&sl, faces[face_num].datalen[2]);
	memcpy(sl.buf+sl.len, faces[face_num].data[2], faces[face_num].datalen[2]);
	sl.len += faces[face_num].datalen[2];
/*	LOG(llevDebug,"sending png %d, len %d\n", face_num, faces[face_num].datalen);*/
	Send_With_Handling(ns, &sl);
    } else {
	LOG(llevError,"Invalid face send mode on client_num (%d)\n",
	    ns->facemode);
    }
    ns->faces_sent[face_num] = 1;
    free(sl.buf);
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
    if(newmap->cells[x][y].count >= MAXMAPCELLFACES) {
	LOG(llevError,"Too many faces in map cell %d %d\n",x,y);
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
    struct MapLayer layers[MAXMAPCELLFACES];
  
    for(k = 0;k<MAXMAPCELLFACES;k++)
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

    cur = compactlayer(ns,sl.buf+sl.len,MAXMAPCELLFACES,newmap);
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
    int x,y,ax, ay, d, face_num1,face_num2,face_num3;
    SockList sl;
    uint16  mask;
    New_Face	*face;

    sl.buf=malloc(MAXSOCKBUF);
    strcpy((char*)sl.buf,"map1 ");
    sl.len=strlen((char*)sl.buf);


    /* x,y are the real map locations.  ax, ay are viewport relative
     * locations.
     */
    ay=0;
    for(y=pl->y-pl->contr->socket.mapy/2; y<=pl->y+pl->contr->socket.mapy/2;y++,ay++) {
	ax=0;
	for(x=pl->x-pl->contr->socket.mapx/2;x<=pl->x+pl->contr->socket.mapx/2;x++,ax++) {
	    d =  pl->contr->blocked_los[ax][ay];

	    /* If the coordinates are not valid, or it is too dark to see,
	     * we tell the client as such
	     */
	    if (out_of_map(pl->map, x, y) || d > 3) {
		/* Was not blocked before, so we need to update client */
		if (pl->contr->socket.lastmap.cells[ax][ay].count!=-1) {
		    mask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
		    SockList_AddShort(&sl, mask);
		    pl->contr->socket.lastmap.cells[ax][ay].count=-1;
#if 0
		    pl->contr->socket.lastmap.cells[ax][ay].faces[0] = blank_face->number;
		    pl->contr->socket.lastmap.cells[ax][ay].faces[1] = blank_face->number;
		    pl->contr->socket.lastmap.cells[ax][ay].faces[2] = blank_face->number;
#else
		    pl->contr->socket.lastmap.cells[ax][ay].faces[0] = 0;
		    pl->contr->socket.lastmap.cells[ax][ay].faces[1] = 0;
		    pl->contr->socket.lastmap.cells[ax][ay].faces[2] = 0;
#endif
		}
	    }
	    else { /* this space is viewable */
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
		int oldlen = sl.len;
		mask = (ax & 0x3f) << 10 | (ay & 0x3f) << 4;
		SockList_AddShort(&sl, mask);

		/* Darkness changed */
		if (pl->contr->socket.lastmap.cells[ax][ay].count != d) {
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
		/* Check to see if floor face ahs changed */
		face = get_map_floor(pl->map, x,y)->face;
		if (face == blank_face) face_num1=0;
		else face_num1 = face->number;

		if (pl->contr->socket.lastmap.cells[ax][ay].faces[0] != face_num1) {
		    mask |= 0x4;    /* floor bit */
		    pl->contr->socket.lastmap.cells[ax][ay].faces[0] = face_num1;
		    SockList_AddShort(&sl, face_num1);
		    if (pl->contr->socket.faces_sent[face_num1] == 0)
			esrv_send_face(&pl->contr->socket,face_num1,0);
		}

		face = get_map_floor2(pl->map, x,y)->face;
		if (face == blank_face) face_num2=0;
		else face_num2 = face->number;

		if (pl->contr->socket.lastmap.cells[ax][ay].faces[1] != face_num2) {
		    mask |= 0x2;    /* middle bit */
		    pl->contr->socket.lastmap.cells[ax][ay].faces[1] = face_num2;
		    SockList_AddShort(&sl, face_num2);
		    if (pl->contr->socket.faces_sent[face_num2] == 0)
			esrv_send_face(&pl->contr->socket,face_num2,0);
		}

		face = get_map(pl->map, x,y)->face;
		if (face == blank_face) face_num3=0;
		else face_num3 = face->number;

		if (pl->contr->socket.lastmap.cells[ax][ay].faces[2] != face_num3) {
		    mask |= 0x1;    /* top bit */
		    pl->contr->socket.lastmap.cells[ax][ay].faces[2] = face_num3;
		    SockList_AddShort(&sl, face_num3);
		    if (pl->contr->socket.faces_sent[face_num3] == 0)
			esrv_send_face(&pl->contr->socket,face_num3,0);
		}
		if ((face_num1 == face_num2 && face_num1 !=0) || 
		    (face_num2 == face_num3 && face_num2 !=0) || 
		    (face_num1 == face_num3 && face_num1 !=0)) {
		    fprintf(stderr,"faces match: %d %d %d\n", face_num1, face_num2, face_num3);
		}

		/* Lets see if the mask is in fact different.  If, we need to
		 * update it, if not, we back it out
		 */
		if (mask & 0xf) {
		    /* yep - different. Only need to update the second byte
		     * because that is the only one that changed.
		     */
		    sl.buf[oldlen+1] = mask & 0xff;
		} else {
		    /* no change, so just reset the len */
		    sl.len = oldlen;
		}
	    }
	} /* for x loop */
    } /* for y loop */
		    
    /* Verify that we in fact do need to send this */
    if (sl.len>strlen("map1 ") || pl->contr->socket.sent_scroll) {
	Send_With_Handling(&pl->contr->socket, &sl);
	pl->contr->socket.sent_scroll = 0;
    }
    free(sl.buf);
}


void draw_client_map(object *pl)
{
    int i,j,ax,ay; /* ax and ay goes from 0 to max-size of arrays */
    New_Face	*face,*floor;
    New_Face	*floor2;
    int d;
    struct Map	newmap;

    if (pl->type != PLAYER) {
	LOG(llevError,"draw_client_map called with non player/non eric-server\n");
	return;
    }

    /* IF player is just joining the game, he isn't here yet, so the map
     * can get swapped out.  If so, don't try to send them a map.  All will
     * be OK once they really log in.
     */
    if (pl->map->in_memory!=MAP_IN_MEMORY) return;
    memset(&newmap, 0, sizeof(struct Map));

    if (pl->contr->socket.map1cmd) {
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
     */
    ay=0;
    for(j=pl->y-pl->contr->socket.mapy/2; j<=pl->y+pl->contr->socket.mapy/2;j++, ay++) {
	ax=0;
	for(i=pl->x-pl->contr->socket.mapx/2;i<=pl->x+pl->contr->socket.mapx/2;i++, ax++) {

	    d =  pl->contr->blocked_los[ax][ay];
	    /* note the out_of_map and d>3 checks are both within the same
	     * negation check.
	     */
	    if (!(out_of_map(pl->map,i,j) || d>3)) {
		face = get_map(pl->map, i, j)->face;
		floor = get_map_floor(pl->map, i,j)->face;
		floor2 = get_map_floor2(pl->map, i,j)->face;

		/* If all is blank, send a blank face. */
		if (face == blank_face &&
		    floor2 == blank_face &&
		    floor == blank_face) {
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
				blank_face->number,&newmap);
		} else { /* actually have something interesting */
		  /* send the darkness mask, if any */
		  if (d ) 
		    esrv_map_setbelow(&pl->contr->socket,ax,ay,
						  dark_faces[d-1]->number,&newmap);

		    if (face != blank_face)
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
					  face->number,&newmap);
		    if (floor2 != blank_face)
			esrv_map_setbelow(&pl->contr->socket,ax,ay,
				floor2->number,&newmap);
		    if (floor != blank_face)
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
    int x,y;
    char buf[MAXSOCKBUF];

    sprintf(buf,"map_scroll %d %d", dx, dy);
    Write_String_To_Socket(ns, buf, strlen(buf));
    /* the x and y here are coordinates for the new map, i.e. if we moved
     (dx,dy), newmap[x][y] = oldmap[x-dx][y-dy] */
    for(x=0;x<ns->mapx;x++) {
	for(y=0;y<ns->mapy;y++) {
	    newmap.cells[x][y].count = 0;
	    if (x+dx < 0 || x+dx >= ns->mapx)
		continue;
	    if (y+dy < 0 || y+dy >= ns->mapy)
		continue;
	    memcpy(&(newmap.cells[x][y]),
		   &(ns->lastmap.cells[x+dx][y+dy]),sizeof(struct MapCell));
	}
    }
    memcpy(&(ns->lastmap), &newmap,sizeof(struct Map));
    ns->sent_scroll = 1;
}



/*
 * static char *rcsid_loop_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & The Crossfire Development Team
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

/* socket.c mainly deals with initialization and higher level socket
 * maintenance (checking for lost connections and if data has arrived.)
 * The reading of data is handled in ericserver.c
 */


#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#include <sockproto.h>
#endif

#ifndef WIN32 /* ---win32 exclude unix headers */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif /* end win32 */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#include <newserver.h>

/*****************************************************************************
 * Start of command dispatch area.
 * The commands here are protocol commands. 
 ****************************************************************************/

/* Either keep this near the start or end of the file so it is
 * at least reasonablye easy to find.
 * There are really 2 commands - those which are sent/received
 * before player joins, and those happen after the player has joined.
 * As such, we have function types that might be called, so
 * we end up having 2 tables.
 */

typedef void (*func_uint8_int_ns) (char*, int, NewSocket *);

struct NsCmdMapping {
    char *cmdname;
    func_uint8_int_ns  cmdproc;
};

typedef void (*func_uint8_int_pl)(char*, int, player *);
struct PlCmdMapping {
    char *cmdname;
    func_uint8_int_pl cmdproc;
    uint8   flag;
};

/*
 * CmdMapping is the dispatch table for the server, used in HandleClient,
 * which gets called when the client has input.  All commands called here 
 * use the same parameter form (char* data, int len, int clientnum.
 * We do implicit casts, because the data that is being passed is
 * unsigned (pretty much needs to be for binary data), however, most
 * of these treat it only as strings, so it makes things easier
 * to cast it here instead of a bunch of times in the function itself.
 * flag is 1 if the player must be in the playing state to issue the
 * command, 0 if they can issue it at any time.
 */
static struct PlCmdMapping plcommands[] = {
    { "examine",	ExamineCmd,	1},
    { "apply",		ApplyCmd,	1},
    { "move",		MoveCmd,	1},
    { "reply",		ReplyCmd,	0},
    { "command",	PlayerCmd,	1},
    { "ncom",		(func_uint8_int_pl)NewPlayerCmd, 1},
    { "lookat",		LookAt,		1},
    { "lock",		(func_uint8_int_pl)LockItem,	1},
    { "mark",		(func_uint8_int_pl)MarkItem,	1},
    { "mapredraw",	MapRedrawCmd,	0},	/* Added: phil */
    { NULL, NULL}	/* terminator */
};

static struct NsCmdMapping nscommands[] = {
    { "addme",		AddMeCmd },
    { "askface",	SendFaceCmd},	/* Added: phil */
    { "requestinfo",	RequestInfo},
    { "setfacemode",	SetFaceMode},
    { "setsound",	SetSound},
    { "setup",		SetUp},
    { "version",	VersionCmd },
    { NULL, NULL}	/* terminator */
};

/* RequestInfo is sort of a meta command - there is some specific
 * request of information, but we call other functions to provide
 * that information.
 */
void RequestInfo(char *buf, int len, NewSocket *ns)
{
    char    *params=NULL, *cp;
    /* No match */
    char bigbuf[MAX_BUF];
    int slen;

    /* Set up replyinfo before we modify any of the buffers - this is used
     * if we don't find a match.
     */
    strcpy(bigbuf,"replyinfo ");
    slen = strlen(bigbuf);
    safe_strcat(bigbuf, buf, &slen, MAX_BUF);

    /* find the first space, make it null, and update the
     * params pointer.
     */
    for (cp = buf; *cp != '\0'; cp++)
	if (*cp==' ') {
	    *cp = '\0';
	    params = cp + 1;
	    break;
	}
    if (!strcmp(buf, "image_info")) send_image_info(ns, params);
    else if (!strcmp(buf,"image_sums")) send_image_sums(ns, params);
    else Write_String_To_Socket(ns, bigbuf, len);
}

void Handle_Oldsocket(NewSocket *ns)
{
    int stat,i;
    CommFunc	command;
    char buf[MAX_BUF],*cp;
    object ob;
    player pl;

    /* This is not the most efficient block, but keeps the code simpler -
     * we basically read a byte at a time until we get a newline, error,
     * or no more characters to read.
     */
    do {
#ifdef WIN32 /* ***win32: change oldsocket read() to recv() */
		stat = recv(ns->fd, ns->inbuf.buf + ns->inbuf.len, 1,0);

	if (stat==-1 && WSAGetLastError() !=WSAEWOULDBLOCK) {
#else
	do {
	    stat = read(ns->fd, ns->inbuf.buf + ns->inbuf.len, 1);
	} while ((stat<0) && (errno == EINTR));

	if (stat<0 && errno != EAGAIN && errno !=EWOULDBLOCK) {
#endif	
	    perror("Handle_Oldsocket got an error.");
	    ns->status = Ns_Dead;
	    return;
	}
	if (stat == 0) return;
	if (stat == MAXSOCKBUF-1) {
	    ns->status = Ns_Dead;
	    LOG(llevDebug, "Old input socket sent too much data without newline\n");
	    return;
	}
    } while (ns->inbuf.buf[ns->inbuf.len++]!='\n');

    ns->inbuf.buf[ns->inbuf.len]=0;

    cp = strchr(ns->inbuf.buf, ' ');
    if (cp) {
	/* Replace the space with a null, skip any more spaces */
	*cp++=0;
	while (isspace(*cp)) cp++;
    }

    /* Strip off all spaces and control characters from end of line */
    for (i=ns->inbuf.len-1; i>=0; i--) {
	if (ns->inbuf.buf[i]<=32) ns->inbuf.buf[i]=0;
	else break;
    }
    ns->inbuf.len=0;	/* reset for next read */

    /* If just a return, don't do anything */
    if (ns->inbuf.buf[0] == 0) return;
    if (!strcasecmp(ns->inbuf.buf,"quit")) {
	ns->status = Ns_Dead;
	return;
    }
    if (!strcasecmp(ns->inbuf.buf, "listen")) {
	if (ns->comment) free(ns->comment);
	if (cp) {
	    char *buf="Socket switched to listen mode\n";

	    ns->comment = strdup_local(cp);
	    ns->old_mode = Old_Listen;
	    cs_write_string(ns, buf, strlen(buf));
	} else {
	    char *buf="Need to supply a comment/url to listen\n";
	    cs_write_string(ns, buf, strlen(buf));
	}
	return;
    }
    if (!strcasecmp(ns->inbuf.buf, "name")) {
	char *cp1=NULL;
	if (cp) cp1= strchr(cp, ' ');
	if (cp1) {
	    *cp1++ = 0;
	    while (isspace(*cp1)) cp1++;
	}
	if (!cp || !cp1) {
	    char *buf="Need to provide a name/password to name\n";
	    cs_write_string(ns, buf, strlen(buf));
	    return;
	}

	if (!verify_player(cp, cp1)) {
	    char *buf="Welcome back\n";
	    if (ns->comment) free(ns->comment);
	    ns->comment = strdup_local(cp);
	    ns->old_mode = Old_Player;
	    cs_write_string(ns, buf, strlen(buf));
	}
	else {
	    char *buf="Could not login you in.  Check your name and password.\n";
	    cs_write_string(ns, buf, strlen(buf));
	}
	return;
    }

    command = find_oldsocket_command(ns->inbuf.buf);
    if (!command && ns->old_mode==Old_Player) {
	command = find_oldsocket_command2(ns->inbuf.buf);
    }
    if (!command) {
	sprintf(buf,"Could not find command: %s\n", ns->inbuf.buf);
	cs_write_string(ns, buf, strlen(buf));
	return;
    }

    /* This is a bit of a hack, but works.  Basically, we make some
     * fake object and player pointers and give at it.
     * This works as long as the functions we are calling don't need
     * to do anything to the object structure (ie, they are only
     * outputting information and not actually updating anything much.)
     */
    ob.contr = &pl;
    pl.ob = &ob;
    ob.type = PLAYER;
    pl.listening = 10;
    pl.socket = *ns;
    pl.outputs_count = 1;
    ob.name = ns->comment;

    command(&ob, cp);
}


/* HandleClient is actually not named really well - we only get here once
 * there is input, so we don't do exception or other stuff here.
 * sock is the output socket information.  pl is the player associated
 * with this socket, null if no player (one of the init_sockets for just
 * starting a connection)
 */

void HandleClient(NewSocket *ns, player *pl)
{
    int len=0,i;
    unsigned char *data;

    /* Loop through this - maybe we have several complete packets here. */
    while (1) {
	/* If it is a player, and they don't have any speed left, we
	 * return, and will read in the data when they do have time.
         */
	if (pl && pl->state==ST_PLAYING && pl->ob != NULL && pl->ob->speed_left < 0) {
	    return;
	}
	    
	if (ns->status == Ns_Old) {
	    Handle_Oldsocket(ns);
	    return;
	}
	i=SockList_ReadPacket(ns->fd, &ns->inbuf, MAXSOCKBUF-1);
	/* Special hack - let the user switch to old mode if in the Ns_Add
	 * phase.  Don't demand they add in the special length bytes
	 */
	if (ns->status == Ns_Add) {
	    if (!strncasecmp(ns->inbuf.buf,"oldsocketmode", 13)) {
		ns->status = Ns_Old;
		ns->inbuf.len=0;
		cs_write_string(ns, "Switched to old socket mode\n", 28);
		LOG(llevDebug,"Switched socket to old socket mode\n");
		return;
	    }
	}

	if (i<0) {
#ifdef ESRV_DEBUG
	    LOG(llevDebug,"HandleClient: Read error on connection player %s\n", (pl?pl->ob->name:"None"));
#endif
	    /* Caller will take care of cleaning this up */
	    ns->status =Ns_Dead;
	    return;
	}
	/* Still dont have a full packet */
	if (i==0) return;

	/* First, break out beginning word.  There are at least
	 * a few commands that do not have any paremeters.  If
	 * we get such a command, don't worry about trying
	 * to break it up.
	 */
	data = (unsigned char *)strchr((char*)ns->inbuf.buf +2, ' ');
	if (data) {
	    *data='\0';
	    data++;
	    len = ns->inbuf.len - (data - ns->inbuf.buf);
	}
	else len=0;

	ns->inbuf.buf[ns->inbuf.len]='\0';  /* Terminate buffer - useful for string data */
	for (i=0; nscommands[i].cmdname !=NULL; i++) {
	    if (strcmp((char*)ns->inbuf.buf+2,nscommands[i].cmdname)==0) {
		nscommands[i].cmdproc((char*)data,len,ns);
		ns->inbuf.len=0;
		return;
	    }
	}
	/* Player must be in the playing state or the flag on the
	 * the command must be zero for the user to use the command -
	 * otherwise, a player cam save, be in the play_again state, and
	 * the map they were on getsswapped out, yet things that try to look
	 * at the map causes a crash.  If the command is valid, but
	 * one they can't use, we still swallow it up.
	 */
	if (pl) for (i=0; plcommands[i].cmdname !=NULL; i++) {
	    if (strcmp((char*)ns->inbuf.buf+2,plcommands[i].cmdname)==0) {
		if (pl->state == ST_PLAYING || plcommands[i].flag == 0)
		    plcommands[i].cmdproc((char*)data,len,pl);
		ns->inbuf.len=0;
		return;
	    }
	}
	/* If we get here, we didn't find a valid command.  Logging
	 * this might be questionable, because a broken client/malicious
	 * user could certainly send a whole bunch of invalid commands.
	 */
	LOG(llevDebug,"Bad command from client (%s)\n",ns->inbuf.buf+2);
    }
}


/*****************************************************************************
 *
 * Low level socket looping - select calls and watchdog udp packet
 * sending.
 *
 ******************************************************************************/

#ifdef WATCHDOG
/*
 * Tell watchdog that we are still alive
 * I put the function here since we should hopefully already be getting
 * all the needed include files for socket support 
 */

void watchdog(void)
{
  static int fd=-1;
  static struct sockaddr_in insock;

  if (fd==-1)
    {
      struct protoent *protoent;

      if ((protoent=getprotobyname("udp"))==NULL ||
        (fd=socket(PF_INET, SOCK_DGRAM, protoent->p_proto))==-1)
      {
        return;
      }
      insock.sin_family=AF_INET;
      insock.sin_port=htons((unsigned short)13325);
      insock.sin_addr.s_addr=inet_addr("127.0.0.1");
    }
  sendto(fd,(void *)&fd,1,0,(struct sockaddr *)&insock,sizeof(insock));
}
#endif

extern unsigned long todtick;

static void block_until_new_connection()
{

    struct timeval Timeout;
    fd_set readfs;
    int cycles;

    LOG(llevInfo, "Waiting for connections...\n");

    cycles=1;
    do {
	/* Every minutes is a bit often for updates - especially if nothing is going
	 * on.  This slows it down to every 6 minutes.
	 */
	cycles++;
	if (cycles%2 == 0)
	    tick_the_clock();
	if (cycles == 7) {
	    metaserver_update();
	    cycles=1;
	}
	FD_ZERO(&readfs);
	FD_SET((uint32)init_sockets[0].fd, &readfs);
	if (settings.fastclock > 0) {
	    Timeout.tv_sec=0;
	    Timeout.tv_usec=50;
	} else {
	    Timeout.tv_sec=60;
	    Timeout.tv_usec=0;
	}
#ifdef WATCHDOG
	watchdog();
#endif
	flush_old_maps();
	}
    while (select(socket_info.max_filedescriptor, &readfs, NULL, NULL, &Timeout)==0);

    reset_sleep(); /* Or the game would go too fast */
}


/* This checks the sockets for input and exceptions, does the right thing.  A 
 * bit of this code is grabbed out of socket.c
 * There are 2 lists we need to look through - init_sockets is a list
 * 
 */
void doeric_server()
{
    int i, pollret;
    fd_set tmp_read, tmp_exceptions, tmp_write;
    struct sockaddr_in addr;
    int addrlen=sizeof(struct sockaddr);
    player *pl, *next;

#ifdef CS_LOGSTATS
    if ((time(NULL)-cst_lst.time_start)>=CS_LOGTIME)
	write_cs_stats();
#endif

    FD_ZERO(&tmp_read);
    FD_ZERO(&tmp_write);
    FD_ZERO(&tmp_exceptions);

    for(i=0;i<socket_info.allocated_sockets;i++) {
	if (init_sockets[i].status == Ns_Dead) {
	    free_newsocket(&init_sockets[i]);
	    init_sockets[i].status = Ns_Avail;
	    socket_info.nconns--;
	} else if (init_sockets[i].status != Ns_Avail){
	    FD_SET((uint32)init_sockets[i].fd, &tmp_read);
	    FD_SET((uint32)init_sockets[i].fd, &tmp_write);
	    FD_SET((uint32)init_sockets[i].fd, &tmp_exceptions);
	}
    }

    /* Go through the players.  Let the loop set the next pl value,
     * since we may remove some
     */
    for (pl=first_player; pl!=NULL; ) {
	if (pl->socket.status == Ns_Dead) {
	    player *npl=pl->next;

	    save_player(pl->ob, 0);
	    if(!QUERY_FLAG(pl->ob,FLAG_REMOVED)) {
		terminate_all_pets(pl->ob);
		remove_ob(pl->ob);
	    }
	    leave(pl,1);
	    final_free_player(pl);
	    pl=npl;
	}
	else {
	    FD_SET((uint32)pl->socket.fd, &tmp_read);
	    FD_SET((uint32)pl->socket.fd, &tmp_write);
	    FD_SET((uint32)pl->socket.fd, &tmp_exceptions);
	    pl=pl->next;
	}
    }

    if (socket_info.nconns==1 && first_player==NULL) 
	block_until_new_connection();

    /* Reset timeout each time, since some OS's will change the values on
     * the return from select.
     */
    socket_info.timeout.tv_sec = 0;
    socket_info.timeout.tv_usec = 0;

    pollret= select(socket_info.max_filedescriptor, &tmp_read, &tmp_write, 
		    &tmp_exceptions, &socket_info.timeout);

    if (pollret==-1) {
	perror("doeric_server: error on select");
	LOG(llevError,"doeric_server: error on select\n");
	return;
    }

    /* We need to do some of the processing below regardless */
/*    if (!pollret) return;*/

    /* Following adds a new connection */
    if (pollret && FD_ISSET(init_sockets[0].fd, &tmp_read)) {
	int newsocknum=0;

#ifdef ESRV_DEBUG
	LOG(llevDebug,"doeric_server: New Connection\n");
#endif
	/* If this is the case, all sockets currently in used */
	if (socket_info.allocated_sockets <= socket_info.nconns) {
	    init_sockets = realloc(init_sockets,sizeof(NewSocket)*(socket_info.nconns+1));
	    if (!init_sockets) fatal(OUT_OF_MEMORY);
	    newsocknum = socket_info.allocated_sockets;
	    socket_info.allocated_sockets++;
	    init_sockets[newsocknum].status = Ns_Avail;
	}
	else {
	    int j;

	    for (j=1; j<socket_info.allocated_sockets; j++)
		if (init_sockets[j].status == Ns_Avail) {
		    newsocknum=j;
		    break;
		}
	}
	init_sockets[newsocknum].fd=accept(init_sockets[0].fd, (struct sockaddr *)&addr, &addrlen);
	if (init_sockets[newsocknum].fd==-1) {
	    perror("doeric_server: error on accept");
	    LOG(llevError,"doeric_server: error on accept\n");
	}
	else {
	    InitConnection(&init_sockets[newsocknum],ntohl(addr.sin_addr.s_addr));
	    socket_info.nconns++;
	}
    }

    /* Check for any exceptions/input on the sockets */
    if (pollret) for(i=1;i<socket_info.allocated_sockets;i++) {
	if (init_sockets[i].status == Ns_Avail) continue;
	if (FD_ISSET(init_sockets[i].fd,&tmp_exceptions)) {
	    free_newsocket(&init_sockets[i]);
	    init_sockets[i].status = Ns_Avail;
	    socket_info.nconns--;
	    continue;
	}
	if (FD_ISSET(init_sockets[i].fd, &tmp_read)) {
	    HandleClient(&init_sockets[i], NULL);
	}
	if (FD_ISSET(init_sockets[i].fd, &tmp_write)) {
	    init_sockets[i].can_write=1;
	}
    }

    /* This does roughly the same thing, but for the players now */
    for (pl=first_player; pl!=NULL; pl=next) {

	next=pl->next;
	if (pl->socket.status==Ns_Dead) continue;

	if (FD_ISSET(pl->socket.fd,&tmp_write)) {
	    if (!pl->socket.can_write)  {
#if 0
		LOG(llevDebug,"Player %s socket now write enabled\n", pl->ob->name);
#endif
		pl->socket.can_write=1;
		write_socket_buffer(&pl->socket);
	    }
	    /* if we get an error on the write_socket buffer, no reason to
	     * continue on this socket.
	     */
	    if (pl->socket.status==Ns_Dead) continue;
	}
	else 	    pl->socket.can_write=0;

	if (FD_ISSET(pl->socket.fd,&tmp_exceptions)) {
	    save_player(pl->ob, 0);
	    if(!QUERY_FLAG(pl->ob,FLAG_REMOVED)) {
		terminate_all_pets(pl->ob);
		remove_ob(pl->ob);
	    }
	    leave(pl,1);
	    final_free_player(pl);
	}
	else {
	    HandleClient(&pl->socket, pl);
	    /* If the player has left the game, then the socket status
	     * will be set to this be the leave function.  We don't
	     * need to call leave again, as it has already been called
	     * once.
	     */
	    if (pl->socket.status==Ns_Dead) {
		save_player(pl->ob, 0);
		if(!QUERY_FLAG(pl->ob,FLAG_REMOVED)) {
		    terminate_all_pets(pl->ob);
		    remove_ob(pl->ob);
		}
		leave(pl,1);
		final_free_player(pl);
	    } else {

		/* Update the players stats once per tick.  More efficient than
		 * sending them whenever they change, and probably just as useful
		 */
		esrv_update_stats(pl);
		draw_client_map(pl->ob);
		if (pl->socket.update_look) esrv_draw_look(pl->ob);
	    }
	}
    }
}

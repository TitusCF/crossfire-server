/*
 * static char *rcsid_init_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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
 * Socket general functions
 *
 * \date 2003-12-02
 *
 * Mainly deals with initialization and higher level socket
 * maintenance (checking for lost connections and if data has arrived.)
 * The reading of data is handled in ericserver.c
 */


#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#ifndef WIN32 /* ---win32 exclude include files */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif /* win32 */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <newserver.h>
#include <loader.h>

Socket_Info socket_info;
socket_struct* init_sockets;

/**
 * Initializes a connection. Really, it just sets up the data structure,
 * socket setup is handled elsewhere.  We do send a version to the
 * client.
 */
void init_connection(socket_struct *ns, const char *from_ip)
{
    SockList sl;
    unsigned char buf[256];
    int	bufsize=65535;	/*Supposed absolute upper limit */
    int oldbufsize;
    socklen_t buflen=sizeof(int);

#ifdef WIN32 /* ***WIN32 SOCKET: init win32 non blocking socket */
	int temp = 1;	

	if(ioctlsocket(ns->fd, FIONBIO , &temp) == -1)
		LOG(llevError,"init_connection:  Error on ioctlsocket.\n");
#else 
    if (fcntl(ns->fd, F_SETFL, O_NONBLOCK)==-1) {
		LOG(llevError,"init_connection:  Error on fcntl.\n");
    }
#endif /* end win32 */

    if (getsockopt(ns->fd,SOL_SOCKET,SO_SNDBUF, (char*)&oldbufsize, &buflen)==-1)
	oldbufsize=0;
    if (oldbufsize<bufsize) {
#ifdef ESRV_DEBUG
	LOG(llevDebug, "Default buffer size was %d bytes, will reset it to %d\n", oldbufsize, bufsize);
#endif
	if(setsockopt(ns->fd,SOL_SOCKET,SO_SNDBUF, (char*)&bufsize, sizeof(bufsize))) {
	    LOG(llevError,"init_connection: setsockopt unable to set output buf size to %d\n", bufsize);
	}
    }
    buflen=sizeof(oldbufsize);
    getsockopt(ns->fd,SOL_SOCKET,SO_SNDBUF, (char*)&oldbufsize, &buflen);
#ifdef ESRV_DEBUG
    LOG(llevDebug, "Socket buffer size now %d bytes\n", oldbufsize);
#endif

    ns->faceset = 0;
    ns->facecache = 0;
    ns->sound = 0;
    ns->exp64 = 0;
    ns->monitor_spells = 0;
    ns->mapmode = Map0Cmd;
    ns->darkness = 1;
    ns->status = Ns_Add;
    ns->mapx = 11;
    ns->mapy = 11;
    ns->newmapcmd= 0;
    ns->itemcmd = 1;	/* Default is version item1 command*/
    ns->ext_mapinfos=0; /*extendedmapinfo datas*/
    ns->EMI_smooth=0; 
    ns->look_position = 0;
    ns->update_look = 0;
    ns->has_readable_type = 0;
    ns->supported_readables = 0;
    ns->monitor_spells = 0;
    ns->tick=0;
    ns->is_bot = 0;
    ns->want_pickup = 0;

    /* we should really do some checking here - if total clients overflows
     * we need to do something more intelligent, because client id's will start
     * duplicating (not likely in normal cases, but malicous attacks that
     * just open and close connections could get this total up.
     */
    ns->inbuf.len=0;
    ns->inbuf.buf=malloc(MAXSOCKRECVBUF);
    /* Basic initialization. Needed because we do a check in
     * handle_client for oldsocketmode without checking the
     * length of data.
     */
    memset(ns->inbuf.buf, 0, MAXSOCKRECVBUF);
    memset(&ns->lastmap,0,sizeof(struct Map));
    if (!ns->faces_sent)
	ns->faces_sent =  calloc(sizeof(*ns->faces_sent),
			     nrofpixmaps);
    ns->faces_sent_len = nrofpixmaps;

    memset(&ns->anims_sent,0,sizeof(ns->anims_sent));
    memset(&ns->stats,0,sizeof(struct statsinfo));
    ns->map_scroll_x=0;
    ns->map_scroll_y=0;
    /* Do this so we don't send a face command for the client for
     * this face.  Face 0 is sent to the client to say clear
     * face information.
     */
    ns->faces_sent[0] = NS_FACESENT_FACE;

    ns->outputbuffer.start=0;
    ns->outputbuffer.len=0;
    ns->can_write=1;
    ns->password_fails = 0;

    ns->sent_scroll=0;
    ns->host=strdup_local(from_ip);
    sprintf((char*)buf, "version %d %d %s\n", VERSION_CS,VERSION_SC, VERSION_INFO);
    sl.buf=buf;
    sl.len=strlen((char*)buf);
    Send_With_Handling(ns, &sl);
#ifdef CS_LOGSTATS
    if (socket_info.nconns>cst_tot.max_conn)
	cst_tot.max_conn = socket_info.nconns;
    if (socket_info.nconns>cst_lst.max_conn)
	cst_lst.max_conn = socket_info.nconns;
#endif
}


/** This sets up the socket and reads all the image information into memory. */
void init_ericserver(void)
{
    struct sockaddr_in	insock;
    struct protoent  *protox;
    struct linger linger_opt;
    char err[MAX_BUF];

#ifdef WIN32 /* ***win32  -  we init a windows socket */
	WSADATA w;

	socket_info.max_filedescriptor = 1;	/* used in select, ignored in winsockets */
	WSAStartup (0x0101,&w);				/* this setup all socket stuff */
	/* ill include no error tests here, winsocket 1.1 should always work */
	/* except some old win95 versions without tcp/ip stack */
#else	/* non windows */

#ifdef HAVE_SYSCONF
  socket_info.max_filedescriptor = sysconf(_SC_OPEN_MAX);
#else
#  ifdef HAVE_GETDTABLESIZE
  socket_info.max_filedescriptor = getdtablesize();
#  else
  "Unable to find usable function to get max filedescriptors";
#  endif
#endif
#endif /* win32 */

    socket_info.timeout.tv_sec = 0;
    socket_info.timeout.tv_usec = 0;
    socket_info.nconns=0;

#ifdef CS_LOGSTATS
    memset(&cst_tot, 0, sizeof(CS_Stats));
    memset(&cst_lst, 0, sizeof(CS_Stats));
    cst_tot.time_start=time(NULL);
    cst_lst.time_start=time(NULL);
#endif

    LOG(llevDebug,"Initialize new client/server data\n");
    socket_info.nconns = 1;
    init_sockets = malloc(sizeof(socket_struct));
    init_sockets[0].faces_sent = NULL; /* unused */
    socket_info.allocated_sockets=1;

    protox = getprotobyname("tcp");
    if (protox==NULL) {
	LOG(llevError,"init_ericserver: Error getting protox\n");
	return;
    }
    init_sockets[0].fd = socket(PF_INET, SOCK_STREAM, protox->p_proto);
    if (init_sockets[0].fd == -1) {
	LOG(llevError, "Cannot create socket: %s\n", strerror_local(errno, err, sizeof(err)));
	exit(-1);
    }
    insock.sin_family = AF_INET;
    insock.sin_port = htons(settings.csport);
    insock.sin_addr.s_addr = htonl(INADDR_ANY);

    linger_opt.l_onoff = 0;
    linger_opt.l_linger = 0;
    if(setsockopt(init_sockets[0].fd,SOL_SOCKET,SO_LINGER,(char *) &linger_opt,
       sizeof(struct linger))) {
	LOG(llevError, "Cannot setsockopt(SO_LINGER): %s\n", strerror_local(errno, err, sizeof(err)));
    }
/* Would be nice to have an autoconf check for this.  It appears that
 * these functions are both using the same calling syntax, just one
 * of them needs extra valus passed.
 */
#if defined(__osf__) || defined(hpux) || defined(sgi) || defined(NeXT) || \
        defined(__sun__) || defined(__linux__) || defined(SVR4) || \
        defined(__FreeBSD__) || defined(__OpenBSD__) || \
        defined(WIN32) /* ---win32 add this here */  || \
        defined(__GNU__) /* HURD */
    {
#ifdef WIN32
    char tmp = 1;
#else
	int tmp =1;
#endif

	if(setsockopt(init_sockets[0].fd,SOL_SOCKET,SO_REUSEADDR, &tmp, sizeof(tmp))) {
	    LOG(llevError, "Cannot setsockopt(SO_REUSEADDR): %s\n", strerror_local(errno, err, sizeof(err)));
	}
    }
#else
    if(setsockopt(init_sockets[0].fd,SOL_SOCKET,SO_REUSEADDR,(char *)NULL,0)) {
	LOG(llevError, "Cannot setsockopt(SO_REUSEADDR): %s\n", strerror_local(errno, err, sizeof(err)));
    }
#endif

    if (bind(init_sockets[0].fd,(struct sockaddr *)&insock,sizeof(insock)) == (-1)) {
	LOG(llevError, "Cannot bind socket to port %d: %s\n", ntohs(insock.sin_port), strerror_local(errno, err, sizeof(err)));
#ifdef WIN32 /* ***win32: close() -> closesocket() */
	shutdown(init_sockets[0].fd,SD_BOTH);
	closesocket(init_sockets[0].fd);
#else
	close(init_sockets[0].fd);
#endif /* win32 */
	exit(-1);
    }
    if (listen(init_sockets[0].fd,5) == (-1))  {
	LOG(llevError, "Cannot listen on socket: %s\n", strerror_local(errno, err, sizeof(err)));
#ifdef WIN32 /* ***win32: close() -> closesocket() */
	shutdown(init_sockets[0].fd,SD_BOTH);
	closesocket(init_sockets[0].fd);
#else
	close(init_sockets[0].fd);
#endif /* win32 */
	exit(-1);
    }
    init_sockets[0].status=Ns_Add;
    read_client_images();
}


/*******************************************************************************
 *
 * Start of functions dealing with freeing of the data.
 *
 ******************************************************************************/

/** Free's all the memory that ericserver allocates. */
void free_all_newserver(void)
{  
    LOG(llevDebug,"Freeing all new client/server information.\n");
    free_socket_images();
    free(init_sockets);
}

/**
 * Frees a socket.
 * Basically, all we need to do here is free all data structures that
 * might be associated with the socket.  It is up to the caller to
 * update the list
 */

void free_newsocket(socket_struct *ns)
{
#ifdef WIN32 /* ***win32: closesocket in windows style */
	shutdown(ns->fd,SD_BOTH);
    if (closesocket(ns->fd)) {
#else
    if (close(ns->fd)) {
#endif /* win32 */

#ifdef ESRV_DEBUG
	LOG(llevDebug,"Error closing socket %d\n", ns->fd);
#endif
    }
    if (ns->stats.range)
	FREE_AND_CLEAR(ns->stats.range);
    if (ns->stats.title)
        FREE_AND_CLEAR(ns->stats.title);
    if (ns->host)
	FREE_AND_CLEAR(ns->host);
    if (ns->inbuf.buf)
	FREE_AND_CLEAR(ns->inbuf.buf);
}

/** Sends the 'goodbye' command to the player, and closes connection. */
void final_free_player(player *pl)
{
    cs_write_string(&pl->socket, "goodbye", 8);
    free_newsocket(&pl->socket);
    free_player(pl);
}


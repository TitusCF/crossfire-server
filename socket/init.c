
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

/* socket.c mainly deals with initialization and higher level socket
 * maintenance (checking for lost connections and if data has arrived.)
 * The reading of data is handled in ericserver.c
 */


#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#ifndef WIN32 //---win32 exclude include files
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif // win32

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <newserver.h>

Socket_Info socket_info;
NewSocket *init_sockets;
FaceInfo faces[MAXFACENUM];
static char *face_types[FACE_TYPES] = {"xbm", "xpm", "png"};


/* read_client_images loads all the iamge types into memory.
 *  This  way, we can easily send them to the client.  We should really do something
 * better than abort on any errors - on the other hand, these are all fatal
 * to the server (can't work around them), but the abort just seems a bit
 * messy (exit would probably be better.)
 */

/* Couple of notes:  We assume that the faces are in a continous block.
 * This works fine for now, but this could perhaps change in the future
 */

/* Function largely rewritten May 2000 to be more general purpose.
 * The server itself does not care what the image data is - to the server,
 * it is just data it needs to allocate.  As such, the code is written
 * to do such.
 */

/* Rotate right from bsd sum. */
#define ROTATE_RIGHT(c) if ((c) & 01) (c) = ((c) >>1) + 0x80000000; else (c) >>= 1;

#define XPM_BUF 10000
void read_client_images()
{
    char filename[400];
    char buf[500];
    char *cp;
    FILE *infile;
    int num,len,compressed, fileno,i;

    for (fileno=0; fileno<FACE_TYPES; fileno++) {
	sprintf(filename,"%s/crossfire.%s",settings.datadir, face_types[fileno]);
	LOG(llevDebug,"Loading image file %s\n", filename);

	if ((infile = open_and_uncompress(filename,0,&compressed))==NULL) {
	    LOG(llevError,"Unable to open %s\n", filename);
	    abort();
	}
	while(fgets(buf, 400, infile)!=NULL) {
	    if(strncmp(buf,"IMAGE ",6)!=0) {
		LOG(llevError,"read_client_images:Bad image line - not IMAGE, instead\n%s",buf);
		abort();
	    }
	    num = atoi(buf+6);
	    if (num<0 || num>=MAXFACENUM) {
		LOG(llevError,"read_client_images: Image num %d not in 0..%d\n%s",
		    num,MAXFACENUM,buf);
		abort();
	    }
	    /* Skip accross the number data */
	    for (cp=buf+6; *cp!=' '; cp++) ;
	    len = atoi(cp);
	    if (len==0 || len>XPM_BUF) {
		LOG(llevError,"read_client_images: length not valid: %d\n%s",
		    len,buf);
		abort();
	    }
	    /* First skip the space, then skip the numbers */
	    for (cp++; *cp!=' '; cp++) ;

	    /* Skip the space in our pointer*/
	    cp++;
	    /* Clear the newline */
	    buf[strlen(buf)-1] = '\0';
	    if (fileno==0) {
		if (faces[num].name != NULL) {
		    LOG(llevError,"read_client_images: duplicate image %d\n%s",
			num,buf);
		    abort();
		}
		faces[num].name = strdup_local(cp);
	    } else {
		/* Just do a sanity check here */
		if (strcmp(faces[num].name, cp)) {
		    LOG(llevError,"read_client_images: image mismatch: %s!=%s\n%s",
			faces[num].name, cp, buf);
		    abort();
		}
	    }
	    faces[num].datalen[fileno] = len;
	    faces[num].data[fileno] = malloc(len);
	    if ((i=fread( faces[num].data[fileno], len, 1, infile))!=1) {
		LOG(llevError,"read_client_images: Did not read desired amount of data, wanted %d, got %d\n%s",
		    len, i, buf);
		    abort();
	    }
	    faces[num].checksum=0;
	    for (i=0; i<len; i++) {
		ROTATE_RIGHT(faces[num].checksum);
		faces[num].checksum += faces[num].data[fileno][i];
		faces[num].checksum &= 0xffffffff;
	    }
	}
	close_and_delete(infile,compressed);
    }
}

/* Initializes a connection - really, it just sets up the data structure,
 * socket setup is handled elsewhere.  We do send a version to the
 * client.
 */
void InitConnection(NewSocket *ns, uint32 from)
{
    SockList sl;
    unsigned char buf[256];
    int	bufsize=65535;	/*Supposed absolute upper limit */
    int oldbufsize;
    int buflen=sizeof(int);

#ifdef WIN32 // ***WIN32 SOCKET: init win32 non blocking socket
	int temp = 1;	

	if(ioctlsocket(ns->fd, FIONBIO , &temp) == -1)
		LOG(llevError,"InitConnection:  Error on ioctlsocket.\n");
#else 
    if (fcntl(ns->fd, F_SETFL, O_NDELAY)==-1) {
		LOG(llevError,"InitConnection:  Error on fcntl.\n");
    }
#endif // end win32

    if (getsockopt(ns->fd,SOL_SOCKET,SO_SNDBUF, (char*)&oldbufsize, &buflen)==-1)
	oldbufsize=0;
    if (oldbufsize<bufsize) {
#ifdef ESRV_DEBUG
	LOG(llevDebug, "Default buffer size was %d bytes, will reset it to %d\n", oldbufsize, bufsize);
#endif
	if(setsockopt(ns->fd,SOL_SOCKET,SO_SNDBUF, (char*)&bufsize, sizeof(&bufsize))) {
	    LOG(llevError,"InitConnection: setsockopt unable to set output buf size to %d\n", bufsize);
	}
    }
    buflen=sizeof(oldbufsize);
    getsockopt(ns->fd,SOL_SOCKET,SO_SNDBUF, (char*)&oldbufsize, &buflen);
#ifdef ESRV_DEBUG
    LOG(llevDebug, "Socket buffer size now %d bytes\n", oldbufsize);
#endif

    ns->facemode = Send_Face_Pixmap;
    ns->facecache = 0;
    ns->sound = 0;
    ns->status = Ns_Add;
    ns->comment = NULL;
    ns->old_mode = 0;

    /* we should really do some checking here - if total clients overflows
     * we need to do something more intelligent, because client id's will start
     * duplicating (not likely in normal cases, but malicous attacks that
     * just open and close connections could get this total up.
     */
    ns->inbuf.len=0;
    ns->inbuf.buf=malloc(MAXSOCKBUF);
    /* Basic initialization. Needed because we do a check in
     * HandleClient for oldsocketmode without checking the
     * length of data.
     */
    ns->inbuf.buf[0] = 0;
    memset(&ns->lastmap,0,sizeof(struct Map));
    memset(&ns->faces_sent,0,sizeof(ns->faces_sent));
    memset(&ns->anims_sent,0,sizeof(ns->anims_sent));
    memset(&ns->stats,0,sizeof(struct statsinfo));
    ns->outputbuffer.start=0;
    ns->outputbuffer.len=0;
    ns->can_write=1;

    ns->sent_scroll=0;
    sprintf((char*)buf,"%d.%d.%d.%d",
          (from>>24)&255, (from>>16)&255, (from>>8)&255, from&255);
    ns->host=strdup_local((char*)buf);
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


/* This sets up the socket and reads all the image information into memory. */
void init_ericserver()
{
    struct sockaddr_in	insock;
    struct protoent  *protox;
    struct linger linger_opt;

#ifdef WIN32 // ***win32  -  we init a windows socket
	WSADATA w;

	socket_info.max_filedescriptor = 1;	// used in select, ignored in winsockets
	WSAStartup (0x0101,&w);				// this setup all socket stuuf
	// ill include no error tests here, winsocket 1.1 should always work
	// except some old win95 versions without tcp/ip stack
#else	// non windows

#ifdef HAVE_SYSCONF
  socket_info.max_filedescriptor = sysconf(_SC_OPEN_MAX);
#else
#  ifdef HAVE_GETDTABLESIZE
  socket_info.max_filedescriptor = getdtablesize();
#  else
  "Unable to find usable function to get max filedescriptors";
#  endif
#endif
#endif // win32

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
    init_sockets = malloc(sizeof(NewSocket));
    socket_info.allocated_sockets=1;

    protox = getprotobyname("tcp");
    if (protox==NULL) {
	LOG(llevError,"init_ericserver: Error getting protox\n");
	return;
    }
    init_sockets[0].fd = socket(PF_INET, SOCK_STREAM, protox->p_proto);
    if (init_sockets[0].fd == -1) {
	perror("Error create new client server socket.");
	LOG(llevError, "Error creating socket on port\n");
	exit(-1);
    }
    insock.sin_family = AF_INET;
    insock.sin_port = htons(settings.csport);
    insock.sin_addr.s_addr = htonl(INADDR_ANY);

    linger_opt.l_onoff = 0;
    linger_opt.l_linger = 0;
    if(setsockopt(init_sockets[0].fd,SOL_SOCKET,SO_LINGER,(char *) &linger_opt,
       sizeof(struct linger))) {
	perror("error on setsockopt LINGER");
	LOG(llevError, "Error on setsockopt LINGER\n");
    }
/* Would be nice to have an autoconf check for this.  It appears that
 * these functions are both using the same calling syntax, just one
 * of them needs extra valus passed.
 */
#if defined(__osf__) || defined(hpux) || defined(sgi) || defined(NeXT) || \
        defined(__sun__) || defined(linux) || defined(SVR4) || defined(__FreeBSD__) || \
	defined(__OpenBSD__) || defined(WIN32) // ---win32 add this here
    {
	char tmp =1;

	if(setsockopt(init_sockets[0].fd,SOL_SOCKET,SO_REUSEADDR, &tmp, sizeof(&tmp))) {
	    perror("error on setsockopt REUSEADDR");
	    LOG(llevError,"error on setsockopt REUSEADDR\n");
	}
    }
#else
    if(setsockopt(init_sockets[0].fd,SOL_SOCKET,SO_REUSEADDR,(char *)NULL,0)) {
	perror("error on setsockopt REUSEADDR");
	LOG(llevError,"error on setsockopt REUSEADDR\n");
    }
#endif

    if (bind(init_sockets[0].fd,(struct sockaddr *)&insock,sizeof(insock)) == (-1)) {
	perror("error on bind command");
	LOG(llevError,"error on bind command\n");
#ifdef WIN32 // ***win32: close() -> closesocket()
	shutdown(init_sockets[0].fd,SD_BOTH);
	closesocket(init_sockets[0].fd);
#else
	close(init_sockets[0].fd);
#endif // win32
	exit(-1);
    }
    if (listen(init_sockets[0].fd,5) == (-1))  {
	perror("error on listen");
	LOG(llevError,"error on listen\n");
#ifdef WIN32 // ***win32: close() -> closesocket()
	shutdown(init_sockets[0].fd,SD_BOTH);
	closesocket(init_sockets[0].fd);
#else
	close(init_sockets[0].fd);
#endif // win32
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

/* Free's all the memory that ericserver allocates. */
void free_all_ericserver()
{  
    int num;

#ifdef ESRV_DEBUG
    LOG(llevDebug,"Freeing all ericserver information.\n");
#endif
    for(num=0;num<MAXFACENUM;num++) {
	if (faces[num].name) {
	    free(faces[num].name);
	    free(faces[num].data);
	}
    }
    free(init_sockets);
}

/* basically, all we need to do here is free all data structures that
 * might be associated with the socket.  It is up to the caller to
 * update the list
 */

void free_newsocket(NewSocket *ns)
{
#ifdef WIN32 // ***win32: closesocket in windows style 
	shutdown(ns->fd,SD_BOTH);
    if (closesocket(ns->fd)) {
#else
    if (close(ns->fd)) {
#endif // win32

#ifdef ESRV_DEBUG
	LOG(llevDebug,"Error closing socket %d\n", ns->fd);
#endif
    }
    if (ns->stats.range)
	free(ns->stats.range);
    if (ns->stats.title)
	free(ns->stats.title);
    if (ns->comment)
	free(ns->comment);
    free(ns->host);
    free(ns->inbuf.buf);
}

void final_free_player(player *pl)
{
    cs_write_string(&pl->socket, "goodbye", 8);
    free_newsocket(&pl->socket);
    free_player(pl);
}



/*
 * static char *rcsid_init_c =
 *    "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to frankj@ifi.uio.no.
*/

/* socket.c mainly deals with initialization and higher level socket
 * maintenance (checking for lost connections and if data has arrived.)
 * The reading of data is handled in ericserver.c
 */


#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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

/* A throway function.  it reads a line, makes sure it is null terminated,
 * and strips off the newline.  Used by readclientimages
 */
static void readbufline(char *buf,int size,FILE *in)
{
  buf[0] = '\0';
  fgets(buf,size,in);
  buf[size-1] = '\0';
  if (buf[0] == '\0')
    return;
  if (buf[strlen(buf)-1] != '\n') {
    fprintf(stderr,"whoa, line '%s' not newline terminated??\n",buf);
    abort();
  }
}

/* read_client_images loads the xpm and pixmas file into memory.  This 
 * way, we can easily send them to the client.  We should really do something
 * better than abort on any errors - on the other hand, these are all fatal
 * to the server (can't work around them), but the abort just seems a bit
 * messy (exit would probably be better.)
 */

/* Eric had set up things to use a method of malloc/realloc to always make
 * sure he had a large enough buffer - this made the code slightly more
 * complicated and harder to read.  It makes more sense to me to just
 * set up a large buffer that should be able to handle any image.  being
 * that most xpm images are less than 1K, a 10,000 byte buffer should easily
 * cover us.
 */

/* Couple of notes:  We assume that the faces are in a continous block.
 * This works fine for now, but this could perhaps change in the future
 * (If clients do image caching, it would be handy for servers to perhaps
 * use different blocks for their new images..
 */
#define XPM_BUF 10000
void read_client_images()
{
    char filename[400];
    char buf[500];
    char databuf[XPM_BUF],*cur;
    FILE *infile;
    int num,len,compressed;

    /* Read in the pixmaps file */
    sprintf(filename,"%s/crossfire.xpm",settings.datadir);
    if ((infile = open_and_uncompress(filename,0,&compressed))==NULL) {
	LOG(llevError,"Unable to open %s\n", filename);
	abort();
    }
    while(1) {
	readbufline(buf,500,infile);
	if (*buf == '\0')
	    break;
	if(strncmp(buf,"ESRV_XPM ",9)!=0 ||
	   buf[14] != ' ') {
	    fprintf(stderr,"whoa, bad esrv_xpm line; not ESRV_XPM ...\n%s",buf);
	    abort();
	}
	num = atoi(buf+9);
	if (num<0 || num>=MAXFACENUM) {
	    LOG(llevError,"whoa, pixmap num %d \\not\\in 0..%d\n%s",
		    num,MAXFACENUM,buf);
	    abort();
	}
	buf[strlen(buf)-1] = '\0';
	if (faces[num].name != NULL) {
	    fprintf(stderr,"whoa, pixmap #%d duplicated??\n",num);
	    abort();
	}
	faces[num].name = malloc(strlen(buf+15)+1);
	strcpy(faces[num].name,buf+15);

	cur = databuf;
	/* Collect all the data for this pixmap */
	while(1) {
	    readbufline(buf,500,infile);
	    if (*buf == '\0') {
		fprintf(stderr,"whoa, pixmap #%d not terminated??\n",num);
		abort();
	    }
	    if (strcmp(buf,"ESRV_XPM_END\n")==0)
		break;
	    len = strlen(buf);
	    if (cur+len > databuf+XPM_BUF) {
		LOG(llevError,"Overflow of XPM_BUF in read_client_images, image %s\n",
		    faces[num].name);
		abort();
	    }
	    strcpy(cur,buf);
	    cur += len;
	}
	/* Collected all the data, put it into the pixmap buffer */
	faces[num].data = malloc(cur-databuf+1);
	faces[num].datalen = cur-databuf+1;
	memcpy(faces[num].data, databuf,cur-databuf);
	faces[num].data[cur-databuf] = '\0';
    }
    close_and_delete(infile,compressed);

    /* Assume bitmap information has same number as pixmap information */
    sprintf(filename,"%s/crossfire.cfb",settings.datadir);
    if ((infile = open_and_uncompress(filename,0,&compressed))==NULL) {
	LOG(llevError,"Can't open %s file",filename);
	abort();
    }
    for(num=0;num<MAXFACENUM;num++) {
	if (faces[num].name == NULL)
	    break; /* Last one -- assumes pixmaps are contiguous. */
	if (fread(faces[num].bitmapdata, 24 * 3, 1, infile) != 1) {
	    printf("Unable to read bitmap data for face #%d\n",num);
	    abort();
	}
    }
    while(num<MAXFACENUM) {
	if (faces[num].name != NULL) {
	    printf("Non-contiguous faces, %d sits in middle of nowhere.\n",num);
	    abort();
	}
	num++;
    }
    close_and_delete(infile, compressed);
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

    if (fcntl(ns->fd, F_SETFL, O_NDELAY)==-1) {
	LOG(llevError,"InitConnection:  Error on fcntl.\n");
    }
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

#ifdef HAVE_SYSCONF
  socket_info.max_filedescriptor = sysconf(_SC_OPEN_MAX);
#else
#  ifdef HAVE_GETDTABLESIZE
  socket_info.max_filedescriptor = getdtablesize();
#  else
  "Unable to find usable function to get max filedescriptors";
#  endif
#endif

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
	LOG(llevError,"init_ericserver: Error getting protox");
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
        defined(__sun__) || defined(linux) || defined(SVR4)
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
	close(init_sockets[0].fd);
	exit(-1);
    }
    if (listen(init_sockets[0].fd,5) == (-1))  {
	perror("error on listen");
	LOG(llevError,"error on listen\n");
	close(init_sockets[0].fd);
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
    if (close(ns->fd)) {
#ifdef ESRV_DEBUG
	LOG(llevDebug,"Error closing socket %d\n", which);
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


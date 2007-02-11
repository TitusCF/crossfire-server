
/*
 * static char *rcsid_sockets_c =
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

    The author can be reached via e-mail to mark@pyramid.com
*/

/**
 * \file
 * Low-level socket-related functions.
 *
 * \date 2003-12-02
 *
 * Contains some base functions that both the client and server
 * can use.  As such, depending what we are being compiled for will
 * determine what we can include.  the client is designed have
 * CFCLIENT defined as part of its compile flags.
 */

#include <global.h>
#include <newclient.h>
#include <sproto.h>

/***********************************************************************
 *
 * SockList functions/utilities
 *
 **********************************************************************/

void SockList_Init(SockList *sl)
{
    sl->len=0;
    sl->buf=NULL;
}

void SockList_AddChar(SockList *sl, char c)
{
    sl->buf[sl->len]=c;
    sl->len++;
}

void SockList_AddShort(SockList *sl, uint16 data)
{
    sl->buf[sl->len++]= (data>>8)&0xff;
    sl->buf[sl->len++] = data & 0xff;
}


void SockList_AddInt(SockList *sl, uint32 data)
{
    sl->buf[sl->len++]= (data>>24)&0xff;
    sl->buf[sl->len++]= (data>>16)&0xff;
    sl->buf[sl->len++]= (data>>8)&0xff;
    sl->buf[sl->len++] = data & 0xff;
}

void SockList_AddInt64(SockList *sl, uint64 data)
{
    sl->buf[sl->len++]= ( char )( (data>>56)&0xff );
    sl->buf[sl->len++]= ( char )( (data>>48)&0xff );
    sl->buf[sl->len++]= ( char )( (data>>40)&0xff );
    sl->buf[sl->len++]= ( char )( (data>>32)&0xff );

    sl->buf[sl->len++]= ( char )( (data>>24)&0xff );
    sl->buf[sl->len++]= ( char )( (data>>16)&0xff );
    sl->buf[sl->len++]= ( char )( (data>>8)&0xff );
    sl->buf[sl->len++] =( char )(  data & 0xff );
}

/**
 * Basically does the reverse of SockList_AddInt, but on
 * strings instead.  Same for the GetShort, but for 16 bits.
 */
int GetInt_String(const unsigned char *data)
{
    return ((data[0]<<24) + (data[1]<<16) + (data[2]<<8) + data[3]);
}

short GetShort_String(const unsigned char *data) {
    return ((data[0]<<8)+data[1]);
}

/******************************************************************************
 *
 * Start of read routines.
 *
 ******************************************************************************/

/**
 * This reads from fd and puts the data in sl.  We return true if we think
 * we have a full packet, 0 if we have a partial packet, or -1 if an error
 * occurred.  The only processing we do is remove the initial size value.  len
 * (As passed) is the size of the buffer allocated in the socklist.  We make
 * the assumption the buffer is at least 2 bytes long.
 */
 
int SockList_ReadPacket(int fd, SockList *sl, int len)
{
    int stat,toread;
    extern int errno;
    char err[MAX_BUF];

    /* Sanity check - shouldn't happen */
    if (sl->len < 0) {
	abort();
    }
    /* We already have a partial packet */
    if (sl->len<2) {
#ifdef WIN32 /* ***WIN32 SockList_ReadPacket: change read() to recv() */

	stat=recv(fd, sl->buf + sl->len, 2-sl->len,0);

#else
	do {
		stat=read(fd, sl->buf + sl->len, 2-sl->len);
	} while ((stat==-1) && (errno==EINTR));
#endif
	if (stat<0) {
	    /* In non blocking mode, EAGAIN is set when there is no
	     * data available.
	     */
#ifdef WIN32 /* ***WIN32 SockList_ReadPacket: error handling for win32 */
	if ((stat==-1) && WSAGetLastError() !=WSAEWOULDBLOCK) {
		if(WSAGetLastError() == WSAECONNRESET)
			LOG(llevDebug,"Connection closed by client\n");
		else
		{
			LOG(llevDebug,"ReadPacket got error %d, returning -1\n",WSAGetLastError());
		}
		return -1;	/* kick this user! */
	    }
#else
            if (errno == ECONNRESET) {
                    LOG(llevDebug, "ReadPacket got error %s, returning -1\n", strerror_local(errno, err, sizeof(err)));
                    return -1;
            }
  	    if (errno != EAGAIN && errno !=EWOULDBLOCK) {
		    LOG(llevDebug, "ReadPacket got error %s, returning 0\n", strerror_local(errno, err, sizeof(err)));
            }
#endif
	    return 0;	/*Error */
	}
	if (stat==0) return -1;
	sl->len += stat;
#ifdef CS_LOGSTATS
	cst_tot.ibytes += stat;
	cst_lst.ibytes += stat;
#endif
	if (stat<2) return 0;	/* Still don't have a full packet */
    }
    /* Figure out how much more data we need to read.  Add 2 from the
     * end of this - size header information is not included.
     */
    toread = 2+(sl->buf[0] << 8) + sl->buf[1] - sl->len;
    if ((toread + sl->len) >= len) {
	LOG(llevError,"SockList_ReadPacket: Want to read more bytes than will fit in buffer (%d>=%d).\n",
	     toread + sl->len, len);
	/* Quick hack in case for 'oldsocketmode' input.  If we are
	 * closing the socket anyways, then reading this extra 100 bytes
	 * shouldn't hurt.
	 */
#ifdef WIN32 /* ***win32 SockList_ReadPacket: change read() to recv() */
	recv(fd, sl->buf+2, 100, 0);
#else
	read(fd, sl->buf+2, 100);
#endif /* end win32 */

	/* return error so the socket is closed */
	return -1;
    }
    do {
#ifdef WIN32 /* ***win32 SockList_ReadPacket: change read() to recv() */
	stat = recv(fd, sl->buf+ sl->len, toread, 0);
#else
	do {
	    stat = read(fd, sl->buf+ sl->len, toread);
	} while ((stat<0) && (errno==EINTR));
#endif
	if (stat<0) {

#ifdef WIN32 /* ***win32 SockList_ReadPacket: change error handling for win32 */
	if ((stat==-1) && WSAGetLastError() !=WSAEWOULDBLOCK) {
		if(WSAGetLastError() == WSAECONNRESET)
			LOG(llevDebug,"Connection closed by client\n");
		else
		{
			LOG(llevDebug,"ReadPacket got error %d, returning -1\n",WSAGetLastError());
		}
		return -1;	/* kick this user! */
	    }
#else
	if (errno != EAGAIN && errno !=EWOULDBLOCK) {
		LOG(llevDebug, "ReadPacket got error %s, returning 0\n", strerror_local(errno, err, sizeof(err)));
	    }
#endif
	    return 0;	/*Error */
	}
	if (stat==0) return -1;
	sl->len += stat;
#ifdef CS_LOGSTATS
	cst_tot.ibytes += stat;
	cst_lst.ibytes += stat;
#endif
	toread -= stat;
	if (toread==0) return 1;
	if (toread < 0) {
	    LOG(llevError,"SockList_ReadPacket: Read more bytes than desired.\n");
	    return 1;
	}
    } while (toread>0);
    return 0;
}

/*******************************************************************************
 *
 * Start of write related routines.
 *
 ******************************************************************************/

/**
 * Adds data to a socket buffer for whatever reason.
 *
 * ns is the socket we are adding the data to, buf is the start of the
 * data, and len is the number of bytes to add.
 */

static void add_to_buffer(socket_struct *ns, const unsigned char *buf, int len)
{
    int avail, end;

    if ((len+ns->outputbuffer.len)>SOCKETBUFSIZE) {
	LOG(llevDebug,"Socket on fd %d has overrun internal buffer - marking as dead\n",
	    ns->fd);
	ns->status = Ns_Dead;
	return;
    }

    /* data + end is where we start putting the new data.  The last byte
     * currently in use is actually data + end -1
     */

    end=ns->outputbuffer.start + ns->outputbuffer.len;
    /* The buffer is already in a wrapped state, so adjust end */
    if (end>=SOCKETBUFSIZE) end-=SOCKETBUFSIZE;
    avail=SOCKETBUFSIZE - end;

    /* We can all fit it behind the current data without wrapping */
    if (avail >=len ) {
	memcpy(ns->outputbuffer.data + end, buf, len);
    }
    else {
	memcpy(ns->outputbuffer.data + end, buf, avail);
	memcpy(ns->outputbuffer.data, buf+avail, len-avail);
    }
    ns->outputbuffer.len += len;
#if 0
    LOG(llevDebug,"Added %d to output buffer, total length now %d, start=%d\n", len,
	ns->outputbuffer.len, ns->outputbuffer.start);
#endif
}

/**
 * Writes data to socket.
 *
 * When the socket is clear to write, and we have backlogged data, this
 * is called to write it out.
 */
void write_socket_buffer(socket_struct *ns)
{
    int amt, max;

    if (ns->outputbuffer.len==0) {
	LOG(llevDebug,"write_socket_buffer called when there is no data, fd=%d\n",
	    ns->fd);
	return;
    }

    do {
	max = SOCKETBUFSIZE - ns->outputbuffer.start;
	if (ns->outputbuffer.len<max) max = ns->outputbuffer.len;

#ifdef WIN32 /* ***win32 write_socket_buffer: change write() to send() */
	amt=send(ns->fd, ns->outputbuffer.data + ns->outputbuffer.start, max,0);
#else
	do {
	    amt=write(ns->fd, ns->outputbuffer.data + ns->outputbuffer.start, max);
	} while ((amt<0) && (errno==EINTR));
#endif

	if (amt < 0) { /* We got an error */

#ifdef WIN32 /* ***win32 write_socket_buffer: change error handling */
	if (amt == -1 && WSAGetLastError() !=WSAEWOULDBLOCK) {
		LOG(llevError,"New socket write failed (wsb) (%d).\n", WSAGetLastError());
#else
	if (errno !=EWOULDBLOCK) {
        char err[MAX_BUF];
		LOG(llevError,"New socket write failed (wsb) (%d: %s).\n",
		    errno, strerror_local(errno, err, sizeof(err)));
#endif
		ns->status=Ns_Dead;
		return;
	    }
	    else { /* EWOULDBLOCK */
		/* can't write it, so store it away. */
		ns->can_write=0;
		return;
	    }
	}
	ns->outputbuffer.start += amt;
	/* wrap back to start of buffer */
	if (ns->outputbuffer.start==SOCKETBUFSIZE) ns->outputbuffer.start=0;
	ns->outputbuffer.len -= amt;
#ifdef CS_LOGSTATS
	cst_tot.obytes += amt;
	cst_lst.obytes += amt;
 #endif
    } while (ns->outputbuffer.len>0);
}

/**
 * This writes data to the socket. - It is very low level -
 * all we try and do is write out the data to the socket
 * provided (ns).  buf is the data to write, len is the number
 * of bytes to write.  IT doesn't return anything - rather, it
 * updates the ns structure if we get an  error.
 */
static void Write_To_Socket(socket_struct *ns, const unsigned char *buf, int len)
{
    int amt=0;
    const unsigned char *pos=buf;
    char err[MAX_BUF];

    if (ns->status == Ns_Dead || !buf) {
	LOG(llevDebug,"Write_To_Socket called with dead socket\n");
	return;
    }
 
#ifndef __GNU__ /* This caused problems on Hurd */
    if (!ns->can_write) {
	add_to_buffer(ns, buf, len);
	return;
    }
#endif
    /* If we manage to write more than we wanted, take it as a bonus */
    while (len>0) {

#ifdef WIN32 /* ***win32 Write_To_Socket: change write() to send() */
	    amt=send(ns->fd, pos, len,0);
#else
	do {
	    amt=write(ns->fd, pos, len);
	} while ((amt<0) && (errno==EINTR));
#endif

	if (amt < 0) { /* We got an error */
#ifdef WIN32 /* ***win32 Write_To_Socket: change error handling */
	if (amt == -1 && WSAGetLastError() !=WSAEWOULDBLOCK) {
		LOG(llevError,"New socket write failed WTS (%d).\n",WSAGetLastError());
#else
	if (errno !=EWOULDBLOCK) {
		LOG(llevError,"New socket write failed WTS (%d: %s).\n", /* ---WIN32 */
		    errno, strerror_local(errno, err, sizeof(err)));
#endif
		ns->status=Ns_Dead;
		return;
	    }
	    else { /* EWOULDBLOCK */
		/* can't write it, so store it away. */
		add_to_buffer(ns, pos, len);
		ns->can_write=0;
		return;
	    }
	}
	/* amt gets set to 0 above in blocking code, so we do this as
	 * an else if to make sure we don't reprocess it.
	 */
	else if (amt==0) {
	    LOG(llevError,"Write_To_Socket: No data written out.\n");
	}
	len -= amt;
	pos += amt;
#ifdef CS_LOGSTATS
	cst_tot.obytes += amt;
	cst_lst.obytes += amt;
 #endif
    }
}


/**
 * Takes a string of data, and writes it out to the socket. A very handy
 * shortcut function.
 */
void cs_write_string(socket_struct *ns, const char *buf, int len)
{
    SockList sl;

    sl.len = len;
    sl.buf = (unsigned char *)buf;
    Send_With_Handling(ns, &sl);
}


/**
 * Calls Write_To_Socket to send data to the client.
 *
 * The only difference in this function is that we take a SockList
 *, and we prepend the length information.
 */
void Send_With_Handling(socket_struct *ns, const SockList *msg)
{
    unsigned char sbuf[2];

    if (ns->status == Ns_Dead || !msg)
	return;

    if (msg->len > MAXSOCKSENDBUF) {
	LOG(llevError,"Trying to send a buffer beyond properly size, len %d > max len %d\n",
	    msg->len, MAXSOCKSENDBUF);
	/* Almost certainly we've overflowed a buffer, so quite now to make
	 * it easier to debug.
	 */
	abort();
    }
    sbuf[0] = ((uint32)(msg->len) >> 8) & 0xFF;
    sbuf[1] = ((uint32)(msg->len)) & 0xFF;
    if (ns->status != Ns_Old) 
	Write_To_Socket(ns, sbuf, 2);
    Write_To_Socket(ns, msg->buf, msg->len);
}

/**
 * Takes a string of data, and writes it out to the socket. A very handy
 * shortcut function.
 */
void Write_String_To_Socket(socket_struct *ns, const char *buf, int len)
{
    SockList sl;

    sl.len = len;
    sl.buf = (unsigned char *)buf;
    Send_With_Handling(ns, &sl);
}


/******************************************************************************
 *
 * statistics logging functions.
 *
 ******************************************************************************/

#ifdef CS_LOGSTATS
/* cst_tot is for the life of the server, cst_last is for the last series of
 * stats
 */
CS_Stats cst_tot, cst_lst;

/**
 * Writes out the gathered stats.  We clear cst_lst.
 */
void write_cs_stats(void)
{
    time_t now=time(NULL);

    /* If no connections recently, don't both to log anything */
    if (cst_lst.ibytes==0 && cst_lst.obytes==0) return;

    /* CSSTAT is put in so scripts can easily find the line */
    LOG(llevInfo, "CSSTAT: %.16s tot %d %d %d %d inc %d %d %d %d\n",
	ctime(&now), cst_tot.ibytes, cst_tot.obytes, cst_tot.max_conn,
	now - cst_tot.time_start, cst_lst.ibytes, cst_lst.obytes,
	cst_lst.max_conn, now - cst_lst.time_start);
    cst_lst.ibytes=0;
    cst_lst.obytes=0;
    cst_lst.max_conn=socket_info.nconns;
    cst_lst.time_start=now;
}
#endif

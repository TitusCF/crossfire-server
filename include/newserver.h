/*
 * static char *rcsid_newserver_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    newserver.h defines various structures and values that are use for the
    new client server communication method.  Values defined here are only
    used on the server side code.  For shared client/server values, see
    newclient.h
*/


#ifndef NEWSERVER_H
#define NEWSERVER_H
#define MAXMAPCELLFACES 50

struct MapCell {
  short faces[MAXMAPCELLFACES];
  int count;
};

struct Map {
  struct MapCell cells[11][11];
};

/* True max is 16383 given current map compaction method */
#define MAXFACENUM  5000
#define MAXANIMNUM  2000

struct statsinfo {
    char *range, *title;
};


/* This contains basic information on the socket structure.  status is its
 * current state.  we set up our on buffers for sending/receiving, so we can
 * handle some higher level functions.  fd is the actual file descriptor we
 * are using.
 */
enum FaceSendMode { Send_Face_Pixmap, Send_Face_Bitmap, Send_Face_None};

enum Sock_Status {Ns_Avail, Ns_Add, Ns_Dead, Ns_Old};

/* Reserver 0 for neither of these being set */
enum Old_Mode {Old_Listen=1, Old_Player=2};

/* The following is the setup for a ring buffer for storing outbut
 * data that the OS can't handle right away.
 */

typedef struct Buffer {
    char    data[SOCKETBUFSIZE];
    int	    start;
    int	    len;
} Buffer;

typedef struct NewSocket {
    enum Sock_Status status;
    int fd;
    enum FaceSendMode facemode;
    struct Map lastmap;
    uint8 faces_sent[MAXFACENUM];
    uint8 anims_sent[MAXANIMNUM];
    struct statsinfo stats;
    /* If we get an incomplete packet, this is used to hold the data. */
    SockList	inbuf;
    char    *host;	    /* Which host it is connected from (ip address)*/
    Buffer  outputbuffer;   /* For undeliverable data */
    uint32  facecache:1;    /* IF true, client is caching images */
    uint32  sent_scroll:1;
    uint32  sound:1;	    /* does the client want sound? */
    uint32  cs_version, sc_version; /* versions of the client */
    uint32  update_look:1;  /* If true, we need to send the look window */
    uint32  can_write:1;    /* Can we write to this socket? */

    /* Below here is information only relevant for old sockets */
    char    *comment;	    /* name or listen comment */
    enum Old_Mode old_mode;
} NewSocket;



#define FACE_TYPES  3

typedef struct FaceInfo {
  char *name;   /* name of the image, including component path names (ie,
                 * ./arch/system/bug.111)
                 */
  char *data[FACE_TYPES];   /* xpm data */
  uint16 datalen[FACE_TYPES];   /* length of the xpm data */
} FaceInfo;


typedef struct Socket_Info {
    struct timeval timeout;	/* Timeout for select */
    int	    max_filedescriptor;	/* max filedescriptor on the system */
    int	    nconns;		/* Number of connections */
    int	    allocated_sockets;	/* number of allocated in init_sockets */
} Socket_Info;

extern Socket_Info socket_info;
extern FaceInfo	faces[MAXFACENUM];  /* face information */

#define VERSION_CS 1021
#define VERSION_SC 1022
#define VERSION_INFO "Crossfire Server"



#endif

/*
 * static char *rcsid_newserver_h =
 *   "$Id$";
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
    newserver.h defines various structures and values that are use for the
    new client server communication method.  Values defined here are only
    used on the server side code.  For shared client/server values, see
    newclient.h
*/


#ifndef NEWSERVER_H
#define NEWSERVER_H
#define MAXMAPCELLFACES 50

#define NUM_LOOK_OBJECTS 50

struct MapCell {
  short faces[MAXMAPCELLFACES];
  char quick_pos[MAXMAPCELLFACES];
  int count;
};

struct Map {
  struct MapCell cells[MAP_CLIENT_X][MAP_CLIENT_Y];
};

/* True max is 16383 given current map compaction method */
#define MAXFACENUM  5000
#define MAXANIMNUM  2000

struct statsinfo {
    char *range, *title, *ext_title;
};


/* This contains basic information on the socket structure.  status is its
 * current state.  we set up our on buffers for sending/receiving, so we can
 * handle some higher level functions.  fd is the actual file descriptor we
 * are using.
 */

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
    struct Map lastmap;
    uint8 faces_sent[MAXFACENUM];
    uint8 anims_sent[MAXANIMNUM];
    struct statsinfo stats;
    /* If we get an incomplete packet, this is used to hold the data. */
    SockList	inbuf;
    char    *host;	    /* Which host it is connected from (ip address)*/
    Buffer  outputbuffer;   /* For undeliverable data */
    uint32  facecache:1;    /* If true, client is caching images */
    uint8   faceset;	    /* Set the client is using, default 0 */
    uint32  sent_scroll:1;
    uint32  ext2:1;         /* enables the new sdl client/server stuff */
    uint32  ext_title_flag;  /* if 1, we should generate and send a new ext_title update */
    uint32  sound:1;	    /* does the client want sound */
    uint32  skillexp:1;	    /* does the client want skill exp data - MT*/
    uint32  map1cmd:1;	    /* Always use map1 protocol command */
    uint32  map2cmd:1;	    /* Always use map2 protocol command */
    uint32  newmapcmd:1;    /* Send newmap command when entering new map SMACFIGGEN*/
    uint32  darkness:1;	    /* True if client wants darkness information */
    uint32  image2:1;	    /* Client wants image2/face2 commands */
    uint32  newanim:1;      /* enable the advanced animation system - MT */
    uint32  cs_version, sc_version; /* versions of the client */
    uint32  update_look:1;  /* If true, we need to send the look window */
    uint32  can_write:1;    /* Can we write to this socket? */
    uint16  look_position;  /* start of drawing of look window */
    uint8   mapx, mapy;	    /* How large a map the client wants */

    /* Below here is information only relevant for old sockets */
    char    *comment;	    /* name or listen comment */
    enum Old_Mode old_mode;
} NewSocket;



#define FACE_TYPES  1

#define PNG_FACE_INDEX	0

typedef struct Socket_Info {
    struct timeval timeout;	/* Timeout for select */
    int	    max_filedescriptor;	/* max filedescriptor on the system */
    int	    nconns;		/* Number of connections */
    int	    allocated_sockets;	/* number of allocated in init_sockets */
} Socket_Info;

extern Socket_Info socket_info;

#define VERSION_CS 1023    /* version >= 1023 understand setup cmd */
#define VERSION_SC 1027
#define VERSION_INFO "Crossfire Server"



#endif

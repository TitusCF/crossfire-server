/*
 * static char *rcsid_newserver_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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

/* Reduce this from 50 to 5 - as it is now, there will never be more
 * than 3 anyways.
 */

#define NUM_LOOK_OBJECTS 50

struct map_cell_struct {
  uint16 faces[MAP_LAYERS];
  uint16 smooth[MAP_LAYERS];
  int darkness;
};

/* This basically defines the largest size an
 * archetype may be - it is used for allocation of
 * some structures, as well as determining how far
 * we should look for the heads of big images.
 */
#define MAX_HEAD_OFFSET	    8

#define MAX_CLIENT_X (MAP_CLIENT_X + MAX_HEAD_OFFSET)
#define MAX_CLIENT_Y (MAP_CLIENT_Y + MAX_HEAD_OFFSET)

struct Map {
  struct map_cell_struct cells[MAX_CLIENT_X][MAX_CLIENT_Y];
};

/* True max is 16383 given current map compaction method */
#define MAXANIMNUM  2000

struct statsinfo {
    char *range, *title;
};


/* This contains basic information on the socket structure.  status is its
 * current state.  we set up our on buffers for sending/receiving, so we can
 * handle some higher level functions.  fd is the actual file descriptor we
 * are using.
 */

enum Sock_Status {Ns_Avail, Ns_Add, Ns_Dead};

/**
 * Only one map mode can actually be used, so lets make it a switch
 * instead of having a bunch of different fields that needed to
 * get toggled.  Note ordering here is important -
 * This way, when a new feature is
 * added, a simple > compare can be done instead a bunch of ==
 */
enum MapMode { Map2Cmd = 0 };

/* The following is the setup for a ring buffer for storing outbut
 * data that the OS can't handle right away.
 */

typedef struct buffer_struct {
    char    data[SOCKETBUFSIZE];
    int	    start;
    int	    len;
} buffer_struct;

/* how many times we are allowed to give the wrong password before being kicked. */
#define MAX_PASSWORD_FAILURES 5

typedef struct socket_struct {
    enum Sock_Status status;
    int fd;
    struct Map lastmap;
    sint8 map_scroll_x, map_scroll_y;
    size_t faces_sent_len;  /* This is the number of elements allocated in faces_sent[] */
    uint8 *faces_sent;      /* This is a bitmap on sent face status */
    uint8 anims_sent[MAXANIMNUM];
    struct statsinfo stats;
    /* If we get an incomplete packet, this is used to hold the data. */
    SockList	inbuf;
    char    *host;	    /* Which host it is connected from (ip address)*/
    uint8   password_fails; /* how many times the player has failed to give the right password */
    buffer_struct outputbuffer;   /* For undeliverable data */
    uint32  facecache:1;    /* If true, client is caching images */
    uint32  sent_scroll:1;
    uint32  sound;	    /* Client sound mode */
    uint32  newmapcmd:1;    /* Send newmap command when entering new map SMACFIGGEN*/
    uint32  darkness:1;	    /* True if client wants darkness information */
    uint32  update_look:1;  /* If true, we need to send the look window */
    uint32  can_write:1;    /* Can we write to this socket? */
    uint32  has_readable_type:1; /* If true client accept additional text information
                                    used to arrange text in books, scrolls, or scripted dialogs */
    uint32  monitor_spells:1; /* Client wishes to be informed when their spell list changes */
    uint32  tick:1;	    /* Client wishes to get tick commands */
    uint32  supported_readables; /* each bit is a readable supported by client */
    uint32  cs_version, sc_version; /* versions of the client */
    enum MapMode mapmode;   /* Type of map commands the client wants. */
    uint16  look_position;  /* start of drawing of look window */
    uint8   mapx, mapy;	    /* How large a map the client wants */
    uint8   faceset;	    /* Set the client is using, default 0 */
    uint32	is_bot:1;		/* Client shouldn't be reported to metaserver */
    /* Below are flags for extedend infos to pass to client
     * with S->C mapextended command */
    uint32  want_pickup:1;  /**< Client wants pickup information when logging in. */
} socket_struct;


#define CLIENT_SUPPORT_READABLES(__sockPtr,__type)\
	( ((__type)>0) &&\
	  ((__sockPtr)->has_readable_type) && \
	  ((__sockPtr)->supported_readables & (1<<(__type))) )

/* Bitmask for the faces_sent[] array - what
 * portion of the face have we sent?
 */
#define NS_FACESENT_FACE	0x1
#define NS_FACESENT_SMOOTH	0x2

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
#define VERSION_SC 1028
#define VERSION_INFO "Crossfire Server"

#endif /* NEWSERVER_H */

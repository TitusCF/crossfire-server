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

struct MapCell {
  short faces[MAP_LAYERS];
  uint16 smooth[MAP_LAYERS];
  int count;	/* This is really darkness in the map1 command */
};

/* This basically defines the largest size an 
 * archetype may be - it is used for allocation of
 * some structures, as well as determining how far
 * we should look for the heads of big images.
 */
#define MAX_HEAD_OFFSET	    6

#define MAX_CLIENT_X (MAP_CLIENT_X + MAX_HEAD_OFFSET)
#define MAX_CLIENT_Y (MAP_CLIENT_Y + MAX_HEAD_OFFSET)

struct Map {
  struct MapCell cells[MAX_CLIENT_X][MAX_CLIENT_Y];
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

enum Sock_Status {Ns_Avail, Ns_Add, Ns_Dead, Ns_Old};

/* Reserver 0 for neither of these being set */
enum Old_Mode {Old_Listen=1, Old_Player=2};

/* Only one map mode can actually be used, so lets make it a switch
 * instead of having a bunch of different fields that needed to
 * get toggled.
 */
enum MapMode {Map0Cmd = 0, Map1Cmd = 1, Map1aCmd=2 };

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
    uint32  sent_scroll:1;
    uint32  ext2:1;         /* enables the new sdl client/server stuff */
    uint32  ext_title_flag;  /* if 1, we should generate and send a new ext_title update */
    uint32  sound:1;	    /* does the client want sound */
    uint32  exp64:1;	    /* Client wants 64 bit exp data, as well as skill data */
    uint32  newmapcmd:1;    /* Send newmap command when entering new map SMACFIGGEN*/
    uint32  darkness:1;	    /* True if client wants darkness information */
    uint32  image2:1;	    /* Client wants image2/face2 commands */
    uint32  update_look:1;  /* If true, we need to send the look window */
    uint32  can_write:1;    /* Can we write to this socket? */
    uint32  cs_version, sc_version; /* versions of the client */
    enum MapMode mapmode;   /* Type of map commands the client wants. */
    uint16  look_position;  /* start of drawing of look window */
    uint8   mapx, mapy;	    /* How large a map the client wants */
    uint8   itemcmd;	    /* What version of the 'item' protocol command to use */
    uint8   faceset;	    /* Set the client is using, default 0 */
    uint32  ext_mapinfos:1;  /* If true client accept additionnal info on maps*/
    /* Below are flags for extedend infos to pass to client 
     * with S->C mapextended command */
    uint32  EMI_smooth:1;   /* Send smooth in extendmapinfos*/

    /* Below here is information only relevant for old sockets */
    char    *comment;	    /* name or listen comment */
    enum Old_Mode old_mode;
} NewSocket;

/*Constants in the form EMI_ is for extended map infos.
 * Even if the client select the additionnal infos it wants
 * on the map, there may exist cases where this whole info
 * is not given in one buch but in separate bunches. This 
 * is done performance reasons (imagine some info related to
 * a visible object and another info related to a 4 square
 * width and height area). At the begin of an extended info packet
 * is a bit field. A bit is activated for each extended info
 * present in the data 
 */
/* Meanings:
 * EMI_NOREDRAW  Take extended infos into account but don't redraw,
 *               some additionnal datas will follow in a new packet
 * EMI_SMOOTH    Datas about smoothing  
 */ 
#define EMI_NOREDRAW        0x01  
#define EMI_SMOOTH          0x02
/*this last one says the bitfield continue un next byte
 * There may be several on contiguous bytes. So there is 7
 * actual bits used per byte, and the number of bytes
 * is not fixed in protocol
 */
#define EMI_HASMOREBITS     0x80

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

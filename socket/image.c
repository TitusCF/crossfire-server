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

/** \file
 * Image related communication
 *
 *  \date 2006-03-18
 *
 * This file deals with the image related communication to the
 * client.  I've located all the functions in this file - this
 * localizes it more, and means that we don't need to declare
 * things like all the structures as globals.
 */

#include <global.h>
#include <sproto.h>

#include <newclient.h>
#include <newserver.h>
#include <loader.h>
#include <image.h>

/**
 * Client tells us what type of faces it wants.  Also sets
 * the caching attribute.
 */

void set_face_mode_cmd(char *buf, int len, socket_struct *ns)
{
    char tmp[256];

    int mask =(atoi(buf) & CF_FACE_CACHE), mode=(atoi(buf) & ~CF_FACE_CACHE);

    if (mode==CF_FACE_NONE) {
        ns->facecache=1;
    } else if (mode!=CF_FACE_PNG) {
        sprintf(tmp,"drawinfo %d %s", NDI_RED,
                "Warning - send unsupported face mode.  Will use Png");
        Write_String_To_Socket(ns, tmp, strlen(tmp));
#ifdef ESRV_DEBUG
        LOG(llevDebug,"set_face_mode_cmd: Invalid mode from client: %d\n",
            mode);
#endif
    }
    if (mask) {
        ns->facecache=1;
    }
}

/**
 * Client has requested pixmap that it somehow missed getting.
 * This will be called often if the client is
 * caching images.
 */

void send_face_cmd(char *buff, int len, socket_struct *ns)
{
    long tmpnum = atoi(buff);
    short facenum=tmpnum & 0xffff;

    if(facenum!=0)
        esrv_send_face(ns, facenum,1);
}

/**
 * Sends a face to a client if they are in pixmap mode
 * nothing gets sent in bitmap mode.
 * If nocache is true (nonzero), ignore the cache setting from the client -
 * this is needed for the askface, in which we really do want to send the
 * face (and askface is the only place that should be setting it).  Otherwise,
 * we look at the facecache, and if set, send the image name.
 */

void esrv_send_face(socket_struct *ns,short face_num, int nocache)
{
    SockList sl;
    int fallback;

    if (face_num <= 0 || face_num >= nrofpixmaps) {
        LOG(llevError,"esrv_send_face (%d) out of bounds??\n",face_num);
        return;
    }

    sl.buf = malloc(MAXSOCKSENDBUF);
    fallback = get_face_fallback(ns->faceset, face_num);

    if (facesets[fallback].faces[face_num].data == NULL) {
        LOG(llevError,"esrv_send_face: faces[%d].data == NULL\n",face_num);
        return;
    }

    if (ns->facecache && !nocache) {
        strcpy((char*)sl.buf, "face2 ");

        sl.len=strlen((char*)sl.buf);
        SockList_AddShort(&sl, face_num);
        SockList_AddChar(&sl, fallback);
        SockList_AddInt(&sl, facesets[fallback].faces[face_num].checksum);
        strcpy((char*)sl.buf + sl.len, new_faces[face_num].name);
        sl.len += strlen(new_faces[face_num].name);
        Send_With_Handling(ns, &sl);
    }
    else {
        strcpy((char*)sl.buf, "image2 ");
        sl.len=strlen((char*)sl.buf);
        SockList_AddInt(&sl, face_num);
        SockList_AddChar(&sl, fallback);
        SockList_AddInt(&sl, facesets[fallback].faces[face_num].datalen);
        memcpy(sl.buf+sl.len, facesets[fallback].faces[face_num].data,
               facesets[fallback].faces[face_num].datalen);
        sl.len += facesets[fallback].faces[face_num].datalen;
        Send_With_Handling(ns, &sl);
    }
    ns->faces_sent[face_num] |= NS_FACESENT_FACE;
    free(sl.buf);
}

/**
 * Sends the number of images, checksum of the face file,
 * and the image_info file information.  See the doc/Developers/protocol
 * if you want further detail.
 */

void send_image_info(socket_struct *ns, char *params)
{
    SockList sl;
    int i;

    sl.buf = malloc(MAXSOCKSENDBUF);

    sprintf((char*)sl.buf,"replyinfo image_info\n%d\n%d\n",
            nrofpixmaps-1, bmaps_checksum);
    for (i=0; i<MAX_FACE_SETS; i++) {
        if (facesets[i].prefix) {
            sprintf((char*)sl.buf + strlen((char*)sl.buf),
                    "%d:%s:%s:%d:%s:%s:%s",
                    i,  facesets[i].prefix, facesets[i].fullname,
                    facesets[i].fallback, facesets[i].size,
                    facesets[i].extension, facesets[i].comment);
        }
    }
    sl.len = strlen((char*)sl.buf);
    Send_With_Handling(ns, &sl);
    free(sl.buf);
}

/**
 * Sends requested face information.
 * \param ns socket to send to
 * \param params contains first and last index of face
 *
 * For each image in [start..stop] sends
 *  - checksum
 *  - name
 */
void send_image_sums(socket_struct *ns, char *params)
{
    int start, stop;
    short i;
    int qq;
    char *cp, buf[MAX_BUF];
    SockList sl;

    sl.buf = malloc(MAXSOCKSENDBUF);

    start = atoi(params);
    for (cp = params; *cp != '\0'; cp++)
        if (*cp == ' ') break;

    stop = atoi(cp);
    if (stop < start || *cp == '\0' || (stop-start)>1000 ||
        stop >= nrofpixmaps) {
        sprintf(buf,"replyinfo image_sums %d %d", start, stop);
        cs_write_string(ns, buf, strlen(buf));
        return;
    }
    sprintf((char*)sl.buf,"replyinfo image_sums %d %d ", start, stop);

    sl.len = strlen((char*)sl.buf);

    for (i=start; i<=stop; i++) {
        if (sl.len+2+4+1+1+strlen(new_faces[i].name)+1 > MAXSOCKSENDBUF) {
            LOG(llevError,
                "send_image_sums: buffer overflow, rejecting range %d..%d\n",
                start, stop);
            sprintf(buf, "replyinfo image_sums %d %d", start, stop);
            cs_write_string(ns, buf, strlen(buf));
            return;
        }

        SockList_AddShort(&sl, i);
        ns->faces_sent[i] |= NS_FACESENT_FACE;

        qq = get_face_fallback(ns->faceset, i);
        SockList_AddInt(&sl, facesets[qq].faces[i].checksum);
        SockList_AddChar(&sl, qq);

        qq = strlen(new_faces[i].name);
        SockList_AddChar(&sl, ( char )( qq + 1 ));
        strcpy((char*)sl.buf + sl.len, new_faces[i].name);
        sl.len += qq;
        SockList_AddChar(&sl, 0);
    }
    Send_With_Handling(ns, &sl);
    free(sl.buf);
}

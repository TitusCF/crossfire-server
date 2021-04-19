/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
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

#include "global.h"

#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "newserver.h"
#include "shared/newclient.h"
#include "sproto.h"
#include "assets.h"

/**
 * Client has requested pixmap that it somehow missed getting.
 * This will be called often if the client is
 * caching images.
 * @param buff data received from the client.
 * @param len length of buff, ignored.
 * @param ns socket to send data to.
 */
void send_face_cmd(char *buff, int len, socket_struct *ns) {
    long tmpnum;
    uint16_t faceid;

    if (len <= 0 || !buff) {
        LOG(llevDebug, "IP '%s' sent bogus send_face_cmd information\n", ns->host);
        return;
    }

    tmpnum = atoi(buff);
    faceid = tmpnum&0xffff;

    if (faceid != 0)
        esrv_send_face(ns, get_face_by_id(faceid), 1);
}

/**
 * Sends a face to a client if they are in pixmap mode,
 * nothing gets sent in bitmap mode.
 * If nocache is true (nonzero), ignore the cache setting from the client -
 * this is needed for the askface, in which we really do want to send the
 * face (and askface is the only place that should be setting it).  Otherwise,
 * we look at the facecache, and if set, send the image name.
 * @param ns socket to send the date to.
 * @param face face to send.
 * @param nocache if 1 then send a 'image2', else depending on client cache setting.
 */
void esrv_send_face(socket_struct *ns, const Face *face, int nocache) {
    SockList sl;

    if (face == NULL) {
        LOG(llevError, "esrv_send_face NULL??\n");
        return;
    }

    SockList_Init(&sl);
    face_sets *fs = find_faceset(get_face_fallback(ns->faceset, face->number));

    if (!fs || fs->faces[face->number].data == NULL) {
        LOG(llevError, "esrv_send_face: faces[%d].data == NULL\n", face->number);
        return;
    }

    if (ns->facecache && !nocache) {
        SockList_AddString(&sl, "face2 ");
        SockList_AddShort(&sl, face->number);
        SockList_AddChar(&sl, fs->id);
        SockList_AddInt(&sl, fs->faces[face->number].checksum);
        SockList_AddString(&sl, face->name);
        Send_With_Handling(ns, &sl);
    } else {
        SockList_AddString(&sl, "image2 ");
        SockList_AddInt(&sl, face->number);
        SockList_AddChar(&sl, fs->id);
        SockList_AddInt(&sl, fs->faces[face->number].datalen);
        SockList_AddData(&sl, fs->faces[face->number].data, fs->faces[face->number].datalen);
        Send_With_Handling(ns, &sl);
    }
    ns->faces_sent[face->number] |= NS_FACESENT_FACE;
    SockList_Term(&sl);
}

/** @todo remove */
static SockList *ugly;
static void do_faceset(const face_sets *fs) {
    SockList_AddPrintf(ugly, "%d:%s:%s:%d:%s:%s:%s\n",
             fs->id, fs->prefix, fs->fullname,
             fs->fallback ? fs->fallback->id : 0, fs->size,
             fs->extension, fs->comment);
}

/**
 * Sends the number of images, checksum of the face file,
 * and the image_info file information.  See the doc/Developers/protocol
 * if you want further detail.
 * @param ns socket to send data to.
 */
void send_image_info(socket_struct *ns) {
    SockList sl;

    SockList_Init(&sl);

    ugly = &sl;
    SockList_AddPrintf(&sl, "replyinfo image_info\n%d\n%d\n", get_faces_count()-1, get_bitmap_checksum());
    facesets_for_each(do_faceset);
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
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
void send_image_sums(socket_struct *ns, char *params) {
    unsigned int start, stop;
    unsigned short i;
    char *cp;
    SockList sl;

    SockList_Init(&sl);

    start = atoi(params);
    for (cp = params; *cp != '\0'; cp++)
        if (*cp == ' ')
            break;

    stop = atoi(cp);
    if (stop < start
    || *cp == '\0'
    || (stop-start) > 1000
    || stop >= get_faces_count()) {
        SockList_AddPrintf(&sl, "replyinfo image_sums %d %d", start, stop);
        Send_With_Handling(ns, &sl);
        SockList_Term(&sl);
        return;
    }
    SockList_AddPrintf(&sl, "replyinfo image_sums %d %d ", start, stop);

    for (i = start; i <= stop; i++) {
        const Face *face = get_face_by_id(i);

        if (SockList_Avail(&sl) < 2+4+1+1+strlen(face->name)+1) {
            LOG(llevError, "send_image_sums: buffer overflow, rejecting range %d..%d\n", start, stop);
            SockList_Reset(&sl);
            SockList_AddPrintf(&sl, "replyinfo image_sums %d %d", start, stop);
            Send_With_Handling(ns, &sl);
            SockList_Term(&sl);
            return;
        }

        SockList_AddShort(&sl, i);
        ns->faces_sent[face->number] |= NS_FACESENT_FACE;

        face_sets *fs = find_faceset(get_face_fallback(ns->faceset, i));
        SockList_AddInt(&sl, fs->faces[i].checksum);
        SockList_AddChar(&sl, fs->id);
        SockList_AddLen8Data(&sl, face->name, strlen(face->name)+1);
    }
    Send_With_Handling(ns, &sl);
    SockList_Term(&sl);
}

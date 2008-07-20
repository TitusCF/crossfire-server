/*
 * static char *rcsid_image_c =
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

    The maintainer of this code can be reached at crossfire-devel@real-time.com
*/

/**
 * @file
 * Handles face-related stuff, including the actual face data.
 */

#include <global.h>
#include <stdio.h>
#include "image.h"

New_Face *new_faces;

/**
 * bmappair and ::xbm are used when looking for the image id numbers
 * of a face by name.  xbm is sorted alphabetically so that bsearch
 * can be used to quickly find the entry for a name.  the number is
 * then an index into the new_faces array.
 * This data is redundant with new_face information - the difference
 * is that this data gets sorted, and that doesn't necessarily happen
 * with the new_face data - when accessing new_face[some number],
 * that some number corresponds to the face at that number - for
 * xbm, it may not.  At current time, these do in fact match because
 * the bmaps file is created in a sorted order.
 */
struct bmappair {
    char *name;
    unsigned int number;
};
/**
 * The xbm array (which contains name and number information, and
 * is then sorted) contains nroffiles entries.
 */
static struct bmappair *xbm=NULL;

/**
 * Following can just as easily be pointers, but
 * it is easier to keep them like this.
 */
New_Face *blank_face, *empty_face, *smooth_face;


/** nroffiles is the actual number of bitmaps defined. */
static int nroffiles = 0;

/** nrofpixmaps is the number of bitmaps loaded.  With
 * the automatic generation of the bmaps file, this is now equal
 * to nroffiles.
 */
int nrofpixmaps = 0;

/**
 * id is the face to smooth, smooth is the 16x2 face used to smooth id.
 */
struct smoothing {
    uint16 id;
    uint16 smooth;
};

/**
 * Contains all defined smoothing entries. smooth is an array of nrofsmooth
 * entries. It is sorted by smooth[].id.
 */
static struct smoothing *smooth=NULL;
int nrofsmooth=0;

/**
 * The only thing this table is used for now is to
 * translate the colorname in the magicmap field of the
 * face into a numeric index that is then sent to the
 * client for magic map commands.  The order of this table
 * must match that of the NDI colors in include/newclient.h.
 */
static const char *const colorname[] = {
    "black",        /* 0  */
    "white",        /* 1  */
    "blue",         /* 2  */
    "red",          /* 3  */
    "orange",       /* 4  */
    "light_blue",   /* 5  */
    "dark_orange",  /* 6  */
    "green",        /* 7  */
    "light_green",  /* 8  */
    "grey",         /* 9  */
    "brown",        /* 10 */
    "yellow",       /* 11 */
    "khaki"         /* 12 */
};

/**
 * Used for bsearch searching.
 */
static int compar(const struct bmappair *a, const struct bmappair *b) {
    return strcmp(a->name, b->name);
}

/**
 * Used for bsearch searching.
 */
static int compar_smooth(const struct smoothing *a, const struct smoothing *b) {
    if (a->id<b->id)
        return -1;
    if (b->id<a->id)
        return 1;
    return 0;
}


/**
 * Finds a color by name.
 *
 * @param name
 * color name, case-sensitive.
 * @return
 * the matching color in the coloralias if found,
 * 0 otherwise.
 *
 * @note
 * 0 will actually be black, so there is no
 * way the calling function can tell if an error occurred or not
 */
static uint8 find_color(const char *name) {
    uint8 i;
    for (i=0;i<sizeof(colorname)/sizeof(*colorname);i++)
        if (!strcmp(name,colorname[i]))
            return i;
    LOG(llevError,"Unknown color: %s\n",name);
    return 0;
}

/**
 * This reads the lib/faces file, getting color and visibility information.
 * it is called by read_bmap_names().
 *
 * @note
 * will call exit() if file doesn't exist.
 */
static void read_face_data(void) {
    char buf[MAX_BUF], *cp;
    New_Face *on_face=NULL;
    FILE *fp;

    snprintf(buf, sizeof(buf), "%s/faces", settings.datadir);
    LOG(llevDebug,"Reading faces from %s...\n",buf);
    if ((fp=fopen(buf,"r"))==NULL) {
        LOG(llevError, "Cannot open faces file: %s\n", strerror_local(errno, buf, sizeof(buf)));
        exit(-1);
    }
    while (fgets(buf, MAX_BUF, fp)!=NULL) {
        if (*buf=='#') continue;
        if (!strncmp(buf,"end",3)) {
            on_face = NULL;
        } else if (!strncmp(buf,"face",4)) {
            unsigned tmp;

            cp = buf + 5;
            cp[strlen(cp)-1] = '\0'; /* remove newline */

            if ((tmp = find_face(cp, (unsigned)-1)) == (unsigned)-1) {
                LOG(llevError,"Could not find face %s\n", cp);
                continue;
            }
            on_face = &new_faces[tmp];
            on_face->visibility=0;
        } else if (on_face==NULL) {
            LOG(llevError,"Got line with no face set: %s\n", buf);
        } else if (!strncmp(buf,"color_fg",8)) {
            cp = buf + 9;
            cp[strlen(cp)-1] = '\0';
            if (on_face->magicmap==255) on_face->magicmap=find_color(cp);
        } else if (!strncmp(buf,"visibility",10)) {
            on_face->visibility = atoi(buf + 11);
        } else if (!strncmp(buf,"magicmap",8)) {
            cp=buf+9;
            cp[strlen(cp)-1] = '\0';
            on_face->magicmap=find_color(cp);
        } else if (!strncmp(buf,"is_floor",8)) {
            int value = atoi(buf+9);
            if (value) on_face->magicmap |= FACE_FLOOR;
        } else LOG(llevDebug,"Got unknown line in faces file: %s\n", buf);
    }
    LOG(llevDebug,"done\n");
    fclose(fp);
}

/**
 * This reads the bmaps file to get all the bitmap names and
 * stuff.  It only needs to be done once, because it is player
 * independent (ie, what display the person is on will not make a
 * difference.)
 *
 * @note
 * will call exit() if file doesn't exist, and abort() in case of memory error.
 */
void read_bmap_names(void) {
    char buf[MAX_BUF], *p, *q;
    FILE *fp;
    int value, nrofbmaps = 0, i;
    size_t l;

    bmaps_checksum=0;
    snprintf(buf, sizeof(buf), "%s/bmaps", settings.datadir);
    LOG(llevDebug,"Reading bmaps from %s...\n",buf);
    if ((fp=fopen(buf,"r"))==NULL) {
        LOG(llevError, "Cannot open bmaps file: %s\n", strerror_local(errno, buf, sizeof(buf)));
        exit(-1);
    }

    /* First count how many bitmaps we have, so we can allocate correctly */
    while (fgets(buf, MAX_BUF, fp)!=NULL)
        if (buf[0] != '#' && buf[0] != '\n')
            nrofbmaps++;
    rewind(fp);

    xbm = (struct bmappair *) malloc(sizeof(struct bmappair) * nrofbmaps);
    if (xbm == NULL) {
        LOG(llevError, "read_bmap_names: xbm memory allocation failure.");
        abort();
    }
    memset(xbm, 0, sizeof(struct bmappair) * nrofbmaps);

    while (nroffiles < nrofbmaps && fgets(buf, MAX_BUF, fp) != NULL) {
        if (*buf == '#')
            continue;

        p = (*buf == '\\') ? (buf + 1): buf;
        if (!(p = strtok(p , " \t")) || !(q = strtok(NULL , " \t\n"))) {
            LOG(llevDebug,"Warning, syntax error: %s\n", buf);
            continue;
        }
        value = atoi(p);
        xbm[nroffiles].name = strdup_local(q);

        /* We need to calculate the checksum of the bmaps file
         * name->number mapping to send to the client.  This does not
         * need to match what sum or other utility may come up with -
         * as long as we get the same results on the same real file
         * data, it does the job as it lets the client know if
         * the file has the same data or not.
         */
        ROTATE_RIGHT(bmaps_checksum);
        bmaps_checksum += value & 0xff;
        bmaps_checksum &= 0xffffffff;

        ROTATE_RIGHT(bmaps_checksum);
        bmaps_checksum += (value >> 8) & 0xff;
        bmaps_checksum &= 0xffffffff;
        for (l=0; l<strlen(q); l++) {
            ROTATE_RIGHT(bmaps_checksum);
            bmaps_checksum += q[l];
            bmaps_checksum &= 0xffffffff;
        }

        xbm[nroffiles].number = value;
        nroffiles++;
        if (value >= nrofpixmaps)
            nrofpixmaps = value+1;
    }
    fclose(fp);

    LOG(llevDebug,"done (got %d/%d/%d)\n",nrofpixmaps,nrofbmaps,nroffiles);

    new_faces = (New_Face *)malloc(sizeof(New_Face) * nrofpixmaps);
    if (new_faces == NULL) {
        LOG(llevError, "read_bmap_names: new_faces memory allocation failure.");
        abort();
    }
    for (i = 0; i < nrofpixmaps; i++) {
        new_faces[i].name = "";
        new_faces[i].number = i;
        new_faces[i].visibility=0;
        new_faces[i].magicmap=255;
    }
    for (i = 0; i < nroffiles; i++) {
        new_faces[xbm[i].number].name = xbm[i].name;
    }

    qsort(xbm, nroffiles, sizeof(struct bmappair), (int (*)(const void*, const void*))compar);

    read_face_data();

    for (i = 0; i < nrofpixmaps; i++) {
        if (new_faces[i].magicmap==255) {
            new_faces[i].magicmap=0;
        }
    }
    /* Actually forcefully setting the colors here probably should not
     * be done - it could easily create confusion.
     */
    blank_face = &new_faces[find_face(BLANK_FACE_NAME, 0)];
    blank_face->magicmap = find_color("khaki") | FACE_FLOOR;

    empty_face = &new_faces[find_face(EMPTY_FACE_NAME, 0)];

    smooth_face = &new_faces[find_face(SMOOTH_FACE_NAME,0)];
}

/**
 * This returns an the face number of face 'name'.  Number is constant
 * during an invocation, but not necessarily between versions (this
 * is because the faces are arranged in alphabetical order, so
 * if a face is removed or added, all faces after that will now
 * have a different number.
 *
 * @param name
 * face to search for
 * @param error
 * value to return if face was not found.
 *
 * @note
 * If a face is not found, then error is returned.  This can be useful if
 * you want some default face used, or can be set to negative
 * so that it will be known that the face could not be found
 * (needed in client, so that it will know to request that image
 * from the server)
 */
unsigned find_face(const char *name, unsigned error) {
    struct bmappair *bp, tmp;
    char *p;

    if ((p = strchr(name, '\n')))
        *p = '\0';

    tmp.name = (char *)name;
    bp = (struct bmappair *)bsearch
         (&tmp, xbm, nroffiles, sizeof(struct bmappair), (int (*)(const void*, const void*))compar);

    return bp ? bp->number : error;
}

/**
 * Reads the smooth file to know how to smooth datas.
 * the smooth file if made of 2 elements lines.
 * lines starting with # are comment
 * the first element of line is face to smooth
 * the next element is the 16x2 faces picture
 * used for smoothing
 *
 * @note
 * will call exit() if file can't be opened.
 */
int read_smooth(void) {
    char buf[MAX_BUF], *p, *q;
    FILE *fp;
    int smoothcount = 0;

    snprintf(buf, sizeof(buf), "%s/smooth", settings.datadir);
    LOG(llevDebug,"Reading smooth from %s...\n",buf);
    if ((fp=fopen(buf,"r"))==NULL) {
        LOG(llevError, "Cannot open smooth file %s: %s\n", strerror_local(errno, buf, sizeof(buf)));
        exit(-1);
    }

    /* First count how many smooth we have, so we can allocate correctly */
    while (fgets(buf, MAX_BUF, fp)!=NULL)
        if (buf[0] != '#' && buf[0] != '\n')
            smoothcount++;
    rewind(fp);

    smooth = (struct smoothing *) malloc(sizeof(struct smoothing) * (smoothcount));
    memset(smooth, 0, sizeof(struct smoothing) * (smoothcount));

    while (nrofsmooth < smoothcount && fgets(buf, MAX_BUF, fp)!=NULL) {
        if (*buf == '#')
            continue;
        p=strchr(buf,' ');
        if (!p)
            continue;
        *p='\0';
        q=buf;
        smooth[nrofsmooth].id=find_face(q,0);
        q=p+1;
        smooth[nrofsmooth].smooth=find_face(q,0);
        nrofsmooth++;
    }
    fclose(fp);

    LOG(llevDebug,"done (got %d smooth entries)\n",nrofsmooth);
    qsort(smooth, nrofsmooth, sizeof(struct smoothing), (int (*)(const void*, const void*))compar_smooth);
    return nrofsmooth;
}

/**
 * Find the smooth face for a given face.
 *
 * @param face the face to find the smoothing face for
 *
 * @param smoothed return value: set to smooth face
 *
 * @return 1=smooth face found, 0=no smooth face found
 */
int find_smooth(uint16 face, uint16 *smoothed) {
    struct smoothing *bp, tmp;

    tmp.id = face;
    bp = (struct smoothing *)bsearch
         (&tmp, smooth, nrofsmooth, sizeof(struct smoothing), (int (*)(const void*, const void*))compar_smooth);
    (*smoothed)=0;
    if (bp)
        (*smoothed)=bp->smooth;
    return bp ? 1 : 0;
}

/**
 * Deallocates memory allocated by read_bmap_names() and read_smooth().
 */
void free_all_images(void) {
    int i;

    for (i=0; i<nroffiles; i++)
        free(xbm[i].name);
    free(xbm);
    free(new_faces);
    free(smooth);
}

/**
 * Checks fallback are correctly defined.
 * This is a simple recursive function that makes sure the fallbacks
 * are all proper (eg, the fall back to defined sets, and also
 * eventually fall back to 0).  At the top level, togo is set to
 * MAX_FACE_SETS.  If togo gets to zero, it means we have a loop.
 * This is only run when we first load the facesets.
 */
static void check_faceset_fallback(int faceset, int togo) {
    int fallback = facesets[faceset].fallback;

    /* proper case - falls back to base set */
    if (fallback == 0) return;

    if (!facesets[fallback].prefix) {
        LOG(llevError,"Face set %d falls to non set faceset %d\n",
            faceset, fallback);
        abort();
    }
    togo--;
    if (togo == 0) {
        LOG(llevError,"Infinite loop found in facesets. aborting.\n");
        abort();
    }
    check_faceset_fallback(fallback, togo);
}

/**
 * Loads all the image types into memory.
 *
 * This  way, we can easily send them to the client.  We should really
 * do something better than abort on any errors - on the other hand,
 * these are all fatal to the server (can't work around them), but the
 * abort just seems a bit messy (exit would probably be better.)
 *
 * Couple of notes:  We assume that the faces are in a continous block.
 * This works fine for now, but this could perhaps change in the future
 *
 * Function largely rewritten May 2000 to be more general purpose.
 * The server itself does not care what the image data is - to the server,
 * it is just data it needs to allocate.  As such, the code is written
 * to do such.
 */

void read_client_images(void) {
    char filename[400];
    char buf[HUGE_BUF];
    char *cp, *cps[7];
    FILE *infile;
    int num,len,compressed, fileno,i, badline;

    memset(facesets, 0, sizeof(facesets));
    snprintf(filename, sizeof(filename), "%s/image_info",settings.datadir);
    if ((infile=open_and_uncompress(filename, 0, &compressed))==NULL) {
        LOG(llevError,"Unable to open %s\n", filename);
        abort();
    }
    while (fgets(buf, HUGE_BUF-1, infile)!=NULL) {
        badline=0;

        if (buf[0] == '#') continue;
        if (!(cps[0] = strtok(buf, ":"))) badline=1;
        for (i=1; i<7; i++) {
            if (!(cps[i] = strtok(NULL, ":"))) badline=1;
        }
        if (badline) {
            LOG(llevError,"Bad line in image_info file, ignoring line:\n  %s",
                buf);
        } else {
            len = atoi(cps[0]);
            if (len >=MAX_FACE_SETS) {
                LOG(llevError,"To high a setnum in image_info file: %d > %d\n",
                    len, MAX_FACE_SETS);
                abort();
            }
            facesets[len].prefix = strdup_local(cps[1]);
            facesets[len].fullname = strdup_local(cps[2]);
            facesets[len].fallback = atoi(cps[3]);
            facesets[len].size = strdup_local(cps[4]);
            facesets[len].extension = strdup_local(cps[5]);
            facesets[len].comment = strdup_local(cps[6]);
        }
    }
    close_and_delete(infile,compressed);
    for (i=0; i<MAX_FACE_SETS; i++) {
        if (facesets[i].prefix) check_faceset_fallback(i, MAX_FACE_SETS);
    }
    /* Loaded the faceset information - now need to load up the
     * actual faces.
     */

    for (fileno=0; fileno<MAX_FACE_SETS; fileno++) {
        /* if prefix is not set, this is not used */
        if (!facesets[fileno].prefix) continue;
        facesets[fileno].faces = calloc(nrofpixmaps, sizeof(face_info));

        snprintf(filename, sizeof(filename), "%s/crossfire.%d",settings.datadir, fileno);
        LOG(llevDebug,"Loading image file %s\n", filename);

        if ((infile = open_and_uncompress(filename,0,&compressed))==NULL) {
            LOG(llevError,"Unable to open %s\n", filename);
            abort();
        }
        while (fgets(buf, HUGE_BUF-1, infile)!=NULL) {
            if (strncmp(buf,"IMAGE ",6)!=0) {
                LOG(llevError, "read_client_images:Bad image line - not IMAGE, instead\n%s",buf);
                abort();
            }
            num = atoi(buf+6);
            if (num<0 || num>=nrofpixmaps) {
                LOG(llevError,
                    "read_client_images: Image num %d not in 0..%d\n%s",
                    num,nrofpixmaps,buf);
                abort();
            }
            /* Skip accross the number data */
            for (cp=buf+6; *cp!=' '; cp++) ;
            len = atoi(cp);
            if (len==0 || len>MAX_IMAGE_SIZE) {
                LOG(llevError,
                    "read_client_images: length not valid: %d > %d \n%s",
                    len,MAX_IMAGE_SIZE,buf);
                abort();
            }
            /* We don't actualy care about the name if the image that
             * is embedded in the image file, so just ignore it.
             */
            facesets[fileno].faces[num].datalen = len;
            facesets[fileno].faces[num].data = malloc(len);
            if ((i=fread(facesets[fileno].faces[num].data,
                         len, 1, infile)) != 1) {
                LOG(llevError,"read_client_images: Did not read desired amount of data, wanted %d, got %d\n%s",
                    len, i, buf);
                abort();
            }
            facesets[fileno].faces[num].checksum=0;
            for (i=0; i<len; i++) {
                ROTATE_RIGHT(facesets[fileno].faces[num].checksum);
                facesets[fileno].faces[num].checksum +=
                    facesets[fileno].faces[num].data[i];
                facesets[fileno].faces[num].checksum &= 0xffffffff;
            }
        }
        close_and_delete(infile,compressed);
    } /* For fileno < MAX_FACE_SETS */
}

face_sets facesets[MAX_FACE_SETS];    /**< All facesets */

/**
 * Checks specified faceset is valid
 * \param fsn faceset number
 */
int is_valid_faceset(int fsn) {
    if (fsn >=0 && fsn < MAX_FACE_SETS && facesets[fsn].prefix) return TRUE;
    return FALSE;
}

/**
 * Frees all faceset information
 */
void free_socket_images(void) {
    int num,q;

    for (num=0;num<MAX_FACE_SETS; num++) {
        if (facesets[num].prefix) {
            for (q=0; q<nrofpixmaps; q++)
                if (facesets[num].faces[q].data)
                    free(facesets[num].faces[q].data);
            free(facesets[num].prefix);
            free(facesets[num].fullname);
            free(facesets[num].size);
            free(facesets[num].extension);
            free(facesets[num].comment);
            free(facesets[num].faces);
        }
    }
}

/**
 * This returns the set we will actually use when sending
 * a face.  This is used because the image files may be sparse.
 * This function is recursive.  imageno is the face number we are
 * trying to send
 *
 * If face is not found in specified faceset, tries with 'fallback' faceset.
 *
 * \param faceset faceset to check
 * \param imageno image number
 *
 */
int get_face_fallback(int faceset, int imageno) {
    /* faceset 0 is supposed to have every image, so just return.  Doing
     * so also prevents infinite loops in the case if it not having
     * the face, but in that case, we are likely to crash when we try
     * to access the data, but that is probably preferable to an infinite
     * loop.
     */
    if (faceset==0) return 0;

    if (!facesets[faceset].prefix) {
        LOG(llevError,"get_face_fallback called with unused set (%d)?\n",
            faceset);
        return 0;   /* use default set */
    }
    if (facesets[faceset].faces[imageno].data) return faceset;
    return get_face_fallback(facesets[faceset].fallback, imageno);
}

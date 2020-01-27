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

/**
 * @file
 * Handles face-related stuff, including the actual face data.
 */

#include "global.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"

#include "assets.h"

/**
 * Following can just as easily be pointers, but
 * it is easier to keep them like this.
 */
const Face *blank_face, *empty_face, *smooth_face;


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
    "khaki",        /* 12 */
    "gray",         /* 13 */
    "light",        /* 14 */
    "cyan",         /* 15 */
    "magenta"       /* 16 */
};

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
uint8_t find_color(const char *name) {
    uint8_t i;

    for (i = 0; i < sizeof(colorname)/sizeof(*colorname); i++)
        if (!strcmp(name, colorname[i]))
            return i;

    LOG(llevError, "Unknown color: %s\n", name);
    return 0;
}

const char *get_colorname(uint8_t index) {
    if (index < sizeof(colorname) / sizeof(*colorname)) {
        return colorname[index];
    }
    return "";
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
int find_smooth(const Face *face, const Face **smoothed) {
    (*smoothed) = NULL;

    if (face && face->smoothface) {
        (*smoothed) = face->smoothface;
        return 1;
    }

    return 0;
}

/**
 * Checks specified faceset is valid
 * \param fsn faceset number
 */
int is_valid_faceset(int fsn) {
    return find_faceset(fsn) != NULL;
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
int get_face_fallback(int faceset, uint16_t imageno) {
    /* faceset 0 is supposed to have every image, so just return.  Doing
     * so also prevents infinite loops in the case if it not having
     * the face, but in that case, we are likely to crash when we try
     * to access the data, but that is probably preferable to an infinite
     * loop.
     */

    face_sets *fs = find_faceset(faceset);
    if (!fs || !fs->prefix) {
        LOG(llevError, "get_face_fallback called with unused set (%d)?\n", faceset);
        return 0;   /* use default set */
    }
    if (imageno < fs->allocated && fs->faces[imageno].data)
        return faceset;

    if (!fs->fallback) {
        return 0;
    }

    return get_face_fallback(fs->fallback->id, imageno);
}

static void do_face(const Face *face) {
    fprintf(stderr, "%5d %50s %50s\n", face->number, face->name, face->smoothface ? face->smoothface->name : "(none)");
}

/**
 * Dump all faces to stderr, for debugging purposes.
 */
void dump_faces(void) {
    fprintf(stderr, "id    name                                               smooth\n");
    faces_for_each(do_face);
}

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
 * This file contains description-related methods that are common to many
 * classes of objects.
 */

#include "global.h"

#include <stdlib.h>
#include <string.h>

#include "ob_methods.h"
#include "ob_types.h"

/**
 * Describes an object, seen by a given observer.
 * @param context The method context
 * @param op The object to describe
 * @param observer The object to make the description to
 * @param buf Buffer that will contain the description
 * @param size buf's size.
 */
void common_ob_describe(const ob_methods *context, const object *op, const object *observer, char *buf, size_t size) {
    char *desc;

    buf[0] = '\0';

    if (op == NULL)
        return;

    desc = stringbuffer_finish(describe_item(op, observer, 0, NULL));
    if (desc[0] != '\0') {
        size_t len;

        query_name(op, buf, size-1);
        buf[size-1] = 0;
        len = strlen(buf);
        if ((len+5) < size) {
            /* Since we know the length, we save a few cpu cycles by using
             * it instead of calling strcat */
            strcpy(buf+len, " ");
            len++;
            strncpy(buf+len, desc, size-len-1);
            buf[size-1] = 0;
        }
    }
    free(desc);
    if (buf[0] == '\0') {
        query_name(op, buf, size-1);
        buf[size-1] = 0;
    }
}

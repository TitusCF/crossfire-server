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
 * The implementation of @ref page_type_83 "duplicator" objects.
 */

#include "global.h"

#include <string.h>

#include "ob_methods.h"
#include "ob_types.h"
#include "sounds.h"
#include "sproto.h"

static method_ret duplicator_type_trigger(ob_methods *context, object *op, object *cause, int state);

/**
 * Initializer for the @ref page_type_83 "duplicator" object type.
 */
void init_type_duplicator(void) {
    register_trigger(DUPLICATOR, duplicator_type_trigger);
}

/**
 * Trigger for @ref page_type_83 "duplicator".
 *
 * Will duplicate a specified object placed on top of it.
 * - connected: what will trigger it.
 * - level: multiplier.  0 to destroy.
 * - other_arch: the object to look for and duplicate.
 *
 * @param op
 * duplicator.
 */
static void move_duplicator(object *op) {
    object *tmp;

    if (!op->other_arch) {
        LOG(llevInfo, "Duplicator with no other_arch! %d %d %s\n", op->x, op->y, op->map ? op->map->path : "nullmap");
        return;
    }

    if (op->above == NULL)
        return;
    for (tmp = op->above; tmp != NULL; tmp = tmp->above) {
        if (strcmp(op->other_arch->name, tmp->arch->name) == 0) {
            if (op->level <= 0) {
                object_remove(tmp);
                object_free_drop_inventory(tmp);
            } else {
                uint64_t new_nrof = (uint64_t)tmp->nrof*op->level;

                if (new_nrof >= 1UL<<31)
                    new_nrof = 1UL<<31;
                tmp->nrof = new_nrof;
            }
            break;
        }
    }
}

/**
 * A @ref page_type_83 "duplicator" is triggered.
 * @param context Ignored.
 * @param op The duplicator being triggered
 * @param cause Ignored.
 * @param state Ignored.
 * @retval METHOD_OK
 */
static method_ret duplicator_type_trigger(ob_methods *context, object *op, object *cause, int state) {
    move_duplicator(op);
    return METHOD_OK;
}

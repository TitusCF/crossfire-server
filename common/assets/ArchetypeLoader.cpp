/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "ArchetypeLoader.h"
#include "Archetypes.h"

extern "C" {
#include "global.h"
#include "loader.h"
}

int arch_init;  /**< True if doing arch initialization @todo remove */

ArchetypeLoader::ArchetypeLoader(Archetypes *archetypes) : m_archetypes(archetypes) {
}

void ArchetypeLoader::processFile(FILE *file, const std::string &filename) {
    archetype *at, *head = NULL, *last_more = NULL;
    int i, first = LO_NEWFILE;

    at = get_archetype_struct();
    CLEAR_FLAG(&at->clone, FLAG_REMOVED);

    arch_init = 1;

    while ((i = load_object(file, &at->clone, first, MAP_STYLE))) {
        first = 0;
        at->clone.speed_left = (float)(-0.1);
        at = m_archetypes->define(at->name, at);

        switch (i) {
        case LL_NORMAL: /* A new archetype, just link it with the previous */
            head = last_more = at;
            at->tail_x = 0;
            at->tail_y = 0;
            break;

        case LL_MORE: /* Another part of the previous archetype, link it correctly */
            at->head = head;
            at->clone.head = &head->clone;
            if (last_more != NULL) {
                last_more->more = at;
                last_more->clone.more = &at->clone;
            }
            last_more = at;

            /* Set FLAG_MONSTER throughout parts if head has it */
            if (QUERY_FLAG(&head->clone, FLAG_MONSTER)) {
                SET_FLAG(&at->clone, FLAG_MONSTER);
            }

            /* If this multipart image is still composed of individual small
             * images, don't set the tail_.. values.  We can't use them anyways,
             * and setting these to zero makes the map sending to the client much
             * easier as just looking at the head, we know what to do.
             */
            if (at->clone.face != head->clone.face) {
                head->tail_x = 0;
                head->tail_y = 0;
            } else {
                if (at->clone.x > head->tail_x)
                    head->tail_x = at->clone.x;
                if (at->clone.y > head->tail_y)
                    head->tail_y = at->clone.y;
            }
            break;
        }

        at = get_archetype_struct();
        CLEAR_FLAG(&at->clone, FLAG_REMOVED);
    }
    at->clone.arch = NULL; /* arch is checked for temporary archetypes if not NULL. */
    free(at);

    arch_init = 0;
}

/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2021 The Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @defgroup module_citybell City bell module
 * Rings the temple bells in the main cities.
 *
 * @{
 */

/**
 * @file
 * This file is part of the @ref module_citybell "city bell module".
 * See this page for more information.
 */

#include <stdarg.h>
#include <string.h>

#include "global.h"
#include "object.h"
#include "sproto.h"

static int last_hr;

// Static functions

/**
 * Ring the temple bells for a player in Scorn.
 *
 * @param pl
 * The object of the player for whom the bells toll.
 */
static void ring_scorn(object *pl) {
    const char *god_name = determine_god(pl);
    char buf[MAX_BUF];
    // TODO: We could be really clever and use the return value of strcmp to reduce comparisons.
    if (!strcmp(god_name, "Devourers") ||
        !strcmp(god_name, "Sorig") ||
        !strcmp(god_name, "Ruggilli") ||
        !strcmp(god_name, "Gaea") ||
        !strcmp(god_name, "Mostrai") ||
        !strcmp(god_name, "Lythander")) {
            snprintf(buf, MAX_BUF, "You hear the bell of the glorious temple of %s.", god_name);
    }
    else if (!strcmp(god_name, "Valriel") ||
        !strcmp(god_name, "Gorokh")) {
            snprintf(buf, MAX_BUF, "You hear the bell of the glorious church of %s.", god_name);
    }
    else
        strcpy(buf, "You hear the bells of the various temples of Scorn.");

    draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, buf);
}

/**
 * Ring the bell in Darcap
 *
 * @param pl
 * The object of the player hearing the bell.
 */
static void ring_darcap(object *pl) {
    const char *god_name = determine_god(pl);

    if (!strcmp(god_name, "Devourers")) {
        draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, "You hear the glorious bell of St. Andreas.");
    }
    else if (!strcmp(god_name, "Valkyrie")) {
        draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, "You hear the bells of the glorious church of Valkyrie.");
    }
    else {
        draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, "You hear the bells of St. Andreas and another church.");
    }
}

/**
 * Ring the bells in Navar
 *
 * @param pl
 * Player object to hear the bells
 */
static void ring_navar(object *pl) {
    const char *god_name = determine_god(pl);

    if (!strcmp(god_name, "Gorokh") ||
        !strcmp(god_name, "Ruggilli") ||
        !strcmp(god_name, "Sorig") ||
        !strcmp(god_name, "Valkyrie") ||
        !strcmp(god_name, "Valriel")) {
            char buf[MAX_BUF];
            snprintf(buf, MAX_BUF, "You hear the bell of the glorious temple of %s.", god_name);
            draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE,buf);
    }
    else if (!strcmp(god_name, "Mostrai")) {
            draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, "You hear the bell of Mostrai's glorious cathedral.");
    }
    else if (!strcmp(god_name, "Gaea")) {
            draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, "You hear the bell of Gaea's glorious shrine.");
    }
    else {
            draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, "You hear the bells of the temples of Navar.");
    }
}

/**
 * Ring the city bells for each player.
 */
static void ring_bell(void) {
    object *pl;
    region *reg;
    const char *reg_name;

    pl = first_player ? first_player->ob : NULL;
    while (pl) {
        // If the player is on a map, then try to ring the bell
        if (pl->map) {
            reg = get_region_by_map(pl->map);
            if (reg) {
                reg_name = reg->name;
                if (!strcmp(reg_name, "scorn")) {
                    ring_scorn(pl);
                }
                else if (!strcmp(reg_name, "darcap")) {
                    ring_darcap(pl);
                }
                else if (!strcmp(reg_name, "navar")) {
                    ring_navar(pl);
                }
            }
        }

        pl = pl->contr->next ? pl->contr->next->ob : NULL;
    }
}

/**
 * Global event handling, only uses EVENT_CLOCK.
 * @param type
 * The event type.
 * @return
 * 0.
 */
static int clock_listener(int *type, ...) {
    va_list args;
    int code;
    timeofday_t tod;

    va_start(args, type);
    code = va_arg(args, int);

    switch (code) {
        case EVENT_CLOCK:
            get_tod(&tod);
            if (tod.hour != last_hr) {
                last_hr = tod.hour;
                ring_bell();
            }
            break;
    }

    va_end(args);

    return 0;
}

void cfcitybell_init(Settings *settings) {
    timeofday_t tod;
    get_tod(&tod);
    last_hr = tod.hour;
    events_register_global_handler(EVENT_CLOCK, clock_listener);

    /* Disable the plugin in case it's still there */
    linked_char *disable = calloc(1, sizeof(linked_char));
    disable->next = settings->disabled_plugins;
    disable->name = strdup("cfcitybell");
    settings->disabled_plugins = disable;
}

void cfcitybell_close() {
}

/*@}*/

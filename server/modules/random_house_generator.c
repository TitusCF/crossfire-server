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
 * @defgroup module_rhg Random house generation module
 * This module links unused exits to random maps.
 * The random map parameters vary based on the map it is in, but each exit will always use
 * the same random seed to have the same layout and content.
 *
 * Exits will only point to a random map if both their @ref obj::slaying "slaying" and
 * @ref obj::msg "msg" fields are empty.
 *
 * @todo
 * - make more parameters vary based on maps
 * - add exits to all towns
 *
 * @{
 */

/**
 * @file
 * This file is part of the @ref module_rhg "random house generation plugin".
 * See this page for more information.
 */

#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "global.h"
#include "object.h"
#include "sproto.h"

/** Link between a map and the exits to generate for it. */
typedef struct house_zone_struct {
    const char *mappath;        /**< Full map path. */
    const char *monsterstyle;   /**< Style of monsters. */
} house_zone_struct;

/** Maps we work on. */
static const house_zone_struct zones[] = {
    /* Scorn */
    { "/world/world_104_115", "city" },
    { "/world/world_105_115", "city" },
    { "/world/world_104_116", "city" },
    { "/world/world_105_116", "city" },
    /* Navar */
    { "/world/world_122_116", "city" },
    { "/world/world_121_116", "city" },
    { "/world/world_122_117", "city" },
    { "/world/world_121_117", "city" },
    { NULL, NULL }
};

/**
 * Get the random map parameters of a map.
 * @param map
 * map to get the zone of.
 * @return
 * NULL if the map shouldn't be processed, else its parameters.
 */
static const house_zone_struct *get_map_zone(const mapstruct *map) {
    int zone;

    for (zone = 0; zones[zone].mappath != NULL; zone++) {
        if (strcmp(zones[zone].mappath, map->path) == 0)
            return &zones[zone];
    }

    return NULL;
}

/**
 * Should we add a random map to this exit?
 * @param exit
 * exit to check.
 * @return
 * 1 if a map should be set, 0 else.
 */
static int is_suitable_exit(object *exit) {
    assert(exit);

    if (exit->type != EXIT)
        return 0;
    if (exit->slaying || exit->msg)
        return 0;

    return 1;
}

/**
 * Get the random map seed. Will always yield the same value for the same (position included) exit.
 * @param exit
 * exit to get the seed of.
 * @param map
 * map the exit is on.
 * @return
 * random seed.
 */
static int get_exit_seed(const object *exit, const mapstruct *map) {
    char r[500];
    int seed = 0, len, w = 0;

    snprintf(r, sizeof(r), "%s!%d,%d*%s", exit->arch->name, exit->x, exit->y, map->path);

    len = strlen(r)-1;
    while (len >= 0) {
        seed ^= ((int)r[len])<<w;
        w += 8;
        w = w%32;
        len--;
    }

    return seed;
}

/**
 * Change an empty exit to point to a random map.
 * @param exit
 * exit to alter.
 * @param zone
 * zone we're part of, to know the random map parameters.
 * @param map
 * map the exit is on.
 */
static void add_exit_to_item(object *exit, const house_zone_struct *zone, const mapstruct *map) {
    char params[MAX_BUF];

    assert(exit);
    assert(zone);

    snprintf(params, sizeof(params), "layoutstyle onion\n"
        "floorstyle indoor\n"
        "wallstyle wooden\n"
        "monsterstyle %s\n"
        "dungeon_level 1\n"
        "dungeon_depth 1\n"
        "decorstyle furniture\n"
        "random_seed %d\n",
        zone->monsterstyle,
        get_exit_seed(exit, map));

    exit->slaying = add_string("/!");
    exit->msg = add_string(params);
}

/**
 * Checks if the map should be processed, and if so process it.
 * @param map
 * map to work on.
 */
static void add_exits_to_map(const mapstruct *map) {
    int x, y;
    const house_zone_struct *zone = get_map_zone(map);

    if (!zone)
        return;

    for (x = 0; x < MAP_WIDTH(map); x++) {
        for (y = 0; y < MAP_HEIGHT(map); y++) {
            FOR_MAP_PREPARE(map, x, y, item) {
                if (is_suitable_exit(item))
                    add_exit_to_item(item, zone, map);
            } FOR_MAP_FINISH();
        }
    }
}

/**
 * Global server event handling. Only uses EVENT_MAPLOAD.
 * @param type
 * unused.
 * @return
 * 0.
 */
static int cfrhg_globalEventListener(int *type, ...) {
    va_list args;
    int rv = 0;
    mapstruct *map;
    int code;

    va_start(args, type);
    code = va_arg(args, int);

    switch (code) {
    case EVENT_MAPLOAD:
        map = va_arg(args, mapstruct *);
        add_exits_to_map(map);
        break;
    }
    va_end(args);

    return rv;
}

static event_registration eg;

/**
 * Module initialisation.
*/
void random_house_generator_init(Settings *settings) {
    eg = events_register_global_handler(EVENT_MAPLOAD, cfrhg_globalEventListener);

    /* Disable the plugin in case it's still there */
    linked_char *disable = calloc(1, sizeof(linked_char));
    disable->next = settings->disabled_plugins;
    disable->name = strdup("cfrhg");
    settings->disabled_plugins = disable;
}

/**
 * Close the module.
 */
void random_house_generator_close() {
    events_unregister_global_handler(EVENT_MAPLOAD, eg);
}
/*@}*/

/*****************************************************************************/
/* Template for version 2.0 plugins.                                         */
/* Contact: yann.chachkoff@myrealbox.com                                     */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001-2005 by Chachkoff Yann (Feel free to deliver your complaints)     */
/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000 Mark Wedel                                            */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */
/*****************************************************************************/

/**
 * @defgroup plugin_citylife City life plugin
 * This plugin adds random NPCs to town, and makes them enter houses, spawns new
 * ones.
 *
 * When a map is loaded, NPCs are randomly added so they appear already. During the
 * course of the server, some will enter houses (disappear), others will exit from
 * houses (appear on a house).
 *
 * For each map to be processed, two things are defined:
 * - spawn zones, where NPCs should be added when the map is loaded
 * - spawn points, from which to add new NPCs while the map is in memory.
 *   Should probably be houses and such.
 *
 * NPCs use a key/value to prevent them from immediately entering the building
 * they exited.
 *
 * @todo
 * - define spawn points/zones for other towns
 * - vary NPCs based on time of day
 * - define "objectives" to go to
 * - make NPCs pause when player talks to them
 *
 * @{
 */

/**
 * @file
 * This file is part of the @ref plugin_citylife "city life plugin".
 * See this page for more information.
 */

#include <citylife.h>
#include <stdarg.h>
#ifndef __CEXTRACT__
#include <citylife_proto.h>
#endif
#include <svnversion.h>

CF_PLUGIN char SvnRevPlugin[] = SVN_REV;

CF_PLUGIN int initPlugin(const char *iversion, f_plug_api gethooksptr) {
    cf_init_plugin(gethooksptr);

    cf_log(llevDebug, PLUGIN_VERSION " init\n");

    return 0;
}

CF_PLUGIN void *getPluginProperty(int *type, ...) {
    va_list args;
    const char *propname;
    int size;
    char *buf;

    va_start(args, type);
    propname = va_arg(args, const char *);

    if (!strcmp(propname, "Identification")) {
        buf = va_arg(args, char *);
        size = va_arg(args, int);
        va_end(args);
        snprintf(buf, size, PLUGIN_NAME);
        return NULL;
    } else if (!strcmp(propname, "FullName")) {
        buf = va_arg(args, char *);
        size = va_arg(args, int);
        va_end(args);
        snprintf(buf, size, PLUGIN_VERSION);
        return NULL;
    }
    va_end(args);
    return NULL;
}

CF_PLUGIN int citylife_runPluginCommand(object *op, char *params) {
    return -1;
}

/** Key to contain whether it's the first move of the NPC or not. */
#define FIRST_MOVE_KEY  "citylife_first_move"

/**
 * Point from which a NPC can come when the map is loaded.
 */
typedef struct {
    int x;
    int y;
} spawn_point;

/**
 * Zone in which to add NPCs when the map was just loaded.
 * NPC will be added in [sx, ex[ and [sy, ey[.
 */
typedef struct {
    int sx, sy, ex, ey;
} spawn_zone;

/**
 * Options for a map.
 */
typedef struct {
    const spawn_point *points;                 /**< Points to spawn from when there is a player on the map. */
    int count_points;                          /**< How many items in points. */
    const spawn_zone *zones;                   /**< Zones where to spawn at load time. */
    int count_zones;                           /**< How many items in zones. */
    int population;                            /**< Maximum of NPCs to add at load time. */
    const char *mapname;                       /**< Map path. */
    const char *const *available_archetypes;   /**< What archetypes can we chose from for an NPC? */
    int archetypes_count;                      /**< Number of items in available_archetypes. */
} mapzone;
/*@}*/
/**
 * @defgroup citylife_scorn Scorn parameters
 * Parameters for the @ref plugin_citylife "City life" plugin for Scorn.
 *
 * The city is pretty rectangular, so quite easy to define large zones to add to.
 * @ingroup plugin_citylife
 */
/*@{*/
/** Zones for map 104_115. */
static const spawn_zone scorn_nw_zones[] = {
    { 40, 26, 50, 50 }
};

/** Points for map 104_115. */
static const spawn_point scorn_nw_points[] = {
    { 41, 37 },
    { 48, 35 },
    { 49, 40 },
    { 47, 22 },
    { 49, 37 }
};

/** Zones for map 105_115. */
static const spawn_zone scorn_ne_zones[] = {
    { 0, 26, 22, 50 }
};

/** Points for map 105_115. */
static const spawn_point scorn_ne_points[] = {
    { 15, 42 },
    { 9, 35 },
    { 15, 29 },
    { 1, 25 },
    { 1, 29 }
};

/** Zones for map 104_116. */
static const spawn_zone scorn_sw_zones[] = {
    { 41, 0, 50, 10 }
};

/** Points for map 104_116. */
static const spawn_point scorn_sw_points[] = {
    { 41, 2 },
    { 46, 8 },
    { 42, 8 }
};

/** Zones for map 105_116. */
static const spawn_zone scorn_se_zones[] = {
    { 0, 0, 13, 10 }
};

/** Points for map 105_116. */
static const spawn_point scorn_se_points[] = {
    { 2, 8 },
    { 11, 8 },
    { 8, 1 },
    { 5, 8 }
};

/** Archetypes to spawn in Scorn. */
static const char *const scorn_archs[] = {
    "c_man",
    "c_woman",
    "child",
    "farmer",
    "fatman",
    "fatwoman",
    "guard",
    "knight",
    "man",
    "nun",
    "sage",
    "woman"
};
/*@}*/

/** @ingroup  plugin_citylife
@{*/
/**
 * All maps we work on.
 */
static const mapzone available_zones[] = {
    { scorn_nw_points, 5, scorn_nw_zones, 1, 2, "/world/world_104_115", scorn_archs, 12 },
    { scorn_ne_points, 1, scorn_ne_zones, 1, 5, "/world/world_105_115", scorn_archs, 12 },
    { scorn_sw_points, 3, scorn_sw_zones, 1, 5, "/world/world_104_116", scorn_archs, 12 },
    { scorn_se_points, 1, scorn_se_zones, 1, 5, "/world/world_105_116", scorn_archs, 12 },
    { NULL, -1, NULL, -1, 1, "", NULL, 0 },
};

/**
 * Finds if a map has a zone defined.
 *
 * @param map
 * candidat map.
 * @return
 * map zone, NULL if not defined.
 */
static const mapzone *get_zone_for_map(mapstruct *map) {
    int test;

    for (test = 0; available_zones[test].count_points != -1; test++) {
        if (strcmp(available_zones[test].mapname, map->path) == 0)
            return &available_zones[test];
    }
    return NULL;
}

/**
 * Creates a NPC for the specified zone, and do needed initialization.
 * @param zone
 * what NPCs to create.
 * @return
 * new NPC, with event handled for time. NULL if invalid archetype in the zone.
 */
static object *get_npc(const mapzone *zone) {
    int arch = RANDOM()%zone->archetypes_count;
    object *npc = cf_create_object_by_name(zone->available_archetypes[arch]);
    object *evt;

    if (!npc) {
        cf_log(llevError, PLUGIN_NAME ": get_npc() got NULL object for %s!\n", zone->available_archetypes[arch]);
        return NULL;
    }

    cf_object_set_flag(npc, FLAG_RANDOM_MOVE, 1);
    /* Prevent disease spreading in Scorn, mostly rabies. */
    cf_object_set_flag(npc, FLAG_UNDEAD, 1);
    /* add a key so NPC will not disappear in the house it came from */
    cf_object_set_key(npc, FIRST_MOVE_KEY, "1", 1);

    evt = cf_create_object_by_name("event_time");
    evt->slaying = cf_add_string(PLUGIN_NAME);
    evt->title = cf_add_string(PLUGIN_NAME);
    cf_object_insert_object(evt, npc);

    return npc;
}

/**
 * Add an NPC somewhere in a spawn zone.
 * @param zone
 * map zone definition from which to get a spawn zone.
 * @param map
 * map to insert into.
 */
static void add_npc_to_zone(const mapzone *zone, mapstruct *map) {
    int which;
    object *npc = get_npc(zone);

    if (!npc)
        return;
    which = RANDOM()%zone->count_zones;
    if (cf_object_teleport(npc, map, zone->zones[which].sx+RANDOM()%(zone->zones[which].ex-zone->zones[which].sx), zone->zones[which].sy+RANDOM()%(zone->zones[which].ey-zone->zones[which].sy))) {
        cf_object_free_drop_inventory(npc);
    }
}

/**
 * Add an NPC somewhere at a spawn point.
 * @param zone
 * map zone definition from which to get a spawn point.
 * @param map
 * map to insert into.
 */
static void add_npc_to_point(const mapzone *zone, mapstruct *map) {
    int which;
    object *npc = get_npc(zone);

    which = RANDOM()%zone->count_points;
    if (cf_object_teleport(npc, map, zone->points[which].x, zone->points[which].y)) {
        cf_object_free_drop_inventory(npc);
    }
}

/**
 * Add some NPCs to the map, based on the zone definition.
 * @param map
 * map to add to.
 */
static void add_npcs_to_map(mapstruct *map) {
    int add;
    const mapzone *zone = get_zone_for_map(map);

    if (!zone)
        return;

    add = 1+RANDOM()%zone->population;
    cf_log(llevDebug, PLUGIN_NAME ": adding %d NPC to map %s.\n", add, map->path);

    while (add-- >= 0) {
        add_npc_to_zone(zone, map);
    }
}

/**
 * Find a suitable map loaded and add an NPC to it.
 */
static void add_npc_to_random_map(void) {
    int count, test;
    mapstruct *list[50];
    int zones[50];
    count = 0;

    cf_log(llevDebug, PLUGIN_NAME ": adding NPC to random map.\n");

    for (test = 0; available_zones[test].count_points != -1 && count < 50; test++) {
        if ((list[count] = cf_map_has_been_loaded(available_zones[test].mapname)) && (list[count]->in_memory == MAP_IN_MEMORY)) {
            zones[count] = test;
            count++;
        }
    }
    if (!count)
        return;

    test = RANDOM()%count;
    add_npc_to_point(&available_zones[zones[test]], list[test]);
}

CF_PLUGIN int citylife_globalEventListener(int *type, ...) {
    va_list args;
    int rv = 0;
    mapstruct *map;
    int code;

    va_start(args, type);
    code = va_arg(args, int);

    switch (code) {
    case EVENT_MAPLOAD:
        map = va_arg(args, mapstruct *);
        add_npcs_to_map(map);
        break;

    case EVENT_CLOCK:
        if (RANDOM()%40 == 0)
            add_npc_to_random_map();
    }
    va_end(args);

    return rv;
}

CF_PLUGIN int postInitPlugin(void) {
    cf_log(llevDebug, PLUGIN_VERSION " post init\n");

    /* Pick the global events you want to monitor from this plugin */

/*
    cf_system_register_global_event(EVENT_BORN, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_CRASH, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_PLAYER_DEATH, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_GKILL, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_LOGIN, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_LOGOUT, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_MAPENTER, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_MAPLEAVE, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_MAPRESET, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_REMOVE, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_SHOUT, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_TELL, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_MUZZLE, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_KICK, PLUGIN_NAME, citylife_globalEventListener);
*/
    cf_system_register_global_event(EVENT_CLOCK, PLUGIN_NAME, citylife_globalEventListener);
    cf_system_register_global_event(EVENT_MAPLOAD, PLUGIN_NAME, citylife_globalEventListener);
/*
    cf_system_register_global_event(EVENT_MAPRESET, PLUGIN_NAME, citylife_globalEventListener);
*/

    return 0;
}

CF_PLUGIN int eventListener(int *type, ...) {
    int rv = 1;
    va_list args;
    /*char *buf;*/
    object *ground, *who/*, *activator, *third, *event*/;
    /*int fix;*/
    const char *value;

    va_start(args, type);

    who = va_arg(args, object *);
    /*activator =*/ va_arg(args, object *);
    /*third =*/ va_arg(args, object *);
    /*buf =*/ va_arg(args, char *);
    /*fix =*/ va_arg(args, int);
    /*event =*/ va_arg(args, object *);
    va_arg(args, talk_info *); /* ignored for now */
    va_end(args);

    /* should our npc disappear? */
    if (RANDOM()%100 < 30) {
        for (ground = cf_map_get_object_at(who->map, who->x, who->y); ground; ground = cf_object_get_object_property(ground, CFAPI_OBJECT_PROP_OB_ABOVE)) {
            if (ground->type == EXIT) {
                object *inv;

                value = cf_object_get_key(who, FIRST_MOVE_KEY);
                if (strcmp(value, "1") == 0) {
                    cf_object_set_key(who, FIRST_MOVE_KEY, "0", 1);
                    break;
                }

                /* must set inventory as no drop, else it'll just drop on the ground */
                for (inv = cf_object_get_object_property(who, CFAPI_OBJECT_PROP_INVENTORY); inv; inv = cf_object_get_object_property(inv, CFAPI_OBJECT_PROP_OB_BELOW))
                    cf_object_set_flag(inv, FLAG_NO_DROP, 1);

                cf_log(llevDebug, PLUGIN_NAME ": NPC entering building.\n");
                cf_object_remove(who);
                cf_object_free_drop_inventory(who);
                return rv;
            }
        }
    }

    /* we have to move manually, because during the night NPCs don't move. */
    cf_object_move(who, 1+RANDOM()%8, NULL);

    return rv;
}

CF_PLUGIN int   closePlugin(void) {
    cf_log(llevDebug, PLUGIN_VERSION " closing\n");
    return 0;
}
/*@}*/

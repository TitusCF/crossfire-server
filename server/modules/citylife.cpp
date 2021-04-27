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
 * @defgroup module_citylife City life module
 * This module adds random NPCs to town, and makes them enter houses, spawns new
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
 * This file is part of the @ref module_citylife "city life module".
 * See this page for more information.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <vector>
#include <string>

extern "C" {
#include "global.h"
#include "object.h"
#include "sproto.h"
}

/** Module name for the event system. */
#define CITYLIFE_NAME   "citylife"

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
struct mapzone {
    mapzone() : population(0) { };

    std::vector<spawn_point> points;                /**< Points to spawn from when there is a player on the map. */
    std::vector<spawn_zone> zones;                  /**< Zones where to spawn at load time. */
    int population;                                 /**< Maximum of NPCs to add at load time. */
    std::vector<std::string> available_archetypes;  /**< What archetypes can we chose from for an NPC? */
};

/** All defined maps, with the path as key. */
static std::unordered_map<std::string, mapzone *> maps;


/** @ingroup  plugin_citylife
@{*/

/**
 * Finds if a map has a zone defined.
 *
 * @param map
 * candidat map.
 * @return
 * map zone, NULL if not defined.
 */
static const mapzone *get_zone_for_map(mapstruct *map) {
    auto find = maps.find(map->path);
    return find == maps.end() ? nullptr : find->second;
}

/**
 * Creates a NPC for the specified zone, and do needed initialization.
 * @param zone
 * what NPCs to create.
 * @return
 * new NPC, with event handled for time. NULL if invalid archetype in the zone.
 */
static object *get_npc(const mapzone *zone) {
    int idx = RANDOM()%zone->available_archetypes.size();
    archetype *arch = try_find_archetype(zone->available_archetypes[idx].c_str());

    if (!arch) {
        LOG(llevError, CITYLIFE_NAME ": get_npc() invalid archetype %s!\n", zone->available_archetypes[idx].c_str());
        return NULL;
    }

    object *npc = arch_to_object(arch);
    object *evt;


    SET_FLAG(npc, FLAG_RANDOM_MOVE);
    /* Prevent disease spreading in cities, mostly rabies. */
    SET_FLAG(npc, FLAG_UNDEAD);
    /* add a key so NPC will not disappear in the house it came from */
    object_set_value(npc, FIRST_MOVE_KEY, "1", 1);

    evt = create_archetype("event_time");
    evt->slaying = add_string(CITYLIFE_NAME);
    evt->title = add_string(CITYLIFE_NAME);
    object_insert_in_ob(evt, npc);

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
    which = RANDOM() % zone->zones.size();
    if (!object_teleport(npc, map, zone->zones[which].sx+RANDOM()%(zone->zones[which].ex-zone->zones[which].sx), zone->zones[which].sy+RANDOM()%(zone->zones[which].ey-zone->zones[which].sy))) {
        object_free_drop_inventory(npc);
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

    which = RANDOM() % zone->points.size();
    if (!object_teleport(npc, map, zone->points[which].x, zone->points[which].y)) {
        object_free_drop_inventory(npc);
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
    LOG(llevDebug, CITYLIFE_NAME ": adding %d NPC to map %s.\n", add, map->path);

    while (--add >= 0) {
        add_npc_to_zone(zone, map);
    }
}

/**
 * Find a suitable map loaded and add an NPC to it.
 */
static void add_npc_to_random_map(void) {
    int count;
    mapstruct *list[50];
    mapzone *zones[50];
    count = 0;

    for (auto map = maps.cbegin(); map != maps.cend() && count < 50; map++) {
        if ((list[count] = has_been_loaded(map->first.c_str())) && (list[count]->in_memory == MAP_IN_MEMORY)) {
            zones[count] = map->second;
            count++;
        }
    }
    if (!count)
        return;

    int selected = RANDOM() % count;
    add_npc_to_point(zones[selected], list[selected]);
}

static int citylife_globalEventListener(int *type, ...) {
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

static int eventListener(int *type, ...) {
    va_list args;
    object *ground, *who, *event;
    const char *value;

    va_start(args, type);

    who = va_arg(args, object *);
    va_arg(args, object *);
    va_arg(args, object *);
    va_arg(args, char *);
    va_arg(args, int);
    event = va_arg(args, object *);
    va_arg(args, talk_info *);
    va_end(args);

    value = object_get_value(who, FIRST_MOVE_KEY);
    if (!value) {
        /**
         * If the key doesn't exist, it means 'who' wasn't created by us,
         * since we do set the key each time.
         * So play it safe and totally remove the event.
         */
        if (event) {
            LOG(llevInfo, "citylife: removing event from object %s which we didn't generate\n", who->name);
            object_remove(event);
        }
        return 1;
    }
    // Set the flag regardless of whether we tried to move through an exit
    if (strcmp(value, "1") == 0) {
        object_set_value(who, FIRST_MOVE_KEY, "0", 1);

        /* must set inventory as no drop, else it'll just drop on the ground */
        for (object *inv = who->inv; inv; inv = inv->below)
            SET_FLAG(inv, FLAG_NO_DROP);
    }
    /* should our npc disappear? -- Only attempt this if not first move */
    else if (RANDOM()%100 < 30) {
        int16_t sx = who->x, sy = who->y;
        mapstruct *map = who->map;
        if (!(get_map_flags(who->map, &map, who->x, who->y, &sx, &sy) & P_OUT_OF_MAP)) {
            for (ground = GET_MAP_OB(map, sx, sy); ground; ground = ground->above) {
                if (ground->type == EXIT) {
                    object_remove(who);
                    object_free_drop_inventory(who);
                    return 1;
                }
            }
        }
    }

    /* We have to move manually, because during the night NPCs don't move. */
    move_ob(who, 1 + RANDOM() % 8, NULL);

    return 1;
}

/**
 * Check if the zone has valid parameters, LOG() when invalid ones.
 * @param zone zone to check.
 * @param path zone path in case of error.
 */
static void check_zone(const mapzone *zone, const char *oath) {
    if (zone->population == 0) {
        LOG(llevError, "zone for %s has population of 0!\n", oath);
    }
    if (zone->available_archetypes.empty()) {
        LOG(llevError, "zone for %s has no archetype!\n", oath);
    }
    if (zone->zones.empty()) {
        LOG(llevError, "zone for %s has no zone!\n", oath);
    }
    if (zone->points.empty()) {
        LOG(llevError, "zone for %s has no spawn point!\n", oath);
    }
}

/**
 * Read a .citylife file.
 * @param reader file reader.
 * @param filename full file path in case of error.
 */
static void load_citylife(BufferReader *reader, const char *filename) {
    char *line;
    mapzone *zone = nullptr;
    char *split[4];
    std::string path;

    while ((line = bufferreader_next_line(reader)) != NULL) {
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char *space = strchr(line, ' ');
        if (!space) {
            LOG(llevError, "citylife: invalid line in file %s:%ld\n", filename, bufferreader_current_line(reader));
            continue;
        }
        *space = '\0';
        space++;

        if (strcmp(line, "map") == 0) {
            if (zone) {
                check_zone(zone, path.c_str());
            }
            path = space;
            auto existing = maps.find(path);
            if (existing == maps.end()) {
                zone = new mapzone();
                maps[path] = zone;
            } else {
                zone = existing->second;
            }
            continue;
        }
        if (!zone) {
            LOG(llevError, "citylife: error, missing 'map' in file %s:%ld\n", filename, bufferreader_current_line(reader));
            continue;
        }
        if (strcmp(line, "population") == 0) {
            zone->population = atoi(space);
            continue;
        }
        if (strcmp(line, "zone") == 0) {
            size_t found = split_string(space, split, 4, ' ');
            if (found != 4) {
                LOG(llevError, "citylife: 'zone' should have 4 values in file %s:%ld\n", filename, bufferreader_current_line(reader));
            } else {
                spawn_zone z;
                z.sx = atoi(split[0]);
                z.sy = atoi(split[1]);
                z.ex = atoi(split[2]);
                z.ey = atoi(split[3]);
                zone->zones.push_back(z);
            }
            continue;
        }
        if (strcmp(line, "point") == 0) {
            size_t found = split_string(space, split, 2, ' ');
            if (found != 2) {
                LOG(llevError, "citylife: 'point' should have 2 values in file %s:%ld\n", filename, bufferreader_current_line(reader));
            } else {
                spawn_point p;
                p.x = atoi(split[0]);
                p.y = atoi(split[1]);
                zone->points.push_back(p);
            }
            continue;
        }
        if (strcmp(line, "arch") == 0) {
            zone->available_archetypes.push_back(space);
            continue;
        }
        LOG(llevError, "citylife: unknown line %s in file %s:%ld\n", line, filename, bufferreader_current_line(reader));
    }

    if (zone) {
        check_zone(zone, path.c_str());
    }
}

static event_registration c, m;

extern "C"
void citylife_init(Settings *settings) {
    c = events_register_global_handler(EVENT_CLOCK, citylife_globalEventListener);
    m = events_register_global_handler(EVENT_MAPLOAD, citylife_globalEventListener);
    events_register_object_handler(CITYLIFE_NAME, eventListener);

    settings->hooks_filename[settings->hooks_count] = ".citylife";
    settings->hooks [settings->hooks_count] = load_citylife;
    settings->hooks_count++;

    /* Disable the plugin in case it's still there */
    linked_char *disable = static_cast<linked_char *>(calloc(1, sizeof(linked_char)));
    disable->next = settings->disabled_plugins;
    disable->name = strdup("citylife");
    settings->disabled_plugins = disable;
}

extern "C"
void citylife_close() {
    events_unregister_global_handler(EVENT_CLOCK, c);
    events_unregister_global_handler(EVENT_MAPLOAD, m);
    events_unregister_object_handler(CITYLIFE_NAME);
    for (auto map : maps) {
        delete map.second;
    }
    maps.clear();
}

/*@}*/

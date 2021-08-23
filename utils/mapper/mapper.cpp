/*
 * Crossfire map browser generator.
 *
 * Author: Nicolas Weeger <nicolas.weeger@laposte.net>, (C) 2006-2021.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/**
 * @file mapper.cpp
 * This program generates, by default, map browsing web pages (but it can be customized for other things).
 *
 * Quick run: without arguments, will make sensible choices.
 *
 * For help, try the -help option.
 *
 * Since this program browses maps from the first map, only maps linked from there will be processed.
 * A command line parameter, "-addmap=", allows to specify more maps.
 *
 * Maps are generated as the server sees them, that is with weather effects, treasures instead of markers,
 * and things like that.
 *
 * Generated files use "templates", stored (by default) in "templates/" subdirectory.
 * A list of files to process is specified on the command line (by default "index.html"),
 * and from there the program will generate all requested pages.
 *
 * Templates use the inja C++ template engine, found at https://github.com/pantor/inja
 * Provided templates should give much samples of use, see "index.html" for entry point.
 *
 * The following objects exist:
 * - map: identified by a unique key, the following fields are defined:
 *  - _key: unique map key
 *  - name: map name as defined in the map itself
 *  - path: map path from the map root
 *  - region: key of the region this map is part of
 *  - level: map level as defined in the map
 *  - lore: map lore as defined in the map
 *  - exits_to: array of map keys that this map contains exits to
 *  - exits_from: array of map keys that link to this map
 *  - npcs: array of npc objects that are on this map
 *  - readables: array of npc objects that represent books on this map
 *  - monsters: array of monster objects
 *  - quests: array of quest_in_map objects that this map is part of
 * - region: identified by a unique key, the following fields are defined:
 *  - _key: unique region identifier
 *  - name: region short name
 *  - longname: region long name
 *  - description: region description
 *  - maps: array of map keys that are part of the region
 *  - links: array of region keys this region has exits to
 * - item: contains the following fields:
 *  - name: item name as found on the map
 *  - power: item power as found on the map
 *  - calc_power: item power as computed by the game
 *  - diff: item specific fields, as an (CF) object difference with the archetype
 *  - maps: array of map keys the item is found on
 * - monster: contains the following fields:
 *  - name: monster's name
 *  - count: how many are found in the world
 *  - maps: array of map keys the monster is found on
 * - system_quest: information about a quest, with the following fields:
 *  - code: unique quest code
 *  - title: quest title as seen by the player
 *  - description: long description, as seen by the player
 *  - steps: only filled if the command-line parameter "-details-quests" is specified, array of steps:
 *   - description: step description
 *   - is_completion: true if this step completes the quest, false else
 * - slaying: information about a key, detector and such, with the following fields:
 *  - slaying: unique code
 *  - doors: array of map keys that contain a door with this slaying
 *  - keys: array of map keys that contain a key with this slaying
 *  - containers: array of map keys that contain a container with this slaying
 *  - detectors: array of map keys that contain a detector with this slaying
 *  - connections: array of map keys that contain a connection with this slaying
 * - quests: information about a quest as defined in map parameters, with the following fields:
 *  - _key: unique quest key
 *  - number: unique quest number
 *  - name: quest name, may be empty
 *  - description: quest description, may be empty
 *  - main_map: key of the main map of the quest, may be empty
 *  - maps: array of quest_in_map objects
 * - quest_in_map: information about a link between a map and a quest
 *  - map: map key
 *  - description: relationship between the quest and the map
 *  - quest: quest name
 *  - number: unique quest number *
 * - npc: information about a NPC or a readable (book, sign, message...), fields:
 *  - name: NPC or readable name as defined in the map
 *  - x: coordinate in the map
 *  - y: coordinate in the map
 *  - message: NPC or readable message
 * - monster: information about a monster, fields:
 *  - name: monster name
 *  - count: number on the map
 *
 * The following variables are available to the templates:
 * - maps: list of maps
 * - regions: list of regions
 * - items: list of special equipment
 * - monsters: list of monster objects
 * - system_quests: list of system quest objects
 * - slaying: list of slaying information objects
 * - quests: list of quests defined in map objects
 *
 * As well as default callbacks provided by inja, mapper adds the following ones:
 * - link_to_page(page_name[, param]): process the "page_name" template, and return a link
 *   to the file. "param" is an optional string that the template will be able to access via the
 *   "param" variable. Pages with the same "page_name" and "param" are considered equal.
 * - substr(what, start[, length]): return the substring from "start", of a specified length or
 *   the end of the string.
 * - picture(item_key[, size]): return the path to the picture of the specified item. "size" is 1 to 5,
 *   with 1 real size and 5 the smallest size. Only map keys are allowed for now.
 * - pad(val, digits): pad "val" to a string of "length" characters, adding 0 in front if needed.
 * - path_to_root: return the relative path, without final /, to the output root of generated files.
 * - sort(list, key[, invert[, ignore_case]]): sort the specified list by the value of 'key'. If 'invert'
 *   is true then invert order. Strings are compared in a case-unsensitive manner unless 'ignore_case' is false.
 * - get_by_field(list, field, value): return the first item in the list having a field 'field' with value 'value'.
 *
 * For maps, 5 pictures are generated, with sizes of 32, 16, 8, 4 and 2 pixels for tiles.
 *
 * To build this program, add the '--enable-mapper' flag to 'configure' then run
 * 'make' at the server root. It requires the GD library and its development files.
 *
 * @todo
 * - split this file in multiple ones for easier maintenance
 * - add missing documentation on variables / functions
 * - add command line argument for large / small picture size
 * - add maximum width/height for small picture
 * - add slaying information to maps themselves
 * - shop catalog
 * - treasure list use
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

extern "C" {
#include "global.h"
#include "sproto.h"
#include "image.h"
#include "quest.h"

#include <gd.h>
#include <gdfonts.h>
#include <gdfontl.h>
#include <gdfontg.h>
}

#include "inja.hpp"
#include <map>
#include <set>

static gdImagePtr *gdfaces;

/** Information about a NPC with a custom message. */
typedef struct struct_npc_info {
    const char *name;       /**< NPC's name. */
    const char *message;    /**< NPC's message. */
    int x, y;               /**< Coordinates in the map. */
} struct_npc_info;

/** List of NPCs with a custom message. */
typedef struct struct_npc_list {
    struct_npc_info **npc;
    size_t count;
    size_t allocated;
} struct_npc_list;

/** Collection of races. */
typedef struct struct_race_list {
    struct struct_race **races; /**< Races on the list. */
    size_t count;                  /**< Number of races. */
    size_t allocated;              /**< Allocated space. */
} struct_race_list;

/** Utility structure to group map-quest link structure. */
typedef struct {
    struct struct_map_in_quest **list;
    size_t count;
    size_t allocated;
} struct_map_in_quest_list;

/** List of maps. */
typedef struct {
    struct struct_map_info **maps;
    size_t count;
    size_t allocated;
} struct_map_list;

/** Map information. */
typedef struct struct_map_info {
    char *path;                                         /**< Full path of the map from the start directory. */
    char *name;                                         /**< Map's name as defined in the map file. */
    char *filename;                                     /**< Filename of the map. */
    char *lore;
    region *cfregion;
    int level, pic_was_done, max_monster, min_monster;
    struct_map_list exits_from;
    struct_map_list exits_to;
    struct_map_in_quest_list quests;

    struct_map_list tiled_maps;

    struct_race_list monsters;

    struct_npc_list npcs;
    struct_npc_list readable;

    struct struct_map_info *tiled_group;
    int height, width;
    int tiled_x_from, tiled_y_from, processed;
    struct struct_map_info *tiles[4];
} struct_map_info;

/** Maps to process or found. */
static struct_map_list maps_list;

/** Pseudo-maps grouping other maps. */
static struct_map_list tiled_map_list;

/** One special item (weapon, shield, ...). */
typedef struct struct_equipment {
    char *name;             /**< Item's name. */
    int power;              /**< Item power as declared by the item itself. */
    int calc_power;         /**< Item power calculated via calc_item_power(). */
    char *diff;             /**< Result of get_ob_diff() with the item's clone. */
    struct_map_list origin; /**< Map(s) this item is found in. */
} struct_equipment;

static struct_equipment **special_equipment = NULL;     /**< Special equipment list. */

static size_t equipment_count = 0;                      /**< Number of items in special_equipment. */

static size_t equipment_allocated = 0;                  /**< Allocated items in special_equipment. */


/** One monster race in the maps. */
typedef struct struct_race {
    char *name;             /**< Monster's name. */
    int count;              /**< Number found on map. */
    struct_map_list origin; /**< Maps to find said monster. */
} struct_race;

static struct_race_list races;     /**< Monsters found in maps. */

/**
 * Blanks a struct_race_list.
 * @param list
 * list to blank.
 */
static void init_race_list(struct_race_list *list) {
    list->races = NULL;
    list->count = 0;
    list->allocated = 0;
}

/**
 * Appends a race to a race list.
 *
 * @param race
 * race to add.
 * @param list
 * list to add to.
 * @param check
 * if 0, don't check if race is already on the list ; else don't make duplicated entries.
 */
static void add_race_to_list(struct_race *race, struct_race_list *list, int check) {
    if (check) {
        size_t test;

        for (test = 0; test < list->count; test++) {
            if (list->races[test] == race)
                return;
        }
    }

    if (list->allocated == list->count) {
        list->allocated += 50;
        list->races = (struct_race **)realloc(list->races, sizeof(struct_race *)*list->allocated);
    }
    list->races[list->count] = race;
    list->count++;
}

/** Path to store generated files. Relative or absolute, shouldn't end with a / */
static char root[500];

/** Number of created pictures for GD. */
static int pics_allocated;

/* Options */
static int generate_pics = 1;     /**< Whether to generate the picture or not. */
static int force_pics = 0;        /**< To force picture regeneration even if map didn't change. */
static int sizes[] = {32, 16, 8, 4, 2};
static const int num_sizes = sizeof(sizes)/sizeof(int);
#define size_large sizes[0]
#define size_small sizes[1]
static int map_limit = -1;        /**< Maximum number of maps to browse, -1 for all. */
static int show_maps = 0;         /**< If set, will generate much information on map loaded. */
static int world_map = 1;         /**< If set, will generate a world map. */
static int world_exit_info = 1;   /**< If set, will generate a world map of exits. */
static int tileset = 0;           /**< Tileset to use to generate pics. */
static bool detail_quests = false;  /**< Whether to show all quests details or not. */
static bool list_system_quests = false;     /**< Whether to show 'system' quests or not. */
static bool display_rendered_template = false;  /**< Whether to display the template to be rendered or not. */

/** Picture statistics. */
static int created_pics = 0; /**< Total created pics. */
static int cached_pics = 0;  /**< Non recreated pics. */

/** Map output formats. */
enum output_format_type {
    OF_PNG = 0,     /**< PNG, default value. */
    OF_JPG = 1      /**< JPG. */
};

/** Extensions depending on output format. */
static const char *output_extensions[] = {
    ".png",
    ".jpg"
};

/** Selected output format. */
static enum output_format_type output_format = OF_PNG;

/** Quality for jpg pictures. */
static int jpeg_quality = -1;

/** Whether to generate raw pics or instancied ones. */
static int rawmaps = 0;

/** Whether to warn of exits without a path */
static int warn_no_path = 0;

/** Region information. */
typedef struct struct_region_info {
    region *reg;                /**< Region. */
    struct_map_list maps_list;  /**< Maps in the region. */
    int sum_x, sum_y, sum;      /**< Sum of locations, to compute name position. */
    int is_world;               /**< If set, this region has at least one map part of the world, thus region name should be written. */
} struct_region_info;

static struct struct_region_info **regions = NULL; /**< Found regions. */
static size_t region_count = 0;         /**< Count of regions. */
static size_t region_allocated = 0;     /**< Allocated size of regions. */

static int list_unused_maps = 0;       /**< If set, program will list maps found in directory but not linked from the first maps. */
static char **found_maps = NULL;       /**< Maps found in directories. */
static int found_maps_count = 0;       /**< Number of items in found_maps. */
static int found_maps_allocated = 0;   /**< Allocated size of found_maps. */

/* Path/exit info */
static gdImagePtr infomap;         /**< World map with exits / roads / blocking / ... */
static int color_unlinked_exit;    /**< Color for exits without a path set. */
static int color_linked_exit;      /**< Exit leading to another map. */
static int color_road;             /**< Road or equivalent. */
static int color_blocking;         /**< Block all movement. */
static int color_slowing;          /**< Slows movement. */

static int **elevation_info;       /**< All elevation spots in the "world_" maps. */
static int elevation_min;          /**< Maximal elevation found. */
static int elevation_max;          /**< Lowest elevation found. */

/* Whether to compute links between regions or not (expensive). */
static bool do_regions_link = false;
/* Links between regions, key is source, value are destinations. */
static std::map<region *, std::set<region *>> region_links;

/** Connection/slaying information. */
#define S_DOOR      0
#define S_KEY       1
#define S_CONTAINER 2
#define S_DETECTOR  3
#define S_CONNECT   4
#define S_MAX       5

/** slaying information. */
typedef struct {
    char *slaying;          /**< Slaying value. */
    struct_map_list maps[S_MAX];
} struct_slaying_info;

static struct_slaying_info **slaying_info = NULL;  /**< Found slaying fields. */
static size_t slaying_count = 0;                      /**< Count of items in slaying_info. */
static size_t slaying_allocated = 0;                  /**< Allocated size of slaying_info. */

/**
 * Initialises a list structure.
 * @param list
 * list to blank.
 */
static void init_map_list(struct_map_list *list) {
    list->maps = NULL;
    list->count = 0;
    list->allocated = 0;
}

static void add_map(struct_map_info *info, struct_map_list *list);

static int is_special_equipment(object *item) {
    if (item->name == item->arch->clone.name && item->title == item->arch->clone.title)
        return 0;
    if (QUERY_FLAG(item, FLAG_NO_PICK))
        return 0;
    if (item->move_block == MOVE_ALL)
        return 0;

    if (IS_SHIELD(item) || IS_WEAPON(item) || IS_ARMOR(item) || IS_ARROW(item) || (item->type == ROD) || (item->type == WAND) || (item->type == RING) || (item->type == AMULET))
        return 1;

    return 0;
}

/**
 * Gets an empty struct_equipment.
 * @return
 * new item.
 */
static struct_equipment *get_equipment(void) {
    struct_equipment *add = (struct_equipment *)calloc(1, sizeof(struct_equipment));

    init_map_list(&add->origin);
    return add;
}

/**
 * Frees a struct_equipment.
 *
 * @param equip
 * item to free.
 */
static void free_equipment(struct_equipment *equip) {
    free(equip->diff);
    free(equip->name);
    free(equip);
}

/**
 * Searches the item list for an identical item, except maps.
 *
 * @param item
 * item to search. The variable may be freed, so must not be used after calling this function.
 * @return
 * item guaranteed to be unique in the item list.
 */
static struct_equipment *ensure_unique(struct_equipment *item) {
    struct_equipment *comp;

    for (size_t check = 0; check < equipment_count; check++) {
        comp = special_equipment[check];

        if (strcmp(comp->name, item->name))
            continue;
        if (comp->power != item->power)
            continue;
        if (comp->calc_power != item->calc_power)
            continue;
        if (strcmp(comp->diff, item->diff))
            continue;

        free_equipment(item);
        return comp;
    }

    if (equipment_count == equipment_allocated) {
        equipment_allocated += 50;
        special_equipment = (struct_equipment **)realloc(special_equipment, sizeof(struct_equipment *)*equipment_allocated);
    }
    special_equipment[equipment_count] = item;
    equipment_count++;

    return item;
}

/**
 * Adds an item to the list of special items.
 *
 * @param item
 * item to add.
 * @param map
 * map it is on.
 * @todo merge items with the same properties.
 */
static void add_one_item(object *item, struct_map_info *map) {
    struct_equipment *add = get_equipment();
    StringBuffer *bf = stringbuffer_new();
    int x, y;
    sstring name, namepl;
    uint32_t nrof;
    object *base;

    x = item->x;
    y = item->y;
    name = item->name;
    namepl = item->name_pl;
    nrof = item->nrof;

    if (item->artifact != NULL) {
        const artifact *artifact;

        artifact = find_artifact(item, item->artifact);
        if (artifact == NULL) {
            LOG(llevError, "could not find artifact %s [%d] to save data\n", item->artifact, item->type);
            base = arch_to_object(item->arch);
        } else {
            base = arch_to_object(item->arch);
            give_artifact_abilities(base, artifact->item);
        }
    }
    else {
        base = arch_to_object(item->arch);
    }

    item->x = base->x;
    item->y = base->y;
    item->name = base->name;
    item->name_pl = base->name_pl;
    item->nrof = base->nrof;

    if (QUERY_FLAG(item, FLAG_IDENTIFIED) && !QUERY_FLAG(base, FLAG_IDENTIFIED)) {
        object_give_identified_properties(base);
        SET_FLAG(base, FLAG_IDENTIFIED);
    }
    if (QUERY_FLAG(item, FLAG_UNPAID))
        SET_FLAG(base, FLAG_UNPAID);
    get_ob_diff(bf, item, base);
    object_free(base, FREE_OBJ_NO_DESTROY_CALLBACK | FREE_OBJ_FREE_INVENTORY);

    add->diff = stringbuffer_finish(bf);

    item->x = x;
    item->y = y;
    item->name = name;
    item->name_pl = namepl;
    item->nrof = nrof;

    if (add->diff == NULL || strcmp(add->diff, "") == 0) {
        free_equipment(add);
        return;
    }

    add->name = strdup(item->name);
    add->power = item->item_power;
    add->calc_power = calc_item_power(item);

    add = ensure_unique(add);
    add_map(map, &add->origin);
}

/**
 * Checks if item and its inventory are worthy to be listed.
 *
 * @param item
 * item to check.
 * @param map
 * map the item is on.
 */
static void check_equipment(object *item, struct_map_info *map) {
    if (is_special_equipment(item))
        add_one_item(item, map);

    FOR_INV_PREPARE(item, inv)
        check_equipment(inv, map);
    FOR_INV_FINISH();
}

/**
 * Sort 2 struct_equipment, first on item power then name.
 * @param a
 * @param b
 * items to compare.
 * @return
 * -1, 0 or 1.
 */
static int sort_equipment(const void *a, const void *b) {
    const struct_equipment *l = *(const struct_equipment **)a;
    const struct_equipment *r = *(const struct_equipment **)b;
    int c = l->power-r->power;

    if (c)
        return c;
    return strcasecmp(l->name, r->name);
}

/**
 * Returns the race for specified name.
 *
 * @param name
 * monster's name.
 * @return
 * race structure.
 */
static struct_race *get_race(const char *name) {
    size_t test;
    struct_race *item;

    for (test = 0; test < races.count; test++) {
        if (strcmp(races.races[test]->name, name) == 0) {
            races.races[test]->count++;
            return races.races[test];
        }
    }

    item = (struct_race *)calloc(1, sizeof(struct_race));
    item->name = strdup(name);
    item->count = 1;
    init_map_list(&item->origin);

    add_race_to_list(item, &races, 0);

    return item;
}

/**
 * Adds a monster to the monster list.
 *
 * @param monster
 * monster to add. Can be any part.
 * @param map
 * map to add the monster to.
 */
static void add_monster(object *monster, struct_map_info *map) {
    struct_race *race;

    if (monster->head && monster != monster->head)
        return;

    map->min_monster = MIN(monster->level, map->min_monster);
    map->max_monster = MAX(monster->level, map->max_monster);

    race = get_race(monster->name);
    add_map(map, &race->origin);
    add_race_to_list(race, &map->monsters, 1);
}

/**
 * Sort 2 struct_race.
 * @param a
 * @param b
 * items to compare.
 * @return
 * -1, 0 or 1.
 */
static int sort_race(const void *a, const void *b) {
    const struct_race *l = *(const struct_race **)a;
    const struct_race *r = *(const struct_race **)b;
    return strcasecmp(l->name, r->name);
}

/**
 * Checks if ::object is considered a road or not.
 * @param item
 * ::object to check.
 * @return
 * 1 if object is a road, 0 else.
 */
static int is_road(object *item) {
    int test;
    /* Archetypes used as roads. */
    const char *roads[] = {
        "cobblestones",
        "flagstone",
        "ice_stone",
        "snow",
        NULL };
    const char *partial[] = {
        "dirtroad_",
        NULL };

    for (test = 0; partial[test] != NULL; test++) {
        if (strstr(item->arch->name, partial[test]) != NULL)
            return 1;
    }

    if (!QUERY_FLAG(item, FLAG_IS_FLOOR))
        return 0;

    for (test = 0; roads[test] != NULL; test++) {
        if (strcmp(item->arch->name, roads[test]) == 0)
            return 1;
    }

    return 0;
}

/**
 * Checks if item blocks movement or not.
 * @param item
 * ::object to test.
 * @return
 * 1 if item blocks all movement, 0 else.
 */
static int is_blocking(object *item) {
    return item->move_block == MOVE_ALL ? 1 : 0;
}

/**
 * Gets the color for an elevation.
 *
 * @param elevation
 * elevation to get color for.
 * @param elevationmap
 * picture that will get the color.
 * @return
 * color.
 */
static int get_elevation_color(int elevation, gdImagePtr elevationmap) {
    if (elevation > 0)
        return gdImageColorResolve(elevationmap, 200*elevation/elevation_max, 0, 0);
    else
        return gdImageColorResolve(elevationmap, 0, 0, 200*elevation/elevation_min);
}

/**
 * Proceses exit / road / blocking information for specified map into the global infomap map.
 *
 * If map isn't a world map, won't do anything.
 *
 * @param map
 * map to write info for.
 */
static void do_exit_map(mapstruct *map) {
    int tx, ty, x, y;
    object *test;
    sstring selevation;

    if (sscanf(map->path, "/world/world_%d_%d", &x, &y) != 2)
        return;

    x -= 100;
    y -= 100;

    for (tx = 0; tx < MAP_WIDTH(map); tx++) {
        for (ty = 0; ty < MAP_HEIGHT(map); ty++) {
            FOR_MAP_PREPARE(map, tx, ty, item) {
                test = HEAD(item);

                if (test->type == EXIT || test->type == TELEPORTER) {
                    if (!test->slaying)
                        gdImageSetPixel(infomap, x*50+tx, y*50+ty, color_unlinked_exit);
                    else
                        gdImageSetPixel(infomap, x*50+tx, y*50+ty, color_linked_exit);
                } else if (is_road(test))
                    gdImageSetPixel(infomap, x*50+tx, y*50+ty, color_road);
                else if (is_blocking(test)) {
                    gdImageSetPixel(infomap, x*50+tx, y*50+ty, color_blocking);
                    /* can't get on the spot, so no need to go on. */
                    break;
                } else if (test->move_slow != 0)
                    gdImageSetPixel(infomap, x*50+tx, y*50+ty, color_slowing);

                selevation = object_get_value(item, "elevation");
                if (selevation) {
                    int32_t elevation = atoi(selevation);
                    elevation_min = MIN(elevation_min, elevation);
                    elevation_max = MAX(elevation_max, elevation);
                    elevation_info[x*50+tx][y*50+ty] = elevation;
                }
            } FOR_MAP_FINISH();
        }
    }
}

void do_auto_apply(mapstruct *m);

/**
 * Sort values alphabetically
 * Used by qsort to sort values alphabetically.
 * @param a
 * First value
 * @param b
 * Second value
 * @return
 * -1 if a is less than b, 0 if a equals b, 1 else.
 */
static int sortbyname(const void *a, const void *b) {
    return (strcmp(*(const char **)a, *(const char **)b));
}

/**
 * Computes the shortest path from one file to another.
 *
 * @param from
 * origin.
 * @param to
 * destination.
 * @param result
 * string that will contain the calculated path. Must be large enough, no test done.
 * @warning
 * from and to must be absolute paths (starting with /).
 */
static void relative_path(const char *from, const char *to, char *result) {
    const char *fslash;
    const char *rslash;

    result[0] = '\0';

    fslash = strchr(from+1, '/');
    if (!fslash) {
        strcpy(result, to+1);
        return;
    }

    rslash = strchr(to+1, '/');
    while (fslash && rslash && (fslash-from == rslash-to) && strncmp(from, to, fslash-from+1) == 0) {
        from = fslash+1;
        to = rslash+1;
        fslash = strchr(fslash+1, '/');
        rslash = strchr(rslash+1, '/');
    }

    while (fslash) {
        strcat(result, "../");
        fslash = strchr(fslash+1, '/');
    }
    if (strlen(result) && result[strlen(result)-1] == '/' && *to == '/')
        result[strlen(result)-1] = '\0';
    strcat(result, to);
}

/**
 * Compares struct_map_info according to the map name or the path if equal.
 *
 * @param left
 * first item.
 * @param right
 * second item.
 * @return
 * comparison on name, and if equal then on whole path.
 */
static int compare_map_info(const struct_map_info *left, const struct_map_info *right) {
    int c;

    if (left->tiled_group)
        left = left->tiled_group;
    if (right->tiled_group)
        right = right->tiled_group;

    c = strcasecmp(left->name, right->name);
    if (c)
        return c;

    return strcasecmp(left->path, right->path);
}

/**
 * Sorts the struct_map_info according to the map name or the path if equal.
 *
 * @param left
 * first item.
 * @param right
 * second item.
 * @return
 * comparison on name, and if equal then on whole path.
 */
static int sort_map_info(const void *left, const void *right) {
    const struct_map_info *l = *(const struct_map_info **)left;
    const struct_map_info *r = *(const struct_map_info **)right;
    return compare_map_info(l, r);
}

/**
 * Sorts an array of struct_region_info by region name.
 *
 * @param left
 * first region.
 * @param right
 * second region.
 * @return
 * result of strcmp() for names.
 */
static int sort_region(const void *left, const void *right) {
    return strcmp((*((struct_region_info **)left))->reg->name, (*((struct_region_info **)right))->reg->name);
}

/************************************
 Start of quest-related definitions.
************************************/

/** Link between a quest and a map. */
typedef struct struct_map_in_quest {
    struct_map_info *map;   /**< Linked map. */
    char *description;      /**< Description associated with the map for the quest. */
    struct struct_quest *quest;    /**< Point back to the quest. */
} struct_map_in_quest;

/** One quest. */
typedef struct struct_quest {
    char *name;                     /**< Quest's name. */
    char *description;              /**< Description, from the main map's lore. */
    int number;                     /**< Unique quest identifier. */
    struct_map_info *mainmap;       /**< Map defining the quest. Can be NULL if quest has no definition or map not processed. */
    struct_map_in_quest_list maps;  /**< Maps part of this quest. */
} struct_quest;

static struct_quest **quests = NULL;   /**< All quests in the game. */

static int quests_count = 0;           /**< Count of quests. */

static int quests_allocated = 0;       /**< Allocated items in quests. */

static void init_struct_map_in_quest_list(struct_map_in_quest_list *list) {
    list->list = NULL;
    list->count = 0;
    list->allocated = 0;
}

static void add_to_struct_map_in_quest_list(struct_map_in_quest_list *list, struct_map_in_quest *item) {
    if (list->count == list->allocated) {
        list->allocated += 10;
        list->list = (struct_map_in_quest **)realloc(list->list, sizeof(struct_map_in_quest *)*list->allocated);
    }
    list->list[list->count++] = item;
}

/**
 * Gets the information for a quest if it exists.
 * @param name quest's name.
 * @return quest information, NULL if no match.
 */
static struct_quest *find_quest_info(const char *name) {
    int test;
    for (test = 0; test < quests_count; test++) {
        if (strcmp(quests[test]->name, name) == 0)
            return quests[test];
    }
    return NULL;
}

/**
 * Gets the information for a quest, create the field if needed.
 *
 * @param name
 * quest's name.
 * @return
 * information, never NULL.
 */
static struct_quest *get_quest_info(const char *name) {
    struct_quest *add = find_quest_info(name);
    if (add) {
        return add;
    }

    if (quests_count == quests_allocated) {
        quests_allocated += 10;
        quests = (struct_quest **)realloc(quests, sizeof(struct_quest *)*quests_allocated);
    }
    add = (struct_quest *)calloc(1, sizeof(struct_quest));
    add->name = strdup(name);
    add->number = quests_count;
    init_struct_map_in_quest_list(&add->maps);
    quests[quests_count] = add;
    quests_count++;
    return add;
}

/**
 * Links a map to a quest.
 *
 * @param map
 * map to link.
 * @param name
 * quest name.
 * @param description
 * associated link description. Must not be NULL.
 */
static void add_map_to_quest(struct_map_info *map, const char *name, const char *description) {
    struct_map_in_quest *add;
    struct_quest *quest = get_quest_info(name);

    add = (struct_map_in_quest *)calloc(1, sizeof(struct_map_in_quest));
    add->map = map;
    add->quest = quest;
    add->description = strdup(description);
    while (strlen(add->description) && add->description[strlen(add->description)-1] == '\n')
        add->description[strlen(add->description)-1] = '\0';
    add_to_struct_map_in_quest_list(&quest->maps, add);
    add_to_struct_map_in_quest_list(&map->quests, add);
}

/**
 * Sorts 2 struct_map_in_quest, on the map's name or path.
 * @param left
 * @param right
 * items to compare.
 * @return
 * -1, 0 or 1.
 */
static int sort_struct_map_in_quest(const void *left, const void *right) {
    int c;

    const struct_map_in_quest *l = *(const struct_map_in_quest **)left;
    const struct_map_in_quest *r = *(const struct_map_in_quest **)right;
    const struct_map_info *ml = l->map;
    const struct_map_info *mr = r->map;

    if (ml->tiled_group)
        ml = ml->tiled_group;
    if (mr->tiled_group)
        mr = mr->tiled_group;

    c = strcasecmp(ml->name, mr->name);
    if (c)
        return c;

    return strcasecmp(ml->path, mr->path);
}

/**
 * Sorts 2 struct_quest, on the map's name or path.
 * @param left
 * @param right
 * items to compare.
 * @return
 * -1, 0 or 1.
 */
static int sort_struct_quest(const void *left, const void *right) {
    const struct_quest *l = *(const struct_quest **)left;
    const struct_quest *r = *(const struct_quest **)right;
    return strcasecmp(l->name, r->name);
}

/**
 * Sets the main map for a quest.
 *
 * @param name
 * quest name.
 * @param mainmap
 * main map to associate.
 * @param description
 * quest description. Must not be NULL.
 */
static void define_quest(const char *name, struct_map_info *mainmap, const char *description) {
    struct_quest *quest = get_quest_info(name);

    if (quest->description || quest->mainmap) {
        printf("warning, multiple quest definition for %s, found in %s and %s.\n", quest->name, quest->mainmap ? quest->mainmap->path : "(unknown map)", mainmap->path);
        return;
    }
    quest->description = strdup(description);
    while (strlen(quest->description) && quest->description[strlen(quest->description)-1] == '\n')
        quest->description[strlen(quest->description)-1] = '\0';
    quest->mainmap = mainmap;
}

/**
 * Extracts from the map's lore quest information if found. May modify map->lore.
 *
 * @param map
 * map to process.
 */
static void process_map_lore(struct_map_info *map) {
    char *start, *end, *next;
    char name[500];
    char description[500];

    start = strstr(map->lore, "@def");
    while (start) {
        description[0] = '\0';
        /* find name */
        end = strstr(start, "\n");
        if (end) {
            strncpy(name, start+5, end-start-5);
            name[end-start-5] = '\0';
            next = end+1;
            end = strstr(next, "@end");
            if (end) {
                strncpy(description, next, end-next);
                description[end-next] = '\0';
                /* need to erase the text. */
                memcpy(start, end+4, strlen(map->lore)-(end-start+3));
                end = start;
            }
            else {
                strcpy(description, next);
                *start = '\0';
                end = NULL;
            }
        } else {
            strcpy(name, start);
            *start = '\0';
            end = NULL;
        }

        define_quest(name, map, description);
        start = end ? strstr(end, "@def") : NULL;
    }

    start = strstr(map->lore, "@quest");
    while (start) {
        description[0] = '\0';
        /* find name */
        end = strstr(start, "\n");
        if (end) {
            strncpy(name, start+7, end-start-7);
            name[end-start-7] = '\0';
            next = end+1;
            end = strstr(next, "@end");
            if (end) {
                strncpy(description, next, end-next);
                description[end-next] = '\0';
                /* need to erase the text. */
                memcpy(start, end+4, strlen(map->lore)-(end-start+3));
                end = start;
            }
            else {
                strcpy(description, next);
                *start = '\0';
                end = NULL;
            }
        } else {
            strcpy(name, start);
            *start = '\0';
            end = NULL;
        }

        add_map_to_quest(map, name, description);
        start = end ? strstr(end, "@quest") : NULL;
    }
}

/************************************
 End of quest-related definitions.
************************************/

/*********
NPC-related stuff
********/

/**
 * Initialise a list of NPCs.
 * @param list
 * list to initialise.
 */
static void init_npc_list(struct_npc_list *list) {
    list->allocated = 0;
    list->count = 0;
    list->npc = NULL;
}

/**
 * Create the struct_npc_info from the specified NPC. It must have a name and message.
 * @param npc
 * NPC to gather info for.
 * @return
 * structure with info.
 */
static struct_npc_info *create_npc_info(const object *npc) {
    struct_npc_info *info = (struct_npc_info *)calloc(1, sizeof(struct_npc_info));

    info->name = strdup(npc->name);
    info->message = strdup(npc->msg);
    info->x = npc->x;
    info->y = npc->y;

    return info;
}

/**
 * Add the specified NPC to the list.
 * @param list
 * where to add the NPC.
 * @param npc
 * NPC to add. Must have a name and message.
 */
static void add_npc_to_map(struct_npc_list *list, const object *npc) {
    if (list->count == list->allocated) {
        list->allocated += 50;
        list->npc = (struct_npc_info **)realloc(list->npc, list->allocated*sizeof(struct_npc_info *));
    }

    list->npc[list->count] = create_npc_info(npc);
    list->count++;
}
/* end of NPC stuff */

/**
 * Adds a map to specified array, if it isn't already.
 *
 * @param info
 * map to add.
 * @param list
 * list to add to.
 *
 * @note
 * will allocate memory and update variables when required.
 */
static void add_map(struct_map_info *info, struct_map_list *list) {
    size_t map;

    for (map = 0; map < list->count; map++)
        if (list->maps[map] == info)
            return;

    if (list->count == list->allocated) {
        list->allocated += 50;
        list->maps = (struct_map_info **)realloc(list->maps, list->allocated*sizeof(struct_map_info *));
    }
    list->maps[list->count] = info;
    list->count++;
}

/**
 * Returns an initialised struct_map_info.
 *
 * @return
 * new struct_map_info.
 */
static struct_map_info *create_map_info(void) {
    struct_map_info *add = (struct_map_info *)calloc(1, sizeof(struct_map_info));

    add->min_monster = 2000;
    init_map_list(&add->exits_to);
    init_map_list(&add->exits_from);
    init_map_list(&add->tiled_maps);
    init_struct_map_in_quest_list(&add->quests);
    init_race_list(&add->monsters);
    init_npc_list(&add->npcs);
    init_npc_list(&add->readable);
    add->tiled_group = NULL;

    return add;
}

/**
 * Create a new tiled map and link it to the tiled map list.
 *
 * @return
 * new tiled map.
 */
static struct_map_info *create_tiled_map(void) {
    struct_map_info *add = create_map_info();

    add_map(add, &tiled_map_list);
    return add;
}

/**
 * Merge two tiled maps groups. This can happen if based on processing we do one map with tiled maps,
 * another with tiled maps, and later figure out the tiles are actually linked.
 *
 * @param map
 * the map that being processed has a tiling to a map in another group. Its group will be the final merging group.
 * @param tiled_map
 * the map tiled to another group. Its group will disappear.
 */
static void merge_tiled_maps(struct_map_info *map, struct_map_info *tiled_map) {
    size_t g;
    struct_map_info *group = tiled_map->tiled_group;
    struct_map_info *change;

    while (group->tiled_maps.count > 0) {
        change = group->tiled_maps.maps[group->tiled_maps.count-1];
        change->tiled_group = map->tiled_group;
        add_map(change, &map->tiled_group->tiled_maps);
        group->tiled_maps.count--;
    }

    for (g = 0; g < tiled_map_list.count; g++) {
        if (tiled_map_list.maps[g] == group) {
            if (g < tiled_map_list.count-1)
                tiled_map_list.maps[g] = tiled_map_list.maps[tiled_map_list.count-1];
            tiled_map_list.count--;
            free(group);
            return;
        }
    }
    printf("tiled_map not in tiled_map_list!");
    abort();

}

/**
 * Gets or creates if required the info structure for a map.
 *
 * @param path
 * map to consider.
 * @return
 * associated structure.
 */
static struct_map_info *get_map_info(const char *path) {
    struct_map_info *add;
    char *tmp;

    for (size_t map = 0; map < maps_list.count; map++) {
        if (strcmp(maps_list.maps[map]->path, path) == 0)
            return maps_list.maps[map];
    }

    add = create_map_info();
    add->path = strdup(path);
    tmp = strrchr((char *)path, '/');
    if (tmp)
        add->filename = strdup(tmp+1);
    else
        add->filename = strdup(path);

    add_map(add, &maps_list);
    return add;
}

/**
 * Marks specified path as processed.
 *
 * @param path
 * map to remove.
 */
static void list_map(const char *path) {
    int index;

    for (index = 0; index < found_maps_count; index++) {
        if (found_maps[index] && strcmp(path, found_maps[index]) == 0) {
            free(found_maps[index]);
            found_maps[index] = NULL;
            return;
        }
    }
    printf("Map processed but not found in directory reading? %s\n", path);
}

/**
 * Links a map to a region.
 *
 * Will not readd the map if already linked.
 *
 * @param map
 * map name.
 * @param reg
 * region to link the map to.
 */
static void add_map_to_region(struct_map_info *map, region *reg) {
    size_t test;
    int x, y;

    for (test = 0; test < region_count; test++) {
        if (regions[test]->reg == reg)
            break;
    }
    if (test == region_count) {
        if (test == region_allocated) {
            region_allocated++;
            regions = (struct_region_info **)realloc(regions, sizeof(struct_region_info *)*region_allocated);
            regions[test] = (struct_region_info *)calloc(1, sizeof(struct_region_info));
        }
        region_count++;
        regions[test]->reg = reg;
    }
    add_map(map, &regions[test]->maps_list);
    if (sscanf(map->path, "/world/world_%d_%d", &x, &y) == 2) {
        regions[test]->sum_x += (x-100);
        regions[test]->sum_y += (y-100);
        regions[test]->sum++;
        regions[test]->is_world = 1;
    }
}

/**
 * Saves a map to a file, based on jpg/png settings.
 *
 * @param file
 * opened file to which to save.
 * @param pic
 * picture to save.
 */
static void save_picture(FILE *file, gdImagePtr pic) {
    if (output_format == OF_PNG)
        gdImagePng(pic, file);
    else
        gdImageJpeg(pic, file, jpeg_quality);
}

/**
 * Creates a link between two maps if they are on different regions.
 * @param source
 * map from.
 * @param dest
 * map to.
 * @param linkname
 * name of the link as it should appear. Unused.
 */
static void add_region_link(mapstruct *source, mapstruct *dest, const char */*linkname*/) {
    region *s, *d;

    s = get_region_by_map(source);
    d = get_region_by_map(dest);
    if (s == d)
        return;

    region_links[s].insert(d);
}

/**
 * Is the slaying field relevant for this item?
 *
 * @param item
 * item to check.
 * @return
 * 1 if relevant, 0 else.
 */
static int is_slaying(object *item) {
    return (item->type == LOCKED_DOOR || item->type == SPECIAL_KEY || item->type == CONTAINER || item->type == CHECK_INV);
}


/**
 * Returns a struct_slaying_info for specified slaying. Creates a new one if required.
 *
 * @param slaying
 * value to get the structure of.
 * @return
 * structure for slaying. Never NULL.
 */
static struct_slaying_info *get_slaying_struct(const char *slaying) {
    for (size_t l = 0; l < slaying_count; l++) {
        if (!strcmp(slaying_info[l]->slaying, slaying))
            return slaying_info[l];
    }
    if (slaying_count == slaying_allocated) {
        slaying_allocated += 10;
        slaying_info = (struct_slaying_info **)realloc(slaying_info, sizeof(struct_slaying_info *)*slaying_allocated);
    }

    struct_slaying_info *add = (struct_slaying_info *)calloc(1, sizeof(struct_slaying_info));
    add->slaying = strdup(slaying);
    for (size_t l = 0; l < S_MAX; l++)
        init_map_list(&add->maps[l]);

    slaying_info[slaying_count] = add;
    slaying_count++;

    return add;
}

/**
 * Adds the specified map to the slaying information if not already present.
 *
 * @param info
 * structure to add to.
 * @param item
 * one of the S_xxx values specifying what type of slaying this is.
 * @param map
 * map to add.
 */
static void add_map_to_slaying(struct_slaying_info *info, int item, struct_map_info *map) {
    add_map(map, &info->maps[item]);
}

/**
 * Adds the item's information to the map.
 *
 * @param map
 * map containing the item.
 * @param item
 * item which slaying field we're considering.
 */
static void add_slaying(struct_map_info *map, object *item) {
    struct_slaying_info *info;

    if (!item->slaying)
        /* can be undefined */
        return;

    info = get_slaying_struct(item->slaying);
    if (item->type == LOCKED_DOOR)
        add_map_to_slaying(info, S_DOOR, map);
    else if (item->type == SPECIAL_KEY)
        add_map_to_slaying(info, S_KEY, map);
    else if (item->type == CONTAINER)
        add_map_to_slaying(info, S_CONTAINER, map);
    else if (item->type == DETECTOR)
        add_map_to_slaying(info, S_DETECTOR, map);
    else
        add_map_to_slaying(info, S_CONNECT, map);
}

/**
 * Recursively checks if the object should be considered for slaying information.
 *
 * @param map
 * map containing the items.
 * @param item
 * item to consider. Must not be NULL.
 */
static void check_slaying_inventory(struct_map_info *map, object *item) {
    FOR_INV_PREPARE(item, inv) {
        if (is_slaying(inv))
            add_slaying(map, inv);
        check_slaying_inventory(map, inv);
    } FOR_INV_FINISH();
}

static void generate_picture_path(const char *path, size_t pic_size, char *out, size_t len) {
    snprintf(out, len, "%s%s.x%zu%s", root, path, pic_size + 1, output_extensions[output_format]);
}

static void write_pictures_from_real_size(const char *path, gdImagePtr real, int width, int height) {
    char picpath[MAX_BUF];

    generate_picture_path(path, 0, picpath, sizeof(picpath));
    make_path_to_file(picpath);
    FILE *out = fopen(picpath, "wb+");
    save_picture(out, real);
    fclose(out);

    for (size_t i = 1; i < num_sizes; i++) {
        generate_picture_path(path, i, picpath, sizeof(picpath));
        gdImagePtr small = gdImageCreateTrueColor(width*sizes[i], height*sizes[i]);
        gdImageCopyResampled(small, real, 0, 0, 0, 0, width*sizes[i], height*sizes[i], width*size_large, height*size_large);
        out = fopen(picpath, "wb+");
        save_picture(out, small);
        fclose(out);
        gdImageDestroy(small);
    }
}

/**
 * Processes a map.
 *
 * Generates the map pictures (big and small), and exit information.
 *
 * @param info
 * map to process.
 */
static void process_map(struct_map_info *info) {
    mapstruct *m;
    int x, y, isworld;
    gdImagePtr pic = nullptr;
    struct stat stats;
    struct stat statspic;
    char exit_path[500];
    char tmppath[MAX_BUF];
    char picpath[num_sizes][MAX_BUF];
    int needpic = 0;
    struct_map_info *link;

    if (list_unused_maps)
        list_map(info->path);

    if (show_maps)
        printf(" processing map %s\n", info->path);

    m = ready_map_name(info->path, 0);
    if (!m) {
        printf("couldn't load map %s\n", info->path);
        return;
    }

    do_exit_map(m);

    if (!rawmaps)
        do_auto_apply(m);

    info->level = m->difficulty;
    if (m->maplore) {
        info->lore = strdup(m->maplore);
        process_map_lore(info);
    }

    isworld = (sscanf(info->path, "/world/world_%d_%d", &x, &y) == 2);

    if (m->name)
        info->name = strdup(m->name);
    else
        info->name = strdup(info->filename);

    info->cfregion = get_region_by_map(m);
    add_map_to_region(info, info->cfregion);

    for (int i = 0; i < num_sizes; i++) {
        generate_picture_path(info->path, i, picpath[i], sizeof(picpath[i]));
    }

    if (force_pics)
        needpic = 1;
    else if (generate_pics) {
        create_pathname(info->path, tmppath, MAX_BUF);
        stat(tmppath, &stats);
        if (stat(picpath[0], &statspic) || (statspic.st_mtime < stats.st_mtime))
            needpic = 1;
    }
    else
        needpic = 0;

    if (needpic) {
        pic = gdImageCreateTrueColor(MAP_WIDTH(m)*size_large, MAP_HEIGHT(m)*size_large);
        created_pics++;
    }
    else
        cached_pics++;

    for (x = 0; x < 4; x++)
        if (m->tile_path[x] != NULL) {
            path_combine_and_normalize(m->path, m->tile_path[x], exit_path, sizeof(exit_path));
            create_pathname(exit_path, tmppath, MAX_BUF);
            if (stat(tmppath, &stats)) {
                printf("  map %s doesn't exist in map %s, for tile %d.\n", exit_path, info->path, x);
            }

            if (isworld) {
                link = get_map_info(exit_path);
                add_map(link, &info->exits_from);
                add_map(info, &link->exits_to);

                if (do_regions_link) {
                    mapstruct *link = ready_map_name(exit_path, 0);

                    if (link && link != m) {
                        /* no need to link a map with itself. Also, if the exit points to the same map, we don't
                        * want to reset it. */
                        add_region_link(m, link, NULL);
                        link->reset_time = 1;
                        link->in_memory = MAP_IN_MEMORY;
                        delete_map(link);
                    }
                }
            } else {
                link = get_map_info(exit_path);
                info->tiles[x] = link;
                if (link->tiled_group) {
                    if (info->tiled_group && link->tiled_group != info->tiled_group) {
                        merge_tiled_maps(info, link);
                        continue;
                    }
                    if (link->tiled_group == info->tiled_group) {
                        continue;
                    }
                    if (!info->tiled_group) {
                        add_map(info, &link->tiled_group->tiled_maps);
                        continue;
                    }
                }

                if (!info->tiled_group) {
                    info->tiled_group = create_tiled_map();
                    add_map(info, &info->tiled_group->tiled_maps);
                }
                link->tiled_group = info->tiled_group;
                add_map(link, &info->tiled_group->tiled_maps);
            }
        }

    info->width = MAP_WIDTH(m);
    info->height = MAP_HEIGHT(m);

    for (x = MAP_WIDTH(m)-1; x >= 0; x--)
        for (y = MAP_HEIGHT(m)-1; y >= 0; y--) {
            FOR_MAP_PREPARE(m, x, y, item) {
                if (item->type == EXIT || item->type == TELEPORTER || item->type == PLAYER_CHANGER) {
                    char ep[500];
                    const char *start;

                    if (!item->slaying) {
                        ep[0] = '\0';
                        if (warn_no_path)
                            printf(" exit without any path at %d, %d on %s\n", item->x, item->y, info->path);
                    } else {
                        memset(ep, 0, 500);
                        if (strcmp(item->slaying, "/!"))
                            strcpy(ep, EXIT_PATH(item));
                        else {
                            if (!item->msg) {
                                printf("  random map without message in %s at %d, %d\n", info->path, item->x, item->y);
                            } else {
                                /* Some maps have a 'exit_on_final_map' flag, ignore it. */
                                start = strstr(item->msg, "\nfinal_map ");
                                if (!start && strncmp(item->msg, "final_map", strlen("final_map")) == 0)
                                    /* Message start is final_map, nice */
                                    start = item->msg;
                                if (start) {
                                    char *end = strchr((char *)start+1, '\n');

                                    start += strlen("final_map")+2;
                                    strncpy(ep, start, end-start);
                                }
                            }
                        }

                        if (strlen(ep)) {
                            path_combine_and_normalize(m->path, ep, exit_path, 500);
                            create_pathname(exit_path, tmppath, MAX_BUF);
                            if (stat(tmppath, &stats)) {
                                printf("  map %s doesn't exist in map %s, at %d, %d.\n", ep, info->path, item->x, item->y);
                            } else {
                                link = get_map_info(exit_path);
                                add_map(link, &info->exits_from);
                                add_map(info, &link->exits_to);

                                if (do_regions_link) {
                                    mapstruct *link = ready_map_name(exit_path, 0);

                                    if (link && link != m) {
                                        /* no need to link a map with itself. Also, if the exit points to the same map, we don't
                                         * want to reset it. */
                                        add_region_link(m, link, item->arch->clone.name);
                                        link->reset_time = 1;
                                        link->in_memory = MAP_IN_MEMORY;
                                        delete_map(link);
                                    }
                                }
                            }
                        }
                    }
                } else if (is_slaying(item))
                    add_slaying(info, item);

                check_equipment(item, info);

                check_slaying_inventory(info, item);

                if (QUERY_FLAG(item, FLAG_MONSTER)) {
                    /* need to get the "real" archetype, as the item's archetype can certainly be a temporary one. */
                    archetype *arch = find_archetype(item->arch->name);

                    add_monster(item, info);
                    if (arch != NULL && (QUERY_FLAG(item, FLAG_UNAGGRESSIVE) || QUERY_FLAG(item, FLAG_FRIENDLY)) && (item->msg != arch->clone.msg) && (item->msg != NULL))
                        add_npc_to_map(&info->npcs, item);
                } else if ((item->type == SIGN || item->type == BOOK) && (item->msg != item->arch->clone.msg) && (item->msg != NULL)) {
                    add_npc_to_map(&info->readable, item);
                }

                if (item->invisible)
                    continue;

                if (needpic) {
                    int sx, sy, hx, hy;

                    if (gdfaces[item->face->number] == NULL) {
                        face_sets *fs = find_faceset(get_face_fallback(tileset, item->face->number));

                        gdfaces[item->face->number] = gdImageCreateFromPngPtr(fs->faces[item->face->number].datalen, fs->faces[item->face->number].data);
                        pics_allocated++;
                    }
                    if (item->head || item->more) {
                        object_get_multi_size(item, &sx, &sy, &hx, &hy);
                    } else {
                        hx = 0;
                        hy = 0;
                    }
                    if (gdfaces[item->face->number] != NULL && ((!item->head && !item->more) || (item->arch->clone.x+hx == 0 && item->arch->clone.y+hy == 0))) {
                        gdImageCopy(pic, gdfaces[item->face->number], x*size_large, y*size_large, 0, 0, gdfaces[item->face->number]->sx, gdfaces[item->face->number]->sy);
                    }
                }
            } FOR_MAP_FINISH();
        }

    if (needpic) {
        write_pictures_from_real_size(info->path, pic, MAP_WIDTH(m), MAP_HEIGHT(m));
        gdImageDestroy(pic);
        info->pic_was_done = 1;
    }

    m->reset_time = 1;
    m->in_memory = MAP_IN_MEMORY;
    delete_map(m);
}

/**
 * Generates a big world map.
 */
static void write_world_map(void) {
#define SIZE 50
    int x, y;
    FILE *out;
    int wx, wy;
    char file[500];
    char mapleft[10], maptop[10], mapright[10], mapbottom[10], mappath[5000];
    char name[100];
    gdImagePtr pic;
    gdImagePtr small;
    gdFontPtr font;
    int color;

    if (!world_map)
        return;

    printf("Generating world map in world.html...");
    fflush(stdout);

    pic = gdImageCreateTrueColor(SIZE*30, SIZE*30);

    strcpy(file, root);
    strcat(file, "/world.html");

    wx = 100;
    wy = 100;

    for (y = 0; y < 30; y++) {
        for (x = 0; x < 30; x++) {
            snprintf(name, sizeof(name), "world_%d_%d", wx, wy);
            snprintf(mapleft, sizeof(mapleft), "%d", SIZE*x);
            snprintf(maptop, sizeof(maptop), "%d", SIZE*y);
            snprintf(mapright, sizeof(mapright), "%d", SIZE*(x+1)-1);
            snprintf(mapbottom, sizeof(mapbottom), "%d", SIZE*(y+1)-1);

            snprintf(mappath, sizeof(mappath), "%s/world/%s.x1%s", root, name, output_extensions[output_format]);

            out = fopen(mappath, "rb");
            if (!out) {
                printf("\n  warning: large pic not found for world_%d_%d", wx, wy);
                wx++;
                continue;
            }
            if (output_format == OF_PNG)
                small = gdImageCreateFromPng(out);
            else
                small = gdImageCreateFromJpeg(out);
            fclose(out);
            if (!small) {
                printf("\n  warning: pic not found for world_%d_%d", wx, wy);
                wx++;
                continue;
            }
            gdImageCopyResized(pic, small, SIZE*x, SIZE*y, 0, 0, SIZE, SIZE, small->sx, small->sy);
            gdImageDestroy(small);

            wx++;
        }
        wy++;
        wx = 100;
    }

    snprintf(mappath, sizeof(mappath), "%s/world_raw%s", root, output_extensions[output_format]);
    out = fopen(mappath, "wb+");
    save_picture(out, pic);
    fclose(out);

    /* Write region names. */
    small = gdImageCreateTrueColor(SIZE*30, SIZE*30);
    font = gdFontGetGiant();
    color = gdImageColorAllocateAlpha(pic, 255, 0, 0, 20);
    for (size_t region = 0; region < region_allocated; region++) {
        if (!regions[region]->is_world || regions[region]->sum == 0)
            continue;

        x = regions[region]->sum_x*SIZE/regions[region]->sum+SIZE/2-strlen(regions[region]->reg->name)*font->w/2;
        y = regions[region]->sum_y*SIZE/regions[region]->sum+SIZE/2-font->h/2;
        gdImageString(small, font, x, y, (unsigned char *)regions[region]->reg->name, color);
        gdImageString(pic, font, x, y, (unsigned char *)regions[region]->reg->name, color);

        /* For exit/road map, size isn't the same. */
        x = regions[region]->sum_x*50/regions[region]->sum+50/2-strlen(regions[region]->reg->name)*font->w/2;
        y = regions[region]->sum_y*50/regions[region]->sum+50/2-font->h/2;
        gdImageString(infomap, font, x, y, (unsigned char *)regions[region]->reg->name, color);
    }

    snprintf(mappath, sizeof(mappath), "%s/world_regions%s", root, output_extensions[output_format]);
    out = fopen(mappath, "wb+");
    save_picture(out, small);
    fclose(out);
    gdImageDestroy(small);

    snprintf(mappath, sizeof(mappath), "%s/world%s", root, output_extensions[output_format]);
    out = fopen(mappath, "wb+");
    save_picture(out, pic);
    fclose(out);
    gdImageDestroy(pic);

    printf(" done.\n");
#undef SIZE
}

/** Ensures all maps have a name (if there was a limit to map processing, some maps will have a NULL name which causes issues). */
static void fix_map_names(void) {
    for (size_t map = 0; map < maps_list.count; map++) {
        if (maps_list.maps[map]->name)
            continue;
        if (!maps_list.maps[map]->filename) {
            printf("map without path!\n");
            abort();
        }
        maps_list.maps[map]->name = strdup(maps_list.maps[map]->filename);
    }
}

/**
 * Ensures all tiled maps have a name, a region, a filename and a path.
 * Will try to find a suitable name and region from the maps in the group.
 * @todo
 * use a better filename, try to get the start of the map filenames.
 */
static void fix_tiled_map(void) {
    size_t map, tile;
    char name[500];
    char *slash, *test;
    region *cfregion;

    for (map = 0; map < tiled_map_list.count; map++) {
        if (tiled_map_list.maps[map]->tiled_maps.count == 0) {
            printf("empty tiled map group!");
            abort();
        }

        snprintf(name, sizeof(name), "tiled_map_group_%zu", map);
        tiled_map_list.maps[map]->filename = strdup(name);

        cfregion = NULL;
        test = NULL;

        for (tile = 0; tile < tiled_map_list.maps[map]->tiled_maps.count; tile++) {
            if (tiled_map_list.maps[map]->tiled_maps.maps[tile]->cfregion == NULL)
                /* map not processed, ignore it. */
                continue;

            if (!cfregion)
                cfregion = tiled_map_list.maps[map]->tiled_maps.maps[tile]->cfregion;
            else if (cfregion != tiled_map_list.maps[map]->tiled_maps.maps[tile]->cfregion) {
                printf("*** warning: tiled maps %s and %s not in same region (%s and %s).\n",
                    tiled_map_list.maps[map]->tiled_maps.maps[0]->path, tiled_map_list.maps[map]->tiled_maps.maps[tile]->path,
                    tiled_map_list.maps[map]->tiled_maps.maps[0]->cfregion->name, tiled_map_list.maps[map]->tiled_maps.maps[tile]->cfregion->name);
                cfregion = NULL;
            }

            if (strcmp(tiled_map_list.maps[map]->tiled_maps.maps[tile]->name, tiled_map_list.maps[map]->tiled_maps.maps[tile]->filename)) {
                /* map has a custom name, use it */
                if (!test)
                    test = tiled_map_list.maps[map]->tiled_maps.maps[tile]->name;
            }
        }

        if (!test) {
            /* this can happen of course if only partial maps were processed, but well... */
            printf("*** warning: tiled map without any name. First map path %s\n", tiled_map_list.maps[map]->tiled_maps.maps[0]->path);
            test = name;
        }

        tiled_map_list.maps[map]->name = strdup(test);
        tiled_map_list.maps[map]->cfregion = cfregion;

        strncpy(name, tiled_map_list.maps[map]->tiled_maps.maps[0]->path, sizeof(name));
        slash = strrchr(name, '/');
        if (!slash)
            snprintf(name, sizeof(name), "/");
        else
            *(slash+1) = '\0';
        strncat(name, tiled_map_list.maps[map]->filename, sizeof(name) - strlen(name) - 1);
        tiled_map_list.maps[map]->path = strdup(name);
    }
}

/**
 * Changes for the list all maps to the tiled map they are part of, if applicable.
 *
 * @param current
 * map currently being processed.
 * @param from
 * list that contains the exits to/from map to be fixed.
 * @param is_from
 * if non zero, <code>from</code> is exit_from field, else it is an exit_to.
 */
static void fix_exits_for_map(struct_map_info *current, struct_map_list *from, int is_from) {
    int map, max;
    struct_map_info *group;

    max = from->count-1;
    for (map = max; map >= 0; map--) {
        if (from->maps[map]->tiled_group) {
            group = from->maps[map]->tiled_group;
            if (map != max)
                from->maps[map] = from->maps[max];
            from->count--;
            max--;
            add_map(group, from);
            add_map(current->tiled_group ? current->tiled_group : current, is_from ? &group->exits_to : &group->exits_from);
        }
    }
}

/** Changes all exits to maps in a tiled map to point directly to the tiled map. Same for region lists. */
static void fix_exits_to_tiled_maps(void) {
    int map, max;
    struct_map_info *group;

    for (map = 0; map < maps_list.count; map++) {
        fix_exits_for_map(maps_list.maps[map], &maps_list.maps[map]->exits_from, 1);
        fix_exits_for_map(maps_list.maps[map], &maps_list.maps[map]->exits_to, 0);
    }

    for (size_t region = 0; region < region_count; region++) {
        max = regions[region]->maps_list.count-1;
        for (map = max; map >= 0; map--) {
            if (regions[region]->maps_list.maps[map]->tiled_group) {
                group = regions[region]->maps_list.maps[map]->tiled_group;
                if (map != max)
                    regions[region]->maps_list.maps[map] = regions[region]->maps_list.maps[max];
                regions[region]->maps_list.count--;
                max--;
                add_map(group, &regions[region]->maps_list);
            }
        }
    }
}

/**
 * Makes all monsters point to tiled maps instead of map when appliable, and merge
 * map monster to tiled map.
 */
static void fix_tiled_map_monsters(void) {
    int map, max;
    struct_map_info *group;

    for (size_t race = 0; race < races.count; race++) {
        max = races.races[race]->origin.count-1;
        for (map = max; map >= 0; map--) {
            if (races.races[race]->origin.maps[map]->tiled_group) {
                group = races.races[race]->origin.maps[map]->tiled_group;
                if (map != max)
                    races.races[race]->origin.maps[map] = races.races[race]->origin.maps[max];
                races.races[race]->origin.count--;
                max--;
                add_map(group, &races.races[race]->origin);
            }
        }
    }

    for (map = 0; map < maps_list.count; map++) {
        if (maps_list.maps[map]->tiled_group) {
            for (size_t race = 0; race < maps_list.maps[map]->monsters.count; race++) {
                add_race_to_list(maps_list.maps[map]->monsters.races[race], &maps_list.maps[map]->tiled_group->monsters, 1);
            }
        }
    }
}

static int tiled_map_need_pic(struct_map_info *map) {
    size_t test;
    char picpath[500];
    struct stat stats;

    for (size_t size = 0; size < num_sizes; size++) {
        generate_picture_path(map->path, size, picpath, sizeof(picpath));
        if (stat(picpath, &stats))
            return 1;
    }

    for (test = 0; test < map->tiled_maps.count; test++) {
        if (map->tiled_maps.maps[test]->pic_was_done)
            return 1;
    }

    return 0;
}

/**
 * Generates the large and small pictures for a tiled map.
 * This uses the large/small pictures made during process_map(), so having a map limit could lead
 * to maps not found and invalid results.
 *
 * @param map
 * tiled map to make the picture of.
 * @todo
 * add a field to struct_map_info to remember if pic was updated or not, and update the tiled map
 * only if one map has changed / the pic doesn't exist.
 */
static void do_tiled_map_picture(struct_map_info *map) {
    int xmin = 0, xmax = 0, ymin = 0, ymax = 0, count, last;
    size_t tiled;
    char picpath[500];
    gdImagePtr large, load;
    FILE *out;
    struct_map_info *current;

    if (!generate_pics)
        return;

    printf(" Generating composite map for %s...", map->name);
    fflush(stdout);

    if (!tiled_map_need_pic(map)) {
        printf(" already uptodate.\n");
        return;
    }

    count = map->tiled_maps.count;
    if (count == 0) {
        printf("Tiled map without tiled maps?\n");
        abort();
    }
    map->tiled_maps.maps[0]->processed = 1;
    map->tiled_maps.maps[0]->tiled_x_from = 0;
    map->tiled_maps.maps[0]->tiled_y_from = 0;

    while (count > 0) {
        last = count;

        for (tiled = 0; tiled < map->tiled_maps.count; tiled++) {
            current = map->tiled_maps.maps[tiled];
            if (current->processed != 1)
                continue;

            count--;

            if ((current->tiles[0]) && (current->tiles[0]->processed == 0)) {
                current->tiles[0]->processed = 1;
                current->tiles[0]->tiled_x_from = current->tiled_x_from;
                current->tiles[0]->tiled_y_from = current->tiled_y_from-current->tiles[0]->height;
            }
            if ((current->tiles[1]) && (current->tiles[1]->processed == 0)) {
                current->tiles[1]->processed = 1;
                current->tiles[1]->tiled_x_from = current->tiled_x_from+current->width;
                current->tiles[1]->tiled_y_from = current->tiled_y_from;
            }
            if ((current->tiles[2]) && (current->tiles[2]->processed == 0)) {
                current->tiles[2]->processed = 1;
                current->tiles[2]->tiled_x_from = current->tiled_x_from;
                current->tiles[2]->tiled_y_from = current->tiled_y_from+current->height;
            }
            if ((current->tiles[3]) && (current->tiles[3]->processed == 0)) {
                current->tiles[3]->processed = 1;
                current->tiles[3]->tiled_x_from = current->tiled_x_from-current->tiles[3]->width;
                current->tiles[3]->tiled_y_from = current->tiled_y_from;
            }
        }

        if (last == count) {
            printf("do_tiled_map_picture: didn't process any map in %s (%d left)??\n", map->path, last);
            abort();
        }
    }

    for (tiled = 0; tiled < map->tiled_maps.count; tiled++) {
        if (map->tiled_maps.maps[tiled]->tiled_x_from < xmin)
            xmin = map->tiled_maps.maps[tiled]->tiled_x_from;
        if (map->tiled_maps.maps[tiled]->tiled_y_from < ymin)
            ymin = map->tiled_maps.maps[tiled]->tiled_y_from;
        if (map->tiled_maps.maps[tiled]->tiled_x_from+map->tiled_maps.maps[tiled]->width > xmax)
            xmax = map->tiled_maps.maps[tiled]->tiled_x_from+map->tiled_maps.maps[tiled]->width;
        if (map->tiled_maps.maps[tiled]->tiled_y_from+map->tiled_maps.maps[tiled]->height > ymax)
            ymax = map->tiled_maps.maps[tiled]->tiled_y_from+map->tiled_maps.maps[tiled]->height;
    }

    large = gdImageCreateTrueColor(size_large*(xmax-xmin), size_large*(ymax-ymin));

    for (tiled = 0; tiled < map->tiled_maps.count; tiled++) {
        generate_picture_path(map->tiled_maps.maps[tiled]->path, 0, picpath, sizeof(picpath));

        out = fopen(picpath, "rb");
        if (!out) {
            printf("\n  do_tiled_map_picture: warning: pic file %s not found for %s (errno=%d)\n", picpath, map->tiled_maps.maps[tiled]->path, errno);
            continue;
        }
        if (output_format == OF_PNG)
            load = gdImageCreateFromPng(out);
        else
            load = gdImageCreateFromJpeg(out);
        fclose(out);
        if (!load) {
            printf("\n  do_tiled_map_picture: warning: pic not found for %s\n", map->tiled_maps.maps[tiled]->path);
            continue;
        }
        gdImageCopy(large, load, size_large*(map->tiled_maps.maps[tiled]->tiled_x_from-xmin), size_large*(map->tiled_maps.maps[tiled]->tiled_y_from-ymin), 0, 0, load->sx, load->sy);
        gdImageDestroy(load);
    }

    write_pictures_from_real_size(map->path, large, xmax-xmin, ymax-ymin);

    gdImageDestroy(large);

    printf(" done.\n");
}

/** Writes the page for a tiled map group. */
static void write_tiled_map_page(struct_map_info *map) {

    do_tiled_map_picture(map);

    /** @todo: do a real page, with the various levels, maps and such. */

//    write_map_page(map);
}

/** Outputs all tiled map pages. */
static void write_tiled_maps(void) {
    printf("Writing tiled map information...\n");

    for (size_t map = 0; map < tiled_map_list.count; map++)
        write_tiled_map_page(tiled_map_list.maps[map]);

    printf(" done.\n");
}

static size_t system_quests_count = 0;
static const quest_definition *system_quests[500];

static void quest_callback(const quest_definition *quest, void *) {
    if (list_system_quests || !quest->quest_is_system) {
        system_quests[system_quests_count++] = quest;
    }
}

static int sort_system_quest(const void *a, const void *b) {
    return strcmp((*((quest_definition **)a))->quest_code, (*((quest_definition **)b))->quest_code);
}

static int sort_system_quest_step(const void *a, const void *b) {
    return (*((quest_step_definition**)a))->step - (*((quest_step_definition**)b))->step;
}

static std::shared_ptr<inja::Environment> env;                  /**< Rendering environment. */
static nlohmann::json all_data;                                 /**< All JSON data available to templates. */
static std::set<std::string> rendered_templates;                /**< List of generated files, to not generate multiple times. */
static std::map<struct_map_info *, std::string> reverse_maps;   /**< Link between a map and its unique identifier. */
static std::map<region *, std::string> reverse_regions;         /**< Link between a region and its unique identifier. */

/**
 * Get the map with the specified key, nullptr if not found.
 * @param key key of the map to find.
 * @return map, nullptr if not found.
 */
static struct_map_info *find_map_by_key(const std::string &key) {
    auto found = std::find_if(reverse_maps.cbegin(), reverse_maps.cend(), [&key] (auto c) { return c.second == key; });
    if (found != reverse_maps.cend())
        return found->first;
    return nullptr;
}

/**
 * Get the region with the specified key, nullptr if not found.
 * @param key key of the region to find.
 * @return region, nullptr if not found.
 */
static region *find_region_by_key(const std::string &key) {
    auto found = std::find_if(reverse_regions.cbegin(), reverse_regions.cend(), [&key] (auto c) { return c.second == key; });
    if (found != reverse_regions.cend())
        return found->first;
    return nullptr;
}

/**
 * Return an array of map identifiers.
 * @param maps items to return the JSON of.
 * @return array.
 */
static nlohmann::json create_maps_array(struct_map_list &maps) {
    nlohmann::json result = nlohmann::json::array();
    for (size_t m = 0; m < maps.count; m++) {
        auto map = reverse_maps.find(maps.maps[m]);
        if (map != reverse_maps.end()) {
            result.push_back(map->second);
        }
    }
    return result;
}

/**
 * Return an array of NPC information.
 * @param list items to return the JSON of.
 * @return array.
 */
static nlohmann::json create_npc_array(struct_npc_list &list) {
    nlohmann::json result;
    for (size_t n = 0; n < list.count; n++) {
        auto npc = list.npc[n];
        result.push_back({
            { "name", npc->name },
            { "x", npc->x },
            { "y", npc->y },
            { "message", npc->message },
        });
    }
    return result;
}

/**
 * Return an array of monster information.
 * @param list items to return the JSON of.
 * @return array.
 */
static nlohmann::json create_race_array(struct_race_list &list) {
    nlohmann::json result;
    for (size_t n = 0; n < list.count; n++) {
        auto race = list.races[n];
        result.push_back({
            { "name", race->name },
            { "count", race->count },
        });
    }
    return result;
}

/**
 * Return an array of map-in-quest items.
 * @param list items to return the JSON of.
 * @return array.
 */
static nlohmann::json create_map_in_quest_array(struct_map_in_quest_list &list) {
    nlohmann::json ret = nlohmann::json::array();
    for (size_t m = 0; m < list.count; m++) {
        auto q = list.list[m];
        if (!q->map || !q->description)
            continue;
        ret.push_back({
            { "map", reverse_maps.find(q->map)->second },
            { "description", q->description },
            { "quest", q->quest->name },
            { "number", q->quest->number },
        });
    }

    return ret;
}

/**
 * Return a JSON map object.
 * @param map map to return the JSON of.
 * @param key map unique identifier.
 * @return JSON object.
 */
static nlohmann::json create_map_object(struct_map_info *map, const std::string &key) {
    return {
        { "_key", key },
        { "name", map->name },
        { "path", map->path },
        { "region", map->cfregion ? reverse_regions[map->cfregion] : "reg_ffff" },
        { "level", map->level },
        { "lore", map->lore && map->lore[0] ? map->lore : "" },
        { "exits_to", create_maps_array(map->exits_to) },
        { "exits_from", create_maps_array(map->exits_from) },
        { "npcs", create_npc_array(map->npcs) },
        { "readables", create_npc_array(map->readable) },
        { "monsters", create_race_array(map->monsters) },
        { "quests", create_map_in_quest_array(map->quests) },
    };
}

/**
 * Return a JSON quest object.
 * @param quest quest to return the JSON of.
 * @param key quest unique key.
 * @return JSON object.
 */
static nlohmann::json create_quest_object(struct_quest *quest, const std::string &key) {
    return {
        { "_key", key },
        { "number", quest->number },
        { "name", quest->name ? quest->name : "" },
        { "description", quest->description ? quest->description : "" },
        { "main_map", quest->mainmap ? reverse_maps.find(quest->mainmap)->second : "" },
        { "maps", create_map_in_quest_array(quest->maps) },
    };
}

/**
 * Fill the ::reverse_maps array with the provided list.
 * @param list maps to put.
 */
static void fill_reverse_maps(struct_map_list &list) {
    char buf[50];
    for (size_t map = 0; map < list.count; map++) {
        auto cur = list.maps[map];
        if (cur->tiled_group)
            continue;
        snprintf(buf, sizeof(buf), "map_%04lu", map);
        reverse_maps.insert(std::make_pair(cur, buf));
        qsort(cur->exits_to.maps, cur->exits_to.count, sizeof(struct_map_info *), sort_map_info);
        qsort(cur->exits_from.maps, cur->exits_from.count, sizeof(struct_map_info *), sort_map_info);
        qsort(cur->monsters.races, cur->monsters.count, sizeof(struct_race *), sort_race);
    }
}

/**
 * Append the contents of src to dest.
 * @param dest where to put items.
 * @param src what to append to dest, unchanged.
 */
static void append_map_list(struct_map_list &dest, struct_map_list &src) {
    for (size_t map = 0; map < src.count; map++)
        add_map(src.maps[map], &dest);
}

/**
 * Return an array of region identifiers.
 * @param regions regions to process.
 * @return JSON object.
 */
static nlohmann::json create_region_array(const std::set<region *> &regions) {
    nlohmann::json ret = nlohmann::json::array();
    for (auto reg : regions) {
        auto r = reverse_regions.find(reg);
        if (r != reverse_regions.end()) {
            ret.push_back((*r).second);
        }
    }
    return ret;
}

/**
 * Add all global variables to the data available to templates.
 * @param json what to fill.
 */
static void fill_json(nlohmann::json &json) {
    nlohmann::json maps;
    char buf[10];
    struct_map_list all_maps;
    bool need_unknown_region = false;

    init_map_list(&all_maps);
    append_map_list(all_maps, maps_list);
    append_map_list(all_maps, tiled_map_list);
    qsort(all_maps.maps, all_maps.count, sizeof(struct_map_info *), sort_map_info);

    fill_reverse_maps(all_maps);

    for (size_t reg = 0; reg < region_count; reg++) {
        auto region = regions[reg];
        snprintf(buf, sizeof(buf), "reg_%04lu", reg);
        reverse_regions.insert(std::make_pair(region->reg, buf));
    }

    for (size_t reg = 0; reg < region_count; reg++) {
        auto region = regions[reg];
        qsort(region->maps_list.maps, region->maps_list.count, sizeof(struct_map_info *), sort_map_info);

        json["regions"].push_back({
            { "_key", reverse_regions[region->reg] },
            { "name", region->reg->name },
            { "longname", region->reg->longname },
            { "description", region->reg->msg ? region->reg->msg : "" },
            { "maps", create_maps_array(region->maps_list) },
            { "links", create_region_array(region_links[region->reg]) },
        });
    }

    for (auto map : reverse_maps) {
        auto cur = map.first;
        if (cur->tiled_group)
            continue;
        if (cur->cfregion == nullptr)
            need_unknown_region = true;
        json["maps"].push_back(create_map_object(cur, map.second));
    }

    if (need_unknown_region) {
        json["regions"].push_back({
            { "_key", "reg_ffff" },
            { "name", "unknown" },
            { "longname", "unknown" },
            { "description", "unknown" },
            { "maps", nlohmann::json::array() },
            { "links", nlohmann::json::array() },
        });
    }

    json["items"] = nlohmann::json::array();
    for (size_t idx = 0; idx < equipment_count; idx++) {
        auto eq = special_equipment[idx];
        json["items"][idx] = {
            { "name", eq->name },
            { "power", eq->power },
            { "calc_power", eq->calc_power },
            { "diff", eq->diff },
            { "maps", create_maps_array(eq->origin) },
        };
    }

    json["monsters"] = nlohmann::json::array();
    for (size_t item = 0; item < races.count; item++) {
        auto race = races.races[item];
        qsort(race->origin.maps, race->origin.count, sizeof(struct_map_info *), sort_map_info);
        json["monsters"].push_back({
            { "name", race->name },
            { "count", race->count },
            { "maps", create_maps_array(race->origin) },
        });
    }

    json["system_quests"] = nlohmann::json::array();
    for (size_t q = 0; q < system_quests_count; q++) {
        auto quest = system_quests[q];
        nlohmann::json j({
            { "code", quest->quest_code },
            { "title", quest->quest_title },
            { "description", quest->quest_description ? quest->quest_description : "" },
            { "replayable", quest->quest_restart },
            { "steps", nlohmann::json::array() },
            { "maps", nlohmann::json::array() },
        });

        if (detail_quests) {
            quest_step_definition *steps[100];
            size_t steps_count = 0;
            quest_step_definition *step = quest->steps;
            while (step) {
                steps[steps_count++] = step;
                step = step->next;
            }
            qsort(steps, steps_count, sizeof(quest_step_definition *), sort_system_quest_step);
            for (size_t s = 0; s < steps_count; s++) {
                j["steps"].push_back({
                    { "description", steps[s]->step_description ? steps[s]->step_description : "" },
                    { "is_completion", steps[s]->is_completion_step ? true : false },
                });
            }

            auto qim = find_quest_info(quest->quest_code);
            if (qim) {
                for (size_t m = 0; m < qim->maps.count; m++) {
                    auto map = reverse_maps.find(qim->maps.list[m]->map);
                    assert(map != reverse_maps.end());
                    j["maps"].push_back({
                        { "description", qim->maps.list[m]->description },
                        { "map", map->second },
                    });
                }
            }
        }
        json["system_quests"].push_back(j);
    }

    json["slaying"] = nlohmann::json::array();
    for (size_t s = 0; s < slaying_count; s++) {
        auto info = slaying_info[s];
        json["slaying"].push_back({
            { "slaying", info->slaying },
            { "doors", create_maps_array(info->maps[S_DOOR]) },
            { "keys", create_maps_array(info->maps[S_KEY]) },
            { "containers", create_maps_array(info->maps[S_CONTAINER]) },
            { "detectors", create_maps_array(info->maps[S_DETECTOR]) },
            { "connections", create_maps_array(info->maps[S_CONNECT]) },
        });
    }

    json["quests"] = nlohmann::json::array();
    for (int quest = 0; quest < quests_count; quest++) {
        qsort(quests[quest]->maps.list, quests[quest]->maps.count, sizeof(struct_map_in_quest *), sort_struct_map_in_quest);
        char buf[100];
        snprintf(buf, sizeof(buf), "quest_%d", quests[quest]->number);
        json["quests"].push_back(create_quest_object(quests[quest], buf));
    }
}

/** Path, relative to output root, of pages being generated. */
static std::vector<std::string> path_stack;

void add_template_to_render(const std::string &template_name, const std::string &output_name, const std::string &param);

/**
 * Compute the relative path from the specified file to the current file.
 * @param path path to get from.
 * @return relative path to the current page.
 */
static std::string path_from_current(const std::string &path) {
    auto p(path);
    char rel[1000];
    if (p[0] != '/')
        p = '/' + p;
    auto current(path_stack.back());
    if (current[0] != '/')
        current = '/' + current;
    relative_path(current.c_str(), p.c_str(), rel);
    return rel;
}

/**
 * Create a link to a page, generating it if needed.
 * @param args link arguments.
 * @return link to the page.
 */
static nlohmann::json generate_page_and_link(inja::Arguments &args) {
    auto template_name = args.at(0)->get<std::string>();
    auto output_name(template_name);
    auto param = (args.size() > 1 ? args.at(1)->get<std::string>() : "");

    if (!param.empty()) {
        output_name = param + "_" + output_name;
        if (param.substr(0, 4) == "map_") {
            auto map = find_map_by_key(param);
            if (map != nullptr) {
                output_name = std::string(map->path + 1) + ".html";
            }
        }
        if (param.substr(0, 4) == "reg_") {
            auto reg = find_region_by_key(param);
            if (reg != nullptr) {
                output_name = std::string(reg->name) + ".html";
            }
        }
    }

    add_template_to_render(template_name, output_name, param);
    return path_from_current(output_name);
}

/**
 * Return the link to the picture of the specified item.
 * @param args function arguments.
 * @return link, empty string if invalid.
 */
static nlohmann::json generate_picture_link(inja::Arguments &args) {
    auto what = args.at(0)->get<std::string>();
    if (what.substr(0, 4) == "map_") {
        auto map = find_map_by_key(what);
        if (map == nullptr)
            return "";
        int size = 0;
        if (args.size() > 1) {
            size = args.at(1)->get<int>() - 1;
        }
        char picpath[1000];
        snprintf(picpath, sizeof(picpath), "%s.x%d%s", map->path, size + 1, output_extensions[output_format]);
        return path_from_current(picpath);
    }
    return "";
}

/**
 * One page to render, with its parameters.
 */
class pageToRender {
public:
    std::string template_name;  /**< Template name to use. */
    std::string output_name;    /**< Output file name. */
    std::string param;          /**< Optional template parameter. */
};
static std::vector<pageToRender> pages; /**< List of pages to render. */

/**
 * Push the specified template, with optional param, on the list of files to process.
 * @param template_name template name to use.
 * @param output_name output file name.
 * @param param additional parameter to the template.
 */
void add_template_to_render(const std::string &template_name, const std::string &output_name, const std::string &param) {
    auto on(output_name);
    if (on[0] != '/')
        on = '/' + on;
    if (rendered_templates.find(on) != rendered_templates.end())
        return;

    rendered_templates.insert(on);
    pageToRender r;
    r.template_name = template_name;
    r.output_name = on;
    r.param = param;
    pages.push_back(r);
}

static std::string templates_root("templates/");    /**< Directory to get templates from, with a leading /. */
static std::vector<std::string> templates;          /**< List of template files to start processing from. */

/**
 * Initialize ::env and set various callbacks and options.
 */
static void init_renderer_env() {
    env = std::make_shared<inja::Environment>(templates_root, std::string(root) + "/");
    env->add_callback("link_to_page", generate_page_and_link);
    env->add_callback("substr", [] (inja::Arguments &args) {
        std::string str = args.at(0)->get<std::string>();
        size_t start = args.at(1)->get<size_t>();
        size_t len = args.size() > 2 ? args.at(2)->get<size_t>() : std::string::npos;
        return str.substr(start, len);
    });
    env->add_callback("picture", generate_picture_link);
    env->add_callback("pad", [] (inja::Arguments &args) {
        char buf[50];
        int val = args.at(0)->get<int>(), digits = args.at(1)->get<int>();
        snprintf(buf, sizeof(buf), "%0*d", digits, val);
        return std::string(buf);
    });
    env->add_callback("path_to_root", 0, [] (inja::Arguments &) {
        std::string r(root);
        char rel[1000];
        auto current(path_stack.back() + '/');
        if (current[0] != '/')
            current = '/' + current;
        relative_path(current.c_str(), r.c_str(), rel);
        return std::string(rel);
    });
    env->add_callback("get_by_field", 3, [] (inja::Arguments &args) {
        auto src = args.at(0);
        auto field = args.at(1)->get<std::string>();
        auto value = args.at(2);
        auto found = std::find_if(src->begin(), src->end(), [&field, &value] (auto item) {
            return item[field] == *value;
        });
        if (found == src->end()) {
            return nlohmann::json();
        }
        return *found;
    });
    env->add_callback("sort", [] (inja::Arguments &args) {
        auto src = args.at(0);
        std::vector<nlohmann::json> ret;
        for (auto i : *src) {
            ret.push_back(i);
        }
        auto field = args.at(1)->get<std::string>();
        bool invert = args.size() > 2 ? args.at(2)->get<bool>() : false;
        bool ignore_case = args.size() > 3 ? args.at(3)->get<bool>() : true;
        std::sort(ret.begin(), ret.end(), [&field, &invert, &ignore_case] (auto left, auto right) {
            nlohmann::json l = left[field], r = right[field];
            if (ignore_case && l.is_string() && r.is_string()) {
                std::string ls(l.get<std::string>()), rs(r.get<std::string>());
                std::transform(ls.begin(), ls.end(), ls.begin(), [](unsigned char c){ return std::tolower(c); });
                std::transform(rs.begin(), rs.end(), rs.begin(), [](unsigned char c){ return std::tolower(c); });
                return invert ? (rs < ls) : (ls < rs);
            }
            return invert ? (r < l) : (l < r);
        });
        return ret;
    });

    env->set_trim_blocks(true);
    env->set_lstrip_blocks(true);
}

/** Directories to ignore for map search. */
static const char *ignore_path[] = {
    "/Info",
    "/editor",
    "/python",
    "/styles",
    "/templates",
    "/test",
    "/unlinked",
    NULL };

/** File names to ignore for map search. */
static const char *ignore_name[] = {
    ".",
    "..",
    ".svn",
    "README",
    NULL };

/**
 * Recursively find all all maps in a directory.
 *
 * @param from
 * path to search from, without trailing /.
 */
static void find_maps(const char *from) {
    struct dirent *file;
    struct stat statbuf;
    int status, ignore;
    char path[1024], full[1024];
    DIR *dir;

    for (ignore = 0; ignore_path[ignore] != NULL; ignore++) {
        if (strcmp(from, ignore_path[ignore]) == 0)
            return;
    }

    snprintf(path, sizeof(path), "%s/%s%s", settings.datadir, settings.mapdir, from);
    dir = opendir(path);

    if (dir) {
        for (file = readdir(dir); file; file = readdir(dir)) {

            for (ignore = 0; ignore_name[ignore] != NULL; ignore++) {
                if (strcmp(file->d_name, ignore_name[ignore]) == 0)
                    break;
            }
            if (ignore_name[ignore] != NULL)
                continue;

            snprintf(full, sizeof(full), "%s/%s", path, file->d_name);

            status = stat(full, &statbuf);
            if ((status != -1) && (S_ISDIR(statbuf.st_mode))) {
                snprintf(full, sizeof(full), "%s/%s", from, file->d_name);
                find_maps(full);
                continue;
            }
            if (found_maps_count == found_maps_allocated) {
                found_maps_allocated += 50;
                found_maps = (char **)realloc(found_maps, found_maps_allocated*sizeof(char *));
            }
            snprintf(full, sizeof(full), "%s/%s", from, file->d_name);
            found_maps[found_maps_count++] = strdup(full);
        }
        closedir(dir);
    }
}

/** Writes the list of unused maps, maps found in the directories but not linked from the other maps. */
static void dump_unused_maps(void) {
    FILE *dump;
    char path[1024];
    int index, found = 0;

    snprintf(path, sizeof(path), "%s/%s", root, "maps.unused");
    dump = fopen(path, "w+");
    if (dump == NULL) {
        printf("Unable to open file maps.unused!\n");
        return;
    }
    for (index = 0; index < found_maps_count; index++) {
        if (found_maps[index] != NULL) {
            fprintf(dump, "%s\n", found_maps[index]);
            free(found_maps[index]);
            found++;
        }
    }
    fclose(dump);
    printf("%d unused maps.\n", found);
}

/** Writes the exit information world map. */
static void write_world_info(void) {
    FILE *file;
    char path[MAX_BUF];
    int x, y;
    gdImagePtr elevationmap;

    if (!world_exit_info)
        return;

    printf("Saving exit/blocking/road information...");
    snprintf(path, sizeof(path), "%s/%s%s", root, "world_info", output_extensions[output_format]);
    file = fopen(path, "wb+");
    save_picture(file, infomap);
    fclose(file);
    printf("done.\n");
    gdImageDestroy(infomap);
    infomap = NULL;

   if (elevation_min == 0 || elevation_max == 0) {
       puts("Error: Could not save elevation world map due to not finding any minimum or maximum elevation.");
       return;
   }

    elevationmap = gdImageCreateTrueColor(30*50, 30*50);;

    for (x = 0; x < 30*50; x++) {
        for (y = 0; y < 30*50; y++) {
            gdImageSetPixel(elevationmap, x, y, get_elevation_color(elevation_info[x][y], elevationmap));
        }
    }

    printf("Saving elevation world map...");
    snprintf(path, sizeof(path), "%s/%s%s", root, "world_elevation", output_extensions[output_format]);
    file = fopen(path, "wb+");
    save_picture(file, elevationmap);
    fclose(file);
    printf("done.\n");
    gdImageDestroy(elevationmap);
    elevationmap = NULL;
}

/**
 * Helper function to sort an array of struct_slaying_info.
 *
 * @param left
 * first item.
 * @param right
 * second item.
 * @return
 * sort order.
 */
static int sort_slaying(const void *left, const void *right) {
    struct_slaying_info *l = *(struct_slaying_info **)left;
    struct_slaying_info *r = *(struct_slaying_info **)right;

    return strcasecmp(l->slaying, r->slaying);
}

/**
 * Prints usage information, and exit.
 *
 * @param program
 * program path.
 */
static void do_help(const char *program) {
    printf("Crossfire Mapper will generate pictures of maps, and create indexes for all maps and regions.\n\n");
    printf("Syntax: %s\n\n", program);
    printf("Optional arguments:\n");
    printf("  -nopics             don't generate pictures.\n");
    printf("  -root=<path>        destination path. Default 'html'.\n");
    printf("  -limit=<number>     stop processing after this number of maps, -1 to do all maps (default).\n");
    printf("  -showmaps           outputs the name of maps as they are processed.\n");
    printf("  -jpg[=quality]      generate jpg pictures, instead of default png. Quality should be 0-95, -1 for automatic.\n");
    printf("  -forcepics          force to regenerate pics, even if pics's date is after map's.\n");
    printf("  -addmap=<map>       adds a map to process. Path is relative to map's directory root.\n");
    printf("  -rawmaps            generates maps pics without items on random (shop, treasure) tiles.\n");
    printf("  -warnnopath         inform when an exit has no path set.\n");
    printf("  -listunusedmaps     finds all unused maps in the maps directory.\n");
    printf("  -noworldmap         don't write the world map in world.png.\n");
    printf("  -noregionslink      don't generate regions relation file.\n");
    printf("  -regionslink        generate regions relation file.\n");
    printf("  -noexitmap          don't generate map of exits.\n");
    printf("  -exitmap            generate map of exits.\n");
    printf("  -tileset=<number>   use specified tileset to generate the pictures. Default 0 (standard).\n");
    printf("  -details-quests     list all quests steps. Default no.\n");
    printf("  -list-system-quests include 'system' quests in quest list. Default no.\n");
    printf("  -templates-dir=[dir]   set the directory to get templates from. Default 'templates/'.\n");
    printf("  -add-template=[file]   add a template to process. May be specified multiple times. If empty, 'index.html' is used.\n");
    printf("  -list-template-to-process display the name of the template about to be rendered. Useful for debugging.");
    printf("\n\n");
    exit(0);
}

/**
 * Handles command-line parameters.
 *
 * @param argc
 * number of parameters, including program name.
 * @param argv
 * arguments, including program name.
 */
static void do_parameters(int argc, char **argv) {
    int arg = 1;
    char path[500];

    root[0] = '\0';

    while (arg < argc) {
        if (strcmp(argv[arg], "-nopics") == 0)
            generate_pics = 0;
        else if (strncmp(argv[arg], "-root=", 6) == 0)
            strncpy(root, argv[arg]+6, 500);
        else if (strncmp(argv[arg], "-limit=", 7) == 0)
            map_limit = atoi(argv[arg]+7);
        else if (strcmp(argv[arg], "-showmaps") == 0)
            show_maps = 1;
        else if (strcmp(argv[arg], "-jpg") == 0) {
            output_format = OF_JPG;
            if (argv[arg][4] == '=') {
                jpeg_quality = atoi(argv[arg]+5);
                if (jpeg_quality < 0)
                    jpeg_quality = -1;
            }
        }
        else if (strcmp(argv[arg], "-forcepics") == 0)
            force_pics = 1;
        else if (strncmp(argv[arg], "-addmap=", 8) == 0) {
            if (*(argv[arg]+8) == '/')
                strncpy(path, argv[arg]+8, 500);
            else
                snprintf(path, 500, "/%s", argv[arg]+8);
            add_map(get_map_info(path), &maps_list);
        }
        else if (strcmp(argv[arg], "-rawmaps") == 0)
            rawmaps = 1;
        else if (strcmp(argv[arg], "-warnnopath") == 0)
            warn_no_path = 1;
        else if (strcmp(argv[arg], "-listunusedmaps") == 0)
            list_unused_maps = 1;
        else if (strcmp(argv[arg], "-noworldmap") == 0)
            world_map = 0;
        else if (strcmp(argv[arg], "-noregionslink") == 0)
            do_regions_link = false;
        else if (strcmp(argv[arg], "-regionslink") == 0)
            do_regions_link = true;
        else if (strcmp(argv[arg], "-noexitmap") == 0)
            world_exit_info = 0;
        else if (strcmp(argv[arg], "-exitmap") == 0)
            world_exit_info = 1;
        else if (strncmp(argv[arg], "-tileset=", 9) == 0) {
            tileset = atoi(argv[arg]+9);
            /* check of validity is done in main() as we need to actually have the sets loaded. */
        } else if (strcmp(argv[arg], "-detail-quests") == 0) {
            detail_quests = 1;
        } else if (strcmp(argv[arg], "-list-system-quests") == 0) {
            list_system_quests = 1;
        } else if (strncmp(argv[arg], "-templates-dir=", 15) == 0) {
            templates_root = argv[arg] + 15;
        } else if (strncmp(argv[arg], "-add-template=", 14) == 0) {
            templates.push_back(argv[arg] + 14);
        } else if (strcmp(argv[arg], "-list-template-to-process") == 0) {
            display_rendered_template = 1;
        } else
            do_help(argv[0]);
        arg++;
    }
    if (!strlen(root))
        strcpy(root, "html");
    if (root[strlen(root)-1] == '/')
        root[strlen(root)-1] = '\0';
    if (map_limit < -1)
        map_limit = -1;

    if (templates_root.empty()) {
        templates_root = "templates/";
    } else if (templates_root[templates_root.length() - 1] != '/') {
        templates_root.append("/");
    }
}

/**
 * Ensures destination directory exists.
 */
static void create_destination(void) {
    char dummy[502];

    strcpy(dummy, root);
    strcat(dummy, "/a");
    make_path_to_file(dummy);
}

/**
 * Helper to write yes/no.
 *
 * @param value
 * value to print.
 * @return
 * "no" if value == 0, "yes" else.
 */
static const char *yesno(int value) {
    return (value ? "yes" : "no");
}

int main(int argc, char **argv) {
    size_t current_map = 0, i;
    char max[50];
    region *dummy;

    init_map_list(&maps_list);
    init_map_list(&tiled_map_list);
    init_race_list(&races);
    pics_allocated = 0;

    do_parameters(argc, argv);

    printf("Initializing Crossfire data...\n");

    settings.debug = llevError;

    init_globals();
    init_library();
    init_readable();
    init_regions();

    init_gods();

    /* Add a dummy region so unlinked maps can be identified. */
    dummy = get_region_struct();
    dummy->fallback = 1;
    dummy->name = strdup_local("unlinked");
    dummy->longname = strdup_local("This dummy region contains all maps without a region set.");
    dummy->longname = strdup_local("This dummy region contains all maps without a region set.");
    dummy->next = first_region;
    first_region = dummy;

    printf("\n\n done.\n\n");

    if (!is_valid_faceset(tileset)) {
        printf("Erreor: invalid tileset %d!\n", tileset);
        exit(1);
    }

    if (templates.empty()) {
        templates.push_back("index.html");
    }

    create_destination();
    gdfaces = (gdImagePtr *)calloc(get_faces_count(), sizeof(gdImagePtr));

    if (map_limit != -1)
        snprintf(max, sizeof(max), "%d", map_limit);
    else
        strcpy(max, "(none)");
    printf("Crossfire map browser generator\n");
    printf("-------------------------------\n\n");
    printf("Parameters:\n");
    printf("  path to write files:                 %s\n", root);
    printf("  maximum number of maps to process:   %s\n", max);
    printf("  will generate map picture:           %s\n", yesno(generate_pics));
    printf("  will always generate map picture:    %s\n", yesno(force_pics));
    printf("  picture output format:               %s\n", output_extensions[output_format]);
    if (output_format == OF_JPG)
        printf("  JPEG quality:                        %d\n", jpeg_quality);
    printf("  show map being processed:            %s\n", yesno(show_maps));
    printf("  generate raw maps:                   %s\n", yesno(rawmaps));
    printf("  warn of exit without path:           %s\n", yesno(warn_no_path));
    printf("  list unused maps:                    %s\n", yesno(list_unused_maps));
    printf("  generate world map:                  %s\n", yesno(world_map));
    printf("  generate exit map:                   %s\n", yesno(world_exit_info));
    printf("  generate regions link file:          %s\n", yesno(do_regions_link));
    printf("  tileset:                             %s\n", find_faceset(tileset)->fullname);
    printf("  detail quest steps:                  %s\n", yesno(detail_quests));
    printf("  list system quests:                  %s\n", yesno(list_system_quests));
    printf("  templates directory:                 %s\n", templates_root.c_str());
    printf("  templates to process:                ");
    const char *sep = "";
    for (auto f : templates) {
        printf("%s%s", sep, f.c_str());
        sep = ", ";
    }
    printf("\n");
    printf("  display template to process:         %s\n", yesno(display_rendered_template));
    printf("\n");

    if (list_unused_maps) {
        printf("listing all maps...");
        find_maps("");
        printf("done, %d maps found.\n", found_maps_count);
        qsort(found_maps, found_maps_count, sizeof(char *), sortbyname);
    }

    /* exit/blocking information. */
    infomap = gdImageCreateTrueColor(30*50, 30*50);
    color_unlinked_exit = gdImageColorResolve(infomap, 255, 0, 0);
    color_linked_exit = gdImageColorResolve(infomap, 255, 255, 255);
    color_road = gdImageColorResolve(infomap, 0, 255, 0);
    color_blocking = gdImageColorResolve(infomap, 0, 0, 255);
    color_slowing = gdImageColorResolve(infomap, 0, 0, 127);
    elevation_info = (int **)calloc(50*30, sizeof(int *));
    for (i = 0; i < 50*30; i++)
        elevation_info[i] = (int *)calloc(50*30, sizeof(int));
    elevation_min = 0;
    elevation_max = 0;

    printf("browsing maps...\n");

    get_map_info(first_map_path);

    while (current_map < maps_list.count) {
        process_map(maps_list.maps[current_map++]);
        if (current_map%100 == 0) {
            printf(" %zu maps processed, %d map pictures created, %d map pictures were uptodate. %d faces used.\n", current_map, created_pics, cached_pics, pics_allocated);
        }
        if ((map_limit != -1) && (current_map == map_limit)) {
            printf(" --- map limit reached, stopping ---\n");
            break;
        }
    }

    printf(" finished map parsing, %zu maps processed, %d map pictures created, %d map pictures were uptodate. Total %d faces used.\n", current_map, created_pics, cached_pics, pics_allocated);

    if (list_unused_maps)
        dump_unused_maps();

    fix_exits_to_tiled_maps();
    fix_map_names();
    fix_tiled_map();
    fix_tiled_map_monsters();
    write_tiled_maps();

    write_world_map();
    write_world_info();

    qsort(maps_list.maps, maps_list.count, sizeof(struct_map_info *), sort_map_info);
    qsort(regions, region_count, sizeof(struct_region_info *), sort_region);
    qsort(special_equipment, equipment_count, sizeof(struct_equipment *), sort_equipment);
    qsort(slaying_info, slaying_count, sizeof(struct_slaying_info *), sort_slaying);
    qsort(races.races, races.count, sizeof(struct_race *), sort_race);
    qsort(quests, quests_count, sizeof(struct_quest *), sort_struct_quest);

    quest_for_each(&quest_callback, NULL);
    qsort(system_quests, system_quests_count, sizeof(quest_definition *), sort_system_quest);

    init_renderer_env();

    fill_json(all_data);

    for (auto file : templates) {
        if (!file.empty()) {
            add_template_to_render(file, file, "");
        }
    }

    while (!pages.empty()) {
        auto p = pages.back();
        pages.pop_back();
        if (p.param.empty())
            all_data.erase("param");
        else
            all_data["param"] = p.param;
        if (display_rendered_template) {
            printf("rendering page %s (%s)\n", p.template_name.c_str(), p.param.c_str());
            fflush(stdout);
        }
        path_stack.push_back(p.output_name);
        env->write(p.template_name, all_data, p.output_name);
        path_stack.pop_back();
    }

    return 0;
}

void do_auto_apply(mapstruct *m) {
    int x, y;

    if (m == NULL)
        return;

    for (x = 0; x < MAP_WIDTH(m); x++)
        for (y = 0; y < MAP_HEIGHT(m); y++)
            FOR_MAP_PREPARE(m, x, y, tmp) {
                if (tmp->inv) {
                    FOR_INV_PREPARE(tmp, invtmp) {
                        if (QUERY_FLAG(invtmp, FLAG_AUTO_APPLY))
                            apply_auto(invtmp);
                        else if (invtmp->type == TREASURE && HAS_RANDOM_ITEMS(invtmp)) {
                            while ((invtmp->stats.hp--) > 0)
                                create_treasure(invtmp->randomitems, invtmp, 0, m->difficulty, 0);
                            invtmp->randomitems = NULL;
                        } else if (invtmp
                        && invtmp->arch
                        && invtmp->type != TREASURE
                        && invtmp->type != SPELL
                        && invtmp->type != CLASS
                        && HAS_RANDOM_ITEMS(invtmp)) {
                            create_treasure(invtmp->randomitems, invtmp, 0, m->difficulty, 0);
                            /* Need to clear this so that we never try to create
                             * treasure again for this object
                             */
                            invtmp->randomitems = NULL;
                        }
                    } FOR_INV_FINISH();
                    /* This is really temporary - the code at the bottom will
                     * also set randomitems to null.  The problem is there are bunches
                     * of maps/players already out there with items that have spells
                     * which haven't had the randomitems set to null yet.
                     * MSW 2004-05-13
                     *
                     * And if it's a spellbook, it's better to set randomitems to NULL too,
                     * else you get two spells in the book ^_-
                     * Ryo 2004-08-16
                     */
                    if (tmp->type == WAND
                    || tmp->type == ROD
                    || tmp->type == SCROLL
                    || tmp->type == FIREWALL
                    || tmp->type == POTION
                    || tmp->type == ALTAR
                    || tmp->type == SPELLBOOK)
                        tmp->randomitems = NULL;
                }

                if (QUERY_FLAG(tmp, FLAG_AUTO_APPLY))
                    apply_auto(tmp);
                else if ((tmp->type == TREASURE || (tmp->type == CONTAINER)) && HAS_RANDOM_ITEMS(tmp)) {
                    while ((tmp->stats.hp--) > 0)
                        create_treasure(tmp->randomitems, tmp, 0, m->difficulty, 0);
                    tmp->randomitems = NULL;
                } else if (tmp->type == TIMED_GATE) {
                    object *head = HEAD(tmp);

                    if (QUERY_FLAG(head, FLAG_IS_LINKED)) {
                        tmp->speed = 0;
                        object_update_speed(tmp);
                    }
                    /* This function can be called everytime a map is loaded, even when
                     * swapping back in.  As such, we don't want to create the treasure
                     * over and ove again, so after we generate the treasure, blank out
                     * randomitems so if it is swapped in again, it won't make anything.
                     * This is a problem for the above objects, because they have counters
                     * which say how many times to make the treasure.
                     */
                } else if (tmp
                && tmp->arch
                && tmp->type != PLAYER
                && tmp->type != TREASURE
                && tmp->type != SPELL
                && tmp->type != PLAYER_CHANGER
                && tmp->type != CLASS
                && HAS_RANDOM_ITEMS(tmp)) {
                    create_treasure(tmp->randomitems, tmp, 0, m->difficulty, 0);
                    tmp->randomitems = NULL;
                }
            } FOR_MAP_FINISH();

    for (x = 0; x < MAP_WIDTH(m); x++)
        for (y = 0; y < MAP_HEIGHT(m); y++)
            FOR_MAP_PREPARE(m, x, y, tmp) {
                if (tmp->above
                && (tmp->type == TRIGGER_BUTTON || tmp->type == TRIGGER_PEDESTAL))
                    check_trigger(tmp, tmp->above);
            } FOR_MAP_FINISH();
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * Dummy functions to link the library.
 */

void draw_ext_info(int, int, const object *, uint8_t, uint8_t, const char *txt) {
    fprintf(logfile, "%s\n", txt);
}

void draw_ext_info_format(int, int, const object *, uint8_t, uint8_t, const char *format, ...) {
    va_list ap;

    va_start(ap, format);
    vfprintf(logfile, format, ap);
    va_end(ap);
}

void ext_info_map(int, const mapstruct *, uint8_t, uint8_t, const char *str1) {
    fprintf(logfile, "ext_info_map: %s\n", str1);
}

void move_firewall(object *) {
}

void emergency_save(int) {
}

void clean_tmp_files(void) {
}

void esrv_send_item(object *, object *) {
}

void dragon_ability_gain(object *, int, int) {
}

void set_darkness_map(mapstruct *) {
}

object *find_skill_by_number(object *, int) {
    return NULL;
}

void esrv_del_item(player *, object *) {
}

void esrv_update_item(int, object *, object *) {
}

void esrv_update_spells(player *) {
}

void rod_adjust(object *) {
}

/*
 * This a modified version of apply_auto: BOOK are not generated, so they don't pollute
 * the readable list.
 */
int apply_auto(object *op) {
    object *tmp = NULL;
    int i;

    switch (op->type) {
    case SHOP_FLOOR:
        if (!HAS_RANDOM_ITEMS(op))
            return 0;
        do {
            i = 10; /* let's give it 10 tries */
            while ((tmp = generate_treasure(op->randomitems, op->stats.exp ? (int)op->stats.exp : MAX(op->map->difficulty, 5))) == NULL && --i)
                ;
            if (tmp == NULL)
                return 0;
            if (QUERY_FLAG(tmp, FLAG_CURSED) || QUERY_FLAG(tmp, FLAG_DAMNED) || tmp->type == BOOK) {
                object_free_drop_inventory(tmp);
                tmp = NULL;
            }
        } while (!tmp);
        SET_FLAG(tmp, FLAG_UNPAID);
        object_insert_in_map_at(tmp, op->map, NULL, 0, op->x, op->y);
        CLEAR_FLAG(op, FLAG_AUTO_APPLY);
        tmp = identify(tmp);
        break;

    case TREASURE:
        if (QUERY_FLAG(op, FLAG_IS_A_TEMPLATE))
            return 0;

        while ((op->stats.hp--) > 0)
            create_treasure(op->randomitems, op, 0, op->stats.exp ? (int)op->stats.exp : op->map == NULL ? 14 : op->map->difficulty, 0);

        /* If we generated an object and put it in this object inventory,
         * move it to the parent object as the current object is about
         * to disappear.  An example of this item is the random_ *stuff
         * that is put inside other objects.
         */
        FOR_INV_PREPARE(op, tmp) {
            object_remove(tmp);
            if (op->env && tmp->type != BOOK)
                object_insert_in_ob(tmp, op->env);
            else
                object_free_drop_inventory(tmp);
            }
        FOR_INV_FINISH();
        object_remove(op);
        object_free_drop_inventory(op);
        break;
    }
    return tmp ? 1 : 0;
}

void apply_auto_fix(mapstruct *) {
}

SockList *player_get_delayed_buffer(player *) {
    return NULL;
}

player *find_player_partial_name(const char *) {
    return NULL;
}

Account_Char *account_char_load(const char *) {
    return NULL;
}

void account_char_save(const char *, Account_Char *) {
}

void account_char_free(Account_Char *) {
}

void command_help(object *, const char *) {
}

#endif /* dummy DOXYGEN_SHOULD_SKIP_THIS */

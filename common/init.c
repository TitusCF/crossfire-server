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
 * Basic initialization for the common library.
 */

#define EXTERN
#define INIT_C

#include "global.h"

#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "output_file.h"
#include "assets.h"

static void init_environ(void);
static void init_defaults(void);
static void init_dynamic(void);
static void init_clocks(void);

/*
 * Anything with non-zero defaults in include/global.h must be set here.
 */
struct Settings settings = {
    .csport = CSPORT,
    .debug = llevInfo,
    .confdir = CONFDIR,
    .datadir = DATADIR,
    .localdir = LOCALDIR,
    .playerdir = PLAYERDIR,
    .mapdir = MAPDIR,
    .regions = REGIONS,
    .uniquedir = UNIQUE_DIR,
    .templatedir = TEMPLATE_DIR,
    .tmpdir = TMPDIR,
    .stat_loss_on_death = STAT_LOSS_ON_DEATH,
    .pk_luck_penalty = PK_LUCK_PENALTY,
    .permanent_exp_ratio = PERMANENT_EXPERIENCE_RATIO,
    .death_penalty_ratio = DEATH_PENALTY_RATIO,
    .death_penalty_level = DEATH_PENALTY_LEVEL,
    .balanced_stat_loss = BALANCED_STAT_LOSS,
    .not_permadeth = NOT_PERMADETH,
    .simple_exp = SIMPLE_EXP,
    .reset_loc_time = RESET_LOCATION_TIME,
    .set_title = SET_TITLE,
    .resurrection = RESURRECTION,
    .search_items = SEARCH_ITEMS,
    .spell_encumbrance = SPELL_ENCUMBRANCE,
    .spell_failure_effects = SPELL_FAILURE_EFFECTS,
    .casting_time = CASTING_TIME,
    .real_wiz = REAL_WIZ,
    .recycle_tmp_maps = RECYCLE_TMP_MAPS,
    .always_show_hp = ALWAYS_SHOW_HP,
    .spellpoint_level_depend = SPELLPOINT_LEVEL_DEPEND,
    .set_friendly_fire = SET_FRIENDLY_FIRE,
    .motd = MOTD,
    .rules = "rules",
    .news = "news",
    .emergency_x = EMERGENCY_X,
    .emergency_y = EMERGENCY_Y,
    .item_power_factor = 1.0,
    /* Armor enchantment stuff */
    .armor_max_enchant = ARMOR_MAX_ENCHANT,
    .armor_weight_reduction = ARMOR_WEIGHT_REDUCTION,
    .armor_weight_linear = ARMOR_WEIGHT_LINEAR,
    .armor_speed_improvement = ARMOR_SPEED_IMPROVEMENT,
    .armor_speed_linear = ARMOR_SPEED_LINEAR,
    .no_player_stealing = 1,
    .create_home_portals = 0,
    .personalized_blessings = 1,
    .pk_max_experience = 5000000,
    .pk_max_experience_percent = 10,
    .starting_stat_min = 3,
    .starting_stat_max = 18,
    .starting_stat_points = 85,
    .roll_stat_points = 115,
    .max_stat = 0,     /* max_stat - will be loaded from stats file */
    .special_break_map = 1,     /* special_break_map, 1 for historical reasons */
    .hooks_count = 0,
    .ignore_assets_errors = 0,
    .archetypes_tracker = NULL,
};

struct Statistics statistics;

/**
 * Perhaps not the best place for this, but needs to be
 * in some file in the common area so that standalone
 * programs, like the random map generator, can be built.
 */
const char *const spellpathnames[NRSPELLPATHS] = {
    "Protection",
    "Fire",
    "Frost",
    "Electricity",
    "Missiles",
    "Self",
    "Summoning",
    "Abjuration",
    "Restoration",
    "Detonation",
    "Mind",
    "Creation",
    "Teleportation",
    "Information",
    "Transmutation",
    "Transferrence",
    "Turning",
    "Wounding",
    "Death",
    "Light"
};


/**
 * This loads the emergency map information from a
 * .emergency file in the map directory.  Doing this makes
 * it easier to switch between map distributions (don't need
 * to recompile.  Note that there is no reason I see that
 * this could not be re-loaded during play, but it seems
 * like there should be little reason to do that.
 *
 * @note
 * If file doesn't exist, will not do anything.
 */
static void init_emergency_mappath(void) {
    char filename[MAX_BUF], tmpbuf[MAX_BUF];
    FILE *fp;
    int online = 0;

    settings.emergency_mapname = strdup_local(EMERGENCY_MAPPATH);

    /* If this file doesn't exist, not a big deal */
    snprintf(filename, sizeof(filename), "%s/%s/.emergency", settings.datadir, settings.mapdir);
    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (fgets(tmpbuf, MAX_BUF-1, fp)) {
            if (tmpbuf[0] == '#')
                continue; /* ignore comments */

            if (online == 0) {
                tmpbuf[strlen(tmpbuf)-1] = 0; /* kill newline */
                free(settings.emergency_mapname);
                settings.emergency_mapname = strdup_local(tmpbuf);
            } else if (online == 1) {
                settings.emergency_x = atoi(tmpbuf);
            } else if (online == 2) {
                settings.emergency_y = atoi(tmpbuf);
            }
            online++;
            if (online > 2)
                break;
        }
        fclose(fp);
        if (online <= 2)
            LOG(llevError, "Online read partial data from %s\n", filename);
        LOG(llevDebug, "emergency map set to %s (%d, %d)\n",
                settings.emergency_mapname,
                settings.emergency_x, settings.emergency_y);
    }
}


/**
 * It is vital that init_library() is called by any functions
 * using this library.
 * If you want to lessen the size of the program using the library,
 * you can replace the call to init_library() with init_globals() and
 * init_function_pointers().  Good idea to also call
 * init_hash_table if you are doing any object loading.
 */
void init_library(void) {
    init_environ();
    init_globals();
    init_stats(FALSE);   /* Needs to be fairly early, since the loader will check
                          * against the settings.max_stat value
                          */

    for (int mess = 0; mess < MAXATTACKMESS; mess++) {
        for (int level = 0; level < MAXATTACKMESS; level++) {
            attack_mess[mess][level].level = -1;
            attack_mess[mess][level].buf1 = NULL;
            attack_mess[mess][level].buf2 = NULL;
            attack_mess[mess][level].buf3 = NULL;
        }
    }

    assets_init();
    init_hash_table();
    i18n_init();
    init_objects();
    init_block();

    assets_collect(settings.datadir, ASSETS_ALL);
    assets_end_load();

    init_clocks();
    init_emergency_mappath();
    init_experience();

    if (assets_dump_undefined() > 0 && !settings.ignore_assets_errors) {
        LOG(llevError, "Assets errors, please fix and restart.\n");
        exit(EXIT_FAILURE);
    }

    /* init_dynamic() loads a map, so needs a region */
    if (init_regions() != 0) {
        LOG(llevError, "Please check that your maps are correctly installed.\n");
        exit(EXIT_FAILURE);
    }

    init_dynamic();
}

/**
 * Initializes values from the environmental variables.
 * it needs to be called very early, since command line options should
 * overwrite these if specified.
 */
static void init_environ(void) {
    char *cp;

    cp = getenv("CROSSFIRE_LIBDIR");
    if (cp)
        settings.datadir = cp;
    cp = getenv("CROSSFIRE_LOCALDIR");
    if (cp)
        settings.localdir = cp;
    cp = getenv("CROSSFIRE_PLAYERDIR");
    if (cp)
        settings.playerdir = cp;
    cp = getenv("CROSSFIRE_MAPDIR");
    if (cp)
        settings.mapdir = cp;
    cp = getenv("CROSSFIRE_UNIQUEDIR");
    if (cp)
        settings.uniquedir = cp;
    cp = getenv("CROSSFIRE_TEMPLATEDIR");
    if (cp)
        settings.templatedir = cp;
    cp = getenv("CROSSFIRE_TMPDIR");
    if (cp)
        settings.tmpdir = cp;
}

/**
 * Initialises all global variables.
 * Might use environment-variables as default for some of them.
 *
 * Setups logfile, and such variables.
 */
void init_globals(void) {
    memset(&statistics, 0, sizeof(struct Statistics));

    /* Log to stderr by default. */
    logfile = stderr;

    /* Try to open the log file specified on the command-line. */
    if (settings.logfilename != NULL) {
        logfile = fopen(settings.logfilename, "a");

        /* If writable, set buffer mode to per-line. */
        if (logfile != NULL) {
            setvbuf(logfile, NULL, _IOLBF, 0);
        } else {
            logfile = stderr;

            LOG(llevError, "Could not open '%s' for logging.\n",
                    settings.logfilename);
        }
    }

    exiting = 0;
    first_player = NULL;
    first_friendly_object = NULL;
    first_map = NULL;
    first_artifactlist = NULL;
    *first_map_ext_path = 0;
    nrofartifacts = 0;
    nrofallowedstr = 0;
    ring_arch = NULL;
    amulet_arch = NULL;
    undead_name = add_string("undead");
    trying_emergency_save = 0;
    init_defaults();
}

/**
 * Cleans all memory allocated for global variables.
 *
 * Will clear:
 *  * attack messages
 *  * emergency map settings
 *  * friendly list
 *  * experience
 *  * regions
 */
void free_globals(void) {
    int msg, attack;
    objectlink *friend;
    region *reg;

    FREE_AND_CLEAR_STR(undead_name);
    for (msg = 0; msg < NROFATTACKMESS; msg++)
        for (attack = 0; attack < MAXATTACKMESS; attack++) {
            free(attack_mess[msg][attack].buf1);
            free(attack_mess[msg][attack].buf2);
            free(attack_mess[msg][attack].buf3);
        }

    free(settings.emergency_mapname);

    while (first_friendly_object) {
        friend = first_friendly_object->next;
        FREE_AND_CLEAR(first_friendly_object);
        first_friendly_object = friend;
    }

    free_experience();

    while (first_region) {
        reg = first_region->next;
        FREE_AND_CLEAR(first_region->name);
        FREE_AND_CLEAR(first_region->parent_name);
        FREE_AND_CLEAR(first_region->jailmap);
        FREE_AND_CLEAR(first_region->msg);
        FREE_AND_CLEAR(first_region->longname);
        FREE_AND_CLEAR(first_region);
        first_region = reg;
    }

    assets_free();
}

/**
 * Initialises global variables which can be changed by options.
 * Called by init_library().
 */
static void init_defaults(void) {
    nroferrors = 0;
}

/**
 * Initializes first_map_path from the archetype collection, and check that
 * some required archetype actually exist.
 *
 * Must be called after archetypes have been initialized.
 *
 * @note
 * will call exit() in case of error.
 */
static void init_dynamic(void) {
    archetype *at = get_archetype_by_type_subtype(MAP, MAP_TYPE_LEGACY);
    if (!at) {
        LOG(llevError, "You need a archetype for a legacy map, with type %d and subtype %d\n", MAP, MAP_TYPE_LEGACY);
        exit(-1);
    }
    if (EXIT_PATH(&at->clone)) {
        mapstruct *first;

        strlcpy(first_map_path, EXIT_PATH(&at->clone), sizeof(first_map_path));
        first = ready_map_name(first_map_path, 0);
        if (!first) {
            LOG(llevError, "Initial map %s can't be found! Please ensure maps are correctly installed.\n", first_map_path);
            LOG(llevError, "Unable to continue without initial map.\n");
            fatal(SEE_LAST_ERROR);
        }
        delete_map(first);
    } else {
        LOG(llevError, "Legacy map must have a 'slaying' field!\n");
        fatal(SEE_LAST_ERROR);
    }

    if (!get_archetype_by_type_subtype(MAP, MAP_TYPE_DEFAULT)) {
        LOG(llevError, "Can not find object of type MAP subtype MAP_TYPE_DEFAULT.\n");
        LOG(llevError, "Are the archetype files up to date? Can not continue.\n");
        fatal(SEE_LAST_ERROR);
    }
}

/** Ingame time */
unsigned long todtick;

/**
 * Write out the current time to the file so time does not
 * reset every time the server reboots.
 */
void write_todclock(void) {
    char filename[MAX_BUF];
    FILE *fp;
    OutputFile of;

    snprintf(filename, sizeof(filename), "%s/clockdata", settings.localdir);
    fp = of_open(&of, filename);
    if (fp == NULL)
        return;
    fprintf(fp, "%lu", todtick);
    of_close(&of);
}

/**
 * Initializes the gametime and TOD counters
 * Called by init_library().
 */
static void init_clocks(void) {
    char filename[MAX_BUF];
    FILE *fp;
    static int has_been_done = 0;

    if (has_been_done)
        return;
    else
        has_been_done = 1;

    snprintf(filename, sizeof(filename), "%s/clockdata", settings.localdir);
    fp = fopen(filename, "r");
    if (fp == NULL) {
        LOG(llevError, "Can't open %s.\n", filename);
        todtick = 0;
        write_todclock();
        return;
    }
    /* Read TOD and default to 0 on failure. */
    if (fscanf(fp, "%lu", &todtick) == 1) {
        LOG(llevDebug, "clockdata: todtick is %lu\n", todtick);
        fclose(fp);
    } else {
        LOG(llevError, "Couldn't parse todtick, using default value 0\n");
        todtick = 0;
        fclose(fp);
        write_todclock();
    }
}

/**
 * Initializes the attack messages.
 * Called by init_library().
 *
 * Memory will be cleared by free_globals().
 */
void init_attackmess(BufferReader *reader, const char *filename) {
    char *buf;
    char *cp, *p;
    int mess = -1, level;
    int mode = 0, total = 0;

    level = 0;
    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#')
            continue;
        /*
         * Skip blanks -- strspn is slightly faster than a loop w/ optimization on
         * Also, note we go from the beginning of the line again, since cp was at the end.
         * While here, also skip tabs for more complete whitespace handling.
         *
         * SilverNexus 2018-01-21
         */
        cp = buf + strspn(buf, " \t");

        if (strncmp(cp, "TYPE:", 5) == 0) {
            p = strtok(buf, ":");
            p = strtok(NULL, ":");
            if (mode == 1) {
                attack_mess[mess][level].level = -1;
                free(attack_mess[mess][level].buf1);
                free(attack_mess[mess][level].buf2);
                free(attack_mess[mess][level].buf3);
                attack_mess[mess][level].buf1 = NULL;
                attack_mess[mess][level].buf2 = NULL;
                attack_mess[mess][level].buf3 = NULL;
            }
            level = 0;
            mess = atoi(p);
            mode = 1;
            continue;
        }
        if (mode == 1) {
            p = strtok(buf, "=");
            attack_mess[mess][level].level = atoi(buf);
            p = strtok(NULL, "=");
            free(attack_mess[mess][level].buf1);
            if (p != NULL)
                attack_mess[mess][level].buf1 = strdup_local(p);
            else
                attack_mess[mess][level].buf1 = strdup_local("");
            mode = 2;
            continue;
        } else if (mode == 2) {
            p = strtok(buf, "=");
            attack_mess[mess][level].level = atoi(buf);
            p = strtok(NULL, "=");
            free(attack_mess[mess][level].buf2);
            if (p != NULL)
                attack_mess[mess][level].buf2 = strdup_local(p);
            else
                attack_mess[mess][level].buf2 = strdup_local("");
            mode = 3;
            continue;
        } else if (mode == 3) {
            p = strtok(buf, "=");
            attack_mess[mess][level].level = atoi(buf);
            p = strtok(NULL, "=");
            free(attack_mess[mess][level].buf3);
            if (p != NULL)
                attack_mess[mess][level].buf3 = strdup_local(p);
            else
                attack_mess[mess][level].buf3 = strdup_local("");
            mode = 1;
            level++;
            total++;
            continue;
        }
    }
    LOG(llevDebug, "attackmsg %s: %d messages in %d categories\n", filename, total, mess+1);
}

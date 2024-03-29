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
 * Server initialization, settings, and command-line handling.
 */

#include "global.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

/* Needed for strcasecmp(). */
#ifndef WIN32
#include <strings.h>
#endif

#include "loader.h"
#include "version.h"
#include "server.h"
#include "sproto.h"
#include "assets.h"
#include "modules.h"

static void help(void);
static void init_beforeplay(void);
static void init_startup(void);

/** If set after command line argument parsing, then the server will exit. */
static int should_exit = 0;

typedef struct module_information {
    const char *name;           /**< Module name, without space. */
    char const *description;    /**< Module long description. */
    bool enabled;               /**< Whether the module is enabled or not. */
    void (*init)();             /**< Initialisation function. */
    void (*close)();            /**< Cleanup function. */
} module_information;

/** All built modules. */
static module_information modules[] = {
    { "citybell", "Ring bells every hour for defined temples", true, cfcitybell_init, cfcitybell_close },
    { "citylife", "Add NPCs in towns", true, citylife_init, citylife_close },
    { "rhg", "Add random maps to exits in towns", false, random_house_generator_init, random_house_generator_close },
    { NULL, NULL, false, NULL, NULL }
};

/**
 * Init all modules which are not disabled.
 */
void init_modules() {
    LOG(llevInfo, "Initializing modules\n");
    for (int module = 0; modules[module].name != NULL; module++) {
        module_information *mod = &modules[module];
        if (!mod->enabled) {
            LOG(llevInfo, "  %s (%s): disabled\n", mod->name, mod->description);
        } else {
            mod->init(&settings);
            LOG(llevInfo, "  %s (%s): activated\n", mod->name, mod->description);
        }
    }
}

/**
 * Clean up all modules which are not disabled.
 */
void close_modules() {
    LOG(llevInfo, "Cleaning modules\n");
    for (int module = 0; modules[module].name != NULL; module++) {
        module_information *mod = &modules[module];
        if (mod->enabled) {
            mod->close(&settings);
            LOG(llevInfo, "  %s (%s): closed\n", mod->name, mod->description);
        }
    }
}

/**
 * List all modules, then exit.
 */
static void list_modules() {
    LOG(llevInfo, "Built-in modules:\n");
    for (int module = 0; modules[module].name != NULL; module++) {
        LOG(llevInfo, " %s: %s -> %s\n", modules[module].name, modules[module].description, modules[module].enabled ? "enabled" : "disabled");
    }
    should_exit = 1;
}

/**
 * Command line option: set logfile name.
 * @param val new name.
 */
static void set_logfile(char *val) {
    settings.logfilename = val;
}

/** Command line option: show version. */
static void call_version(void) {
    puts(FULL_VERSION);
    exit(EXIT_SUCCESS);
}

/** Command line option: debug flag. */
static void set_debug(void) {
    settings.debug = llevDebug;
}

/** Command line option: unset debug flag. */
static void unset_debug(void) {
    settings.debug = llevInfo;
}

/** Command line option: monster debug flag. */
static void set_mondebug(void) {
    settings.debug = llevMonster;
}

/** Command line option: dump monsters. */
static void set_dumpmon1(void) {
    settings.dumpvalues = 1;
}

/** Command line option: dump abilities. */
static void set_dumpmon2(void) {
    settings.dumpvalues = 2;
}

/** Command line option: dump artifacts. */
static void set_dumpmon3(void) {
    settings.dumpvalues = 3;
}

/** Command line option: dump spells. */
static void set_dumpmon4(void) {
    settings.dumpvalues = 4;
}

/** Command line option: ? */
static void set_dumpmon5(void) {
    settings.dumpvalues = 5;
}

/** Command line option: dump races. */
static void set_dumpmon6(void) {
    settings.dumpvalues = 6;
}

/** Command line option: dump alchemy. */
static void set_dumpmon7(void) {
    settings.dumpvalues = 7;
}

/** Command line option: dump gods. */
static void set_dumpmon8(void) {
    settings.dumpvalues = 8;
}

/** Command line option: dump alchemy costs. */
static void set_dumpmon9(void) {
    settings.dumpvalues = 9;
}

/**
 * Command line option: dump monster treasures.
 * @param name monster's name to dump treasure for.
 */
static void set_dumpmont(const char *name) {
    settings.dumpvalues = 10;
    settings.dumparg = name;
}

/**
 * Command line option: set data path.
 * @param path new path.
 */
static void set_datadir(const char *path) {
    settings.datadir = path;
}

/**
 * Command line option: set configuration path.
 * @param path new path.
 */
static void set_confdir(const char *path) {
    settings.confdir = path;
}

/**
 * Command line option: set local path.
 * @param path new path.
 */
static void set_localdir(const char *path) {
    settings.localdir = path;
}

/**
 * Command line option: set map path.
 * @param path new path.
 */
static void set_mapdir(const char *path) {
    settings.mapdir = path;
}

/**
 * Command line option: set regions file name.
 * @param path new name.
 */
static void set_regions(const char *path) {
    settings.regions = path;
}

/**
 * Command line option: set unique path.
 * @param path new path.
 */
static void set_uniquedir(const char *path) {
    settings.uniquedir = path;
}

/**
 * Command line option: set template path.
 * @param path new path.
 */
static void set_templatedir(const char *path) {
    settings.templatedir = path;
}

/**
 * Command line option: set player path.
 * @param path new path.
 */
static void set_playerdir(const char *path) {
    settings.playerdir = path;
}

/**
 * Command line option: set temporary file path.
 * @param path new path.
 */
static void set_tmpdir(const char *path) {
    settings.tmpdir = path;
}

/**
 * Command line option: ignore assets errors.
 */
static void set_ignore_assets_errors() {
    settings.ignore_assets_errors = 1;
}

static void server_pack_assets(const char *assets, const char *filename) {
    assets_pack(assets, filename);
    should_exit = 1;
}

static void free_materials(void);

/**
 * Change the server's port. Will exit() if invalid value.
 *
 * @param val
 * port to use. Must be a valid one, between 1 and 65535 inclusive.
 */
static void set_csport(const char *val) {
    int port = atoi(val);
    if (port <= 0 || port > 65535) {
        LOG(llevError, "%d is an invalid csport number, must be between 1 and 65535.\n", port);
        exit(1);
    }
    settings.csport = port;
}

/**
 * Disable a plugin.
 * @param name plugin's name, without extension.
 */
static void set_disable_plugin(const char *name) {
    linked_char *disable = calloc(1, sizeof(linked_char));
    disable->next = settings.disabled_plugins;
    disable->name = strdup(name);
    settings.disabled_plugins = disable;
}

/**
 * Change the 'enabled' flag of a module.
 * @param name module's name, or 'All'.
 * @param enabled new value of the 'enabled' flag.
 * @note will fatal() if the module name is invalid.
 */
static void do_module(const char *name, bool enabled) {
    bool one = false;
    for (int module = 0; modules[module].name; module++) {
        if (strcmp("All", name) == 0 || strcmp(modules[module].name, name) == 0) {
            modules[module].enabled = enabled;
            one = true;
        }
    }
    if (!one) {
        LOG(llevError, "Invalid module name %s\n", name);
        fatal(SEE_LAST_ERROR);
    }
}

/**
 * Disable a module.
 * @param name module's name, or 'All'.
 */
static void set_disable_module(const char *name) {
    do_module(name, false);
}

/**
 * Enable a module.
 * @param name module's name, or 'All'.
 */
static void set_enable_module(const char *name) {
    do_module(name, true);
}

/**
 * Dump all animations, then exit.
 */
static void server_dump_animations(void) {
    dump_animations();
    cleanup();
}

/**
 * Dump all faces, then exit.
 */
static void server_dump_faces(void) {
    dump_faces();
    cleanup();
}

/** Typedefs used when calling option handlers. */
/*@{*/
typedef void (*cmdlinefunc_args0)(void);
typedef void (*cmdlinefunc_args1)(const char* arg1);
typedef void (*cmdlinefunc_args2)(const char* arg1, const char* arg2);
/*@}*/

/**
 * One command line option definition.
 * Most of this is shamelessly stolen from XSysStats.  But since that is
 * also my program, no problem.
 */
struct Command_Line_Options {
    const char *cmd_option; /**< How it is called on the command line. */
    uint8_t num_args;         /**< Number or args it takes. */
    uint8_t pass;             /**< What pass this should be processed on. @todo describe passes :) */
    void (*func)();         /**< function to call when we match this.
                             * if num_args is true, than that gets passed
                             * to the function, otherwise nothing is passed
                             */
};

/**
 * Actual valid command line options.
 * The way this system works is pretty simple - parse_args takes
 * the options passed to the program and a pass number.  If an option
 * matches both in name and in pass (and we have enough options),
 * we call the associated function.  This makes writing a multi
 * pass system very easy, and it is very easy to add in new options.
 */
static struct Command_Line_Options options[] = {
    /** Pass 1 functions - Stuff that can/should be called before we actually
     * initialize any data.
     */
    { "-conf", 1, 1, set_confdir },
    { "-d", 0, 1, set_debug },
    { "-data", 1, 1, set_datadir },
    { "-disable-plugin", 1, 1, set_disable_plugin },
    { "-disable-module", 1, 1, set_disable_module },
    { "-enable-module", 1, 1, set_enable_module },
    { "-list-modules", 0, 1, list_modules },
    { "-h", 0, 1, help },
    { "-ignore-assets-errors", 0, 1, set_ignore_assets_errors },
    { "-local", 1, 1, set_localdir },
    { "-log", 1, 1, set_logfile },
    { "-maps", 1, 1, set_mapdir },
    { "-mon", 0, 1, set_mondebug },
    { "-n", 0, 1, unset_debug },
    { "-playerdir", 1, 1, set_playerdir },
    { "-regions", 1, 1, set_regions },
    { "-templatedir", 1, 1, set_templatedir },
    { "-tmpdir", 1, 1, set_tmpdir },
    { "-uniquedir", 1, 1, set_uniquedir },
    { "-v", 0, 1, call_version },

#ifdef WIN32
    /* Windows service stuff */
    { "-regsrv", 0, 1, service_register },
    { "-unregsrv", 0, 1, service_unregister },
    { "-srv", 0, 1, service_handle },
#endif

    /** Pass 2 functions.  Most of these could probably be in pass 1,
     * as they don't require much of anything to bet set up.
     */
    { "-p", 1, 2, set_csport },

    /** Start of pass 3 information. In theory, by pass 3, all data paths
     * and defaults should have been set up.
     */
    { "-m", 0, 3, set_dumpmon1 },
    { "-m2", 0, 3, set_dumpmon2 },
    { "-m3", 0, 3, set_dumpmon3 },
    { "-m4", 0, 3, set_dumpmon4 },
    { "-m5", 0, 3, set_dumpmon5 },
    { "-m6", 0, 3, set_dumpmon6 },
    { "-m7", 0, 3, set_dumpmon7 },
    { "-m8", 0, 3, set_dumpmon8 },
    { "-m9", 0, 3, set_dumpmon9 },
    { "-mt", 1, 3, set_dumpmont },
    { "-mexp", 0, 3, dump_experience },
    { "-mq", 0, 3, dump_quests },
    { "-dump-anims", 0, 3, server_dump_animations },
    { "-dump-faces", 0, 3, server_dump_faces },
    { "-pack-assets", 2, 3, server_pack_assets },
};

/**
 * Parse command line arguments.
 *
 * Note since this may be called before the library has been set up,
 * we don't use any of crossfires built in logging functions.
 *
 * @param argc
 * length of argv.
 * @param argv
 * arguments.
 * @param pass
 * initialization pass arguments to use.
 * @todo describe pass.
 */
static void parse_args(int argc, char *argv[], int pass) {
    size_t i;
    int on_arg = 1;

    while (on_arg < argc) {
        for (i = 0; i < sizeof(options)/sizeof(struct Command_Line_Options); i++) {
            if (!strcmp(options[i].cmd_option, argv[on_arg])) {
                /* Found a matching option, but should not be processed on
                 * this pass.  Just skip over it
                 */
                if (options[i].pass != pass) {
                    on_arg += options[i].num_args+1;
                    break;
                }
                if (options[i].num_args) {
                    if ((on_arg+options[i].num_args) >= argc) {
                        fprintf(stderr, "%s requires an argument.\n", options[i].cmd_option);
                        exit(1);
                    }

                    if (options[i].num_args == 1)
                        ((cmdlinefunc_args1)options[i].func)(argv[on_arg+1]);
                    if (options[i].num_args == 2)
                        ((cmdlinefunc_args2)options[i].func)(argv[on_arg+1], argv[on_arg+2]);
                    on_arg += options[i].num_args+1;
                } else { /* takes no args */
                    ((cmdlinefunc_args0)options[i].func)();
                    on_arg++;
                }
                break;
            }
        }
        if (i == sizeof(options)/sizeof(struct Command_Line_Options)) {
            fprintf(stderr, "Unknown option: %s\n", argv[on_arg]);
            fprintf(stderr, "Type '%s -h' for usage.\n", argv[0]);
            exit(1);
        }
    }

    if (should_exit) {
        cleanup();
    }
}

/**
 * Creates an empty materialtype_t structure.
 *
 * @return
 * new blanked structure.
 * @note
 * will fatal() instead of returning NULL.
 */
static materialtype_t *get_empty_mat(void) {
    materialtype_t *mt;
    int i;

    mt = (materialtype_t *)malloc(sizeof(materialtype_t));
    if (mt == NULL)
        fatal(OUT_OF_MEMORY);
    mt->name = NULL;
    mt->description = NULL;
    for (i = 0; i < NROFATTACKS; i++) {
        mt->save[i] = 0;
        mt->mod[i] = 0;
    }
    mt->next = NULL;
    return mt;
}

/**
 * Loads the materials.
 * @todo describe materials and such.
 */
static void load_materials(BufferReader *reader, const char *filename) {
    char *buf, *cp, *next;
    materialtype_t *mt;
    int i, value;
    (void)filename;

    mt = get_empty_mat();
    if (!materialt) {
        materialt = mt;
    } else {
        materialtype_t *a = materialt;
        while (a->next != NULL) {
            a = a->next;
        }
        a->next = mt;
    }

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#')
            continue;
        cp = buf;
        while (*cp == ' ') /* Skip blanks */
            cp++;
        if (!strncmp(cp, "name", 4)) {
            /* clean up the previous entry */
            if (mt->next != NULL) {
                if (mt->description == NULL)
                    mt->description = add_string(mt->name);
                mt = mt->next;
            }
            mt->next = get_empty_mat();
            mt->name = add_string(strchr(cp, ' ')+1);
        } else if (!strncmp(cp, "description", 11)) {
            mt->description = add_string(strchr(cp, ' ')+1);
        } else if (sscanf(cp, "material %d", &value)) {
            mt->material = value;
        } else if (!strncmp(cp, "saves", 5)) {
            cp = strchr(cp, ' ')+1;
            for (i = 0; i < NROFATTACKS; i++) {
                if (cp == NULL) {
                    mt->save[i] = 0;
                    continue;
                }
                if ((next = strchr(cp, ',')) != NULL)
                    *(next++) = '\0';
                sscanf(cp, "%d", &value);
                mt->save[i] = (int8_t)value;
                cp = next;
            }
        } else if (!strncmp(cp, "mods", 4)) {
            cp = strchr(cp, ' ')+1;
            for (i = 0; i < NROFATTACKS; i++) {
                if (cp == NULL) {
                    mt->save[i] = 0;
                    continue;
                }
                if ((next = strchr(cp, ',')) != NULL)
                    *(next++) = '\0';
                sscanf(cp, "%d", &value);
                mt->mod[i] = (int8_t)value;
                cp = next;
            }
        }
    }
    free(mt->next);
    mt->next = NULL;
    LOG(llevDebug, "loaded material type data\n");
}

/**
 * Frees all memory allocated to materials.
 */
static void free_materials(void) {
    materialtype_t *next;

    while (materialt) {
        next = materialt->next;
        free(materialt);
        materialt = next;
    }
    materialt = NULL;
}

/**
 * This loads the settings file.  There could be debate whether this should
 * be here or in the common directory - but since only the server needs this
 * information, having it here probably makes more sense.
 */
static void load_settings(void) {
    char buf[MAX_BUF], *cp, dummy[1];
    int has_val;
    FILE *fp;

    dummy[0] = '\0';
    snprintf(buf, sizeof(buf), "%s/settings", settings.confdir);

    /* We don't require a settings file at current time, but down the road,
     * there will probably be so many values that not having a settings file
     * will not be a good thing.
     */
    if ((fp = fopen(buf, "r")) == NULL) {
        LOG(llevError, "Warning: No settings file found\n");
        return;
    }
    while (fgets(buf, MAX_BUF-1, fp) != NULL) {
        if (buf[0] == '#')
            continue;
        /* eliminate newline */
        if ((cp = strrchr(buf, '\n')) != NULL)
            *cp = '\0';

        /* Skip over empty lines */
        if (buf[0] == 0)
            continue;

        /* Skip all the spaces and set them to nulls.  If not space,
         * set cp to "" to make strcpy's and the like easier down below.
         */
        if ((cp = strchr(buf, ' ')) != NULL) {
            while (*cp == ' ')
                *cp++ = 0;
            has_val = 1;
        } else {
            cp = dummy;
            has_val = 0;
        }

        if (!strcasecmp(buf, "metaserver_notification")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.meta_on = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.meta_on = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for metaserver_notification: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "metaserver_server")) {
            if (has_val)
                safe_strncpy(settings.meta_server, cp, sizeof(settings.meta_server));
            else
                LOG(llevError, "load_settings: metaserver_server must have a value.\n");
        } else if (!strcasecmp(buf, "motd")) {
            if (has_val)
                safe_strncpy(settings.motd, cp, sizeof(settings.motd));
            else
                LOG(llevError, "load_settings: motd must have a value.\n");
        } else if (!strcasecmp(buf, "metaserver_host")) {
            if (has_val)
                safe_strncpy(settings.meta_host, cp, sizeof(settings.meta_host));
            else
                LOG(llevError, "load_settings: metaserver_host must have a value.\n");
        } else if (!strcasecmp(buf, "port")) {
            set_csport(cp);
        } else if (!strcasecmp(buf, "metaserver_port")) {
            int port = atoi(cp);

            if (port < 1 || port > 65535)
                LOG(llevError, "load_settings: metaserver_port must be between 1 and 65535, %d is invalid\n", port);
            else
                settings.meta_port = port;
        } else if (!strcasecmp(buf, "metaserver_comment")) {
            safe_strncpy(settings.meta_comment, cp, sizeof(settings.meta_comment));
        } else if (!strcasecmp(buf, "worldmapstartx")) {
            int size = atoi(cp);

            if (size < 0)
                LOG(llevError, "load_settings: worldmapstartx must be at least 0, %d is invalid\n", size);
            else
                settings.worldmapstartx = size;
        } else if (!strcasecmp(buf, "worldmapstarty")) {
            int size = atoi(cp);

            if (size < 0)
                LOG(llevError, "load_settings: worldmapstarty must be at least 0, %d is invalid\n", size);
            else
                settings.worldmapstarty = size;
        } else if (!strcasecmp(buf, "worldmaptilesx")) {
            int size = atoi(cp);

            if (size < 1)
                LOG(llevError, "load_settings: worldmaptilesx must be greater than 1, %d is invalid\n", size);
            else
                settings.worldmaptilesx = size;
        } else if (!strcasecmp(buf, "worldmaptilesy")) {
            int size = atoi(cp);

            if (size < 1)
                LOG(llevError, "load_settings: worldmaptilesy must be greater than 1, %d is invalid\n", size);
            else
                settings.worldmaptilesy = size;
        } else if (!strcasecmp(buf, "worldmaptilesizex")) {
            int size = atoi(cp);

            if (size < 1)
                LOG(llevError, "load_settings: worldmaptilesizex must be greater than 1, %d is invalid\n", size);
            else
                settings.worldmaptilesizex = size;
        } else if (!strcasecmp(buf, "worldmaptilesizey")) {
            int size = atoi(cp);

            if (size < 1)
                LOG(llevError, "load_settings: worldmaptilesizey must be greater than 1, %d is invalid\n", size);
            else
                settings.worldmaptilesizey = size;
        } else if (!strcasecmp(buf, "fastclock")) {
            int lev = atoi(cp);

            if (lev < 0)
                LOG(llevError, "load_settings: fastclock must be at least 0, %d is invalid\n", lev);
            else
                settings.fastclock = lev;
        } else if (!strcasecmp(buf, "not_permadeth")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.not_permadeth = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.not_permadeth = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for not_permadeth: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "resurrection")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.resurrection = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.resurrection = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for resurrection: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "set_title")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.set_title = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.set_title = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for set_title: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "search_items")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.search_items = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.search_items = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for search_items: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "spell_encumbrance")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.spell_encumbrance = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.spell_encumbrance = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for spell_encumbrance: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "spell_failure_effects")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.spell_failure_effects = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.spell_failure_effects = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for spell_failure_effects: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "casting_time")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.casting_time = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.casting_time = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for casting_time: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "real_wiz")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.real_wiz = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.real_wiz = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for real_wiz: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "recycle_tmp_maps")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.recycle_tmp_maps = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.recycle_tmp_maps = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for recycle_tmp_maps: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "always_show_hp")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.always_show_hp = 2;
            } else if (!strcasecmp(cp, "damaged")) {
                settings.always_show_hp = 1;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.always_show_hp = 0;
            } else {
                LOG(llevError, "load_settings: Unknown value for always_show_hp: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "who_format")) {
            if (has_val)
                safe_strncpy(settings.who_format, cp,
                        sizeof(settings.who_format));
        } else if (!strcasecmp(buf, "who_wiz_format")) {
            if (has_val) {
                safe_strncpy(settings.who_wiz_format, cp,
                        sizeof(settings.who_wiz_format));
            }
        } else if (!strcasecmp(buf, "spellpoint_level_depend")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.spellpoint_level_depend = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.spellpoint_level_depend = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for spellpoint_level_depend: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "stat_loss_on_death")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.stat_loss_on_death = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.stat_loss_on_death = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for stat_loss_on_death: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "use_permanent_experience")) {
            LOG(llevError, "use_permanent_experience is deprecated, usepermenent_experience_percentage instead\n");
        } else if (!strcasecmp(buf, "permanent_experience_percentage")) {
            int val = atoi(cp);
            if (val < 0 || val > 100)
                LOG(llevError, "load_settings: permenent_experience_percentage must be between 0 and 100, %d is invalid\n", val);
            else
                settings.permanent_exp_ratio = val;
        } else if (!strcasecmp(buf, "death_penalty_percentage")) {
            int val = atoi(cp);
            if (val < 0 || val > 100)
                LOG(llevError, "load_settings: death_penalty_percentage must be between 0 and 100, %d is invalid\n", val);
            else
                settings.death_penalty_ratio = val;
        } else if (!strcasecmp(buf, "death_penalty_levels")) {
            int val = atoi(cp);
            if (val < 0 || val > 255)
                LOG(llevError, "load_settings: death_penalty_levels can not be negative, %d is invalid\n", val);
            else
                settings.death_penalty_level = val;
        } else if (!strcasecmp(buf, "balanced_stat_loss")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.balanced_stat_loss = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.balanced_stat_loss = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for balanced_stat_loss: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "simple_exp")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.simple_exp = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.simple_exp = FALSE;
            } else {
                LOG(llevError, "load_settings: Unknown value for simple_exp: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "item_power_factor")) {
            float tmp = atof(cp);
            if (tmp < 0)
                LOG(llevError, "load_settings: item_power_factor must be a positive number (%f < 0)\n", tmp);
            else
                settings.item_power_factor = tmp;
        } else if (!strcasecmp(buf, "pk_luck_penalty")) {
            int16_t val = atoi(cp);

            if (val < -100 || val > 100)
                LOG(llevError, "load_settings: pk_luck_penalty must be between -100 and 100, %d is invalid\n", val);
            else
                settings.pk_luck_penalty = val;
        } else if (!strcasecmp(buf, "set_friendly_fire")) {
            int val = atoi(cp);

            if (val < 1 || val > 100)
                LOG(llevError, "load_settings: set_friendly_fire must be between 1 an 100, %d is invalid\n", val);
            else
                settings.set_friendly_fire = val;
        } else if (!strcasecmp(buf, "armor_max_enchant")) {
            int max_e = atoi(cp);
            if (max_e <= 0)
                LOG(llevError, "load_settings: armor_max_enchant is %d\n", max_e);
            else
                settings.armor_max_enchant = max_e;
        } else if (!strcasecmp(buf, "armor_weight_reduction")) {
            int wr = atoi(cp);
            if (wr < 0)
                LOG(llevError, "load_settings: armor_weight_reduction is %d\n", wr);
            else
                settings.armor_weight_reduction = wr;
        } else if (!strcasecmp(buf, "armor_weight_linear")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.armor_weight_linear = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.armor_weight_linear = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for armor_weight_linear: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "armor_speed_improvement")) {
            int wr = atoi(cp);
            if (wr < 0)
                LOG(llevError, "load_settings: armor_speed_improvement is %d\n", wr);
            else
                settings.armor_speed_improvement = wr;
        } else if (!strcasecmp(buf, "armor_speed_linear")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.armor_speed_linear = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.armor_speed_linear = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for armor_speed_linear: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "no_player_stealing")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.no_player_stealing = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.no_player_stealing = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for no_player_stealing: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "create_home_portals")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.create_home_portals = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.create_home_portals = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for create_home_portals: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "personalized_blessings")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.personalized_blessings = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.personalized_blessings = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for personalized_blessings: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "pk_max_experience")) {
            int64_t pkme = atoll(cp);
            if (pkme < 0)
                pkme = -1;
            settings.pk_max_experience = pkme;
        } else if (!strcasecmp(buf, "pk_max_experience_percent")) {
            int pkmep = atoi(cp);
            if (pkmep < 0) {
                LOG(llevError, "load_settings: pk_max_experience_percent should be positive or zero (was \"%s\")\n", cp);
            } else
                settings.pk_max_experience_percent = pkmep;
        } else if (!strcasecmp(buf, "allow_denied_spells_writing")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.allow_denied_spells_writing = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.allow_denied_spells_writing = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for allow_denied_spells_writing: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "allow_broken_converters")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.allow_broken_converters = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.allow_broken_converters = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for allow_broken_converters: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "log_timestamp")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.log_timestamp = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.log_timestamp = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for log_timestamp: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "log_timestamp_format")) {
            free(settings.log_timestamp_format);
            settings.log_timestamp_format = strdup_local(cp);
        } else if (!strcasecmp(buf, "starting_stat_min")) {
            int val = atoi(cp);

            if (val < 1 || val > settings.max_stat || val > settings.starting_stat_max)
                LOG(llevError, "load_settings: starting_stat_min (%d) need to be within %d-%d (%d)\n",
                    val, 1, settings.max_stat, settings.starting_stat_max);
            else
                settings.starting_stat_min = val;
        } else if (!strcasecmp(buf, "starting_stat_max")) {
            int val = atoi(cp);

            if (val < 1 || val > settings.max_stat || val<settings.starting_stat_min)
                LOG(llevError, "load_settings: starting_stat_max (%d) need to be within %d-%d (%d)\n",
                    val, 1, settings.max_stat, settings.starting_stat_min);
            else
                settings.starting_stat_max = val;
        } else if (!strcasecmp(buf, "starting_stat_points")) {
            int val = atoi(cp);

            if (val < NUM_STATS * settings.starting_stat_min ||
                val > NUM_STATS * settings.starting_stat_max)
                LOG(llevError, "load_settings: starting_stat_points (%d) need to be within %d-%d\n",
                    val, NUM_STATS * settings.starting_stat_min, NUM_STATS * settings.starting_stat_max);
            else
                settings.starting_stat_points = val;
        } else if (!strcasecmp(buf, "roll_stat_points")) {
            int val = atoi(cp);

            /* The 3 and 18 values are hard coded in because we know that
             * roll_stat() generates a value between 3 and 18 - if that ever
             * changed, this code should change also, but that code will eventually
             * go away.
             */
            if (val < NUM_STATS * 3 || val > NUM_STATS * 18)
                LOG(llevError, "load_settings: roll_stat_points need to be within %d-%d\n",
                    NUM_STATS * 3,  NUM_STATS * 18);
            else
                settings.roll_stat_points = val;
        } else if (!strcasecmp(buf, "special_break_map")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.special_break_map = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.special_break_map = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for special_break_map: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "ignore_plugin_compatibility")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.ignore_plugin_compatibility = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.ignore_plugin_compatibility = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for ignore_plugin_compatibility: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "account_block_create")) {
            if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
                settings.account_block_create = TRUE;
            } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
                settings.account_block_create = FALSE;
            } else {
                LOG(llevError, "load_settings: unknown value for account_block_create: %s\n", cp);
            }
        } else if (!strcasecmp(buf, "account_trusted_host")) {
            free(settings.account_trusted_host);
            settings.account_trusted_host = strdup_local(cp);
        } else if (!strcasecmp(buf, "crypt_mode")) {
            int val = atoi(cp);
            if (val != 0 && val != 1) {
                LOG(llevError, "load_settings: crypt_mode must be 0 or 1\n");
            } else {
                settings.crypt_mode = val;
            }
        } else if (!strcasecmp(buf, "min_name")) {
            int val = atoi(cp);

            if (val < 1 || val > MAX_NAME )
                LOG(llevError, "load_settings: min_name (%d) need to be within %d-%d\n",
                    val, 1, MAX_NAME);
            else
                settings.min_name = val;
        } else {
            LOG(llevError, "Unknown value in settings file: %s\n", buf);
        }
    }
    fclose(fp);
    if (settings.log_timestamp_format == NULL)
        settings.log_timestamp_format = strdup_local("%y/%m/%d %H:%M:%S");

    /*
    * The who formats are defined in config to be blank. They should have been
    * overridden by the settings file, if there are no entries however, it will
    * have stayed blank. Since this probably isn't what is wanted, we will check if
    * new formats have been specified, and if not we will use the old defaults.
    */
    if (!strcmp(settings.who_format, ""))
        strcpy(settings.who_format, "%N_%T%t%h%d%b%n<%m>");
    if (!strcmp(settings.who_wiz_format, ""))
        strcpy(settings.who_wiz_format, "%N_%T%t%h%d%b%nLevel %l <%m>(@%i)(%c)");
}

void add_server_collect_hooks() {
    settings.hooks_filename[settings.hooks_count] = "/materials";
    settings.hooks[settings.hooks_count] = load_materials;
    settings.hooks_count++;
    settings.hooks_filename[settings.hooks_count] = "/races";
    settings.hooks[settings.hooks_count] = load_races;
    settings.hooks_count++;
}

static void init_db() {
    char path[MAX_BUF];
    snprintf(path, sizeof(path), "%s/%s", settings.localdir, "server.db");
    int ret = sqlite3_open(path, &server_db);
    if (ret != SQLITE_OK) {
        LOG(llevError, "Could not open database %s: %s\n", path,
            sqlite3_errmsg(server_db));
        fatal(SEE_LAST_ERROR);
    }

    sqlite3_exec(server_db, "CREATE TABLE IF NOT EXISTS schema ('table' TEXT PRIMARY KEY, 'version' INT);", NULL, NULL, NULL);
    shop_transactions_init();
}

/**
 * This is the main server initialization function.
 *
 * Called only once, when starting the program.
 * @param argc argument count.
 * @param argv arguments on the command line.
 */
void init(int argc, char **argv) {
    logfile = stderr;

    /* First argument pass - right now it does nothing, but in the future specifying
     * the LibDir in this pass would be reasonable. */
    parse_args(argc, argv, 1);

    add_server_collect_hooks();
    init_modules();

    init_library();     /* Must be called early */
    load_settings();    /* Load the settings file */
    parse_args(argc, argv, 2);

    LOG(llevInfo, "Crossfire %s\n", FULL_VERSION);
    SRANDOM(time(NULL));

    init_startup();     /* Check shutdown/forbid files */
    init_signals();     /* Sets up signal interceptions */
    commands_init();    /* Sort command tables */
    init_db();
    read_map_log();     /* Load up the old temp map files */
    init_skills();
    init_ob_methods();
    cftimer_init();
    hiscore_init();

    parse_args(argc, argv, 3);

    init_beforeplay();
    init_server();
    metaserver2_init();
    accounts_load();
    reset_sleep();
}

/**
 * Frees all memory allocated around here:
 * - materials
 * - races
 */
void free_server(void) {
    free_materials();
    free_races();
    free_quest();
    while (settings.disabled_plugins) {
        linked_char *next = settings.disabled_plugins->next;
        free((void *)settings.disabled_plugins->name);
        free(settings.disabled_plugins);
        settings.disabled_plugins = next;
    }
}

/**
 * Display the command line options and exits.
 */
static void help(void) {
    printf("Usage: crossfire-server [options]\n\n");

    printf("Options:\n");
    printf(" -conf        Set the directory to find configuration files.\n");
    printf(" -d           Turn on extra debugging messages.\n");
    printf(" -data        Set the data (share/) directory (archetypes, treasures, etc).\n");
    printf(" -disable-module\n"
           "              Disable specified module, by its name\n"
           "              Can be specified multiple times. 'All' disables all modules.\n");
    printf(" -enable-module\n"
           "              Enable specified module, by its name\n"
           "              Can be specified multiple times. 'All' enables all modules.\n");
    printf(" -disable-plugin\n"
           "              Disables specified plugin. Use the name without the extension.\n"
           "              Can be specified multiple times. 'All' disables all plugins.\n");
    printf(" -dump-anims  Dump animations.\n");
    printf(" -h           Print this help message.\n");
    printf(" -ignore-assets-errors\n");
    printf("               Allow going on even if there are errors in assets.\n");
    printf("               Warning: this may lead to strange behaviour.\n");
    printf(" -list-modules\n"
           "              List built-in modules and exit.\n");
    printf(" -local       Set the local data (var/) directory.\n");
    printf(" -log <file>  Write logging information to the given file.\n");
    printf(" -m           List suggested experience for all monsters.\n");
    printf(" -m2          Dump monster abilities.\n");
    printf(" -m3          Dump artifact information.\n");
    printf(" -m4          Dump spell information.\n");
    printf(" -m5          Dump skill information.\n");
    printf(" -m6          Dump race information.\n");
    printf(" -m7          Dump alchemy information.\n");
    printf(" -m8          Dump gods information.\n");
    printf(" -m9          Dump more alchemy information (formula checking).\n");
    printf(" -maps        Set the map directory.\n");
    printf(" -mexp        Dump the experience table.\n");
    printf(" -mon         Turn on monster debugging.\n");
    printf(" -mq          Dump the quest list.\n");
    printf(" -mt <name>   Dump a list of treasures for a monster.\n");
    printf(" -n           Turn off debugging messages if on by default.\n");
    printf(" -p <port>    Specifies the port to listen on for incoming connections.\n");
    printf(" -pack-assets <type> <filename>\n");
    printf("              Packs specified assets type to the specified filename.\n");
    printf("              Valid assets type are: archs, treasures, faces, messages, facesets, artifacts, formulae, images, quests.\n");
    printf("              The file format will be tar ('images') or text (everything else).\n");
    printf("              It is possible to combine multiple assets by using '+', for instance 'faces+messages+artifacts'.\n");
    printf("                In this case the file will be in tar format.\n");
    printf(" -playerdir   Set the player files directory.\n");
    printf(" -regions     Set the region file.\n");
    printf(" -templatedir Set the template map directory.\n");
    printf(" -tmpdir      Set the directory for temporary files (mostly maps.)\n");
    printf(" -uniquedir   Set the unique items/maps directory.\n");
    printf(" -v           Print version information.\n");
    exit(EXIT_SUCCESS);
}

/**
 * Called before the server starts listening to connections, processes various
 * dump-related options.
 */
static void init_beforeplay(void) {
    init_regions();    /* If not called before, reads all regions from file */
    init_archetype_pointers(); /* Setup global pointers to archetypes */
    finish_races();    /* overwrite race designations using entries in lib/races file */
    assets_finish_archetypes_for_play();
    init_gods(); /* init linked list of gods from archs*/
    init_readable(); /* inits useful arrays for readable texts */

    switch (settings.dumpvalues) {
    case 1:
        print_monsters();
        cleanup();

    case 2:
        dump_abilities();
        cleanup();

    case 3:
        dump_artifacts();
        cleanup();

    case 4:
        dump_spells();
        cleanup();

    case 5:
        cleanup();

    case 6:
        dump_races();
        cleanup();

    case 7:
        dump_alchemy();
        cleanup();

    case 8:
        dump_gods();
        cleanup();

    case 9:
        dump_alchemy_costs();
        cleanup();

    case 10:
        dump_monster_treasure(settings.dumparg);
        cleanup();
    }
}

/**
 * Checks if starting the server is allowed.
 *
 * @todo describe forbid_play() and such restrictions.
 */
static void init_startup(void) {
    char buf[MAX_BUF];
    FILE *fp;

#ifdef SHUTDOWN_FILE
    snprintf(buf, sizeof(buf), "%s/%s", settings.confdir, SHUTDOWN_FILE);
    if ((fp = fopen(buf, "r")) != NULL) {
        while (fgets(buf, MAX_BUF-1, fp) != NULL)
            printf("%s", buf);
        fclose(fp);
        exit(1);
    }
#endif

    if (forbid_play()) { /* Maybe showing highscore should be allowed? */
        LOG(llevError, "CrossFire: Playing not allowed.\n");
        exit(-1);
    }
}

/**
 * Signal handler that begins a normal server shutdown.
 */
static void signal_shutdown(int signum_unused) {
    (void) signum_unused; /* avoid unused warning if enambled */
    shutdown_flag += 1;
}

/**
 * SIGHUP handler.
 * SIGHUP handlers on daemons typically make them reread the config
 * files and reinitialize itself.  This behaviour is better left for
 * an explicit shutdown and restart with Crossfire, as there is just
 * too much persistent runtime state.  However, another function of
 * SIGHUP handlers is to reopen the log file for logrotate's benefit.
 * We can do that here.
 *
 * @param i
 * unused.
 */
static void rec_sighup(int i) {
    (void)i;
    /* Don't call LOG().  It calls non-reentrant functions.  The other
     * signal handlers shouldn't really call LOG() either. */
    if (logfile != stderr) {
        reopen_logfile = 1;
    }
}

/**
 * Setup our signal handlers.
 */
void init_signals(void) {
#ifndef WIN32 /* init_signals() remove signals */
    struct sigaction sa;

    sa.sa_sigaction = NULL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = rec_sighup;
    sigaction(SIGHUP, &sa, NULL);
    signal(SIGINT, signal_shutdown);
    signal(SIGPIPE, SIG_IGN);
#endif /* win32 */
}

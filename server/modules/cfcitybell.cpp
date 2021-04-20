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

extern "C" {
#include "global.h"
#include "object.h"
#include "sproto.h"
#include <string.h>
}

#include <string>
#include <unordered_map>

static int last_hr;

/** One region with bells. */
struct Region {
    std::unordered_map<std::string, std::string> bells; /**< Map between a god's name and the message to display. */
    std::string fallback;                               /**< Message if the god's name is not in ::bells. */
};

/** All defined regions. */
static std::unordered_map<std::string, Region *> regions;

/**
 * Load a .bells file.
 * @param reader reader.
 * @param filename file name.
 */
static void load_bells(BufferReader *reader, const char *filename) {
    Region *current = NULL;
    char *line;
    char *split[20];

    while ((line = bufferreader_next_line(reader))) {
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }
        char *space = strchr(line, ' ');
        if (!space) {
            LOG(llevError, "Invalid bell line '%s' in %s:%ld\n", line, filename, bufferreader_current_line(reader));
            continue;
        }
        *space = '\0';
        space++;

        if (strcmp(line, "region") == 0) {
            current = new Region();
            regions[space] = current;
            continue;
        }
        if (!current) {
            LOG(llevError, "Missing 'region' in bell file %s\n", filename);
            continue;
        }
        size_t count = split_string(line, split, sizeof(split), ',');
        for (size_t i = 0; i < count; i++) {
            if (strcmp(split[i], "*") == 0) {
                current->fallback = space;
            } else {
                current->bells[split[i]] = space;
            }
        }
    }
}

/**
 * Ring the city bells for each player.
 */
static void ring_bell(void) {

    object *pl = first_player ? first_player->ob : NULL;
    while (pl) {
        // If the player is on a map, then try to ring the bell
        if (pl->map) {
            region *reg = get_region_by_map(pl->map);
            if (reg) {
                auto found = regions.find(reg->name);
                if (found != regions.end()) {
                    const char *god_name = determine_god(pl);
                    auto god = found->second->bells.find(god_name);
                    std::string msg = god == found->second->bells.end() ? found->second->fallback : god->second;
                    auto r = msg.find("%god");
                    if (r != std::string::npos) {
                        msg.replace(r, 4, god_name);
                    }
                    draw_ext_info(NDI_UNIQUE|NDI_ORANGE, 0, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, msg.c_str());
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

/**
 * Citybells module initialisation.
 * @param settings server settings.
 */
extern "C"
void cfcitybell_init(Settings *settings) {
    timeofday_t tod;
    get_tod(&tod);
    last_hr = tod.hour;
    events_register_global_handler(EVENT_CLOCK, clock_listener);

    settings->hooks_filename[settings->hooks_count] = ".bells";
    settings->hooks[settings->hooks_count] = load_bells;
    settings->hooks_count++;

    /* Disable the plugin in case it's still there */
    linked_char *disable = static_cast<linked_char *>(calloc(1, sizeof(linked_char)));
    disable->next = settings->disabled_plugins;
    disable->name = strdup("cfcitybell");
    settings->disabled_plugins = disable;
}

extern "C"
void cfcitybell_close() {
    for (auto reg : regions) {
        delete reg.second;
    }
    regions.clear();
}

/*@}*/

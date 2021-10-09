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

#include "TreasureLoader.h"
#include "Archetypes.h"
#include "Treasures.h"
#include "Utils.h"

extern "C" {
#include "string.h"
#include "global.h"
#include "compat.h"
}

TreasureLoader::TreasureLoader(Treasures *treasures, Archetypes *archetypes)
 : m_treasures(treasures), m_archetypes(archetypes) {
}

/**
 * Allocate and return the pointer to an empty treasure structure.
 *
 * @return
 * new structure, blanked, never NULL.
 *
 * @note
 * will call fatal() if memory allocation error.
 * @ingroup page_treasure_list
 */

static treasure *get_empty_treasure(void) {
    treasure *t = (treasure *)calloc(1, sizeof(treasure));
    if (t == NULL)
        fatal(OUT_OF_MEMORY);
    t->item = NULL;
    t->name = NULL;
    t->next = NULL;
    t->next_yes = NULL;
    t->next_no = NULL;
    t->chance = 100;
    t->magic = 0;
    t->nrof = 0;
    return t;
}

extern size_t nroftreasures;

/**
 * Reads one treasure from the file, including the 'yes', 'no' and 'more' options.
 *
 * @param fp
 * file to read from.
 * @param[out] line
 * position in file.
 * @return
 * read structure, never NULL.
 *
 * @todo
 * check if change_name is still used, and remove it if no.
 * @ingroup page_treasure_list
 */

treasure *TreasureLoader::loadTreasure(BufferReader *reader, const std::string &filename) {
    char *buf, *cp, variable[MAX_BUF];
    treasure *t = get_empty_treasure();
    int value;

    nroftreasures++;
    while ((buf = bufferreader_next_line(reader)) != NULL) {

        if (*buf == '#')
            continue;
        cp = buf;
        while (isspace(*cp)) /* Skip blanks */
            cp++;

        if (sscanf(cp, "arch %s", variable)) {
            if (t->item) {
                LOG(llevError, "treasure: duplicate 'arch' in %s:%d\n", filename.c_str(), bufferreader_current_line(reader));
            }
            t->item = m_archetypes->get(variable);
        } else if (sscanf(cp, "list %s", variable)) {
            if (t->name) {
                LOG(llevError, "treasure: duplicate 'name' in %s:%d\n", filename.c_str(), bufferreader_current_line(reader));
                free_string(t->name);
            }
            t->name = add_string(variable);
        } else if (sscanf(cp, "change_name %s", variable))
            t->change_arch.name = add_string(variable);
        else if (sscanf(cp, "change_title %s", variable))
            t->change_arch.title = add_string(variable);
        else if (sscanf(cp, "change_slaying %s", variable))
            t->change_arch.slaying = add_string(variable);
        else if (sscanf(cp, "chance %d", &value))
            t->chance = (uint8_t)value;
        else if (sscanf(cp, "nrof %d", &value))
            t->nrof = (uint16_t)value;
        else if (sscanf(cp, "magic %d", &value))
            t->magic = (uint8_t)value;
        else if (!strcmp(cp, "yes"))
            t->next_yes = loadTreasure(reader, filename);
        else if (!strcmp(cp, "no"))
            t->next_no = loadTreasure(reader, filename);
        else if (!strcmp(cp, "end"))
            return t;
        else if (!strcmp(cp, "more")) {
            t->next = loadTreasure(reader, filename);
            return t;
        } else
            LOG(llevError, "Unknown treasure-command: '%s', last entry %s in %s:%d\n", cp, t->name ? t->name : "null", filename.c_str(), bufferreader_current_line(reader));
    }
    LOG(llevError, "treasure lacks 'end' in %s at line %d.\n", filename.c_str(), bufferreader_current_line(reader));
    fatal(SEE_LAST_ERROR);
    return t;
}

/**
 * Load all treasures from a buffer.
 * @param buffer where to read from.
 * @param filename full path of the file for logging purposes.
 */
void TreasureLoader::load(BufferReader *reader, const std::string& filename) {
    char *buf, name[MAX_BUF];
    treasure *t;

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#' || *buf == '\0')
            continue;

        if (sscanf(buf, "treasureone %s", name) || sscanf(buf, "treasure %s", name)) {
            treasurelist *tl = (treasurelist *)calloc(1, sizeof(treasurelist));
            tl->name = add_string(name);
            tl->items = loadTreasure(reader, filename);

            /* This is a one of the many items on the list should be generated.
             * Add up the chance total, and check to make sure the yes & no
             * fields of the treasures are not being used.
             */
            if (!strncmp(buf, "treasureone", 11)) {
                for (t = tl->items; t != NULL; t = t->next) {
                    if (t->next_yes || t->next_no) {
                        LOG(llevError, "Treasure %s is one item, but on treasure %s\n", tl->name, t->item ? t->item->name : t->name);
                        LOG(llevError, "  the next_yes or next_no field is set\n");
                    }
                    tl->total_chance += t->chance;
                }
            }

            m_treasures->define(tl->name, tl);
        } else
            LOG(llevError, "Treasure-list didn't understand: %s in %s:%d\n", buf, filename.c_str(), bufferreader_current_line(reader));
    }
}

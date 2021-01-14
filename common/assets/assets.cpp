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

/**
 * @file
 * Handles asset collection.
 */

#ifndef WIN32 /* ---win32 exclude headers */
#include <dirent.h>
#include <sys/stat.h>
#include "../../include/autoconf.h"
#endif

#include "compat.h"
#include "assets.h"
#include "AssetsManager.h"
#include "AssetCollector.h"
#include "TreasureLoader.h"
#include "ArchetypeLoader.h"
#include "PngLoader.h"
#include "FacesetLoader.h"
#include "FaceLoader.h"
#include "WrapperLoader.h"
#include "MessageLoader.h"
#include "Faces.h"
#include <string.h>

#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <map>
#include <memory>

#include "AssetWriter.h"
#include "TreasureWriter.h"
#include "FaceWriter.h"
#include "AnimationWriter.h"
#include "ArchetypeWriter.h"
#include "MessageWriter.h"
#include "image.h"
#include "FacesetWriter.h"
#include "ArtifactWriter.h"
#include "FormulaeWriter.h"

#include "microtar.h"

static AssetsManager *manager = nullptr;

/**
 * Init assets-related variables.
 */
void assets_init() {
    manager = new AssetsManager();
}

/**
 * Free all assets-related memory.
 */
void assets_free() {
    delete manager;
    manager = nullptr;
}

/**
 * Number of treasure items, for malloc info.
 */
size_t nroftreasures = 0;

/**
 * Checks if a treasure if valid. Will also check its yes and no options.
 *
 * Will LOG() to error.
 *
 * @param t
 * treasure to check.
 * @param tl
 * needed only so that the treasure name can be printed out.
 * @ingroup page_treasure_list
 */
static void check_treasurelist(treasure *t, const treasurelist *tl) {
    if (t->item == NULL && t->name == NULL)
        LOG(llevError, "Treasurelist %s has element with no name or archetype\n", tl->name);
    if (t->chance >= 100 && t->next_yes && (t->next || t->next_no))
        LOG(llevError, "Treasurelist %s has element that has 100%% generation, next_yes field as well as next or next_no\n", tl->name);
    if (t->name && strcmp(t->name, "NONE"))
        find_treasurelist(t->name);
    if (t->next)
        check_treasurelist(t->next, tl);
    if (t->next_yes)
        check_treasurelist(t->next_yes, tl);
    if (t->next_no)
        check_treasurelist(t->next_no, tl);
}

/**
 * Collect all assets from the specified directory and all its subdirectories.
 * @param datadir directory to search from.
 */
void assets_collect(const char* datadir) {
    LOG(llevInfo, "Starting to collect assets from %s\n", datadir);

    AssetCollector collector;
    collector.addLoader(new TreasureLoader(manager->treasures(), manager->archetypes()));
    collector.addLoader(new ArchetypeLoader(manager->archetypes()));
    collector.addLoader(new PngLoader(manager->faces()));
    collector.addLoader(new FacesetLoader());
    collector.addLoader(new FaceLoader(manager->faces(), manager->animations()));
    collector.addLoader(new MessageLoader(manager->messages()));
    collector.addLoader(new WrapperLoader("/artifacts", init_artifacts));
    collector.addLoader(new WrapperLoader("/formulae", init_formulae));
    collector.addLoader(new WrapperLoader("/attackmess", init_attackmess));
    for (uint8_t hook = 0; hook < settings.hooks_count; hook++) {
        collector.addLoader(new WrapperLoader(settings.hooks_filename[hook], settings.hooks[hook]));
    }
    collector.collect(datadir);

    LOG(llevInfo, "Finished collecting assets from %s\n", datadir);
}

/**
 * Check all generators have the other_arch set or something in inventory.
 */
static void check_generators(void) {
    int abort = 0;

    manager->archetypes()->each([&abort] (const auto& arch) {
        if (!QUERY_FLAG(&arch->clone, FLAG_GENERATOR))
            return;

        if (!QUERY_FLAG(&arch->clone, FLAG_CONTENT_ON_GEN) && arch->clone.other_arch == NULL) {
            LOG(llevError, "Fatal: %s is generator without content_on_gen but lacks other_arch.\n", arch->name);
            abort = 1;
            return;
        }
        if (QUERY_FLAG(&arch->clone, FLAG_CONTENT_ON_GEN) && arch->clone.inv == NULL) {
            LOG(llevError, "Fatal: %s is generator with content_on_gen but lacks inventory.\n", arch->name);
            abort = 1;
            return;
        }
    });

    if (abort)
        fatal(SEE_LAST_ERROR);
}

/**
 * This checks all summonable items for move_type and other things.
 * Will call fatal() if an error is found.
 */
void check_summoned(void) {
    manager->archetypes()->each([] (const auto& arch) {
        if (arch->clone.type == SPELL && arch->clone.subtype == SP_SUMMON_GOLEM && arch->clone.other_arch) {
            if (arch->clone.other_arch->clone.move_type == 0) {
                LOG(llevError, "Summonable archetype %s [%s] has no move_type defined!\n", arch->clone.other_arch->name, arch->clone.other_arch->clone.name);
                fatal(SEE_LAST_ERROR);
            }
        }
    });
}

/**
 * This ensures all spells have a skill defined, calling fatal() if any error was found.
 */
static void check_spells(void) {
    int abort = 0;

    manager->archetypes()->each([&abort] (const auto& arch) {
        if (arch->clone.type == SPELL && arch->clone.skill == NULL) {
            LOG(llevError, "Spell archetype %s [%s] has no skill defined!\n", arch->name, arch->clone.name);
            abort = 1;
        }
    });

    if (abort)
        fatal(SEE_LAST_ERROR);
}

/**
 * Dump and reset referenced but undefined assets.
 * @return total number of referenced but not defined assets.
 */
size_t assets_dump_undefined() {
    return manager->dumpUndefined();
}

/**
 * Called after collect is complete, to check various things.
 */
void assets_end_load() {
    check_generators();
    check_spells();
    check_summoned();
    manager->treasures()->each([] (auto list) {
        if (list->items) {
            check_treasurelist(list->items, list);
        }
    });
}

/**
 * Searches for the given treasurelist in the globally linked list
 * of treasurelists which has been built by load_treasures().
 *
 * Will LOG() to error if not found.
 *
 * @param name
 * treasure list to search.
 * @return
 * match, or NULL if treasurelist doesn't exist or is 'none'.
 * @ingroup page_treasure_list
 */
treasurelist *find_treasurelist(const char *name) {
    if (!strcmp(name, "none"))
        return NULL;
    return manager->treasures()->get(name);
}

size_t assets_number_of_treasures() {
    return nroftreasures;
}
size_t assets_number_of_treasurelists() {
    return manager->treasures()->count();
}

size_t assets_number_of_archetypes() {
    return manager->archetypes()->count();
}

archetype *get_next_archetype(archetype *current) {
    return manager->archetypes()->next(current);
}

archetype *find_archetype(const char *name) {
    return manager->archetypes()->get(name);
}

archetype *try_find_archetype(const char *name) {
    return manager->archetypes()->find(name);
}

int get_animations_count() {
    return manager->animations()->count();
}

size_t assets_number_of_animations() {
    return manager->animations()->count();
}

Animations *find_animation(const char *name) {
    return manager->animations()->get(name);
}

Animations *try_find_animation(const char *name) {
    return manager->animations()->find(name);
}

void animations_for_each(anim_op op) {
    manager->animations()->each(*op);
}

const Face *find_face(const char *name) {
    return manager->faces()->get(name);
}

const Face *try_find_face(const char *name, const Face *error) {
    auto found = manager->faces()->find(name);
    if (found)
        return found;
    return error;
}

size_t get_faces_count() {
    return manager->faces()->count();
}

void faces_for_each(face_op op) {
    manager->faces()->each(*op);
}

void facesets_for_each(faceset_op op) {
    manager->facesets()->each(*op);
}

void archetypes_for_each(arch_op op) {
    manager->archetypes()->each(*op);
}

AssetsManager *getManager() {
    return manager;
}

/**
 * Get a face from its unique identifier.
 * @param id face identifier.
 * @return matching face, NULL if no face with this identifier.
 * @todo move back to image.c when migrated
 */
const Face *get_face_by_id(uint16_t id) {
    return manager->faces()->findById(id);
}

GeneralMessage *get_random_message() {
    return manager->messages()->random();
}

/**
 * Find the message from its identifier.
 * @param identifier message's identifier.
 * @return corresponding message, NULL if no such message.
 */
const GeneralMessage *get_message_from_identifier(const char *identifier) {
    return manager->messages()->find(identifier);
}

int get_bitmap_checksum() {
    return manager->faces()->checksum();
}

face_sets *find_faceset(int id) {
    return manager->facesets()->findById(id);
}

template<class T>
static void do_pack(AssetWriter<T> *writer, AssetsCollection<T> *assets, StringBuffer *buf) {
    assets->each([writer, buf] (T *asset) {
       writer->write(asset, buf);
    });
    delete writer;
}

static void pack_artifacts(StringBuffer *buf) {
    ArtifactWriter writer;
    artifactlist *list = first_artifactlist;
    while (list) {
        writer.write(list, buf);
        list = list->next;
    }
}

static void pack_formulae(StringBuffer *buf) {
    FormulaeWriter writer;
    recipelist *list = get_formulalist(1);
    while (list) {
        writer.write(list, buf);
        list = list->next;
    }
}

static void build_filename(const char *name, const char *prefix, char *dest, size_t max) {
    auto dot = strrchr(name, '.');
    if (!dot) {
        snprintf(dest, max, "%s.%s", name, prefix);
        return;
    }
    memset(dest, 0, max);
    dot++;

    memcpy(dest, name, dot - name);
    strncat(dest, prefix, max);
    strncat(dest, ".", max);
    strncat(dest, dot, max);
    strncat(dest, ".png", max);
}

static void pack_images(const char *filename) {
    auto now = time(NULL);
    mtar_t tar;
    mtar_open(&tar, filename, "w");
    manager->faces()->each([&tar, &now] (const auto face) {
        manager->facesets()->each([&tar, &now, &face] (const auto fs) {
            if (!fs->prefix || fs->allocated <= face->number || !fs->faces[face->number].datalen) {
                return;
            }
            mtar_header_t h;
            memset(&h, 0, sizeof(h));
            build_filename(face->name, fs->prefix, h.name, sizeof(h.name));
            h.size = fs->faces[face->number].datalen;
            h.type = MTAR_TREG;
            h.mode = 0664;
            h.mtime = now;
            /* Build raw header and write */
            mtar_write_header(&tar, &h);
            mtar_write_data(&tar, fs->faces[face->number].data, fs->faces[face->number].datalen);
        });
    });
    mtar_finalize(&tar);
    mtar_close(&tar);
}

/**
 * Write smoothing information to the specified StringBuffer.
 * Used when packing archetypes, since smooth is there...
 * @param buf where to add information.
 */
static void add_smooth(StringBuffer *buf) {
    manager->faces()->each([&buf] (const auto face) {
       if (face->smoothface) {
           stringbuffer_append_printf(buf, "smoothface %s %s\n", face->name, face->smoothface->name);
       }
    });
}

void assets_pack(const char *type, const char *filename) {
    StringBuffer *buf = stringbuffer_new();
    if (strcmp(type, "treasures") == 0) {
        do_pack(new TreasureWriter(), manager->treasures(), buf);
    } else if (strcmp(type, "faces") == 0) {
        do_pack(new FaceWriter(), manager->faces(), buf);
        do_pack(new AnimationWriter(), manager->animations(), buf);
    } else if (strcmp(type, "archs") == 0) {
        do_pack(new ArchetypeWriter(), manager->archetypes(), buf);
        add_smooth(buf);
    } else if (strcmp(type, "messages") == 0) {
        do_pack(new MessageWriter(), manager->messages(), buf);
    } else if (strcmp(type, "facesets") == 0) {
        do_pack(new FacesetWriter(), manager->facesets(), buf);
    } else if (strcmp(type, "artifacts") == 0) {
        pack_artifacts(buf);
    } else if (strcmp(type, "formulae") == 0) {
        pack_formulae(buf);
    } else if (strcmp(type, "images") == 0) {
        pack_images(filename);
        stringbuffer_delete(buf);
        return;
    } else {
        LOG(llevError, "Invalid asset type '%s'\n", type);
        fatal(SEE_LAST_ERROR);
    }

    size_t length = stringbuffer_length(buf);
    char *data = stringbuffer_finish(buf);

    FILE *out = fopen(filename, "w+");
    if (!out) {
        LOG(llevError, "Failed to open file '%s'\n", filename);
        fatal(SEE_LAST_ERROR);
    }
    if (fwrite(static_cast<void*>(data), 1, length, out) != length) {
        LOG(llevError, "Failed to write all data!\n", filename);
        fclose(out);
        fatal(SEE_LAST_ERROR);
    }
    free(data);
    fclose(out);
}

void assets_finish_archetypes_for_play() {
    manager->archetypes()->each([] (archetype *arch) {
        object *op = &arch->clone;
        if (op->speed < 0) {
            op->speed_left = op->speed_left - RANDOM() % 100 / 100.0;
            op->speed = -op->speed; // Make this always positive
        }
    });
}

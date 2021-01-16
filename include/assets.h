#ifndef ASSETS_H
#define ASSETS_H

/**
 * @file
 * C function wrappers to interact with assets.
 * Should not be used by C++ code, except for getManager() to get the AssetsManager.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "global.h"
#include "image.h"

void assets_init();
void assets_free();
void assets_collect(const char* datadir);
size_t assets_dump_undefined();
void assets_end_load();

typedef void(*arch_op)(archetype *);
typedef void(*face_op)(const Face *);
typedef void(*anim_op)(const Animations *);
typedef void(*faceset_op)(const face_sets *);

size_t assets_number_of_archetypes();
void archetypes_for_each(arch_op op);

size_t assets_number_of_treasures();
size_t assets_number_of_treasurelists();

size_t assets_number_of_animations();
void animations_for_each(anim_op op);

void faces_for_each(face_op op);

void facesets_for_each(faceset_op op);

GeneralMessage *get_random_message();

/**
 * Pack the specified assets in a file.
 * @param type assets type, see actual implementation.
 * @param filename destination file.
 */
void assets_pack(const char *type, const char *filename);

void assets_finish_archetypes_for_play();

long recipe_find_ingredient_cost(const char *name);

#ifdef __cplusplus
}

class AssetsManager *getManager();

#endif

#endif /* ASSETS_H */

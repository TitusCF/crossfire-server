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

/**
 * @defgroup ASSETS_xxx Assets flags
 *
 * What to collect when calling assets_collect().
 */
/*@{*/
#define ASSETS_ARCHETYPES                 0x0001
#define ASSETS_FACES                      0x0002
#define ASSETS_ANIMATIONS                 0x0004
#define ASSETS_FACESETS                   0x0008
#define ASSETS_TREASURES                  0x0010
#define ASSETS_PNG                        0x0020
#define ASSETS_MESSAGES                   0x0040
#define ASSETS_ARTIFACTS                  0x0080
#define ASSETS_FORMULAE                   0x0100
#define ASSETS_ATTACK_MESSAGES            0x0200
#define ASSETS_ALL                        0xFFFF
/*@}*/

void assets_init();
void assets_free();
void assets_collect(const char* datadir, int what);
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

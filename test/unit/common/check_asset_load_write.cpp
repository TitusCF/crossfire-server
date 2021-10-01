extern "C" {
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "global.h"
#include <toolkit_common.h>
#include <math.h>
}
#include "Archetypes.h"
#include "Treasures.h"
#include "TreasureLoader.h"
#include "TreasureWriter.h"
#include "AnimationWriter.h"
#include "FaceLoader.h"
#include "Faces.h"
#include "Animations.h"
#include "assets.h"

static void setup(void) {
    init_hash_table();
    assets_init();
}

static void teardown(void) {
    assets_free();
}

char *generate_name(size_t max_length) {
    static char buf[250];
    size_t len = rndm(5, MIN(max_length, sizeof(buf) - 1));
    buf[len] = 0;
    for (size_t c = 0; c < len; c++) {
        buf[c] = 'a' + rndm(0, 25);
    }
    return buf;
}

template<class T>
static void write_load(T *asset, AssetWriter<T> &writer, AssetLoader &loader) {
    StringBuffer *sb = stringbuffer_new();

    writer.write(asset, sb);

    auto length = stringbuffer_length(sb);
    auto data = stringbuffer_finish(sb);
    BufferReader *br = bufferreader_init_from_memory(NULL, data, length);

    loader.load(br, "dummy");

    bufferreader_destroy(br);
    free(data);
}

treasure *generate_random_treasure(Archetypes &arches, int depth, bool single) {
    treasure *trs = static_cast<treasure *>(calloc(1, sizeof(treasure)));
    trs->chance = rndm(0, 15);
    if (rndm(0, 1)) {
        trs->name = generate_name(50);
    } else {
        trs->item = arches.get(generate_name(50));
    }
    trs->magic = rndm(0, 6);
    trs->nrof = rndm(0, 5);
    if (rndm(0, 5 - depth) != 0) {
        trs->next = generate_random_treasure(arches, depth + 1, single);
    }
    if (!single && rndm(0, 2) == 0) {
        trs->next_no = generate_random_treasure(arches, depth + 1, single);
    }
    if (!single && rndm(0, 2) == 0) {
        trs->next_yes = generate_random_treasure(arches, depth + 1, single);
    }
    return trs;
}

treasurelist *generate_random_list(Archetypes &arches) {
    treasurelist *list = static_cast<treasurelist *>(calloc(1, sizeof(treasurelist)));
    list->name = add_string(generate_name(50));
    bool single = rndm(0, 1) == 0;
    list->items = generate_random_treasure(arches, 0, single);
    if (single) {
        auto i = list->items;
        while (i) {
            list->total_chance += i->chance;
            i = i->next;
        }
    }
    return list;
}

bool equal(const treasure *left, treasure *right) {
    if (left == nullptr) {
        fail_unless(right == nullptr, "left treasure == null but right treasure != null");
        return true;
    }
    fail_unless(right != nullptr, "left treasure != null but right treasure == null");
    fail_unless(left->chance == right->chance, "different chance %d %d", left->chance, right->chance);
    fail_unless(left->magic == right->magic, "different magic %d %d", left->magic, right->magic);
    fail_unless(left->nrof == right->nrof, "different nrof %d %d", left->nrof, right->nrof);
    return true;
}

bool equal(const treasurelist *left, const treasurelist *right) {
    if (left == nullptr) {
        fail_unless(right == nullptr, "left list == null but right list != null");
        return true;
    }
    fail_unless(right != nullptr, "left list != null but right list == null");
    fail_unless(strcmp(left->name, right->name) == 0, "left list name %s != right list name %s", left->name, right->name);
    fail_unless(equal(left->items, right->items), "left list items != right list items");
    fail_unless(left->total_chance == right->total_chance, "left list total_chance %d != right list total_chance %d", left->total_chance, right->total_chance);
    return true;
}

START_TEST(test_treasure) {
    Archetypes arch;
    Treasures trs;
    TreasureLoader loader(&trs, &arch);
    TreasureWriter writer;

    auto generated = generate_random_list(arch);

    write_load(generated, writer, loader);

    auto loaded = trs.find(generated->name);
    fail_unless(loaded, "Treasurelist should have been loaded!");
    fail_unless(equal(generated, loaded), "loaded treasurelist isn't the same as generated one");
}
END_TEST

static Animations *generate_animation(Faces &faces) {
    auto anim = static_cast<Animations *>(calloc(1, sizeof(Animations)));
    anim->name = generate_name(50);
    anim->facings = pow(2, rndm(0, 3));
    anim->num_animations = rndm(1, 16);
    anim->faces = static_cast<const Face **>(calloc(anim->num_animations, sizeof(const Face *)));
    for (uint8_t f = 0; f < anim->num_animations; f++) {
        anim->faces[f] = faces.get(generate_name(25));
    }
    return anim;
}

START_TEST(test_animation) {
    Faces faces;
    AllAnimations anims;
    Animations *anim = generate_animation(faces);

    AnimationWriter writer;
    FaceLoader loader(&faces, &anims);

    write_load(anim, writer, loader);

    auto loaded = anims.find(anim->name);
    fail_unless(loaded, "face should have been loaded");
    ck_assert_str_eq(anim->name, loaded->name);
    ck_assert_int_eq(anim->num_animations, loaded->num_animations);
    for (uint8_t f = 0; f < anim->num_animations; f++) {
        ck_assert_str_eq(anim->faces[f]->name, loaded->faces[f]->name);
    }
    ck_assert_int_eq(anim->facings, loaded->facings);
}
END_TEST

static Suite *shstr_suite(void) {
    Suite *s = suite_create("asset_load_write");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_loop_test(tc_core, test_treasure, 0, 25);
    tcase_add_loop_test(tc_core, test_animation, 0, 25);

    return s;
}

int main(void) {
    int nf;
    Suite *s = shstr_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/common/asset_load_write.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/asset_load_write.out");
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

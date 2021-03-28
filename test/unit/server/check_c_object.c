/*
 * static char *rcsid_check_c_object_c =
 *   "$Id$";
 */

/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2002 Mark Wedel & Crossfire Development Team
 * Copyright (C) 1992 Frank Tore Johansen
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
 *
 * The authors can be reached via e-mail at crossfire-devel@real-time.com
 */

/*
 * This is the unit tests file for server/c_object.c
 */

#include <global.h>
#include <stdlib.h>
#include <check.h>
#include <loader.h>
#include <toolkit_common.h>
#include <sproto.h>

static void setup(void) {
}

static void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

static object *find_best_apply_object_match(object *start, object *pl, const char *params, int aflag) {
    object *tmp, *best = NULL;
    int match_val = 0, tmpmatch;

    for (tmp = start; tmp; tmp = tmp->below) {
        if (tmp->invisible)
            continue;
        if ((tmpmatch = object_matches_string(pl, tmp, params)) > match_val) {
            if ((aflag == AP_APPLY) && (QUERY_FLAG(tmp, FLAG_APPLIED)))
                continue;
            if ((aflag == AP_UNAPPLY) && (!QUERY_FLAG(tmp, FLAG_APPLIED)))
                continue;
            match_val = tmpmatch;
            best = tmp;
        }
    }
    return best;
}

START_TEST(test_find_best_apply_object_match) {
    object *pl, *found;
    object *gorokh, *cloak, *other;

    pl = create_archetype("kobold");
    fail_unless(pl != NULL, "can't find kobold archetype.");

    gorokh = create_archetype("cloak");
    gorokh->title = add_string("of Gorokh");
    CLEAR_FLAG(gorokh, FLAG_IDENTIFIED);
    object_insert_in_ob(gorokh, pl);

    cloak = create_archetype("cloak");
    object_insert_in_ob(cloak, pl);

    other = create_archetype("gem");
    object_insert_in_ob(other, pl);

    found = find_best_apply_object_match(pl->inv, pl, "all", 0);
    fail_unless(found == other, "not found gem but %s", found ? found->name : "nothing");

    found = find_best_apply_object_match(pl->inv, pl, "cloak", 0);
    fail_unless(found == cloak, "didn't find cloak but %s", found ? found->name : "nothing");

    found = find_best_apply_object_match(pl->inv, pl, "Gorokh", 0);
    fail_unless(found == NULL, "Gorokh found %s instead of nothing", found ? found->name : "nothing??");
}
END_TEST

START_TEST(test_put_object_in_sack) {
    mapstruct *test_map;
    object *sack, *obj, *sack2, *dummy;

    dummy = create_archetype("orc");

    test_map = get_empty_map(5, 5);
    fail_unless(test_map != NULL, "can't create test map");

    sack = create_archetype("gem");
    object_insert_in_map_at(sack, test_map, NULL, 0, 0, 0);
    fail_unless(GET_MAP_OB(test_map, 0, 0) == sack);

    obj = create_archetype("gem");
    obj->nrof = 1;
    object_insert_in_map_at(obj, test_map, NULL, 0, 1, 0);
    put_object_in_sack(dummy, sack, obj, 1);
    fail_unless(GET_MAP_OB(test_map, 1, 0) == obj, "object was removed from map?");
    fail_unless(sack->inv == NULL, "sack's inventory isn't null?");

    object_remove(sack);
    object_free_drop_inventory(sack);

    /* basic insertion */
    sack = create_archetype("sack");
    sack->nrof = 1;
    fail_unless(sack->type == CONTAINER, "sack isn't a container?");
    object_insert_in_map_at(sack, test_map, NULL, 0, 0, 0);
    fail_unless(GET_MAP_OB(test_map, 0, 0) == sack, "sack not put on map?");

    SET_FLAG(sack, FLAG_APPLIED);
    put_object_in_sack(dummy, sack, obj, 1);
    fail_unless(sack->inv == obj, "object not inserted into sack?");
    fail_unless(GET_MAP_OB(test_map, 1, 0) == NULL, "object wasn't removed from map?");

    object_remove(obj);
    object_insert_in_map_at(obj, test_map, NULL, 0, 1, 0);
    sack->weight_limit = 1;
    obj->weight = 5;

    put_object_in_sack(dummy, sack, obj, 1);
    fail_unless(sack->inv == NULL, "item was put in sack even if too heavy?");
    fail_unless(GET_MAP_OB(test_map, 1, 0) == obj, "object was removed from map?");

    /* now for sack splitting */
    sack->nrof = 2;
    obj->weight = 1;

    put_object_in_sack(dummy, sack, obj, 1);
    fail_unless(sack->nrof == 1, "sack wasn't split?");
    fail_unless(sack->above != NULL, "no new sack created?");
    fail_unless(sack->inv == obj, "object not inserted in old sack?");
    fail_unless(sack == obj->env, "object's env not updated?");

    /* now moving to/from containers */
    obj->nrof = 2;
    sack2 = sack->above;
    SET_FLAG(sack2, FLAG_APPLIED);
    dummy->container = sack;
    put_object_in_sack(dummy, sack, sack2, 1);
    fail_unless(sack2->inv == NULL, "sack2's not empty?");
    fail_unless(sack->inv == obj, "obj wasn't transferred?");

    /* move between containers and split containers */
    object_remove(sack2);
    object_insert_in_map_at(sack2, test_map, NULL, 0, 2, 0);
    SET_FLAG(sack2, FLAG_APPLIED);
    sack2->nrof = 2;
    dummy->container = sack2;
    put_object_in_sack(dummy, sack2, sack, 0);
    fail_unless(sack->inv == NULL, "sack wasn't put into sack2?");
    fail_unless(sack2->inv != NULL, "sack2 wasn't filled?");
    fail_unless(sack2->above != NULL, "sack2 wasn't split?");
    fail_unless(sack2->above->inv == NULL, "sack2's split was filled?");

    free_map(test_map);
}
END_TEST

#define TEST_ITEMS_COUNT    11
static char *test_items[TEST_ITEMS_COUNT] = {
    "sword",
    "broadsword",
    "power_crystal",
    "plate_mail",
    "full_helmet",
    "ring",
    "amulet",
    "rod_light",
    "platinacoin",
    "gem",
    "dungeon_magic" // Keep last to have contiguous indexes in checks - never picked up
};

#define DO_TAKE 1
#define DO_DROP 2

/**
 * Actual testing function.
 * It creates all items in @ref test_items, puts them on map or in inventory,
 * and either takes or drops with the specified parameter.
 * After "param", there should be 2 integer lists finished by -1,
 * specifying first the index of items to lock, then the index of items
 * that should be picked/dropped by the commant.
 * @param do_what either DO_TAKE or DO_DROP.
 * @param param parameter to the take or drop command.
 * @param ... items to lock, then items that should be non pickable, then items which should be taken/dropped.
 */
static void do_test(const int do_what, const char *param, ...) {
    mapstruct *test_map;
    object *items[TEST_ITEMS_COUNT], *monster;
    int check[TEST_ITEMS_COUNT];
    const char *action = do_what == DO_TAKE ? "picked" : "dropped";

    test_map = get_empty_map(1, 1);
    fail_unless(test_map != NULL, "couldn't create map");
    monster = create_archetype("kobold");
    fail_unless(monster != NULL, "couldn't create kobold");

    object_insert_in_map_at(monster, test_map, NULL, 0, 0, 0);

    object *top = monster;
    for (int i = 0; i < TEST_ITEMS_COUNT; i++) {
        archetype *arch = try_find_archetype(test_items[i]);
        fail_unless(arch != NULL, "couldn't find arch %s", test_items[i]);
        items[i] = arch_to_object(arch);
        fail_unless(items[i] != NULL, "couldn't create %s", test_items[i]);
        if (do_what == DO_TAKE) {
            object_insert_in_map_at(items[i], test_map, top, INS_BELOW_ORIGINATOR, 0, 0);
        } else {
            object_insert_in_ob(items[i], monster);
        }
        top = items[i];
    }

    va_list args;
    va_start(args, param);
    int idx;
    while ((idx = va_arg(args, int)) != -1) {
        SET_FLAG(items[idx], FLAG_INV_LOCKED);
    }
    while ((idx = va_arg(args, int)) != -1) {
        SET_FLAG(items[idx], FLAG_NO_PICK);
    }

    if (do_what == DO_TAKE) {
        command_take(monster, param);
    } else {
        command_drop(monster, param);
    }

    memset(check, do_what == DO_TAKE ? 0 : 1, sizeof(check));
    while ((idx = va_arg(args, int)) != -1) {
        check[idx] = do_what == DO_TAKE ? 1 : 0;
    }
    for (int i = 0; i < TEST_ITEMS_COUNT; i++) {
        if (do_what == DO_TAKE) {
            if (check[i]) {
                fail_unless(items[i]->env == monster, "%s wasn't %s up with %s!", test_items[i], action, param);
            } else {
                fail_unless(items[i]->env == NULL && items[i]->map == test_map, "%s was %s up with %s!", test_items[i], action, param);
            }
        } else {
            if (!check[i]) {
                fail_unless(items[i]->env == NULL && items[i]->map == test_map, "%s wasn't %s up with %s!", test_items[i], action, param);
            } else {
                fail_unless(items[i]->env == monster, "%s was %s up with %s!", test_items[i], action, param);
            }
        }
    }
}

START_TEST(test_command_take) {
    do_test(DO_TAKE, "", -1, -1, 0, -1); // First item below
    do_test(DO_TAKE, "", -1, 0, -1, 1, -1); // First pickable item below
    do_test(DO_TAKE, "azer", -1, -1, -1); // Nothing
    do_test(DO_TAKE, "swo", -1, -1, 0, 1, -1);
    do_test(DO_TAKE, "broad", -1, -1, 1, -1);
    do_test(DO_TAKE, "glow", -1, -1, 2, -1);
    do_test(DO_TAKE, "plate", -1, -1, 3, -1);
    do_test(DO_TAKE, "full", -1, -1, 4, -1);
    do_test(DO_TAKE, "rod", -1, -1, 7, -1);
    do_test(DO_TAKE, "ing", -1, -1, 2, 5, -1);
    do_test(DO_TAKE, "coin", -1, -1, 8, -1);
    do_test(DO_TAKE, "all", -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1); // Special versions
    do_test(DO_TAKE, "*", -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1); // Special versions

    // By index
    do_test(DO_TAKE, "#aze", -1, -1, -1);
    do_test(DO_TAKE, "#0", -1, -1, -1);
    do_test(DO_TAKE, "#-7", -1, -1, -1);
    do_test(DO_TAKE, "#3", -1, -1, 2, -1);

    // By item type
    do_test(DO_TAKE, "*melee", -1, -1, 0, 1, -1);
    do_test(DO_TAKE, "*armour", -1, -1, 3, -1);
    do_test(DO_TAKE, "*valuables", -1, -1, 8, 9, -1);
    do_test(DO_TAKE, "*jewels", -1, -1, 5, 6, -1);
}
END_TEST

START_TEST(test_command_drop) {
    do_test(DO_DROP, "", -1, -1, -1);
    do_test(DO_DROP, "azer", -1, -1, -1);
    do_test(DO_DROP, "sword", 1, -1, -1, 0, -1);

    // By index: items are inserted in reverse order, and invisible ones (dungeon_magic) aren't counted
    do_test(DO_DROP, "#aze", -1, -1, -1);
    do_test(DO_DROP, "#0", -1, -1, -1);
    do_test(DO_DROP, "#-7", -1, -1, -1);
    do_test(DO_DROP, "#8", -1, -1, 2, -1);

    // By item type
    do_test(DO_DROP, "*melee", 0, -1, -1, 1, -1);
    do_test(DO_DROP, "*melee", 0, 1, -1, -1, -1);
    do_test(DO_DROP, "*armour", -1, -1, 3, -1);
    do_test(DO_DROP, "*valuables", 9, -1, -1, 8, -1);
    do_test(DO_DROP, "*jewels", -1, -1, 5, 6, -1);
}
END_TEST

static Suite *c_object_suite(void) {
    Suite *s = suite_create("c_object");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_find_best_apply_object_match);
    tcase_add_test(tc_core, test_put_object_in_sack);
    tcase_add_test(tc_core, test_command_take);
    tcase_add_test(tc_core, test_command_drop);

    return s;
}

int main(void) {
    int nf;
    Suite *s = c_object_suite();
    SRunner *sr = srunner_create(s);

    settings.debug = 0;
    settings.logfilename = "c_object.out";
    cctk_setdatadir(SOURCE_ROOT "lib");
    init(0, NULL);

    srunner_set_fork_status (sr, CK_NOFORK);

    srunner_set_xml(sr, LOGDIR "/unit/server/c_object.xml");
    srunner_set_log(sr, LOGDIR "/unit/server/c_object.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

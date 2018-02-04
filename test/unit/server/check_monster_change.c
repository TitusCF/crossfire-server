/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2002-2018 Mark Wedel & Crossfire Development Team
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
 * Test case for monster changing.
 *
 * TODO:
 * Given where change_object is (time.c),
 * this should be merged into check_time.c
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

static object *do_monster_change(object *op, mapstruct *map, short expect, const char *new_name);

START_TEST(test_monster_change) {
    object *monster = NULL, *tmp = NULL;
    object *floor = NULL;
    mapstruct *map = NULL;
    int x, y;
    
    map = get_empty_map(5, 5);
    // Fill the floor
    for (x = 0; x < 5; ++x)
    {
        for (y = 0; y < 5; ++y)
        {
            floor = create_archetype("battleground");
            fail_unless(floor != NULL, "cannot find arch battleground");
            object_insert_in_map_at(floor, map, NULL, 0, x, y);
        }
    }
    // Add a monster.
    monster = create_archetype("kobold");
    fail_unless(monster != NULL, "failed to find arch kobold");
    // First test: 1-tile to 1-tile conversion.
    monster->other_arch = find_archetype("ogre");
    // Ensure we got other_arch
    fail_unless(monster->other_arch != NULL, "Could not find other_arch to transform object %s.", monster->name);
    monster->map = map;
    monster->stats.food = 1; // Tell it to make one of the next unit
    object_insert_in_map_at(monster, map, NULL, 0 , 0 , 0);
    tmp = do_monster_change(monster, map, 1, "ogre");
    fail_unless(monster != tmp, "We got the same object back when we should have gotten a new one!");
    // Now we do a change to a larger monster as the next test.
    monster = tmp;
    monster->other_arch = find_archetype("angel_knight");
    // Ensure we got other_arch
    fail_unless(monster->other_arch != NULL, "Could not find other_arch to transform object %s.", monster->name);
    monster->stats.food = 1; // Tell it to make one of the next unit
    tmp = do_monster_change(monster, map, 1, "angel_knight");
    fail_unless(monster != tmp, "We got the same object back when we should have gotten a new one!");
    // Change from multipart to multipart as the next test.
    monster = tmp;
    monster->other_arch = find_archetype("bear");
    // Ensure we got other_arch
    fail_unless(monster->other_arch != NULL, "Could not find other_arch to transform object %s.", monster->name);
    monster->stats.food = 1; // Tell it to make one of the next unit
    tmp = do_monster_change(monster, map, 1, "bear");
    fail_unless(monster != tmp, "We got the same object back when we should have gotten a new one!");
    // Change from multipart back down to one.
    monster = tmp;
    monster->other_arch = find_archetype("bee");
    // Ensure we got other_arch
    fail_unless(monster->other_arch != NULL, "Could not find other_arch to transform object %s.", monster->name);
    monster->stats.food = 1; // Tell it to make one of the next unit
    tmp = do_monster_change(monster, map, 1, "bee");
    fail_unless(monster != tmp, "We got the same object back when we should have gotten a new one!");
    // Last test -- Try to change to something way too big for the map.
    monster = tmp;
    monster->other_arch = find_archetype("Greater_Demon");
    // Ensure we got other_arch
    fail_unless(monster->other_arch != NULL, "Could not find other_arch to transform object %s.", monster->name);
    monster->stats.food = 1; // Tell it to make one of the next unit
    tmp = do_monster_change(monster, map, 0, "Greater_Demon");
    fail_unless(monster == tmp, "We expected to get the same monster, but got a different one.");
}
END_TEST

/**
 * Function to change the object and return the result.
 *
 * @param op
 * The object to change
 *
 * @param map
 * The map we should look for the new object on
 *
 * @param expect
 * 1 if success expected, 0 if failure expected
 *
 * @return
 * Pointer to the new object, if successful, pointer to the old object if failure.
 */
static object *do_monster_change(object *op, mapstruct *map, short expect, const char *new_name)
{
    change_object(op);
    if (expect)
        // Now we make sure our monster is freed or removed.
        fail_unless(QUERY_FLAG(op, FLAG_FREED) || QUERY_FLAG(op, FLAG_REMOVED), "original object not removed on expected success!");
    else
    {
        // Expect failure and reinsertion of original
        fail_unless(!QUERY_FLAG(op, FLAG_FREED) && !QUERY_FLAG(op, FLAG_FREED), "Expected failure, was successful in change!");
        return op;
    }
    // And now we have the glorious task of finding the new object. It will not be where the other one was.
    for (int x = 0; x < 5; ++x)
        for (int y = 0; y < 5; ++y)
        {
            // There should be only one thing on the map.
            op = map_find_by_flag(map, x, y, FLAG_MONSTER);
            if (op != NULL)
            {
                // Found the object! Break from the loops.
                // Using a goto to break out of two loops. So sue me. :P
                goto no_more_looping;
            }
        }
no_more_looping:
    // Make sure we found the next object
    fail_unless(op != NULL, "Could not find new object %s in the map.", new_name);
    return op;
}

static Suite *change_suite(void) {
    Suite *s = suite_create("change_monster");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_monster_change);

    return s;
}

int main(void) {
    int nf;
    Suite *s = change_suite();
    SRunner *sr= srunner_create(s);
    
    /* If you wish to debug the program, uncomment this line. */
    srunner_set_fork_status (sr, CK_NOFORK);

    settings.debug = 0;
    init(0, NULL);

    srunner_set_xml(sr, LOGDIR "/unit/server/monster_change.xml");
    srunner_set_log(sr, LOGDIR "/unit/server/monster_change.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

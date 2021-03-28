/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2020 Crossfire Development Team
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

/**
 * @file
 * Unit test for bug https://sourceforge.net/p/crossfire/bugs/883/
 * Monsters will not apply weapons in their inventory
 */

#include <stdlib.h>
#include <check.h>
#include <global.h>
#include <sproto.h>
#include <toolkit_common.h>

static void setup(void) {
}

static void teardown(void) {
}

START_TEST(test_apply) {
    object *monster;
    object *sword;

    monster = create_archetype("goblin");
    fail_unless(monster, "couldn't create goblin");
    sword = create_archetype("sword");
    fail_unless(sword, "couldn't create sword");

    object_insert_in_ob(sword, monster);
    monster_check_apply_all(monster);

    fail_unless(QUERY_FLAG(sword, FLAG_APPLIED), "sword should be applied");
}
END_TEST

static Suite *bug_suite(void) {
    Suite *s = suite_create("bug");
    TCase *tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_apply);
    tcase_set_timeout(tc_core, 0);

    return s;
}

int main(void) {
    int nf;
    Suite *s = bug_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_fork_status(sr, CK_NOFORK);
    cctk_setdatadir(SOURCE_ROOT "lib");
    init(0, NULL);

    srunner_set_xml(sr, LOGDIR "/bugs/bugtrack/883.xml");
    srunner_set_log(sr, LOGDIR "/bugs/bugtrack/883.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

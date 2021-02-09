/*
 * CrossFire, A Multiplayer game for X-windows
 *
 * Copyright (C) 2021 Crossfire Development Team
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
 * This is the unit tests file for a duplicate skills bug that didn't
 * get a tracker number. In short, when both the race and class had
 * the same skill, the player would get a copy of each due to the objects
 * not exactly matching when object_can_merge was called.
 */

#include <stdlib.h>
#include <check.h>
#include <global.h>
#include <sproto.h>

static void setup(void) {
}

static void teardown(void) {
}

START_TEST(test_skills) {
	// We will be using the halfling thief as our example, since both get stealing.
	player *pl;
	archetype *race_arch, *class_arch;
	living stats;
	int result, num_stealing = 0;
	object *ob_inv;
	socket_struct ns;

	race_arch = find_archetype("halfling_player");
	fail_unless(race_arch != NULL, "Cannot find halfing arch to test on.");

	class_arch = find_archetype("thief_class");
	fail_unless(class_arch != NULL, "Cannot find thief class to test on.");

	// Pretend we sent faces
	ns.faces_sent = calloc(sizeof(uint8_t), get_faces_count());
	fail_unless(ns.faces_sent != NULL, "Could not allocate socket space to remember what faces are sent.");

	pl = add_player(&ns, ADD_PLAYER_NEW | ADD_PLAYER_NO_MAP | ADD_PLAYER_NO_STATS_ROLL);
	fail_unless(pl != NULL, "Could not create a player to handle.");

	// This should ensure we don't get negative stats no matter what we choose.
	stats.Str = stats.Dex = stats.Con = stats.Int = stats.Wis = stats.Pow = stats.Cha = 20;

	result = apply_race_and_class(pl->ob, race_arch, class_arch, &stats);
	fail_unless(result == 0, "Could not apply race and class to player.");

	// Now we loop through the inventory of the object and look for the number of times we find stealing as a skill.
	// We should find it exactly once.
	ob_inv = pl->ob->inv;
	while (ob_inv) {
		if (ob_inv->type == SKILL && strcmp(ob_inv->arch->name, "skill_stealing") == 0) {
			++num_stealing;
		}
		ob_inv = ob_inv->below;
	}

	fail_unless(num_stealing < 2, "Found duplicate stealing skills in player inventory.");
	fail_unless(num_stealing > 0, "Failed to find any stealing skills in player inventory.");

	// Cleanup
	free(ns.faces_sent);
}
END_TEST

static Suite *bug_suite(void) {
    Suite *s = suite_create("bug");
    TCase *tc_core = tcase_create("Core");

    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_skills);
    tcase_set_timeout(tc_core, 0);

    return s;
}

int main(void) {
    int nf;
    Suite *s = bug_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_fork_status(sr, CK_NOFORK);
    init(0, NULL);

    srunner_set_xml(sr, LOGDIR "/bugs/bugtrack/duplicate_skills.xml");
    srunner_set_log(sr, LOGDIR "/bugs/bugtrack/duplicate_skills.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

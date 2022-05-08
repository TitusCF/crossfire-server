/*
 * static char *rcsid_check_swap_c =
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
 * This is the unit tests file for server/swap.c
 */

#include <stdlib.h>
#include <check.h>

#include "global.h"
#include "sproto.h"
#include "map.h"

void setup(void) {
    /* put any initialisation steps here, they will be run before each testcase */
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

static mapstruct *do_map(const char *tmpname, const char *reset_group) {
    mapstruct *map = get_empty_map(4, 4);
    fail_unless(first_map == map, "Map not added on list");
    map->in_memory = MAP_SWAPPED;
    map->reset_time = 1;
    map->tmpname = strdup_local(tmpname);
    if (reset_group) {
        map->reset_group = add_string(reset_group);
    }
    return map;
}

START_TEST(test_simple_reset) {
    do_map("map1", NULL);
    flush_old_maps();
    fail_unless(first_map == NULL, "Map wasn't reset");
}
END_TEST

START_TEST(test_simple_reset_group) {
    do_map("1", "rg");
    flush_old_maps();
    fail_unless(first_map == NULL, "Map wasn't reset");
}
END_TEST

START_TEST(test_reset_group_other) {
    do_map("1", "rg");
    do_map("2", NULL)->reset_time = seconds() + 50;
    flush_old_maps();
    fail_unless(first_map != NULL, "Map was reset");
    fail_unless(first_map->next == NULL, "Map group wasn't reset");
}
END_TEST

START_TEST(test_reset_group_all) {
    do_map("1", "rg");
    do_map("2", "rg");
    flush_old_maps();
    fail_unless(first_map == NULL, "Map weren't reset");
}
END_TEST

START_TEST(test_reset_group_blocked_time) {
    do_map("1", "rg");
    mapstruct *block = do_map("2", "rg");
    block->reset_time = seconds() + 50;
    flush_old_maps();
    fail_unless(first_map == block, "Map were reset");
    fail_unless(first_map->next, "Other map should not have been reset");

    block->reset_time = 1;
    flush_old_maps();
    fail_unless(first_map == NULL, "Map weren't reset");
}
END_TEST

START_TEST(test_reset_group_blocked_not_swapped) {
    do_map("1", "rg");
    mapstruct *block = do_map("2", "rg");
    block->in_memory = MAP_IN_MEMORY;
    flush_old_maps();
    fail_unless(first_map == block, "Map were reset");
    fail_unless(first_map->next, "Other map should not have been reset");

    block->in_memory = MAP_SWAPPED;
    flush_old_maps();
    fail_unless(first_map == NULL, "Map weren't reset");
}
END_TEST

Suite *swap_suite(void) {
    Suite *s = suite_create("swap");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_simple_reset);
    tcase_add_test(tc_core, test_simple_reset_group);
    tcase_add_test(tc_core, test_reset_group_other);
    tcase_add_test(tc_core, test_reset_group_all);
    tcase_add_test(tc_core, test_reset_group_blocked_time);
    tcase_add_test(tc_core, test_reset_group_blocked_not_swapped);

    return s;
}

int main(void) {
    int nf;
    Suite *s = swap_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_xml(sr, LOGDIR "/unit/server/swap.xml");
    srunner_set_log(sr, LOGDIR "/unit/server/swap.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

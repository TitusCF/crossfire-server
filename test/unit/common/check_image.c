/*
 * static char *rcsid_check_image_c =
 *   "$Id$";
 */

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

/*
 * This is the unit tests file for common/image.c
 */

#include <stdlib.h>
#include <check.h>
#include "global.h"
#include <toolkit_common.h>
#include "image.h"
#include "assets.h"

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/image.out");
    cctk_init_std_archetypes();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

static void test_faceset(const face_sets *fs) {
    for (size_t idx = 0; idx < get_faces_count(); idx++) {
        const Face *face = get_face_by_id(idx);
        int fs_id = get_face_fallback(fs->id, idx);
        const face_sets *actual = find_faceset(fs_id);
        fail_unless(actual != NULL, "couldn't find faceset %d", fs_id);
        fail_unless(idx < actual->allocated, "found face id %d but allocated %d!", idx, actual->allocated);
        fail_unless(actual->faces[idx].datalen > 0, "empty face %d (%s) for faceset %d", idx, face ? face->name : "(null)", fs_id);
    }
}

START_TEST(test_all_faces_have_data) {
    facesets_for_each(test_faceset);
}
END_TEST

Suite *image_suite(void) {
    Suite *s = suite_create("image");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_all_faces_have_data);

    return s;
}

int main(void) {
    int nf;
    Suite *s = image_suite();
    SRunner *sr = srunner_create(s);

    srunner_set_fork_status(sr, CK_NOFORK);

    srunner_set_xml(sr, LOGDIR "/unit/common/image.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/image.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

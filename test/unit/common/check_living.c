/*
 * static char *rcsid_check_living_c =
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
 * This is the unit tests file for common/living.c
 */

#include <global.h>
#include <stdlib.h>
#include <check.h>
#include <toolkit_common.h>
#include <malloc.h>

void setup(void) {
    cctk_setdatadir(BUILD_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/living.out");
    cctk_init_std_archetypes();
    init_experience();
}

void teardown(void) {
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST(test_fix_object) {
#define TESTS   50
#define SEED    80432
#define ARCHS   3
    static const char *archs[ARCHS] = { "pl_dragon", "pl_half_orc", "human_player" };

    object *ob, *grace, *mana;
    player *pl;
    int test,i;

    int wc[3][50] = {
        { 21, 3, 3, 22, 2, 10, 18, 10, 20, 14, 1, 14, 15, 9, 19, 4, 2, 3, 16, 17, 9, 2, 11, 14, 4, 11, 14, 12, 8, 20, 16, 21, 19, 3, 12, 6, 14, 8, 10, 14, 15, 17, 6, 21, 16, 9, 3, 19, 2, 20}
        , { 6, 23, 1, 2, 6, 14, 7, 6, 23, 13, 17, 9, 9, 19, 11, 18, 5, 15, 2, 3, 15, 9, 13, 13, 6, 19, 5, 21, 2, 12, 15, 6, 23, 19, 7, 20, 19, 3, 22, 7, 7, 12, 14, 15, 7, 15, 8, 10, 23, 11}
        , { 1, 8, 5, 16, 16, 15, 9, 21, 4, 16, 20, 17, 12, 19, 1, 3, 23, 4, 2, 10, 17, 3, 20, 6, 19, 21, 1, 7, 12, 20, 8, 2, 23, 17, 17, 18, 22, 17, 6, 3, 17, 3, 18, 13, 6, 3, 5, 17, 23, 1}

    };

    int maxgr[3][50] = {
        { 244, 100, 142, 190, 160, 35, 50, 78, 186, 37, 204, 246, 230, 248, 38, 66, 200, 144, 104, 156, 206, 198, 208, 46, 62, 226, 120, 220, 242, 88, 136, 250, 160, 90, 198, 58, 42, 162, 204, 100, 216, 46, 94, 32, 206, 90, 86, 40, 116, 106}
        , { 62, 118, 164, 228, 206, 186, 124, 98, 38, 228, 186, 152, 66, 112, 90, 220, 96, 236, 90, 126, 74, 236, 206, 128, 210, 242, 222, 88, 58, 74, 126, 38, 192, 170, 132, 148, 46, 116, 138, 88, 206, 34, 52, 144, 122, 36, 54, 114, 204, 52}
        , { 42, 192, 232, 110, 164, 206, 90, 238, 160, 238, 164, 246, 66, 226, 56, 114, 128, 208, 88, 250, 82, 42, 40, 228, 130, 88, 92, 102, 176, 48, 37, 60, 172, 206, 42, 148, 120, 44, 88, 162, 74, 232, 108, 110, 198, 190, 212, 80, 48, 152}

    };

    int maxsp[3][50] = {
        { 64, 234, 64, 106, 156, 234, 214, 178, 88, 222, 218, 70, 208, 100, 140, 170, 136, 218, 210, 178, 76, 112, 39, 94, 162, 100, 234, 242, 218, 236, 236, 246, 58, 178, 122, 34, 160, 92, 72, 190, 156, 238, 214, 178, 184, 166, 156, 178, 54, 102}
        , { 156, 218, 84, 48, 146, 86, 70, 78, 46, 222, 142, 142, 70, 116, 158, 164, 66, 176, 188, 218, 94, 132, 190, 80, 232, 56, 68, 190, 188, 134, 226, 222, 54, 32, 224, 160, 39, 128, 48, 188, 130, 200, 150, 112, 112, 184, 222, 110, 146, 220}
        , { 74, 64, 154, 240, 134, 126, 86, 176, 82, 246, 162, 212, 198, 236, 172, 146, 33, 88, 72, 68, 218, 166, 198, 94, 134, 48, 116, 156, 126, 214, 84, 218, 184, 126, 172, 170, 54, 224, 48, 50, 152, 164, 208, 76, 66, 154, 106, 31, 62, 212}
    };

    int maxhp[3][50] = {
        { 44, 226, 226, 36, 238, 160, 80, 158, 56, 118, 244, 112, 110, 164, 62, 220, 240, 224, 94, 82, 168, 240, 148, 112, 212, 148, 116, 136, 178, 60, 96, 48, 64, 226, 138, 200, 114, 178, 152, 112, 106, 90, 194, 50, 94, 162, 228, 70, 236, 52}
        , { 200, 34, 246, 236, 198, 114, 186, 196, 34, 128, 82, 166, 162, 70, 146, 72, 210, 108, 234, 224, 108, 164, 124, 126, 192, 66, 208, 42, 236, 138, 102, 198, 35, 64, 188, 56, 64, 230, 36, 184, 188, 132, 118, 104, 186, 106, 174, 156, 34, 142}
        , { 244, 176, 204, 100, 92, 104, 164, 50, 218, 92, 58, 90, 132, 64, 242, 226, 34, 214, 234, 152, 90, 224, 54, 194, 62, 50, 250, 188, 138, 60, 174, 240, 32, 90, 82, 74, 36, 82, 194, 226, 84, 226, 78, 124, 192, 224, 202, 82, 33, 248}
    };

    int ac[3][50] = {
        { 4, -7, -7, 6, -7, -7, -2, -7, 2, -7, -7, -7, -7, -7, 1, -7, -7, -7, -4, -2, -7, -7, -7, -7, -7, -7, -7, -7, -7, 2, -4, 4, 1, -7, -7, -7, -7, -7, -7, -7, -6, -3, -7, 3, -4, -7, -7, 0, -7, 3}
        , { 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13}
        , { 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13}
    };

    /** if you want to generate the test results, change 0 to 1 and run this test. */
#define GENERATE    0

#if GENERATE
    StringBuffer *swc = stringbuffer_new();
    StringBuffer *smaxgr = stringbuffer_new();
    StringBuffer *smaxsp = stringbuffer_new();
    StringBuffer *smaxhp = stringbuffer_new();
    StringBuffer *sac = stringbuffer_new();
    const char *sep = "";

    stringbuffer_append_printf(swc, "\tint wc[%d][%d] = {\n\t", ARCHS, TESTS);
    stringbuffer_append_printf(smaxgr, "\tint maxgr[%d][%d] = {\n\t", ARCHS, TESTS);
    stringbuffer_append_printf(smaxsp, "\tint maxsp[%d][%d] = {\n\t", ARCHS, TESTS);
    stringbuffer_append_printf(smaxhp, "\tint maxhp[%d][%d] = {\n\t", ARCHS, TESTS);
    stringbuffer_append_printf(sac, "\tint ac[%d][%d] = {\n\t", ARCHS, TESTS);
#endif

    SRANDOM(SEED);

    pl = calloc(1, sizeof(player));

    for (i = 0; i < ARCHS; i++)
    {
#if GENERATE
        stringbuffer_append_printf(swc, "%s{ ", sep);
        stringbuffer_append_printf(smaxgr, "%s{ ", sep);
        stringbuffer_append_printf(smaxsp, "%s{ ", sep);
        stringbuffer_append_printf(smaxhp, "%s{ ", sep);
        stringbuffer_append_printf(sac, "%s{ ", sep);
        sep = "";
#endif

        ob = create_archetype(archs[i]);
        fail_unless(ob != NULL, "invalid archetype %s", archs[i]);
        if (ob->type == PLAYER)
            ob->contr = pl;

        grace = create_archetype("skill_praying");
        object_insert_in_ob(grace, ob);
        mana = create_archetype("skill_pyromancy");
        object_insert_in_ob(mana, ob);

        for (test = 0; test < TESTS; test++)
        {
            ob->stats.Wis = 1 + RANDOM() % MAX_STAT;
            ob->stats.Int = 1 + RANDOM() % MAX_STAT;
            ob->stats.Pow = 1 + RANDOM() % MAX_STAT;
            ob->level = 1 + RANDOM() % settings.max_level;
            grace->level = 1 + RANDOM() % settings.max_level;
            mana->level = 1 + RANDOM() % settings.max_level;

            fix_object(ob);

#if GENERATE
            stringbuffer_append_printf(swc, "%s%d", sep, ob->stats.wc);
            stringbuffer_append_printf(smaxgr, "%s%d", sep, ob->stats.maxgrace);
            stringbuffer_append_printf(smaxsp, "%s%d", sep, ob->stats.maxsp);
            stringbuffer_append_printf(smaxhp, "%s%d", sep, ob->stats.maxhp);
            stringbuffer_append_printf(sac, "%s%d", sep, ob->stats.ac);
            sep = ", ";
#else
            fail_unless(ob->stats.wc == wc[i][test], "wc [test %d, arch %d]: got %d instead of %d", test, i, ob->stats.wc, wc[i][test]);
            fail_unless(ob->stats.maxgrace == maxgr[i][test], "gr: got %d instead of %d", ob->stats.maxgrace, maxgr[i][test]);
            fail_unless(ob->stats.maxsp == maxsp[i][test], "sp: got %d instead of %d", ob->stats.maxsp, maxsp[i][test]);
            fail_unless(ob->stats.maxhp == maxhp[i][test], "hp: got %d instead of %d", ob->stats.maxhp, maxhp[i][test]);
            fail_unless(ob->stats.ac == ac[i][test], "ac: got %d instead of %d", ob->stats.ac, ac[i][test]);
#endif
        }
        object_free2(ob, FREE_OBJ_FREE_INVENTORY);

#if GENERATE
        stringbuffer_append_string(swc, "}\n\t");
        stringbuffer_append_string(smaxgr, "}\n\t");
        stringbuffer_append_string(smaxsp, "}\n\t");
        stringbuffer_append_string(smaxhp, "}\n\t");
        stringbuffer_append_string(sac, "}\n\t");
#endif
    }

    free(pl);

#if GENERATE
    stringbuffer_append_string(swc, "\n}; \n");
    stringbuffer_append_string(smaxgr, "\n}; \n");
    stringbuffer_append_string(smaxsp, "\n}; \n");
    stringbuffer_append_string(smaxhp, "\n}; \n");
    stringbuffer_append_string(sac, "\n}; \n");
    {
        char *pwc = stringbuffer_finish(swc);
        char *pmaxgr = stringbuffer_finish(smaxgr);
        char *pmaxsp = stringbuffer_finish(smaxsp);
        char *pmaxhp = stringbuffer_finish(smaxhp);
        char *pac = stringbuffer_finish(sac);
        printf("%s\n", pwc);
        printf("%s\n", pmaxgr);
        printf("%s\n", pmaxsp);
        printf("%s\n", pmaxhp);
        printf("%s\n", pac);
        free(pwc);
        free(pmaxgr);
        free(pmaxsp);
        free(pmaxhp);
        free(pac);
    }
    fail("Here are the results.");
#endif
}
END_TEST

Suite *living_suite(void) {
    Suite *s = suite_create("living");
    TCase *tc_core = tcase_create("Core");

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core, setup, teardown);

    suite_add_tcase(s, tc_core);
    tcase_add_test(tc_core, test_fix_object);

    return s;
}

int main(void) {
    int nf;
    Suite *s = living_suite();
    SRunner *sr = srunner_create(s);

    /* to debug, uncomment this line */
    srunner_set_fork_status(sr, CK_NOFORK);

    srunner_set_xml(sr, LOGDIR "/unit/common/living.xml");
    srunner_set_log(sr, LOGDIR "/unit/common/living.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

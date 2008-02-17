/*
 * static char *rcsid_check_loader_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/*
 * This is the unit tests file for common/loader.c
 */

#include <global.h>
#include <stdlib.h>
#include <check.h>
#include <loader.h>
#include <toolkit_common.h>
#include <object.h>
#include <stringbuffer.h>

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/loader.out");
    cctk_init_std_archetypes();
}

void teardown(void)
{
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST (test_get_ob_diff)
{
    StringBuffer* buf;
    object* orc;
    archetype* arch;
    char* result;

    arch = find_archetype("orc");
    fail_unless(arch != NULL, "Can't find 'orc' archetype!");
    orc = arch_to_object(arch);
    fail_unless(orc != NULL, "Couldn't create first orc!");

    buf = stringbuffer_new();
    get_ob_diff(buf, orc, &arch->clone);
    result = stringbuffer_finish(buf);
    fail_unless(result && result[0] == '\0', "diff obj/clone was %s!", result);
    free(result);

    orc->speed = 0.5;
    orc->speed_left = arch->clone.speed_left;
    FREE_AND_COPY(orc->name, "Orc chief");

    buf = stringbuffer_new();
    get_ob_diff(buf, orc, &arch->clone);
    result = stringbuffer_finish(buf);
    fail_unless(result && strcmp(result, "name Orc chief\nspeed 0.500000\n") == 0, "diff modified obj/clone was %s!", result);
    free(result);

    orc->stats.hp = 50;
    orc->expmul = 8.5;

    buf = stringbuffer_new();
    get_ob_diff(buf, orc, &arch->clone);
    result = stringbuffer_finish(buf);
    fail_unless(result && strcmp(result, "name Orc chief\nhp 50\nexpmul 8.500000\nspeed 0.500000\n") == 0, "2n diff modified obj/clone was %s!", result);
    free(result);
}
END_TEST

Suite *loader_suite(void)
{
  Suite *s = suite_create("loader");
  TCase *tc_core = tcase_create("Core");
    /*setup and teardown will be called before each test in testcase 'tc_core' */
  tcase_add_checked_fixture(tc_core,setup,teardown);

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_get_ob_diff);

  return s;
}

int main(void)
{
  int nf;
  Suite *s = loader_suite();
  SRunner *sr = srunner_create(s);
  srunner_set_xml(sr,LOGDIR "/unit/common/loader.xml");
  srunner_set_log(sr,LOGDIR "/unit/common/loader.out");
  srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

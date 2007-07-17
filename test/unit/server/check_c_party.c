/*
 * static char *rcsid_check_c_party_c =
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
 * This is the unit tests file for server/c_party.c
 */

#include <stdlib.h>
#include <check.h>
#include <global.h>
#include <sproto.h>

void setup(void) {
    /* put any initialisation steps here, they will be run before each testcase */
}

void teardown(void)
{
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST (test_party)
{
    partylist* p1, *p2, *p3;
    object* pl;

    fail_unless(get_firstparty() == NULL, "firstparty should be NULL!");

    pl = calloc(1, sizeof(object));
    pl->name = "player";
    fail_unless(pl != NULL, "memory allocation failure");
    pl->contr = calloc(1, sizeof(player));
    fail_unless(pl->contr != NULL, "memory allocation failure");
    first_player = pl->contr; /* needed because obsolete parties uses this. */

    p1 = form_party(pl, "test1");
    fail_unless(p1 != NULL, "form_party failed.");
    fail_unless(get_firstparty() == p1, "firstparty wasn't updated");
    fail_unless(strcmp(p1->partyname, "test1") == 0, "wrong party name");
    fail_unless(p1 == pl->contr->party, "player wasn't added to party");
    fail_unless(strcmp(p1->partyleader, "player") == 0, "wrong party leader");

    p2 = form_party(pl, "test2");
    fail_unless(p2 != NULL, "form_party failed.");
    fail_unless(get_firstparty()->next == p2, "party incorrectly linked");

    remove_party(p1);

    fail_unless(get_firstparty() == p2, "party incorrectly removed");

    p3 = form_party(pl, "test3");
    fail_unless(p3 != NULL, "form_party failed");
    fail_unless(get_firstparty()->next == p3, "party p3 incorrectly linked");
    fail_unless(pl->contr->party == p3, "p3 incorrectly assigned to pl");

    obsolete_parties();
    fail_unless(get_firstparty() == p3, "party p2 wasn't removed by obsolete_parties(), party %s still there", get_firstparty() ? get_firstparty()->partyname : "NULL party?");
}
END_TEST

Suite *c_party_suite(void)
{
  Suite *s = suite_create("c_party");
  TCase *tc_core = tcase_create("Core");
    /*setup and teardown will be called before each test in testcase 'tc_core' */
  tcase_add_checked_fixture(tc_core,setup,teardown);

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_party);

  return s;
}

int main(void)
{
  int nf;
  Suite *s = c_party_suite();
  SRunner *sr = srunner_create(s);
  srunner_set_xml(sr,LOGDIR "/unit/server/c_party.xml");
  srunner_set_log(sr,LOGDIR "/unit/server/c_party.out");
  srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

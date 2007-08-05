/*
 * static char *rcsid_check_c_object_c =
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
 * This is the unit tests file for server/c_object.c
 */

#include <global.h>
#include <stdlib.h>
#include <check.h>
#include <loader.h>
#include <toolkit_common.h>
#include <sproto.h>

void setup(void) {
#if 0
/*  settings.datadir="lib";
  settings.logfilename="/unit/server/c_object.out";
    settings.archetypes="archetypes";
    settings.treasures="treasures.bld";
    init_globals();
    init_hash_table();
    init_objects();
    init_vars();
    init_block();
    read_bmap_names ();
    read_smooth();
    init_anim();
    init_archetypes();*/
#endif
}

void teardown(void)
{
    /* put any cleanup steps here, they will be run after each testcase */
}

static object *find_best_apply_object_match(object *start, object* pl, const char *params, int aflag)
{
    object *tmp, *best=NULL;
    int match_val=0,tmpmatch;

    for (tmp=start; tmp; tmp=tmp->below) {
        if (tmp->invisible) continue;
        if ((tmpmatch=item_matched_string(pl, tmp, params))>match_val) {
            if ((aflag==AP_APPLY) && (QUERY_FLAG(tmp,FLAG_APPLIED))) continue;
            if ((aflag==AP_UNAPPLY) && (!QUERY_FLAG(tmp,FLAG_APPLIED))) continue;
            match_val=tmpmatch;
            best=tmp;
        }
    }
    return best;
}

START_TEST (test_find_best_apply_object_match)
{
    object *pl, *found;
    object *gorokh, *cloak, *other;

    pl = create_archetype("kobold");
    fail_unless(pl != NULL, "can't find kobold archetype.");

    gorokh = create_archetype("cloak");
    gorokh->title = add_string("of Gorokh");
    CLEAR_FLAG(gorokh, FLAG_IDENTIFIED);
    insert_ob_in_ob(gorokh, pl);

    cloak = create_archetype("cloak");
    insert_ob_in_ob(cloak, pl);

    other = create_archetype("gem");
    insert_ob_in_ob(other, pl);

    found = find_best_apply_object_match(pl->inv, pl, "all", 0);
    fail_unless(found == other, "not found gem but %s", found ? found->name : "nothing");

    found = find_best_apply_object_match(pl->inv, pl, "cloak", 0);
    fail_unless(found == cloak, "didn't find cloak but %s", found ? found->name : "nothing");

    found = find_best_apply_object_match(pl->inv, pl, "Gorokh", 0);
    fail_unless(found == NULL, "Gorokh found %s instead of nothing", found ? found->name : "nothing??");
}
END_TEST

Suite *c_object_suite(void)
{
  Suite *s = suite_create("c_object");
  TCase *tc_core = tcase_create("Core");
    /*setup and teardown will be called before each test in testcase 'tc_core' */
  tcase_add_checked_fixture(tc_core,setup,teardown);

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_find_best_apply_object_match);

  return s;
}

int main(void)
{
  int nf;
  Suite *s = c_object_suite();
  SRunner *sr = srunner_create(s);

  settings.debug = 0;
  settings.logfilename="/unit/server/c_object.out";
  init(0, NULL);

  srunner_set_xml(sr,LOGDIR "/unit/server/c_object.xml");
  srunner_set_log(sr,LOGDIR "/unit/server/c_object.out");
  srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

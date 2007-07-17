/*
 * static char *rcsid_check_alchemy_c =
 *   "$Id: check_alchemy.c 4640 2006-06-07 21:44:18Z tchize $";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2007 Mark Wedel & Crossfire Development Team
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
 * This tests the comet spell.  My main motivation for writing this
 * was to have a consistent test I could use for performance testing.
 * But I also wanted to make sure that the results were close before and
 * after the performance changes - make the spell use less cpu time
 * but having drastically different results probably isn't a good thing
 * either.
 * To really be useful, everything needs to be compiled with profiling
 * (-pg).  This can be done like 'setenv CFLAGS -pg; ./configure;
 * make clean; make'.  The make clean is necessary because it won't
 * recompile the objects based just on changes the the CFLAGS.
 *
 * Note that this test, even after performance changes are done, still
 * isn't bad - it checks several things - map creation, spell casting,
 * etc.  It wouldn't be hard to use this as a template to test things
 * like resistance code, etc.
 */

#include <stdlib.h>
#include <check.h>

#include <global.h>
#include <sproto.h>

#define TEST_MAP_SIZE    40
#define NUM_TICKS_TO_RUN    500
#define NUM_COMETS_TO_CAST  30
#define STARTING_HP	25000

/* The percentage, either plus or minus, that the results can
 * vary from the baseline and still be considered OK.
 * Note a good sanity check to make sure you put in the correct
 * values is to set this to 0.0 - in that case, checks should
 * pass.
 */
#define HP_DELTA    5

/* The first time you set up a test, you want to dump the
 * initial values to put into the hp_row/hp_diag arrays.
 * If this is set, it prints those values instead of doing
 * a comparision.
 */
/*#define PRINT_DEBUG_HP */

mapstruct *test_map;
object *mon;

void setup(void) {
    object *mon1;
    int x, i;


    test_map = get_empty_map(TEST_MAP_SIZE, TEST_MAP_SIZE);

    mon=create_archetype("orc");
    fail_unless(mon!=NULL, "Unable to find orc object");

    /* We customize the monster a bit so it is consistent -
     * give it a bunch of HP so it can survive the attacks,
     * set it speed to 0 so it doesn't do anything, etc.
     */
    for (i=0; i<NROFATTACKS; i++)
	mon->resist[i] = 95;
    mon->stats.hp=STARTING_HP;
    mon->stats.maxhp=STARTING_HP;
    mon->speed=0.0;
    mon->speed_left=0.0;
    SET_FLAG(mon, FLAG_STAND_STILL);
    update_ob_speed(mon);

    /* We now make copies of this custom monster and put it into the
     * map.  We make a diagonal from one corner to another,
     * as well as a line across the middle of the map (\ overlayed with -)
     * We could fill most of the map with monsters, but I think the
     * diagonal + horizontal should give a pretty representative
     * value of creatures being hit.
     */
    for (x=0; x<TEST_MAP_SIZE; x++) {
	mon1 = get_object();
	copy_object(mon, mon1);
	mon1->x = x;
	mon1->y = TEST_MAP_SIZE/2;
	mon1->map = test_map;
	insert_ob_in_map(mon1, mon1->map, NULL, 0);

	if (x != TEST_MAP_SIZE/2) {
	    mon1 = get_object();
	    copy_object(mon, mon1);
	    mon1->x = x;
	    mon1->y = x;
	    mon1->map = test_map;
	    insert_ob_in_map(mon1, mon1->map, NULL, 0);
	}
    }

}

void teardown(void)
{

    free_map(test_map, 1);

}

void check_hp(const char *test, int hp_row[TEST_MAP_SIZE], int hp_diag[TEST_MAP_SIZE])
{
    object *our_mon;
    int x, diff;

#ifdef PRINT_DEBUG_HP
    printf("\nDumping debug hp for test %s\n ", test);
#endif

    /* Dump the HP of the monsters now.  We do it in 2 passes,
     * as I think it is easier to read that way.
     */
    for (x=0; x<TEST_MAP_SIZE; x++) {
	our_mon = GET_MAP_OB(test_map, x, TEST_MAP_SIZE/2);
	if (!our_mon) {
	    fail("Monster destroyed at %d, %d\n", x, TEST_MAP_SIZE/2);
	    continue;
	}
	fail_unless(mon->name == our_mon->name,
		    "Could not find our monster on the space?");

#ifdef PRINT_DEBUG_HP
	printf("%d, ", our_mon->stats.hp);
#else

	if (our_mon->stats.hp == hp_row[x]) {
	    diff = 0;
	} else 	if (our_mon->stats.hp < hp_row[x]) {
	    diff = 100 - (STARTING_HP - hp_row[x]) * 100 /
		((STARTING_HP - our_mon->stats.hp)? (STARTING_HP - our_mon->stats.hp) : 1);
	} else {
	    diff = -(100 -(STARTING_HP - our_mon->stats.hp) * 100 /
		((STARTING_HP - hp_row[x])? (STARTING_HP - hp_row[x]):1));
	}

	if (FABS(diff) > HP_DELTA) {
	    fail("Mon (%d, %d) has hp out of range (%d != %d +/- %d, diff %d)\n",
 		 our_mon->x,
		 our_mon->y, our_mon->stats.hp, hp_row[x],
		 HP_DELTA, diff);
	}
#endif
    }

#ifdef PRINT_DEBUG_HP
    printf("\n\n");
#endif

    for (x=0; x<TEST_MAP_SIZE; x++) {
	our_mon = GET_MAP_OB(test_map, x, x);
	if (!our_mon) {
	    fprintf(stderr,"Monster destroyed at %d, %d\n", x, x);
	    continue;
	}

	fail_unless(mon->name == our_mon->name,
		    "Could not find our monster on the space?");

#ifdef PRINT_DEBUG_HP
	printf("%d, ", our_mon->stats.hp);
#else
	if (our_mon->stats.hp == hp_diag[x]) {
	    diff = 0;
	}
	else if (our_mon->stats.hp < hp_diag[x]) {
	    diff = 100 - (STARTING_HP - hp_diag[x]) * 100 /
		((STARTING_HP - our_mon->stats.hp)? (STARTING_HP - our_mon->stats.hp) : 1);
	} else {
	    diff = -(100 -(STARTING_HP - our_mon->stats.hp) * 100 /
		((STARTING_HP - hp_diag[x])? (STARTING_HP - hp_diag[x]):1));
	}

	if (FABS(diff) > HP_DELTA) {
	    fail("Mon (%d, %d) has hp out of range (%d != %d +/- %d, diff %d)\n",
 		 our_mon->x,
		 our_mon->y, our_mon->stats.hp, hp_diag[x],
		 HP_DELTA, diff);
	}
#endif
    }
}


START_TEST (cast_one_comet)
{
    int	hp_row[TEST_MAP_SIZE]= {25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 24895, 24890, 24885, 24880, 24875, 24870, 24865, 24860, 24855, 24850, 24845, 24840, 24827, 24840, 24845, 24850, 24855, 24860, 24865, 24870, 24875, 24880, 24885, 24890, 24895, 25000, 25000, 25000, 25000, 25000, 25000, 25000},
	hp_diag[TEST_MAP_SIZE] = {25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 24895, 24890, 24885, 24880, 24875, 24870, 24865, 24860, 24855, 24850, 24845, 24840, 24827, 24840, 24845, 24850, 24855, 24860, 24865, 24870, 24875, 24880, 24885, 24890, 24895, 25000, 25000, 25000, 25000, 25000, 25000, 25000};
    object *comet, *rod;
    int tick;

    rod=create_archetype("rod_heavy");
    rod->level=100;
    comet = create_archetype("spell_comet");
    insert_ob_in_ob(comet, rod);

    rod->map = test_map;
    rod->x = TEST_MAP_SIZE/2;
    rod->y = TEST_MAP_SIZE - 1;

    insert_ob_in_map(rod, rod->map, NULL, 0);

    cast_spell(rod, rod, 1, rod->inv, NULL);
    for (tick=0; tick < NUM_TICKS_TO_RUN; tick++) {
	process_events(NULL);
    }

    check_hp("cast_one_comet", hp_row, hp_diag);

}
END_TEST

START_TEST (cast_random_comet)
{
    object *comet, *rod;
    int tick, num_cast=0;
    int	hp_row[TEST_MAP_SIZE]= {23522, 23380, 23217, 23172, 23137, 23007, 22882, 22762, 22655, 22527, 22412, 22407, 22307, 22312, 22217, 22235, 22235, 22217, 22312, 22412, 22412, 22470, 22620, 22770, 22915, 23060, 23200, 23335, 23365, 23400, 23535, 23670, 23800, 23930, 24055, 24180, 24200, 24325, 24445, 24565},
	hp_diag[TEST_MAP_SIZE] = {25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 23215, 23025, 22830, 22745, 22570, 22510, 22360, 22315, 22280, 22255, 22340, 22425, 22412, 22312, 22317, 22330, 22317, 22417, 22522, 22632, 22647, 22672, 22815, 22945, 23062, 23192, 23327, 23467, 23512, 23675, 23825, 23970};

    rod=create_archetype("rod_heavy");
    rod->level=100;
    comet = create_archetype("spell_comet");
    insert_ob_in_ob(comet, rod);

    rod->map = test_map;
    rod->x = TEST_MAP_SIZE/2;
    rod->y = TEST_MAP_SIZE - 1;

    insert_ob_in_map(rod, rod->map, NULL, 0);

    for (tick=0; tick < NUM_TICKS_TO_RUN; tick++) {
	if (num_cast < NUM_COMETS_TO_CAST && (tick %1)==0) {
	    remove_ob(rod);

	    /* The idea here on the x is to shuffle the spaces around
	     * a little, as a more typical case is comets
	     * blowing up on different spaces.
	     */
	    rod->x = (tick *59) % 37;
	    rod->y = TEST_MAP_SIZE - 1;
	    rod->map = test_map;
	    insert_ob_in_map(rod, rod->map, NULL, 0);

	    cast_spell(rod, rod, 1, rod->inv, NULL);
	    num_cast++;
	}
	process_events(NULL);
    }
    check_hp("cast_random_comet", hp_row, hp_diag);

}
END_TEST


START_TEST (cast_bunch_comet)
{
    object *comet, *rod;
    int tick, num_cast=0;
    int	hp_row[TEST_MAP_SIZE]= {25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 21850, 21700, 21550, 21400, 21250, 21100, 20950, 20800, 20650, 20500, 20350, 20200, 19810, 20200, 20350, 20500, 20650, 20800, 20950, 21100, 21250, 21400, 21550, 21700, 21850, 25000, 25000, 25000, 25000, 25000, 25000, 25000},
	hp_diag[TEST_MAP_SIZE] = {25000, 25000, 25000, 25000, 25000, 25000, 25000, 25000, 21850, 21700, 21550, 21400, 21250, 21100, 20950, 20800, 20650, 20500, 20350, 20200, 19810, 20200, 20350, 20500, 20650, 20800, 20950, 21100, 21250, 21400, 21550, 21700, 21850, 25000, 25000, 25000, 25000, 25000, 25000, 25000};



    rod=create_archetype("rod_heavy");
    rod->level=100;
    comet = create_archetype("spell_comet");
    insert_ob_in_ob(comet, rod);

    rod->map = test_map;
    rod->x = TEST_MAP_SIZE/2;
    rod->y = TEST_MAP_SIZE - 1;

    insert_ob_in_map(rod, rod->map, NULL, 0);

    for (tick=0; tick < NUM_TICKS_TO_RUN; tick++) {
	if (num_cast < NUM_COMETS_TO_CAST && (tick %1)==0) {
	    cast_spell(rod, rod, 1, rod->inv, NULL);
	    num_cast++;
	}
	process_events(NULL);
    }
    check_hp("cast_bunch_comet", hp_row, hp_diag);

}
END_TEST

Suite *comet_suite(void)
{
    Suite *s = suite_create("comet");
    TCase *tc_core = tcase_create("Core");

    /* check by defaults has a 2 second timeout - that isn't
     * fast enough on my system - a run of 30 comets takes about
     * 7 seconds.  Setting this to 20 is enough, but on a slower
     * system may not be.
     */
    tcase_set_timeout(tc_core, 20);

    /*setup and teardown will be called before each test in testcase 'tc_core' */
    tcase_add_checked_fixture(tc_core,setup,teardown);

    suite_add_tcase (s, tc_core);
    tcase_add_test(tc_core, cast_one_comet);
    tcase_add_test(tc_core, cast_bunch_comet);
    tcase_add_test(tc_core, cast_random_comet);

    return s;
}

int main(void)
{
    int nf;

    Suite *s = comet_suite();
    SRunner *sr = srunner_create(s);

    /* Don't want to fork - if we do, we lose the profile (-pg)
     * compiled information, which is what I used to determine if
     * things are more efficient.
     */
    srunner_set_fork_status (sr, CK_NOFORK);

    /* Only want to run this once, so don't put it in setup() */
    init(0, NULL);

    srunner_set_xml(sr,LOGDIR "/unit/server/comet.xml");
    srunner_set_log(sr,LOGDIR "/unit/server/comet.out");
    srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    fprintf(stderr,"Got %ld supressions, %ld spell merges, %ld full tables\n",
	    statistics.spell_suppressions, statistics.spell_merges, statistics.spell_hash_full);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

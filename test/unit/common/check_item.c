/*
 * static char *rcsid_check_item_c =
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
 * This is the unit tests file for common/item.c
 */

#include <global.h>
#include <stdlib.h>
#include <check.h>
#include <loader.h>
#include <toolkit_common.h>

void setup(void) {
    cctk_setdatadir(SOURCE_ROOT "lib");
    cctk_setlog(LOGDIR "/unit/common/item.out");
    cctk_init_std_archetypes();
    init_gods();
}

void teardown(void)
{
    /* put any cleanup steps here, they will be run after each testcase */
}

START_TEST (test_describe_item)
{
    object *test;
    char buf[5000];
    int check;
    treasurelist *list;

    static const char *archs[] = {
        "gem",
        "food",
        "lantern_1",
        "blood",
        "ring",
        "amulet",
        "goblin",
        "potion_restoration",
        "axe_2",
        "elven_bow",
        "helmet_of_brilliance",
        "holy_avenger",
        "scimitar",
        "shield",
        "fl_corpse",
        "booze",
        "poison",
        "deathshead",
        "zombie",
        "talisman_evocation",
        "talisman_sorcery",
        "dragon",
        "titan",
        "speedboots",
        "t_star1",
        "girdle_con",
        "god_finger",
        "shining_finger",
        "high_boots_w",
        "plate_mail",
        "robe",
        "scale_mail",
        "DShieldms",
        "holy_shield",
        NULL };
    static const char *arch_results[] = {
        "",
        "(food+200)",
        "(glowing)",
        "",
        "",
        "",
        "(slow movement)(wield weapon)(archer)(wear armour)(wear ring)",
        "",
        "(dam+6)(weapon speed 10)(Attacks: physical)",
        "(wc+1)(dam+15)(Attacks: physical)",
        "(Int+2)(Pow+2)(ac+2)(item_power +5)(magic+1)(armour +5)",
        "(Str+1)(Wis+2)(Cha+2)(dam+15)(item_power +25)(weapon speed 6)(Attacks: weaponmagic, blinding)(resist magic +30)(resist drain +100)",
        "(dam+8)(weapon speed 8)(Attacks: physical)",
        "(ac+1)(armour +5)",
        "(food+600)",
        "(food+100)(slay vial_poison:poison)",
        "",
        "(extremely fast movement)(undead)(spellcaster)(Spell abilities:)(paralyze)(fear ability)(cause black death)(cause red death)(face of death)(meteor swarm)(hellfire)(burning hands)(large fireball)(mass confusion)(negative energy bolt)(negative energy ball)(slow ability)(Attacks: physical, cold)(armour +75)(resist magic +100)(resist fire +100)(resist electricity +90)(resist cold +100)(resist confusion +100)(resist acid +90)(resist drain +100)(resist weaponmagic +80)(resist ghosthit +100)(resist poison +100)(resist slow +100)(resist paralyzation +100)(resist fear +100)(resist cancellation +65)(resist depletion +100)(resist death +100)(resist chaos +100)(resist counterspell +65)(resist god power +80)(resist blindness +100)",
        "(slow movement)(undead)(Attacks: physical)(resist cold +50)(resist fear +100)",
        "",
        "",
        "(normal movement)(see invisible)(spellcaster)(Spell abilities:)(burning hands ability)(medium fireball ability)(fear ability)(Attacks: physical)(resist fire +100)(resist cold -100)(resist confusion -100)(resist fear +100)(resist blindness +50)",
        "(fast movement)(see invisible)(wield weapon)(archer)(wear armour)(wear ring)(read scroll)(spellcaster)(Spell abilities:)(paralyze)(fear ability)(small lightning)(large lightning)(slow ability)(resist magic +50)(resist electricity +100)(resist fear +100)",
        "(speed +6)(item_power +6)(armour +3)",
        "(dam+3)(weapon speed 2)(Attacks: physical)",
        "(Con+2)(item_power +1)",
        "(Str+2)(Dex-1)(dam+3)(item_power +2)(armour +3)",
        "(Str+2)(dam+3)(item_power +1)(armour +3)",
        "(Cha+1)(ac+1)(Spell regen penalty 4)(armour +4)(resist blindness +1)",
        "(ac+5)(Max speed 0.70)(Spell regen penalty 30)(armour +40)",
        "(ac+1)(Max speed 1.20)",
        "(ac+3)(Max speed 0.90)(Spell regen penalty 10)(armour +20)",
        "(Cha-5)(ac+7)(item_power +10)(reflect spells)(reflect missiles)(armour +15)(resist fire +30)(resist drain +100)(resist ghosthit +80)",
        "(ac+4)(item_power +6)(armour +10)(resist drain +100)(resist ghosthit +50)",
        NULL };

    /* if you change the order, the result will quite certainly change, as the generation depends on the value returned
        by rand() - changing the order changes the item generated... */
    static const char *treasures[] = {
        "random_knowledge",
        "missile_weapons",
        "random_talisman",
        "rare_weapons",
        "random_food",
        "random_artifact",
        "random_read",
        "random_amulet",
        "random_artifact",
        "random_amulet",
        "random_artifact",
        "standard_old",
        NULL
    };
    static const char *treasure_results[] = {
        "",
        "(wc+1)(dam+2)(item_power +2)(Attacks: physical)",
        "",
        "(dam+6)(item_power +3)(weapon speed 7)(Attacks: physical)",
        "(food+200)",
        "(Con+2)(Cha-1)(dam+10)(item_power +15)(weapon speed 5)(regeneration+1)(Attacks: weaponmagic)(resist drain +100)(resist poison +30)",
        "",
        "",
        "(Str+1)(dam+9)(item_power +20)(weapon speed 6)(regeneration+1)(Attacks: electricity, drain)(resist magic +30)(resist electricity +30)(resist drain +100)",
        "",
        "(Str+1)(armour +3)",
        "",
        NULL
    };

    for (check = 0; archs[check] != NULL; check++) {
        test = cctk_create_game_object(archs[check]);
        fail_unless(test != NULL, "couldn't create arch %s", archs[check]);
        SET_FLAG(test, FLAG_IDENTIFIED);
        describe_item(test, NULL, buf, sizeof(buf));

        /* if you're adding items, uncomment that so make finding the good value easier. */
        /*
        if (strcmp(buf, arch_results[check]))
            printf("describe_item(%s) returned \"%s\" instead of \"%s\"\n", archs[check], buf, arch_results[check]);
        */

        fail_unless(strcmp(buf, arch_results[check]) == 0, "describe_item(%s) returned \"%s\" instead of \"%s\"", archs[check], buf, arch_results[check]);

        free_object(test);
    }

    /* we initialize the random generator to always be able to reproduce, and we use rand() in case RANDOM is something else. */
    srand(100);
    for (check = 0; treasures[check] != NULL; check++) {
        list = find_treasurelist(treasures[check]);
        fail_unless(list != NULL, "couldn't find treasure list %s", treasures[check]);
        test = generate_treasure(list, 50);
        fail_if(test == NULL, "couldn't create item from treasure list %s", treasures[check]);
        SET_FLAG(test, FLAG_IDENTIFIED);
        describe_item(test, NULL, buf, sizeof(buf));

        /* if you're adding lists, uncomment that so make finding the good value easier. */
        /*
        if (strcmp(buf, treasure_results[check]))
            printf("Item %d describe_item(treasure %s) returned \"%s\" instead of \"%s\"\n", check, treasures[check], buf, treasure_results[check]);
        */

        fail_unless(strcmp(buf, treasure_results[check]) == 0, "describe_item(treasure %s) returned \"%s\" instead of \"%s\"", treasures[check], buf, treasure_results[check]);

        free_object(test);

    }
}
END_TEST

Suite *item_suite(void)
{
  Suite *s = suite_create("item");
  TCase *tc_core = tcase_create("Core");
    /*setup and teardown will be called before each test in testcase 'tc_core' */
  tcase_add_checked_fixture(tc_core,setup,teardown);

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_describe_item);

  return s;
}

int main(void)
{
  int nf;
  Suite *s = item_suite();
  SRunner *sr = srunner_create(s);
  srunner_set_xml(sr,LOGDIR "/unit/common/item.xml");
  srunner_set_log(sr,LOGDIR "/unit/common/item.out");
  srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

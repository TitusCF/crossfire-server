/*
 * static char *rcsid_check_object_c =
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
 * This is the unit tests file for common/object.c
 */

#include <stdlib.h>
#include <check.h>


void setup(void) {
    /* put any initialisation steps here, they will be run before each testcase */
}

void teardown(void)
{
    /* put any cleanup steps here, they will be run after each testcase */
}

/*
 * Things to check
 * can_merge
 * sum_weight
 * object_get_env_recursive
 * is_player_inv
 * dump_object
 * dump_all_objects
 * find_object
 * find_object_name
 * free_all_object_data
 * get_owner
 * clear_owner
 * set_owner
 * copy_owner
 * reset_object
 * clear_object
 * copy_object
 * get_object
 * update_turn_face
 * update_ob_speed
 * remove_from_active_list
 * update_object
 * free_object
 * count_free
 * count_used
 * count_active
 * sub_weight
 * remove_ob
 * merge_ob
 * insert_ob_in_map_at
 * insert_ob_in_map
 * replace_insert_ob_in_map
 * get_split_ob
 * decrease_ob_nr
 * add_weight
 * insert_ob_in_ob
 * check_move_on
 * present_arch
 * present
 * present_in_ob
 * present_in_ob_by_name
 * present_arch_in_ob
 * flag_inv
 * unflag_inv
 * set_cheat
 * find_free_spot
 * find_first_free_spot
 * get_search_arr
 * find_dir
 * distance
 * find_dir_2
 * absdir
 * dirdiff
 * can_see_monsterP
 * can_pick
 * object_create_clone
 * was_destroyed
 * find_obj_by_type_subtype
 * get_ob_key_link
 * get_ob_key_value
 * set_ob_key_value
 * find_best_weapon_used_match
 * item_matched_string
 */
/** This is the test to check the behaviour of the method
 *  int can_merge(object *ob1, object *ob2);
 */
START_TEST (test_can_merge)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  signed long sum_weight(object *op);
 */
START_TEST (test_sum_weight)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *object_get_env_recursive(object *op);
 */
START_TEST (test_object_get_env_recursive)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *is_player_inv(object *op);
 */
START_TEST (test_is_player_inv)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void dump_object(object *op);
 */
START_TEST (test_dump_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void dump_all_objects(void);
 */
START_TEST (test_dump_all_objects)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *find_object(tag_t i);
 */
START_TEST (test_find_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *find_object_name(const char *str);
 */
START_TEST (test_find_object_name)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void free_all_object_data(void);
 */
START_TEST (test_free_all_object_data)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *get_owner(object *op);
 */
START_TEST (test_get_owner)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void clear_owner(object *op);
 */
START_TEST (test_clear_owner)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void set_owner(object *op, object *owner);
 */
START_TEST (test_set_owner)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void copy_owner(object *op, object *clone);
 */
START_TEST (test_copy_owner)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void reset_object(object *op);
 */
START_TEST (test_reset_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void clear_object(object *op);
 */
START_TEST (test_clear_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void copy_object(object *op2, object *op);
 */
START_TEST (test_copy_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *get_object(void);
 */
START_TEST (test_get_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void update_turn_face(object *op);
 */
START_TEST (test_update_turn_face)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void update_ob_speed(object *op);
 */
START_TEST (test_update_ob_speed)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void remove_from_active_list(object *op);
 */
START_TEST (test_remove_from_active_list)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void update_object(object *op, int action);
 */
START_TEST (test_update_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void free_object(object *ob);
 */
START_TEST (test_free_object)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int count_free(void);
 */
START_TEST (test_count_free)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int count_used(void);
 */
START_TEST (test_count_used)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int count_active(void);
 */
START_TEST (test_count_active)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void sub_weight(object *op, signed long weight);
 */
START_TEST (test_sub_weight)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void remove_ob(object *op);
 */
START_TEST (test_remove_ob)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *merge_ob(object *op, object *top);
 */
START_TEST (test_merge_ob)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *insert_ob_in_map_at(object *op, mapstruct *m, object *originator, int flag, int x, int y);
 */
START_TEST (test_insert_ob_in_map_at)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *insert_ob_in_map(object *op, mapstruct *m, object *originator, int flag);
void replace_insert_ob_in_map(const char *arch_string, object *op);
 */
START_TEST (test_insert_ob_in_map)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void replace_insert_ob_in_map(const char *arch_string, object *op);
 */
START_TEST (test_replace_insert_ob_in_map)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *get_split_ob(object *orig_ob, uint32 nr);
 */
START_TEST (test_get_split_ob)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *decrease_ob_nr(object *op, uint32 i);
 */
START_TEST (test_decrease_ob_nr)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void add_weight(object *op, signed long weight);
 */
START_TEST (test_add_weight)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *insert_ob_in_ob(object *op, object *where);
 */
START_TEST (test_insert_ob_in_ob)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int check_move_on(object *op, object *originator);
 */
START_TEST (test_check_move_on)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *present_arch(const archetype *at, mapstruct *m, int x, int y);
 */
START_TEST (test_present_arch)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *present(unsigned char type, mapstruct *m, int x, int y);
 */
START_TEST (test_present)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *present_in_ob(unsigned char type, const object *op);
 */
START_TEST (test_present_in_ob)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *present_in_ob_by_name(int type, const char *str, const object *op);
 */
START_TEST (test_present_in_ob_by_name)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *present_arch_in_ob(const archetype *at, const object *op);
 */
START_TEST (test_present_arch_in_ob)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void flag_inv(object *op, int flag);
void unflag_inv(object *op, int flag);
 */
START_TEST (test_flag_inv)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void unflag_inv(object *op, int flag);
 */
START_TEST (test_unflag_inv)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void set_cheat(object *op);
 */
START_TEST (test_set_cheat)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int find_free_spot(const object *ob, mapstruct *m, int x, int y, int start, int stop);
 */
START_TEST (test_find_free_spot)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int find_first_free_spot(const object *ob, mapstruct *m, int x, int y);
 */
START_TEST (test_find_first_free_spot)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void get_search_arr(int *search_arr);
 */
START_TEST (test_get_search_arr)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int find_dir(mapstruct *m, int x, int y, object *exclude);
 */
START_TEST (test_find_dir)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int distance(const object *ob1, const object *ob2);
 */
START_TEST (test_distance)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int find_dir_2(int x, int y);
 */
START_TEST (test_find_dir_2)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int absdir(int d);
 */
START_TEST (test_absdir)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int dirdiff(int dir1, int dir2);
 */
START_TEST (test_dirdiff)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int can_see_monsterP(mapstruct *m, int x, int y, int dir);
 */
START_TEST (test_can_see_monsterP)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int can_pick(const object *who, const object *item);
 */
START_TEST (test_can_pick)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *object_create_clone(object *asrc);
 */
START_TEST (test_object_create_clone)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int was_destroyed(const object *op, tag_t old_tag);
 */
START_TEST (test_was_destroyed)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *find_obj_by_type_subtype(const object *who, int type, int subtype);
 */
START_TEST (test_find_obj_by_type_subtype)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  key_value *get_ob_key_link(const object *ob, const char *key);
 */
START_TEST (test_get_ob_key_link)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  const char *get_ob_key_value(const object *op, const char *const key);
 */
START_TEST (test_get_ob_key_value)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int set_ob_key_value(object *op, const char *key, const char *value, int add_key);
 */
START_TEST (test_set_ob_key_value)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *find_best_weapon_used_match(object *pl, const char *params);
 */
START_TEST (test_find_best_weapon_used_match)
{
    /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int item_matched_string(object *pl, object *op, const char *name);
 */
START_TEST (test_item_matched_string)
{
    /*TESTME*/
}
END_TEST


Suite *object_suite(void)
{
  Suite *s = suite_create("object");
  TCase *tc_core = tcase_create("Core");
    /*setup and teardown will be called before each test in testcase 'tc_core' */
  tcase_add_checked_fixture(tc_core,setup,teardown); 

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_can_merge);
  tcase_add_test(tc_core, test_sum_weight);
  tcase_add_test(tc_core, test_object_get_env_recursive);
  tcase_add_test(tc_core, test_is_player_inv);
  tcase_add_test(tc_core, test_dump_object);
  tcase_add_test(tc_core, test_dump_all_objects);
  tcase_add_test(tc_core, test_find_object);
  tcase_add_test(tc_core, test_find_object_name);
  tcase_add_test(tc_core, test_free_all_object_data);
  tcase_add_test(tc_core, test_get_owner);
  tcase_add_test(tc_core, test_clear_owner);
  tcase_add_test(tc_core, test_set_owner);
  tcase_add_test(tc_core, test_copy_owner);
  tcase_add_test(tc_core, test_reset_object);
  tcase_add_test(tc_core, test_clear_object);
  tcase_add_test(tc_core, test_copy_object);
  tcase_add_test(tc_core, test_get_object);
  tcase_add_test(tc_core, test_update_turn_face);
  tcase_add_test(tc_core, test_update_ob_speed);
  tcase_add_test(tc_core, test_remove_from_active_list);
  tcase_add_test(tc_core, test_update_object);
  tcase_add_test(tc_core, test_free_object);
  tcase_add_test(tc_core, test_count_free);
  tcase_add_test(tc_core, test_count_used);
  tcase_add_test(tc_core, test_count_active);
  tcase_add_test(tc_core, test_sub_weight);
  tcase_add_test(tc_core, test_remove_ob);
  tcase_add_test(tc_core, test_merge_ob);
  tcase_add_test(tc_core, test_insert_ob_in_map_at);
  tcase_add_test(tc_core, test_insert_ob_in_map);
  tcase_add_test(tc_core, test_replace_insert_ob_in_map);
  tcase_add_test(tc_core, test_get_split_ob);
  tcase_add_test(tc_core, test_decrease_ob_nr);
  tcase_add_test(tc_core, test_add_weight);
  tcase_add_test(tc_core, test_insert_ob_in_ob);
  tcase_add_test(tc_core, test_check_move_on);
  tcase_add_test(tc_core, test_present_arch);
  tcase_add_test(tc_core, test_present);
  tcase_add_test(tc_core, test_present_in_ob);
  tcase_add_test(tc_core, test_present_in_ob_by_name);
  tcase_add_test(tc_core, test_present_arch_in_ob);
  tcase_add_test(tc_core, test_flag_inv);
  tcase_add_test(tc_core, test_unflag_inv);
  tcase_add_test(tc_core, test_set_cheat);
  tcase_add_test(tc_core, test_find_free_spot);
  tcase_add_test(tc_core, test_find_first_free_spot);
  tcase_add_test(tc_core, test_get_search_arr);
  tcase_add_test(tc_core, test_find_dir);
  tcase_add_test(tc_core, test_distance);
  tcase_add_test(tc_core, test_find_dir_2);
  tcase_add_test(tc_core, test_absdir);
  tcase_add_test(tc_core, test_dirdiff);
  tcase_add_test(tc_core, test_can_see_monsterP);
  tcase_add_test(tc_core, test_can_pick);
  tcase_add_test(tc_core, test_object_create_clone);
  tcase_add_test(tc_core, test_was_destroyed);
  tcase_add_test(tc_core, test_find_obj_by_type_subtype);
  tcase_add_test(tc_core, test_get_ob_key_link);
  tcase_add_test(tc_core, test_get_ob_key_value);
  tcase_add_test(tc_core, test_set_ob_key_value);
  tcase_add_test(tc_core, test_find_best_weapon_used_match);
  tcase_add_test(tc_core, test_item_matched_string);

  return s;
}

int main(void)
{
  int nf;
  Suite *s = object_suite();
  SRunner *sr = srunner_create(s);
  srunner_set_xml(sr,LOGDIR "/unit/common/object.xml");
  srunner_set_log(sr,LOGDIR "/unit/common/object.out");
  srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

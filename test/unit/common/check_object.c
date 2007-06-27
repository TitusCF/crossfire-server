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

#include <global.h>
#include <stdlib.h>
#include <check.h>
#include <loader.h>
#include <toolkit_common.h>


void setup(void) {
  cctk_setdatadir(SOURCE_ROOT "lib");
  cctk_setlog(LOGDIR "/unit/common/object.out");
  printf("set log to %s\n",LOGDIR "/unit/common/object.out");
  cctk_init_std_archetypes();
}

void teardown(void)
{
    /* put any cleanup steps here, they will be run after each testcase */
}

/*
 * Things to check
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
 * item_matched_string
 */
/** This is the test to check the behaviour of the method
 *  int can_merge(object *ob1, object *ob2);
 */
START_TEST (test_can_merge)
{
  object *ob1;
  object *ob2;
  ob1=cctk_create_game_object(NULL);
  ob2=cctk_create_game_object(NULL);
  fail_unless(can_merge(ob1,ob2),"Should be able to merge 2 same object");
  ob2->name=add_string("Not same name");
  fail_unless(!can_merge(ob1,ob2),"Should not be able to merge 2 object with different names");
  ob2=cctk_create_game_object(NULL);
  ob2->type++;
  fail_unless(!can_merge(ob1,ob2),"Should not be able to merge 2 object with different types");
  ob2=cctk_create_game_object(NULL);
  ob1->nrof=(1UL<<31)-1;
  ob2->nrof=1;
  fail_unless(!can_merge(ob1,ob2),"Should not be able to merge 2 object if result nrof goes to 1<<31 or higher");
  /*TESTME*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  signed long sum_weight(object *op);
 */
START_TEST (test_sum_weight)
{
  object *ob1;
  object *ob2;
  object *ob3;
  object *ob4;
  unsigned long sum;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  ob4 = cctk_create_game_object(NULL);
  ob1->weight=10; /*This should not be taken into account by sum_weight*/
  ob1->type=CONTAINER;
  ob1->stats.Str=40; /*40% reduction of weight*/
  ob2->weight=6;
  ob2->nrof=10;
  ob3->weight=7;
  ob4->weight=8;
  insert_ob_in_ob(ob2,ob1);
  insert_ob_in_ob(ob3,ob1);
  insert_ob_in_ob(ob4,ob1);
  sum=sum_weight(ob1);
  fail_unless(sum==45,"Sum of object's inventory should be 45 ((6*10+7+8)*.6) but was %lu.",sum);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *object_get_env_recursive(object *op);
 */
START_TEST (test_object_get_env_recursive)
{
  object *ob1;
  object *ob2;
  object *ob3;
  object *ob4;
  object *result;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  ob4 = cctk_create_game_object(NULL);
  insert_ob_in_ob(ob2,ob1);
  insert_ob_in_ob(ob3,ob2);
  insert_ob_in_ob(ob4,ob3);
  result=object_get_env_recursive(ob4);
  fail_unless(result==ob1,"Getting top level container for ob4(%p) should bring ob1(%p) but brought %p.",ob4,ob1,result);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *get_player_container(object *op);
 */
START_TEST (test_get_player_container)
{
  object *ob1;
  object *ob2;
  object *ob3;
  object *ob4;
  object *result;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  ob4 = cctk_create_game_object(NULL);
  insert_ob_in_ob(ob2,ob1);
  insert_ob_in_ob(ob3,ob2);
  insert_ob_in_ob(ob4,ob3);
  result=get_player_container(ob4);
  fail_unless(result==NULL,"Getting containing player for ob4(%p) should bring NULL but brought %p while not contained in a player.",ob4,result);
  ob1->type=PLAYER;
  result=get_player_container(ob4);
  fail_unless(result==ob1,"Getting containing player for ob4(%p) should bring ob1(%p) but brought %p while ob1 is player.",ob4,ob1,result);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void dump_object(object *op);
 */
START_TEST (test_dump_object)
{
  object *ob1;
  object *ob2;
  object *ob3;
  char buf[HUGE_BUF];
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  insert_ob_in_ob(ob2,ob1);
  insert_ob_in_ob(ob3,ob2);
  dump_object(ob1, buf, sizeof(buf));
  fail_unless(strstr(buf,"arch")!=NULL,"The object dump should contain 'arch' but was %s",buf);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void dump_all_objects(void);
 */
START_TEST (test_dump_all_objects)
{
  object *ob1;
  object *ob2;
  object *ob3;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  dump_all_objects(); /*Should not crash, that all i can test*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *find_object(tag_t i);
 */
START_TEST (test_find_object)
{
  object *ob1;
  object *result;
  ob1 = cctk_create_game_object(NULL);
  ob1 = cctk_create_game_object(NULL);
  ob1 = cctk_create_game_object(NULL);
  result=find_object(ob1->count);
  fail_unless(result==ob1,"Should find ob1(%p) while search for item %d but got %p",ob1,ob1->count,result);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *find_object_name(const char *str);
 */
START_TEST (test_find_object_name)
{
  object *ob1;
  object *result;
  ob1 = cctk_create_game_object(NULL);
  ob1->name=add_string("This is a name");
  ob1 = cctk_create_game_object(NULL);
  ob1->name=add_string("This is another name");
  ob1 = cctk_create_game_object(NULL);
  ob1->name=add_string("This is the key name");
  result=find_object_name(add_string("This is the key name"));
  fail_unless(result==ob1,"Searching for object with name 'This is the key name' returned %p(%s) instead of ob1(%p)",result,result?result->name:"null",ob1);
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
  object *ob1;
  object *ob2;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  set_owner(ob2,ob1);
  CLEAR_FLAG(ob1, FLAG_REMOVED);
  CLEAR_FLAG(ob2, FLAG_REMOVED);
  fail_unless(get_owner(ob2)==ob1,"Owner of ob2(%p) shoud be ob1(%p) but was %p",ob2,ob1,get_owner(ob2));
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void clear_owner(object *op);
 */
START_TEST (test_clear_owner)
{
  object *ob1;
  object *ob2;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  set_owner(ob2,ob1);
  fail_unless(ob2->owner!=NULL,"Prior to testing clear_owner, owner of ob2 was wrongly initialized");
  clear_owner(ob2);
  fail_unless(ob2->owner==NULL,"After clear_owner ob2 still had an owner");
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void set_owner(object *op, object *owner);
 */
START_TEST (test_set_owner)
{
  object *ob1;
  object *ob2;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  set_owner(ob2,ob1);
  fail_unless(ob2->owner==ob1,"After set_owner ob2(%p) owner should be ob1(%p) but was (%p)",ob2,ob1,ob2->owner);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void copy_owner(object *op, object *clone);
 */
START_TEST (test_copy_owner)
{
  object *ob1;
  object *ob2;
  object *ob3;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  set_owner(ob2,ob1);
  copy_owner(ob3,ob2);
  fail_unless(get_owner(ob2)==get_owner(ob3),"After copy_owner, ob3 and ob2 should have same owner (ob1=%p) but got %p and %p",
              get_owner(ob3),get_owner(ob2));
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void reset_object(object *op);
 */
START_TEST (test_reset_object)
{
  object *ob1;
  object *result;
  ob1 = cctk_create_game_object(NULL);
  reset_object(ob1);
  fail_unless(ob1->name == NULL,"Field name of ob1 was not NULLified by reset_object");
  fail_unless(ob1->name_pl == NULL,"Field name_pl of ob1 was not NULLified by reset_object");
  fail_unless(ob1->title == NULL,"Field title of ob1 was not NULLified by reset_object");
  fail_unless(ob1->race == NULL,"Field race of ob1 was not NULLified by reset_object");
  fail_unless(ob1->slaying == NULL,"Field slaying of ob1 was not NULLified by reset_object");
  fail_unless(ob1->skill == NULL,"Field skill of ob1 was not NULLified by reset_object");
  fail_unless(ob1->msg == NULL,"Field msg of ob1 was not NULLified by reset_object");
  fail_unless(ob1->materialname == NULL,"Field materialname of ob1 was not NULLified by reset_object");
  fail_unless(ob1->lore == NULL,"Field lore of ob1 was not NULLified by reset_object");
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void clear_object(object *op);
 */
START_TEST (test_clear_object)
{
  object *ob1;
  const char* reference;
  ob1 = cctk_create_game_object(NULL);
  cctk_set_object_strings(ob1,"This is a test String");
  reference=add_string("This is a test String");
  clear_object(ob1);
  fail_unless(ob1->name == NULL,"Field name of ob1 was not cleaned by clear_object");
  fail_unless(ob1->name_pl == NULL,"Field name_pl of ob1 was not cleaned by clear_object");
  fail_unless(ob1->title == NULL,"Field title of ob1 was not cleaned by clear_object");
  fail_unless(ob1->race == NULL,"Field race of ob1 was not cleaned by clear_object");
  fail_unless(ob1->slaying == NULL,"Field slaying of ob1 was not cleaned by clear_object");
  fail_unless(ob1->skill == NULL,"Field skill of ob1 was not cleaned by clear_object");
  fail_unless(ob1->msg == NULL,"Field msg of ob1 was not cleaned by clear_object");
  fail_unless(ob1->materialname == NULL,"Field materialname of ob1 was not cleaned by clear_object");
  fail_unless(ob1->lore == NULL,"Field lore of ob1 was not cleaned by clear_object");
  fail_unless(query_refcount(reference)==1,
              "The number of references to string should drop back to 1 but was %d",query_refcount(reference));
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void copy_object(object *op2, object *op);
 */
START_TEST (test_copy_object)
{
  object *ob1;
  object *ob2;
  const char* reference;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  cctk_set_object_strings(ob1,"test String1");
  cctk_set_object_strings(ob2,"test String2");
  reference=add_string("test String2");
  copy_object(ob1,ob2);
  fail_unless(ob1->name==ob2->name,"Field name of ob1 should match ob2");
  fail_unless(ob1->name_pl==ob2->name_pl,"Field name_pl of ob1 should match ob2");
  fail_unless(ob1->title==ob2->title,"Field title of ob1 should match ob2");
  fail_unless(ob1->race==ob2->race,"Field race of ob1 should match ob2");
  fail_unless(ob1->slaying==ob2->slaying,"Field slaying of ob1 should match ob2");
  fail_unless(ob1->skill==ob2->skill,"Field skill of ob1 should match ob2");
  fail_unless(ob1->msg==ob2->msg,"Field msg of ob1 should match ob2");
  fail_unless(ob1->materialname==ob2->materialname,"Field materialname of ob1 should match ob2");
  fail_unless(ob1->lore==ob2->lore,"Field lore of ob1 should match ob2");
  fail_unless(query_refcount(reference)==1, "refcount of marker string is not dropped to 1 after copy object, some string field were not cleaned. refcount: %d",query_refcount(reference));
}
END_TEST


/**
 *  This is the test to check the behaviour of the method
 *  object *get_object(void);
 */
START_TEST (test_get_object)
{
  object* ob;
  long int i;
  ob=get_object();
  fail_unless(ob!=NULL,"Should get an object after calling get_object()");
  fail_unless(ob->name==NULL,"Field name has not been nullified by get_object()");
  fail_unless(ob->name_pl==NULL,"Field name_pl has not been nullified by get_object()");
  fail_unless(ob->title==NULL,"Field title has not been nullified by get_object()");
  fail_unless(ob->race==NULL,"Field race has not been nullified by get_object()");
  fail_unless(ob->slaying==NULL,"Field slaying has not been nullified by get_object()");
  fail_unless(ob->skill==NULL,"Field skill has not been nullified by get_object()");
  fail_unless(ob->lore==NULL,"Field lore has not been nullified by get_object()");
  fail_unless(ob->msg==NULL,"Field msg has not been nullified by get_object()");
  fail_unless(ob->materialname==NULL,"Field materialname has not been nullified by get_object()");
  fail_unless(ob->prev==NULL,"Field prev has not been nullified by get_object()");
  fail_unless(ob->active_next==NULL,"Field active_next has not been nullified by get_object()");
  fail_unless(ob->active_prev==NULL,"Field active_prev has not been nullified by get_object()");
  /* did you really thing i'll go with only one object? */
  /* let's go for about 2M allocations in a row, let's test roughness */
  for(i=0;i<1U<<17;i++){
    ob=get_object();
    fail_unless(ob!=NULL,"Should get an object after calling get_object() (iteration %l)",i);
    if (!(i&((1<<13)-1)))
        LOG(llevDebug,"%ldk items created with get_object\n",i>>10);
  }
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void update_turn_face(object *op);
 */
START_TEST (test_update_turn_face)
{
  object *ob1;
  New_Face *face1;
  New_Face *face2;
  const char* reference;
  ob1 = cctk_create_game_object("xan");
  ob1->direction=1;
  update_turn_face(ob1);
  face1=ob1->face;
  ob1->direction=5;
  update_turn_face(ob1);
  face2=ob1->face;
  fail_unless(face2!=face1,"2 opposite direction should provide different faces after update_turn_face");

}
END_TEST

#define IS_OBJECT_ACTIVE(op) (op->active_next || op->active_prev || op==active_objects)
/** This is the test to check the behaviour of the method
 *  void update_ob_speed(object *op);
 */
START_TEST (test_update_ob_speed)
{
  object *ob1;
  object *ob2;
  object *ob3;
  object *ob4;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  ob4 = cctk_create_game_object(NULL);
  ob1->speed=MIN_ACTIVE_SPEED;
  update_ob_speed(ob1);
  fail_unless(!IS_OBJECT_ACTIVE(ob1),"Object with absolute speed <=MIN_ACTIVE_SPEED(%f) should not be made active (speed=%f)",MIN_ACTIVE_SPEED,ob1->speed);
  ob1->speed=-MIN_ACTIVE_SPEED;
  update_ob_speed(ob1);
  fail_unless(!IS_OBJECT_ACTIVE(ob1),"Object with absolute speed <=MIN_ACTIVE_SPEED(%f) should not be made active (speed=%f)",MIN_ACTIVE_SPEED,ob1->speed);
  ob1->speed=MIN_ACTIVE_SPEED*2;
  update_ob_speed(ob1);
  fail_unless(IS_OBJECT_ACTIVE(ob1),"Object with absolute speed >MIN_ACTIVE_SPEED(%f) should be made active (speed=%f)",MIN_ACTIVE_SPEED,ob1->speed);
  ob2->speed=-MIN_ACTIVE_SPEED*2;
  update_ob_speed(ob2);
  fail_unless(IS_OBJECT_ACTIVE(ob2),"Object with absolute speed >MIN_ACTIVE_SPEED(%f) should be made active (speed=%f)",MIN_ACTIVE_SPEED,ob2->speed);
  ob4->speed=ob3->speed=ob2->speed;
  update_ob_speed(ob3);
  update_ob_speed(ob4);
  fail_unless(IS_OBJECT_ACTIVE(ob3),"Object with absolute speed >MIN_ACTIVE_SPEED(%f) should be made active (speed=%f)",MIN_ACTIVE_SPEED,ob3->speed);
  fail_unless(IS_OBJECT_ACTIVE(ob4),"Object with absolute speed >MIN_ACTIVE_SPEED(%f) should be made active (speed=%f)",MIN_ACTIVE_SPEED,ob4->speed);
  ob1->speed=0.0;
  ob2->speed=0.0;
  ob3->speed=0.0;
  ob4->speed=0.0;
  update_ob_speed(ob1);
  update_ob_speed(ob2);
  update_ob_speed(ob3);
  update_ob_speed(ob4);
  fail_unless(!IS_OBJECT_ACTIVE(ob1),"Object with absolute speed 0.0 should be inactivated",ob1->speed);
  fail_unless(!IS_OBJECT_ACTIVE(ob2),"Object with absolute speed 0.0 should be inactivated",ob2->speed);
  fail_unless(!IS_OBJECT_ACTIVE(ob3),"Object with absolute speed 0.0 should be inactivated",ob3->speed);
  fail_unless(!IS_OBJECT_ACTIVE(ob4),"Object with absolute speed 0.0 should be inactivated",ob4->speed);
}
END_TEST

/** This is the test to check the behaviour of the method
 *  void remove_from_active_list(object *op);
 */
START_TEST (test_remove_from_active_list)
{
  object *ob1;
  ob1 = cctk_create_game_object(NULL);
  ob1->speed=MIN_ACTIVE_SPEED*2;
  update_ob_speed(ob1);
  fail_unless(IS_OBJECT_ACTIVE(ob1),"Object with absolute speed >MIN_ACTIVE_SPEED(%f) should be made active (speed=%f)",MIN_ACTIVE_SPEED,ob1->speed);
  remove_from_active_list(ob1);
  fail_unless(!IS_OBJECT_ACTIVE(ob1),"After call to remove_from_active_list, object should be made inactive");
}
END_TEST
#undef IS_OBJECT_ACTIVE


/** This is the test to check the behaviour of the method
 *  void update_object(object *op, int action);
 */
START_TEST (test_update_object)
{
    /*TESTME (this one need a map loading, left for later*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void free_object(object *ob);
 */
START_TEST (test_free_object)
{
  object *ob1;
  object *ob2;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  insert_ob_in_ob(ob2,ob1);
  free_object(ob1);
  fail_unless(QUERY_FLAG(ob1,FLAG_FREED),"Freeing ob1 should mark it freed");
  fail_unless(QUERY_FLAG(ob2,FLAG_FREED),"Freeing ob1 should mark it's content freed");
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int count_free(void);
 */
START_TEST (test_count_free)
{
  object *ob1;
  int free1,free2;
  ob1 = cctk_create_game_object(NULL);
  free1 = count_free();
  ob1 = cctk_create_game_object(NULL);
  free2 = count_free();
  fail_unless((free2==free1-1),"after creating an object, the count_free() should return one less (%d) but returned %d",free1-1,free2);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int count_used(void);
 */
START_TEST (test_count_used)
{
  object *ob1;
  int used1,used2;
  ob1 = cctk_create_game_object(NULL);
  used1 = count_used();
  ob1 = cctk_create_game_object(NULL);
  used2 = count_used();
  fail_unless((used2==used1+1),"after creating an object, the count_used() should return one more (%d) but returned %d",used1-1,used2);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  int count_active(void);
 */
START_TEST (test_count_active)
{
  object *ob1;
  int active1,active2;
  ob1 = cctk_create_game_object(NULL);
  ob1->speed=MIN_ACTIVE_SPEED*2;
  update_ob_speed(ob1);
  active1=count_active();
  ob1 = cctk_create_game_object(NULL);
  ob1->speed=MIN_ACTIVE_SPEED*2;
  update_ob_speed(ob1);
  active2=count_active();
  fail_unless((active2==active1+1),"after activating an additional object, count_active should return one less %d but returned %d",active1-1,active2);
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void sub_weight(object *op, signed long weight);
 */
START_TEST (test_sub_weight)
{
  object *ob1;
  object *ob2;
  object *ob3;
  object *ob4;
  unsigned long sum;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  ob4 = cctk_create_game_object(NULL);
  ob1->weight=10; /*This should not be taken into account by sum_weight*/
  ob1->type=CONTAINER;
  ob2->type=CONTAINER;
  ob3->type=CONTAINER;
  ob1->stats.Str=40; /*40% reduction of weight*/
  ob2->weight=10;
  ob3->weight=10;
  ob4->weight=10;
  insert_ob_in_ob(ob2,ob1);
  insert_ob_in_ob(ob3,ob2);
  insert_ob_in_ob(ob4,ob3);
  sum=sum_weight(ob1);
  fail_unless(sum==18,"Sum of object's inventory should be 18 (30*0.6+10) but was %lu.",sum);
  sub_weight(ob4,10);
  fail_unless(ob1->carrying==12,"after call to sub_weight, carrying of ob1 should be 22 but was %d",ob1->carrying);

}
END_TEST


/** This is the test to check the behaviour of the method
 *  void remove_ob(object *op);
 */
START_TEST (test_remove_ob)
{
  /*TESTME test those
     ob with more
     player inv
     remove from map
  */
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *merge_ob(object *op, object *top);
 */
START_TEST (test_merge_ob)
{
  object *ob1;
  object *ob2;
  object *ob3;
  object *ob4;
  object *op;
  ob1 = cctk_create_game_object(NULL);
  ob2 = cctk_create_game_object(NULL);
  ob3 = cctk_create_game_object(NULL);
  ob4 = cctk_create_game_object(NULL);
  op = cctk_create_game_object(NULL);
  ob1->below=ob2;
  ob2->below=ob3;
  ob3->below=ob4;
  ob2->above=ob1;
  ob3->above=ob2;
  ob4->above=ob3;
  ob1->name=add_string("test");
  ob2->name=add_string("test2");
  ob3->name=add_string("test3");

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
 */
START_TEST (test_insert_ob_in_map)
{
    mapstruct* map;
    object* first = NULL;
    object* second = NULL;
    object* third = NULL;
    object* floor = NULL;
    object* got = NULL;

    map = get_empty_map(5, 5);
    fail_unless(map != NULL, "get_empty_map returned NULL.");

    /* First, simple tests for insertion. */
    floor = cctk_create_game_object("woodfloor");
    fail_unless(floor != NULL, "create woodfloor failed");
    floor->x = 3;
    floor->y = 3;

    got = insert_ob_in_map(floor, map, NULL, 0);
    fail_unless(got == floor, "woodfloor shouldn't disappear");
    fail_unless(floor == GET_MAP_OB(map, 3, 3), "woodfloor should be first object");

    first = cctk_create_game_object("barrel");
    fail_unless(first != NULL, "create barrel failed");
    first->x = 3;
    first->y = 3;

    got = insert_ob_in_map(first, map, NULL, 0);
    fail_unless(got == first, "barrel shouldn't disappear");
    fail_unless(floor == GET_MAP_OB(map, 3, 3), "woodfloor should still be first object");
    fail_unless(floor->above == first, "barrel should be above floor");

    second = cctk_create_game_object("gem");
    fail_unless(second != NULL, "create gem failed");
    second->nrof = 1;
    second->x = 3;
    second->y = 3;

    got = insert_ob_in_map(second, map, NULL, INS_ABOVE_FLOOR_ONLY);
    fail_unless(got == second, "gem shouldn't disappear");
    fail_unless(floor == GET_MAP_OB(map, 3, 3), "woodfloor should still be first object");
    fail_unless(floor->above == second, "gem should be above floor");
    fail_unless(second->above == first, "barrel should be above gem");

    third = cctk_create_game_object("bed_1");
    fail_unless(third != NULL, "create bed_1 failed");
    third->nrof = 1;
    third->x = 3;
    third->y = 3;

    got = insert_ob_in_map(third, map, first, INS_BELOW_ORIGINATOR);
    fail_unless(got == third, "bed_1 shouldn't disappear");
    fail_unless(floor == GET_MAP_OB(map, 3, 3), "woodfloor should still be first object");
    fail_unless(third->above == first, "bed should be below barrel");
    fail_unless(third->below == second, "bed should be above gem");

    /* Merging tests. */
    third = cctk_create_game_object("gem");
    fail_unless(third != NULL, "create gem failed");
    third->nrof = 1;
    third->x = 3;
    third->y = 3;

    got = insert_ob_in_map(third, map, NULL, 0);
    fail_unless(got == third, "gem shouldn't disappear");
    fail_unless(QUERY_FLAG(second, FLAG_FREED), "first gem should have been removed.");
    fail_unless(third->nrof == 2, "second gem should have nrof 2");

    second = cctk_create_game_object("gem");
    fail_unless(second != NULL, "create gem failed");
    second->nrof = 1;
    second->x = 3;
    second->y = 3;
    second->value = 1;

    got = insert_ob_in_map(second, map, NULL, 0);
    fail_unless(got == second, "modified gem shouldn't disappear");
    fail_unless(second->nrof == 1, "modified gem should have nrof 1");

    /* Now check sacrificing, on another spot.
      Can't work here, as altar logic is in server.
      -> move that there.
     */
/*    first = cctk_create_game_object("altar");
    fail_unless(first != NULL, "create altar failed");
    first->x = 2;
    first->y = 2;
    first->stats.food = 5;
    first->value = 0;
    fail_unless(insert_ob_in_map(first, map, NULL, 0) == first, "altar shouldn't disappear");
    fail_unless(GET_MAP_MOVE_ON(map, 2, 2) & MOVE_WALK == MOVE_WALK, "floor should have MOVE_WALK set");

    second = cctk_create_game_object("food");
    fail_unless(second != NULL, "create food failed");
    second->nrof = 5;
    second->x = 2;
    second->y = 2;
    got = insert_ob_in_map(second, map, NULL, 0);
    fail_unless(got == NULL, "insert_ob_in_map(food) should have returned NULL");
    fail_unless(QUERY_FLAG(second, FLAG_FREED), "food should have been freed");*/
}
END_TEST


/** This is the test to check the behaviour of the method
 *  void replace_insert_ob_in_map(const char *arch_string, object *op);
 */
START_TEST (test_replace_insert_ob_in_map)
{
    mapstruct* map;
    object* first = NULL;
    object* got = NULL;

    map = get_empty_map(5, 5);
    fail_unless(map != NULL, "get_empty_map returned NULL.");

    /* Single tile object */
    first = cctk_create_game_object("barrel");
    fail_unless(first != NULL, "create barrel failed");

    got = insert_ob_in_map_at(first, map, NULL, 0, 0, 0);
    fail_unless(got == first, "item shouldn't be destroyed");

    first = cctk_create_game_object("dragon");
    fail_unless(first != NULL, "create dragon failed");
    fail_unless(first->more != NULL, "no other body part");

    got = insert_ob_in_map_at(first, map, NULL, 0, 1, 1);
    fail_unless(got == first, "item shouldn't be destroyed");

    fail_unless(GET_MAP_OB(map, 1, 1) == first, "item isn't on 1,1");
    fail_unless(GET_MAP_OB(map, 2, 1) != NULL, "no item on 2,1");
    fail_unless(GET_MAP_OB(map, 2, 1)->head == first, "head of 2,1 isn't 1,1");
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *get_split_ob(object *orig_ob, uint32 nr);
 */
START_TEST (test_get_split_ob)
{
    object* first = NULL;
    object* second = NULL;
    char err[50];

    first = cctk_create_game_object("gem");
    fail_unless(first != NULL, "create gem failed");
    first->nrof = 5;

    second = get_split_ob(first, 2, err, sizeof(err));
    fail_unless(second != NULL, "should return an item");
    fail_unless(second->nrof == 2, "2 expected to split");
    fail_unless(first->nrof == 3, "3 should be left");

    second = get_split_ob(first, 3, err, sizeof(err));
    fail_unless(second != NULL, "should return an item");
    fail_unless(QUERY_FLAG(first, FLAG_FREED), "first should be freed");

    first = get_split_ob(second, 10, err, sizeof(err));
    fail_unless(first == NULL, "should return NULL");
    fail_unless(second->nrof == 3, "3 should be left");
}
END_TEST


/** This is the test to check the behaviour of the method
 *  object *decrease_ob_nr(object *op, uint32 i);
 */
START_TEST (test_decrease_ob_nr)
{
    object* first = NULL;
    object* second = NULL;

    first = cctk_create_game_object("gem");
    fail_unless(first != NULL, "create gem failed");
    first->nrof = 5;

    second = decrease_ob_nr(first, 3);
    fail_unless(second == first, "gem shouldn't be destroyed");

    second = decrease_ob_nr(first, 2);
    fail_unless(second == NULL, "decrease_ob_nr should return NULL");
    fail_unless(QUERY_FLAG(first, FLAG_FREED), "gem should have been freed");
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
    object* container = NULL;
    object* item = NULL;

    item = cctk_create_game_object("gem");
    fail_unless(item != NULL, "create gem failed");
    item->weight = 50;

    /* Bookshelves have no weight reduction. */
    container = cctk_create_game_object("bookshelf");
    fail_unless(container != NULL, "create bookshelf failed");

    insert_ob_in_ob(item, container);
    fail_unless(container->inv == item, "item not inserted");
    fail_unless(container->carrying == 50, "container should carry 50 and not %d", container->carrying);

    remove_ob(item);
    fail_unless(container->carrying == 0, "container should carry 0 and not %d", container->carrying);

    /* Sacks have a Str of 10, so will reduce the weight. */
    container = cctk_create_game_object("sack");
    fail_unless(container != NULL, "create sack failed");

    insert_ob_in_ob(item, container);
    fail_unless(container->inv == item, "item not inserted");
    fail_unless(container->carrying == 45, "container should carry 45 and not %d", container->carrying);
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
 *  int item_matched_string(object *pl, object *op, const char *name);
 */
START_TEST (test_item_matched_string)
{
    object *pl;
    object *o1, *o2, *o3, *o4;
    int val;

    pl = cctk_create_game_object("kobold");
    fail_unless(pl != NULL, "couldn't create kobold");
    pl->contr = (player*)calloc(1, sizeof(player));
    fail_unless(pl->contr != NULL, "couldn't alloc contr");

    o1 = cctk_create_game_object("cloak");
    fail_unless(o1 != NULL, "couldn't find cloak archetype");
    o1->title = add_string("of Gorokh");
    CLEAR_FLAG(o1, FLAG_IDENTIFIED);

    val = item_matched_string(pl, o1, "all");
    fail_unless(val == 1, "all didn't match cloak");
    val = item_matched_string(pl, o1, "Gorokh");
    fail_unless(val == 0, "unidentified cloak matched title with value %d", val);
    val = item_matched_string(pl, o1, "random");
    fail_unless(val == 0, "unidentified cloak matched random value with value %d", val);

    SET_FLAG(o1, FLAG_IDENTIFIED);
    val = item_matched_string(pl, o1, "Gorokh");
    fail_unless(val != 0, "identified cloak didn't match title with value %d", val);

    o2 = cctk_create_game_object("cloak");
    SET_FLAG(o2, FLAG_UNPAID);
    val = item_matched_string(pl, o2, "unpaid");
    fail_unless(val == 2, "unpaid cloak didn't match unpaid");
    val = item_matched_string(pl, o2, "cloak");
    fail_unless(val != 0, "unpaid cloak didn't match cloak with %d", val);
    val = item_matched_string(pl, o2, "wrong");
    fail_unless(val == 0, "unpaid cloak matched wrong name %d", val);
}
END_TEST



Suite *object_suite(void)
{
  Suite *s = suite_create("object");
  TCase *tc_core = tcase_create("Core");
    /*setup and teardown will be called before each test in testcase 'tc_core' */
  tcase_add_unchecked_fixture(tc_core,setup,teardown);

  suite_add_tcase (s, tc_core);
  tcase_add_test(tc_core, test_can_merge);
  tcase_add_test(tc_core, test_sum_weight);
  tcase_add_test(tc_core, test_object_get_env_recursive);
  tcase_add_test(tc_core, test_get_player_container);
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
  tcase_add_test(tc_core, test_item_matched_string);

  return s;
}

int main(void)
{
  int nf;
  SRunner *sr;
  Suite *s = object_suite();
  sr = srunner_create(s);
  srunner_set_xml(sr,LOGDIR "/unit/common/object.xml");

  srunner_run_all(sr, CK_ENV); /*verbosity from env variable*/
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

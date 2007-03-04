/*****************************************************************************/
/* Crossfire Plugin Interface Common Parts                                   */
/* Version: 2.0beta8 (also known as "Alexander")                             */
/* Contact: yann.chachkoff@myrealbox.com                                     */
/*****************************************************************************/
/* The CPICP are the various functions of the server that can be used by all */
/* plugins. It is recommended that plugins do not call the server-provided   */
/* functions directly, but instead use the wrappers provided here by the     */
/* CPICP interface. By doing so, various validity checks can be performed    */
/* by CPICP, protecting the server from potential nasty effects of passing   */
/* it wrong parameter values.                                                */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001-2005 by Chachkoff Yann (Feel free to deliver your complaints)     */
/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000 Mark Wedel                                            */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */
/*****************************************************************************/
#include <plugin_common.h>

static f_plug_api cfapiSystem_add_string = NULL;
static f_plug_api cfapiSystem_register_global_event = NULL;
static f_plug_api cfapiSystem_remove_string = NULL;
static f_plug_api cfapiSystem_unregister_global_event = NULL;
static f_plug_api cfapiSystem_strdup_local = NULL;
static f_plug_api cfapiSystem_find_animation = NULL;
static f_plug_api cfapiSystem_log = NULL;
static f_plug_api cfapiSystem_get_time = NULL;
static f_plug_api cfapiSystem_timer_create = NULL;
static f_plug_api cfapiSystem_timer_destroy = NULL;

static f_plug_api cfapiMap_create_path = NULL;

static f_plug_api cfapiObject_get_property = NULL;
static f_plug_api cfapiObject_set_property = NULL;
static f_plug_api cfapiObject_apply = NULL;
static f_plug_api cfapiObject_identify = NULL;
static f_plug_api cfapiObject_describe = NULL;
static f_plug_api cfapiObject_drain = NULL;
static f_plug_api cfapiObject_fix = NULL;
static f_plug_api cfapiObject_give_skill = NULL;
static f_plug_api cfapiObject_transmute = NULL;
static f_plug_api cfapiObject_remove = NULL;
static f_plug_api cfapiObject_delete = NULL;
static f_plug_api cfapiObject_clone = NULL;
static f_plug_api cfapiObject_find = NULL;
static f_plug_api cfapiObject_create = NULL;
static f_plug_api cfapiObject_insert = NULL;
static f_plug_api cfapiObject_split = NULL;
static f_plug_api cfapiObject_merge = NULL;
static f_plug_api cfapiObject_distance = NULL;
static f_plug_api cfapiObject_update = NULL;
static f_plug_api cfapiObject_clear = NULL;
static f_plug_api cfapiObject_reset = NULL;
static f_plug_api cfapiCheck_inventory = NULL;
static f_plug_api cfapiObject_activate_rune = NULL;
static f_plug_api cfapiObject_check_trigger = NULL;
static f_plug_api cfapiObject_query_money = NULL;
static f_plug_api cfapiObject_query_cost = NULL;
static f_plug_api cfapiObject_cast = NULL;
static f_plug_api cfapiObject_learn_spell = NULL;
static f_plug_api cfapiObject_forget_spell = NULL;
static f_plug_api cfapiObject_check_spell = NULL;
static f_plug_api cfapiObject_pay_amount = NULL;
static f_plug_api cfapiObject_pay_item = NULL;
static f_plug_api cfapiObject_transfer = NULL;
static f_plug_api cfapiObject_find_archetype_inside = NULL;
static f_plug_api cfapiObject_out_of_map = NULL;
static f_plug_api cfapiObject_drop = NULL;
static f_plug_api cfapiObject_take = NULL;
static f_plug_api cfapiObject_say = NULL;
static f_plug_api cfapiObject_speak = NULL;
static f_plug_api cfapiMap_get_property = NULL;
static f_plug_api cfapiMap_set_property = NULL;
static f_plug_api cfapiMap_get_map = NULL;
static f_plug_api cfapiMap_message = NULL;
static f_plug_api cfapiMap_get_object_at = NULL;
static f_plug_api cfapiMap_present_arch_by_name = NULL;
static f_plug_api cfapiMap_get_flags = NULL;
static f_plug_api cfapiPlayer_find = NULL;
static f_plug_api cfapiPlayer_message = NULL;
static f_plug_api cfapiPlayer_send_inventory = NULL;
static f_plug_api cfapiObject_teleport = NULL;
static f_plug_api cfapiObject_pickup = NULL;
static f_plug_api cfapiObject_get_key = NULL;
static f_plug_api cfapiObject_set_key = NULL;
static f_plug_api cfapiObject_move = NULL;
static f_plug_api cfapiObject_apply_below = NULL;
static f_plug_api cfapiArchetype_get_first = NULL;
static f_plug_api cfapiArchetype_get_property = NULL;
static f_plug_api cfapiParty_get_property = NULL;
static f_plug_api cfapiRegion_get_property = NULL;
static f_plug_api cfapiPlayer_can_pay = NULL;
static f_plug_api cfapiFriendlylist_get_next = NULL;

#define GET_HOOK( x, y, z ) \
    { \
    x = ( f_plug_api )getHooks( &z, 1, y ); \
    if ( z != CFAPI_FUNC ) {\
        printf( "unable to find hook %s!\n", y ); return 0; \
    } }

int cf_init_plugin( f_plug_api getHooks )
{
    int z;
    GET_HOOK( cfapiSystem_strdup_local, "cfapi_system_strdup_local", z );
    GET_HOOK( cfapiSystem_add_string, "cfapi_system_add_string", z );
    GET_HOOK( cfapiSystem_register_global_event, "cfapi_system_register_global_event", z );
    GET_HOOK( cfapiSystem_remove_string, "cfapi_system_remove_string", z );
    GET_HOOK( cfapiSystem_unregister_global_event, "cfapi_system_unregister_global_event", z );
    GET_HOOK( cfapiSystem_find_animation, "cfapi_system_find_animation", z );
    GET_HOOK( cfapiObject_get_property, "cfapi_object_get_property", z );
    GET_HOOK( cfapiObject_set_property, "cfapi_object_set_property", z );
    GET_HOOK( cfapiObject_apply, "cfapi_object_apply", z );
    GET_HOOK( cfapiObject_identify, "cfapi_object_identify", z );
    GET_HOOK( cfapiObject_describe, "cfapi_object_describe", z );
    GET_HOOK( cfapiObject_drain, "cfapi_object_drain", z );
    GET_HOOK( cfapiObject_fix, "cfapi_object_fix", z );
    GET_HOOK( cfapiObject_give_skill, "cfapi_object_give_skill", z );
    GET_HOOK( cfapiObject_transmute, "cfapi_object_transmute", z );
    GET_HOOK( cfapiObject_remove, "cfapi_object_remove", z );
    GET_HOOK( cfapiObject_delete, "cfapi_object_delete", z );
    GET_HOOK( cfapiObject_clone, "cfapi_object_clone", z );
    GET_HOOK( cfapiObject_find, "cfapi_object_find", z );
    GET_HOOK( cfapiObject_create, "cfapi_object_create", z );
    GET_HOOK( cfapiObject_insert, "cfapi_object_insert", z );
    GET_HOOK( cfapiObject_split, "cfapi_object_split", z );
    GET_HOOK( cfapiObject_merge, "cfapi_object_merge", z );
    GET_HOOK( cfapiObject_distance, "cfapi_object_distance", z );
    GET_HOOK( cfapiObject_update, "cfapi_object_update", z );
    GET_HOOK( cfapiObject_clear, "cfapi_object_clear", z );
    GET_HOOK( cfapiObject_reset, "cfapi_object_reset", z );
    GET_HOOK( cfapiObject_activate_rune, "cfapi_object_spring_trap", z );
    GET_HOOK( cfapiObject_check_trigger, "cfapi_object_check_trigger", z );
    GET_HOOK( cfapiObject_query_money,"cfapi_object_query_money", z );
    GET_HOOK( cfapiObject_query_cost,"cfapi_object_query_cost", z );
    GET_HOOK( cfapiObject_cast,"cfapi_object_cast", z );
    GET_HOOK( cfapiObject_learn_spell,"cfapi_object_learn_spell", z );
    GET_HOOK( cfapiObject_forget_spell,"cfapi_object_forget_spell", z );
    GET_HOOK( cfapiObject_check_spell,"cfapi_object_check_spell", z );
    GET_HOOK( cfapiObject_pay_amount,"cfapi_object_pay_amount", z );
    GET_HOOK( cfapiObject_pay_item,"cfapi_object_pay_item", z );
    GET_HOOK( cfapiObject_transfer, "cfapi_object_transfer", z );
    GET_HOOK( cfapiObject_find_archetype_inside, "cfapi_object_find_archetype_inside", z );
    GET_HOOK( cfapiObject_remove, "cfapi_object_remove", z );
    GET_HOOK( cfapiObject_delete, "cfapi_object_delete", z );
    GET_HOOK( cfapiObject_out_of_map, "cfapi_map_out_of_map", z );
    GET_HOOK( cfapiObject_drop, "cfapi_object_drop", z );
    GET_HOOK( cfapiObject_take, "cfapi_object_take", z );
    GET_HOOK( cfapiObject_say, "cfapi_object_say", z );
    GET_HOOK( cfapiObject_speak, "cfapi_object_speak", z );
    /*GET_HOOK( cfapiCheck_inventory, "cfapi_check_inventory", z );*/
    GET_HOOK( cfapiMap_create_path, "cfapi_map_create_path", z );
    GET_HOOK( cfapiMap_get_property,"cfapi_map_get_property", z );
    GET_HOOK( cfapiMap_set_property,"cfapi_map_set_property", z );
    GET_HOOK( cfapiMap_get_map, "cfapi_map_get_map", z );
    GET_HOOK( cfapiMap_message, "cfapi_map_message", z );
    GET_HOOK( cfapiMap_get_object_at, "cfapi_map_get_object_at", z );
    GET_HOOK( cfapiMap_present_arch_by_name, "cfapi_map_present_arch_by_name", z );
    GET_HOOK( cfapiMap_get_flags, "cfapi_map_get_flags", z );
    GET_HOOK( cfapiPlayer_find, "cfapi_player_find", z );
    GET_HOOK( cfapiPlayer_message, "cfapi_player_message", z );
    GET_HOOK( cfapiPlayer_send_inventory, "cfapi_player_send_inventory", z );
    GET_HOOK( cfapiObject_teleport, "cfapi_object_teleport", z );
    GET_HOOK( cfapiObject_pickup, "cfapi_object_pickup", z );
    GET_HOOK( cfapiObject_get_key, "cfapi_object_get_key", z );
    GET_HOOK( cfapiObject_set_key, "cfapi_object_set_key", z );
    GET_HOOK( cfapiObject_move, "cfapi_object_move", z );
    GET_HOOK( cfapiObject_apply_below, "cfapi_object_apply_below", z );
    GET_HOOK( cfapiArchetype_get_first, "cfapi_archetype_get_first", z );
    GET_HOOK( cfapiArchetype_get_property, "cfapi_archetype_get_property", z );
    GET_HOOK( cfapiParty_get_property, "cfapi_party_get_property", z );
    GET_HOOK( cfapiRegion_get_property, "cfapi_region_get_property", z );
    GET_HOOK( cfapiPlayer_can_pay, "cfapi_player_can_pay", z );
    GET_HOOK( cfapiSystem_log, "cfapi_log", z );
    GET_HOOK( cfapiSystem_get_time, "cfapi_system_get_time", z );
    GET_HOOK( cfapiSystem_timer_create, "cfapi_system_timer_create", z );
    GET_HOOK( cfapiSystem_timer_destroy, "cfapi_system_timer_destroy", z );
    GET_HOOK( cfapiFriendlylist_get_next, "cfapi_friendlylist_get_next", z );
    return 1;
}

/* Should get replaced by tons of more explicit wrappers */
void* cf_map_get_property(mapstruct* map, int propcode)
{
    int val;
    return cfapiMap_get_property(&val, propcode, map);
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_map_set_int_property(mapstruct* map, int propcode, int value)
{
    int val;
    return cfapiMap_set_property(&val, map, propcode,value);
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_get_property(object* op, int propcode)
{
    int val;
    return cfapiObject_get_property(&val, op, propcode);
}
sint16 cf_object_get_resistance(object* op, int rtype)
{
    int val;
    return *(sint16*)cfapiObject_get_property(&val, op, CFAPI_OBJECT_PROP_RESIST, rtype);
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_set_int_property(object* op, int propcode, int value)
{
    int val;
    return cfapiObject_set_property(&val, op, propcode,value);
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_set_long_property(object* op, int propcode, long value)
{
    int val;
    return cfapiObject_set_property(&val, op, propcode,value);
}
void* cf_object_set_float_property(object* op, int propcode, float value)
{
    int val;
    return cfapiObject_set_property(&val, op, propcode, value);
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_set_string_property(object* op, int propcode, char* value)
{
    int val;
    return cfapiObject_set_property(&val, op, propcode,value);
}
void* cf_object_set_object_property(object* op, int propcode, object* value)
{
    int val;
    return cfapiObject_set_property(&val, op, propcode,value);
}
void* cf_object_set_experience(object* op, sint64 exp, const char* skill, int arg)
{
    int val;
    return cfapiObject_set_property(&val, op, CFAPI_OBJECT_PROP_EXP, exp, strlen(skill) > 0 ? skill : NULL, arg);
}
void cf_player_move(player* pl, int dir)
{
    int val;
    cfapiObject_move(&val,1,pl,dir);
}
void cf_object_move(object* op, int dir, object*originator)
{
    int val;
    cfapiObject_move(&val,0,op,dir,originator);
}
object* cf_player_send_inventory(object* op)
{
    int val;
    return cfapiPlayer_send_inventory(&val, op);
}

/**
 * Wrapper for manual_apply().
 *
 * Checks for unpaid items before applying.
 *
 * @param op
 * ::object object being applied.
 * @param author
 * ::object causing op to be applied.
 * @param flags
 * special (always apply/unapply) flags.  Nothing is done with
 * them in this function - they are passed to apply_special().
 * @return
 * - 0: player or monster can't apply objects of that type
 * - 1: has been applied, or there was an error applying the object
 * - 2: objects of that type can't be applied if not in inventory
 *
 */
int cf_object_apply(object* op, object* author, int flags)
{
    int val, ret;
    cfapiObject_apply(&val,op,author,flags, &ret);
    return ret;
}

/**
 * Wrapper for player_apply_below().
 *
 * @param op
 * player applying below.
 */
void cf_object_apply_below(object* op)
{
    int val;
    cfapiObject_apply_below(&val,op);
}
void cf_object_remove(object* op)
{
    int val;
    cfapiObject_remove(&val, op);
}
void cf_object_free(object* op)
{
    int val;
    cfapiObject_delete(&val, op);
}
object* cf_object_present_archname_inside(object* op, char* whatstr)
{
    int val;
    return cfapiObject_find_archetype_inside(&val, op, 0, whatstr);
}
int cf_object_transfer(object* op,int x,int y,int r,object* orig)
{
    int val;
    /*    return *(int*)cfapiObject_transfer(&val,op,1,x,y,op->map);*/
    return *(int*)cfapiObject_transfer(&val,op,0,x,y,r,orig);
}
int cf_object_change_map(object* op, int x, int y, mapstruct* map)
{
    int val;
    return *(int*)cfapiObject_transfer(&val,op,1,x,y,map);
    /*cfapiObject_transfer(&val,op,1,x,y,map);
    return 0;*/
}
object* cf_map_get_object_at(mapstruct* m, int x, int y)
{
    int val;
    return cfapiMap_get_object_at(&val, m, x, y);
}
void cf_map_message(mapstruct* m, char* msg, int color)
{
    int val;
    cfapiMap_message(&val, m, msg, color);
}
object* cf_object_clone(object* op, int clonetype)
{
    int val;
    return (object*)cfapiObject_clone(&val, op, clonetype);
}
int cf_object_pay_item(object* op, object* buyer)
{
    int val;
    return *(int*)cfapiObject_pay_item(&val, op, buyer);
}
int cf_object_pay_amount(object* op, uint64 amount)
{
    int val;
    return *(int*)cfapiObject_pay_amount(&val, op, amount);
}
int cf_object_cast_spell(object* caster, object* ctoo, int dir, object* sp, char* flags)
{
    int val;
    return *(int*)cfapiObject_cast(&val, caster, ctoo, dir, sp, flags);
}
/* Should there be a difference nowadays between that and cast_spell ? */
int cf_object_cast_ability(object* caster, object* ctoo, int dir, object* sp, char* flags)
{
    int val;
    return *(int*)cfapiObject_cast(&val, caster, ctoo, dir, sp, flags);
}
void cf_object_learn_spell(object* op, object* sp)
{
    int val;
    cfapiObject_learn_spell(&val, op, sp, 0);
}
void cf_object_forget_spell(object* op, object* sp)
{
    int val;
    cfapiObject_forget_spell(&val, op, sp);
}
object* cf_object_check_for_spell(object* op, char* spellname)
{
    int val;
    return cfapiObject_check_spell(&val, op, spellname);
}
void cf_player_message(object* op, char* txt, int flags)
{
    int val;
    cfapiPlayer_message(&val, flags, 0, op, txt);
}

player* cf_player_find(char* txt)
{
    int val;
    return cfapiPlayer_find(&val, txt);
}
char* cf_player_get_ip(object* op)
{
    int val;
    return cfapiObject_get_property(&val, op, CFAPI_PLAYER_PROP_IP);
}
object* cf_player_get_marked_item(object* op)
{
    int val;
    return cfapiObject_get_property(&val, op, CFAPI_PLAYER_PROP_MARKED_ITEM);
}
void cf_player_set_marked_item(object* op, object* ob)
{
	int val;
	cfapiObject_set_property(&val, op, CFAPI_PLAYER_PROP_MARKED_ITEM, ob);
}
partylist* cf_player_get_party(object* op)
{
	int val;
	return cfapiObject_get_property(&val, op, CFAPI_PLAYER_PROP_PARTY);
}
void cf_player_set_party(object* op, partylist* party)
{
	int val;
	cfapiObject_set_property(&val, op, CFAPI_PLAYER_PROP_PARTY, party);
}
int cf_player_can_pay(object* pl)
{
    int val;
    return *(int*)cfapiPlayer_can_pay(&val, pl);
}

mapstruct* cf_map_get_map( char* name )
{
    int val;
    return cfapiMap_get_map( &val, 1, name, 0);
}
mapstruct*   cf_map_get_first(void)
{
    int val;
    return cfapiMap_get_map( &val, 3);
}
int cf_object_query_money( object* op)
{
    int val;
    return *(int*)cfapiObject_query_money( &val, op);
}
int cf_object_query_cost( object* op, object* who, int flags)
{
    int val;
    return *(int*)cfapiObject_query_cost(&val,op,who,flags);
}
void cf_object_activate_rune( object* op , object* victim)
{
    int val;
    if ( op )
        cfapiObject_activate_rune( &val, op, victim );
}
int cf_object_check_trigger( object* op, object* cause )
{
    int val;
    return *(int*)cfapiObject_check_trigger( &val, op, cause );
}
int cf_object_out_of_map( object* op, int x, int y)
{
    int val;
    return *(int*)cfapiObject_out_of_map(&val,op->map,x,y);
}
void cf_object_drop( object* op, object* author)
{
    int val;
    cfapiObject_drop( &val, op, author );
}
void cf_object_take( object* op, object* author)
{
    int val;
    cfapiObject_take( &val, op, author );
}
void cf_object_say( object* op, char* msg)
{
    int val;
    cfapiObject_say( &val, op, msg );
}
void cf_object_speak( object* op, char* msg)
{
    int val;
    cfapiObject_speak( &val, op, msg );
}
object* cf_object_insert_object(object* op, object* container)
{
    int val;
    return cfapiObject_insert(&val, op, 3, container);
}
char* cf_get_maps_directory(char* str)
{
    int val;

    /*printf("Calling with %s, type 0\n",str);
    printf("Value of the pointer: %p\n", cfapiMap_create_path);*/
    return cfapiMap_create_path(&val, 0, str);
}
object* cf_create_object()
{
    int val;
    return cfapiObject_create(&val, 0);
}
object* cf_create_object_by_name( const char* name )
{
    int val;
    return cfapiObject_create(&val, 1, name);
}
void cf_free_object( object* ob )
{
    int val;
    if ( ob )
        cfapiObject_delete( &val, ob );
}

void cf_system_register_global_event( int event, const char* name, f_plug_api hook )
{
    int val;
    cfapiSystem_register_global_event( &val, event, name, hook );
}

void cf_fix_object( object* pl )
{
    int val;
    if ( pl )
        cfapiObject_fix( &val, pl );
}

char* cf_add_string( char* str )
{
    int val;
    if ( !str )
        return NULL;
    return cfapiSystem_add_string( &val, str );
}

void cf_free_string( char* str )
{
    int val;
    if ( str )
        cfapiSystem_remove_string( &val, str );
}

char* cf_query_name( object* ob )
{
    int val;
    return cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_NAME );
}

char* cf_query_name_pl( object* ob )
{
    int val;
    return cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_NAME_PLURAL );
}

char* cf_query_base_name( object* ob, int plural )
{
    int val;
    return cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_BASE_NAME, plural );
}

object* cf_insert_ob_in_ob( object* ob, object* where )
{
    int val;
    return cfapiObject_insert( &val, ob, 3, where );
}

const char* cf_object_get_msg( object* ob )
{
    int val;
    return cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_MESSAGE );
}

void cf_object_set_weight( object* ob, int weight )
{
    int val;
    cfapiObject_set_property( &val, ob, CFAPI_OBJECT_PROP_WEIGHT, weight );
}

void cf_object_set_weight_limit( object* ob, int weight_limit )
{
    int val;
    cfapiObject_set_property( &val, ob, CFAPI_OBJECT_PROP_WEIGHT_LIMIT, weight_limit );
}

int cf_object_get_weight( object* ob )
{
    int val;
    return *( int* )cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_WEIGHT );
}

int cf_object_get_weight_limit( object* ob )
{
    int val;
    int *dummy;
    dummy = cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_WEIGHT_LIMIT );
    return *dummy;
}

/**
 * @return -1=nrof is invalid, 0=nrof is ok#
 */
int cf_object_set_nrof( object* ob, int nrof )
{
    int val;

    if (nrof < 0)
        return -1;

    cfapiObject_set_property( &val, ob, CFAPI_OBJECT_PROP_NROF, nrof );
    return 0;
}

int cf_object_get_nrof( object* ob )
{
    int val;
    return *( int* )cfapiObject_get_property( &val, ob, CFAPI_OBJECT_PROP_NROF );
}

int cf_object_get_flag( object* ob, int flag )
{
    int type;
    int rv;
    rv = *(int*)cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_FLAGS, flag );
    if (rv != 0)
        return 1;
    else
        return 0;
}

void cf_object_set_flag( object* ob, int flag, int value )
{
    int type;
    cfapiObject_set_property( &type, ob, CFAPI_OBJECT_PROP_FLAGS, flag, value ? 1 : 0 );
}

object* cf_object_insert_in_ob( object* ob, object* where )
{
    int type;

    if (!cf_object_get_flag(ob,FLAG_REMOVED))
    {
        cfapiObject_remove( &type, ob );
    }

    return cfapiObject_insert( &type, ob, 3, where );
}

object* cf_map_insert_object_there(mapstruct* where, object* op, object* originator, int flags)
{
    int val;
    return (object*)cfapiObject_insert(&val, op, 1, where, originator, flags);
}
object* cf_map_insert_object(mapstruct* where , object* op, int x, int y)
{
    int type;
    return cfapiObject_insert( &type, op, 0, where, NULL, 0 , x, y);
}
int cf_object_teleport( object* op, mapstruct* map, int x, int y )
{
    int val;
    return *( int* )cfapiObject_teleport( &val, op, map, x, y );
}
object* cf_map_present_arch_by_name(const char* str, mapstruct* map, int nx, int ny)
{
    int val;
    return (object*)cfapiMap_present_arch_by_name(&val, str,map,nx,ny);
}

static int cf_get_map_int_property(mapstruct* map, int property)
{
	int type;
	void* rv;
	rv = cfapiMap_get_property(&type, property, map);
	if ( !rv || type != CFAPI_INT )
		return PLUGIN_ERROR_INT;
	return *(int*)rv;
}

int cf_map_get_difficulty(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_DIFFICULTY);
}

int cf_map_get_reset_time(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_RESET_TIME);
}

int cf_map_get_reset_timeout(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_RESET_TIMEOUT);
}

int cf_map_get_players(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_PLAYERS);
}

int cf_map_get_darkness(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_DARKNESS);
}

int cf_map_get_width(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_WIDTH);
}

int cf_map_get_height(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_HEIGHT);
}

int cf_map_get_enter_x(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_ENTER_X);
}

int cf_map_get_enter_y(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_ENTER_Y);
}

int cf_map_get_temperature(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_TEMPERATURE);
}

int cf_map_get_pressure(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_PRESSURE);
}

int cf_map_get_humidity(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_HUMIDITY);
}

int cf_map_get_windspeed(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_WINDSPEED);
}

int cf_map_get_winddir(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_WINDDIR);
}

int cf_map_get_sky(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_SKY);
}

int cf_map_get_wpartx(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_WPARTX);
}

int cf_map_get_wparty(mapstruct* map)
{
	return cf_get_map_int_property(map,CFAPI_MAP_PROP_WPARTY);
}

void cf_object_update( object* op, int flags)
{
    int val;
    cfapiObject_update(&val, op, flags);
}
void cf_object_pickup( object* op, object* what)
{
    int val;
    cfapiObject_pickup(&val, op, what);
}
char* cf_strdup_local(char* txt)
{
    int val;
    return (char*)cfapiSystem_strdup_local(&val, txt);
}
int cf_map_get_flags( mapstruct* map, mapstruct** nmap, sint16 x, sint16 y, sint16* nx, sint16* ny )
{
    int val;
    return *( int* )cfapiMap_get_flags(&val, map, nmap, x, y, nx, ny);
}

/**
 * Wrapper for find_animation().
 * @param txt
 * the animation's name
 * @return
 * animation number, or 0 if no match found (animation 0 is initialized as the 'bug' face
 */
int cf_find_animation(const char* txt)
{
    int val, anim;
    cfapiSystem_find_animation(&val, txt, &anim);
    return anim;
}
void cf_log( LogLevel logLevel, const char* format, ... )
{
    int val;

    /* Copied from common/logger.c */
    char buf[20480];  /* This needs to be really really big - larger than any other buffer, since that buffer may
    	need to be put in this one. */
    va_list ap;
    va_start(ap, format);
    buf[0] = '\0';
    vsprintf(buf, format, ap);

    cfapiSystem_log(&val, logLevel, buf);

    va_end(ap);
}

void cf_get_time( timeofday_t* tod )
{
    int val;
    cfapiSystem_get_time(&val, tod);
}

/**
 * Creates a timer, equivalent of calling cftimer_create().
 *
 * @param ob
 * ::object that will get called. Should handle ::EVENT_TIMER.
 * @param delay
 * delay, seconds or ticks.
 * @param mode
 * timer mode, ::TIMER_MODE_SECONDS or ::TIMER_MODE_CYCLES
 * @return
 * timer identifier, or one of ::TIMER_ERR_ID, ::TIMER_ERR_OBJ or ::TIMER_ERR_MODE
 */
int cf_timer_create(object* ob, long delay, int mode)
{
    int val, timer;
    cfapiSystem_timer_create(&val, ob, delay, mode, &timer);
    return timer;
}

/**
 * Destroys specified timer, equivalent of calling cftimer_destroy().
 *
 * @param id
 * timer to destroy
 * @return
 * ::TIMER_ERR_ID if invalid id, ::TIMER_ERR_NONE else.
 */
int cf_timer_destroy(int id)
{
    int val, code;
    cfapiSystem_timer_destroy(&val, id, &code);
    return code;
}

/**
 * Gets value for specified key, equivalent of get_ob_key_value().
 * @param op
 * ::object for which we search a key.
 * @param keyname
 * key to look for. Not required to be a shared string.
 * @return
 * value (shared string), or NULL if not found.
 */
const char* cf_object_get_key(object* op, const char* keyname)
{
    int val;
    const char* value;
    cfapiObject_get_key(&val, op, keyname, &value);
    return value;
}

/**
 * Sets a value for specified key, equivalent to set_ob_key_value().
 * @param op
 * ::object which will contain the key/value
 * @param keyname
 * key
 * @param value
 * value
 * @param add_key
 * if 0, key is only updated if it exists, else it's updated or added.
 * @return
 * TRUE or FALSE.
 */
int cf_object_set_key(object* op, const char* keyname, const char* value, int add_key)
{
    int val, ret;
    cfapiObject_set_key(&val, op, keyname, value, add_key, &ret);
    return ret;
}

/* Archetype-related functions */
archetype*cf_archetype_get_first()
{
    int val;
    return cfapiArchetype_get_first(&val);
}

const char*  cf_archetype_get_name(archetype* arch)
{
    int val;
    return cfapiArchetype_get_property(&val, arch, CFAPI_ARCH_PROP_NAME);
}

archetype* cf_archetype_get_next(archetype* arch)
{
    int val;
    return cfapiArchetype_get_property(&val, arch, CFAPI_ARCH_PROP_NEXT);
}

archetype* cf_archetype_get_more(archetype* arch)
{
    int val;
    return cfapiArchetype_get_property(&val, arch, CFAPI_ARCH_PROP_MORE);
}

archetype* cf_archetype_get_head(archetype* arch)
{
    int val;
    return cfapiArchetype_get_property(&val, arch, CFAPI_ARCH_PROP_HEAD);
}

object* cf_archetype_get_clone(archetype* arch)
{
    int val;
    return cfapiArchetype_get_property(&val, arch, CFAPI_ARCH_PROP_CLONE);
}

/* Party-related functions */
partylist* cf_party_get_first(void)
{
	int val;
	return cfapiParty_get_property(&val, NULL, CFAPI_PARTY_PROP_NEXT);
}

const char* cf_party_get_name(partylist* party)
{
	int val;
	return cfapiParty_get_property(&val, party, CFAPI_PARTY_PROP_NAME);
}

partylist* cf_party_get_next(partylist* party)
{
	int val;
	return cfapiParty_get_property(&val, party, CFAPI_PARTY_PROP_NEXT);
}

const char* cf_party_get_password(partylist* party)
{
	int val;
	return cfapiParty_get_property(&val, party, CFAPI_PARTY_PROP_PASSWORD);
}

player* cf_party_get_first_player(partylist* party)
{
	int val;
	return cfapiParty_get_property(&val, party, CFAPI_PARTY_PROP_PLAYER, NULL);
}

player* cf_party_get_next_player(partylist* party, player* op)
{
	int val;
	return cfapiParty_get_property(&val, party, CFAPI_PARTY_PROP_PLAYER, op);
}

region* cf_region_get_first(void)
{
	int val;
	return cfapiRegion_get_property(&val, NULL, CFAPI_REGION_PROP_NEXT);
}

const char* cf_region_get_name(region* reg)
{
	int val;
	return cfapiRegion_get_property(&val, reg, CFAPI_REGION_PROP_NAME);
}

region* cf_region_get_next(region* reg)
{
	int val;
	return cfapiRegion_get_property(&val, reg, CFAPI_REGION_PROP_NEXT);
}

region* cf_region_get_parent(region* reg)
{
	int val;
	return cfapiRegion_get_property(&val, reg, CFAPI_REGION_PROP_PARENT);
}

const char* cf_region_get_longname(region* reg)
{
	int val;
	return cfapiRegion_get_property(&val, reg, CFAPI_REGION_PROP_LONGNAME);
}

const char* cf_region_get_message(region* reg)
{
	int val;
	return cfapiRegion_get_property(&val, reg, CFAPI_REGION_PROP_MESSAGE);
}

/* Friendlylist functions. */
object* cf_friendlylist_get_first(void)
{
    int val;
    return cfapiFriendlylist_get_next(&val, NULL);
}
object* cf_friendlylist_get_next(object* ob)
{
    int val;
    return cfapiFriendlylist_get_next(&val, ob);
}


#ifdef WIN32
int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info)
{
	/* Get the time, if they want it */
	if (time_Info != NULL) {
		time_Info->tv_sec = time(NULL);
		time_Info->tv_usec = timeGetTime()*1000;
	}
	/* Get the timezone, if they want it */
	if (timezone_Info != NULL) {
		_tzset();
		timezone_Info->tz_minuteswest = _timezone;
		timezone_Info->tz_dsttime = _daylight;
	}
	/* And return */
	return 0;
}
#endif

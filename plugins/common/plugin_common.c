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
#include <assert.h>

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
static f_plug_api cfapiSet_random_map_variable = NULL;
static f_plug_api cfapiGenerate_random_map = NULL;

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
    GET_HOOK( cfapiSet_random_map_variable, "cfapi_set_random_map_variable", z );
    GET_HOOK( cfapiGenerate_random_map, "cfapi_generate_random_map", z );

    return 1;
}

/* Should get replaced by tons of more explicit wrappers */
void* cf_map_get_property(mapstruct* map, int propcode)
{
    int type;
    return cfapiMap_get_property(&type, propcode, map);
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_map_set_int_property(mapstruct* map, int propcode, int value)
{
    int type;
    return cfapiMap_set_property(&type, map, propcode,value);
}
/* Should get replaced by tons of more explicit wrappers */
sint16 cf_object_get_resistance(object* op, int rtype)
{
    int type;
    sint16 resist;
    cfapiObject_get_property(&type, op, CFAPI_OBJECT_PROP_RESIST, rtype, &resist);
    assert(type == CFAPI_INT16);
    return resist;
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_set_int_property(object* op, int propcode, int value)
{
    int type;
    return cfapiObject_set_property(&type, op, propcode,value);
}
int cf_object_get_int_property(object* op, int propcode)
{
    int type, value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_INT);
    return value;
}
void* cf_object_set_movetype_property(object* op, int propcode, MoveType value)
{
    int type;
    return cfapiObject_set_property(&type, op, propcode, value);
}
MoveType cf_object_get_movetype_property(object* op, int propcode)
{
    int type;
    MoveType value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_MOVETYPE);
    return value;
}
object* cf_object_get_object_property(object* op, int propcode)
{
    int type;
    object* value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_POBJECT);
    return value;
}
mapstruct* cf_object_get_map_property(object* op, int propcode)
{
    int type;
    mapstruct* value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_PMAP);
    return value;
}
sint64 cf_object_get_int64_property(object* op, int propcode)
{
    int type;
    sint64 value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_SINT64);
    return value;
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_set_long_property(object* op, int propcode, long value)
{
    int type;
    return cfapiObject_set_property(&type, op, propcode,value);
}
void* cf_object_set_float_property(object* op, int propcode, float value)
{
    int type;
    return cfapiObject_set_property(&type, op, propcode, value);
}
float cf_object_get_float_property(object* op, int propcode)
{
    int type;
    float value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_FLOAT);
    return value;
}
archetype* cf_object_get_archetype_property(object* op, int propcode)
{
    int type;
    archetype* value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_PARCH);
    return value;
}
partylist* cf_object_get_partylist_property(object* op, int propcode)
{
    int type;
    partylist* value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_PPARTY);
    return value;
}
double cf_object_get_double_property(object* op, int propcode)
{
    int type;
    double value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_DOUBLE);
    return value;
}
sstring cf_object_get_sstring_property(object* op, int propcode)
{
    int type;
    sstring value;
    cfapiObject_get_property(&type, op, propcode, &value);
    assert(type == CFAPI_SSTRING);
    return value;
}
char* cf_object_get_string_property(object* op, int propcode, char* buf, int size)
{
    int type;
    cfapiObject_get_property(&type, op, propcode, buf, size);
    assert(type == CFAPI_STRING);
    return buf;
}
/* Should get replaced by tons of more explicit wrappers */
void* cf_object_set_string_property(object* op, int propcode, char* value)
{
    int type;
    return cfapiObject_set_property(&type, op, propcode,value);
}
void* cf_object_set_object_property(object* op, int propcode, object* value)
{
    int type;
    return cfapiObject_set_property(&type, op, propcode,value);
}
void* cf_object_set_experience(object* op, sint64 exp, const char* skill, int arg)
{
    int type;
    return cfapiObject_set_property(&type, op, CFAPI_OBJECT_PROP_EXP, exp, strlen(skill) > 0 ? skill : NULL, arg);
}
void cf_player_move(player* pl, int dir)
{
    int type, ret;
    cfapiObject_move(&type,1,pl,dir, &ret);
}
void cf_object_move(object* op, int dir, object*originator)
{
    int type, ret;
    cfapiObject_move(&type,0,op,dir,originator, &ret);
}
object* cf_player_send_inventory(object* op)
{
    int type;
    return cfapiPlayer_send_inventory(&type, op);
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
    int type, ret;
    cfapiObject_apply(&type,op,author,flags, &ret);
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
    int type;
    cfapiObject_apply_below(&type,op);
}
void cf_object_remove(object* op)
{
    int type;
    cfapiObject_remove(&type, op);
}
void cf_object_free(object* op)
{
    int type;
    cfapiObject_delete(&type, op);
}
object* cf_object_present_archname_inside(object* op, char* whatstr)
{
    int type;
    return cfapiObject_find_archetype_inside(&type, op, 0, whatstr);
}
int cf_object_transfer(object* op,int x,int y,int r,object* orig)
{
    int type;
    /*    return *(int*)cfapiObject_transfer(&type,op,1,x,y,op->map);*/
    return *(int*)cfapiObject_transfer(&type,op,0,x,y,r,orig);
}
int cf_object_change_map(object* op, int x, int y, mapstruct* map)
{
    int type;
    return *(int*)cfapiObject_transfer(&type,op,1,x,y,map);
    /*cfapiObject_transfer(&type,op,1,x,y,map);
    return 0;*/
}
object* cf_map_get_object_at(mapstruct* m, int x, int y)
{
    int type;
    return cfapiMap_get_object_at(&type, m, x, y);
}
void cf_map_message(mapstruct* m, char* msg, int color)
{
    int type;
    cfapiMap_message(&type, m, msg, color);
}
object* cf_object_clone(object* op, int clonetype)
{
    int type;
    return (object*)cfapiObject_clone(&type, op, clonetype);
}
int cf_object_pay_item(object* op, object* buyer)
{
    int type;
    return *(int*)cfapiObject_pay_item(&type, op, buyer);
}
int cf_object_pay_amount(object* op, uint64 amount)
{
    int type;
    return *(int*)cfapiObject_pay_amount(&type, op, amount);
}
int cf_object_cast_spell(object* caster, object* ctoo, int dir, object* sp, char* flags)
{
    int type;
    return *(int*)cfapiObject_cast(&type, caster, ctoo, dir, sp, flags);
}
/* Should there be a difference nowadays between that and cast_spell ? */
int cf_object_cast_ability(object* caster, object* ctoo, int dir, object* sp, char* flags)
{
    int type;
    return *(int*)cfapiObject_cast(&type, caster, ctoo, dir, sp, flags);
}
void cf_object_learn_spell(object* op, object* sp)
{
    int type;
    cfapiObject_learn_spell(&type, op, sp, 0);
}
void cf_object_forget_spell(object* op, object* sp)
{
    int type;
    cfapiObject_forget_spell(&type, op, sp);
}
object* cf_object_check_for_spell(object* op, char* spellname)
{
    int type;
    return cfapiObject_check_spell(&type, op, spellname);
}
void cf_player_message(object* op, char* txt, int flags)
{
    int type;
    cfapiPlayer_message(&type, flags, 0, op, txt);
}

player* cf_player_find(char* txt)
{
    int type;
    return cfapiPlayer_find(&type, txt);
}
sstring cf_player_get_ip(object* op)
{
    int type;
    sstring value;
    cfapiObject_get_property(&type, op, CFAPI_PLAYER_PROP_IP, &value);
    assert(type == CFAPI_SSTRING);
    return value;
}
object* cf_player_get_marked_item(object* op)
{
    int type;
    object* value;
    cfapiObject_get_property(&type, op, CFAPI_PLAYER_PROP_MARKED_ITEM, &value);
    assert(type == CFAPI_POBJECT);
    return value;
}
void cf_player_set_marked_item(object* op, object* ob)
{
	int type;
	cfapiObject_set_property(&type, op, CFAPI_PLAYER_PROP_MARKED_ITEM, ob);
}
partylist* cf_player_get_party(object* op)
{
	int type;
	return cfapiObject_get_property(&type, op, CFAPI_PLAYER_PROP_PARTY);
}
void cf_player_set_party(object* op, partylist* party)
{
	int type;
	cfapiObject_set_property(&type, op, CFAPI_PLAYER_PROP_PARTY, party);
}
int cf_player_can_pay(object* pl)
{
    int type;
    return *(int*)cfapiPlayer_can_pay(&type, pl);
}

mapstruct* cf_map_get_map( char* name )
{
    int type;
    return cfapiMap_get_map( &type, 1, name, 0);
}
mapstruct*   cf_map_get_first(void)
{
    int type;
    return cfapiMap_get_map( &type, 3);
}
int cf_object_query_money( object* op)
{
    int type;
    return *(int*)cfapiObject_query_money( &type, op);
}
int cf_object_query_cost( object* op, object* who, int flags)
{
    int type;
    return *(int*)cfapiObject_query_cost(&type,op,who,flags);
}
void cf_object_activate_rune( object* op , object* victim)
{
    int type;
    if ( op )
        cfapiObject_activate_rune( &type, op, victim );
}
int cf_object_check_trigger( object* op, object* cause )
{
    int type;
    return *(int*)cfapiObject_check_trigger( &type, op, cause );
}
int cf_object_out_of_map( object* op, int x, int y)
{
    int type;
    return *(int*)cfapiObject_out_of_map(&type,op->map,x,y);
}
void cf_object_drop( object* op, object* author)
{
    int type;
    cfapiObject_drop( &type, op, author );
}
void cf_object_take( object* op, object* author)
{
    int type;
    cfapiObject_take( &type, op, author );
}
void cf_object_say( object* op, char* msg)
{
    int type;
    cfapiObject_say( &type, op, msg );
}
void cf_object_speak( object* op, char* msg)
{
    int type;
    cfapiObject_speak( &type, op, msg );
}
object* cf_object_insert_object(object* op, object* container)
{
    int type;
    return cfapiObject_insert(&type, op, 3, container);
}
char* cf_get_maps_directory(char* str)
{
    int type;

    /*printf("Calling with %s, type 0\n",str);
    printf("Value of the pointer: %p\n", cfapiMap_create_path);*/
    return cfapiMap_create_path(&type, 0, str);
}
object* cf_create_object()
{
    int type;
    return cfapiObject_create(&type, 0);
}
object* cf_create_object_by_name( const char* name )
{
    int type;
    return cfapiObject_create(&type, 1, name);
}
void cf_free_object( object* ob )
{
    int type;
    if ( ob )
        cfapiObject_delete( &type, ob );
}

void cf_system_register_global_event( int event, const char* name, f_plug_api hook )
{
    int type;
    cfapiSystem_register_global_event( &type, event, name, hook );
}

void cf_fix_object( object* pl )
{
    int type;
    if ( pl )
        cfapiObject_fix( &type, pl );
}

char* cf_add_string( char* str )
{
    int type;
    if ( !str )
        return NULL;
    return cfapiSystem_add_string( &type, str );
}

void cf_free_string( char* str )
{
    int type;
    if ( str )
        cfapiSystem_remove_string( &type, str );
}

char* cf_query_name(object* ob, char* name, int size)
{
    int type;
    cfapiObject_get_property(&type, ob, CFAPI_OBJECT_PROP_NAME, name, size);
    assert(type == CFAPI_STRING);
    return name;
}

sstring cf_query_name_pl( object* ob )
{
    int type;
    sstring value;
    cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_NAME_PLURAL, &value );
    assert(type == CFAPI_SSTRING);
    return value;
}

char* cf_query_base_name(object* ob, int plural, char* name, int size)
{
    int type;
    cfapiObject_get_property(&type, ob, CFAPI_OBJECT_PROP_BASE_NAME, name, size);
    assert(type == CFAPI_STRING);
    return name;
}

object* cf_insert_ob_in_ob( object* ob, object* where )
{
    int type;
    return cfapiObject_insert( &type, ob, 3, where );
}

sstring cf_object_get_msg( object* ob )
{
    int type;
    sstring value;
    cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_MESSAGE, &value );
    assert(type == CFAPI_SSTRING);
    return value;
}

void cf_object_set_weight( object* ob, int weight )
{
    int type;
    cfapiObject_set_property( &type, ob, CFAPI_OBJECT_PROP_WEIGHT, weight );
}

void cf_object_set_weight_limit( object* ob, int weight_limit )
{
    int type;
    cfapiObject_set_property( &type, ob, CFAPI_OBJECT_PROP_WEIGHT_LIMIT, weight_limit );
}

int cf_object_get_weight( object* ob )
{
    int type, weight;
    cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_WEIGHT, &weight );
    return weight;
}

int cf_object_get_weight_limit( object* ob )
{
    int type, limit;
    cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_WEIGHT_LIMIT, &limit );
    return limit;
}

/**
 * @return -1=nrof is invalid, 0=nrof is ok#
 */
int cf_object_set_nrof( object* ob, int nrof )
{
    int type;

    if (nrof < 0)
        return -1;

    cfapiObject_set_property( &type, ob, CFAPI_OBJECT_PROP_NROF, nrof );
    return 0;
}

int cf_object_get_nrof( object* ob )
{
    int type, nrof;
    cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_NROF, &nrof );
    return nrof;
}

int cf_object_get_flag( object* ob, int flag )
{
    int type;
    int rv;
    cfapiObject_get_property( &type, ob, CFAPI_OBJECT_PROP_FLAGS, flag, &rv );
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
    int type;
    return (object*)cfapiObject_insert(&type, op, 1, where, originator, flags);
}
object* cf_map_insert_object(mapstruct* where , object* op, int x, int y)
{
    int type;
    return cfapiObject_insert( &type, op, 0, where, NULL, 0 , x, y);
}
int cf_object_teleport( object* op, mapstruct* map, int x, int y )
{
    int type;
    return *( int* )cfapiObject_teleport( &type, op, map, x, y );
}
object* cf_map_present_arch_by_name(const char* str, mapstruct* map, int nx, int ny)
{
    int type;
    return (object*)cfapiMap_present_arch_by_name(&type, str,map,nx,ny);
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
    int type;
    cfapiObject_update(&type, op, flags);
}
void cf_object_pickup( object* op, object* what)
{
    int type;
    cfapiObject_pickup(&type, op, what);
}
char* cf_strdup_local(char* txt)
{
    int type;
    return (char*)cfapiSystem_strdup_local(&type, txt);
}
int cf_map_get_flags( mapstruct* map, mapstruct** nmap, sint16 x, sint16 y, sint16* nx, sint16* ny )
{
    int type;
    return *( int* )cfapiMap_get_flags(&type, map, nmap, x, y, nx, ny);
}

int cf_random_map_set_variable(RMParms* rp, const char* buf)
{
    int type, ret;
    cfapiSet_random_map_variable(&type, rp, buf, &ret);
    return ret;
}

mapstruct* cf_random_map_generate(const char *filename, RMParms *RP, char** use_layout)
{
    int type;
    mapstruct* map;
    cfapiGenerate_random_map(&type, filename, RP, use_layout, &map);
    return map;
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
    int type, anim;
    cfapiSystem_find_animation(&type, txt, &anim);
    return anim;
}
void cf_log( LogLevel logLevel, const char* format, ... )
{
    int type;

    /* Copied from common/logger.c */
    char buf[20480];  /* This needs to be really really big - larger than any other buffer, since that buffer may
    	need to be put in this one. */
    va_list ap;
    va_start(ap, format);
    buf[0] = '\0';
    vsprintf(buf, format, ap);

    cfapiSystem_log(&type, logLevel, buf);

    va_end(ap);
}

void cf_get_time( timeofday_t* tod )
{
    int type;
    cfapiSystem_get_time(&type, tod);
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
    int type, timer;
    cfapiSystem_timer_create(&type, ob, delay, mode, &timer);
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
    int type, code;
    cfapiSystem_timer_destroy(&type, id, &code);
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
    int type;
    const char* value;
    cfapiObject_get_key(&type, op, keyname, &value);
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
    int type, ret;
    cfapiObject_set_key(&type, op, keyname, value, add_key, &ret);
    return ret;
}

/* Archetype-related functions */
archetype*cf_archetype_get_first()
{
    int type;
    archetype* value;
    cfapiArchetype_get_first(&type, &value);
    assert(type == CFAPI_PARCH);
    return value;
}

sstring  cf_archetype_get_name(archetype* arch)
{
    int type;
    sstring name;
    cfapiArchetype_get_property(&type, arch, CFAPI_ARCH_PROP_NAME, &name);
    assert(type == CFAPI_SSTRING);
    return name;
}

archetype* cf_archetype_get_next(archetype* arch)
{
    int type;
    archetype* value;
    cfapiArchetype_get_property(&type, arch, CFAPI_ARCH_PROP_NEXT, &value);
    assert(type == CFAPI_PARCH);
    return value;
}

archetype* cf_archetype_get_more(archetype* arch)
{
    int type;
    archetype* value;
    cfapiArchetype_get_property(&type, arch, CFAPI_ARCH_PROP_MORE, &value);
    assert(type == CFAPI_PARCH);
    return value;
}

archetype* cf_archetype_get_head(archetype* arch)
{
    int type;
    archetype* value;
    cfapiArchetype_get_property(&type, arch, CFAPI_ARCH_PROP_HEAD, &value);
    assert(type == CFAPI_PARCH);
    return value;
}

object* cf_archetype_get_clone(archetype* arch)
{
    int type;
    object* value;
    cfapiArchetype_get_property(&type, arch, CFAPI_ARCH_PROP_CLONE, &value);
    assert(type == CFAPI_POBJECT);
    return value;
}

/* Party-related functions */
partylist* cf_party_get_first(void)
{
	int type;
	return cfapiParty_get_property(&type, NULL, CFAPI_PARTY_PROP_NEXT);
}

const char* cf_party_get_name(partylist* party)
{
	int type;
	return cfapiParty_get_property(&type, party, CFAPI_PARTY_PROP_NAME);
}

partylist* cf_party_get_next(partylist* party)
{
	int type;
	return cfapiParty_get_property(&type, party, CFAPI_PARTY_PROP_NEXT);
}

const char* cf_party_get_password(partylist* party)
{
	int type;
	return cfapiParty_get_property(&type, party, CFAPI_PARTY_PROP_PASSWORD);
}

player* cf_party_get_first_player(partylist* party)
{
	int type;
	return cfapiParty_get_property(&type, party, CFAPI_PARTY_PROP_PLAYER, NULL);
}

player* cf_party_get_next_player(partylist* party, player* op)
{
	int type;
	return cfapiParty_get_property(&type, party, CFAPI_PARTY_PROP_PLAYER, op);
}

region* cf_region_get_first(void)
{
	int type;
	return cfapiRegion_get_property(&type, NULL, CFAPI_REGION_PROP_NEXT);
}

const char* cf_region_get_name(region* reg)
{
	int type;
	return cfapiRegion_get_property(&type, reg, CFAPI_REGION_PROP_NAME);
}

region* cf_region_get_next(region* reg)
{
	int type;
	return cfapiRegion_get_property(&type, reg, CFAPI_REGION_PROP_NEXT);
}

region* cf_region_get_parent(region* reg)
{
	int type;
	return cfapiRegion_get_property(&type, reg, CFAPI_REGION_PROP_PARENT);
}

const char* cf_region_get_longname(region* reg)
{
	int type;
	return cfapiRegion_get_property(&type, reg, CFAPI_REGION_PROP_LONGNAME);
}

const char* cf_region_get_message(region* reg)
{
	int type;
	return cfapiRegion_get_property(&type, reg, CFAPI_REGION_PROP_MESSAGE);
}

/* Friendlylist functions. */
object* cf_friendlylist_get_first(void)
{
    int type;
    return cfapiFriendlylist_get_next(&type, NULL);
}
object* cf_friendlylist_get_next(object* ob)
{
    int type;
    return cfapiFriendlylist_get_next(&type, ob);
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

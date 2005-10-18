/*****************************************************************************/
/* Crossfire Plugin Interface Common Parts                                   */
/* Version: 2.0beta8 (also known as "Alexander")                             */
/* Contact: yann.chachkoff@myrealbox.com                                     */
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
/*                                                                           */ /*****************************************************************************/
#ifndef PLUGIN_COMMON_H
#define PLUGIN_COMMON_H

#include <plugin.h>

extern int cf_init_plugin( f_plug_api getHooks );
extern void cf_system_register_global_event( int event, const char* name, f_plug_api hook );

extern void* cf_object_set_int_property(object* op, int propcode, int value);
extern void* cf_object_get_property(object* op, int propcode);
extern void         cf_free_object( object* ob );
extern void         cf_fix_object( object* pl );
extern char*        cf_add_string( char* str );
extern void         cf_free_string( char* str );
extern char*        cf_strdup_local(char* txt);
extern char*        cf_query_name( object* ob );
extern char*        cf_query_base_name( object* ob, int plural );
extern object*      cf_insert_ob_in_ob( object* ob, object* where );
extern const char*  cf_object_get_msg( object* );
extern void         cf_object_set_weight( object* ob, int weight );
extern int          cf_object_get_weight( object* ob );
extern void         cf_object_set_nrof( object*, int nrof );
extern int          cf_object_get_nrof( object* );
extern int          cf_object_get_flag( object* ob, int flag );
extern void         cf_object_set_flag( object* ob, int flag, int value );
extern object*      cf_object_insert_in_ob( object* ob, object* where );
extern char*        cf_get_maps_directory( char* str );
extern void*        cf_object_set_string_property(object* op, int propcode, char* value);
extern void         cf_object_activate_rune( object* op , object* victim);
extern int          cf_object_check_trigger( object* op, object* cause );
extern void*        cf_map_get_property(mapstruct* map, int propcode);
extern void*        cf_map_set_int_property(mapstruct* map, int propcode, int value);
extern int          cf_object_query_money( object* op);
extern int          cf_object_query_cost( object* op, object* who, int flags);
extern mapstruct*   cf_map_get_map( char* name );
extern char*        cf_player_get_ip(object* op);
extern void*        cf_object_set_string_property(object* op, int propcode, char* value);
extern player*      cf_player_find(char* txt);
extern void         cf_player_message(object* op, char* txt, int flags);
extern int          cf_object_cast_spell(object* caster, object* ctoo, int dir, object* sp, char* flags);
extern void         cf_object_learn_spell(object* op, object* sp);
extern void         cf_object_forget_spell(object* op, object* sp);
extern object*      cf_object_check_for_spell(object* op, char* spellname);
extern int          cf_object_cast_ability(object* caster, object* ctoo, int dir, object* sp, char* flags);
extern int          cf_object_pay_amount(object* op, uint64 amount);
extern int          cf_object_pay_item(object* op, object* buyer);
extern void         cf_map_message(mapstruct* m, char* msg, int color);
extern void*        cf_object_set_long_property(object* op, int propcode, long value);
extern int          cf_object_transfer(object* op,int x,int y,int r,object* orig);
extern int          cf_object_out_of_map( object* op, int x, int y);
extern void         cf_object_drop( object* op, object* author);
extern void         cf_object_take( object* op, object* author);
extern void         cf_object_say( object* op, char* msg);
extern void         cf_object_speak( object* op, char* msg);
extern object*      cf_object_insert_object(object* op, object* container);
extern object*      cf_map_get_object_at(mapstruct* m, int x, int y);
extern object*      cf_object_present_archname_inside(object* op, char* whatstr);
extern object*      cf_player_send_inventory(object* op);
extern void         cf_object_apply(object* op, object* author, int flags);
extern void         cf_object_remove(object* op);
extern void         cf_object_free(object* op);
extern object*      cf_create_object();
extern object*      cf_create_object_by_name( const char* name );
extern object*      cf_map_insert_object(mapstruct* where, object* op, int x, int y);
extern int          cf_object_change_map(object* op, int x, int y, mapstruct* map);
extern int          cf_object_teleport( object* ob, mapstruct* map, int x, int y );
extern object*      cf_map_present_arch_by_name(object* what, mapstruct* map, int nx, int ny);
extern void         cf_object_update( object* op, int flags);
extern void         cf_object_pickup( object* op, object* what);
extern int          cf_map_get_flags( mapstruct* map, mapstruct** nmap, sint16 x, sint16 y, sint16* nx, sint16* ny );
extern int          cf_find_animation(char* txt);
extern char*        cf_object_get_key(object* op, char* keyname);
extern void         cf_object_set_key(object* op, char* keyname, char* value);
extern int          cf_object_get_resistance(object* op, int rtype);
extern void         cf_player_move(player* pl, int dir);
extern void         cf_object_move(object* op, object*originator, int dir);
extern void         cf_object_apply_below(object* op);
extern object*      cf_object_clone(object* op, int clonetype);
extern object*      cf_map_insert_object_there(mapstruct* where, object* op, object* originator, int flags);

/* temp */
extern f_plug_api cfapi_object_get_property;
extern f_plug_api cfapi_object_set_property;
extern f_plug_api cfapi_object_apply;
extern f_plug_api cfapi_object_remove;

#endif /* PLUGIN_COMMON_H */

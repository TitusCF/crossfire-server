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

#ifdef WIN32
#define CF_PLUGIN __declspec(dllexport)
#else
#define CF_PLUGIN
#endif

#include <plugin.h>

#define PLUGIN_ERROR_INT        0x0FFFFF

extern int cf_init_plugin( f_plug_api getHooks );
extern void cf_system_register_global_event( int event, const char* name, f_plug_api hook );

/* General functions */
extern char*        cf_add_string( char* str );
extern void         cf_free_string( char* str );
extern char*        cf_strdup_local(char* txt);
extern char*        cf_get_maps_directory( char* str );
extern int          cf_find_animation(char* txt);

/* Objects */
extern void* cf_object_set_int_property(object* op, int propcode, int value);
extern void* cf_object_get_property(object* op, int propcode);
extern void         cf_free_object( object* ob );
extern void         cf_fix_object( object* pl );
extern char*        cf_query_name( object* ob );
extern char*        cf_query_name_pl( object* ob );
extern char*        cf_query_base_name( object* ob, int plural );
extern object*      cf_insert_ob_in_ob( object* ob, object* where );
extern const char*  cf_object_get_msg( object* );
extern void         cf_object_set_weight( object* ob, int weight );
extern int          cf_object_get_weight( object* ob );
extern void         cf_object_set_weight_limit( object* ob, int weight );
extern int          cf_object_get_weight_limit( object* ob );
extern int          cf_object_set_nrof( object*, int nrof );
extern int          cf_object_get_nrof( object* );
extern int          cf_object_get_flag( object* ob, int flag );
extern void         cf_object_set_flag( object* ob, int flag, int value );
extern object*      cf_object_insert_in_ob( object* ob, object* where );
extern void*        cf_object_set_string_property(object* op, int propcode, char* value);
extern void         cf_object_activate_rune( object* op , object* victim);
extern int          cf_object_check_trigger( object* op, object* cause );
extern int          cf_object_query_money( object* op);
extern int          cf_object_query_cost( object* op, object* who, int flags);
extern void*        cf_object_set_string_property(object* op, int propcode, char* value);
extern int          cf_object_cast_spell(object* caster, object* ctoo, int dir, object* sp, char* flags);
extern void         cf_object_learn_spell(object* op, object* sp);
extern void         cf_object_forget_spell(object* op, object* sp);
extern object*      cf_object_check_for_spell(object* op, char* spellname);
extern int          cf_object_cast_ability(object* caster, object* ctoo, int dir, object* sp, char* flags);
extern int          cf_object_pay_amount(object* op, uint64 amount);
extern int          cf_object_pay_item(object* op, object* buyer);
extern void*        cf_object_set_long_property(object* op, int propcode, long value);
extern int          cf_object_transfer(object* op,int x,int y,int r,object* orig);
extern int          cf_object_out_of_map( object* op, int x, int y);
extern void         cf_object_drop( object* op, object* author);
extern void         cf_object_take( object* op, object* author);
extern void         cf_object_say( object* op, char* msg);
extern void         cf_object_speak( object* op, char* msg);
extern object*      cf_object_insert_object(object* op, object* container);
extern object*      cf_object_present_archname_inside(object* op, char* whatstr);
extern void         cf_object_apply(object* op, object* author, int flags);
extern void         cf_object_remove(object* op);
extern void         cf_object_free(object* op);
extern object*      cf_create_object(void);
extern object*      cf_create_object_by_name( const char* name );
extern int          cf_object_change_map(object* op, int x, int y, mapstruct* map);
extern int          cf_object_teleport( object* ob, mapstruct* map, int x, int y );
extern void         cf_object_update( object* op, int flags);
extern void         cf_object_pickup( object* op, object* what);
extern char*        cf_object_get_key(object* op, char* keyname);
extern void         cf_object_set_key(object* op, char* keyname, char* value);
extern int          cf_object_get_resistance(object* op, int rtype);
extern void         cf_object_move(object* op, object*originator, int dir);
extern void         cf_object_apply_below(object* op);
extern object*      cf_object_clone(object* op, int clonetype);

/* Maps */
extern void*        cf_map_get_property(mapstruct* map, int propcode);
extern void*        cf_map_set_int_property(mapstruct* map, int propcode, int value);
extern mapstruct*   cf_map_get_map( char* name );
extern mapstruct*   cf_map_get_first(void);
extern void         cf_map_message(mapstruct* m, char* msg, int color);
extern object*      cf_map_get_object_at(mapstruct* m, int x, int y);
extern object*      cf_map_insert_object(mapstruct* where, object* op, int x, int y);
extern object*      cf_map_present_arch_by_name(const char* str, mapstruct* map, int nx, int ny);
extern int          cf_map_get_flags( mapstruct* map, mapstruct** nmap, sint16 x, sint16 y, sint16* nx, sint16* ny );
extern object*      cf_map_insert_object_there(mapstruct* where, object* op, object* originator, int flags);
extern int          cf_map_get_difficulty(mapstruct* map);
extern int          cf_map_get_reset_time(mapstruct* map);
extern int          cf_map_get_reset_timeout(mapstruct* map);
extern int          cf_map_get_players(mapstruct* map);
extern int          cf_map_get_darkness(mapstruct* map);
extern int          cf_map_get_width(mapstruct* map);
extern int          cf_map_get_height(mapstruct* map);
extern int          cf_map_get_enter_x(mapstruct* map);
extern int          cf_map_get_enter_y(mapstruct* map);
extern int          cf_map_get_temperature(mapstruct* map);
extern int          cf_map_get_pressure(mapstruct* map);
extern int          cf_map_get_humidity(mapstruct* map);
extern int          cf_map_get_windspeed(mapstruct* map);
extern int          cf_map_get_winddir(mapstruct* map);
extern int          cf_map_get_sky(mapstruct* map);
extern int          cf_map_get_wpartx(mapstruct* map);
extern int          cf_map_get_wparty(mapstruct* map);

/* Players */
extern char*        cf_player_get_ip(object* op);
extern object*      cf_player_get_marked_item(object* op);
extern void         cf_player_set_marked_item(object* op, object* ob);
extern player*      cf_player_find(char* txt);
extern void         cf_player_message(object* op, char* txt, int flags);
extern object*      cf_player_send_inventory(object* op);
extern void         cf_player_move(player* pl, int dir);
extern partylist*   cf_player_get_party(object* op);
extern void         cf_player_set_party(object* op, partylist* party);

/* Archetypes */
extern archetype*	cf_archetype_get_first(void);
extern const char*  cf_archetype_get_name(archetype* arch);
extern archetype*   cf_archetype_get_next(archetype* arch);
extern archetype*   cf_archetype_get_more(archetype* arch);
extern archetype*   cf_archetype_get_head(archetype* arch);
extern object*      cf_archetype_get_clone(archetype* arch);

/* Parties */
extern partylist*   cf_party_get_first(void);
extern const char*  cf_party_get_name(partylist* party);
extern partylist*   cf_party_get_next(partylist* party);
extern const char*  cf_party_get_password(partylist* party);
extern player*      cf_party_get_first_player(partylist* party);
extern player*      cf_party_get_next_player(partylist* party, player* op);

/* Regions */
extern region*      cf_region_get_first(void);
extern const char*  cf_region_get_name(region* reg);
extern region*      cf_region_get_next(region* reg);
extern region*      cf_region_get_parent(region* reg);
extern const char*  cf_region_get_longname(region* reg);
extern const char*  cf_region_get_message(region* reg);

/* temp */
extern f_plug_api cfapi_object_get_property;
extern f_plug_api cfapi_object_set_property;
extern f_plug_api cfapi_object_apply;
extern f_plug_api cfapi_object_remove;

#ifdef WIN32

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};
int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);

#endif

#endif /* PLUGIN_COMMON_H */

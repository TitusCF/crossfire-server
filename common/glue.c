/*
 * static char *rcsid_glue_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
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

    The author can be reached via e-mail to master@rahul.net
*/

#include <global.h>

/*
 * All this glue is currently needed to connect the game with the
 * server.  I'll try to make the library more able to "stand on it's
 * own legs" later; not done in 5 minutes to separate two parts of
 * the code which were connected, well, can you say "spagetti"?
 */

type_func_int	emergency_save_func;
type_func_void	clean_tmp_files_func;
type_func_map	fix_auto_apply_func;
type_func_ob	remove_friendly_object_func;
type_func_void	process_active_maps_func;
type_func_map	update_buttons_func;
type_func_int_int_ob_cchar	draw_info_func;
type_move_apply_func	move_apply_func;
type_func_ob	draw_func;
type_func_ob_ob	monster_check_apply_func;
type_func_void	init_blocksview_players_func;
type_func_int_map_char	info_map_func;
type_func_ob	move_teleporter_func;
type_func_ob move_firewall_func;
type_func_ob_int  trap_adjust_func;
type_func_ob    move_creator_func;
type_func_ob_ob esrv_send_item_func;
type_func_player_int esrv_del_item_func;
type_func_int_ob_ob esrv_update_item_func;

static char *fatalmsgs[80]={
  "Failed to allocate memory",
  "Failed repeatedly to load maps",
  "Hashtable for archetypes is too small",
  "Too many errors"
};

/*
 * Initialise all function-pointers to dummy-functions which will
 * do nothing and return nothing (of value at least).
 * Very healthy to do this when using the library, since
 * function pointers are being called throughout the library, without
 * being checked first.
 * init_library() calls this function.
 */

void init_function_pointers() {
  emergency_save_func = dummy_function_int;
  clean_tmp_files_func = dummy_function;
  fix_auto_apply_func = dummy_function_map;
  remove_friendly_object_func = dummy_function_ob;
  process_active_maps_func = dummy_function;
  update_buttons_func = dummy_function_map;
  draw_info_func = dummy_draw_info;
  move_apply_func = dummy_move_apply_func;
  draw_func = dummy_function_ob;
  monster_check_apply_func = dummy_function_ob2;
  init_blocksview_players_func = dummy_function;
  info_map_func = dummy_function_mapstr;
  move_teleporter_func = dummy_function_ob;
  move_firewall_func = dummy_function_ob;
  trap_adjust_func = dummy_function_ob_int;
  move_creator_func = dummy_function_ob;
  esrv_send_item_func = dummy_function_ob2;
  esrv_del_item_func = dummy_function_player_int;
  esrv_update_item_func = dummy_function_int_ob_ob;

}

/*
 * Specifies which function to call when there is an emergency save.
 */

void set_emergency_save(type_func_int addr) {
  emergency_save_func = addr;
}

/*
 * Specifies which function to call to clean temporary files.
 */

void set_clean_tmp_files(type_func_void addr) {
  clean_tmp_files_func = addr;
}

/*
 * Specifies which function to call to fix auto-apply (objects which
 * applies themselves when their map is loaded).
 */

void set_fix_auto_apply(type_func_map addr) {
  fix_auto_apply_func = addr;
}

/*
 * Specifies which function to call to remove an object in the
 * linked list of friendly objects.
 */

void set_remove_friendly_object(type_func_ob addr) {
  remove_friendly_object_func = addr;
}

/*
 * Specify which function to call to do some work in active
 * maps.  That function might get called if there are several players,
 * and one player is using too much cpu to either load a map, or to
 * load pixmaps.
 */

void set_process_active_maps(type_func_void addr) {
  process_active_maps_func = addr;
}

/*
 * Specify which function to call to recoordinate all buttons.
 */

void set_update_buttons(type_func_map addr) {
  update_buttons_func = addr;
}

/*
 * Specify which function to call to draw text to the window
 * of a player.
 */

void set_draw_info(type_func_int_int_ob_cchar addr) {
  draw_info_func = addr;
}

/*
 * Specify which function to call to apply an object.
 */

void set_move_apply(type_move_apply_func addr) {
  move_apply_func = addr;
}

/*
 * Specify which function to call to draw on the graphic-window
 * of a player.
 */

void set_draw(type_func_ob addr) {
  draw_func = addr;
}

/*
 * Specify which function to call to check if a monster can
 * apply an object.
 */

void set_monster_check_apply(type_func_ob_ob addr) {
  monster_check_apply_func = addr;
}

/*
 * Specify which functino to call to initialise the blocksview[] array.
 */

void set_init_blocksview_players(type_func_void addr) {
  init_blocksview_players_func = addr;
}

void set_info_map(type_func_int_map_char addr) {
  info_map_func = addr;
}

void set_move_teleporter(type_func_ob addr) {
  move_teleporter_func = addr;
}

void set_move_firewall(type_func_ob addr) {
  move_firewall_func = addr;
}

void set_trap_adjust(type_func_ob_int addr) {
  trap_adjust_func = addr;
}

void set_move_creator(type_func_ob addr) {
  move_creator_func = addr;
}

void set_esrv_send_item (type_func_ob_ob addr) {
  esrv_send_item_func = addr;
}

void set_esrv_update_item (type_func_int_ob_ob addr) {
  esrv_update_item_func = addr;
}

void set_esrv_del_item(type_func_player_int addr) {
  esrv_del_item_func = addr;
}

/*
 * fatal() is meant to be called whenever a fatal signal is intercepted.
 * It will call the emergency_save and the clean_tmp_files functions.
 */

void fatal(int err) {
  fprintf(logfile,"Fatal: %s\n",fatalmsgs[err]);
  (*emergency_save_func)(0);
  (*clean_tmp_files_func)();
  fprintf(logfile,"Exiting...\n");
  exit(err);
}

#ifndef __Making_docs__ /* Don't want documentation on these */

/*
 * These are only dummy functions, to avoid having any function-pointers
 * having the possibility of pointing to NULL (or random location),
 * thus I don't have to check the contents of a function-pointer each
 * time I want to jump to it.
 */

void dummy_function_int(int i) {
}

void dummy_function_int_int(int i, int j) {
}

void dummy_function_player_int(player *p, int j) {
}


void dummy_function() {
}

void dummy_function_map(mapstruct *m) {
}

void dummy_function_ob(object *ob) {
}

void dummy_function_ob2(object *ob, object *ob2) {
}

int dummy_function_ob2int(object *ob, object *ob2) {
  return 0;
}

void dummy_function_ob_int(object *ob, int i) {
}

void dummy_function_txtnr(char *txt, int nr) {
  fprintf(logfile,"%d: %s\n",nr,txt);
}

void dummy_draw_info(int a, int b, object *ob, const char *txt) {
  fprintf(logfile, "%s\n", txt);
}

void dummy_function_mapstr(int a, mapstruct *map, char *str) {
  fprintf(logfile, "info_map: %s\n", str);
}

void dummy_function_int_ob_ob (int n, object *ob, object *ob2) {
}

void dummy_move_apply_func (object *ob, object *ob2, object *ob3) {
}

#endif


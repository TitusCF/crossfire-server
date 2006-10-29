/*
 * static char *rcsid_funcpoint_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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

    This file was made after an idea by vidarl@ifi.uio.no
*/

#ifndef FUNCPOINT_H
#define FUNCPOINT_H

/*
 * These function-pointers are defined in common/glue.c
 * The functions used to set and initialise them are also there.
 *
 * Massive change. Those functions are just defined, no callback, and they should be implemented.
 * This means glue.c code & such can go away almost entirely.
 * Ryo 2005-07-15
 */

extern void	move_apply(object *, object *, object *);
extern void	emergency_save(int);
extern void	clean_tmp_files(void);
extern void	fix_auto_apply(mapstruct *);
extern void	init_blocksview_players(void);
extern void	monster_check_apply(object *, object *);
extern void	remove_friendly_object(object *);
extern void	update_buttons(mapstruct *);
extern void	move_teleporter(object *);
extern void	move_firewall(object *);
extern void	move_creator(object *);
extern void move_marker(object *);
extern void	move_duplicator(object *);
extern void trap_adjust(object *, int);
extern void	esrv_send_item(object *, object *);
extern void	esrv_del_item(player *, int);
extern void	esrv_update_item(int, object *, object *);
extern void	esrv_update_spells(player *);
extern void	set_darkness_map(mapstruct *m);
extern void dragon_ability_gain(object *, int, int);
extern void	weather_effect(const char *);
extern object *	find_skill_by_number(object *, int);
extern int execute_event(object *, int, object *, object *, const char *, int);
extern int execute_global_event(int , ...);

#endif

/*
 * static char *rcsid_c_move_c =
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

    The author can be reached via e-mail to crossfire-devel@real-time.com
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <skills.h>

static int move_internal (object *op, char *params, int dir)
{
  if (params) {
    if (params[0] == 'f') {
      if (!op->contr->fire_on) {
	op->contr->fire_on =1;
	move_player(op, dir);
	op->contr->fire_on =0;
	return 0;
      }
    } else if (params[0] == 'r' && !op->contr->run_on)
      op->contr->run_on =1;
  }
  move_player(op, dir);
  return 0;
}

int command_east (object *op, char *params)
{
  return move_internal(op, params, 3);
}

int command_north (object *op, char *params)
{
  return move_internal(op, params, 1);
}

int command_northeast (object *op, char *params)
{
  return move_internal(op, params, 2);
}

int command_northwest (object *op, char *params)
{
  return move_internal(op, params, 8);
}

int command_south (object *op, char *params)
{
  return move_internal(op, params, 5);
}

int command_southeast (object *op, char *params)
{
  return move_internal(op, params, 4);
}

int command_southwest (object *op, char *params)
{
  return move_internal(op, params, 6);
}

int command_west (object *op, char *params)
{
  return move_internal(op, params, 7);
}

int command_stay (object *op, char *params)
{
  if (!op->contr->fire_on && (!params || params[0] != 'f'))
    return 0;
  fire(op, 0);
  return 0;
}


/*  peterm:  command_search finds runes.  It uses:
           trap_see uses various rules to determine whether
                   the trap is seen or not.  It informs op if
                   a trap is found.
	   trap_show(object *trap, object *where) briefly shows up the trap, 
	    as if it detonated.  This is just to inform the player of where the trap
            is:  it might be hidden underneath something.
  It searches both around the player--9 squares counting the one he's on
  and the inventories of all the objects around him for traps.  (Including his own!!  Hmmm.)

*/

int command_search (object *op, char *params) {  
   if(!change_skill(op,SK_FIND_TRAPS))
        return 0;
   else {
        int success = do_skill(op,0,NULL);
        return success;
   }
}

int command_disarm (object *op, char *params) {
   if(!change_skill(op,SK_REMOVE_TRAP)) 
        return 0;
   else {
        int success = do_skill(op,0,NULL); 
        return success;
   }
}

  

/*
 * static char *rcsid_egoitem_c =
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

    The authors can be reached via e-mail to crossfire-devel@real-time.com
*/
 

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#if 0
/* This is no longer use - should be removed if in fact it won't be used.
 * MSW 2006-06-02
 */

/* GROS: I put this here, because no other file seemed quite good. Returns 1 if
 * the artifact could be created.
 */
static int create_artifact(object *op, const char *artifactname)
{
        artifactlist *al;
        artifact *art;
        char *temptitle;
        al = find_artifactlist(op->type);
        if (al==NULL)
                return 0;
        for (art=al->items; art!=NULL; art=art->next)
        {
                temptitle = (char *)(malloc(strlen(art->item->name) + 5));
                strcpy(temptitle, " of ");
                strcat(temptitle, art->item->name);
                if (!strcmp (temptitle, artifactname))
                {
                        give_artifact_abilities(op, art->item);
                        free(temptitle);
                        return 1;
                }

                free(temptitle);
        };
        return 0;
}
#endif

/*  peterm:  do_power_crystal

  object *op, object *crystal

  This function handles the application of power crystals.
  Power crystals, when applied, either suck power from the applier,
  if he's at full spellpoints, or gives him power, if it's got 
  spellpoins stored.  

*/

int apply_power_crystal(object *op, object *crystal) {
  int available_power;
  int power_space;
  int power_grab;

  available_power =  op->stats.sp - op->stats.maxsp;
  power_space = crystal->stats.maxsp - crystal->stats.sp;
  power_grab = 0;
  if(available_power>=0 && power_space> 0 )  
        power_grab = MIN ( power_space, 0.5 * op->stats.sp );
  if(available_power < 0 && crystal->stats.sp >0 ) 
        power_grab = - MIN( -available_power, crystal->stats.sp);

  op->stats.sp-=power_grab;
  crystal->stats.sp +=power_grab;
  crystal->speed = (float)crystal->stats.sp/(float)crystal->stats.maxsp;
  update_ob_speed(crystal);
  if (op->type == PLAYER)
    esrv_update_item(UPD_ANIMSPEED, op, crystal);

  return 1;
}


/*
 * static char *rcsid_player_c =
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
#include <funcpoint.h>

/*
 * Returns a newly allocated and initialised and correctly
 * linked player structure.
 */

player *get_player_ob() {
  int i;
  player *new,*tmp;

  new = (player *) CALLOC(1,sizeof(player));
  if(new==NULL)
    fatal(OUT_OF_MEMORY);
  if(!editor) {
    tmp=first_player;
    while(tmp!=NULL&&tmp->next!=NULL)
      tmp=tmp->next;
    if(tmp!=NULL)
      tmp->next=new;
    else
      first_player=new;
  }
  new->next=NULL;
  new->known_spell = 0;
  new->removed=0;
  new->party_number=-1;
  new->last_known_spell = 0;
  new->state=0;

  new->use_pixmaps=0;
  new->color_pixmaps=0;
  new->ob = get_object();
  new->ob->type = PLAYER;
  new->ob->contr = new;
  new->ob->name = add_string("logon");
  new->shoottype = range_none;
  new->last_shoot = range_size;
  new->braced =0;
  memset(&new->last_resist, 0, sizeof(new->last_resist));
#ifdef SEARCH_ITEMS
  new->search_str[0]=0;
#endif

#ifdef USE_SWAP_STATS
  new->Swap_First = -1;
#endif

  for (i=0; i<NUM_OUTPUT_BUFS; i++) {
	new->outputs[i].buf=NULL;
	new->outputs[i].first_update=0;
	new->outputs[i].count=0;
  }
  new->outputs_sync=16;		/* Every 2 seconds */
  new->outputs_count=1;		/* Keeps present behaviour */
  new->mark=NULL;
  new->mark_count=0;
  new->last_flags=0;
  new->last_weight_limit=0;
  return new;
}

void free_player(player *pl) {

    if (first_player!=pl) {
	player *prev=first_player;
	while(prev!=NULL&&prev->next!=NULL&&prev->next!=pl)
	    prev=prev->next;
	if(prev->next!=pl) {
	    LOG(llevError,"Free_player: Can't find previous player.\n");
	    exit(1);
	}
	prev->next=pl->next;
    } else first_player=pl->next;

    if(pl->ob != NULL) {
	if (!QUERY_FLAG(pl->ob, FLAG_REMOVED)) remove_ob(pl->ob);
	free_object(pl->ob);
    }

    CFREE(pl);
}

/* find_skill() - looks for the skill and returns a pointer to it if found */

object *find_skill(object *op, int skillnr) {
    object *tmp, *skill1=NULL;
#ifdef LINKED_SKILL_LIST
  objectlink *obl;

  if(op->sk_list!=NULL) {   /* use fast method */ 
     for(obl=op->sk_list;obl;obl=obl->next) 
         if(obl->id==skillnr&&obl->ob) return obl->ob;
  } 
#endif

    /* *sigh*; we didnt find it. Perhaps because the skill we requested
     * is an unapplied tool. Lets search entire inventory */
    for (tmp=op->inv;tmp;tmp=tmp->below) {
	if(tmp->type==SKILL&&tmp->stats.sp==skillnr) {
	    if (tmp->invisible) return tmp;
	    else skill1=tmp;
	}
    }
    return skill1;
}

/*
 * static char *rcsid_swamp_c =
 *   "$Id$";
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

void walk_on_deep_swamp (object *op, object *victim)
{
  if (victim->type == PLAYER && ! QUERY_FLAG (victim, FLAG_FLYING)
      && victim->stats.hp >= 0)
  {
	new_draw_info (NDI_UNIQUE, 0, victim, "You are down to your knees "
                       "in the swamp.");
	op->stats.food = 1;
	victim->speed_left -= SLOW_PENALTY(op);
  }
}

void move_deep_swamp (object *op)
{
  object *above = op->above;
  object *nabove;

  while(above) {
    nabove = above->above;
    if (above->type == PLAYER && 
	!QUERY_FLAG(above, FLAG_FLYING) &&
	above->stats.hp >= 0) {

      if (op->stats.food < 1) {
        LOG (llevDebug, "move_deep_swamp(): player is here, but state is "
             "%d\n", op->stats.food);
        op->stats.food = 1;
      }
      switch(op->stats.food) {
	case 1:
	  if (rndm(0, 2) == 0) {
	    new_draw_info(NDI_UNIQUE, 0,above, "You are down to your waist in the wet swamp.");
	    op->stats.food = 2;
	    above->speed_left -= SLOW_PENALTY(op);
	  }
	  break;
	case 2:
	  if (rndm(0, 2) == 0) {
	    new_draw_info(NDI_UNIQUE, 0,above, "You are down to your NECK in the dangerous swamp.");
	    op->stats.food = 3;
	    strcpy(above->contr->killer,"drowning in a swamp");
	    above->stats.hp--;
	    above->speed_left -= SLOW_PENALTY(op);
	  }
	  break;
	case 3:
	  if (rndm(0, 4) == 0) {
	    /* player is ready to drown - only woodsman skill can save him */
	    if (rndm(0, 4) == 0 || !change_skill(above, lookup_skill_by_name("woodsman"))) {
	      op->stats.food = 0;
	      new_draw_info_format(NDI_UNIQUE | NDI_ALL, 1, NULL,
		"%s disappeared into a swamp.",above->name);
	      strcpy(above->contr->killer,"drowning in a swamp");
	      
	      above->stats.hp = -1;
	      kill_player(above); /* player dies in the swamp */
	    }
	    else {
	      op->stats.food = 2;
	      new_draw_info(NDI_UNIQUE, 0,above, "You almost drowned in the swamp! You");
	      new_draw_info(NDI_UNIQUE, 0,above, "survived due to your woodsman skill.");
	    }
	  }
	break;
      }
    } else if (!QUERY_FLAG(above, FLAG_ALIVE)) {
      if (rndm(0, 2) == 0) decrease_ob(above);
    }
    above = nabove;
  }
}

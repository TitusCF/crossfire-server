/*
 * static char *rcsid_swamp_c =
 *   "$Id$";
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

void deep_swamp(object *op, int walk_on)
{
  object *above = op->above;
  object *nabove;

  while(above) {
    nabove = above->above;
    if (above->type == PLAYER && 
	!QUERY_FLAG(above, FLAG_FLYING) &&
	above->stats.hp >= 0) {

      if (op->stats.food == 0 || walk_on) {
	new_draw_info(NDI_UNIQUE, 0,above, "You are down to your knees in the swamp.");
	op->stats.food = 1;
	above->speed_left -= SLOW_PENALTY(op);
      } else {
	switch(op->stats.food) {
	case 1:
	  if (RANDOM()%3 == 0) {
	    new_draw_info(NDI_UNIQUE, 0,above, "You are down to your waist in the wet swamp.");
	    op->stats.food = 2;
	    above->speed_left -= SLOW_PENALTY(op);
	  }
	  break;
	case 2:
	  if (RANDOM()%3 == 0) {
	    new_draw_info(NDI_UNIQUE, 0,above, "You are down to your NECK in the dangerous swamp.");
	    op->stats.food = 3;
	    strcpy(above->contr->killer,"drowning in a swamp");
	    above->stats.hp--;
	    above->speed_left -= SLOW_PENALTY(op);
	  }
	  break;
	case 3:
	  if (RANDOM()%5 == 0) {
	    op->stats.food = 0;
	    new_draw_info_format(NDI_UNIQUE | NDI_ALL, 1, NULL,
		"%s disappeared into a swamp.",above->name);
	    strcpy(above->contr->killer,"drowning in a swamp");
	    above->stats.hp = -1;
	  }
	  break;
	}
      }
    } else if (!QUERY_FLAG(above, FLAG_ALIVE)) {
      if (RANDOM()%3 == 0) decrease_ob(above);
    }
    above = nabove;
  }
}

/*
 * Movement commands
 *	++Jam
 */

#include <global.h>
#include <sproto.h>
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
#ifndef ALLOW_SKILLS
   object *tmp,*tmp2;
   int i;
  /*First we search all around us for runes and traps, which are
    all type RUNE */
   for(i=0;i<9;i++) { 
	/*  Check everything in the square for trapness */
	if(out_of_map(op->map,op->x + freearr_x[i],op->y + freearr_y[i])) continue;
	for(tmp = get_map_ob(op->map, op->x + freearr_x[i], op->y +freearr_y[i]);
	    tmp!=NULL;tmp=tmp->above) 
	  {
	    /*  And now we'd better do an inventory traversal of each
	        of these objects' inventory */
	    for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below) 
		if(tmp2->type==RUNE) { if(trap_see(op,tmp2)) trap_show(tmp2,tmp); }
	    if(tmp->type==RUNE) { if(trap_see(op,tmp)) trap_show(tmp,tmp);}
	    }
   }
  return 0;
#else	/* ALLOW_SKILLS */ 
   if(!change_skill(op,SK_FIND_TRAPS))
        return 0;
   else {
        int success = do_skill(op,0,NULL);
        return success;
   }
#endif /* ALLOW_SKILLS */ 
}

int command_disarm (object *op, char *params) {
#ifndef ALLOW_SKILLS
  object *tmp,*tmp2;
  int i;

  /*First we search all around us for runes and traps, which are
    all type RUNE */
   for(i=0;i<9;i++) { 
        if(out_of_map(op->map,op->x + freearr_x[i],op->y + freearr_y[i])) continue;
	/*  Check everything in the square for trapness */
	for(tmp = get_map_ob(op->map, op->x + freearr_x[i], op->y +freearr_y[i]);
	    tmp!=NULL;tmp=tmp->above) 
	  {
	    /*  And now we'd better do an inventory traversal of each
	        of these objects' inventory */
	    for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below) 
		if(tmp2->type==RUNE&&tmp2->stats.Cha<=1) { 
			trap_show(tmp2,tmp); 
			return trap_disarm(op,tmp2,1);
		}
	    if(tmp->type==RUNE&&tmp->stats.Cha<=1) { 
		trap_show(tmp,tmp);
		return trap_disarm(op,tmp,1);
	    }
	  }
   }

return 0;
#else
   if(!change_skill(op,SK_REMOVE_TRAP)) 
        return 0;
   else {
        int success = do_skill(op,0,NULL); 
        return success;
   }
#endif
}

  

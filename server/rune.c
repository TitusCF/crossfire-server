/*
 * static char *rcsid_rune_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <errno.h>
#ifdef sequent
/* stoopid sequent includes don't do this like they should */
extern char * sys_errlist[];
extern int sys_nerr;
#endif

extern object *objects;

/*  peterm:  
    write_rune:
    op:  rune writer
    dir:  orientation of rune, direction rune's contained spell will
	    be cast in, if applicable
    inspell:  the spell index of the spell in the rune (if any)
    level:  level of casting of the rune
    runename:  name of the rune or message displayed by the rune for
		a rune of marking 

*/

int write_rune(object *op,int dir,int inspell,int level,char *runename) { 
    object *tmp;
    archetype *at=NULL;
    char buf[MAX_BUF];
    
    int nx,ny;

      if(!dir) {
	dir=1;
    } 

    nx=op->x+freearr_x[dir];
    ny=op->y+freearr_y[dir];
    if(blocked(op->map,nx,ny)) {
	new_draw_info(NDI_UNIQUE, 0,op,"Can't make a rune there!");
	return 0;
    }
    for(tmp=get_map_ob(op->map,nx,ny);tmp!=NULL;tmp=tmp->above)
	if(tmp->type==RUNE) break;
    if(tmp){
#if 0
	new_draw_info(NDI_UNIQUE, 0,op,"You only succeed in strengthening that rune.");
	tmp->stats.hp++;
	return 1;
#endif
      new_draw_info(NDI_UNIQUE, 0,op,"You can't write a rune there.");
      return 0;
    }
    if(inspell) {  /* can't have runes of small fireball!!!  */
	if(inspell==-1) { new_draw_info(NDI_UNIQUE, 0,op,"You can't make a rune containing");
			  new_draw_info(NDI_UNIQUE, 0,op,"a spell you don't know.  (idiot!)");
			  return 0;
	}
	at=find_archetype(runename);

	/* What it compares to should probably be expanded.  But basically,
	 * creating a rune of sword should not be allowed. */
	if (at && at->clone.type!=RUNE) {
	     new_draw_info_format(NDI_UNIQUE, 0, op,
		"You can't make a rune of %s", runename);
		return 0;
	}
        /* next it attempts to look up a rune_archetype for this spell
		by doing some string manipulations */
	if(!at) {
		char insp[MAX_BUF];
		int i;
		strcpy(insp,spells[inspell].name);
		for(i=0;i<strlen(insp);i++)
			if(insp[i]==' ') insp[i]='_';
		sprintf(buf,"%s%s","rune_",insp);
		at=find_archetype(buf);
	}
		
	if(!at) tmp=get_archetype("generic_rune");
	else
	tmp=arch_to_object(at);

	tmp->stats.sp=inspell; /* the spell it contains */

	sprintf(buf,"You set off a rune of %s",spells[inspell].name);
	tmp->msg=add_string(buf); 
	at=NULL;


	/*  the at=find_archetye(runename) is neccessary because
	 * tmp=get_archetype returns a singulirity, not a null,
	 * when it cannot find the archetype.
	 * note: if some smartass
	 * cast rune of marking, and gives the exact name
	 * of a powerful rune, it won't do him any good,
	 *  because a rune of marking will have level 0
	 * and will thus never detonate. 
	 */
    } else if (level==-2 || (at=find_archetype(runename))==NULL) {
	char rune[HUGE_BUF];

	level=0;
	tmp=get_archetype("rune_mark"); /* this is a rune of marking */
	at=NULL;
	if (runename) {
	    strncpy(rune, runename, HUGE_BUF-2);
	    rune[HUGE_BUF-2] = 0;
	    strcat(rune, "\n");
	} else {
	    /* Not totally efficient, but keeps code simpler */
	    strcpy(rune, "There is no message\n");
	}
	tmp->msg = add_string(rune);
    }
    if(at) tmp=get_archetype(runename);
    tmp->stats.Cha = op->level/2;  /* the invisibility parameter */
    tmp->x=nx;
    tmp->y=ny;
    tmp->map = op->map;
    tmp->direction=dir;  /* where any spell will go upon detonation */
    tmp->level=SK_level(op);  /* what level to cast the spell at */
    if(inspell||tmp->stats.dam) set_owner(tmp,op); /* runes without need no owner */
    insert_ob_in_map(tmp,op->map,op,0);
    return 1;

}


/*  move_rune:  peterm
  comments on runes:
    rune->level	    :	    level at which rune will cast its spell.
    rune->hp	    :	    number of detonations before rune goes away
    rune->sp	    :	    index of the spell the rune casts
    rune->msg	    :	    message the rune displays when it goes off
    rune->direction :	    direction it will cast a spell in
    rune->dam	    :	    damage the rune will do if it doesn't cast spells
    rune->attacktype:	    type of damage it does, if not casting spells
    rune->slaying   :       string description of the spell in the rune

*/

void move_rune(object *op) {
    int det=0;
    if(!op->level) {return;}  /* runes of level zero cannot detonate. */
    det=op->invisible;
    if(!(rndm(0, MAX(1,(op->stats.Cha))-1)))
	{
	op->invisible=0;
	op->speed_left-=1;
	}
    else
	op->invisible=1;
    if(op->invisible!=det)
    update_object(op,UP_OBJ_FACE);
}


/*  peterm: rune_attack

  function handles those runes which detonate but do not cast spells.  */


void rune_attack(object *op,object *victim)
{
    if(victim) {
         tag_t tag = victim->count;
	 hit_player(victim,op->stats.dam,op,op->attacktype);
         if (was_destroyed (victim, tag))
                return;
	 /*  if there's a disease in the needle, put it in the player */
	 if(op->randomitems!=NULL) create_treasure(op->randomitems,op,0,
		(victim->map?victim->map->difficulty:1),0);
	 if(op->inv && op->inv->type == DISEASE) {
		object *disease=op->inv;
		infect_object(victim, disease, 1);
		remove_ob(disease);
		free_object(disease);
	 }
    }
    else  hit_map(op,0,op->attacktype);
}

/*  This function generalizes attacks by runes/traps.  This ought to make
    it possible for runes to attack from the inventory, 
    it'll spring the trap on the victim.  */
   
void spring_trap(object *trap,object *victim)
{  int spell_in_rune;
   object *env;
   tag_t trap_tag = trap->count;

  /* Prevent recursion */
  if (trap->stats.hp <= 0)
    return;

  /*  get the spell number from the name in the slaying field, and set
      that as the spell to be cast. */
  if ((spell_in_rune = look_up_spell_by_name (NULL, trap->slaying)) != -1)
    trap->stats.sp=spell_in_rune;

  /* Only living objects can trigger runes that don't cast spells, as
   * doing direct damage to a non-living object doesn't work anyway.
   * Typical example is an arrow attacking a door.
   */
  if ( ! QUERY_FLAG (victim, FLAG_ALIVE) && ! trap->stats.sp)
    return;

  trap->stats.hp--;  /*decrement detcount */
  if(victim) if(victim->type==PLAYER) new_draw_info(NDI_UNIQUE, 0,victim,trap->msg);
  /*  Flash an image of the trap on the map so the poor sod
   *   knows what hit him.  */
  for (env = trap; env->env != NULL; env = env->env)
    ;
  trap_show(trap,env);  

  if ( ! trap->stats.sp)
  {
    rune_attack(trap,victim); 
    if (was_destroyed (trap, trap_tag))
      return;
  }
  else
  {
    /* This is necessary if the trap is inside something else */
    remove_ob(trap);
    trap->x=victim->x;trap->y=victim->y;
    insert_ob_in_map(trap,victim->map,trap,0);
    if (was_destroyed (trap, trap_tag))
      return;
    cast_spell(trap,trap,trap->direction,trap->stats.sp,1,spellNormal,NULL);
  }

  if (trap->stats.hp <= 0) {
    trap->type=98;  /* make the trap impotent */
    trap->stats.food=20;  /* make it stick around until its spells are gone */
    SET_FLAG(trap,FLAG_IS_USED_UP);
  }
}

/*  dispel_rune:  by peterm  
    dispels the target rune, depending on the level of the actor
and the level of the rune  risk flag, if true, means that there is
a chance that the trap/rune will detonate */

int dispel_rune(object *op,int dir,int risk)
{
  object *tmp,*tmp2;
  int searchflag = 1;
   if(out_of_map(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) return 0;
   for(tmp=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
        tmp!=NULL;  tmp=tmp->above) 
	{
	  if(tmp->type==RUNE) break;
		/* now search tmp's inventory for traps */
	  for(tmp2=tmp->inv;tmp2!=NULL;tmp2=tmp2->below) {
		if(tmp2->type==RUNE) { 
			tmp=tmp2;
			searchflag=0;
			break;
		}
	  }
	  if(!searchflag) break;
	}
		
  if(tmp==NULL)               /*no rune there. */
    {
	new_draw_info(NDI_UNIQUE, 0,op,"There's no trap there!");
	return 0;
    }
  trap_disarm(op,tmp,risk);
  return 1;
	
}

int trap_see(object *op,object *trap) {
  char buf[MAX_BUF];
  int chance;

  chance = random_roll(0, 99, op, PREFER_HIGH);;
  
  /*  decide if we see the rune or not */
  if((trap->stats.Cha==1) || (chance >
        MIN(95,MAX(5,((int)((float) (op->map->difficulty 
	+ trap->level + trap->stats.Cha-op->level)/10.0 * 50.0))))))
  {
      sprintf(buf,"You spot a %s!",trap->name);
      new_draw_info(NDI_UNIQUE, 0,op,buf);
      return 1;
  }
  return 0;
}

int trap_show(object *trap, object *where) {
    object *tmp2;

    if(where==NULL) return 0;
    tmp2=get_archetype("runedet");
    tmp2->face=&new_faces[GET_ANIMATION(trap, 0)];
    tmp2->x=where->x;tmp2->y=where->y;tmp2->map=where->map;
    insert_ob_in_map(tmp2,where->map,NULL,0);
    return 1;

}

#ifndef sqr
#define sqr(x) ((x)*(x))
#endif
int trap_disarm(object *disarmer, object *trap, int risk) {

  int trapworth;  /* need to compute the experience worth of the trap
                     before we kill it */
  int disarmer_level = SK_level (disarmer);

  /* this formula awards a more reasonable amount of exp */
  trapworth =  MAX(1,trap->level)  * disarmer->map->difficulty *
	sqr(MAX(trap->stats.dam,spells[trap->stats.sp].sp)) /
	disarmer_level;

    if(!(random_roll(0, (MAX(2,
       MIN(20,trap->level-disarmer_level
	   +5 - disarmer->stats.Dex/2))-1), disarmer, PREFER_LOW)))
        {
            new_draw_info(NDI_UNIQUE, 0,disarmer,"You successfuly disarm it!");
            remove_ob(trap);
            free_object(trap);
	    /* If it is your own trap, (or any players trap), don't you don't
	     * get exp for it.
	     */
	    if (trap->owner && trap->owner->type!=PLAYER && risk)
		return trapworth;
	    else return 1; /* give minimal exp and say success */
        }
    else
        {
            new_draw_info(NDI_UNIQUE, 0,disarmer,"You fail to disarm the trap.");
	    if(! (random_roll(0, (MAX(2,disarmer_level-trap->level 
	       + disarmer->stats.Dex/2-6))-1, disarmer, PREFER_LOW)) &&risk) {
		new_draw_info(NDI_UNIQUE, 0,disarmer,"In fact, you set it off!");
		spring_trap(trap,disarmer);
	    }
            return 0;
        }
}


/*  traps need to be adjusted for the difficulty of the map.  The
default traps are too strong for wimpy level 1 players, and 
unthreatening to anyone of high level */

void trap_adjust(object *trap, int difficulty)
{ int i;
  /*  first we set the sp value of the trap if it has a spell in it. */
  if(trap->slaying) {
	trap->stats.sp = look_up_spell_name(trap->slaying);
	trap->stats.dam = 0;
  }
  if(trap->stats.sp ==-1) trap->stats.sp = 0;

  /* now we set the trap level to match the difficulty of the level */
  /* the formula below will give a level from 1 to (2*difficulty) with */
  /* a peak probability at difficulty */

  trap->level = MAX(1, rndm(0, difficulty-1) + rndm(0, difficulty-1));

  /* set the hiddenness of the trap, similar formula to above */
  trap->stats.Cha = rndm(0, 19) + rndm(0, difficulty-1) + rndm(0, difficulty-1);

  /* set the damage of the trap if it's not a spellcasting trap 
	we get 0-4 pts of damage per level of difficulty of the map in
        the trap*/

  if(trap->stats.sp == 0) {
	trap->stats.dam = 0;
	for(i=0;i<difficulty;i++) trap->stats.dam+=rndm(0, 4);
  }
    

  /*  the poison trap special case */
  if(trap->attacktype & AT_POISON)
      trap->stats.dam = MAX(1, rndm(0, difficulty-1));  

  /*  so we get an appropriate amnt of exp for AT_DEATH traps */
  if(trap->attacktype & AT_DEATH) trap->stats.dam = 127;

}

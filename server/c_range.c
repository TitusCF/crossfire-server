/*
 * static char *rcsid_c_range_c =
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/* This file deals with range related commands (casting, shooting,
 * throwing, etc.
 */

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <version.h>
#include <spells.h>
#include <skills.h>
#include <newclient.h>
#include <commands.h>


int command_invoke(object *op, char *params)
{
	return command_cast_spell(op, params, 'i');
}

int command_cast(object *op, char *params)
{
	return command_cast_spell(op, params, 'c');
}

int command_prepare(object *op, char *params)
{
	return command_cast_spell(op, params, 'p');
}

/* object *op is the caster, params is the spell name.  We return the index
 * value of the spell in the spells array for a match, -1 if there is no
 * match, -2 if there are multiple matches.  Note that 0 is a valid entry, so
 * we can't use that as failure.
 *
 * Modified 03/24/98 - extra parameter 'options' specifies if the search is
 * done with the length of the input spell name, or the length of the stored
 * spell name.  This allows you to find out if the spell name entered had
 * extra optional parameters at the end (ie: marking rune <text>)
 *
 */
static int find_spell_byname(object *op, char *params, int options)
{
    int numknown; /* number of spells known by op */
    int spnum;  /* number of spell that is being cast */
    int match=-1,i;
    int paramlen=0;

    if(QUERY_FLAG(op, FLAG_WIZ))
	numknown = NROFREALSPELLS;
    else 
	numknown = op->contr->nrofknownspells;

    for(i=0;i<numknown;i++){
	if (QUERY_FLAG(op, FLAG_WIZ)) spnum = i;
	else spnum = op->contr->known_spells[i];

        if (!options)
          paramlen=strlen(params);
          
	if (!strncmp(params, spells[spnum].name, options?strlen(spells[spnum].name):paramlen)) {
	    /* We already found a match previously - thus params is not
	     * not unique, so return -2 stating this.
	     */
	    if (match>=0) return -2;
	    else match=spnum;
	}
    }
    return match;
}


/* Shows all spells that op knows.  If params is supplied, the must match
 * that.  If cleric is 1, show cleric spells, if not set, show mage
 * spells.
 */
static void show_matching_spells(object *op, char *params, int cleric)
{
    int i,spnum,first_match=0;
    char lev[80], cost[80];

    for (i=0; i<(QUERY_FLAG(op, FLAG_WIZ)?NROFREALSPELLS:op->contr->nrofknownspells); i++) {	
	if (QUERY_FLAG(op,FLAG_WIZ)) spnum=i;
	else spnum = op->contr->known_spells[i];

	if (spells[spnum].cleric != cleric) continue;
	if (params && strncmp(spells[spnum].name,params, strlen(params)))
		continue;
	if (!first_match) {
	    first_match=1;
	    if (!cleric)
		new_draw_info(NDI_UNIQUE, 0, op, "Mage spells");
	    else
		new_draw_info(NDI_UNIQUE, 0, op, "Priest spells");
	    new_draw_info(NDI_UNIQUE, 0,op,"[ sp] [lev] spell name");
	}
	if (spells[spnum].path & op->path_denied) {
	    strcpy(lev,"den");
            strcpy(cost,"den");
	} else {
	    sprintf(lev,"%3d",spells[spnum].level);
            sprintf(cost,"%3d",SP_level_spellpoint_cost(op,op,spnum));
        }

	new_draw_info_format(NDI_UNIQUE,0,op,"[%s] [%s] %s",
		cost, lev, spells[spnum].name);
    }
}



/* sets up to cast a spell.  op is the caster, params is the spell name,
 * and command is the first letter of the spell type (c=cast, i=invoke, 
 * p=prepare).  Invoke casts a spell immediately, where as cast (and I believe
 * prepare) just set up the range type.
 */

int command_cast_spell (object *op, char *params, char command)
{
    int castnow=0;
    char *cp=NULL;
    int spnum=-1, spnum2=-1;  /* number of spell that is being cast */

    if(!op->contr->nrofknownspells&&!QUERY_FLAG(op, FLAG_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,op,"You don't know any spells.");
        return 1;
    }
    /* Remove control of the golem */
    if(op->contr->golem!=NULL) {
        remove_friendly_object(op->contr->golem);
        remove_ob(op->contr->golem);
        free_object(op->contr->golem);
        op->contr->golem=NULL;
    }

    if (command=='i') castnow = 1;
    if(params!=NULL) {

#if 0
	/* rune of fire, rune of ... are special cases, break it into 'rune' and
	 * then put 'fire, marking, whatever' in cp. */
	if (strncmp(params,"rune",4)) {
	    cp =strstr(params, " of ");
	    if (cp) {
		*cp='\0';
		cp +=4;
	    }
	}
#endif
        /* This replaces the above.  It assumes simply that if the name of
         * the spell being cast as input by the player is shorter than or
         * equal to the length of the spell name, then there is no options
         * but if it is longer, then everything after the spell name is
         * an option.  It determines if the spell name is shorter or
         * longer by first iterating through the actual spell names, checking
         * to the length of the typed in name.  If that fails, then it checks
         * to the length of each spell name.  If that passes, it assumes that
         * anything after the length of the actual spell name is extra options
         * typed in by the player (ie: marking rune Hello there) */
	if ( ((spnum2 = spnum = find_spell_byname(op, params, 0)) < 0) && 
	    ((spnum = find_spell_byname(op, params, 1)) >= 0) ) {
          params[strlen(spells[spnum].name)] = '\0';
          cp = &params[strlen(spells[spnum].name)+1];
          if (strncmp(cp,"of ",3) == 0)
            cp += 3;
        }	         

	if (spnum>=0) {
	    rangetype orig_rangetype=op->contr->shoottype;
	    op->contr->shoottype=range_magic;
	    if(op->type==PLAYER) { 
		 /* if we don't change to the correct spell numb,
		  * check_skill_to_fire will be confused as to which
		  * spell casting skill to ready for the player!
		  * I set the code to change back to the old spellnum
		  * after we check, but is this really needed?? -b.t. */

		int orig_spn = op->contr->chosen_spell;
		op->contr->chosen_spell=spnum;
		if(!check_skill_to_fire(op)) {  
		    op->contr->shoottype=orig_rangetype;
		    return 0; 
		    }
		op->contr->chosen_spell=orig_spn;
	    }

	    if (castnow) { 
		int value;

		    /* Need to switch shoottype to range_magic - otherwise 
                     * cast_spell doesn't check to see if the character 
                     * has enough spellpoints. 
		     * Note: now done above this -b.t. */
		    /* op->contr->shoottype=range_magic; */ 

		value = cast_spell(op,op,op->facing,spnum,0,spellNormal,cp);
		op->contr->shoottype=orig_rangetype;

		if(spells[spnum].cleric) 
			op->stats.grace -= value;
		else 
			op->stats.sp -= value;
	    } 
	    /* We are not casting now */
	    else op->contr->chosen_spell=spnum; 
	    
	    return 1;
	} /* found a matching spell */
    } /* params supplied */

    /* We get here if cast was given without options or we could not find
     * the requested spell.  List all the spells the player knows (if
     * spnum = -1) or spells matching params if spnum=-2
     */

    new_draw_info(NDI_UNIQUE, 0,op,"Cast what spell?  Choose one of:");
    show_matching_spells(op, (spnum2==-2)?params:NULL, 0);
    new_draw_info(NDI_UNIQUE,0,op,"");
    show_matching_spells(op, (spnum2==-2)?params:NULL, 1);

    return 1;
}

/**************************************************************************/

/* Returns TRUE if the range specified (int r) is legal - that is,
 * the character has an item that is equipped for that range type.
 * return 0 if there is no item of that range type that is usable.
 */

int legal_range(object *op,int r) {
    int i;

    switch(r) {
	case range_none: /* "Nothing" is always legal */
	    return 1;
	case range_bow:
	case range_misc:
	    if (op->contr->ranges[r]) return 1;
	    else return 0;

	case range_magic: /* cast spells */
	    if (op->contr->nrofknownspells == 0)
		return 0;
	    for (i = 0; i < op->contr->nrofknownspells; i++)
		if (op->contr->known_spells[i] == op->contr->chosen_spell)
		    return 1;
	    op->contr->chosen_spell = op->contr->known_spells[0];
	    return 1;

	case range_golem: /* Use scrolls */
	    if (op->contr->golem) return 1;
	    return 0;

	case range_skill:
	    if (op->chosen_skill)
		return 1;
	    else
		return 0;
    }
    /* No match above, must not be valid */
    return 0;
}

void change_spell(object *op,char k) {
    /* Try disabling this so that the player can switch ranges and
     * still keep his golems.  There seems to be some ways that
     * this currently happens but directly switching to another skill
     * by equipping items or the like.
     */
#if 0
    if(op->contr->golem!=NULL) {
	remove_friendly_object(op->contr->golem);
	remove_ob(op->contr->golem);
	free_object(op->contr->golem);
	op->contr->golem=NULL;
    }
#endif

    do {
	op->contr->shoottype += ((k == '+') ? 1 : -1);
	if(op->contr->shoottype >= range_size)
	    op->contr->shoottype = range_none;
	else if (op->contr->shoottype <= range_bottom)
	    op->contr->shoottype = (rangetype)(range_size-1);
    } while (!legal_range(op,op->contr->shoottype));

    /* Legal range has already checked that we have an appropriate item
     * that uses the slot, so we don't need to be too careful about 
     * checking the status of the object.
     */
    switch(op->contr->shoottype) {
	case range_none:
	    new_draw_info(NDI_UNIQUE, 0,op, "No ranged attack chosen.");
	    break;

	case range_golem:
	    new_draw_info(NDI_UNIQUE, 0,op, "You regain control of your golem.");
	    break;

	case range_bow:
	    new_draw_info_format(NDI_UNIQUE, 0,op, "Switched to %s and %s.", query_name(op->contr->ranges[range_bow]),
		     op->contr->ranges[range_bow]->race ? op->contr->ranges[range_bow]->race : "nothing");
	    break;

	case range_magic:
	    new_draw_info_format(NDI_UNIQUE, 0,op,"Switched to spells (%s).",
		    spells[op->contr->chosen_spell].name);
	    break;

	case range_misc:
	    new_draw_info_format(NDI_UNIQUE, 0,op, "Switched to %s.", query_base_name(op->contr->ranges[range_misc], 0));
	    break;

	case range_skill: 
	    new_draw_info_format(NDI_UNIQUE, 0,op, "Switched to skill: %s", op->chosen_skill ?  
		 op->chosen_skill->name : "none");
	    break;

	default:
	    break;
    }
}


int command_rotateshoottype (object *op, char *params)
{
  if (!params)
      change_spell(op,'+');
  else
    change_spell(op, params[0]);
  return 0;
}

int command_throw (object *op, char *params)
{

   if(!change_skill(op,SK_THROWING))
        return 0;
   else {
        int success = do_skill(op,op->facing,params);
        return success;
   }
}

int command_rotatespells (object *op, char *params)
{
  player *pl=op->contr;
  int i, j;

  if(pl->shoottype != range_magic) {
    if(pl->nrofknownspells > 0) {
      pl->shoottype = range_magic;
      pl->chosen_spell = pl->known_spells[0];
    } else
          new_draw_info(NDI_UNIQUE, 0,op,"You know no spells.");
    return 0;
  }

  for(i=0;i<pl->nrofknownspells;i++)
    if(pl->known_spells[i]==pl->chosen_spell)
	    {
	j =1;
	if(params)
	  sscanf(params, "%d", &j);
	i +=j + (int)pl->nrofknownspells;
	i = i % (int)pl->nrofknownspells;
	pl->chosen_spell=pl->known_spells[i];
	return 1;
	    }
  pl->chosen_spell=pl->known_spells[0];
  return 1;
}


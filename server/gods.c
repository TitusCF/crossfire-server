/*
 * static char *rcsid_gods_c =
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
*/


/* Oct 3, 1995 - Code laid down for initial gods, priest alignment, and
 * monster race initialization. b.t.
 */

/* Sept 1996 - moved code over to object -oriented gods -b.t. */

#include <global.h>
#include <living.h>
#include <object.h>
#include <spells.h>
#include <sounds.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

static int worship_forbids_use(object *op, object *exp_obj, uint32 flag, const char *string);
static void stop_using_item(object *op, int type, int number);
static void update_priest_flag (object *god, object *exp_ob, uint32 flag);
static void god_intervention(object *op, object *god, object *skill);
static int god_examines_priest (object *op, object *god);
static int god_examines_item(object *god, object *item);
static const char *get_god_for_race(const char *race);

/**
 * Returns the id of specified god.
 */
static int lookup_god_by_name(const char *name) {
    int godnr=-1;
    size_t nmlen = strlen(name);
 
    if(name && strcmp(name,"none")) { 
	godlink *gl;
	for(gl=first_god;gl;gl=gl->next)
	    if(!strncmp(name,gl->name,MIN(strlen(gl->name),nmlen)))
		break;
	if(gl) godnr=gl->id;
    }
    return godnr;
}

/**
 * Returns pointer to specified god's object through pntr_to_god_obj..
 */
object *find_god(const char *name) {
    object *god=NULL;

    if(name) { 
	godlink *gl;

	for(gl=first_god;gl;gl=gl->next)
	    if(!strcmp(name,gl->name)) break;
	if(gl) god=pntr_to_god_obj(gl);
    }
    return god;
}

/**
 * Determines if op worships a god.
 * Returns the godname if they do or "none" if they have no god.
 * In the case of an NPC, if they have no god, we try and guess 
 * who they should worship based on their race. If that fails we 
 * give them a random one.
 */

const char *determine_god(object *op) {
    int godnr = -1;
    const char *godname;

    /* spells */
    if ((op->type == SPELL || op->type == SPELL_EFFECT) &&
          op->title)
    {
	if (lookup_god_by_name(op->title)>=0) return op->title;
    }

    if(op->type!= PLAYER && QUERY_FLAG(op,FLAG_ALIVE)) {
	
	/* find a god based on race */
	if(!op->title) {
	    if (op->race !=NULL) {
		godname=get_god_for_race(op->race);
		if (godname!=NULL) {
		    op->title = add_string(godname);
		}
	    }
	}

	/* find a random god */
	if(!op->title) {
	    godlink *gl = first_god;

	    godnr = rndm(1, gl->id);
	    while(gl) {
		if(gl->id == godnr) break;
		gl=gl->next;
	    }
	    op->title = add_string(gl->name);
	}

	return op->title;
    }


    /* The god the player worships is in the praying skill (native skill
     * not skill tool).  Since a player can only have one instance of
     * that skill, once we find it, we can return, either with the
     * title or "none".
     */
    if(op->type==PLAYER) {
	object *tmp;
        for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
	    if (tmp->type == SKILL && tmp->subtype == SK_PRAYING) {
		if (tmp->title) return (tmp->title);
		else return("none");
	    }
    }
    return ("none");
}

/**
 * Returns 1 if s1 and s2 are the same - either both NULL, or strcmp( ) == 0
 */
static int same_string (const char *s1, const char *s2)
{
    if (s1 == NULL)
        if (s2 == NULL)
            return 1;
        else
            return 0;
    else
        if (s2 == NULL)
            return 0;
        else
            return strcmp (s1, s2) == 0;
}


/**
 * Checks for any occurrence of the given 'item' in the inventory of 'op' (recursively).
 * Any matching items in the inventory are deleted, and a
 * message is displayed to the player.
 */
static void follower_remove_similar_item (object *op, object *item)
{
    object *tmp, *next;
    
    if (op && op->type == PLAYER && op->contr) {
        /* search the inventory */
        for (tmp = op->inv; tmp != NULL; tmp = next) {
	    next = tmp->below;   /* backup in case we remove tmp */
        
	    if (tmp->type == item->type
		&& same_string (tmp->name, item->name)
		&& same_string (tmp->title, item->title)
		&& same_string (tmp->msg, item->msg)
		&& same_string (tmp->slaying, item->slaying)) {
	        
	        /* message */
		if (tmp->nrof > 1)
		    draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
					 "The %s crumble to dust!",
					 "The %s crumble to dust!",
					 query_short_name(tmp));
		else
		    draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_REMOVE,
					 "The %s crumbles to dust!",
					 "The %s crumbles to dust!",
					 query_short_name(tmp));
	        
	        remove_ob(tmp);    /* remove obj from players inv. */
		esrv_del_item(op->contr, tmp->count); /* notify client */
		free_object(tmp);  /* free object */
	    }
	    if (tmp->inv)
	      follower_remove_similar_item(tmp, item);
	}
    }
}

/**
 * Checks for any occurrence of the given 'item' in the inventory of 'op' (recursively).
 * Returns 1 if found, else 0.
 */
static int follower_has_similar_item (object *op, object *item)
{
    object *tmp;

    for (tmp = op->inv; tmp != NULL; tmp = tmp->below) {
        if (tmp->type == item->type
            && same_string (tmp->name, item->name)
            && same_string (tmp->title, item->title)
            && same_string (tmp->msg, item->msg)
            && same_string (tmp->slaying, item->slaying))
            return 1;
        if (tmp->inv && follower_has_similar_item (tmp, item))
            return 1;
    }
    return 0;
}

/**
 * God gives an item to the player.
 */
static int god_gives_present (object *op, object *god, treasure *tr)
{
    object *tmp;

    if (follower_has_similar_item (op, &tr->item->clone))
        return 0;

    tmp = arch_to_object (tr->item);
    draw_ext_info_format (NDI_UNIQUE, 0, op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_ADD,
			  "%s lets %s appear in your hands.",
			  god->name, query_short_name (tmp),
			  god->name, query_short_name (tmp));
    tmp = insert_ob_in_ob (tmp, op);
    if (op->type == PLAYER)
        esrv_send_item (op, tmp);
    return 1;
}

/**
 * Player prays at altar.
 * Checks for god changing, divine intervention, and so on.
 */
void pray_at_altar(object *pl, object *altar, object *skill) {
    object *pl_god=find_god(determine_god(pl));

    /* Lauwenmark: Handle for plugin altar-parying (apply) event */
    if (execute_event(altar, EVENT_APPLY,pl,NULL,NULL,SCRIPT_FIX_ALL)!=0)
        return;

    /* If non consecrate altar, don't do anything */
    if (!altar->other_arch) return;

    /* hmm. what happend depends on pl's current god, level, etc */
    if(!pl_god) {  /*new convert */
	become_follower(pl,&altar->other_arch->clone);
	return;

    } else if(!strcmp(pl_god->name,altar->other_arch->clone.name)) {
        /* pray at your gods altar */
	int bonus = (pl->stats.Wis+skill->level)/10;

	/* we can get neg grace up faster */
	if(pl->stats.grace<0) pl->stats.grace+=(bonus>-1*(pl->stats.grace/10) ?
			bonus : -1*(pl->stats.grace/10));
	/* we can super-charge grace to 2x max */
	if(pl->stats.grace<(2*pl->stats.maxgrace)) {
	    pl->stats.grace+=bonus/2;
	}
	if(pl->stats.grace>(2*pl->stats.maxgrace)) {
	    pl->stats.grace=(2*pl->stats.maxgrace);
	}

	/* Every once in a while, the god decides to checkup on their
	 * follower, and may intervene to help them out.
	 */
	bonus = MAX(1, bonus + MAX(pl->stats.luck, -3)); /* -- DAMN -- */

	if(((random_roll(0, 399, pl, PREFER_LOW))-bonus)<0)
	  god_intervention(pl,pl_god, skill);

    } else { /* praying to another god! */
	uint64 loss = 0;
	int angry=1;

	/* I believe the logic for detecting opposing gods was completely
	 * broken - I think it should work now.  altar->other_arch
	 * points to the god of this altar (which we have
	 * already verified is non null).  pl_god->other_arch
	 * is the opposing god - we need to verify that exists before
	 * using its values.
	 */
        if(pl_god->other_arch &&
           (altar->other_arch->name==pl_god->other_arch->name)) {
	    angry=2;
	    if(random_roll(0, skill->level+2, pl, PREFER_LOW)-5 > 0) {
		object *tmp;

		/* you really screwed up */
		angry=3;
		draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,pl, 
				     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
				     "Foul Priest! %s punishes you!",
				     "Foul Priest! %s punishes you!",
				     pl_god->name);
		tmp=create_archetype(LOOSE_MANA);
		cast_magic_storm(pl,tmp, pl_god->level+20);
	    } else 
		draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,pl,
				     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
				     "Foolish heretic! %s is livid!",
				     "Foolish heretic! %s is livid!",
				     pl_god->name);
	} else 
	    draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,pl,
				 MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
                                "Heretic! %s is angered!",
                                "Heretic! %s is angered!",
				 pl_god->name);
 
	/* whether we will be successfull in defecting or not -
	 * we lose experience from the clerical experience obj 
	 */

	loss = angry * (skill->stats.exp / 10);
	if(loss)
	    change_exp(pl, -random_roll64(0, loss, pl, PREFER_LOW),
		   skill?skill->skill:"none", SK_SUBTRACT_SKILL_EXP);

	/* May switch Gods, but its random chance based on our current level
	 * note it gets harder to swap gods the higher we get 
	 */
	if((angry==1) && !(random_roll(0, skill->level, pl, PREFER_LOW))) {
	    become_follower(pl,&altar->other_arch->clone);
	} else {
	    /* toss this player off the altar.  He can try again. */
	    draw_ext_info(NDI_UNIQUE|NDI_NAVY,0,pl,
			  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			  "A divine force pushes you off the altar.", NULL);

	    move_player(pl,absdir(pl->facing + 4)); /* back him off the way he came. */
	} 
    }
}

/**
 * Removes special prayers given by a god.
 */
static void check_special_prayers (object *op, object *god)
{
    /* Ensure that 'op' doesn't know any special prayers that are not granted
     * by 'god'.
     */
    treasure *tr;
    object *tmp, *next_tmp;
    int remove=0;

    /* Outer loop iterates over all special prayer marks */
    for (tmp = op->inv; tmp; tmp = next_tmp) {
        next_tmp = tmp->below;

	/* we mark special prayers with the STARTEQUIP flag, so if it isn't
	 * in that category, not something we need to worry about.
	 */
        if (tmp->type != SPELL || !QUERY_FLAG(tmp, FLAG_STARTEQUIP))
            continue;

        if (god->randomitems == NULL) {
            LOG (llevError, "BUG: check_special_prayers(): god %s without randomitems\n", god->name);
            do_forget_spell (op, tmp->name);
            continue;
	}

        /* Inner loop tries to find the special prayer in the god's treasure
         * list. We default that the spell should be removed.
	 */
	remove=1;
        for (tr = god->randomitems->items; tr; tr = tr->next)
        {
            object *item;

            if (tr->item == NULL)
                continue;
            item = &tr->item->clone;

	    /* Basically, see if the matching spell is granted by this god. */

            if (tr->item->clone.type == SPELL && tr->item->clone.name == tmp->name) {
		remove=0;
		break;
	    }
	}
	if (remove) {
	    /* just do the work of removing the spell ourselves - we already
	     * know that the player knows the spell
	     */
	    draw_ext_info_format (NDI_UNIQUE|NDI_NAVY, 0, op,
				  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
				  "You lose knowledge of %s.",
				  "You lose knowledge of %s.",
				  tmp->name);
	    player_unready_range_ob(op->contr, tmp);
	    remove_ob(tmp);
	    free_object(tmp);
	}

    }
}

/**
 * This function is called whenever a player has
 * switched to a new god. It handles basically all the stat changes
 * that happen to the player, including the removal of godgiven
 * items (from the former cult).
 */
void become_follower (object *op, object *new_god) {
    object *old_god = NULL;                      /* old god */
    treasure *tr;
    object *item, *skop, *next;
    int i,sk_applied;
    int undeadified = 0; /* Turns to true if changing god can changes the undead
                          * status of the player.*/
    old_god = find_god(determine_god(op));
    
    /* take away any special god-characteristic items. */
    for(item=op->inv;item!=NULL;item=next) {
	next = item->below;
	/* remove all invisible startequip items which are
	 *  not skill, exp or force 
 	 */
        if(QUERY_FLAG(item,FLAG_STARTEQUIP) && item->invisible &&
	   (item->type != SKILL) && (item->type != EXPERIENCE) &&
	   (item->type != FORCE)) {

	    if (item->type == SPELL)
		draw_ext_info_format(NDI_UNIQUE|NDI_NAVY, 0, op,
				     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
				     "You lose knowledge of %s.",
				     "You lose knowledge of %s.",
				     item->name);
	    player_unready_range_ob(op->contr, item);
	    remove_ob(item);
	    free_object(item);
	}
    }
    
    /* remove any godgiven items from the old god */
    if (old_god) {
        for(tr=old_god->randomitems->items; tr!=NULL; tr = tr->next) {
	    if (tr->item && QUERY_FLAG(&tr->item->clone, FLAG_STARTEQUIP))
	        follower_remove_similar_item(op, &tr->item->clone);
	}
    }
    
    if(!op||!new_god) return;

    if(op->race && new_god->slaying && strstr(op->race,new_god->slaying)) { 
	draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,op,
			     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			     "Fool! %s detests your kind!",
			     "Fool! %s detests your kind!",
			     new_god->name);
        if(random_roll(0, op->level-1, op, PREFER_LOW)-5>0) {
	    object *tmp = create_archetype(LOOSE_MANA);
	    cast_magic_storm(op,tmp, new_god->level+10);
	}
	return;
    }


    /* give the player any special god-characteristic-items. */
    for(tr=new_god->randomitems->items; tr!=NULL; tr = tr->next) {
      if(tr->item && tr->item->clone.invisible && tr->item->clone.type != SPELLBOOK &&
         tr->item->clone.type != BOOK && tr->item->clone.type != SPELL)
        god_gives_present(op,new_god,tr); }


    draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,op,
			 MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			 "You become a follower of %s!",
			 "You become a follower of %s!",
			 new_god->name);

    for (skop = op->inv; skop != NULL; skop=skop->below)
	if (skop->type == SKILL && skop->subtype == SK_PRAYING) break;

    /* Player has no skill - give them the skill */
    if (!skop) {
	/* The arhetype should always be defined - if we crash here because it doesn't,
	 * things are really messed up anyways.
	 */
	skop = give_skill_by_name(op, get_archetype_by_type_subtype(SKILL, SK_PRAYING)->clone.skill);
	link_player_skills(op);
    }

    sk_applied=QUERY_FLAG(skop,FLAG_APPLIED); /* save skill status */

    /* Clear the "undead" status. We also need to force a call to change_abil,
     * so I set undeadified for that.
     * - gros, 21th July 2006.
     */
    if ((old_god)&&(QUERY_FLAG(old_god,FLAG_UNDEAD)))
    {
        CLEAR_FLAG(skop,FLAG_UNDEAD);
        undeadified=1;
    }

    if(skop->title) { /* get rid of old god */ 
	draw_ext_info_format(NDI_UNIQUE,0,op, 
			     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			     "%s's blessing is withdrawn from you.",
			     "%s's blessing is withdrawn from you.",
			     skop->title);

	/* The point of this is to really show what abilities the player just lost */
	if (sk_applied||undeadified) {
	    CLEAR_FLAG(skop,FLAG_APPLIED); 
	    (void) change_abil(op,skop);
	}
       free_string(skop->title);
    }

    /* now change to the new gods attributes to exp_obj */
    skop->title = add_string(new_god->name);
    skop->path_attuned=new_god->path_attuned;
    skop->path_repelled=new_god->path_repelled;
    skop->path_denied=new_god->path_denied;
    /* copy god's resistances */
    memcpy(skop->resist, new_god->resist, sizeof(new_god->resist));

    /* make sure that certain immunities do NOT get passed
     * to the follower! 
     */
    for (i=0; i<NROFATTACKS; i++)
      if (skop->resist[i] > 30 && (i==ATNR_FIRE || i==ATNR_COLD ||
	  i==ATNR_ELECTRICITY || i==ATNR_POISON))
	skop->resist[i] = 30;

    skop->stats.hp= (sint16) new_god->last_heal;
    skop->stats.sp= (sint16) new_god->last_sp;
    skop->stats.grace= (sint16) new_god->last_grace;
    skop->stats.food= (sint16) new_god->last_eat;
    skop->stats.luck= (sint8) new_god->stats.luck;
    /* gods may pass on certain flag properties */
    update_priest_flag(new_god,skop,FLAG_SEE_IN_DARK);
    update_priest_flag(new_god,skop,FLAG_REFL_SPELL);
    update_priest_flag(new_god,skop,FLAG_REFL_MISSILE);
    update_priest_flag(new_god,skop,FLAG_STEALTH);
    update_priest_flag(new_god,skop,FLAG_MAKE_INVIS);
    update_priest_flag(new_god,skop,FLAG_UNDEAD);
    update_priest_flag(new_god,skop,FLAG_BLIND);
    update_priest_flag(new_god,skop,FLAG_XRAYS); /* better have this if blind! */

    draw_ext_info_format(NDI_UNIQUE,0,op,
			 MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			 "You are bathed in %s's aura.",
			 "You are bathed in %s's aura.",
			 new_god->name);

    /* Weapon/armour use are special...handle flag toggles here as this can
     * only happen when gods are worshipped and if the new priest could
     * have used armour/weapons in the first place.
     *
     * This also can happen for monks which cannot use weapons. In this case
     * do not allow to use weapons even if the god otherwise would allow it.
     */
    if (!present_in_ob_by_name(FORCE, "no weapon force", op))
	update_priest_flag(new_god,skop,FLAG_USE_WEAPON);
    update_priest_flag(new_god,skop,FLAG_USE_ARMOUR);

    if(worship_forbids_use(op,skop,FLAG_USE_WEAPON,"weapons"))
	stop_using_item(op,WEAPON,2);

    if(worship_forbids_use(op,skop,FLAG_USE_ARMOUR,"armour")) {
	stop_using_item(op,ARMOUR,1);
	stop_using_item(op,HELMET,1);
	stop_using_item(op,BOOTS,1);
	stop_using_item(op,GLOVES,1);
	stop_using_item(op,SHIELD,1);
    }

    SET_FLAG(skop,FLAG_APPLIED);
    (void) change_abil(op,skop);

    /* return to previous skill status */
    if (!sk_applied) CLEAR_FLAG(skop,FLAG_APPLIED);

    check_special_prayers (op, new_god);
}

/**
 * Forbids or let player use something item type.
 * op is the player.
 * exp_obj is the widsom experience.
 * flag is the flag to check against.
 * string is the string to print out.
 */

static int worship_forbids_use (object *op, object *exp_obj, uint32 flag, const char *string) {

    if(QUERY_FLAG(&op->arch->clone,flag)) {
	if(QUERY_FLAG(op,flag)!=QUERY_FLAG(exp_obj,flag)) {
	    update_priest_flag(exp_obj,op,flag);
	    if(QUERY_FLAG(op,flag))
		draw_ext_info_format(NDI_UNIQUE,0,op,
				     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
				     "You may use %s again.",
				     "You may use %s again.",
				     string);
	    else {
		draw_ext_info_format(NDI_UNIQUE,0,op,
				     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
				     "You are forbidden to use %s.",
				     "You are forbidden to use %s.",
				     string);
		return 1;
	    }
	}
    }
    return 0;
}

/**
 * Unapplies up to number worth of items of type
 */
static void stop_using_item ( object *op, int type, int number ) {
  object *tmp;

  for(tmp=op->inv;tmp&&number;tmp=tmp->below)
    if(tmp->type==type&&QUERY_FLAG(tmp,FLAG_APPLIED)) {
        apply_special (op, tmp, AP_UNAPPLY | AP_IGNORE_CURSE);
	number--;
    }
}

/**
 * If the god does/doesnt have this flag, we
 * give/remove it from the experience object if it doesnt/does
 * already exist. For players only!
 */

static void update_priest_flag (object *god, object *exp_ob, uint32 flag) {
      if(QUERY_FLAG(god,flag)&&!QUERY_FLAG(exp_ob,flag))
          SET_FLAG(exp_ob,flag);
      else if(QUERY_FLAG(exp_ob,flag)&&!QUERY_FLAG(god,flag))
      {
	/*  When this is called with the exp_ob set to the player,
	 * this check is broken, because most all players arch
	 * allow use of weapons.  I'm not actually sure why this
	 * check is here - I guess if you had a case where the
	 * value in the archetype (wisdom) should over ride the restrictions
	 * the god places on it, this may make sense.  But I don't think
	 * there is any case like that.
	 */
	 
/*        if (!(QUERY_FLAG(&(exp_ob->arch->clone),flag)))*/
	CLEAR_FLAG(exp_ob,flag);
      };
}



archetype *determine_holy_arch (object *god, const char *type)
{
    treasure *tr;

    if ( ! god || ! god->randomitems) {
        LOG (llevError, "BUG: determine_holy_arch(): no god or god without "
             "randomitems\n");
        return NULL;
    }

    for (tr = god->randomitems->items; tr != NULL; tr = tr->next) {
        object *item;

        if ( ! tr->item)
            continue;
        item = &tr->item->clone;

        if (item->type == BOOK && item->invisible
            && strcmp (item->name, type) == 0)
            return item->other_arch;
    }
    return NULL;
}

/**
 * God helps player by removing curse and/or damnation.
 */
static int god_removes_curse (object *op, int remove_damnation)
{
    object *tmp;
    int success = 0;

    for (tmp = op->inv; tmp; tmp = tmp->below) {
	if (tmp->invisible)
	    continue;
        if (QUERY_FLAG (tmp, FLAG_DAMNED) && ! remove_damnation)
            continue;
        if (QUERY_FLAG (tmp, FLAG_CURSED) || QUERY_FLAG (tmp, FLAG_DAMNED)) {
            success = 1;
            CLEAR_FLAG (tmp, FLAG_DAMNED);
            CLEAR_FLAG (tmp, FLAG_CURSED);
            CLEAR_FLAG (tmp, FLAG_KNOWN_CURSED);
            if (op->type == PLAYER)
                esrv_send_item (op, tmp);
        }
    }

    if (success)
        draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
                "You feel like someone is helping you.", NULL);
    return success;
}

static int follower_level_to_enchantments (int level, int difficulty)
{
    if (difficulty < 1) {
        LOG (llevError, "follower_level_to_enchantments(): "
             "difficulty %d is invalid\n", difficulty);
        return 0;
    }

    if (level <= 20)
        return level / difficulty;
    if (level <= 40)
        return (20 + (level - 20) / 2) / difficulty;
    return (30 + (level - 40) / 4) / difficulty;
}

/**
 * God wants to enchant weapon.
 * Affected weapon is the applied one (weapon or bow). It's checked to make sure
 * it isn't a weapon for another god. If all is all right, update weapon with
 * attacktype, slaying and such.
 */
static int god_enchants_weapon (object *op, object *god, object *tr, object *skill)
{
    char buf[MAX_BUF];
    object *weapon;
    uint32 attacktype;
    int tmp;

    for (weapon = op->inv; weapon; weapon = weapon->below)
        if ((weapon->type == WEAPON || weapon->type == BOW)
	    && QUERY_FLAG (weapon, FLAG_APPLIED))
            break;
    if (weapon == NULL || god_examines_item (god, weapon) <= 0)
        return 0;

    /* First give it a title, so other gods won't touch it */
    if ( ! weapon->title) {
        sprintf (buf, "of %s", god->name);
        weapon->title = add_string (buf);
        if (op->type == PLAYER) 
	    esrv_update_item (UPD_NAME, op, weapon);
        draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_CHANGE,
		       "Your weapon quivers as if struck!", NULL);
    }

    /* Allow the weapon to slay enemies */
    if ( ! weapon->slaying && god->slaying) {
        weapon->slaying = add_string (god->slaying);
        draw_ext_info_format (NDI_UNIQUE, 0, op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_CHANGE,
                "Your %s now hungers to slay enemies of your god!",
                "Your %s now hungers to slay enemies of your god!",
                weapon->name);
        return 1;
    }

    /* Add the gods attacktype */
    attacktype = (weapon->attacktype == 0) ? AT_PHYSICAL : weapon->attacktype;
    if ((attacktype & god->attacktype) != god->attacktype) {
        draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_CHANGE,
		       "Your weapon suddenly glows!", NULL);
        weapon->attacktype = attacktype | god->attacktype;
        return 1;
    }

    /* Higher magic value */
    tmp = follower_level_to_enchantments (skill->level, tr->level);
    if (weapon->magic < tmp) {
        draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_CHANGE,
                "A phosphorescent glow envelops your weapon!", NULL);
        weapon->magic++;
        if (op->type == PLAYER)
            esrv_update_item (UPD_NAME, op, weapon);
        return 1;
    }

    return 0;
}


/**
 * Every once in a while the god will intervene to help the worshiper.
 * Later, this fctn can be used to supply quests, etc for the 
 * priest. -b.t. 
 * called from pray_at_altar() currently. 
 */

static void god_intervention (object *op, object *god, object *skill)
{
    treasure *tr;

    if ( ! god || ! god->randomitems) {
        LOG (llevError, 
	     "BUG: god_intervention(): no god or god without randomitems\n");
        return;
    }

    check_special_prayers (op, god);

    /* lets do some checks of whether we are kosher with our god */
    if (god_examines_priest (op, god) < 0)
        return;

    draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
		   "You feel a holy presence!", NULL);

    for (tr = god->randomitems->items; tr != NULL; tr = tr->next) {
        object *item;

        if (tr->chance <= random_roll(0, 99, op, PREFER_HIGH))
            continue;

        /* Treasurelist - generate some treasure for the follower */
        if (tr->name) {
            treasurelist *tl = find_treasurelist (tr->name);
            if (tl == NULL)
                continue;

            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_ITEM, MSG_TYPE_ITEM_ADD,
		   "Something appears before your eyes.  You catch it before it falls to the ground.",
		   NULL);

            create_treasure (tl, op, GT_STARTEQUIP | GT_ONLY_GOOD
                                      | GT_UPDATE_INV, skill->level, 0);
            return;
        }

        if ( ! tr->item) {
            LOG (llevError, "BUG: empty entry in %s's treasure list\n",
                 god->name);
            continue;
        }
        item = &tr->item->clone;

        /* Grace limit */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "grace limit") == 0) {
            if (op->stats.grace < item->stats.grace
                || op->stats.grace < op->stats.maxgrace) {
		object *tmp;

                /* Follower lacks the required grace for the following
                 * treasure list items. */

		        tmp = create_archetype(HOLY_POSSESSION);
                cast_change_ability(op, op, tmp, 0, 1);
		        free_object(tmp);
                return;
            }
            continue;
        }

        /* Restore grace */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "restore grace") == 0)
        {
            if (op->stats.grace >= 0)
                continue;
            op->stats.grace = random_roll(0, 9, op, PREFER_HIGH);
            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
                    "You are returned to a state of grace.", NULL);
            return;
        }

        /* Heal damage */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "restore hitpoints") == 0)
        {
            if (op->stats.hp >= op->stats.maxhp)
                continue;
            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
                    "A white light surrounds and heals you!", NULL);
            op->stats.hp = op->stats.maxhp;
            return;
        }

        /* Restore spellpoints */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "restore spellpoints") == 0)
        {
            int max = op->stats.maxsp * (item->stats.maxsp / 100.0);
            /* Restore to 50 .. 100%, if sp < 50% */
            int new_sp = random_roll(1000, 1999, op, PREFER_HIGH) / 2000.0 * max;
            if (op->stats.sp >= max / 2)
                continue;
            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
			   "A blue lightning strikes your head but doesn't hurt you!", NULL);
            op->stats.sp = new_sp;
        }

        /* Various heal spells */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "heal spell") == 0)
        {
	    object *tmp;
	    int success;

	    tmp = create_archetype_by_object_name(item->slaying);

	    success = cast_heal (op, op, tmp, 0);
	    free_object(tmp);
	    if (success) return;
	    else continue;
        }

        /* Remove curse */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "remove curse") == 0)
        {
            if (god_removes_curse (op, 0))
                return;
            else
                continue;
        }

        /* Remove damnation */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "remove damnation") == 0)
        {
            if (god_removes_curse (op, 1))
                return;
            else
                continue;
        }

        /* Heal depletion */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "heal depletion") == 0)
        {
            object *depl;
            archetype *at;
            int i;

            if ((at = find_archetype(ARCH_DEPLETION)) == NULL) {
                LOG (llevError, "Could not find archetype depletion.\n");
                continue;
            }
            depl = present_arch_in_ob (at, op);
            if (depl == NULL)
                continue;
            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
                    "Shimmering light surrounds and restores you!", NULL);
            for (i = 0; i < NUM_STATS; i++)
                if (get_attr_value (&depl->stats, i))
                    draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE,
				   MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
				   restore_msg[i], restore_msg[i]);
            remove_ob (depl);
            free_object (depl);
            fix_player (op);
            return;
        }
  
        /* Voices */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "voice_behind") == 0)
        {
            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
                    "You hear a voice from behind you, but you don't dare to "
                    "turn around:", NULL);
            draw_ext_info (NDI_WHITE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
			   item->msg, item->msg);
            return;
        }

        /* Messages */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "message") == 0)
        {
            draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
			   item->msg, item->msg);
            return;
        }

        /* Enchant weapon */
        if (item->type == BOOK && item->invisible
            && strcmp (item->name, "enchant weapon") == 0)
        {
            if (god_enchants_weapon (op, god, item, skill))
                return;
            else
                continue;
        }

        /* Spellbooks - works correctly only for prayers */
        if (item->type == SPELL)
        {
            if (check_spell_known (op, item->name))
                continue;
            if (item->level > skill->level)
                continue;

	    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
				 "%s grants you use of a special prayer!",
				 "%s grants you use of a special prayer!",
				 god->name);
	    do_learn_spell (op, item, 1);
	    return;

	}

        /* Other gifts */
        if ( ! item->invisible) {
          if (god_gives_present (op, god, tr)) 
                return;
            else
                continue;
        }
        /* else ignore it */
    }

    draw_ext_info (NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_PRAY,
		   "You feel rapture.", NULL);
}

/**
 * Checks and maybe punishes someone praying.
 * All applied items are examined, if player is using more items of other gods,
 * s/he loses experience in praying or general experience if no praying.
 */
static int god_examines_priest (object *op, object *god) {
    int reaction=1;
    object *item=NULL, *skop;

    for(item=op->inv;item;item=item->below) {
	if(QUERY_FLAG(item,FLAG_APPLIED)) {
	    reaction+=god_examines_item(god,item)*(item->magic?abs(item->magic):1);
	}
    }

    /* well, well. Looks like we screwed up. Time for god's revenge */
    if(reaction<0) { 
	int loss = 10000000;
	int angry = abs(reaction);

	for (skop = op->inv; skop != NULL; skop=skop->below)
	    if (skop->type == SKILL && skop->subtype == SK_PRAYING) break;

	if (skop)
	    loss = 0.05 * (float) skop->stats.exp;
	change_exp(op, -random_roll(0, loss*angry-1, op, PREFER_LOW),
		   skop?skop->skill:"none", SK_SUBTRACT_SKILL_EXP);
	if(random_roll(0, angry, op, PREFER_LOW)) {
	    object *tmp = create_archetype(LOOSE_MANA);
	    cast_magic_storm(op,tmp,op->level+(angry*3));
	}
	draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			     "%s becomes angry and punishes you!",
			     "%s becomes angry and punishes you!",
			     god->name);
    }
    return reaction;
}

/**
 * God checks item the player is using.
 * Return either -1 (bad), 0 (neutral) or
 * 1 (item is ok). If you are using the item of an enemy
 * god, it can be bad...-b.t.
 */

static int god_examines_item(object *god, object *item) {
    char buf[MAX_BUF];

    if(!god||!item) return 0;

    if(!item->title) return 1; /* unclaimed item are ok */

    sprintf(buf,"of %s",god->name);
    if(!strcmp(item->title,buf)) return 1; /* belongs to that God */ 

    if(god->title) { /* check if we have any enemy blessed item*/ 
	sprintf(buf,"of %s",god->title);
	if(!strcmp(item->title,buf)) {
	    if(item->env) {
		draw_ext_info_format(NDI_UNIQUE|NDI_NAVY,0,item->env,
			     MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			     "Heretic! You are using %s!",
			     "Heretic! You are using %s!",
			     query_name(item));
	    }
	    return -1;
	}
    }
    return 0; /* item is sacred to a non-enemy god/or is otherwise magical */ 
}


#if 0
/*
 * get_god no longer used - should perhaps be removed.
 * MSW 2006-06-02
 */

/**
 * Returns priest's god's id. 
 * Straight calls lookup_god_by_name
 */

static int get_god(object *priest) {
  int godnr=lookup_god_by_name(determine_god(priest)); 

  return godnr;
}
#endif

/**
 * Returns a string that is the name of the god that should be natively worshipped by a 
 * creature of who has race *race
 * if we can't find a god that is appropriate, we return NULL
 */
static const char *get_god_for_race(const char *race) {
    godlink *gl=first_god;
    const char *godname=NULL;

    if (race == NULL) return NULL;
    while(gl) {
	if (gl->arch->clone.race && !strcasecmp(gl->arch->clone.race, race)) {
	    godname=gl->name;
	    break;
	}
	gl=gl->next;
    }
    return godname;
}
/**
 * Changes the attributes of cone, smite, and ball spells as needed by the code.
 * Returns false if there was no race to assign to the slaying field of the spell, but
 * the spell attacktype contains AT_HOLYWORD.  -b.t.
 */

int tailor_god_spell(object *spellop, object *caster) {
    object *god=find_god(determine_god(caster));
    int caster_is_spell=0; 

    if (caster->type==SPELL_EFFECT || caster->type == SPELL) caster_is_spell=1;

    /* if caster is a rune or the like, it doesn't worship anything.  However,
     * if this object is owned by someone, then the god that they worship
     * is relevant, so use that.
     */
    if (!god && get_owner(caster)) god=find_god(determine_god(get_owner(caster)));

    if ( ! god || (spellop->attacktype & AT_HOLYWORD && !god->race)) {
        if ( ! caster_is_spell)
            draw_ext_info(NDI_UNIQUE, 0, caster, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOD,
			  "This prayer is useless unless you worship an appropriate god", NULL);
        else
            LOG (llevError, "BUG: tailor_god_spell(): no god\n");
        free_object(spellop);
        return 0;
    }

    /* either holy word or godpower attacks will set the slaying field */
    if (spellop->attacktype & AT_HOLYWORD || spellop->attacktype & AT_GODPOWER) { 
         if (spellop->slaying) {
             free_string(spellop->slaying);
             spellop->slaying = NULL;
         }
         if(!caster_is_spell)
            spellop->slaying = add_string(god->slaying);
	 else if(caster->slaying) 
	    spellop->slaying = add_string(caster->slaying);
    }

    /* only the godpower attacktype adds the god's attack onto the spell */
    if(spellop->attacktype & AT_GODPOWER)
         spellop->attacktype=spellop->attacktype|god->attacktype;

    /* tack on the god's name to the spell */
    if(spellop->attacktype&AT_HOLYWORD||spellop->attacktype&AT_GODPOWER) { 
         if(spellop->title) 
	   free_string(spellop->title);
         spellop->title=add_string(god->name);
         if(spellop->title){
	    char buf[MAX_BUF]; 
	    sprintf(buf,"%s of %s",spellop->name,spellop->title);
	    FREE_AND_COPY(spellop->name, buf);
	    FREE_AND_COPY(spellop->name_pl, buf);
	}
    } 

    return 1;
}


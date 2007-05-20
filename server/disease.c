/*
 * static char *rcsid_disease_c =
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

/*  This file contains all the code implementing diseases,
    except for odds and ends in attack.c and in
	 living.c*/



/*

For DISEASES:
Stat            Property        Definition

attacktype      Attack effects  Attacktype of the disease. usu. AT_GODPOWER.
other_arch      Creation        object created and dropped when symptom moved.
title           Message         When the "disease" "infects" something, it will
                                print "title victim!!!" to the player who owns
				the "disease".
wc+             Infectiousness  How well the plague spreads person-to-person
magic+          Range           range of infection
Stats*          Disability      What stats are reduced by the disease (str con...)
maxhp+          Persistence     How long the disease can last OUTSIDE the host.
value           TimeLeft        Counter for persistence
dam^            Damage          How much damage it does (%?).
maxgrace+       Duration        How long before the disease is naturally cured.
food            DurCount        Counter for Duration

speed           Speed           How often the disease moves.
last_sp^        Lethargy        Percentage of max speed--10 = 10% speed.

maxsp^          Mana deplete    Saps mana.
ac^             Progressiveness How the diseases increases in severity.
last_eat*^      Deplete food    saps food if negative
last_heal       GrantImmunity   If nonzero, disease does NOT grant immunity
                                when it runs out

exp             experience      experience awarded when plague cured
hp*^            ReduceRegen     reduces regeneration of disease-bearer
sp*^            ReduceSpRegen   reduces spellpoint regeneration

name            Name            Name of the plague
msg             message         What the plague says when it strikes.
race            those affected  species/race the plague strikes (* = everything)
level           Plague Level    General description of the plague's deadliness
armour          Attenuation     reduction in wc per generation of disease.
                                This builds in a self-limiting factor.


Explanations:
* means this # should be negative to cause adverse effect.
+ means that this effect is modulated in spells by ldur
^ means that this effect is modulated in spells by ldam

attacktype is the attacktype used by the disease to smite "dam" damage with.

wc/127 is the chance of someone in range catching it.

magic is the range at which infection may occur.  If negative, the range is
NOT level dependent.

Stats are stat modifications.  These should typically be negative.

maxhp is how long the disease will persist if the host dies and "drops" it,
      in "disease moves", i.e., moves of the disease.  If negative, permanent.


value is the counter for maxhp, it starts at maxhp and drops...

dam     if positive, it is straight damage.  if negative, a %-age.

maxgrace  how long in "disease moves" the disease lasts in the host, if negative,
          permanent until cured.

food    if negative, disease is permanent.  otherwise, decreases at <speed>,
        disease goes away at food=0, set to "maxgrace" on infection.

speed is the speed of the disease, how fast "disease moves" occur.

last_sp is the lethargy imposed on the player by the disease.  A lethargy
       of "1" reduces the players speed to 1% of its normal value.

maxsp how much mana is sapped per "disease move".  if negative, a %-age is
     taken.

ac  every "disease move" the severity of the symptoms are increased by
    ac/100.  (severity = 1 + (accumlated_progression)/100)

last_eat  increases food usage if negative.



For SYMPTOMS:

Stats            modify stats
hp               modify regen
value            progression counter (multiplier = value/100)
food             modify food use (from last_eat in DISEASE)
maxsp            suck mana ( as noted for DISEASE)
last_sp          Lethargy
msg              What to say
speed            speed of movement, from DISEASE


*/


#include <global.h>
#include <object.h>
#include <living.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <spells.h>
#include <sounds.h>
#include <skills.h>

static int remove_symptoms(object *disease);
static object *find_symptom(object *disease);
static int check_infection(object *disease);
static int do_symptoms(object *disease);
static int grant_immunity(object *disease);



/*  IMPLEMENTATION NOTES

	 Diseases may be contageous.  They are objects which exist in a player's
inventory.  They themselves do nothing, except modify Symptoms, or
spread to other live objects.  Symptoms are what actually damage the player:
these are their own object. */

/* check if victim is susceptible to disease. */
static int is_susceptible_to_disease(object *victim, object *disease)
{
    /* Non living and DMs are immune. */
    if(!QUERY_FLAG(victim,FLAG_ALIVE) || QUERY_FLAG(victim, FLAG_WIZ)) return 0;

    if(strstr(disease->race, "*") && !QUERY_FLAG(victim, FLAG_UNDEAD))
	return 1;

    if((disease->race == undead_name) && QUERY_FLAG(victim, FLAG_UNDEAD))
	return 1;

    if((victim->race && strstr(disease->race, victim->race)) ||
       strstr(disease->race, victim->name))
	return 1;

    return 0;
}

int move_disease(object *disease) {
    /*  first task is to determine if the disease is inside or outside of someone.
     * If outside, we decrement 'value' until we're gone.
     */

    /* DMs don't infect, and don't suffer either. */
    if (disease->env && QUERY_FLAG(disease->env, FLAG_WIZ))
        return 0;

    if(disease->env==NULL) { /* we're outside of someone */
	if ( disease->stats.maxhp > 0 ) disease->value--;
	if(disease->value==0) {
	    remove_ob(disease);
	    free_object(disease);
	    return 1;
	}
    } else {
	/* if we're inside a person, have the disease run its course */
	/* negative foods denote "perpetual" diseases. */
	if(disease->stats.food>0) {
	    disease->stats.food--;
	    if (disease->stats.food==0) {
		remove_symptoms(disease);  /* remove the symptoms of this disease */
		grant_immunity(disease);
		remove_ob(disease);
		free_object(disease);
		return 1;
	    }
	}
    }
    /*  check to see if we infect others */
    check_infection(disease);

    /* impose or modify the symptoms of the disease */
    if(disease->env && is_susceptible_to_disease(disease->env, disease))
	do_symptoms(disease);

    return 0;
}

/* remove any symptoms of disease
 * Modified by MSW 2003-03-28 do try to find all the symptom the
 * player may have - I think through some odd interactoins with
 * disease level and player level and whatnot, a player could get
 * more than one symtpom to a disease.
 */

static int remove_symptoms(object *disease) {
    object *symptom, *victim=NULL;

    while ((symptom = find_symptom(disease)) != NULL) {
	if (!victim) victim=symptom->env;
	remove_ob(symptom);
	free_object(symptom);
    }
    if(victim)
        fix_object(victim);
    return 0;
}

/* argument is a disease */
static object * find_symptom(object *disease) {
  object *walk;

  /* check the inventory for symptoms */
  for(walk=disease->env->inv;walk;walk = walk->below)
	 if(!strcmp(walk->name,disease->name)&&walk->type==SYMPTOM) return walk;
  return NULL;
}

/*  searches around for more victims to infect */
static int check_infection(object *disease) {
    int x,y,range, mflags;
    mapstruct *map, *map2;
    object *tmp;
    sint16 i, j, i2, j2;

    range = abs(disease->magic);
    if(disease->env) {
	x = disease->env->x;
	y = disease->env->y;
	map=disease->env->map;
    }
    else {
	x = disease->x;
	y = disease->y;
	map = disease->map;
    }

    if(map == NULL) return 0;
    for(i=x-range;i<=x+range;i++) {
	for(j=y-range;j<=y+range;j++) {
	    mflags = get_map_flags(map,&map2, i,j, &i2, &j2);
	    if (!(mflags & P_OUT_OF_MAP) && (mflags & P_IS_ALIVE)) {
		for(tmp=get_map_ob(map2,i2,j2);tmp;tmp=tmp->above) {
		    infect_object(tmp,disease,0);
		}
	    }
	}
    }
    return 1;
}


/*  check to see if an object is infectable:
 *	 objects with immunity aren't infectable.
 *	 objects already infected aren't infectable.
 *	 dead objects aren't infectable.
 *	 undead objects are infectible only if specifically named.
*/
int infect_object(object *victim, object *disease, int force) {
    object *tmp;
    object *new_disease;

    /* don't infect inanimate objects */
    if(!QUERY_FLAG(victim,FLAG_MONSTER) && !(victim->type==PLAYER)) return 0;

    /* check and see if victim can catch disease:  diseases
     *  are specific
     */
    if(!is_susceptible_to_disease(victim, disease)) return 0;

    /* If disease is on battleground, only infect other victims on battleground.
       Not checking results in spectators being infected, which could lead to PK. */
    if ((disease->map && op_on_battleground(disease, NULL, NULL)) || (disease->env && op_on_battleground(disease->env, NULL, NULL)))
        if (!op_on_battleground(victim, NULL, NULL))
            return 0;

    /* roll the dice on infection before doing the inventory check!  */
    if(!force && (random_roll(0, 126, victim, PREFER_HIGH) >= disease->stats.wc))
	return 0;

    /* do an immunity check */
    if(victim->head) tmp = victim->head->inv;
    else tmp = victim->inv;

    /* There used to (IMO) be a flaw in the below - it used to be the case
     * that if level check was done for both immunity and disease. This could
     * result in a person with multiple afflictions of the same disease
     * (eg, level 1 cold, level 2 cold, level 3 cold, etc), as long as
     * they were cast in that same order.  Instead, change it so that
     * if you diseased, you can't get diseased more.
     */

    for(/* tmp initialized in if, above */;tmp;tmp=tmp->below) {
	if(tmp->type == SIGN && !strcmp(tmp->name,disease->name) && tmp->level >= disease->level)
		return 0;  /*Immune! */
	else if (tmp->type==DISEASE && !strcmp(tmp->name,disease->name))
	    return 0;	/* already diseased */
    }

    /*  If we've gotten this far, go ahead and infect the victim.  */
    new_disease = get_object();
    copy_object(disease,new_disease);
    new_disease->stats.food=disease->stats.maxgrace;
    new_disease->value=disease->stats.maxhp;
    new_disease->stats.wc -= disease->last_grace;  /* self-limiting factor */

    /* Unfortunately, set_owner does the wrong thing to the skills pointers
     *  resulting in exp going into the owners *current* chosen skill.
     */

    if(get_owner(disease)) {
	set_owner(new_disease,disease->owner);

	/* Only need to update skill if different */
	if (new_disease->skill != disease->skill) {
	    if (new_disease->skill) free_string(new_disease->skill);
	    if (disease->skill) new_disease->skill = add_refcount(disease->skill);
	}
    }
    else {  /* for diseases which are passed by hitting, set owner and praying skill*/
	if(disease->env && disease->env->type==PLAYER) {
	    object *player = disease->env;

	    set_owner(new_disease,player);
	    /* the skill pointer for these diseases should already be set up -
	     * hardcoding in 'praying' is not the right approach.
	     */
	}
    }

    insert_ob_in_ob(new_disease,victim);
    /* This appears to be a horrible case of overloading 'NO_PASS'
     * for meaning in the diseases.
     */
    new_disease->move_block = 0;
    if(new_disease->owner && new_disease->owner->type==PLAYER) {
	 char buf[128];
	 /* if the disease has a title, it has a special infection message
	  * This messages is printed in the form MESSAGE victim
	  */
	 if(new_disease->title)
	    sprintf(buf,"%s %s!!",disease->title,victim->name);
	 else
	    sprintf(buf,"You infect %s with your disease, %s!",victim->name,new_disease->name);

	 if(victim->type == PLAYER)
	    draw_ext_info(NDI_UNIQUE | NDI_RED, 0, new_disease->owner,
			  MSG_TYPE_ATTACK, MSG_TYPE_ATTACK_DID_HIT,
			  buf, buf);
	 else
	    draw_ext_info(0, 4, new_disease->owner, MSG_TYPE_ATTACK, MSG_TYPE_ATTACK_DID_HIT,
			  buf, buf);
    }
    if(victim->type==PLAYER)
	 draw_ext_info(NDI_UNIQUE | NDI_RED,0,victim, MSG_TYPE_ATTRIBUTE,
		       MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START,
		       "You suddenly feel ill.", NULL);

    return 1;

}



/*  this function monitors the symptoms caused by the disease (if any),
causes symptoms, and modifies existing symptoms in the case of
existing diseases.  */

static int do_symptoms(object *disease) {
    object *symptom;
    object *victim;
    object *tmp;
    victim = disease->env;

    /* This is a quick hack - for whatever reason, disease->env will point
     * back to disease, causing endless loops.  Why this happens really needs
     * to be found, but this should at least prevent the infinite loops.
     */

    if(victim == NULL || victim==disease)
	return 0;/* no-one to inflict symptoms on */

    /* DMs don't suffer from diseases. */
    if (QUERY_FLAG(victim, FLAG_WIZ))
        return 0;

    symptom = find_symptom(disease);
    if(symptom==NULL) {
	/* no symptom?  need to generate one! */
	object *new_symptom;

	/* first check and see if the carrier of the disease is immune.  If so, no symptoms!  */
	if(!is_susceptible_to_disease(victim, disease)) return 0;

	/* check for an actual immunity */
	/* do an immunity check */
	if(victim->head) tmp = victim->head->inv;
	else tmp = victim->inv;

	for(/* tmp initialized in if, above */;tmp;tmp=tmp->below) {
	    if(tmp->type == SIGN)  /* possibly an immunity, or diseased*/
		if(!strcmp(tmp->name,disease->name) && tmp->level >= disease->level)
		    return 0;  /*Immune! */
	}

	new_symptom = create_archetype(ARCH_SYMPTOM);

	/* Something special done with dam.  We want diseases to be more
	 * random in what they'll kill, so we'll make the damage they
	 * do random, note, this has a weird effect with progressive diseases.
	 */
	if(disease->stats.dam != 0) {
	    int dam = disease->stats.dam;

	    /* reduce the damage, on average, 50%, and making things random. */

	    dam = random_roll(1, FABS(dam), victim, PREFER_LOW);
	    if(disease->stats.dam < 0) dam = -dam;
	    new_symptom->stats.dam = dam;
	}


	new_symptom->stats.maxsp = disease->stats.maxsp;
	new_symptom->stats.food = new_symptom->stats.maxgrace;

	FREE_AND_COPY(new_symptom->name, disease->name);
	FREE_AND_COPY(new_symptom->name_pl, disease->name);
	new_symptom->level = disease->level;
	new_symptom->speed = disease->speed;
	new_symptom->value = 0;
	new_symptom->stats.Str =disease->stats.Str;
	new_symptom->stats.Dex = disease->stats.Dex;
	new_symptom->stats.Con = disease->stats.Con;
	new_symptom->stats.Wis = disease->stats.Wis;
	new_symptom->stats.Int = disease->stats.Int;
	new_symptom->stats.Pow = disease->stats.Pow;
	new_symptom->stats.Cha = disease->stats.Cha;
	new_symptom->stats.sp  = disease->stats.sp;
	new_symptom->stats.food =disease->last_eat;
	new_symptom->stats.maxsp = disease->stats.maxsp;
	new_symptom->last_sp = disease->last_sp;
	new_symptom->stats.exp = 0;
	new_symptom->stats.hp = disease->stats.hp;
	new_symptom->msg = add_string(disease->msg);
	new_symptom->attacktype = disease->attacktype;
	new_symptom->other_arch = disease->other_arch;

	set_owner(new_symptom,disease->owner);
	if (new_symptom->skill != disease->skill) {
	    if (new_symptom->skill) free_string(new_symptom->skill);
	    if (disease->skill) new_symptom->skill = add_refcount(disease->skill);
	}
	new_symptom->move_block=0;
	insert_ob_in_ob(new_symptom,victim);
	return 1;
    }

    /* now deal with progressing diseases:  we increase the debility
     * caused by the symptoms.
     */

    if(disease->stats.ac!=0) {
	float scale;

	symptom->value += disease->stats.ac;
	scale = 1.0 + symptom->value/100.0;
	/* now rescale all the debilities */
	symptom->stats.Str = (int) (scale*disease->stats.Str);
	symptom->stats.Dex = (int) (scale*disease->stats.Dex);
	symptom->stats.Con = (int) (scale*disease->stats.Con);
	symptom->stats.Wis = (int) (scale*disease->stats.Wis);
	symptom->stats.Int = (int) (scale*disease->stats.Int);
	symptom->stats.Pow = (int) (scale*disease->stats.Pow);
	symptom->stats.Cha = (int) (scale*disease->stats.Cha);
	symptom->stats.dam = (int) (scale*disease->stats.dam);
	symptom->stats.sp = (int)  (scale*disease->stats.sp);
	symptom->stats.food = (int)  (scale*disease->last_eat);
	symptom->stats.maxsp = (int)  (scale*disease->stats.maxsp);
	symptom->last_sp = (int)  (scale*disease->last_sp);
	symptom->stats.exp = 0;
	symptom->stats.hp = (int) (scale*disease->stats.hp);
	symptom->msg = add_string(disease->msg);
	symptom->attacktype = disease->attacktype;
	symptom->other_arch = disease->other_arch;
    }
    SET_FLAG(symptom,FLAG_APPLIED);
    fix_object(victim);
    return 1;
}


/*  grants immunity to plagues we've seen before.  */
static int grant_immunity(object *disease) {
  object * immunity;
  object *walk;
  /* Don't give immunity to this disease if last_heal is set. */
  if(disease->last_heal) return 0;
  /*  first, search for an immunity of the same name */
  for(walk=disease->env->inv;walk;walk=walk->below) {
	 if(walk->type==98 && !strcmp(disease->name,walk->name)) {
		walk->level = disease->level;
		return 1; /* just update the existing immunity. */
	 }
  }
  immunity = create_archetype("immunity");
  immunity->name = add_string(disease->name);
  immunity->level = disease->level;
  immunity->move_block = 0;
  insert_ob_in_ob(immunity,disease->env);
  return 1;

}


/*  make the symptom do the nasty things it does  */

int move_symptom(object *symptom) {
    object *victim = symptom->env;
    object *new_ob;
    int sp_reduce;
    tag_t tag = symptom->count;

    if(victim == NULL || victim->map==NULL) {  /* outside a monster/player, die immediately */
        remove_ob(symptom);
        free_object(symptom);
        return 0;
    }

    if(symptom->stats.dam > 0)  hit_player(victim,symptom->stats.dam,symptom,symptom->attacktype,1);
    else hit_player(victim,MAX(1,-victim->stats.maxhp * symptom->stats.dam / 100.0),symptom,symptom->attacktype,1);

    /* In most cases, if the victim has been freed, the logic that
     * does that will also free the symptom, so check for that.
     */
    if(QUERY_FLAG(victim, FLAG_FREED) && !was_destroyed(symptom,tag)) {
        remove_ob(symptom);
        free_object(symptom);
        return 0;
    }

    if(symptom->stats.maxsp>0) sp_reduce = symptom->stats.maxsp;
    else sp_reduce = MAX(1,victim->stats.maxsp * symptom->stats.maxsp/100.0);
    victim->stats.sp = MAX(0,victim->stats.sp - sp_reduce);

    /* create the symptom "other arch" object and drop it here
     * under every part of the monster
     * The victim may well have died.
     */

    if(victim->map==NULL) return 0;
    if(symptom->other_arch) {
        object *tmp;
        tmp=victim;
        if(tmp->head!=NULL) tmp=tmp->head;
        for(/*tmp initialized above */;tmp!=NULL;tmp=tmp->more) {
            char name[MAX_BUF];
            new_ob = arch_to_object(symptom->other_arch);
            snprintf(name, sizeof(name), "%s's %s", victim->name, new_ob->name);
            FREE_AND_COPY(new_ob->name, name);
            if (new_ob->name_pl != NULL) {
                snprintf(name, sizeof(name), "%s's %s", victim->name, new_ob->name_pl);
                FREE_AND_COPY(new_ob->name_pl, name);
            }
            new_ob->x = tmp->x;
            new_ob->y = tmp->y;
            new_ob->map = victim->map;
            insert_ob_in_map(new_ob,victim->map,victim,0);
        }
    }
    draw_ext_info(NDI_UNIQUE | NDI_RED,0,victim,
		  MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START,
		  symptom->msg, symptom->msg);

    return 1;
}


/*  possibly infect due to direct physical contact
  i.e., AT_PHYSICAL-- called from "hit_player_attacktype" */

int check_physically_infect(object *victim, object *hitter) {
  object *walk;
  /* search for diseases, give every disease a chance to infect */
  for(walk=hitter->inv;walk!=NULL;walk=walk->below)
	 if(walk->type==DISEASE) infect_object(victim,walk,0);
  return 1;
}

/* do the cure disease stuff, from the spell "cure disease" */

int cure_disease(object *sufferer,object *caster) {
    object *disease, *next;
    int casting_level;
    int cure=0;

    if(caster) casting_level = caster->level;
    else casting_level = 1000;  /* if null caster, CURE all.  */

    for(disease=sufferer->inv;disease;disease=next) {
	next=disease->below;

	if(disease->type==DISEASE) {  /* attempt to cure this disease */
	    /* If caster lvel is higher than disease level, cure chance
	     * is automatic.  If lower, then the chance is basically
	     * 1 in level_diff - if there is a 5 level difference, chance
	     * is 1 in 5.
	     */
	    if ((casting_level >= disease->level) ||
		 ( ! (random_roll(0, (disease->level - casting_level - 1),
				  caster, PREFER_LOW) ))) {

		    remove_symptoms(disease);
		    remove_ob(disease);
		    cure=1;
		    if(caster) change_exp(caster,disease->stats.exp,
					  caster->chosen_skill?caster->chosen_skill->skill:NULL, 0);
		    free_object(disease);
	    }
	}
    }
    if (cure) {
	/* Only draw these messages once */
	if (caster)
	    draw_ext_info_format(NDI_UNIQUE,0,caster, MSG_TYPE_SPELL, MSG_TYPE_SPELL_HEAL,
				 "You cure a disease!", NULL);

	draw_ext_info(NDI_UNIQUE,0,sufferer,
		      MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
		      "You no longer feel diseased.", NULL);
    }
  return 1;
}

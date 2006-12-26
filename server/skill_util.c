/*
 * static char *rcsid_skill_util_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copryight (C) 2006 Mark Wedel & Crossfire Development Team
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

/* Created July 95 to separate skill utilities from actual skills -b.t. */

/* Reconfigured skills code to allow linking of skills to experience
 * categories. This is done solely through the init_new_exp_system() fctn.
 * June/July 1995 -b.t. thomas@astro.psu.edu
 */

/* July 1995 - Initial associated skills coding. Experience gains
 * come solely from the use of skills. Overcoming an opponent (in combat,
 * finding/disarming a trap, stealing from somebeing, etc) gains
 * experience. Calc_skill_exp() handles the gained experience using
 * modifications in the skills[] table. - b.t.
 */

/* define the following for skills utility debuging */
/* #define SKILL_UTIL_DEBUG */

#define WANT_UNARMED_SKILLS

#include <global.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <living.h>     /* for defs of STR,CON,DEX,etc. -b.t.*/
#include <spells.h>

static int attack_hth(object *pl, int dir, const char *string, object *skill);
static int attack_melee_weapon(object *op, int dir, const char *string, object *skill);

const char *skill_names[NUM_SKILLS];

/* init_skills basically just sets up the skill_names table
 * above.  The index into the array is set up by the
 * subtypes.
 */
void init_skills(void) {
    int i;
    archetype *at;

    for (i=0; i<NUM_SKILLS; i++)
	skill_names[i] = NULL;

    for(at = first_archetype;at!=NULL;at=at->next) {
	if (at->clone.type == SKILL) {
	    if (skill_names[at->clone.subtype] != NULL) {
		LOG(llevError, "init_skills: multiple skill using same subtype %d, %s, %s\n",
		    at->clone.subtype, skill_names[at->clone.subtype], at->clone.skill);
	    } else {
		skill_names[at->clone.subtype] = add_refcount(at->clone.skill);
	    }
	}
    }

    /* This isn't really an error if there is no skill subtype set, but
     * checking for this may catch some user errors.
     */
    for (i=1; i<NUM_SKILLS; i++) {
	if (!skill_names[i])
	    LOG(llevError, "init_skills: skill subtype %d doesn't have a name?\n",
		i);
    }
}


/* This function goes through the player inventory and sets
 * up the last_skills[] array in the player object.
 * the last_skills[] is used to more quickly lookup skills - 
 * mostly used for sending exp.
 */
void link_player_skills(object *op)
{
    object *tmp;

    for (tmp=op->inv; tmp; tmp=tmp->below) {
	if (tmp->type == SKILL) {
	    /* This is really a warning, hence no else below */
	    if (op->contr->last_skill_ob[tmp->subtype] && op->contr->last_skill_ob[tmp->subtype] != tmp) {
		LOG(llevError,"Multiple skills with the same subtype? %s, %s\n",
		    op->contr->last_skill_ob[tmp->subtype]->skill, tmp->skill);
	    }
	    if (tmp->subtype >= NUM_SKILLS) {
		LOG(llevError,"Invalid subtype number %d (range 0-%d)\n",
		    tmp->subtype, NUM_SKILLS);
	    } else {
		op->contr->last_skill_ob[tmp->subtype] = tmp;
		op->contr->last_skill_exp[tmp->subtype] = -1;
	    }
	}
    }
}

/* This returns the skill pointer of the given name (the
 * one that accumlates exp, has the level, etc).
 *
 * It is presumed that the player will be needing to actually
 * use the skill, so thus if use of the skill requires a skill
 * tool, this code will equip it.
 */
object *find_skill_by_name(object *who, const char *name)
{
    object *skill=NULL, *skill_tool=NULL, *tmp;

    if (!name) return NULL;

    /* We make sure the length of the string in the object is greater
     * in length than the passed string. Eg, if we have a skill called
     * 'hi', we don't want to match if the user passed 'high'
     */
    for (tmp=who->inv; tmp!=NULL; tmp=tmp->below) {
	if (tmp->type == SKILL && !strncasecmp(name, tmp->skill, strlen(name)) &&
	    strlen(tmp->skill) >= strlen(name)) skill = tmp;

	/* Try to find appropriate skilltool.  If the player has one already
	 * applied, we try to keep using that one.
	 */
	else if (tmp->type == SKILL_TOOL && !strncasecmp(name, tmp->skill, strlen(name)) &&
	    strlen(tmp->skill) >= strlen(name)) {
	    if (QUERY_FLAG(tmp, FLAG_APPLIED)) skill_tool = tmp;
	    else if (!skill_tool || !QUERY_FLAG(skill_tool, FLAG_APPLIED))
		skill_tool = tmp;
	}
    }
    /* If this is a skill that can be used without a tool, return it */
    if (skill && QUERY_FLAG(skill, FLAG_CAN_USE_SKILL)) return skill;

    /* Player has a tool to use the skill.  IF not applied, apply it -
     * if not successful, return null.  If they do have the skill tool
     * but not the skill itself, give it to them.
     */
    if (skill_tool) {
	if (!QUERY_FLAG(skill_tool, FLAG_APPLIED)) {
	    if (apply_special(who, skill_tool, 0)) return NULL;
	}
	if (!skill) {
	    skill = give_skill_by_name(who, skill_tool->skill);
	    link_player_skills(who);
	}
	return skill;
    }
    return NULL;
}


/* This returns the skill pointer of the given name (the
 * one that accumlates exp, has the level, etc).
 *
 * It is presumed that the player will be needing to actually
 * use the skill, so thus if use of the skill requires a skill
 * tool, this code will equip it.
 * 
 * This code is basically the same as find_skill_by_name() above,
 * but instead a skill name, we search by matching number.
 * this replaces find_skill.
 */
object *find_skill_by_number(object *who, int skillno)
{
    object *skill=NULL, *skill_tool=NULL, *tmp;

    if (skillno < 1 || skillno >= NUM_SKILLS) return NULL;

    for (tmp=who->inv; tmp!=NULL; tmp=tmp->below) {
	if (tmp->type == SKILL && tmp->subtype == skillno) skill = tmp;

	/* Try to find appropriate skilltool.  If the player has one already
	 * applied, we try to keep using that one.
	 */
	else if (tmp->type == SKILL_TOOL && tmp->subtype == skillno) {
	    if (QUERY_FLAG(tmp, FLAG_APPLIED)) skill_tool = tmp;
	    else if (!skill_tool || !QUERY_FLAG(skill_tool, FLAG_APPLIED))
		skill_tool = tmp;
	}
    }
    /* If this is a skill that can be used without a tool, return it */
    if (skill && QUERY_FLAG(skill, FLAG_CAN_USE_SKILL)) return skill;

    /* Player has a tool to use the skill.  IF not applied, apply it -
     * if not successful, return null.  If they do have the skill tool
     * but not the skill itself, give it to them.
     */
    if (skill_tool) {
	if (!QUERY_FLAG(skill_tool, FLAG_APPLIED)) {
	    if (apply_special(who, skill_tool, 0)) return NULL;
	}
	if (!skill) {
	    skill = give_skill_by_name(who, skill_tool->skill);
	    link_player_skills(who);
	}
	return skill;
    }
    return NULL;
}

/* This changes the objects skill to new_skill.
 * note that this function doesn't always need to get used -
 * you can now add skill exp to the player without the chosen_skill being
 * set.  This function is of most interest to players to update
 * the various range information.
 * if new_skill is null, this just unapplies the skill.
 * flag has the current meaning:
 * 0x1: If set, don't update the range pointer.  This is useful when we
 *   need to ready a new skill, but don't want to clobber range.
 * return 1 on success, 0 on error
 */

int change_skill (object *who, object *new_skill, int flag)
{
    int old_range;

    if ( who->type != PLAYER )
        return 0;
    
    old_range = who->contr->shoottype;

    if (who->chosen_skill && who->chosen_skill == new_skill)
    {
        /* optimization for changing skill to current skill */
        if (who->type == PLAYER && !(flag & 0x1))
            who->contr->shoottype = range_skill;
        return 1;
    }

    if (!new_skill || who->chosen_skill) 
	if (who->chosen_skill) apply_special(who, who->chosen_skill, AP_UNAPPLY);

    /* Only goal in this case was to unapply a skill */
    if (!new_skill) return 0;

    if (apply_special (who, new_skill, AP_APPLY)) {
	return 0;
    }
    if (flag & 0x1)
	who->contr->shoottype = old_range;

    return 1;
}

/* This function just clears the chosen_skill and range_skill values
 * inthe player.
 */
void clear_skill(object *who)
{
    who->chosen_skill = NULL;
    CLEAR_FLAG(who, FLAG_READY_SKILL);
    if (who->type == PLAYER) {
	who->contr->ranges[range_skill] = NULL;
	if (who->contr->shoottype == range_skill)
	    who->contr->shoottype = range_none;
    }
}

/**
 * Main skills use function-similar in scope to cast_spell().
 * We handle all requests for skill use outside of some combat here. 
 * We require a separate routine outside of fire() so as to allow monsters 
 * to utilize skills.
 * This is changed (2002-11-30) from the old method that returned
 * exp - no caller needed that info, but it also prevented the callers
 * from know if a skill was actually used, as many skills don't
 * give any exp for their direct use (eg, throwing).
 * It returns 0 if no skill was used.
 * @param op The object actually using the skill
 * @param part An object taking part of the skill usage, used by throwing
 * @param skill The skill used by op
 * @param dir The direction in which the skill is used
 * @param string A parameter string, necessary to use some skills
 * @return 1 if the use of the skill was successful, 0 otherwise
 */

int do_skill (object *op, object *part, object *skill, int dir, const char *string) {
    int success=0, exp=0;
    object *tmp;

    if (!skill) return 0;

    /* The code below presumes that the skill points to the object that
     * holds the exp, level, etc of the skill.  So if this is a player
     * go and try to find the actual real skill pointer, and if the
     * the player doesn't have a bucket for that, create one.
     */
    if (skill->type != SKILL && op->type == PLAYER) {
	for (tmp = op->inv; tmp!=NULL; tmp=tmp->below) {
	    if (tmp->type == SKILL && tmp->skill == skill->skill) break;
	}
	if (!tmp) tmp=give_skill_by_name(op, skill->skill);
	skill = tmp;
    }

    switch(skill->subtype) {
	case SK_LEVITATION:
	    /* Not 100% sure if this will work with new movement code -
	     * the levitation skill has move_type for flying, so when
	     * equipped, that should transfer to player, when not,
	     * shouldn't.
	     */
	    if(QUERY_FLAG(skill,FLAG_APPLIED)) { 
		CLEAR_FLAG(skill,FLAG_APPLIED);
		draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			      "You come to earth.", NULL);
	    }
	    else {
		SET_FLAG(skill,FLAG_APPLIED);
		draw_ext_info(NDI_UNIQUE,0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_SUCCESS,
			      "You rise into the air!.", NULL);
	    }
        fix_object(op);
	    success=1;
	    break;

	case SK_STEALING:
	    exp = success = steal(op, dir, skill);
	    break;

	case SK_LOCKPICKING:
	    exp = success = pick_lock(op, dir, skill);
	    break;

	case SK_HIDING:
	    exp = success = hide(op, skill);
	    break;

	case SK_JUMPING:
	    success = jump(op, dir, skill);
	    break;

	case SK_INSCRIPTION:
	    exp = success = write_on_item(op,string, skill);
	    break;

	case SK_MEDITATION:
	    meditate(op, skill);
	    success=1;
	    break;
	    /* note that the following 'attack' skills gain exp through hit_player() */

	case SK_KARATE:
	    (void) attack_hth(op,dir,"karate-chopped", skill);
	    break;

	case SK_PUNCHING:
	    (void) attack_hth(op,dir,"punched", skill);
	    break;

	case SK_FLAME_TOUCH:
	    (void) attack_hth(op,dir,"flamed", skill);
	    break;

	case SK_CLAWING:
	    (void) attack_hth(op,dir,"clawed", skill);
	    break;

	case SK_ONE_HANDED_WEAPON:
	case SK_TWO_HANDED_WEAPON:
	    (void) attack_melee_weapon(op,dir,NULL, skill);
	    break;

	case SK_FIND_TRAPS:
	    exp = success = find_traps(op, skill);
	    break;

	case SK_SINGING:
	    exp = success = singing(op,dir, skill);
	    break;

	case SK_ORATORY:
	    exp = success = use_oratory(op,dir, skill);
	    break;

	case SK_SMITHERY:
	case SK_BOWYER:
	case SK_JEWELER:
	case SK_ALCHEMY:
	case SK_THAUMATURGY:
	case SK_LITERACY:
	case SK_WOODSMAN:
        if (use_alchemy(op) == 0)
			exp = success = skill_ident(op,skill);
	    break;

	case SK_DET_MAGIC:
	case SK_DET_CURSE:
	    exp = success = skill_ident(op,skill);
	    break;

	case SK_DISARM_TRAPS:
	    exp = success = remove_trap(op,dir, skill);
	    break;

	case SK_THROWING:
	    success = skill_throw(op,part,dir,string, skill);
	    break;

	case SK_SET_TRAP:
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			  "This skill is not currently implemented.", NULL);
	    break;

	case SK_USE_MAGIC_ITEM:
	case SK_MISSILE_WEAPON:
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			  "There is no special attack for this skill.", NULL);
	    break;

	case SK_PRAYING:
	    success = pray(op, skill);
	    break;

	case SK_BARGAINING:
	    success = describe_shop(op);
	    break;

	case SK_SORCERY:
	case SK_EVOCATION:
	case SK_PYROMANCY:
	case SK_SUMMONING:
	case SK_CLIMBING:
	    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			  "This skill is already in effect.", NULL);
	    break;

	default:
	    LOG(llevDebug,"%s attempted to use unknown skill: %d\n"
                ,query_name(op), op->chosen_skill->stats.sp);
	    break;
    }

    /* For players we now update the speed_left from using the skill. 
     * Monsters have no skill use time because of the random nature in 
     * which use_monster_skill is called already simulates this.
     * If certain skills should take more/less time, that should be
     * in the code for the skill itself.
     */
    
    if(op->type==PLAYER) op->speed_left -= 1.0;

    /* this is a good place to add experience for successfull use of skills.
     * Note that add_exp() will figure out player/monster experience
     * gain problems.
     */
 
    if(success && exp) change_exp(op,exp, skill->skill, SK_SUBTRACT_SKILL_EXP);
            
    return success;
}

/* calc_skill_exp() - calculates amount of experience can be gained for
 * successfull use of a skill.  Returns value of experience gain.
 * Here we take the view that a player must 'overcome an opponent'
 * in order to gain experience. Examples include foes killed combat,
 * finding/disarming a trap, stealing from somebeing, etc.
 * The gained experience is based primarily on the difference in levels,
 * exp point value of vanquished foe, the relevent stats of the skill being
 * used and modifications in the skills[] table.
 *
 * For now, monsters and players will be treated differently. Below I give
 * the algorithm for *PLAYER* experience gain. Monster exp gain is simpler.
 * Monsters just get 10% of the exp of the opponent.
 *
 * players get a ratio, eg, opponent lvl / player level.  This is then
 * multiplied by various things.  If simple exp is true, then
 * this multiplier, include the level difference, is always 1.
 * This revised method prevents some cases where there are big gaps
 * in the amount you get just because you are now equal level vs lower
 * level
 * who is player/creature that used the skill.
 * op is the object that was 'defeated'.
 * skill is the skill used.  If no skill is used, it should just
 * point back to who.
 *
 */

int calc_skill_exp(object *who, object *op, object *skill) {
    int op_exp=0,op_lvl= 0;
    float base,value,lvl_mult=0.0;

    if (!skill) skill = who;

    /* Oct 95 - where we have an object, I expanded our treatment
     * to 3 cases:
     * non-living magic obj, runes and everything else.
     *
     * If an object is not alive and magical we set the base exp higher to
     * help out exp awards for skill_ident skills. Also, if
     * an item is type RUNE, we give out exp based on stats.Cha
     * and level (this was the old system) -b.t. 
     */ 

    if(!op) { 		/* no item/creature */ 
        op_lvl= who->map->difficulty < 1 ? 1: who->map->difficulty;
        op_exp = 0;
    } else if(op->type==RUNE || op->type==TRAP) { /* all traps. If stats.Cha > 1 we use that
				 * for the amount of experience */
        op_exp = op->stats.Cha>1 ? op->stats.Cha : op->stats.exp;
        op_lvl = op->level;
    } else { 		/* all other items/living creatures */ 
        op_exp = op->stats.exp;
	op_lvl = op->level;
        if(!QUERY_FLAG(op,FLAG_ALIVE)) { /* for ident/make items */ 
	    op_lvl += 5 * abs(op->magic);
	} 
    }

    if(op_lvl<1) op_lvl = 1;

    if(who->type!=PLAYER) {             /* for monsters only */
        return ((int) (op_exp*0.1)+1);	/* we add one to insure positive value is returned */
    } else {                            /* for players */
	base = op_exp;
	/* if skill really is a skill, then we can look at the skill archetype for
	 * bse reward value (exp) and level multiplier factor.
	 */
	if (skill->type == SKILL) {
	    base += skill->arch->clone.stats.exp;
	    if (settings.simple_exp) {
		if (skill->arch->clone.level)
		    lvl_mult = (float) skill->arch->clone.level / 100.0;
		else
		    lvl_mult = 1.0;	/* no adjustment */
	    }
	    else {
		if (skill->level) 
		    lvl_mult = ((float) skill->arch->clone.level * (float) op_lvl) / ((float) skill->level * 100.0);
		else
		    lvl_mult = 1.0;
	    }
	} else {
	    /* Don't divide by zero here! */
	    lvl_mult = (float) op_lvl / (float) (skill->level?skill->level:1);
	}
    }
 
    /* assemble the exp total, and return value */
 
    value =  base * lvl_mult;
    if (value < 1) value=1;	/* Always give at least 1 exp point */
    
#ifdef SKILL_UTIL_DEBUG
      LOG(llevDebug,"calc_skill_exp(): who: %s(lvl:%d)  op:%s(lvl:%d)\n",
		who->name,skill->level,op->name,op_lvl);
#endif
    return ( (int) value); 
}

/* Learn skill. This inserts the requested skill in the player's
 * inventory. The skill field of the scroll should have the
 * exact name of the requested skill.
 * This one actually teaches the player the skill as something 
 * they can equip.
 * Return 0 if the player knows the skill, 1 if the
 * player learns the skill, 2 otherwise.
 */
 
int
learn_skill (object *pl, object *scroll) {
    object *tmp;

    if (!scroll->skill) {
	LOG(llevError,"skill scroll %s does not have skill pointer set.\n", scroll->name);
	return 2;
    }

    /* can't use find_skill_by_name because we want skills the player knows
     * but can't use natively.
     */

    for (tmp=pl->inv; tmp!=NULL; tmp=tmp->below)
	if (tmp->type == SKILL && !strncasecmp(scroll->skill, tmp->skill, strlen(scroll->skill))) break;

    /* player already knows it */
    if (tmp && QUERY_FLAG(tmp, FLAG_CAN_USE_SKILL)) return 0;



    /* now a random change to learn, based on player Int.
     * give bonus based on level - otherwise stupid characters
     * might never be able to learn anything.
     */
    if(random_roll(0, 99, pl, PREFER_LOW)>(learn_spell[pl->stats.Int] + (pl->level/5)))
	return 2; /* failure :< */

    if (!tmp)
	tmp = give_skill_by_name(pl, scroll->skill);

    if (!tmp) {
	LOG(llevError,"skill scroll %s does not have valid skill name (%s).\n", scroll->name, scroll->skill);
	return 2;
    }

    SET_FLAG(tmp, FLAG_CAN_USE_SKILL);
    link_player_skills(pl);
    return 1;
}

/* Gives a percentage clipped to 0% -> 100% of a/b. */
/* Probably belongs in some global utils-type file? */
static int clipped_percent(sint64 a, sint64 b)
{
  int rv;

  if (b <= 0)
    return 0;

  rv = (int)((100.0f * ((float)a) / ((float)b) ) + 0.5f);
  
  if (rv < 0)
    return 0;
  else if (rv > 100)
    return 100;
  
  return rv;
}

/* show_skills() - Meant to allow players to examine
 * their current skill list.
 * This shows the amount of exp they have in the skills.
 * we also include some other non skill related info (god,
 * max weapon improvments, item power).
 * Note this function is a bit more complicated becauase we
 * we want ot sort the skills before printing them.  If we
 * just dumped this as we found it, this would be a bit
 * simpler.
 */
 
void show_skills(object *op, const char* search) {
    object *tmp=NULL;
    char buf[MAX_BUF];
    const char *cp;
    int i,num_skills_found=0;
    static const char *const periods = "........................................";
    /* Need to have a pointer and use strdup for qsort to work properly */
    char skills[NUM_SKILLS][MAX_BUF];


    for (tmp=op->inv; tmp!=NULL; tmp=tmp->below) {
	if (tmp->type == SKILL) {
		if ( search && strstr(tmp->name,search)==NULL ) continue;
	    /* Basically want to fill this out to 40 spaces with periods */
	    sprintf(buf,"%s%s", tmp->name, periods);
	    buf[40] = 0;

	    if (settings.permanent_exp_ratio) {
		sprintf(skills[num_skills_found++],"%slvl:%3d (xp:%" FMT64 "/%" FMT64 "/%d%%)",
			 buf,tmp->level,
			 tmp->stats.exp,
			 level_exp(tmp->level+1, op->expmul),
			 clipped_percent(tmp->perm_exp,tmp->stats.exp));
	    } else {
		sprintf(skills[num_skills_found++], "%slvl:%3d (xp:%" FMT64 "/%" FMT64 ")",
			 buf,tmp->level,
			 tmp->stats.exp,
			 level_exp(tmp->level+1, op->expmul));
	    }
	    /* I don't know why some characters get a bunch of skills, but
	     * it sometimes happens (maybe a leftover from bugier earlier code
	     * and those character are still about).  In any case, lets handle
	     * it so it doesn't crash the server - otherwise, one character may
	     * crash the server numerous times.
	     */
	    if (num_skills_found >= NUM_SKILLS) {
		draw_ext_info(NDI_RED, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
		      "Your character has too many skills. "
		      "Something isn't right - contact the server admin", NULL);
		break;
	    }
	}
    }

    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_LIST,
		  "Player skills:", NULL);

    if (num_skills_found > 1) qsort(skills, num_skills_found, MAX_BUF, (int (*)(const void*, const void*))strcmp);

    for (i=0; i<num_skills_found; i++) {
	draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_LIST,
		      skills[i], skills[i]);
    }

    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_LIST,
	"You can handle %d weapon improvements.",
	"You can handle %d weapon improvements.",
	 op->level/5+5);

    cp = determine_god(op);
    draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_LIST,
			 "You worship %s.", 
			 "You worship %s.", 
			 cp?cp:"no god at current time");

    draw_ext_info_format(NDI_UNIQUE,0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_LIST,
			 "Your equipped item power is %d out of %d\n",
			 "Your equipped item power is %d out of %d\n",
			 op->contr->item_power, op->level);
}

/* use_skill() - similar to invoke command, it executes the skill in the 
 * direction that the user is facing. Returns false if we are unable to 
 * change to the requested skill, or were unable to use the skill properly.
 * This is tricky because skills can have spaces.  We basically roll 
 * our own find_skill_by_name so we can try to do better string matching.
 */

int use_skill(object *op, const char *string) {
    object *skop;
    size_t len;

    if (!string) return 0;

    for (skop = op->inv; skop != NULL; skop=skop->below) {
	if (skop->type == SKILL && QUERY_FLAG(skop, FLAG_CAN_USE_SKILL) &&
	    !strncasecmp(string, skop->skill, MIN(strlen(string), strlen(skop->skill))))
	    break;
	else if (skop->type == SKILL_TOOL && 
	    !strncasecmp(string, skop->skill, MIN(strlen(string), strlen(skop->skill))))
	    break;
    }
    if (!skop) {
	draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
		     "Unable to find skill %s",
		     "Unable to find skill %s",
		     string);
	return 0;
    }

    len=strlen(skop->skill);

    /* All this logic goes and skips over the skill name to find any
     * options given to the skill.  Its pretty simple - if there
     * are extra parameters (as deteremined by string length), we
     * want to skip over any leading spaces.
     */
    if(len>=strlen(string)) {
	string=NULL;
    } else {
	string += len;
	while(*string==0x20) string++;
	if(strlen(string)==0) string = NULL;
    }
   
#ifdef SKILL_UTIL_DEBUG
    LOG(llevDebug,"use_skill() got skill: %s\n",sknum>-1?skills[sknum].name:"none");
#endif

    /* Change to the new skill, then execute it. */
    if(do_skill(op,op,skop, op->facing,string)) return 1;
 
    return 0;
}



/* This finds the best unarmed skill the player has, and returns
 * it.  Best can vary a little - we consider clawing to always
 * be the best for dragons.
 * This could be more intelligent, eg, look at the skill level
 * of the skill and go from there (eg, a level 40 puncher is
 * is probably better than level 1 karate).  OTOH, if you
 * don't bother to set up your skill properly, that is the players
 * problem (although, it might be nice to have a preferred skill
 * field the player can set.
 * Unlike the old code, we don't give out any skills - it is
 * possible you just don't have any ability to get into unarmed
 * combat.  If everyone race/class should have one, this should
 * be handled in the starting treasurelists, not in the code.
 */
static object *find_best_player_hth_skill(object *op)
{
    object *tmp, *best_skill=NULL;
    int	dragon = is_dragon_pl(op), last_skill=sizeof(unarmed_skills), i;

    /* Dragons are a special case - gros 25th July 2006 */
    if (dragon)
    {
        tmp = find_skill_by_number(op, SK_CLAWING);
        if (tmp) /* I suppose it should always be true - but maybe there's
                  * draconic toothache ? :) */
            return tmp;
    }
    for (tmp=op->inv; tmp; tmp=tmp->below) {
	if (tmp->type == SKILL) {

	    /* The order in the array is preferred order.  So basically,
	     * we just cut down the number to search - eg, if we find a skill
	     * early on in flame touch, then we only need to look into the unarmed_array
	     * to the entry before flame touch - don't care about the entries afterward,
	     * because they are infrerior skills.
	     * if we end up finding the best skill (i==0) might as well return
	     * right away - can't get any better than that.
	     */
	    for (i=0; i<last_skill; i++) {
		if (tmp->subtype == unarmed_skills[i] && QUERY_FLAG(tmp, FLAG_CAN_USE_SKILL)) {
		    best_skill = tmp;
		    last_skill = i;
		    if (i==0) return best_skill;
		}
	    }
	}
    }
    return best_skill;
}

/* do_skill_attack() - We have got an appropriate opponent from either
 * move_player_attack() or skill_attack(). In this part we get on with
 * attacking, take care of messages from the attack and changes in invisible.
 * Returns true if the attack damaged the opponent.
 * tmp is the targetted monster.
 * op is what is attacking
 * string is passed along to describe what messages to describe
 * the damage.
 */
 
static int do_skill_attack(object *tmp, object *op, const char *string, object *skill) {
    int success; 

   /* For Players only: if there is no ready weapon, and no "attack" skill
    * is readied either then try to find a skill for the player to use.
    * it is presumed that if skill is set, it is a valid attack skill (eg,
    * the caller should have set it appropriately).  We still want to pass
    * through that code if skill is set to change to the skill.
    */
    if(op->type==PLAYER) {
	if (!QUERY_FLAG(op,FLAG_READY_WEAPON)) {
	    size_t i;

	    if (!skill) {
		/* See if the players chosen skill is a combat skill, and use
		 * it if appropriate.
		 */
		if (op->chosen_skill) {
		    for (i=0; i<sizeof(unarmed_skills); i++)
			if (op->chosen_skill->subtype == unarmed_skills[i]) {
			    skill = op->chosen_skill;
			    break;
			}
		}
		/* If we didn't find a skill above, look harder for a good skill */
		if (!skill) {
		    skill = find_best_player_hth_skill(op);

		    if (!skill) {
			draw_ext_info(NDI_BLACK, 0, op,
				      MSG_TYPE_SKILL, MSG_TYPE_SKILL_MISSING,
				      "You have no unarmed combat skills!", NULL);
			return 0;
		    }
		}
	    }
	    if (skill != op->chosen_skill) {
		/* now try to ready the new skill */
		if(!change_skill(op,skill,0)) {  /* oh oh, trouble! */
		    draw_ext_info_format(NDI_UNIQUE, 0, tmp, 
					 MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
					 "Couldn't change to skill %s",
					 "Couldn't change to skill %s",
					 skill->name);
		    return 0;
		}
	    }
	} else {
	    /* Seen some crashes below where current_weapon is not set,
	     * even though the flag says it is.  So if current weapon isn't set,
	     * do some work in trying to find the object to use.
	     */
	    if (!op->current_weapon) {
		object *tmp;

		LOG(llevError,"Player %s does not have current weapon set but flag_ready_weapon is set\n",
		    op->name);
		for (tmp=op->inv; tmp; tmp=tmp->below)
		    if (tmp->type == WEAPON && QUERY_FLAG(tmp, FLAG_APPLIED)) break;

		if (op->current_weapon_script)
		    FREE_AND_CLEAR_STR(op->current_weapon_script);
		if (!tmp) {
		    LOG(llevError,"Could not find applied weapon on %s\n",
			op->name);
		    op->current_weapon=NULL;
		    return 0;
		} else {
		    op->current_weapon = tmp;
		    op->current_weapon_script=add_string(query_name(tmp));
		}
	    }

	    /* Has ready weapon - make sure chosen_skill is set up properly */
	    if (!op->chosen_skill || op->current_weapon->skill != op->chosen_skill->skill) {
		change_skill(op, find_skill_by_name(op, op->current_weapon->skill), 1);
	    }
	}
    }

    /* lose invisiblity/hiding status for running attacks */
 
    if(op->type==PLAYER && op->contr->tmp_invis) {
	op->contr->tmp_invis=0;
	op->invisible=0;
	op->hide=0;
	update_object(op,UP_OBJ_FACE);
    }
 
    success = attack_ob(tmp,op);
 
    /* print appropriate  messages to the player */
 
    if(success && string!=NULL && tmp && !QUERY_FLAG(tmp,FLAG_FREED)) {
	if(op->type==PLAYER)
	    draw_ext_info_format(NDI_UNIQUE, 0,op,
			 MSG_TYPE_ATTACK, MSG_TYPE_ATTACK_DID_HIT,
			 "You %s %s!",
			 "You %s %s!",
			 string,query_name(tmp));
	else if(tmp->type==PLAYER)
	    draw_ext_info_format(NDI_UNIQUE, 0,tmp,
			 MSG_TYPE_VICTIM, MSG_TYPE_VICTIM_WAS_HIT,
			 "%s %s you!",
			 "%s %s you!",
			 query_name(op),string);
    }
    return success;
}                         


/* skill_attack() - Core routine for use when we attack using a skills
 * system. In essence, this code handles
 * all skill-based attacks, ie hth, missile and melee weapons should be
 * treated here. If an opponent is already supplied by move_player(),
 * we move right onto do_skill_attack(), otherwise we find if an
 * appropriate opponent exists.
 *
 * This is called by move_player() and attack_hth()
 *
 * Initial implementation by -bt thomas@astro.psu.edu
 */
 
int skill_attack (object *tmp, object *pl, int dir, const char *string, object *skill) {
    sint16 tx,ty;
    mapstruct *m;
    int mflags;
 
    if(!dir) dir=pl->facing;
    tx=freearr_x[dir];
    ty=freearr_y[dir];
 
    /* If we don't yet have an opponent, find if one exists, and attack.
     * Legal opponents are the same as outlined in move_player_attack()
     */
 
    if(tmp==NULL) {
	m = pl->map;
	tx = pl->x + freearr_x[dir];
	ty = pl->y + freearr_y[dir];

	mflags = get_map_flags(m, &m, tx, ty, &tx, &ty);
	if (mflags & P_OUT_OF_MAP) return 0;

	/* space must be blocked for there to be anything interesting to do */
	if (!(mflags&P_IS_ALIVE)
	&& !OB_TYPE_MOVE_BLOCK(pl, GET_MAP_MOVE_BLOCK(m, tx, ty))) {
	    return 0;
	}

	for(tmp=get_map_ob(m, tx, ty); tmp; tmp=tmp->above)
	    if((QUERY_FLAG(tmp,FLAG_ALIVE) && tmp->stats.hp>=0)
	       || QUERY_FLAG(tmp, FLAG_CAN_ROLL)
	       || tmp->type==LOCKED_DOOR ) {
		/* Don't attack party members */
                if((pl->type==PLAYER && tmp->type==PLAYER) && (pl->contr->party!=NULL
                       && pl->contr->party==tmp->contr->party))
                                return 0;
                break;
	    }
    }
    if (!tmp) {
	if(pl->type==PLAYER)
	    draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_FAILURE,
			  "There is nothing to attack!", NULL);
	return 0;
    }

    return do_skill_attack(tmp,pl,string, skill);
}


/* attack_hth() - this handles all hand-to-hand attacks -b.t. */
/* July 5, 1995 - I broke up attack_hth() into 2 parts. In the first
 * (attack_hth) we check for weapon use, etc in the second (the new
 * function skill_attack() we actually attack.
 */

static int attack_hth(object *pl, int dir, const char *string, object *skill) {
    object *enemy=NULL,*weapon;

    if(QUERY_FLAG(pl, FLAG_READY_WEAPON))
	for(weapon=pl->inv;weapon;weapon=weapon->below) {
	    if (weapon->type==WEAPON && QUERY_FLAG(weapon, FLAG_APPLIED)) {
		if (apply_special(pl, weapon, AP_UNAPPLY | AP_NOPRINT)) {
		    draw_ext_info_format(NDI_UNIQUE, 0,pl,
				 MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
				  "You are unable to unwield %s in order to attack with %s.",
				  "You are unable to unwield %s in order to attack with %s.",
				  query_name(weapon), skill->name);
		    return 0;
		} else {
		    draw_ext_info(NDI_UNIQUE, 0,pl, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
				  "You unwield your weapon in order to attack.", NULL);
		    break;
		}
	    }
	}
    return skill_attack(enemy,pl,dir,string, skill);
}


/* attack_melee_weapon() - this handles melee weapon attacks -b.t.
 * For now we are just checking to see if we have a ready weapon here.
 * But there is a real neato possible feature of this scheme which
 * bears mentioning:
 * Since we are only calling this from do_skill() in the future
 * we may make this routine handle 'special' melee weapons attacks
 * (like disarming manuever with sai) based on player SK_level and
 * weapon type.
 */

static int attack_melee_weapon(object *op, int dir, const char *string, object *skill) {

    if(!QUERY_FLAG(op, FLAG_READY_WEAPON)) {
	if(op->type==PLAYER)
	    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_SKILL, MSG_TYPE_SKILL_ERROR,
			  "You have no ready weapon to attack with!", NULL);
	return 0;
    }
    return skill_attack(NULL,op,dir,string, skill);

}

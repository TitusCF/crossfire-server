/*
 * static char *rcsid_living_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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

#include <global.h>
#include <funcpoint.h>

/**
 * Handy little macro that adds exp and keeps it within bounds.  Since
 * we are now using 64 bit values, I'm not all concerned about overflow issues
 * with exptotal wrapping.  exptotal is typically op->exp, or op->perm_exp
 */
#define ADD_EXP(exptotal, exp) {exptotal += exp; if (exptotal > MAX_EXPERIENCE) exptotal = MAX_EXPERIENCE; }

static const int con_bonus[MAX_STAT + 1]={
  -6,-5,-4,-3,-2,-1,-1,0,0,0,0,1,2,3,4,5,6,7,8,9,10,12,14,16,18,20,
  22,25,30,40,50
};

/* changed the name of this to "sp_bonus" from "int_bonus" 
 * because Pow can now be the stat that controls spellpoint
 * advancement. -b.t.
 */
static const int sp_bonus[MAX_STAT + 1]={
  -10,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,12,15,20,25,
  30,40,50,70,100
};

static const int grace_bonus[MAX_STAT +1] = {
    -10,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,12,15,20,25,
  30,40,50,70,100
};

/**
 * 0.92.7 Changed way charisma works.  Values now 
 * represent how much more it costs to buy something than to sell it
 * (10, a value of 10 means it is that if it costs 50 gp to buy, you
 * would only get 5 gp when you sell.)  Let query_cost do the calculations
 * on how to really do this.  Buy keeping it this simple number, it is
 * much easier to know how things will be influenced.  A value of '1' means
 * buying and selling is both the same value - any value less than or equal
 * to 1 should not be used.  
 * At least as of now, the only place that uses this code is query_cost,
 * in server/shop.c.  This bonus is split evenly between buying and selling
 * (ie, if the bonus is 2.0, then items are bought for 1.33 list, and sold
 * at .667
 * This is figured by diff=(y-1)/(1+y), and for buy, it is 1+diff, for sell
 * it is 1-diff
 */
const float cha_bonus[MAX_STAT + 1]={10.0, 10.0, 9.0, 8.0, 7.0, 6.0, /*<-5*/
    5.0, 4.5, 4.0, 3.5, 3.0, /*<-10*/ 2.9, 2.8, 2.7, 2.6, 2.5, /*<-15*/
    2.4, 2.3, 2.2, 2.1, 2.0, /*<-20*/ 1.95, 1.90, 1.85, 1.80, 1.75, /*25 */
    1.70, 1.65, 1.60, 1.55, 1.50 /*30 */
};

const int dex_bonus[MAX_STAT + 1]={
  -4,-3,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,6,6,7
};

/** speed_bonus uses dex as its stat */
const float speed_bonus[MAX_STAT + 1]={
  -0.4, -0.4, -0.3, -0.3, -0.2, -0.2, -0.2, -0.1, -0.1, -0.1, -0.05, 0, 0, 0,
  0.05, 0.1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0, 1.2, 1.4,
  1.6, 1.8, 2.0, 2.5, 3.0
};

/**
 * dam_bonus, thaco_bonus, max_carry, weight limit all are based on
 * strength.
 */
const int dam_bonus[MAX_STAT + 1]={
  -2,-2,-2,-1,-1,-1,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,6,6,7,8,10,15
};

const int thaco_bonus[MAX_STAT + 1]={
  -2,-2,-1,-1,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,6,7,8,10
};

/* Max you can carry before you start getting extra speed penalties */
const int max_carry[MAX_STAT + 1]={
  2,4,7,11,16,22,29,37,46,56,67,79,92,106,121,137,154,172,191,211,232,254,277,
  301,326,352,400,450,500,600,1000
};

/**
 * weight_limit - the absolute most a character can carry - a character can't
 * pick stuff up if it would put him above this limit.
 * value is in grams, so we don't need to do conversion later
 * These limits are probably overly generous, but being there were no values
 * before, you need to start someplace.
 */

const uint32 weight_limit[MAX_STAT+ 1] = {
    200000,  /* 0 */
    250000,300000,350000,400000,500000,	    /* 5*/
    600000,700000,800000,900000,1000000,    /* 10 */
    1100000,1200000,1300000,1400000,1500000,/* 15 */
    1650000,1800000,1950000,2100000,2250000,/* 20 */
    2400000,2550000,2700000,2850000,3000000, /* 25 */
    3250000,3500000,3750000,4000000,4500000  /*30 */ 
};

const int learn_spell[MAX_STAT + 1]={
  0,0,0,1,2,4,8,12,16,25,36,45,55,65,70,75,80,85,90,95,100,100,100,100,100,
  100,100,100,100,100,100
};

const int cleric_chance[MAX_STAT + 1]={
  100,100,100,100,90,80,70,60,50,40,35,30,25,20,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,0
};

const int turn_bonus[MAX_STAT + 1]={
  -1,-1,-1,-1,-1,-1,-1,-1,0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,6,7,8,9,10,12,15
};

const int fear_bonus[MAX_STAT + 1]={
  3,3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/**
 * Since this is nowhere defined ...
 * Both come in handy at least in function add_exp()
*/
#define MAX_EXPERIENCE  levels[settings.max_level]

/**
 * Because exp_obj sum to make the total score,
 * we cannot allow that sum to exceed the maximum
 * amount of experience a player can gain. Thus
 * we define MAX_EXP_IN_OBJ. It is important to try
 * to make the value of MAX_EXP_CAT close to the
 * actual number of experience objects in the game,
 * otherwise the maximum level in any experience
 * category could be quite low. To help the situation
 * out a little I added 10 more levels, and jacked
 * up the last level experience value. Its out of 
 * line with progression of previous levels, so 
 * if more levels are desired, this should be fixed.
 *  -b.t.
 */
#define MAX_EXP_IN_OBJ levels[settings.max_level]/(MAX_EXP_CAT - 1) 

#ifndef WIN32
extern uint64 *levels;
#else
extern sint64 *levels;
#endif

#define MAX_SAVE_LEVEL 110
/* This no longer needs to be changed anytime the number of
 * levels is increased - rather, did_make_save will do the
 * right thing and always use range within this table.
 * for safety, savethrow should not be accessed directly anymore,
 * and instead did_make_save should be used instead.
 */
static const int savethrow[MAX_SAVE_LEVEL+1]={
  18,
  18,17,16,15,14,14,13,13,12,12,12,11,11,11,11,10,10,10,10, 9,
   9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6,
   6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

const char *const attacks[NROFATTACKS] = {
  "physical", "magical", "fire", "electricity", "cold", "confusion",
  "acid", "drain", "weaponmagic", "ghosthit", "poison", "slow",
  "paralyze", "turn undead", "fear", "cancellation", "depletion", "death",
  "chaos","counterspell","god power","holy power","blinding", "",
  "life stealing"
};

static const char *const drain_msg[NUM_STATS] = {
  "Oh no! You are weakened!",
  "You're feeling clumsy!",
  "You feel less healthy",
  "You suddenly begin to lose your memory!",
  "Your face gets distorted!",
  "Watch out, your mind is going!", 
  "Your spirit feels drained!"
};
const char *const restore_msg[NUM_STATS] = {
  "You feel your strength return.",
  "You feel your agility return.",
  "You feel your health return.",
  "You feel your wisdom return.",
  "You feel your charisma return.",
  "You feel your memory return.", 
  "You feel your spirits return."
};
const char *const gain_msg[NUM_STATS] = {
	"You feel stronger.",
	"You feel more agile.",
	"You feel healthy.",
	"You feel wiser.",
	"You seem to look better.",
	"You feel smarter.", 
	"You feel more potent."
};
const char *const lose_msg[NUM_STATS] = {
	"You feel weaker!",
	"You feel clumsy!",
	"You feel less healthy!",
	"You lose some of your memory!",
	"You look ugly!",
	"You feel stupid!", 
	"You feel less potent!"
};

const char *const statname[NUM_STATS] = {
  "strength", "dexterity", "constitution", "wisdom", "charisma", "intelligence","power" 
};

const char *const short_stat_name[NUM_STATS] = {
  "Str", "Dex", "Con", "Wis", "Cha", "Int","Pow" 
};

/**
 * Sets Str/Dex/con/Wis/Cha/Int/Pow in stats to value, depending on
 * what attr is (STR to POW).
 */

void set_attr_value(living *stats,int attr,sint8 value) {
  switch(attr) {
  case STR:
    stats->Str=value;
    break;
  case DEX:
    stats->Dex=value;
    break;
  case CON:
    stats->Con=value;
    break;
  case WIS:
    stats->Wis=value;
    break;
  case POW:
    stats->Pow=value;
    break;
  case CHA:
    stats->Cha=value;
    break;
  case INT:
    stats->Int=value;
    break;
  }
}

/**
 * Like set_attr_value(), but instead the value (which can be negative)
 * is added to the specified stat.
 */

void change_attr_value(living *stats,int attr,sint8 value) {
  if (value==0) return;
  switch(attr) {
  case STR:
    stats->Str+=value;
    break;
  case DEX:
    stats->Dex+=value;
    break;
  case CON:
    stats->Con+=value;
    break;
  case WIS:
    stats->Wis+=value;
    break;
  case POW:
    stats->Pow+=value;
    break;
  case CHA:
    stats->Cha+=value;
    break;
  case INT:
    stats->Int+=value;
    break;
  default:
	LOG(llevError,"Invalid attribute in change_attr_value: %d\n", attr);
  }
}

/**
 * Returns the specified stat.  See also set_attr_value().
 */

sint8 get_attr_value(const living *stats,int attr) {
  switch(attr) {
  case STR:
    return(stats->Str);
  case DEX:
    return(stats->Dex);
  case CON:
    return(stats->Con);
  case WIS:
    return(stats->Wis);
  case CHA:
    return(stats->Cha);
  case INT:
    return(stats->Int);
  case POW:
    return(stats->Pow);
  }
  return 0;
}

/**
 * Ensures that all stats (str/dex/con/wis/cha/int) are within the
 * 1-30 stat limit.
 */
void check_stat_bounds(living *stats) {
  int i,v;
  for(i=0;i<NUM_STATS;i++)
    if((v=get_attr_value(stats,i))>MAX_STAT)
      set_attr_value(stats,i,MAX_STAT);
    else if(v<MIN_STAT)
      set_attr_value(stats,i,MIN_STAT);
}

#define ORIG_S(xyz,abc)	(op->contr->orig_stats.abc)

/**
 * Rather than having a whole bunch of if (flag) draw.. else _draw,
 * make this macro to clean those up.  Not usuable outside change_abil
 * function since some of the values passed to draw_ext_info are hardcoded.
 */
#define DIFF_MSG(flag, subtype1, subtype2, msg1, msg2) \
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, (flag>0)?subtype1:subtype2, (flag>0)?msg1:msg2, NULL);

/**
 * return 1 if we sucessfully changed a stat, 0 if nothing was changed.
 * flag is set to 1 if we are applying the object, -1 if we are removing
 * the object.
 * It is the calling functions responsibilty to check to see if the object
 * can be applied or not.
 * The main purpose of calling this function is the messages that are
 * displayed - fix_object should really always be called after this when
 * removing an object - that is because it is impossible to know if some object
 * is the only source of an attacktype or spell attunement, so this function
 * will clear the bits, but the player may still have some other object
 * that gives them that ability.
 */
int change_abil(object *op, object *tmp) {
    int flag=QUERY_FLAG(tmp,FLAG_APPLIED)?1:-1,i,j,success=0;
    object refop;
    int potion_max=0;
  
    /* remember what object was like before it was changed.  note that
     * refop is a local copy of op only to be used for detecting changes
    * found by fix_object.  refop is not a real object 
     */
    memcpy(&refop, op, sizeof(object));

    if(op->type==PLAYER) {
	if (tmp->type==POTION) {
	    potion_max=1;
	    for(j=0;j<NUM_STATS;j++) {
		int nstat, ostat;

		ostat = get_attr_value(&(op->contr->orig_stats),j);
		i = get_attr_value(&(tmp->stats),j);

		/* nstat is what the stat will be after use of the potion */
		nstat = flag*i + ostat;

		/* Do some bounds checking.  While I don't think any
		 * potions do so right now, there is the potential for potions
		 * that adjust that stat by more than one point, so we need
		 * to allow for that.
		 */
		if (nstat < 1 && i*flag < 0 ) nstat = 1;
		else if (nstat > 20 + get_attr_value(&(op->arch->clone.stats),j)) {
		    nstat =  20 + get_attr_value(&(op->arch->clone.stats),j);
		}
		if (nstat != ostat) {
		    set_attr_value(&(op->contr->orig_stats), j, nstat);
		    potion_max=0;
		}
		else if (i) {
		    /* potion is useless - player has already hit the natural maximum */
		    potion_max = 1;
		}
	    }
	    /* This section of code ups the characters normal stats also.  I am not
         * sure if this is strictly necessary, being that fix_object probably
	     * recalculates this anyway.
	     */
	    for(j=0;j<NUM_STATS;j++)
		change_attr_value(&(op->stats),j,flag*get_attr_value(&(tmp->stats),j));
	    check_stat_bounds(&(op->stats));
	} /* end of potion handling code */
    }

    /* reset attributes that fix_object doesn't reset since it doesn't search
     * everything to set 
     */
    if(flag == -1) {
	op->attacktype&=~tmp->attacktype;
	op->path_attuned&=~tmp->path_attuned;
	op->path_repelled&=~tmp->path_repelled;
	op->path_denied&=~tmp->path_denied;
	/* Presuming here that creatures only have move_type,
	 * and not the other move_ fields.
	 */
	op->move_type &= ~tmp->move_type;
    }

    /* call fix_object since op object could have whatever attribute due
     * to multiple items.  if fix_object always has to be called after
     * change_ability then might as well call it from here
     */
    fix_object(op);

    /* Fix player won't add the bows ability to the player, so don't
     * print out message if this is a bow.
     */
    if(tmp->attacktype & AT_CONFUSION && tmp->type != BOW) {
	success=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_ATTACKTYPE_GAIN, MSG_TYPE_ATTRIBUTE_ATTACKTYPE_LOSS,
		 "Your hands begin to glow red.",
		 "Your hands stop glowing red.");
    }
    if ( QUERY_FLAG(op,FLAG_LIFESAVE) != QUERY_FLAG(&refop,FLAG_LIFESAVE)){
	success=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_PROTECTION_GAIN, MSG_TYPE_ATTRIBUTE_PROTECTION_LOSS,
		 "You feel very protected.", 
		 "You don't feel protected anymore.");
    }
    if ( QUERY_FLAG(op,FLAG_REFL_MISSILE) != QUERY_FLAG(&refop,FLAG_REFL_MISSILE)){
	success=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_PROTECTION_GAIN, MSG_TYPE_ATTRIBUTE_PROTECTION_LOSS,
		 "A magic force shimmers around you.",
		 "The magic force fades away.");
    }
    if ( QUERY_FLAG(op,FLAG_REFL_SPELL) != QUERY_FLAG(&refop,FLAG_REFL_SPELL)){
	success=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_PROTECTION_GAIN, MSG_TYPE_ATTRIBUTE_PROTECTION_LOSS,
		 "You feel more safe now, somehow.",
		 "Suddenly you feel less safe, somehow.");
    }
    /* movement type has changed.  We don't care about cases where
     * user has multiple items giving the same type appled like we
     * used to - that is more work than what we gain, plus messages
     * can be misleading (a little higher could be miscontrued from
     * from fly high)
     */
    if (tmp->move_type && op->move_type != refop.move_type) {
	success=1;

	/* MOVE_FLY_HIGH trumps MOVE_FLY_LOW - changing your move_fly_low
	 * status doesn't make a difference if you are flying high
	 */
	if (tmp->move_type & MOVE_FLY_LOW && !(op->move_type & MOVE_FLY_HIGH)) {
	    DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_MOVE, MSG_TYPE_ATTRIBUTE_MOVE, 
		     "You start to float in the air!.", 
		     "You float down to the ground.");
	}

	if (tmp->move_type & MOVE_FLY_HIGH) {
	    /* double conditional - second case covers if you have move_fly_low -
	     * in that case, you don't actually land
	     */
	    DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_MOVE, MSG_TYPE_ATTRIBUTE_MOVE,
		     "You soar into the air air!.", 
		     (op->move_type&MOVE_FLY_LOW ? "You fly lower in the air":
		      "You float down to the ground."));
	}
	if (tmp->move_type & MOVE_SWIM)
	    DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_MOVE, MSG_TYPE_ATTRIBUTE_MOVE,
		     "You feel ready for a swim",
		     "You no longer feel like swimming");

	/* Changing move status may mean you are affected by things you weren't before */
	check_move_on(op, op);
    }

    /* becoming UNDEAD... a special treatment for this flag. Only those not
     * originally undead may change their status 
     */ 
    if(!QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD)) 
	if ( QUERY_FLAG(op,FLAG_UNDEAD) != QUERY_FLAG(&refop,FLAG_UNDEAD)) {
	    success=1;
	    if(flag>0) {
		if(op->race) free_string(op->race); 
		op->race=add_string("undead");
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
			      "Your lifeforce drains away!", NULL);
	    } else {
		if(op->race) free_string(op->race); 
		if(op->arch->clone.race) 
		    op->race=add_string(op->arch->clone.race);  
		else
		    op->race = NULL;
		draw_ext_info(NDI_UNIQUE, 0, op,MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
			      "Your lifeforce returns!", NULL);
	    }
	}

    if ( QUERY_FLAG(op,FLAG_STEALTH) != QUERY_FLAG(&refop,FLAG_STEALTH)){
	success=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_START, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END,
		 "You walk more quietly.", 
		 "You walk more noisily.");
    }
    if ( QUERY_FLAG(op,FLAG_MAKE_INVIS) != QUERY_FLAG(&refop,FLAG_MAKE_INVIS)){
	success=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_START, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END,
		 "You become transparent.", 
		 "You can see yourself.");
    }
    /* blinded you can tell if more blinded since blinded player has minimal
     * vision 
     */
    if(QUERY_FLAG(tmp,FLAG_BLIND)) {
	success=1;
	if(flag>0) {
	    if(QUERY_FLAG(op,FLAG_WIZ))
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START,
			      "Your mortal self is blinded.", NULL);
	    else { 
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START,
			      "You are blinded.", NULL);
		SET_FLAG(op,FLAG_BLIND);
		if(op->type==PLAYER)
		    op->contr->do_los=1;
	    }
	} else {
	    if(QUERY_FLAG(op,FLAG_WIZ))
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			      "Your mortal self can now see again.", NULL);
	    else {
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END,
			      "Your vision returns.", NULL);
		CLEAR_FLAG(op,FLAG_BLIND);
		if(op->type==PLAYER)
		    op->contr->do_los=1;
	    }
	}
    }

    if ( QUERY_FLAG(op,FLAG_SEE_IN_DARK) != QUERY_FLAG(&refop,FLAG_SEE_IN_DARK)){
	success=1;
        if(op->type==PLAYER)
	    op->contr->do_los=1;
	DIFF_MSG(flag, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_START, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END,
		 "Your vision is better in the dark.", 
		 "You see less well in the dark.");
    }

    if ( QUERY_FLAG(op,FLAG_XRAYS) != QUERY_FLAG(&refop,FLAG_XRAYS)){
	success=1;
	if(flag>0) {
	    if(QUERY_FLAG(op,FLAG_WIZ))
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_START,
			      "Your vision becomes a little clearer.", NULL);
	    else {
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_START,
			      "Everything becomes transparent.", NULL);
		if(op->type==PLAYER)
		    op->contr->do_los=1;
	    }
	} else {
	    if(QUERY_FLAG(op,FLAG_WIZ))
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END,
			      "Your vision becomes a bit out of focus.", NULL);
	    else {
		draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END,
			      "Everything suddenly looks very solid.", NULL);
		if(op->type==PLAYER)
		    op->contr->do_los=1;
	    }
	}
    }

    if(tmp->stats.luck) {
	success=1;
	DIFF_MSG(flag*tmp->stats.luck, MSG_TYPE_ATTRIBUTE_STAT_GAIN, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
		 "You feel more lucky.",
		 "You feel less lucky.");
    }

    if(tmp->stats.hp && op->type==PLAYER) {
	success=1;
	DIFF_MSG(flag*tmp->stats.hp, MSG_TYPE_ATTRIBUTE_STAT_GAIN, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
		 "You feel much more healthy!",
		 "You feel much less healthy!");
    }

    if(tmp->stats.sp && op->type==PLAYER && tmp->type!=SKILL) {
	success=1;
	DIFF_MSG(flag*tmp->stats.sp, MSG_TYPE_ATTRIBUTE_STAT_GAIN, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
		 "You feel one with the powers of magic!",
		 "You suddenly feel very mundane.");
    }

    /* for the future when artifacts set this -b.t. */
    if(tmp->stats.grace && op->type==PLAYER) {
	success=1;
	DIFF_MSG(flag*tmp->stats.grace, MSG_TYPE_ATTRIBUTE_STAT_GAIN, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
		 "You feel closer to your god!", 
		 "You suddenly feel less holy.");
    }

    if(tmp->stats.food && op->type==PLAYER) {
	success=1;
	DIFF_MSG(flag*tmp->stats.food, MSG_TYPE_ATTRIBUTE_STAT_GAIN, MSG_TYPE_ATTRIBUTE_STAT_LOSS,
		 "You feel your digestion slowing down.", 
		 "You feel your digestion speeding up.");
    }

    /* Messages for changed resistance */
    for (i=0; i<NROFATTACKS; i++) {
	if (i==ATNR_PHYSICAL) continue;	/* Don't display about armour */
    
	if (op->resist[i] != refop.resist[i]) {
	    success=1;
	    if (op->resist[i] > refop.resist[i])
		draw_ext_info_format(NDI_UNIQUE|NDI_BLUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_PROTECTION_GAIN,
			"Your resistance to %s rises to %d%%.",
			"Your resistance to %s rises to %d%%.",
			change_resist_msg[i], op->resist[i]);
	    else
		draw_ext_info_format(NDI_UNIQUE|NDI_BLUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_PROTECTION_LOSS,
			"Your resistance to %s drops to %d%%.",
			"Your resistance to %s drops to %d%%.",
			change_resist_msg[i], op->resist[i]);
	}
    }

    if(tmp->type!=EXPERIENCE && !potion_max) {
	for (j=0; j<NUM_STATS; j++) {
	    if ((i=get_attr_value(&(tmp->stats),j))!=0) {
		success=1;
		DIFF_MSG(i * flag, MSG_TYPE_ATTRIBUTE_STAT_GAIN, MSG_TYPE_ATTRIBUTE_STAT_LOSS, gain_msg[j], lose_msg[j]);
	    }
	}
    }
    return success;
}

/**
 * Stat draining by Vick 930307
 * (Feeling evil, I made it work as well now.  -Frank 8)
 */
void drain_stat(object *op) {
    drain_specific_stat(op, RANDOM()%NUM_STATS);
}

void drain_specific_stat(object *op, int deplete_stats) {
    object *tmp;
    archetype *at;

    at = find_archetype(ARCH_DEPLETION);
    if (!at) {
	LOG(llevError, "Couldn't find archetype depletion.\n");
	return;
    } else {
	tmp = present_arch_in_ob(at, op);
	if (!tmp) {
	    tmp = arch_to_object(at);
	    tmp = insert_ob_in_ob(tmp, op);
	    SET_FLAG(tmp,FLAG_APPLIED);
	}
    }

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_STAT_LOSS, drain_msg[deplete_stats], NULL);
    change_attr_value(&tmp->stats, deplete_stats, -1);
    fix_object(op);
}

/**
 * A value of 0 indicates timeout, otherwise change the luck of the object.
 * via an applied bad_luck object.
 */
void change_luck(object *op, int value) {
  object *tmp;
  archetype *at;
  int new_luck;

  at = find_archetype("luck");
  if (!at)
    LOG(llevError, "Couldn't find archetype luck.\n");
  else {
    tmp = present_arch_in_ob(at, op);
    if (!tmp) {
      if (!value)
        return;
      tmp = arch_to_object(at);
      tmp = insert_ob_in_ob(tmp, op);
      SET_FLAG(tmp,FLAG_APPLIED);
    }
    if (value) {
      /* Limit the luck value of the bad luck object to +/-100. This
       * (arbitrary) value prevents overflows (both in the bad luck object and
       * in op itself).
       */
      new_luck = tmp->stats.luck+value;
      if (new_luck >= -100 && new_luck <= 100) {
      op->stats.luck+=value;
	tmp->stats.luck = new_luck;
      }
    } else {
      if (!tmp->stats.luck) {
        return;
      }
      /* Randomly change the players luck.  Basically, we move it
       * back neutral (if greater>0, subtract, otherwise add)
       */
      if (RANDOM()%(FABS(tmp->stats.luck)) >= RANDOM()%30) {
        int diff = tmp->stats.luck>0?-1:1;
        op->stats.luck += diff;
        tmp->stats.luck += diff;
      }
    }
  }
}

/**
 * Subtracts stat-bonuses given by the class which the player has chosen.
 */
void remove_statbonus(object *op) {
  op->stats.Str -= op->arch->clone.stats.Str;
  op->stats.Dex -= op->arch->clone.stats.Dex;
  op->stats.Con -= op->arch->clone.stats.Con;
  op->stats.Wis -= op->arch->clone.stats.Wis;
  op->stats.Pow -= op->arch->clone.stats.Pow;
  op->stats.Cha -= op->arch->clone.stats.Cha;
  op->stats.Int -= op->arch->clone.stats.Int;
  op->contr->orig_stats.Str -= op->arch->clone.stats.Str;
  op->contr->orig_stats.Dex -= op->arch->clone.stats.Dex;
  op->contr->orig_stats.Con -= op->arch->clone.stats.Con;
  op->contr->orig_stats.Wis -= op->arch->clone.stats.Wis;
  op->contr->orig_stats.Pow -= op->arch->clone.stats.Pow;
  op->contr->orig_stats.Cha -= op->arch->clone.stats.Cha;
  op->contr->orig_stats.Int -= op->arch->clone.stats.Int;
}

/**
 * Adds stat-bonuses given by the class which the player has chosen.
 */
void add_statbonus(object *op) {
  op->stats.Str += op->arch->clone.stats.Str;
  op->stats.Dex += op->arch->clone.stats.Dex;
  op->stats.Con += op->arch->clone.stats.Con;
  op->stats.Wis += op->arch->clone.stats.Wis;
  op->stats.Pow += op->arch->clone.stats.Pow;
  op->stats.Cha += op->arch->clone.stats.Cha;
  op->stats.Int += op->arch->clone.stats.Int;
  op->contr->orig_stats.Str += op->arch->clone.stats.Str;
  op->contr->orig_stats.Dex += op->arch->clone.stats.Dex;
  op->contr->orig_stats.Con += op->arch->clone.stats.Con;
  op->contr->orig_stats.Wis += op->arch->clone.stats.Wis;
  op->contr->orig_stats.Pow += op->arch->clone.stats.Pow;
  op->contr->orig_stats.Cha += op->arch->clone.stats.Cha;
  op->contr->orig_stats.Int += op->arch->clone.stats.Int;
}

/**
 * Updates all abilities given by applied objects in the inventory
 * of the given object.
 *
 * This functions starts from base values (archetype or player object)
 * and then adjusts them according to what the player/monster has equipped.
 *
 * July 95 - inserted stuff to handle new skills/exp system - b.t.
 * spell system split, grace points now added to system  --peterm
 *
 * Note that a player always has stats reset to their initial value.
 */
void fix_object(object *op) {
    int i,j;
    float f,max=9,added_speed=0,bonus_speed=0, sp_tmp,speed_reduce_from_disease=1;
    int weapon_weight=0,weapon_speed=0;
    int best_wc=0, best_ac=0, wc=0, ac=0;
    int prot[NROFATTACKS], vuln[NROFATTACKS], potion_resist[NROFATTACKS];
    object *grace_obj=NULL,*mana_obj=NULL,*wc_obj=NULL,*tmp;

    /* First task is to clear all the values back to their original values */
    if(op->type==PLAYER) {
        for(i=0;i<NUM_STATS;i++) {
            set_attr_value(&(op->stats),i,get_attr_value(&(op->contr->orig_stats),i));
        }
        if (settings.spell_encumbrance == TRUE)
            op->contr->encumbrance=0;

        op->attacktype=0;
        op->contr->digestion = 0;
        op->contr->gen_hp = 0;
        op->contr->gen_sp = 0;
        op->contr->gen_grace = 0;
        op->contr->gen_sp_armour = 10;
        op->contr->item_power = 0;

        /* Don't clobber all the range_ values.  range_golem otherwise
         * gets reset for no good reason, and we don't want to reset
         * range_magic (what spell is readied).  These three below
         * well get filled in based on what the player has equipped.
         */
        op->contr->ranges[range_bow] = NULL;
        op->contr->ranges[range_misc] = NULL;
        op->contr->ranges[range_skill] = NULL;
    } /* If player */
    memcpy(op->body_used, op->body_info, sizeof(op->body_info));

    if(op->slaying!=NULL) {
        free_string(op->slaying);
        op->slaying=NULL;
    }
    if(!QUERY_FLAG(op,FLAG_WIZ)) {
        CLEAR_FLAG(op, FLAG_XRAYS);
        CLEAR_FLAG(op, FLAG_MAKE_INVIS);
    }

    CLEAR_FLAG(op,FLAG_LIFESAVE);
    CLEAR_FLAG(op,FLAG_STEALTH);
    CLEAR_FLAG(op,FLAG_BLIND);
    if (!QUERY_FLAG (&op->arch->clone, FLAG_REFL_SPELL))
        CLEAR_FLAG(op,FLAG_REFL_SPELL);
    if (!QUERY_FLAG (&op->arch->clone, FLAG_REFL_MISSILE))
        CLEAR_FLAG(op,FLAG_REFL_MISSILE);
    if(!QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD))
        CLEAR_FLAG(op,FLAG_UNDEAD);
    if (!QUERY_FLAG (&op->arch->clone, FLAG_SEE_IN_DARK))
        CLEAR_FLAG(op,FLAG_SEE_IN_DARK);

    op->path_attuned=op->arch->clone.path_attuned;
    op->path_repelled=op->arch->clone.path_repelled;
    op->path_denied=op->arch->clone.path_denied;
    op->glow_radius=op->arch->clone.glow_radius;
    op->move_type = op->arch->clone.move_type;
    op->chosen_skill = NULL;

    /* initializing resistances from the values in player/monster's
     * archetype clone
     */
    memcpy(&op->resist, &op->arch->clone.resist, sizeof(op->resist));

    for (i=0;i<NROFATTACKS;i++) {
        if (op->resist[i] > 0)
            prot[i]= op->resist[i], vuln[i]=0;
        else
            vuln[i]= -(op->resist[i]), prot[i]=0;
        potion_resist[i]=0;
    }

    wc=op->arch->clone.stats.wc;
    op->stats.dam=op->arch->clone.stats.dam;

    /* for players which cannot use armour, they gain AC -1 per 3 levels,
     * plus a small amount of physical resist, those poor suckers. ;) 
     * the fact that maxlevel is factored in could be considered sort of bogus -
     * we should probably give them some bonus and cap it off - otherwise,
     * basically, if a server updates its max level, these playes may find
     * that their protection from physical goes down
     */
    if(!QUERY_FLAG(op,FLAG_USE_ARMOUR) && op->type==PLAYER) {
        ac=MAX(-10,op->arch->clone.stats.ac - op->level/3);
        prot[ATNR_PHYSICAL] += ((100-prot[AT_PHYSICAL])*(80*op->level/settings.max_level))/100;
    } else
        ac=op->arch->clone.stats.ac;

    op->stats.luck=op->arch->clone.stats.luck;
    op->speed = op->arch->clone.speed;

    /* OK - we've reset most all the objects attributes to sane values.
     * now go through and make adjustments for what the player has equipped.
     */

    for(tmp=op->inv;tmp!=NULL;tmp=tmp->below) {
        /* See note in map.c:update_position about making this additive 
         * since light sources are never applied, need to put check here.
         */
        if (tmp->glow_radius > op->glow_radius)
            op->glow_radius=tmp->glow_radius;

        /* This happens because apply_potion calls change_abil with the potion
         * applied so we can tell the player what chagned.  But change_abil
         * then calls this function.
         */
        if (QUERY_FLAG(tmp, FLAG_APPLIED) && tmp->type == POTION)
            continue;

        /* For some things, we don't care what is equipped */
        if (tmp->type == SKILL) {
            /* Want to take the highest skill here. */
            if (IS_MANA_SKILL(tmp->subtype)) {
                if (!mana_obj)
                    mana_obj = tmp;
                else if (tmp->level > mana_obj->level)
                    mana_obj = tmp;
            }
            if (IS_GRACE_SKILL(tmp->subtype)) {
                if (!grace_obj)
                    grace_obj=tmp;
                else if (tmp->level > grace_obj->level)
                    grace_obj = tmp;
            }
        }

        /* Container objects are not meant to adjust a players, but other applied
         * objects need to make adjustments.
         * This block should handle all player specific changes 
         * The check for Praying is a bit of a hack - god given bonuses are put
         * in the praying skill, and the player should always get those.
         * It also means we need to put in additional checks for applied below,	
         * because the skill shouldn't count against body positions being used
         * up, etc.
         */
        if ((QUERY_FLAG(tmp,FLAG_APPLIED) && tmp->type!=CONTAINER && tmp->type!=CLOSE_CON) ||
            (tmp->type == SKILL && tmp->subtype == SK_PRAYING)) {
            if(op->type==PLAYER) {
                if (tmp->type == BOW) 
                    op->contr->ranges[range_bow] = tmp;

                if (tmp->type == WAND || tmp->type == ROD || tmp->type==HORN) 
                    op->contr->ranges[range_misc] = tmp;

                for(i=0;i<NUM_STATS;i++)
                    change_attr_value(&(op->stats),i,get_attr_value(&(tmp->stats),i));

                /* these are the items that currently can change digestion, regeneration,
                 * spell point recovery and mana point recovery.  Seems sort of an arbitary
                 * list, but other items store other info into stats array. 
                 */
                if ((tmp->type == EXPERIENCE)  || (tmp->type == WEAPON) ||
                     (tmp->type == ARMOUR)   || (tmp->type == HELMET) ||
                     (tmp->type == SHIELD)   || (tmp->type == RING) ||
                     (tmp->type == BOOTS)    || (tmp->type == GLOVES) ||
                     (tmp->type == AMULET )  || (tmp->type == GIRDLE) ||
                     (tmp->type == BRACERS ) || (tmp->type == CLOAK) ||
                     (tmp->type == DISEASE)  || (tmp->type == FORCE) ||
                     (tmp->type == SKILL)) {
                    op->contr->digestion    += tmp->stats.food;
                    op->contr->gen_hp       += tmp->stats.hp;
                    op->contr->gen_sp       += tmp->stats.sp;
                    op->contr->gen_grace    += tmp->stats.grace;
                    op->contr->gen_sp_armour+= tmp->gen_sp_armour;
                    op->contr->item_power	+= tmp->item_power;
                     }
            } /* if this is a player */

            /* Update slots used for items */
            if (QUERY_FLAG(tmp,FLAG_APPLIED)) {
                for (i=0; i<NUM_BODY_LOCATIONS; i++)
                    op->body_used[i] += tmp->body_info[i];
            }

            if(tmp->type==SYMPTOM) {
                speed_reduce_from_disease = tmp->last_sp / 100.0;
                if(speed_reduce_from_disease ==0)
                    speed_reduce_from_disease = 1;
            }

            /* Pos. and neg. protections are counted seperate (-> pro/vuln).
             * (Negative protections are calculated extactly like positive.)
             * Resistance from potions are treated special as well. If there's
             * more than one potion-effect, the bigger prot.-value is taken. 
             */
            if (tmp->type != POTION) {
                for (i=0; i<NROFATTACKS; i++) {
                    /* Potential for cursed potions, in which case we just can use
                     * a straight MAX, as potion_resist is initialized to zero.
                     */
                    if (tmp->type==POTION_EFFECT) {
                        if (potion_resist[i])
                            potion_resist[i] = MAX(potion_resist[i], tmp->resist[i]);
                        else
                            potion_resist[i] = tmp->resist[i];
                    }
                    else if (tmp->resist[i] > 0) 
                        prot[i] += ((100-prot[i])*tmp->resist[i])/100;
                    else if (tmp->resist[i] < 0)
                        vuln[i] += ((100-vuln[i])*(-tmp->resist[i]))/100;
                }
            }

            /* There may be other things that should not adjust the attacktype */
            if (tmp->type!=BOW && tmp->type != SYMPTOM)
                op->attacktype|=tmp->attacktype;

            op->path_attuned|=tmp->path_attuned;
            op->path_repelled|=tmp->path_repelled;
            op->path_denied|=tmp->path_denied;
            op->stats.luck+=tmp->stats.luck;
            op->move_type |= tmp->move_type;

            if(QUERY_FLAG(tmp,FLAG_LIFESAVE))
                SET_FLAG(op,FLAG_LIFESAVE);
            if(QUERY_FLAG(tmp,FLAG_REFL_SPELL))
                SET_FLAG(op,FLAG_REFL_SPELL);
            if(QUERY_FLAG(tmp,FLAG_REFL_MISSILE))
                SET_FLAG(op,FLAG_REFL_MISSILE);
            if(QUERY_FLAG(tmp,FLAG_STEALTH))
                SET_FLAG(op,FLAG_STEALTH);
            if(QUERY_FLAG(tmp,FLAG_XRAYS))
                SET_FLAG(op,FLAG_XRAYS);
            if(QUERY_FLAG(tmp,FLAG_BLIND))
                SET_FLAG(op,FLAG_BLIND);
            if(QUERY_FLAG(tmp,FLAG_SEE_IN_DARK))
                SET_FLAG(op,FLAG_SEE_IN_DARK);

            if(QUERY_FLAG(tmp,FLAG_UNDEAD) && !QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD))
                SET_FLAG(op,FLAG_UNDEAD);

            if(QUERY_FLAG(tmp,FLAG_MAKE_INVIS)) {
                SET_FLAG(op,FLAG_MAKE_INVIS); 
                op->invisible=1;
            }

            if(tmp->stats.exp && tmp->type!=SKILL) {
                if(tmp->stats.exp > 0) {
                    added_speed+=(float)tmp->stats.exp/3.0;
                    bonus_speed+=1.0+(float)tmp->stats.exp/3.0;
                } else
                    added_speed+=(float)tmp->stats.exp;
            }

            switch(tmp->type) {
                /* skills modifying the character -b.t. */ 
                /* for all skills and skill granting objects */ 
                case SKILL:
                    if (!QUERY_FLAG(tmp,FLAG_APPLIED))
                        break;

                    if (IS_COMBAT_SKILL(tmp->subtype))
                        wc_obj=tmp;

                    if (op->chosen_skill) {
                        LOG(llevDebug, "fix_object, op %s has multiple skills applied\n", op->name);
                    }
                    op->chosen_skill = tmp;
                    if(tmp->stats.dam>0) { 	/* skill is a 'weapon' */ 
                        if(!QUERY_FLAG(op,FLAG_READY_WEAPON)) 
                            weapon_speed = (int) WEAPON_SPEED(tmp);
                        if(weapon_speed<0)
                            weapon_speed = 0;
                        weapon_weight=tmp->weight;
                        op->stats.dam+=tmp->stats.dam*(1 + (op->chosen_skill->level/9));
                        if(tmp->magic)
                            op->stats.dam += tmp->magic;
                    }
                    if(tmp->stats.wc)
                        wc-=(tmp->stats.wc+tmp->magic);

                    if(tmp->slaying!=NULL) {
                        if (op->slaying != NULL)
                            free_string (op->slaying);
                        add_refcount(op->slaying = tmp->slaying);
                    }

                    if(tmp->stats.ac)
                        ac-=(tmp->stats.ac+tmp->magic);
                    if(settings.spell_encumbrance == TRUE && op->type==PLAYER)
                        op->contr->encumbrance+=(int)3*tmp->weight/1000;
                    if (op->type == PLAYER)
                        op->contr->ranges[range_skill] = op;
                    break;

                case SKILL_TOOL:
                    if (op->chosen_skill) {
                        LOG(llevDebug, "fix_object, op %s has multiple skills applied\n", op->name);
                    }
                    op->chosen_skill = tmp;
                    if (op->type == PLAYER)
                        op->contr->ranges[range_skill] = op;
                    break;

                case SHIELD:
                    if(settings.spell_encumbrance == TRUE && op->type==PLAYER)
                        op->contr->encumbrance+=(int)tmp->weight/2000;
                case RING:
                case AMULET:
                case GIRDLE:
                case HELMET:
                case BOOTS:
                case GLOVES:
                case CLOAK:
                    if(tmp->stats.wc)
                        wc-=(tmp->stats.wc+tmp->magic);
                    if(tmp->stats.dam)
                        op->stats.dam+=(tmp->stats.dam+tmp->magic);
                    if(tmp->stats.ac)
                        ac-=(tmp->stats.ac+tmp->magic);
                    break;

                case WEAPON:
                    wc-=(tmp->stats.wc+tmp->magic);
                    if(tmp->stats.ac&&tmp->stats.ac+tmp->magic>0)
                        ac-=tmp->stats.ac+tmp->magic;
                    op->stats.dam+=(tmp->stats.dam+tmp->magic);
                    weapon_weight=tmp->weight;
                    weapon_speed=((int)WEAPON_SPEED(tmp)*2-tmp->magic)/2;
                    if(weapon_speed<0)
                        weapon_speed=0;
                    if(tmp->slaying!=NULL) {
                        if (op->slaying != NULL)
                            free_string (op->slaying);
                        add_refcount(op->slaying = tmp->slaying);
                    }
                    /* If there is desire that two handed weapons should do
                     * extra strength damage, this is where the code should
                     * go.
                     */
                    op->current_weapon = tmp;
                    if(settings.spell_encumbrance == TRUE && op->type==PLAYER)
                        op->contr->encumbrance+=(int)3*tmp->weight/1000;
                    break;

                    case ARMOUR: /* Only the best of these three are used: */
                        if(settings.spell_encumbrance == TRUE && op->type==PLAYER)
                            op->contr->encumbrance+=(int)tmp->weight/1000;

                case BRACERS:
                case FORCE:
                    if(tmp->stats.wc) { 
                        if(best_wc<tmp->stats.wc+tmp->magic) {
                            wc+=best_wc;
                            best_wc=tmp->stats.wc+tmp->magic;
                        } else
                            wc+=tmp->stats.wc+tmp->magic;
                    }
                    if(tmp->stats.ac) {
                        if(best_ac<tmp->stats.ac+tmp->magic) {
                            ac+=best_ac; /* Remove last bonus */
                            best_ac=tmp->stats.ac+tmp->magic;
                        }
                        else /* To nullify the below effect */
                            ac+=tmp->stats.ac+tmp->magic;
                    }
                    if(tmp->stats.wc)
                        wc-=(tmp->stats.wc+tmp->magic);
                    if(tmp->stats.ac)
                        ac-=(tmp->stats.ac+tmp->magic);
                    if(ARMOUR_SPEED(tmp) && ARMOUR_SPEED(tmp)/10.0<max)
                        max=ARMOUR_SPEED(tmp)/10.0;
                    break;
            } /* switch tmp->type */
            } /* item is equipped */
    } /* for loop of items */

    /* We've gone through all the objects the player has equipped.  For many things, we
     * have generated intermediate values which we now need to assign.
     */

    /* 'total resistance = total protections - total vulnerabilities'.
     * If there is an uncursed potion in effect, granting more protection
     * than that, we take: 'total resistance = resistance from potion'.
     * If there is a cursed (and no uncursed) potion in effect, we take
     * 'total resistance = vulnerability from cursed potion'. 
     */
    for (i=0; i<NROFATTACKS; i++) {
        op->resist[i] = prot[i] - vuln[i];
        if (potion_resist[i] && ((potion_resist[i] > op->resist[i]) ||
            (potion_resist[i] < 0)))
            op->resist[i] = potion_resist[i];
    }

    /* Figure out the players sp/mana/hp totals. */
    if(op->type==PLAYER) {
        int pl_level;

        check_stat_bounds(&(op->stats));
        pl_level=op->level;

        if(pl_level<1)
            pl_level=1; /* safety, we should always get 1 levels worth of hp! */ 

        /* You basically get half a con bonus/level.  But we do take into account rounding,
         * so if your bonus is 7, you still get 7 worth of bonus every 2 levels.
         */
        for(i=1,op->stats.maxhp=0;i<=pl_level&&i<=10;i++) {
            j = op->contr->levhp[i] + con_bonus[op->stats.Con] / 2;
            if(i%2 && con_bonus[op->stats.Con]%2) {
                if (con_bonus[op->stats.Con]>0) 
                    j++;
                else
                    j--;
            }
            op->stats.maxhp+=j>1?j:1;	/* always get at least 1 hp/level */
        }

        for(i=11;i<=op->level;i++)
            op->stats.maxhp+=2;

        if(op->stats.hp>op->stats.maxhp)
            op->stats.hp=op->stats.maxhp;

        /* Sp gain is controlled by the level of the player's 
         * relevant experience object (mana_obj, see above) 
         */ 
        /* following happen when skills system is not used */
        if(!mana_obj)
            mana_obj = op;
        if(!grace_obj)
            grace_obj = op;

        /* set maxsp */
        if(!mana_obj || !mana_obj->level || op->type!=PLAYER)
            mana_obj = op;

        if (mana_obj == op && op->type == PLAYER) {
            op->stats.maxsp = 1;
        } else {
            sp_tmp=0.0;
            for(i=1;i<=mana_obj->level&&i<=10;i++) {
                float stmp;

                /* Got some extra bonus at first level */
                if(i<2) {
                    stmp = op->contr->levsp[i] +((2.0 * (float)sp_bonus[op->stats.Pow] + 
                        (float)sp_bonus[op->stats.Int])/6.0); 
                } else {
                    stmp=(float)op->contr->levsp[i]
                        +(2.0 * (float)sp_bonus[op->stats.Pow] + 
                        (float)sp_bonus[op->stats.Int])/12.0;
                }
                if (stmp<1.0)
                    stmp=1.0;
                sp_tmp+=stmp;
            }
            op->stats.maxsp=(int)sp_tmp;

            for(i=11;i<=mana_obj->level;i++)
                op->stats.maxsp+=2;
        }

        /* Characters can get their sp supercharged via rune of transferrance */
        if(op->stats.sp>op->stats.maxsp*2)
            op->stats.sp=op->stats.maxsp*2;

        /* set maxgrace, notice 3-4 lines below it depends on both Wis and Pow */
        if(!grace_obj || !grace_obj->level || op->type!=PLAYER)
            grace_obj = op;

        if (grace_obj == op && op->type == PLAYER) {
            op->stats.maxgrace = 1;
        } else {
            /* store grace in a float - this way, the divisions below don't create
             * big jumps when you go from level to level - with int's, it then
             * becomes big jumps when the sums of the bonuses jump to the next
             * step of 8 - with floats, even fractional ones are useful.
             */
            sp_tmp=0.0;
            for(i=1,op->stats.maxgrace=0;i<=grace_obj->level&&i<=10;i++) {
                float grace_tmp=0.0;

                /* Got some extra bonus at first level */
                if(i<2) {
                    grace_tmp = op->contr->levgrace[i]+(((float)grace_bonus[op->stats.Pow] + 
                        2.0 * (float)grace_bonus[op->stats.Wis])/6.0); 
                } else {
                    grace_tmp=(float)op->contr->levgrace[i]
                        +((float)grace_bonus[op->stats.Pow] + 
                        2.0 * (float)grace_bonus[op->stats.Wis])/12.0;
                }
                if (grace_tmp<1.0)
                    grace_tmp=1.0;
                sp_tmp+=grace_tmp;
            }
            op->stats.maxgrace=(int)sp_tmp;

            /* two grace points per level after 11 */
            for(i=11;i<=grace_obj->level;i++)
                op->stats.maxgrace+=2;
        }
        /* No limit on grace vs maxgrace */

        if(op->contr->braced) {
            ac+=2;
            wc+=4;
        }
        else
            ac-=dex_bonus[op->stats.Dex];

        /* In new exp/skills system, wc bonuses are related to 
         * the players level in a relevant exp object (wc_obj)
         * not the general player level -b.t. 
         * I changed this slightly so that wc bonuses are better
         * than before. This is to balance out the fact that 
         * the player no longer gets a personal weapon w/ 1
         * improvement every level, now its fighterlevel/5. So
         * we give the player a bonus here in wc and dam
         * to make up for the change. Note that I left the 
         * monster bonus the same as before. -b.t.
         */

        if(op->type==PLAYER && wc_obj && wc_obj->level>1) { 
            wc-=(wc_obj->level+thaco_bonus[op->stats.Str]);
            for(i=1;i<wc_obj->level;i++) { 
                /* addtional wc every 6 levels */ 
                if(!(i%6)) wc--; 
                /* addtional dam every 4 levels. */ 
                if(!(i%4) && (dam_bonus[op->stats.Str]>=0)) 
                    op->stats.dam+=(1+(dam_bonus[op->stats.Str]/5));
            }
        } else 
            wc-=(op->level+thaco_bonus[op->stats.Str]);

            op->stats.dam+=dam_bonus[op->stats.Str];

            if(op->stats.dam<1)
                op->stats.dam=1;

            op->speed = 1.0 + speed_bonus[op->stats.Dex];
            if (settings.search_items && op->contr->search_str[0])
                op->speed -= 1;
            if (op->attacktype==0)
                op->attacktype=op->arch->clone.attacktype;

    } /* End if player */

    if(added_speed>=0)
        op->speed+=added_speed/10.0;
    else /* Something wrong here...: */
        op->speed /= (float)(1.0-added_speed);

    /* Max is determined by armour */
    if(op->speed>max)
        op->speed=max;

    if(op->type == PLAYER) {
	/* f is a number the represents the number of kg above (positive num)
	 * or below (negative number) that the player is carrying.  If above
	 * weight limit, then player suffers a speed reduction based on how
	 * much above he is, and what is max carry is
	 */
	f=(op->carrying/1000)-max_carry[op->stats.Str];
	if(f>0) op->speed=op->speed/(1.0+f/max_carry[op->stats.Str]);
    }

    op->speed+=bonus_speed/10.0; /* Not affected by limits */

    /* Put a lower limit on speed.  Note with this speed, you move once every
     * 100 ticks or so.  This amounts to once every 12 seconds of realtime.
     */
    op->speed = op->speed * speed_reduce_from_disease;

    if (op->speed<0.01 && op->type==PLAYER)
        op->speed=0.01;

    if(op->type == PLAYER) {
        float M,W,s,D,K,S,M2;

        /* (This formula was made by vidarl@ifi.uio.no)
         * Note that we never used these values again - basically
         * all of these could be subbed into one big equation, but
         * that would just be a real pain to read.
         */
        M=(max_carry[op->stats.Str]-121)/121.0;
        M2=max_carry[op->stats.Str]/100.0;
        W=weapon_weight/20000.0;
        s=2-weapon_speed/10.0;
        D=(op->stats.Dex-14)/14.0;
        K=1 + M/3.0 - W/(3*M2) + op->speed/5.0 + D/2.0;
        K*=(4+op->level)/(float)(6+op->level)*1.2;
        if(K<=0)
            K=0.01;
        S=op->speed/(K*s);
        op->contr->weapon_sp=S;
    }
    /* I want to limit the power of small monsters with big weapons: */
    if(op->type!=PLAYER&&op->arch!=NULL&&
       op->stats.dam>op->arch->clone.stats.dam*3)
        op->stats.dam=op->arch->clone.stats.dam*3;

    /* Prevent overflows of wc - best you can get is ABS(120) - this
     * should be more than enough - remember, AC is also in 8 bits,
     * so its value is the same.
     */
    if (wc>120)
        wc=120;
    else if (wc<-120)
        wc=-120;
    op->stats.wc=wc;

    if (ac>120)
        ac=120;
    else if (ac<-120)
        ac=-120;
    op->stats.ac=ac;

    /* if for some reason the creature doesn't have any move type,
     * give them walking as a default.
     * The second case is a special case - to more closely mimic the
     * old behaviour - if your flying, your not walking - just
     * one or the other.
     */
    if (op->move_type == 0)
        op->move_type = MOVE_WALK;
    else if (op->move_type & (MOVE_FLY_LOW | MOVE_FLY_HIGH))
        op->move_type &= ~MOVE_WALK;

    update_ob_speed(op);

    /* It is quite possible that a player's spell costing might have changed,
     * so we will check that now.
     */
    if (op->type == PLAYER) esrv_update_spells(op->contr);
}

/**
 * Returns true if the given player is a legal class.
 * The function to add and remove class-bonuses to the stats doesn't
 * check if the stat becomes negative, thus this function
 * merely checks that all stats are 1 or more, and returns
 * false otherwise.
 */
int allowed_class(const object *op) {
  return op->stats.Dex>0&&op->stats.Str>0&&op->stats.Con>0&&
         op->stats.Int>0&&op->stats.Wis>0&&op->stats.Pow>0&&
	 op->stats.Cha>0;
}

/**
 * Set the new dragon name after gaining levels or
 * changing ability focus (later this can be extended to
 * eventually change the player's face and animation)
 *
 * Note that the title is written to 'own_title' in the
 * player struct. This should be changed to 'ext_title'
 * as soon as clients support this!
 * Please, anyone, write support for 'ext_title'.
 */
void set_dragon_name(object *pl, const object *abil, const object *skin) {
  int atnr=-1;  /* attacknumber of highest level */
  int level=0;  /* highest level */
  int i;

  /* Perhaps do something more clever? */
  if (!abil || !skin) return;
  
  /* first, look for the highest level */
  for(i=0; i<NROFATTACKS; i++) {
    if (atnr_is_dragon_enabled(i) &&
	(atnr==-1 || abil->resist[i] > abil->resist[atnr])) {
      level = abil->resist[i];
      atnr = i;
    }
  }
  
  /* now if there are equals at highest level, pick the one with focus,
     or else at random */
  if (atnr_is_dragon_enabled(abil->stats.exp) &&
      abil->resist[abil->stats.exp] >= level)
    atnr = abil->stats.exp;
  
  level = (int)(level/5.);
  
  /* now set the new title */
  if (pl->contr != NULL) {
    if(level == 0)
      sprintf(pl->contr->title, "%s hatchling", attacks[atnr]);
    else if (level == 1)
      sprintf(pl->contr->title, "%s wyrm", attacks[atnr]);
     else if (level == 2)
      sprintf(pl->contr->title, "%s wyvern", attacks[atnr]);
    else if (level == 3)
      sprintf(pl->contr->title, "%s dragon", attacks[atnr]);
    else {
      /* special titles for extra high resistance! */
      if (skin->resist[atnr] > 80)
	sprintf(pl->contr->title, "legendary %s dragon", attacks[atnr]);
      else if (skin->resist[atnr] > 50)
	sprintf(pl->contr->title, "ancient %s dragon", attacks[atnr]);
      else
	sprintf(pl->contr->title, "big %s dragon", attacks[atnr]);
    }
  }
  
  strcpy(pl->contr->own_title, "");
}

/**
 * This function is called when a dragon-player gains
 * an overall level. Here, the dragon might gain new abilities
 * or change the ability-focus.
 */
void dragon_level_gain(object *who) {
    object *abil = NULL;    /* pointer to dragon ability force*/
    object *skin = NULL;    /* pointer to dragon skin force*/
    object *tmp = NULL;     /* tmp. object */
  
    /* now grab the 'dragon_ability'-forces from the player's inventory */
    for (tmp=who->inv; tmp!=NULL; tmp=tmp->below) {
	if (tmp->type == FORCE) {
	    if (strcmp(tmp->arch->name, "dragon_ability_force")==0)
		abil = tmp;
	    if (strcmp(tmp->arch->name, "dragon_skin_force")==0)
		skin = tmp;
	}
    }
    /* if the force is missing -> bail out */
    if (abil == NULL) return;
  
    /* The ability_force keeps track of maximum level ever achieved.
     * New abilties can only be gained by surpassing this max level 
     */
    if (who->level > abil->level) {
	/* increase our focused ability */
	abil->resist[abil->stats.exp]++;

	if (abil->resist[abil->stats.exp]>0 && abil->resist[abil->stats.exp]%5 == 0) {
	    /* time to hand out a new ability-gift */
	    dragon_ability_gain(who, (int)abil->stats.exp,
			       (int)((1+abil->resist[abil->stats.exp])/5.));
	}
    
	if (abil->last_eat > 0 && atnr_is_dragon_enabled(abil->last_eat)) {
	    /* apply new ability focus */
	    draw_ext_info_format(NDI_UNIQUE|NDI_BLUE, 0, who, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_RACE,
		"Your metabolism now focuses on %s!",
		"Your metabolism now focuses on %s!",
		change_resist_msg[abil->last_eat]);
      
	    abil->stats.exp = abil->last_eat;
	    abil->last_eat = 0;
	}
    
	abil->level = who->level;
    }
  
    /* last but not least, set the new title for the dragon */
    set_dragon_name(who, abil, skin);
}

/**
 * Handy function - given the skill name skill_name, we find the skill
 * archetype/object, set appropriate values, and insert it into
 * the object (op) that is passed.
 * We return the skill - this makes it easier for calling functions that
 * want to do something with it immediately.
 */
object *give_skill_by_name(object *op, const char *skill_name)
{
    object *skill_obj;
    archetype *skill_arch;

    skill_arch = get_archetype_by_skill_name(skill_name, SKILL);
    if (!skill_arch) {
	LOG(llevError, "add_player_exp: couldn't find skill %s\n", skill_name);
	return NULL;
    }
    skill_obj = arch_to_object(skill_arch);
    if (!skill_obj) {
        LOG(llevError, "add_player_exp: couldn't instanciate skill %s\n", skill_name);
        return NULL;
    }
    /* clear the flag - exp goes into this bucket, but player
     * still doesn't know it.
     */
    CLEAR_FLAG(skill_obj, FLAG_CAN_USE_SKILL);
    skill_obj->stats.exp = 0;
    skill_obj->level = 1;
    insert_ob_in_ob(skill_obj, op);
    if (op->contr) {
	op->contr->last_skill_ob[skill_obj->subtype] = skill_obj;
	op->contr->last_skill_exp[skill_obj->subtype] = -1;
    }
    return skill_obj;
}


/**
 * player_lvl_adj() - for the new exp system. we are concerned with
 * whether the player gets more hp, sp and new levels.
 * Note this this function should only be called for players.  Monstes
 * don't really gain levels
 * who is the player, op is what we are checking to gain the level
 * (eg, skill)
 */
void player_lvl_adj(object *who, object *op) {
    char buf[MAX_BUF];
    
    if(!op)        /* when rolling stats */ 
	op = who;	
 
    if(op->level < settings.max_level && op->stats.exp >= level_exp(op->level+1,who->expmul)) {
	op->level++;
	
	if (op != NULL && op == who && op->stats.exp > 1 && is_dragon_pl(who))
	  dragon_level_gain(who);

        /* Only roll these if it is the player (who) that gained the level */
	if(who && op==who && (who->level < 11) && who->type==PLAYER) { 
	    who->contr->levhp[who->level] = die_roll(2, 4, who, PREFER_HIGH)+1;
	    who->contr->levsp[who->level] = die_roll(2, 3, who, PREFER_HIGH);
	    who->contr->levgrace[who->level]=die_roll(2, 2, who, PREFER_HIGH)-1;
	}

    if(who)
        fix_object(who);
	if(op->level>1) {
	    if (op->type!=PLAYER)
		sprintf(buf,"You are now level %d in the %s skill.",op->level,op->name);
	    else
		sprintf(buf,"You are now level %d.",op->level);

	    if(who) draw_ext_info(NDI_UNIQUE|NDI_RED, 0, who, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_LEVEL_GAIN,buf, buf);
	}
	player_lvl_adj(who,op); /* To increase more levels */
    } else if (op->level>1 && op->stats.exp<level_exp(op->level,who->expmul)) {
	op->level--;
	if(who)
        fix_object(who);
	if(op->type!=PLAYER) {
	    if(who) 
		draw_ext_info_format(NDI_UNIQUE|NDI_RED, 0, who, MSG_TYPE_ATTRIBUTE, MSG_TYPE_ATTRIBUTE_LEVEL_LOSS,
			"You are now level %d in the %s skill.",
			"You are now level %d in the %s skill.",
			op->level,op->name);
	}
	player_lvl_adj(who,op); /* To decrease more levels */
    }
    /* check if the spell data has changed */
    esrv_update_spells(who->contr);
}

/**
 * Returns how much experience is needed for a player to become
 * the given level.  level should really never exceed max_level
 */

sint64 level_exp(int level,double expmul) {
    if (level > settings.max_level)
	return expmul * levels[settings.max_level];
    return expmul * levels[level];
}

/**
 * Ensure that the permanent experience requirements in an exp object are met.
 * This really just checks 'op to make sure the perm_exp value is within
 * proper range.  Note that the checking of what is passed through
 * has been reduced.  Since there is now a proper field for perm_exp,
 * this can now work on a much larger set of objects.
 */
void calc_perm_exp(object *op)
{
    int p_exp_min;
    
    /* Ensure that our permanent experience minimum is met.
     * permenent_exp_ratio is an integer percentage, we divide by 100 
     * to get the fraction   */
    p_exp_min = (int)(settings.permanent_exp_ratio * (float)(op->stats.exp)/100);

    if (op->perm_exp < p_exp_min)
        op->perm_exp = p_exp_min;

    /* Cap permanent experience. */
    if (op->perm_exp < 0)
        op->perm_exp = 0;
    else if (op->perm_exp > MAX_EXPERIENCE)
        op->perm_exp = MAX_EXPERIENCE;
}


/**
 * Add experience to a player - exp should only be positive.
 * Updates permanent exp for the skill we are adding to.
 * skill_name is the skill to add exp to.  Skill name can be
 *   NULL, in which case exp increases the players general
 *   total, but not any particular skill.
 * flag is what to do if the player doesn't have the skill:
 */
static void add_player_exp(object *op, sint64 exp, const char *skill_name, int flag)
{
    object *skill_obj=NULL;
    sint64 limit, exp_to_add;
    int i;

    /* prevents some forms of abuse. */
    if(op->contr->braced) exp=exp/5;

    /* Try to find the matching skill.
     * We do a shortcut/time saving mechanism first - see if it matches
     * chosen_skill.  This means we don't need to search through
     * the players inventory.
     */
    if (skill_name) {
	if (op->chosen_skill && op->chosen_skill->type == SKILL &&
	   !strcmp(skill_name, op->chosen_skill->skill))
	    skill_obj = op->chosen_skill;
	else {
	    for (i=0; i<NUM_SKILLS; i++)
		if (op->contr->last_skill_ob[i] && 
		  !strcmp(op->contr->last_skill_ob[i]->skill, skill_name)) {
		    skill_obj = op->contr->last_skill_ob[i];
		    break;
		}

	    /* Player doesn't have the skill.  Check to see what to do, and give
	     * it to the player if necessary
	     */
	    if (!skill_obj) {
		if (flag == SK_EXP_NONE) return;
		else if (flag == SK_EXP_ADD_SKILL)
		    give_skill_by_name(op, skill_name);
	    }
	}
    }

    /* Basically, you can never gain more experience in one shot
     * than half what you need to gain for next level.
     */
    exp_to_add = exp;
    limit=(levels[op->level+1]-levels[op->level])/2;
    if (exp_to_add > limit) exp_to_add=limit;

    ADD_EXP(op->stats.exp, (float) exp_to_add * (skill_obj? skill_obj->expmul:1));
    if (settings.permanent_exp_ratio) {
	ADD_EXP(op->perm_exp, (float) exp_to_add * PERM_EXP_GAIN_RATIO * (skill_obj? skill_obj->expmul:1));
	calc_perm_exp(op);
    }

    player_lvl_adj(op,NULL);
    if (skill_obj) {
	exp_to_add = exp;
	limit=(levels[skill_obj->level+1]-levels[skill_obj->level])/2;
	if (exp_to_add > limit) exp_to_add=limit;
	ADD_EXP(skill_obj->stats.exp, exp_to_add);
	if (settings.permanent_exp_ratio) {
	    skill_obj->perm_exp += exp_to_add * PERM_EXP_GAIN_RATIO;
	    calc_perm_exp(skill_obj);
	}
	player_lvl_adj(op,skill_obj);
    }
}

/**
 * This function checks to make sure that object 'op' can
 * lost 'exp' experience.  It returns the amount of exp
 * object 'op' can in fact lose - it basically makes
 * adjustments based on permanent exp and the like.
 * This function should always be used for losing experience -
 * the 'exp' value passed should be positive - this is the
 * amount that should get subtract from the player.
 */
sint64 check_exp_loss(const object *op, sint64 exp)
{
    sint64 del_exp;

    if (exp > op->stats.exp) exp = op->stats.exp;
    if (settings.permanent_exp_ratio) {
	del_exp = (op->stats.exp - op->perm_exp) * PERM_EXP_MAX_LOSS_RATIO;
	if (del_exp < 0) del_exp = 0;
	if (exp > del_exp) exp=del_exp;
    }
    return exp;
}

sint64 check_exp_adjust(const object *op, sint64 exp)
{
    if (exp<0) return check_exp_loss(op, exp);
    else return MIN(exp, MAX_EXPERIENCE - op->stats.exp);
}


/**
 * Subtracts experience from player.
 * if skill is set and flag == SK_SUBTRACT_SKILL_EXP, then we
 * only subtract from the matching skill.  Otherwise,
 * this subtracts a portion from all
 * skills the player has.  Eg, if we figure the player is losing 10%
 * of his total exp, what happens is he loses 10% from all his skills.
 * Note that if permanent exp is used, player may not in fact lose
 * as much as listed.  Eg, if player has gotten reduced to the point
 * where everything is at the minimum perm exp, he would lose nothing.
 * exp is the amount of exp to subtract - thus, it should be
 * a postive number.
 */
static void subtract_player_exp(object *op, sint64 exp, const char *skill, int flag)
{
    float fraction = (float) exp/(float) op->stats.exp;
    object *tmp;
    sint64 del_exp;

    for(tmp=op->inv;tmp;tmp=tmp->below)
	if(tmp->type==SKILL && tmp->stats.exp) { 
	    if (flag == SK_SUBTRACT_SKILL_EXP && skill && !strcmp(tmp->skill, skill)) {
		del_exp = check_exp_loss(tmp, exp);
		tmp->stats.exp -= del_exp;
		player_lvl_adj(op, tmp);
	    } else if (flag != SK_SUBTRACT_SKILL_EXP) {
		/* only want to process other skills if we are not trying
		 * to match a specific skill.
		 */
		del_exp = check_exp_loss(tmp, tmp->stats.exp * fraction);
		tmp->stats.exp -= del_exp;
		player_lvl_adj(op, tmp);
	    }
	}
    if (flag != SK_SUBTRACT_SKILL_EXP) {
	del_exp = check_exp_loss(op, exp);
	op->stats.exp -= del_exp;
	player_lvl_adj(op,NULL); 
    }
}



/**
 * change_exp() - changes experience to a player/monster.  This
 * does bounds checking to make sure we don't overflow the max exp.
 *
 * The exp passed is typically not modified much by this function -
 * it is assumed the caller has modified the exp as needed.
 * skill_name is the skill that should get the exp added.
 * flag is what to do if player doesn't have the skill.
 * these last two values are only used for players.
 */
void change_exp(object *op, sint64 exp, const char *skill_name, int flag) {

#ifdef EXP_DEBUG
#ifndef WIN32
    LOG(llevDebug,"change_exp() called for %s, exp = %lld\n",query_name(op),exp); 
#else
    LOG(llevDebug,"change_exp() called for %s, exp = %I64d\n",query_name(op),exp); 
#endif
#endif

    /* safety */
    if(!op) { 
	LOG(llevError,"change_exp() called for null object!\n"); 
	return; 
    }

    /* if no change in exp, just return - most of the below code
     * won't do anything if the value is 0 anyways.
     */
    if (exp == 0) return;

    /* Monsters are easy - we just adjust their exp - we   
     * don't adjust level, since in most cases it is unrelated to
     * the exp they have - the monsters exp represents what its
     * worth.
     */
    if(op->type != PLAYER) {
	/* Sanity check */
	if (!QUERY_FLAG(op, FLAG_ALIVE)) return;

	/* reset exp to max allowed value.  We subtract from
	 * MAX_EXPERIENCE to prevent overflows.  If the player somehow has
	 * more than max exp, just return.
	 */
	if (exp > 0 && ( op->stats.exp > (MAX_EXPERIENCE - exp))) {
	    exp = MAX_EXPERIENCE - op->stats.exp;
	    if (exp < 0) return;
	}

	op->stats.exp += exp;
    }
    else {				/* Players only */ 
	if(exp>0) 
	    add_player_exp(op, exp, skill_name, flag);
	else
	    /* note that when you lose exp, it doesn't go against
	     * a particular skill, so we don't need to pass that
	     * along.
	     */
	    subtract_player_exp(op, FABS(exp), skill_name, flag);

    }
}

/**
 * Applies a death penalty experience, the size of this is defined by the 
 * settings death_penalty_percentage and death_penalty_levels, and by the
 * amount of permenent experience, whichever gives the lowest loss.
 */
void apply_death_exp_penalty(object *op) {
    object *tmp;
    sint64 loss;
    sint64 percentage_loss;  /* defined by the setting 'death_penalty_percent' */
    sint64 level_loss;   /* defined by the setting 'death_penalty_levels */

    for(tmp=op->inv;tmp;tmp=tmp->below)
	if(tmp->type==SKILL && tmp->stats.exp) { 

	    percentage_loss = tmp->stats.exp * settings.death_penalty_ratio/100;
	    level_loss = tmp->stats.exp - levels[MAX(0,tmp->level - settings.death_penalty_level)];

	    /* With the revised exp system, you can get cases where
	     * losing several levels would still require that you have more
	     * exp than you currently have - this is true if the levels
	     * tables is a lot harder.
	     */
	    if (level_loss < 0) level_loss = 0;

	    loss = check_exp_loss(tmp, MIN(level_loss, percentage_loss));

	    tmp->stats.exp -= loss;
	    player_lvl_adj(op,tmp);
	}

    percentage_loss = op->stats.exp * settings.death_penalty_ratio/100;
    level_loss = op->stats.exp - levels[MAX(0,op->level - settings.death_penalty_level)];
    if (level_loss < 0) level_loss = 0;
    loss = check_exp_loss(op, MIN(level_loss, percentage_loss));

    op->stats.exp -= loss;
    player_lvl_adj(op,NULL);
}

/**
 * This function takes an object (monster/player, op), and
 * determines if it makes a basic save throw by looking at the
 * save_throw table.  level is the effective level to make
 * the save at, and bonus is any plus/bonus (typically based on
 * resistance to particular attacktype.
 * Returns 1 if op makes his save, 0 if he failed
 */
int did_make_save(const object *op, int level, int bonus)
{
    if (level > MAX_SAVE_LEVEL) level = MAX_SAVE_LEVEL;

    if ((random_roll(1, 20, op, PREFER_HIGH) + bonus) < savethrow[level])
	return 0;
    return 1;
}

/*
 * static char *rcsid_living_c =
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
#include <funcpoint.h>


static int con_bonus[MAX_STAT + 1]={
  -6,-5,-4,-3,-2,-1,-1,0,0,0,0,1,2,3,4,5,6,7,8,9,10,12,14,16,18,20,
  22,25,30,40,50
};
/* changed the name of this to "sp_bonus" from "int_bonus" 
 * because Pow can now be the stat that controls spellpoint
 * advancement. -b.t.
 */
static int sp_bonus[MAX_STAT + 1]={
  -10,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,12,15,20,25,
  30,40,50,70,100
};

static int grace_bonus[MAX_STAT +1] = {
    -10,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,12,15,20,25,
  30,40,50,70,100
};

/* 0.92.7 Changed way charisma works.  Values now 
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

float cha_bonus[MAX_STAT + 1]={10.0, 10.0, 9.0, 8.0, 7.0, 6.0, /*<-5*/
    5.0, 4.5, 4.0, 3.5, 3.0, /*<-10*/ 2.9, 2.8, 2.7, 2.6, 2.5, /*<-15*/
    2.4, 2.3, 2.2, 2.1, 2.0, /*<-20*/ 1.95, 1.90, 1.85, 1.80, 1.75, /*25 */
    1.70, 1.65, 1.60, 1.55, 1.50 /*30 */
};

int dex_bonus[MAX_STAT + 1]={
  -4,-3,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,6,6,7
};

/* speed_bonus uses dex as its stat */
float speed_bonus[MAX_STAT + 1]={
  -0.4, -0.4, -0.3, -0.3, -0.2, -0.2, -0.2, -0.1, -0.1, -0.1, -0.05, 0, 0, 0,
  0.05, 0.1, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 1.0, 1.2, 1.4,
  1.6, 1.8, 2.0, 2.5, 3.0
};

/* dam_bonus, thaco_bonus, max_carry, weight limit all are based on
 * strength.
 */
int dam_bonus[MAX_STAT + 1]={
  -2,-2,-2,-1,-1,-1,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,6,6,7,8,10,15
};
int thaco_bonus[MAX_STAT + 1]={
  -2,-2,-1,-1,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,6,7,8,10
};

/* Max you can carry before you start getting extra speed penalties */
int max_carry[MAX_STAT + 1]={
  2,4,7,11,16,22,29,37,46,56,67,79,92,106,121,137,154,172,191,211,232,254,277,
  301,326,352,400,450,500,600,1000
};

/* weight_limit - the absolute most a character can carry - a character can't
 * pick stuff up if it would put him above this limit.
 * value is in grams, so we don't need to do conversion later
 * These limits are probably overly generous, but being there were no values
 * before, you need to start someplace.
 */

int weight_limit[MAX_STAT+ 1] = {
    200000,  /* 0 */
    250000,300000,350000,400000,500000,	    /* 5*/
    600000,700000,800000,900000,1000000,    /* 10 */
    1100000,1200000,1300000,1400000,1500000,/* 15 */
    1650000,1800000,1950000,2100000,2250000,/* 20 */
    2400000,2550000,2700000,2850000,3000000, /* 25 */
    3250000,3500000,3750000,4000000,4500000  /*30 */ 
};

int learn_spell[MAX_STAT + 1]={
  0,0,0,1,2,4,8,12,16,25,36,45,55,65,70,75,80,85,90,95,100,100,100,100,100,
  100,100,100,100,100,100
};
int cleric_chance[MAX_STAT + 1]={
  100,100,100,100,100,100,100,90,80,70,60,50,45,40,35,30,25,20,15,10,5,0,-5,-10,-15,-20,-25,-30,-35,-40,-50
};
int turn_bonus[MAX_STAT + 1]={
  -1,-1,-1,-1,-1,-1,-1,-1,0,0,0,1,1,1,2,2,2,3,3,3,4,4,5,5,6,7,8,9,10,12,15
};
int fear_bonus[MAX_STAT + 1]={
  3,3,3,3,2,2,2,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
int learn_prayer_chance[MAX_STAT + 1] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,10,15,20,30,40,60
};

/*
   Since this is nowhere defined ...
   Both come in handy at least in function add_exp()
*/

#define MAXLEVEL      110
#define MAX_EXPERIENCE  levels[MAXLEVEL]

/* because exp_obj sum to make the total score,
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

#define MAX_EXP_IN_OBJ levels[MAXLEVEL]/(MAX_EXP_CAT - 1) 

static uint32 levels[MAXLEVEL+1]={
0,
0,1000,2000,4000, 8000,
16000,32000,64000,125000,250000,		/* 10 */
500000,900000,1400000,2000000,2600000,
3300000,4100000,4900000,5700000,6600000,	/* 20 */
7500000,8400000,9300000,10300000,11300000,
12300000,13300000,14400000,15500000,16600000,	/* 30 */
17700000,18800000,19900000,21100000,22300000,	
23500000,24700000,25900000,27100000,28300000,	/* 40 */
29500000,30800000,32100000,33400000,34700000,
36000000,37300000,38600000,39900000,41200000,	/* 50 */
42600000,44000000,45400000,46800000,48200000,
49600000,51000000,52400000,53800000,55200000,	/* 60 */
56600000,58000000,59400000,60800000,62200000,
63700000,65200000,66700000,68200000,69700000,	/* 70 */
71200000,72700000,74200000,75700000,77200000,
78700000,80200000,81700000,83200000,84700000,	/* 80 */
86200000,87700000,89300000,90900000,92500000,
94100000,95700000,97300000,98900000,100500000,	/* 90 */
102100000,103700000,105300000,106900000,108500000,
110100000,111700000,113300000,114900000,116500000,	/* 100 */
118100000,119700000,121300000,122900000,124500000, 	
126100000,127700000,129300000,130900000,785400000	/* 110 */};

static uint32 new_levels[MAXLEVEL+1]={
0,
0,2000,4000, 8000,
16000,32000,64000,125000,250000,		/* 9 */
500000,900000,1400000,2000000,2600000,
3300000,4100000,4900000,5700000,6600000,	/* 19 */
7500000,8400000,9300000,10300000,11300000,
12300000,13300000,14400000,15500000,16600000,	/* 29 */
17700000,18800000,19900000,21100000,22300000,	
23500000,24700000,25900000,27100000,28300000,	/* 39 */
29500000,30800000,32100000,33400000,34700000,
36000000,37300000,38600000,39900000,41200000,	/* 49 */
42600000,44000000,45400000,46800000,48200000,
49600000,51000000,52400000,53800000,55200000,	/* 59 */
56600000,58000000,59400000,60800000,62200000,
63700000,65200000,66700000,68200000,69700000,	/* 69 */
71200000,72700000,74200000,75700000,77200000,
78700000,80200000,81700000,83200000,84700000,	/* 79 */
86200000,87700000,89300000,90900000,92500000,
94100000,95700000,97300000,98900000,100500000,	/* 89 */
102100000,103700000,105300000,106900000,108500000,
110100000,111700000,113300000,114900000,116500000,	/* 99 */
118100000,119700000,121300000,122900000,124500000, 	
126100000,127700000,129300000,130900000,785400000,
1570800000	/* 110 */
};

/* Max level is 100.  By making it 101, it means values 0->100 are valid.
 * Thus, we can use op->level directly, and it also works for level 0 people.
 */
int savethrow[MAXLEVEL+1]={
  18,
  18,17,16,15,14,14,13,13,12,12,12,11,11,11,11,10,10,10,10, 9,
   9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6,
   6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

char *attacks[NROFATTACKS] = {
  "physical", "magical", "fire", "electricity", "cold", "confusion",
  "acid", "drain", "weaponmagic", "ghosthit", "poison", "slow",
  "paralyze", "turn undead", "fear", "cancellation", "depletion", "death",
  "chaos","counterspell","god power","holy power","blinding", ""
};

static char *drain_msg[7] = {
  "Oh no! You are weakened!",
  "You're feeling clumsy!",
  "You feel less healthy",
  "You suddenly begin to lose your memory!",
  "Your face gets distorted!",
  "Watch out, your mind is going!", 
  "Your spirit feels drained!"
};
char *restore_msg[7] = {
  "You feel your strength return.",
  "You feel your agility return.",
  "You feel your health return.",
  "You feel your wisdom return.",
  "You feel your charisma return.",
  "You feel your memory return.", 
  "You feel your spirits return."
};
char *gain_msg[7] = {
	"You feel stronger.",
	"You feel more agile.",
	"You feel healthy.",
	"You feel wiser.",
	"You seem to look better.",
	"You feel smarter.", 
	"You feel more potent."
};
char *lose_msg[7] = {
	"You feel weaker!",
	"You feel clumsy!",
	"You feel less healthy!",
	"You lose some of your memory!",
	"You look ugly!",
	"You feel stupid!", 
	"You feel less potent!"
};

char *statname[7] = {
  "strength", "dexterity", "constitution", "wisdom", "charisma", "intelligence","power" 
};

char *short_stat_name[7] = {
  "Str", "Dex", "Con", "Wis", "Cha", "Int","Pow" 
};


/*
 * sets Str/Dex/con/Wis/Cha/Int/Pow in stats to value, depending on
 * what attr is (STR to POW).
 */

void
set_attr_value(living *stats,int attr,signed char value) {
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

/*
 * Like set_attr_value(), but instead the value (which can be negative)
 * is added to the specified stat.
 */

void
change_attr_value(living *stats,int attr,signed char value) {
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

/*
 * returns the specified stat.  See also set_attr_value().
 */

signed char
get_attr_value(living *stats,int attr) {
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

/*
 * Ensures that all stats (str/dex/con/wis/cha/int) are within the
 * 1-30 stat limit.
 */

void check_stat_bounds(living *stats) {
  int i,v;
  for(i=0;i<7;i++)
    if((v=get_attr_value(stats,i))>MAX_STAT)
      set_attr_value(stats,i,MAX_STAT);
    else if(v<MIN_STAT)
      set_attr_value(stats,i,MIN_STAT);
}

#define ORIG_S(xyz,abc)	(op->contr->orig_stats.abc)

/*
 * Adds abilities to the first object based on what the second object
 * gives to appliers.  If the second object does not have the APPLIED
 * flag set, it is assumed that it is being unapplied, and any abilities
 * it gives are subtracted from the first object.
 * (This is of course a problem now, since several objects may give
 * the same abilities, thus change_abil() is used mostly to display
 * messages, while fix_player() is called afterwards.)
 * Also writes a more or less informative message to the first object
 * about what abilities were gained/lost.
 */

/* if immunity has changed then display that.  If not immune then show change
 * in resistance if resistance has changed or if immunity has changed and if
 * resistant then say resistant after saying no longer immune.
 */
#define CHANGE_ABIL_IMMUNE_MACRO(AT,im1,im2,re1,re2) \
  if ( (op->immune & (AT) ) != (refop.immune & (AT))) { \
    success = 1; \
    if(flag>0) \
      (*draw_info_func)(NDI_UNIQUE, 0, op, im1 ); \
    else { \
      (*draw_info_func)(NDI_UNIQUE, 0, op, im2 ); \
	  if (refop.protected& (AT)) (*draw_info_func)(NDI_UNIQUE, 0, op, re1 ); \
	}\
  } else if (  ( ! (op->immune & (AT) )) \
		 && ((op->protected& (AT) ) != (refop.protected& (AT) ))) {\
    success=1; \
    if(flag>0) \
      (*draw_info_func)(NDI_UNIQUE, 0, op, re1 ); \
    else \
      (*draw_info_func)(NDI_UNIQUE, 0, op, re2 ); \
  } \

/* return 1 if we sucessfully changed a stat, 0 if nothing was changed. */
/* flag is set to 1 if we are applying the object, -1 if we are removing
 * the object.
 * It is the calling functions responsibilty to check to see if the object
 * can be applied or not.
 */
int change_abil(object *op, object *tmp) {
  int flag=QUERY_FLAG(tmp,FLAG_APPLIED)?1:-1,i,j,success=0;
  object refop;

  /* remember what object was like before it was changed.  note that
   * refop is a local copy of op only to be used for detecting changes
   * found by fix_player.  refop is not a real object */
  memcpy(&refop, op, sizeof(object));

  if(op->type==PLAYER) {
    if (tmp->type==POTION) {
      for(j=0;j<7;j++) {
        i = get_attr_value(&(op->contr->orig_stats),j);

	/* Check to see if stats are within limits such that this can be
	 * applied.
	 */
        if (((i+flag*get_attr_value(&(tmp->stats),j))<=
	    (20+tmp->stats.sp + get_attr_value(&(op->arch->clone.stats),j)))
	    && i>0)
	{
            change_attr_value(&(op->contr->orig_stats),j,
                          flag*get_attr_value(&(tmp->stats),j));
	    tmp->stats.sp=0;/* Fix it up for super potions */
	}
	else {
		set_attr_value(&(tmp->stats),j,0);
	}
      }
    /* This section of code ups the characters normal stats also.  I am not
     * sure if this is strictly necessary, being that fix_player probably
     * recalculates this anyway.
     */
    for(j=0;j<7;j++)
      change_attr_value(&(op->stats),j,flag*get_attr_value(&(tmp->stats),j));
    check_stat_bounds(&(op->stats));
    } /* end of potion handling code */
  }

  /* reset attributes that fix_player doesn't reset since it doesn't search
   * everything to set */
  if(flag == -1)
    op->attacktype&=~tmp->attacktype,
    op->path_attuned&=~tmp->path_attuned,
    op->path_repelled&=~tmp->path_repelled,
    op->path_denied&=~tmp->path_denied;

  /* call fix_player since op object could have whatever attribute due
   * to multiple items.  if fix_player always has to be called after
   * change_ability then might as well call it from here
   */
  fix_player(op);

  if(tmp->attacktype & AT_CONFUSION) {
    success=1;
    if(flag>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"Your hands begin to glow red.");
    else
      (*draw_info_func)(NDI_UNIQUE, 0, op,"Your hands stop glowing red.");
  }
  if ( QUERY_FLAG(op,FLAG_LIFESAVE) != QUERY_FLAG(&refop,FLAG_LIFESAVE)){
    success=1;
    if(flag>0) {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel very protected.");
    } else {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You don't feel protected anymore.");
    }
  }
  if ( QUERY_FLAG(op,FLAG_REFL_MISSILE) != QUERY_FLAG(&refop,FLAG_REFL_MISSILE)){
    success=1;
    if(flag>0) {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"A magic force shimmers around you.");
    } else {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"The magic force fades away.");
    }
  }
  if ( QUERY_FLAG(op,FLAG_REFL_SPELL) != QUERY_FLAG(&refop,FLAG_REFL_SPELL)){
    success=1;
    if(flag>0) {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel more safe now, somehow.");
    } else {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"Suddenly you feel less safe, somehow.");
    }
  }
  if(QUERY_FLAG(tmp,FLAG_FLYING)) {
    if(flag>0) {
      success=1;
      /* if were already flying then now flying higher */
      if ( QUERY_FLAG(op,FLAG_FLYING) == QUERY_FLAG(&refop,FLAG_FLYING))
        (*draw_info_func)(NDI_UNIQUE, 0, op,"You float a little higher in the air.");
      else {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"You start to float in the air!.");
        SET_FLAG(op,FLAG_FLYING);
        if(op->speed>1)
          op->speed=1;
      }
    } else {
      success=1;
      /* if were already flying then now flying lower */
      if ( QUERY_FLAG(op,FLAG_FLYING) == QUERY_FLAG(&refop,FLAG_FLYING))
        (*draw_info_func)(NDI_UNIQUE, 0, op,"You float a little lower in the air.");
      else {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"You float down to the ground.");
	check_walk_on (op, op);
      }
    }
  }

  /* becoming UNDEAD... a special treatment for this flag. Only those not
   * originally undead may change their status */ 
  if(!QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD)) 
    if ( QUERY_FLAG(op,FLAG_UNDEAD) != QUERY_FLAG(&refop,FLAG_UNDEAD)) {
      success=1;
      if(flag>0) {
        if(op->race) free_string(op->race); 
	op->race=add_string("undead");
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your lifeforce drains away!");
      } else {
        if(op->race) free_string(op->race); 
        if(op->arch->clone.race) 
	   op->race=add_string(op->arch->clone.race);  
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your lifeforce returns!");
      }
    }

  if ( QUERY_FLAG(op,FLAG_STEALTH) != QUERY_FLAG(&refop,FLAG_STEALTH)){
    success=1;
    if(flag>0) {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You walk more quietly.");
    } else {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You walk more noisily.");
    }
  }
  if ( QUERY_FLAG(op,FLAG_MAKE_INVIS) != QUERY_FLAG(&refop,FLAG_MAKE_INVIS)){
    success=1;
    if(flag>0) {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You become transparent.");
    } else {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You can see yourself.");
    }
  }
  /* blinded you can tell if more blinded since blinded player has minimal
   * vision */
  if(QUERY_FLAG(tmp,FLAG_BLIND)) {
    success=1;
    if(flag>0) {
      if(QUERY_FLAG(op,FLAG_WIZ))
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your mortal self is blinded.");
      else { 
        (*draw_info_func)(NDI_UNIQUE, 0, op,"You are blinded.");
        SET_FLAG(op,FLAG_BLIND);
        if(op->type==PLAYER)
          op->contr->do_los=1;
      }  
    } else {
      if(QUERY_FLAG(op,FLAG_WIZ))
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your mortal self can now see again.");
      else {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your vision returns.");
        CLEAR_FLAG(op,FLAG_BLIND);
        if(op->type==PLAYER)
          op->contr->do_los=1;
      }  
    }  
  }

  if ( QUERY_FLAG(op,FLAG_SEE_IN_DARK) != QUERY_FLAG(&refop,FLAG_SEE_IN_DARK)){
    success=1;
    if(flag>0) {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your vision is better in the dark.");
        if(op->type==PLAYER)
          op->contr->do_los=1;
    } else {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"You see less well in the dark.");
        if(op->type==PLAYER)
          op->contr->do_los=1;
    }  
  }  

  if ( QUERY_FLAG(op,FLAG_XRAYS) != QUERY_FLAG(&refop,FLAG_XRAYS)){
    success=1;
    if(flag>0) {
      if(QUERY_FLAG(op,FLAG_WIZ))
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your vision becomes a little clearer.");
      else {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Everything becomes transparent.");
        if(op->type==PLAYER)
          op->contr->do_los=1;
      }
    } else {
      if(QUERY_FLAG(op,FLAG_WIZ))
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Your vision becomes a bit out of focus.");
      else {
        (*draw_info_func)(NDI_UNIQUE, 0, op,"Everything suddenly looks very solid.");
        if(op->type==PLAYER)
          op->contr->do_los=1;
      }
    }
  }
  if(tmp->stats.luck) {
    success=1;
    if(flag>0) {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel more lucky.");
    } else {
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel less lucky.");
    }
  }
  if(tmp->stats.hp && op->type==PLAYER) {
    success=1;
    if(flag*tmp->stats.hp>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel much more healthy!");
    else
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel much less healthy!");
  }
  if(tmp->stats.sp && op->type==PLAYER && tmp->type!=SKILL) {
    success=1;
    if(flag*tmp->stats.sp>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel one with the powers of magic!");
    else
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You suddenly feel very mundane.");
  }
  /* for the future when artifacts set this -b.t. */
  if(tmp->stats.grace && op->type==PLAYER) {
    success=1;
     if(flag*tmp->stats.grace>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel closer to your god!");
    else 
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You suddenly feel less holy.");
  } 
  if(tmp->stats.food && op->type==PLAYER) {
    success=1;
    if(tmp->stats.food*flag>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel your digestion slowing down.");
    else
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel your digestion speeding up.");
  }
	CHANGE_ABIL_IMMUNE_MACRO(AT_PHYSICAL, "You feel much less solid.",
	   "You suddenly feel very solid.", "You feel less solid.",
	   "You feel more solid.");

	CHANGE_ABIL_IMMUNE_MACRO(AT_MAGIC, "You feel immune to magic.",
	   "You feel less immune to magic.", "You feel more resistant to magic.",
	   "You feel less resistant to magic.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_FIRE, "You feel immune to fire.",
	   "You feel less immune to fire.", "You feel resistant to fire.",
	   "You feel less resistant to fire.");

  if (( op->vulnerable&AT_FIRE) != (refop.vulnerable&AT_FIRE)) {
    success=1;
    if(flag>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel exposed to fire.");
    else
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel less exposed to fire.");
  }
  CHANGE_ABIL_IMMUNE_MACRO(AT_COLD, "You feel immune to cold.",
	   "You feel less immune to cold.", "You feel resistant to cold.",
	   "You feel less resistant to cold.");

  if (( op->vulnerable&AT_COLD) != (refop.vulnerable&AT_COLD)) {
    success=1;
    if(flag>0)
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel exposed to cold.");
    else
      (*draw_info_func)(NDI_UNIQUE, 0, op,"You feel less exposed to cold.");
  }
  CHANGE_ABIL_IMMUNE_MACRO(AT_ELECTRICITY, "You feel immune to electricity.",
	   "You feel less immune to electricity.", "You feel more resistant to electricity.",
	   "You feel less resistant to electricity.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_ACID, "You feel immune to acid.",
	   "You feel less immune to acid.", "You feel more resistant to acid.",
	   "You feel less resistant to acid.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_DRAIN, "You feel very full of life.",
	   "You shiver, everything seems so bleak.", "You feel more resistant to draining.",
	   "You feel less resistant to draining.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_POISON, "You feel extremely healthy.",
	   "You feel extremely less healthy!", "You feel more resistant to poison.",
	   "You feel less resistant to poison.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_SLOW, "You feel in sync with time.",
	   "You feel out of sync with time.", "You feel more in sync with time.",
	   "You feel less in sync with time.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_PARALYZE, "You feel very unrestrained.",
	   "You feel more restrained.", "You feel more resistant to paralyzation.",
	   "You feel less resistant to paralyzation.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_FEAR, "You feel extremely brave.",
	   "You feel less brave.", "You feel more resistant to fear.",
	   "You feel less resistant to fear.");

  CHANGE_ABIL_IMMUNE_MACRO(AT_CONFUSION, "You feel more sure of yourself.",
	   "You feel less sure of yourself.", "You feel more resistant to confusion.",
	   "You feel less resistant to confusion.");

  if(tmp->type!=EXPERIENCE)
    for (j=0; j<7; j++) {
      if ((i=get_attr_value(&(tmp->stats),j))!=0) {
	success=1;
	if (i * flag > 0)
		(*draw_info_func)(NDI_UNIQUE, 0, op, gain_msg[j]);
	else
		(*draw_info_func)(NDI_UNIQUE, 0, op, lose_msg[j]);
      }
    }
  return success;
}

/*
 * Stat draining by Vick 930307
 * (Feeling evil, I made it work as well now.  -Frank 8)
 */

void drain_stat(object *op) {
  drain_specific_stat(op, RANDOM()%7);
}

void drain_specific_stat(object *op, int deplete_stats) {
  object *tmp;
  archetype *at;

  at = find_archetype("depletion");
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

  (*draw_info_func)(NDI_UNIQUE, 0, op, drain_msg[deplete_stats]);
  change_attr_value(&tmp->stats, deplete_stats, -1);
  fix_player(op);
}

/*
 * A value of 0 indicates timeout, otherwise change the luck of the object.
 * via an applied bad_luck object.
 */

void change_luck(object *op, int value) {
  object *tmp;
  archetype *at;
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
      op->stats.luck+=value;
      tmp->stats.luck+=value;
    } else {
      if (!tmp->stats.luck) {
        LOG(llevDebug, "Internal error in change_luck().\n");
        return;
      }
      /* Randomly change the players luck.  Basically, we move it
       * back neutral (if greater>0, subtract, otherwise add)
       * I believe this is supposed to be > and not >= - this means
       * if your luck is -1/1, it won't get adjusted - only when your
       * luck is worse can you hope for improvment.
       * note that if we adjusted it with it is -1/1, that check above
       * for 0 luck will happen, resulting in error.
       */
      if (RANDOM()%(FABS(tmp->stats.luck)) > RANDOM()%30)
        tmp->stats.luck += tmp->stats.luck>0?-1:1;
    }
  }
}

/*
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

/*
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

/*
 * Updates all abilities given by applied objects in the inventory
 * of the given object.  Note: This function works for both monsters
 * and players; the "player" in the name is purely an archaic inheritance.
 */
/* July 95 - inserted stuff to handle new skills/exp system - b.t.
   spell system split, grace points now added to system  --peterm
 */

void fix_player(object *op) {
  int i,j;
  float f,max=9,added_speed=0,bonus_speed=0, sp_tmp,speed_reduce_from_disease=1;
  float M,W,s,D,K,S,M2;
  int weapon_weight=0,weapon_speed=0;
  int best_wc=0, best_ac=0, wc=0;
  object *grace_obj=NULL,*mana_obj=NULL,*hp_obj=NULL,*wc_obj=NULL,*tmp;
  if(op->type==PLAYER) {
    for(i=0;i<7;i++) {
      set_attr_value(&(op->stats),i,get_attr_value(&(op->contr->orig_stats),i));
    }
#ifdef SPELL_ENCUMBRANCE
    op->contr->encumbrance=0;
#endif
#ifdef ALLOW_SKILLS
    if(op->chosen_skill&&op->chosen_skill->exp_obj)
       op->chosen_skill->level=op->chosen_skill->exp_obj->level;
#endif
        op->attacktype=0;    
	op->contr->digestion = 0;
	op->contr->gen_hp = 0;
	op->contr->gen_sp = 0;
	op->contr->gen_grace = 0;
	op->contr->gen_sp_armour = 10;
  }
  if(op->slaying!=NULL) {
    free_string(op->slaying);
    op->slaying=NULL;
  }
  if(!QUERY_FLAG(op,FLAG_WIZ)) {
	CLEAR_FLAG(op, FLAG_FLYING);
	CLEAR_FLAG(op, FLAG_XRAYS);
	CLEAR_FLAG(op, FLAG_MAKE_INVIS);
  }
  CLEAR_FLAG(op,FLAG_LIFESAVE);
  CLEAR_FLAG(op,FLAG_REFL_SPELL);
  CLEAR_FLAG(op,FLAG_REFL_MISSILE);
  CLEAR_FLAG(op,FLAG_STEALTH);
  if(!QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD))
    CLEAR_FLAG(op,FLAG_UNDEAD);
  CLEAR_FLAG(op,FLAG_BLIND);
  CLEAR_FLAG(op,FLAG_SEE_IN_DARK);

  op->path_attuned=op->arch->clone.path_attuned;
  op->path_repelled=op->arch->clone.path_repelled;
  op->path_denied=op->arch->clone.path_denied;

  op->protected=op->arch->clone.protected;
  op->vulnerable=op->arch->clone.vulnerable;
  op->immune=op->arch->clone.immune;
  op->armour=op->arch->clone.armour;
  wc=op->arch->clone.stats.wc;
  op->stats.dam=op->arch->clone.stats.dam;


  if(!QUERY_FLAG(op,FLAG_USE_ARMOUR) && op->type==PLAYER) 
	/* for players which cannot use armour, they gain AC -1 per 3 levels. */
	op->stats.ac=MAX(-10,op->arch->clone.stats.ac - op->level/3);
  else
      op->stats.ac=op->arch->clone.stats.ac;


  op->stats.luck=op->arch->clone.stats.luck;
  op->speed = op->arch->clone.speed;

  for(tmp=op->inv;tmp!=NULL;tmp=tmp->below) 
    if(QUERY_FLAG(tmp,FLAG_APPLIED) && tmp->type!=CONTAINER && tmp->type!=CLOSE_CON) {
      if(op->type==PLAYER
#ifdef ALLOW_SKILLS /* The meaning of stats in skill or experience objects is different -
		     * we use them solely to link skills to experience, thus it is 
		     * inappropriate to allow these applied objects to change stats */ 
           && tmp->type!=EXPERIENCE 
#endif
      ) {
        for(i=0;i<7;i++)
          change_attr_value(&(op->stats),i,get_attr_value(&(tmp->stats),i));
	/* these are the items that currently can change digestion, regeneration,
	 * spell point recovery and mana point recovery.  Seems sort of an arbitary
	 * list, but other items store other info into stats array. */
	if ( (tmp->type == EXPERIENCE)  || (tmp->type == WEAPON)
	     || (tmp->type == ARMOUR)   || (tmp->type == HELMET)
	     || (tmp->type == SHIELD)   || (tmp->type == RING)
	     || (tmp->type == BOOTS)    || (tmp->type == GLOVES)
	     || (tmp->type == AMULET )  || (tmp->type == GIRDLE)
	     || (tmp->type == BRACERS ) || (tmp->type == CLOAK) 
	     || (tmp->type == DISEASE)  || (tmp->type == FORCE) ){
	  op->contr->digestion     += tmp->stats.food;
	  op->contr->gen_hp        += tmp->stats.hp;
	  op->contr->gen_sp        += tmp->stats.sp;
	  op->contr->gen_grace     += tmp->stats.grace;
	  op->contr->gen_sp_armour += tmp->last_heal;
	  if (tmp->last_heal) LOG(llevDebug,"Object %s applied, gen_sp_armour + %d = %d\n", tmp->name, tmp->last_heal, op->contr->gen_sp_armour);
	}
      }

      if(tmp->type==SYMPTOM) {
	speed_reduce_from_disease = tmp->last_sp / 100.0;
	if(speed_reduce_from_disease ==0) speed_reduce_from_disease = 1;
      }

      op->protected|=tmp->protected;
      op->vulnerable|=tmp->vulnerable;
      if (tmp->type!=BOW) {
	op->attacktype|=tmp->attacktype;
/*        if (tmp->attacktype) LOG(llevDebug,"Object %s applied, attacktype=0x%x\n", tmp->name, tmp->attacktype);*/
      }
      op->immune|=tmp->immune;
      op->path_attuned|=tmp->path_attuned;
      op->path_repelled|=tmp->path_repelled;
      op->path_denied|=tmp->path_denied;
      op->stats.luck+=tmp->stats.luck;
      if(QUERY_FLAG(tmp,FLAG_LIFESAVE))
        SET_FLAG(op,FLAG_LIFESAVE);
      if(QUERY_FLAG(tmp,FLAG_REFL_SPELL))
        SET_FLAG(op,FLAG_REFL_SPELL);
      if(QUERY_FLAG(tmp,FLAG_REFL_MISSILE))
        SET_FLAG(op,FLAG_REFL_MISSILE);
      if(QUERY_FLAG(tmp,FLAG_STEALTH))
        SET_FLAG(op,FLAG_STEALTH);
      if(QUERY_FLAG(tmp,FLAG_UNDEAD)&&!QUERY_FLAG(&op->arch->clone,FLAG_UNDEAD))
        SET_FLAG(op,FLAG_UNDEAD);
      if(QUERY_FLAG(tmp,FLAG_MAKE_INVIS)) {
        SET_FLAG(op,FLAG_MAKE_INVIS); op->invisible=1; }

      if(QUERY_FLAG(tmp,FLAG_FLYING)) {
        SET_FLAG(op,FLAG_FLYING);
        if(!QUERY_FLAG(op,FLAG_WIZ))
          max=1;
      }
      if(QUERY_FLAG(tmp,FLAG_XRAYS))
        SET_FLAG(op,FLAG_XRAYS);
	
      if(QUERY_FLAG(tmp,FLAG_BLIND)) 
	SET_FLAG(op,FLAG_BLIND);

      if(QUERY_FLAG(tmp,FLAG_SEE_IN_DARK)) 
	SET_FLAG(op,FLAG_SEE_IN_DARK);

      if(tmp->stats.exp 
#ifdef ALLOW_SKILLS /* we get BIG problems w/o this line! */
	   && tmp->type!=EXPERIENCE
#endif
      ) {
        if(tmp->stats.exp > 0) {
          added_speed+=(float)tmp->stats.exp/3.0;
          bonus_speed+=1.0+(float)tmp->stats.exp/3.0;
        } else
          added_speed+=(float)tmp->stats.exp;
      }
      switch(tmp->type) {

     /* EXPERIENCE objects. What we are doing here is looking for "relevant" 
      * experience objects. Some of these will be used to calculate 
      * level-based changes in player status. For expample, the 
      * experience object which has exp_obj->stats.Str set controls the 
      * wc bonus of the player. -b.t.
      */
#ifdef ALLOW_SKILLS
      case EXPERIENCE: 
	if(op->type!=PLAYER)    /* Only players should have these. */ 
	  LOG(llevError,"Error: %s has exp_obj in invenory\n",op->name); 
	else {
	   if (tmp->stats.Str && !wc_obj) 
		wc_obj = tmp;
	   if (tmp->stats.Con && !hp_obj) 
		hp_obj = tmp;
	   if (tmp->stats.Pow && !mana_obj)  /* for spellpoint determ */ 
		mana_obj = tmp;
	   if (tmp->stats.Wis && !grace_obj)
		grace_obj = tmp; 
	}
        break;
			
      case SKILL: 		/* skills modifying the character -b.t. */ 
				/* for all skills and skill granting objects */ 

	if(tmp==op->chosen_skill) { 

	  wc-=tmp->stats.wc; 

	  if(tmp->stats.dam>0) { 	/* skill is a 'weapon' */ 
	    if(!QUERY_FLAG(op,FLAG_READY_WEAPON)) 
                weapon_speed = (int) WEAPON_SPEED(tmp);
	    if(weapon_speed<0) weapon_speed = 0;
            weapon_weight=tmp->weight;
            op->stats.dam+=tmp->stats.dam*(1 + (op->chosen_skill->level/9));
	    if(tmp->magic) op->stats.dam += tmp->magic;
	  }

          if(tmp->stats.wc)
            wc-=(tmp->stats.wc+tmp->magic);

          if(tmp->armour)
            op->armour+=((100-op->armour)*tmp->armour)/100;

          if(tmp->slaying!=NULL) {
            if (op->slaying != NULL)
              free_string (op->slaying);
            add_refcount(op->slaying = tmp->slaying);
          }

	  if(tmp->stats.ac)
            op->stats.ac-=(tmp->stats.ac+tmp->magic);

#ifdef SPELL_ENCUMBRANCE
        if(op->type==PLAYER) op->contr->encumbrance+=(int)3*tmp->weight/1000;
#endif
	}
	break;
#endif /* ALLOW_SKILLS */
      case SHIELD:
#ifdef SPELL_ENCUMBRANCE
	if(op->type==PLAYER) op->contr->encumbrance+=(int)tmp->weight/2000;
#endif
      case RING:
      case AMULET:
      case GIRDLE:
      case HELMET:
      case BOOTS:
      case GLOVES:
      case CLOAK:
        if(tmp->armour)
          op->armour+=((100-op->armour)*tmp->armour)/100;
        if(tmp->stats.wc)
          wc-=(tmp->stats.wc+tmp->magic);
        if(tmp->stats.dam)
          op->stats.dam+=(tmp->stats.dam+tmp->magic);
        if(tmp->stats.ac)
          op->stats.ac-=(tmp->stats.ac+tmp->magic);
        break;
      case WEAPON:
        wc-=(tmp->stats.wc+tmp->magic);
        if(tmp->stats.ac&&tmp->stats.ac+tmp->magic>0)
          op->stats.ac-=tmp->stats.ac+tmp->magic;
        if(tmp->armour)
          op->armour+=((100-op->armour)*tmp->armour)/100;
        op->stats.dam+=(tmp->stats.dam+tmp->magic);
        weapon_weight=tmp->weight;
        weapon_speed=((int)WEAPON_SPEED(tmp)*2-tmp->magic)/2;
        if(weapon_speed<0) weapon_speed=0;
        if(tmp->slaying!=NULL) {
          if (op->slaying != NULL)
            free_string (op->slaying);
          add_refcount(op->slaying = tmp->slaying);
        }
#ifdef SPELL_ENCUMBRANCE
	if(op->type==PLAYER) op->contr->encumbrance+=(int)3*tmp->weight/1000;
#endif
        break;
      case ARMOUR: /* Only the best of these three are used: */
#ifdef SPELL_ENCUMBRANCE
	if(op->type==PLAYER) op->contr->encumbrance+=(int)tmp->weight/1000;
#endif
      case BRACERS:
      case FORCE:
        if(tmp->armour)
          op->armour+=((100-op->armour)*tmp->armour)/100;
        if(tmp->stats.wc) { 
          if(best_wc<tmp->stats.wc+tmp->magic) {
             wc+=best_wc;
            best_wc=tmp->stats.wc+tmp->magic;
	  } else
	    wc+=tmp->stats.wc+tmp->magic;
	}
        if(tmp->stats.ac) {
          if(best_ac<tmp->stats.ac+tmp->magic) {
            op->stats.ac+=best_ac; /* Remove last bonus */
            best_ac=tmp->stats.ac+tmp->magic;
          }
          else /* To nullify the below effect */
            op->stats.ac+=tmp->stats.ac+tmp->magic;
        }
        if(tmp->stats.wc) wc-=(tmp->stats.wc+tmp->magic);
        if(tmp->stats.ac) op->stats.ac-=(tmp->stats.ac+tmp->magic);
        if(ARMOUR_SPEED(tmp)&&ARMOUR_SPEED(tmp)/10.0<max)
          max=ARMOUR_SPEED(tmp)/10.0;
        break;
      }
    } /* Item is equipped - end of for loop going through items. */

  /* awarding hp -- I changed this so that the mean between the 'fighter'
   * level (hp_obj->level) and overall scores are used. The system of hp
   * progression remains unchanged for no skills. b.t.
   */
  if(op->type==PLAYER) {
    int pl_level;
    check_stat_bounds(&(op->stats));
    if(!hp_obj) hp_obj = op; /* happens when skills are not used */ 
#if 0
    pl_level = (op->level + hp_obj->level)/2;
#else
    pl_level=op->level;
#endif
    if(pl_level<1) pl_level=1; /* safety, we should always get 1 levels worth of hp! */ 
    for(i=1,op->stats.maxhp=0;i<=pl_level&&i<=10;i++) {
      j=op->contr->levhp[i]+con_bonus[op->stats.Con]/2;
      if(i%2&&con_bonus[op->stats.Con]%2)
        j++;
      op->stats.maxhp+=j>1?j:1;
    }
    for(i=11;i<=op->level;i++)
      op->stats.maxhp+=2;
    if(op->stats.hp>op->stats.maxhp)
      op->stats.hp=op->stats.maxhp;

    /* Sp gain is controlled by the level of the player's 
     * relevant experience object (mana_obj, see above) 
     */ 
	/* following happen when skills system is not used */
    if(!mana_obj) mana_obj = op;
    if(!grace_obj) grace_obj = op;

	 /* set maxsp */
    if(!mana_obj || !mana_obj->level || op->type!=PLAYER) mana_obj = op;
    for(i=1,op->stats.maxsp=0;i<=mana_obj->level&&i<=10;i++) {
      j=op->contr->levsp[i]+sp_bonus[op->stats.Pow]/2;
      if((i%2) && (sp_bonus[op->stats.Pow]%2)) {
	if (sp_bonus[op->stats.Pow]>0)
            j++;
	else
	    j--;
      }
      op->stats.maxsp+=j>1?j:1;
    }
    for(i=11;i<=mana_obj->level;i++)
      op->stats.maxsp+=2;
    /* Characters can get their sp supercharged via rune of transferrance */
    if(op->stats.sp>op->stats.maxsp*2)
      op->stats.sp=op->stats.maxsp*2;

    /* set maxgrace, notice 3-4 lines below it depends on both Wis and Pow */
    if(!grace_obj || !grace_obj->level || op->type!=PLAYER) grace_obj = op;

    /* store grace in a float - this way, the divisions below don't create
     * big jumps when you go from level to level - with int's, it then
     * becomes big jumps when the sums of the bonuses jump to the next
     * step of 8 - with floats, even fractional ones are useful.
     */
    sp_tmp=0.0;
    for(i=1,op->stats.maxgrace=0;i<=grace_obj->level&&i<=10;i++) {
	float grace_tmp=0.0;

	grace_tmp=(float)op->contr->levgrace[i]
		+((float)grace_bonus[op->stats.Pow] + 
		  (float)grace_bonus[op->stats.Wis])/8.0;
	/* changed the starting grace for playability -b.t. */ 
	if(i<2) 
	    grace_tmp += 1.0+(((float)grace_bonus[op->stats.Pow] + 
		(float)grace_bonus[op->stats.Wis])/4.0); 
	 /*  I'll omit the odd even stuff from above for now  --PeterM */
	if (grace_tmp<1.0) grace_tmp=1.0;
	sp_tmp+=grace_tmp;
    }
    op->stats.maxgrace=(int)sp_tmp;
    /* one grace point per level after 11 */
    for(i=11;i<=grace_obj->level;i++)
	op->stats.maxgrace+=2;

     /* I'll also allow grace to be larger than the maximum, for who am I
      * to put limits on the whims of the gods?  I omit any fix for overlarge
      * grace--PeterM */

    if(op->contr->braced)
      op->stats.ac+=2;
    else
      op->stats.ac-=dex_bonus[op->stats.Dex];

   /* In new exp/skills system, wc bonuses are related to 
    * the players level in a relevant exp object (wc_obj)
    * not the general player level -b.t.  */
   /* I changed this slightly so that wc bonuses are better
    * than before. This is to balance out the fact that 
    * the player no longer gets a personal weapon w/ 1
    * improvementevery level, now its fighterlevel/5. So
    * we give the player a bonus here in wc and dam
    * to make up for the change. Note that I left the 
    * monster bonus the same as before. -b.t.
    */

#ifdef ALLOW_SKILLS
    if(op->type==PLAYER && wc_obj && wc_obj->level>1) { 
      int i;
      wc-=(wc_obj->level+thaco_bonus[op->stats.Str]);
      for(i=1;i<wc_obj->level;i++) { 
	/* addtional wc every 6 levels */ 
	if(!(i%6)) wc--; 
	/* addtional dam every 4 levels. */ 
	if(!(i%4)&&!(dam_bonus[op->stats.Str]<0)) 
	    op->stats.dam+=(1+(dam_bonus[op->stats.Str]/5));
      }
    } else 
#endif /* ALLOW_SKILLS */ 
    wc-=(op->level+thaco_bonus[op->stats.Str]);

    op->stats.dam+=dam_bonus[op->stats.Str];
    if(op->contr->braced)
      wc+=4;
    if(op->stats.dam<1)
      op->stats.dam=1;
    op->speed=1.0+speed_bonus[op->stats.Dex];
#ifdef SEARCH_ITEMS
    if (op->contr->search_str[0])
      op->speed -= 1;
#endif
    if (op->attacktype==0)
	op->attacktype=op->arch->clone.attacktype;
  }
  if(added_speed>=0)
    op->speed+=added_speed/10.0;
  else /* Something wrong here...: */
    op->speed /= (float)(1.0-added_speed);
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
  if (op->speed<0.01 && op->type==PLAYER) op->speed=0.01;

  if(op->type == PLAYER) {
/* (This formula was made by vidarl@ifi.uio.no) */
    M=(max_carry[op->stats.Str]-121)/121.0;
    M2=max_carry[op->stats.Str]/100.0;
    W=weapon_weight/20000.0;
    s=2-weapon_speed/10.0;
    D=(op->stats.Dex-14)/14.0;
    K=1 + M/3.0 - W/(3*M2) + op->speed/5.0 + D/2.0;
    K*=(4+op->level)/(float)(6+op->level)*1.2;
    if(K<=0) K=0.01;
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
  if (wc>120) wc=120;
  else if (wc<-120) wc=-120;
  op->stats.wc=wc;

  update_ob_speed(op);

}

/*
 * Returns true if the given player is a legal class.
 * The function to add and remove class-bonuses to the stats doesn't
 * check if the stat becomes negative, thus this function
 * merely checks that all stats are 1 or more, and returns
 * false otherwise.
 */

int allowed_class(object *op) {
  return op->stats.Dex>0&&op->stats.Str>0&&op->stats.Con>0&&
         op->stats.Int>0&&op->stats.Wis>0&&op->stats.Pow>0&&
	 op->stats.Cha>0;
}

/*
 * Returns how much experience is needed for a player to become
 * the given level.
 */

uint32 level_exp(int level,double expmul) {
    static long int bleep=1650000; 

    if(level<=100) {
	if (settings.simple_exp) {
	    return expmul * new_levels[level];
	} else {
	    return expmul * levels[level];
	}
    }

    /*  return required_exp; */
    if (settings.simple_exp) {
	return expmul*(new_levels[100]+bleep*(level-100));
    } else {
	return expmul*(levels[100]+bleep*(level-100));
    }
}

#ifdef ALLOW_SKILLS /* new experience system */ 

/* add_exp() - new algorithm. Revamped experience gain/loss routine. 
 * Based on the old add_exp() function - but tailored to add experience 
 * to experience objects. The way this works-- the code checks the 
 * current skill readied by the player (chosen_skill) and uses that to
 * identify the appropriate experience object. Then the experience in
 * the object, and the player's overall score are updated. In the case
 * of exp loss, all exp categories which have experience are equally
 * reduced. The total experience score of the player == sum of all 
 * exp object experience.  - b.t. thomas@astro.psu.edu 
 */
 
void add_exp(object *op, int exp) {
    object *exp_ob=NULL; /* the object into which experience will go */ 
    object *tmp,*pl_exp[MAX_EXP_CAT];
    int old_exp,del_exp=0,nrofexp=0,i;

    /* safety */
    if(!op) { 
	LOG(llevError,"add_exp() called for null object!\n"); 
	return; 
    }

#ifdef EXP_DEBUG
    LOG(llevDebug,"add_exp() called for %s, exp = %d\n",query_name(op),exp); 
#endif

    if(op->type != PLAYER) { 		/* for Monsters only */
	/* Sanity check */
	if (!QUERY_FLAG(op, FLAG_ALIVE)) return;
	exp =  adjust_exp(op,exp);
    }
    else {				/* Players only */ 
	if(exp>0) { /* ADDING exp (to one object) */ 
	    int limit=0;

	    if(!op->chosen_skill) { 
		LOG(llevError,"add_exp() called for %s w/ no ready skill.\n",op->name);
		return;
	    } else if(!op->exp_obj && !op->chosen_skill->exp_obj) { 
		/* This shouldn't be an error - killing monsters via scrolls
		 * will get here, because use_magic skill doesn't give exp -
		 * this means things like rods and scrolls.
		 */
		LOG(llevDebug,"add_exp() called for skill w/o exp obj (%s), .\n", 
		    op->chosen_skill->name);
		return;
	    }

	    /* if op->exp_obj is set, then the player has killed with an 
	     * animated object cf. fireball */
	    if(op->exp_obj)  
		exp_ob = op->exp_obj;
	    else
		exp_ob = op->chosen_skill->exp_obj; 

	    /* General adjustments for playbalance */ 

	    /* I changed this from the old algorithm. Formerly,
	     * if the experience you would have gained was 
	     * greater than 1/2 your TOTAL experience, you only
	     * gained 1/2 the award (which could *still be greater
	     * than 1/2 your total exp!) (Actually, incorrect.  Previously
	     * if it was greater than half your total, you would gain half your
	     * your total - mw) . Now, exp gain is limited
	     * so the exp award can be no *more than* 1/2 of 
	     * the exp is takes to get from your present level to 
	     * the next one (basically, no one can learn faster than
	     * that!).
	     */

	    if(exp_ob->level < MAXLEVEL) { 
		if (settings.simple_exp) {
		    limit=(new_levels[exp_ob->level+1]-new_levels[exp_ob->level])/2;
		} else {
		    limit=(levels[exp_ob->level+1]-levels[exp_ob->level])/2;
		}
		if (exp > limit) exp=limit;
	    } else { /* there is no going any higher! */ 
		return;
	    }

	    /* prevents some forms of abuse */
	    if(op->type==PLAYER && op->contr->braced) exp=exp/5; 

	    /* Adding exp properly - we have to make sure that:
	     *	1) op->stats.exp<MAX_EXPERIENCE 
	     *	2) exp_ob->stats.exp<MAX_EXP_IN_OBJ
	     * 	3) sum of exp obj experience = op->stats.exp
	     */

	    exp = adjust_exp(op,exp);	/* op->stats.exp < MAX_EXPERIENCE */ 
	    old_exp = exp;	
	    exp = adjust_exp(exp_ob,exp);   /* exp_ob->stats.exp < MAX_EXP_IN_OBJ */ 
					    /* check and  adjustment */ 
	    if(old_exp>exp) op->stats.exp+=(exp-old_exp); 
	    player_lvl_adj(op,NULL);   
	    player_lvl_adj(op,exp_ob);   
  
	} else if (op->type ==PLAYER && exp < 0) { /* SUBTRACT exp (from all exp obj) */ 
	    float fraction = (float) exp/(float) op->stats.exp;

	    /* Sub exper - we will remove the same fraction of experience
	     * from all experience objects. 
	     */

	    for(tmp=op->inv;tmp;tmp=tmp->below)
		if(tmp->type==EXPERIENCE && tmp->stats.exp) { 
		    pl_exp[nrofexp] = tmp;
		    nrofexp++; 
		}

	    /* we do experience objects first here */
	    for(i=0;i<nrofexp;i++) { 
		del_exp += adjust_exp(pl_exp[i],(pl_exp[i]->stats.exp*fraction)); 
		player_lvl_adj(op,pl_exp[i]); /* adj exp object */ 
	    }

	    (void) adjust_exp(op, del_exp);
	    player_lvl_adj(op,NULL); 

	} else if (op->type !=PLAYER && exp < 0) { /* SUBTRACT monster exp */ 

	    (void) adjust_exp(op, exp);
	    player_lvl_adj(op,NULL);

	} else { /* No exp gain, but check lvl adj */ 

	    if(op->chosen_skill && op->chosen_skill->exp_obj)
		player_lvl_adj(op, op->chosen_skill->exp_obj);
	    player_lvl_adj(op,NULL); 
	}

	/* reset the player exp_obj to NULL */
	if(op->exp_obj) op->exp_obj = NULL;
    }
}

#else /*ALLOW_SKILLS */ /* add_exp() - the old exp system */

/*
 * Adds (or subtracts) experience to a living object.  If it is a player,
 * checks for level-gain/loss is done.
 * The routines for gaining/losing levels is also within this function.
 */

void add_exp(object *op,int exp) {
    char buf[MAX_BUF];

    if (exp > op->stats.exp / 2) {
	if (op->stats.exp < 100)
	    exp = 50;
	else
	    exp = op->stats.exp / 2;
    }

    if(op->type==PLAYER && op->contr->braced) exp=exp/5; 
    /* shuffled some stuff into here  -b.t. */
    exp = adjust_exp(op,exp);

    if(op->type==PLAYER) {
	if(op->level < MAXLEVEL && op->stats.exp >= level_exp(op->level+1,op->expmul)) {
	    op->level++;
	    if(op->level < 11) {

		op->contr->levhp[op->level] = (int) RANDOM()%4 + (int) RANDOM()%4 + 3;
		op->contr->levsp[op->level] = (int) RANDOM()%3 + (int) RANDOM()%3 + 2;
		  op->contr->levgrace[op->level]=(int)RANDOM()%2 + (int) RANDOM()%2 + 1;
	    }
	    fix_player(op);
	    if(op->level>1) {
		sprintf(buf,"You are now level %d.",op->level);
		(*draw_info_func)(NDI_UNIQUE, 0, op,buf);
	    }
	    add_exp(op,0); /* To increase more levels */
	} else if(op->level>1&&op->stats.exp<level_exp(op->level,op->expmul)) {
	    op->level--;
	    fix_player(op);
	    sprintf(buf,"You are now level %d.",op->level);
	    (*draw_info_func)(NDI_UNIQUE, 0, op,buf);
	    add_exp(op,0); /* To decrease more levels */
	}
    }
}    

#endif /* ALLOW_SKILLS */ 

/* player_lvl_adj() - for the new exp system. we are concerned with
 * whether the player gets more hp, sp and new levels.
 * -b.t.
 */

void player_lvl_adj(object *who, object *op) {
    char buf[MAX_BUF];

    if(!op)        /* when rolling stats */ 
	op = who;	
 
    if(op->level < MAXLEVEL && op->stats.exp >= level_exp(op->level+1,op->expmul)) {
	op->level++;

	if(who && (who->level < 11) && op->type!=EXPERIENCE) { 
	    who->contr->levhp[who->level] = (int) RANDOM()%4 + (int) RANDOM()%4 + 3;
	    who->contr->levsp[who->level] = (int) RANDOM()%3 + (int) RANDOM()%3 + 2;
			 who->contr->levgrace[who->level]=(int)RANDOM()%2 + (int) RANDOM()%2 + 1;
	}

	if(who) fix_player(who);
	if(op->level>1 && op->type==EXPERIENCE) {
	    sprintf(buf,"You are now level %d in %s based skills.",op->level,op->name);
	    if(who) (*draw_info_func)(NDI_UNIQUE|NDI_RED, 0, who,buf);
	}
	player_lvl_adj(who,op); /* To increase more levels */
    } else if(op->level>1&&op->stats.exp<level_exp(op->level,op->expmul)) {
	op->level--;
	if(who) fix_player(who);
	if(op->type==EXPERIENCE) {
	    sprintf(buf,"You are now level %d in %s based skills.",op->level,op->name);
	    if(who) (*draw_info_func)(NDI_UNIQUE|NDI_RED, 0, who,buf);
	}
	player_lvl_adj(who,op); /* To decrease more levels */
    }
}

/* adjust_exp() - make sure that we don't exceed max or min set on
 * experience
 */
 
int adjust_exp(object *op, int exp) {
    int max_exp = MAX_EXPERIENCE;

    op->stats.exp += exp;
    if(op->stats.exp < 0) {
	exp -= op->stats.exp;
	op->stats.exp = 0;
    }
    /* reset max_exp value if we have experience obj */
    if(op->type==EXPERIENCE) 
	max_exp = MAX_EXP_IN_OBJ;

    if(op->stats.exp>max_exp) {
	exp = exp - (op->stats.exp - max_exp);
	op->stats.exp=max_exp;
    }
    return exp;	/* return the actual amount changed stats.exp by */ 
}

/* check_dm_add_exp() - called from c_wiz.c. Needed by ALLOW_SKILLS
 * code. -b.t.
 */

int check_dm_add_exp_to_obj(object *exp_ob, int i) {

    if((exp_ob->stats.exp + i) < 0) 
	i= -1*(exp_ob->stats.exp);
    else if((exp_ob->stats.exp +i)> MAX_EXP_IN_OBJ)
	i= MAX_EXP_IN_OBJ - exp_ob->stats.exp;
    return i;
}

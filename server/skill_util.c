/*
 * static char *rcsid_skill_util_c =
 *   "$Id$";
 */
/*
    CrossFire, A Multiplayer game for X-windows

    Copryight (C) 2000 Mark Wedel
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

#include <global.h>
#include <object.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <living.h>     /* for defs of STR,CON,DEX,etc. -b.t.*/
#include <skills.h>
#include <skillist.h>
#include <spells.h>

/* table for stat modification of exp */

float stat_exp_mult[MAX_STAT+1]={
 0.0,
 0.01, 0.1, 0.3, 0.5,			/* 1 - 4 */
 0.6, 0.7, 0.8,				/* 5 - 7 */
 0.85, 0.9, 0.95, 0.96,			/* 8 - 11 */
 0.97, 0.98, 0.99,			/* 12 - 14 */
 1.0, 1.01, 1.02, 1.03, 1.04,		/* 15 - 19 */
 1.05, 1.07, 1.09, 1.12, 1.15,		/* 20 - 24 */
 1.2, 1.3, 1.4, 1.5, 1.7, 2.0
};


/* do_skill() - Main skills use function-similar in scope to cast_spell(). 
 * We handle all requests for skill use outside of some combat here. 
 * We require a separate routine outside of fire() so as to allow monsters 
 * to utilize skills. Success should be the amount of experience gained 
 * from the activity. Also, any skills that monster will use, will need
 * to return a positive value of success if performed correctly. Usually
 * this is handled in calc_skill_exp(). Thus failed skill use re-
 * sults in a 0, or false value of 'success'.
 *  - b.t.  thomas@astro.psu.edu
 */

int do_skill (object *op, int dir, char *string) {
  int success=0;        /* needed for monster_skill_use() too */
  int skill = op->chosen_skill->stats.sp;

    switch(skill) {
	 case SK_LEVITATION:
		if(QUERY_FLAG(op,FLAG_FLYING)) { 
		  CLEAR_FLAG(op,FLAG_FLYING);
		  new_draw_info(NDI_UNIQUE,0,op,"You come to earth.");
		}
		else {
		  SET_FLAG(op,FLAG_FLYING);
		  new_draw_info(NDI_UNIQUE,0,op,"You rise into the air!.");
		}
		  			 
		break;
      case SK_STEALING:
        success = steal(op, dir);
        break;
      case SK_LOCKPICKING:
        success = pick_lock(op, dir);
        break;
      case SK_HIDING:
        success = hide(op);
        break;
      case SK_JUMPING:
        success = jump(op, dir);
        break;
      case SK_INSCRIPTION:
        success = write_on_item(op,string);
        break;
      case SK_MEDITATION:
        meditate(op);
        break;
	/* note that the following 'attack' skills gain exp through hit_player() */
      case SK_KARATE:
        (void) attack_hth(op,dir,"karate-chopped");
        break;
      case SK_BOXING:
        (void) attack_hth(op,dir,"punched");
        break;
      case SK_FLAME_TOUCH:
        (void) attack_hth(op,dir,"flamed");
        break;
      case SK_CLAWING:
        (void) attack_hth(op,dir,"clawed");
        break;
      case SK_MELEE_WEAPON:
        (void) attack_melee_weapon(op,dir,NULL);
        break;
      case SK_FIND_TRAPS:
        success = find_traps(op);
        break;
      case SK_MUSIC:
        success = singing(op,dir);
        break;
      case SK_ORATORY:
        success = use_oratory(op,dir);
        break;
      case SK_SMITH:
      case SK_BOWYER:
      case SK_JEWELER:
      case SK_ALCHEMY:
      case SK_THAUMATURGY:
      case SK_LITERACY:
      case SK_DET_MAGIC:
      case SK_DET_CURSE:
      case SK_WOODSMAN:
        success = skill_ident(op);
        break;
      case SK_REMOVE_TRAP:
        success = remove_trap(op,dir);
        break;
      case SK_THROWING:
	success = skill_throw(op,dir,string);
	break;
      case SK_SET_TRAP:
           new_draw_info(NDI_UNIQUE, 0,op,"This skill is not currently implemented.");
        break;
      case SK_USE_MAGIC_ITEM:
      case SK_MISSILE_WEAPON:
           new_draw_info(NDI_UNIQUE, 0,op,"There is no special attack for this skill.");
        break;
      case SK_PRAYING:
	success = pray(op);
	break;
      case SK_SPELL_CASTING:
      case SK_CLIMBING:
      case SK_BARGAINING:
           new_draw_info(NDI_UNIQUE, 0,op,"This skill is already in effect.");
        break;
      default:
        LOG(llevDebug,"%s attempted to use unknown skill: %d\n"
                ,query_name(op), op->chosen_skill->stats.sp);
        break;
    }
 
   /* For players we now update the speed_left from using the skill. 
    * Monsters have no skill use time because of the random nature in 
    * which use_monster_skill is called already simulates this. -b.t.
    */

    if(op->type==PLAYER) op->speed_left -= get_skill_time(op,skill);

   /* this is a good place to add experience for successfull use of skills.
    * Note that add_exp() will figure out player/monster experience
    * gain problems.
    */
 
    if(success&&skills[skill].category!=EXP_NONE) add_exp(op,success);
            
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
 *  Example: the basic exp gain for player 'who' how "vanquished" oppoenent
 * 'op' using skill 'sk' is:
 *
 *      EXP GAIN = (op->exp + skills[sk].bexp) * lvl_mult * stat_mult
 *
 *  where lvl_mult is
 *
 *  for(pl->level < op->level):: 
 *    lvl_mult  = skills[sk].lexp * (op->level - pl->level) 
 *  for(pl->level = op->level):: 
 *    lvl_mult  = skills[sk].lexp; 
 *  for(pl->level > op->level):: 
 *    lvl_mult = op_lvl/pl_lvl;
 *
 *  and stat_mult is taken from stat_exp_mult[] table above.
 *
 * Coded by b.t. thomas@astro.psu.edu
 */

int calc_skill_exp(object *who, object *op) {
    int who_lvl= SK_level(who);
    int sk,op_exp=0,op_lvl= 0;
    float base,value,stat_mult=1.0,lvl_mult=0.0;

    /* sometimes we dont have a specific opponent to 'overcome'.
     * 'hiding' is just such a case. For these skills, our
     * 'opponent is the map we are on. Thus experience
     * gain is just skills[sk].bexp, and modified by the
     * map difficulty instead of an opponent SK_level. 
     * In the case that map difficulty =< player then op_lvl = 0
     */

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
    } else if(op->type==RUNE) { /* all traps. If stats.Cha > 1 we use that
				 * for the amount of experience */
        op_exp = op->stats.Cha>1 ? op->stats.Cha : op->stats.exp;
        op_lvl = op->level;
    } else { 		/* all other items/living creatures */ 
        op_exp = op->stats.exp;
        if(!QUERY_FLAG(op,FLAG_ALIVE)) { /* for ident/make items */ 
	   op_lvl = op->level;
	    /* This makes no sense, lets simplify it */
	    /*op_lvl += op->magic ? 0: (5 * abs(op->magic));*/
	    op_lvl += 5 * abs(op->magic);
	} else /* its a monster, grab its level */
	   op_lvl= SK_level(op);
    }

    if(op_lvl<1) op_lvl = 1;

    if(who->type!=PLAYER) {             /* for monsters only */
        return ((int) (op_exp*0.1)+1);	/* we add one to insure positive value is returned */
    } else {                            /* for players */
      /* FIRST, what skill are we using? */
      if(who->chosen_skill==NULL) {
          LOG(llevError,"Bad call of calc_skill_exp() by player. \n");
          return 0;
      } else
          sk = who->chosen_skill->stats.sp;

      base = op_exp + skills[sk].bexp;  /* get base exp */


  /* get level multiplier */
      if(who_lvl < op_lvl)        
          lvl_mult=(float) skills[sk].lexp * (float) ((float) op_lvl - (float) who_lvl);
      else if(who_lvl == op_lvl)        
          lvl_mult = (float) skills[sk].lexp;
      else /* player is higher level than the object */
          lvl_mult = ((float) ((float) op_lvl)/((float) who_lvl));

      stat_mult = calc_stat_mult(who,sk);
    }
 
    /* assemble the exp total, and return value */
 
    value =  base * (lvl_mult * stat_mult);
    
#ifdef SKILL_UTIL_DEBUG
      LOG(llevDebug,"calc_skill_exp(): who: %s(lvl:%d)  op:%s(lvl:%d)\n",
		who->name,who_lvl,op->name,op_lvl);
      LOG(llevDebug,"%s(%s): base_exp=%f lvl_mult=%f stat_mult=%f value=%f\n",
                who->name,skills[sk].name,base,lvl_mult,stat_mult,value);
#endif
    return ( (int) value); 
}


/* calc_stat_mult() - figures a factor to modify gained experience by
 * based on the sum of relvant stats of skill 'sk' being used by 'who'.
 */
 
float calc_stat_mult(object *who,int sk) {
    int sum;
    float factor = 1.0;

    if (settings.simple_exp) return 1.0;
         
    sum = get_weighted_skill_stat_sum (who,sk);
 
    /* look up factor for weighted sum */
    if(sum <MAX_STAT) factor = stat_exp_mult[sum];
 
    return factor;
}

/* find relevant stats or a skill then return their weighted sum. 
 * I admit the calculation is done in a retarded way.
 * If stat1==NO_STAT_VAL this isnt an associated stat. Returns
 * zero then. -b.t. 
 */  

int get_weighted_skill_stat_sum ( object *who, int sk) {
   float sum;
   int number = 1;
 
    if(skills[sk].stat1==NO_STAT_VAL) {
          return 0;
    } else
         sum = get_attr_value(&(who->stats),skills[sk].stat1);

    if(skills[sk].stat2!=NO_STAT_VAL) {
         sum += get_attr_value(&(who->stats),skills[sk].stat2);
         number++;
    }

    if(skills[sk].stat3!=NO_STAT_VAL) {
         sum += get_attr_value(&(who->stats),skills[sk].stat3);
         number++;
    }

    return ((int) sum/number);
}

/* init_new_exp_system() - called in init(). This function will reconfigure
 * the properties of skills array and experience categories, and links
 * each skill to the appropriate experience category
 * -b.t. thomas@astro.psu.edu
 */

void init_new_exp_system() {
  static int init_new_exp_done = 0;

  if (init_new_exp_done)
    return;
  init_new_exp_done = 1;
 
  init_exp_obj();     /* locate the experience objects and create list of them */
  link_skills_to_exp(); /* link skills to exp cat, based on shared stats */
  (void) read_skill_params(); /* overide skills[] w/ values from skill_params */ 

}
void dump_skills()
{
    char buf[MAX_BUF];
    int i;

    fprintf(stderr, "\n");
    fprintf(stderr, "exper_catgry \t str \t dex \t con \t wis \t cha \t int \t pow \n");
    for (i = 0; i < nrofexpcat; i++)
      fprintf(stderr, "%d-%s \t %d \t %d \t %d \t %d \t %d \t %d \t %d \n",
		i,exp_cat[i]->name,
                exp_cat[i]->stats.Str,exp_cat[i]->stats.Dex,exp_cat[i]->stats.Con,
                exp_cat[i]->stats.Wis,exp_cat[i]->stats.Cha,exp_cat[i]->stats.Int, 
		exp_cat[i]->stats.Pow);
 
    fprintf(stderr, "\n");
    sprintf(buf,"%20s  %12s  %4s %4s %4s  %5s %5s %5s\n",
	"sk#       Skill name","ExpCat","Time","Base","xlvl","Stat1","Stat2","Stat3");
    fprintf(stderr, buf);
    sprintf(buf,"%20s  %12s  %4s %4s %4s  %5s %5s %5s\n",
	"---       ----------","------","----","----","----","-----","-----","-----");
    fprintf(stderr, buf);
    for (i = 0; i < NROFSKILLS; i++) {
      sprintf(buf,"%2d-%17s  %12s  %4d %4ld %4g  %5s %5s %5s\n", i, skills[i].name,
        exp_cat[skills[i].category]!=NULL?exp_cat[skills[i].category]->name:"NONE",
        skills[i].time,
        skills[i].bexp,
        skills[i].lexp,
        skills[i].stat1!= NO_STAT_VAL ? short_stat_name[skills[i].stat1]: "---",
        skills[i].stat2!= NO_STAT_VAL ? short_stat_name[skills[i].stat2]: "---",
        skills[i].stat3!= NO_STAT_VAL ? short_stat_name[skills[i].stat3]: "---");
      fprintf(stderr, buf); 
    }
    exit(0);
}

/* init_exp_obj() - this routine looks through all the assembled
 * archetypes for experience objects then copies their values into
 * the exp_cat[] array. - bt.
 */
 
void init_exp_obj() {
    archetype *at;

    nrofexpcat = 0;
    for(at=first_archetype;at!=NULL;at=at->next)
        if(at->clone.type==EXPERIENCE) {
             exp_cat[nrofexpcat] = get_object() ;
	     exp_cat[nrofexpcat]->level = 1;
	     exp_cat[nrofexpcat]->stats.exp = 0;
             copy_object(&at->clone, exp_cat[nrofexpcat]);
             nrofexpcat++;
             if(nrofexpcat == MAX_EXP_CAT) {
                 LOG(llevError,"Aborting! Reached limit of available experience\n");
                 LOG(llevError,"categories. Need to increase value of MAX_EXP_CAT.\n");
                 exit (0);
             }
        }
 
    if(nrofexpcat == 0) {
        LOG(llevError,"Aborting! No experience objects found in archetypes.\n");
        exit (0);
    } 
}
/* link_skills_to_exp() - this links the skills in skills[] to the appropriate
 * experience category.  Linking is based only on the stats (1,2,3) of the
 * skill. If none of the stats assoc. w/ a skill match those in any experience 
 * object then the skill becomes 'miscellaneous' -b.t.
 */
 
void link_skills_to_exp() {
   int i=0,j=0;
 
        for(i=0;i<NROFSKILLS;i++)
            for(j=0;j<nrofexpcat;j++)
                if(check_link(skills[i].stat1,exp_cat[j])) {
                        skills[i].category = j;
                        continue;
                } else if(skills[i].category == EXP_NONE 
			&& check_link(skills[i].stat2,exp_cat[j])) {
                        skills[i].category = j;
                        continue;
                } else if(skills[i].category == EXP_NONE 
			&& check_link(skills[i].stat3,exp_cat[j])) {
                        skills[i].category = j;
                        continue;
        	/* failed to link, set to EXP_NONE */
                } else if (j==nrofexpcat || skills[i].stat1 == NO_STAT_VAL) {
                        skills[i].category = EXP_NONE;
                        continue;
                }

}

/* check_link() - this returns true if the specified stat is set in the
 * experience object. Wish this was more 'generalized'. Added new stats
 * will cause this routine to abort CF. - b.t.
 */
 
int check_link (int stat, object *exp) {
 
        switch (stat) {
            case STR:
                if(exp->stats.Str) return 1;
                break;
            case CON:
                if(exp->stats.Con) return 1;
                break;
            case DEX:
                if(exp->stats.Dex) return 1;
                break;
            case INT:
                if(exp->stats.Int) return 1;
                break;
            case WIS:
                if(exp->stats.Wis) return 1;
                break;
            case POW:
                if(exp->stats.Pow) return 1;
		break;
            case CHA:
                if(exp->stats.Cha) return 1;
                break;
            case NO_STAT_VAL:
                return 0;
            default:
                LOG(llevError, "Aborting! Tried to link skill with unknown stat!\n");
                exit (0);
        }
        return 0;
}        

/* read_skill_params() - based on init_spell_params(). This
 * function should read a file 'skill_params' in the /lib 
 * directory.  -b.t.
 *
 *  format of the file 'skill_params' is:
 *	name
 *      EXP_CAT, bexp, lexp, stat1, stat2, stat3
 * 
 *  see the values in lib/skill_params for more inspiration/direction
 */

void read_skill_params () {
  FILE *skill_params;
  char fname[MAX_BUF];
  char skill_name[256];
  char skill_attrib[256];
  int cat,bexp,time,stat1,stat2,stat3,skillindex;
  float lexp;
 
  sprintf(fname,"%s/%s",settings.datadir,"skill_params");
  LOG(llevDebug,"Reading skill_params from %s...",fname);
  if(! (skill_params=fopen(fname,"r")))
    {
        perror(fname);
        return;
    }
 
  while(!feof(skill_params))
    {
	/* Large buf, so that long comments don't kill it. */
	fgets(skill_name,255,skill_params);
	if (*skill_name=='#') continue;
        skillindex=lookup_skill_by_name(skill_name);
        if(skillindex == -1) {
            LOG(llevError,"\nskill_params has unrecognized skill: %s",skill_name);
            continue;
        }
        fgets(skill_attrib,255,skill_params);
        sscanf(skill_attrib,"%d %d %d %f %d %d %d",
		&cat,&time,&bexp,&lexp,&stat1,&stat2,&stat3);
        skills[skillindex].category=cat;
        skills[skillindex].time=time;
        skills[skillindex].bexp=bexp;
        skills[skillindex].lexp=lexp;
        skills[skillindex].stat1=stat1;
        skills[skillindex].stat2=stat2;
        skills[skillindex].stat3=stat3;
    }
    fclose(skill_params);
    LOG(llevDebug,"done.\n");
}

/* lookup_skill_by_name() - based on look_up_spell_by_name - b.t.
 * Given name, we return the index of skill 'string' in the skill
 * array, -1 if no skill is found.
 */

int lookup_skill_by_name(char *string) {
  int skillnr=0, nmlen;
  char name[MAX_BUF];

  if(!string) return -1;
  
  strcpy(name, string);
  nmlen=strlen(name);

  for (skillnr=0; skillnr<NROFSKILLS; skillnr++) {
	if (!strncmp(name,skills[skillnr].name,MIN(strlen(skills[skillnr].name),
	    nmlen))) return skillnr;
  }
  return -1;
}
 
/* check_skill_to_fire() -  */

int check_skill_to_fire(object *who) {
  int skillnr=0;
  rangetype shoottype=range_none;

  if(who->type!=PLAYER) return 1; 

  switch((shoottype = who->contr->shoottype)) {
    case range_bow:
       skillnr = SK_MISSILE_WEAPON;
       break;
    case range_none:
    case range_skill:
       return 1; 
       break;
    case range_magic:
        if(spells[who->contr->chosen_spell].cleric)
            skillnr = SK_PRAYING;
        else
            skillnr = SK_SPELL_CASTING;
        break;
    case range_scroll:
    case range_rod:
    case range_horn:
    case range_wand:
       skillnr = SK_USE_MAGIC_ITEM;
       break;
    default:
       LOG(llevError,"Warning: bad call of check_skill_to_fire()\n");
       return 0;
  }
  if (change_skill(who,skillnr)) { 
#ifdef SKILL_UTIL_DEBUG
      LOG(llevDebug,"check_skill_to_fire(): got skill:%s for %s\n"
        ,skills[skillnr].name,who->name);
#endif
    who->contr->shoottype=shoottype;
    return 1;
  } else 
    return 0;
}

/* check_skill_to_apply() - When a player tries to use an
 * object which requires a skill this function is called.
 * (examples are weapons like swords and bows)
 * It does 2 things: checks for appropriate skill in player inventory
 * and alters the player status by making the appropriate skill
 * the new chosen_skill. This routine works for both players
 * applying and unapplying objects. Returns true if the skill exists
 * and could be readied or we successfully unapplyied an object.
 * -bt. thomas@astro.psu.edu
 */

/* IMPORTANT NOTE: always call this fctn *after* FLAG_APPLY is set/unset
 * and *before* change_abil() is called.
 */
 
int check_skill_to_apply(object *who, object *item) {
    int skill=0;
    rangetype shoottype=range_none;
 
    if(who->type!=PLAYER) return 1; /* this fctn only for players */

/* first figure out the required skill and ending shoottype          
 * from the item */
 
    switch(item->type) {
      case WEAPON:
        skill = SK_MELEE_WEAPON;
        shoottype = range_skill;
        break;
      case BOW:
        skill = SK_MISSILE_WEAPON;
        shoottype = range_bow;
        break;
      case POTION:
      case SCROLL:
        skill = SK_USE_MAGIC_ITEM; /* not literacy! */
        shoottype = range_scroll;
	break;
      case ROD:
        skill = SK_USE_MAGIC_ITEM;
        shoottype = range_rod; 
	break;
      case WAND:                
        skill = SK_USE_MAGIC_ITEM;
        shoottype = range_wand; 
        break;
      case HORN:                
        skill = SK_USE_MAGIC_ITEM;
        shoottype = range_horn;
        break;
      default:
        LOG(llevDebug,"Warning: bad call of check_skill_to_apply()\n");
        LOG(llevDebug,"No skill exists for item: %s\n",query_name(item));
        return 0;
    }
 
/* now we alter player status appropriately */
 
    if(!QUERY_FLAG(item,FLAG_APPLIED)) {     /* we are trying to unapply */
 
        if(who->chosen_skill && who->chosen_skill->stats.sp == skill)
           (void) change_skill(who,-1);
        return 0;
 
    } else {                               /* we are trying to apply */
 
        if(!who->chosen_skill || (who->chosen_skill
             && who->chosen_skill->stats.sp!=skill))
                if(!change_skill(who,skill)) {
                    new_draw_info(NDI_UNIQUE, 0,who,"You can't use that!");
                    CLEAR_FLAG(item,FLAG_APPLIED);
                    return 0;
                }
    }
 
    who->contr->shoottype=shoottype;
    return 1;
}

/* init_player_exp() - makes various checks and initialization of player 
 * experience objects. If things aren't cool then we change them here.
 *  -b.t.
 */

int init_player_exp(object *pl) {
  int i,j,exp_index=0;
  object *tmp,*exp_ob[MAX_EXP_CAT];

   if(pl->type!=PLAYER) {
        LOG(llevError, "init_player_exp(): called non-player %s.\n",pl->name);
	return 0;
   }
   /* first-pass find all current exp objects */
   for(tmp=pl->inv;tmp;tmp=tmp->below)
      if(tmp->type==EXPERIENCE) {
           exp_ob[exp_index] = tmp;
           exp_index++;
      } else if (exp_index == MAX_EXP_CAT)
    	   return 0;	 
 
   /* second - if pl has wrong nrof experience objects
    * then give player a complete roster.
    */   

  /*  No exp objects - situation for a new player 
   *  or pre-skills/exp code player file */
   if(!exp_index) {  
        for(j=0;j<nrofexpcat;j++) {
           tmp = get_object();
           copy_object(exp_cat[j],tmp);
           insert_ob_in_ob(tmp,pl);
	   tmp->stats.exp = pl->stats.exp/nrofexpcat;
           exp_ob[j] = tmp;
           esrv_send_item(pl, tmp);
           exp_index++;
        }
  } else if (exp_index!=nrofexpcat) { /* situation for old save file */
     if(exp_index<nrofexpcat) 
         LOG(llevError,"init_player_exp(): player has too few experience objects.\n");
     if(exp_index>nrofexpcat) 
         LOG(llevError,"init_player_exp(): player has too many experience objects.\n");
     for(j=0;j<nrofexpcat;j++)
         for(i=0;i<=exp_index;i++)
           if(!strcmp(exp_ob[i]->name,exp_cat[j]->name)) {
              break;
           } else if(i==exp_index) {
              tmp = get_object();
              copy_object(exp_cat[j],tmp);
              insert_ob_in_ob(tmp,pl);
	      exp_ob[i] = tmp; 
              esrv_send_item(pl, tmp);
              exp_index++;
           }
   }

   /* now we loop through one more time and set "apply"
    * flag on valid expericence objects. Fix_player()
    * requires this, and we get the bonus of being able
    * to ignore invalid experience objects in the player
    * inventory (player file from another game set up?).
    * Also, we reset the score (or "total" experience) of 
    * the player to be the sum of all valid experience 
    * objects.
    */

   pl->stats.exp = 0;
   for(i=0;i<exp_index;i++) { 
	if(!QUERY_FLAG(exp_ob[i],FLAG_APPLIED)) 
  	     SET_FLAG(exp_ob[i],FLAG_APPLIED); 
        /* GD: Update perm exp when loading player. */
        if (settings.use_permanent_experience)
            calc_perm_exp(exp_ob[i]);
	pl->stats.exp += exp_ob[i]->stats.exp;
	player_lvl_adj(NULL, exp_ob[i]);
   } 
   return 1;
}


/* unlink_skill() - removes skill from a player skill list and
 * unlinks the pointer to the exp object */ 

void unlink_skill(object *skillop) {
  object *op=skillop?skillop->env:NULL;

  if(!op||op->type!=PLAYER) { 
	LOG(llevError,"Error: unlink_skill() called for non-player!\n");
	return;
  }

#ifdef LINKED_SKILL_LIST
  if(remove_skill_from_list(op,skillop))
#endif
      skillop->exp_obj = NULL;
}

int remove_skill_from_list(object *op, object *skillop) {
  objectlink *obl=op->sk_list,*first,*prev=NULL,*nxt;

  if(!skillop) return 0;
 
  prev=first=obl;
  while(obl) {
     nxt = obl->next ? obl->next: NULL;
     if(obl->id==skillop->stats.sp) {
          if(obl==first) {
              op->sk_list = nxt;
              nxt = (nxt&&nxt->next) ? nxt->next: NULL;
          }
          if(prev) prev->next = nxt;
#if 0
          LOG(llevDebug,"Removing skill: %s from list\n",
                skillop->name);
#endif
          CFREE(obl);
          obl = NULL;
          return 1;
     } else
          prev = obl;
     obl=nxt;
  }   
     
  if(first) op->sk_list = first;
  return 0; 
}

/* link_player_skills() - linking skills with experience objects
 * and creating a linked list of skills for later fast access.
 *
 * Returns true if successfull. To be usefull this routine has to
 * be called *after* init_player_exp() and give_initial_items()
 *
 * fix_player() should be called somewhere after this function because objects
 * are unapplied here, but fix_player() is not called here.
 *
 * Aug 95 added feature whereby old player save files will get a
 * (needed) basic assortment of skills (see basic_skill array).
 * 
 * Sept 96. deleted spellcasting and praying as 'basic' skills,
 * then added in throwing to basic list. -b.t.
 *
 * - b.t. thomas@astro.psu.edu
 */

int link_player_skills(object *pl) {
  archetype *at=NULL;
  int i,j,cat=0,sk_index=0,exp_index=0,old_file=1;
  object *tmp,*sk_ob[100],*exp_ob[MAX_EXP_CAT];
  static char *basic_skills[] = { "skill_melee_weapon",
			   "skill_missile_weapon",
			   "skill_use_magic_item",
			   "skill_throwing",
			   "skill_find_traps",
			   "skill_remove_trap"};

   /* We're going to unapply all skills */
   pl->chosen_skill = NULL;
   CLEAR_FLAG (pl, FLAG_READY_SKILL);

   /* first find all exp and skill objects */
   for(tmp=pl->inv;tmp&&sk_index<100;tmp=tmp->below)
      if(tmp->type==EXPERIENCE) {
           exp_ob[exp_index] = tmp;
           tmp->nrof=1; /* to handle multiple instances */
           exp_index++;
      } else if (tmp->type==SKILL) {
           /* for startup, lets unapply all skills */
           CLEAR_FLAG (tmp, FLAG_APPLIED);
           if (tmp->invisible) {
             tmp->nrof=1; /* to handle multiple instances */
             sk_ob[sk_index]=tmp; 
             sk_index++;
           }
	   if(!strcmp(tmp->name,skills[SK_USE_MAGIC_ITEM].name)) old_file = 0;
      } 

   if(exp_index!=nrofexpcat) {
       LOG(llevError,"link_player_skills() - player has bad number of exp obj\n");
       if(!init_player_exp(pl)) { 
          LOG(llevError,"link_player_skills() - failed to correct problem.\n");
	  return 0;
       }
       (void) link_player_skills(pl);
       return 1;
   } 

   /* This looks like we have an old player file, so give those basic
    * skills to the player in this case. 
    */
	/* No, don't do this anymore. */
   if(0) { 
	object *tmp2;
	int limit = sizeof(basic_skills)/sizeof(char *);

	/* this is solely for old characters */
	if(pl->contr->orig_stats.Pow==0) { 
		pl->stats.Pow = pl->stats.Int;
		pl->contr->orig_stats.Pow = pl->contr->orig_stats.Int;
	}

	for(i=0;i<limit;i++) 
	   if((at=find_archetype(basic_skills[i]))!=NULL) {
	      int add = 1;
	      tmp = arch_to_object(at);
	      /* check if they already have this (basic) skill */
	      for(tmp2=pl->inv;tmp2;tmp2=tmp2->below)
		if(tmp2->type==SKILL && !strcmp(tmp2->name,tmp->name)) {
			add = 0;
			break;
		}

			if(add) { 
			  insert_ob_in_ob(tmp,pl);
			  sk_ob[sk_index] = tmp;
			  sk_index++; 
				LOG(llevDebug,"Added basic skill: %s to inventory of %s\n",
				basic_skills[i], pl->name);
	      }
	   } else { 
		  LOG(llevError, 
				"init_player_skills() - can't find basic skill: %s\n",basic_skills[i]);
		  return 0;
	   }
	
	}
  /* Ok, create linked list and link the associated skills to exp objects */
   for(i=0;i<sk_index;i++) {
#ifdef LINKED_SKILL_LIST
  	objectlink *obl;
	obl = (objectlink *) malloc(sizeof(objectlink));
	obl->ob=sk_ob[i];
	obl->id=sk_ob[i]->stats.sp; 
	obl->next = pl->sk_list;
	pl->sk_list = obl;
#endif
        cat = skills[sk_ob[i]->stats.sp].category;
	if(cat==EXP_NONE) continue;
 
        for(j=0;exp_ob[j]!=NULL&&j<exp_index;j++)
           if(!strcmp(exp_cat[cat]->name,exp_ob[j]->name)) break;
 
        sk_ob[i]->exp_obj = exp_ob[j];
   }
   return 1;
}   

/* link_player_skill() - links a  skill to exp object when applied or learned by
 * a player. Returns true if can link. Returns false if got misc
 * skill - bt.
 */ 

int link_player_skill(object *pl, object *skillop) {
  object *tmp;
  int cat;
#ifdef LINKED_SKILL_LIST
  objectlink *obl;

  /* add it to the linked list */
  obl = (objectlink *) malloc(sizeof(objectlink));
  obl->ob=skillop;
  obl->id=skillop->stats.sp;
  obl->next = pl->sk_list;
  pl->sk_list = obl;
#endif

  cat = skills[skillop->stats.sp].category; 

  if(cat!=EXP_NONE) {   /* ok the skill has an exp object, now find right one in pl inv */ 
      for(tmp=pl->inv;tmp;tmp=tmp->below)
          if(tmp->type==EXPERIENCE&&!strcmp(exp_cat[cat]->name,tmp->name)) {	
              skillop->exp_obj = tmp;
	      break;
	   }
      return 1;
  }
  skillop->exp_obj = NULL;

  return 0; 
}

/* Learn skill. This inserts the requested skill in the player's
 * inventory. The 'slaying' field of the scroll should have the
 * exact name of the requested archetype (there should be a better way!?)
 * -bt. thomas@nomad.astro.psu.edu
 */
 
int
learn_skill (object *pl, object *scroll) {
object *tmp;
archetype *skill;
#ifdef LINKED_SKILL_LIST
objectlink *obl;
#else
object *tmp2;
#endif
      skill=find_archetype(scroll->slaying);
     if(!skill)
           return 2;
     tmp=arch_to_object(skill);
     if(!tmp) return 2;

/* check if player already has it */
#ifdef LINKED_SKILL_LIST
     for(obl=pl->sk_list;obl;obl=obl->next)
	if(obl->ob->invisible
	 &&(obl->ob->stats.sp==tmp->stats.sp)) return 0;
#else /* LINKED_SKILL_LIST */ 
     for(tmp2=pl->inv;tmp2;tmp2=tmp2->below)
	if(tmp2->type==SKILL&&tmp2->invisible
	 &&tmp2->stats.sp==tmp->stats.sp) return 0;
# endif /* LINKED_SKILL_LIST */

     /* now a random change to learn, based on player Int */
    if(RANDOM()%100>learn_spell[pl->stats.Int])
	return 2; /* failure :< */

    /* Everything is cool. Give'em the skill */
    (void) insert_ob_in_ob(tmp,pl);
    (void) link_player_skill(pl,tmp);  
    esrv_send_item(pl, tmp);
    return 1;
}

/* Gives a percentage clipped to 0% -> 100% of a/b. */
/* Probably belongs in some global utils-type file? */
static int clipped_percent(int a, int b)
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
 * their current skill list. b.t. (thomas@nomad.astro.psu.edu)
 * I have now added capability to show assoc. experience objects too.
 * Inportant note: the value of chosen_skill->level is not always
 * accurate--it is only updated in change_skill(), therefore long
 * unused skills may have wrong value. This is why we call 
 * skill->exp_obj->level instead (the level of the associated experience
 * object -- which is always right).
 * -b.t.
 */
 
void show_skills(object *op) {
  object *tmp=NULL;
  char buf[MAX_BUF], *in;
  int i,length,is_first;
#ifdef LINKED_SKILL_LIST
  objectlink *obl;
 
  if (!op->sk_list) {
      new_draw_info(NDI_UNIQUE, 0,op,"You know no skills.");
      op->chosen_skill=NULL;    /* double safety - for those who lose skills */
      return;
  } else {
#endif
      length = 31;
      in = "";
      if (op)
        clear_win_info(op);
      /* sprintf(buf,"Player Skills      Category    /lvl"); */ 
      sprintf(buf,"Player skills by experience category");
      new_draw_info(NDI_UNIQUE, 0,op,buf);
#ifdef LINKED_SKILL_LIST
  }
#endif

/* print out skills by category */
  for (i=0;i<=nrofexpcat;i++) {
    char Special[100];
    Special[0]='\0';
    is_first=1;
    if(i==nrofexpcat) i=EXP_NONE; /* skip to misc exp category */
#ifdef LINKED_SKILL_LIST
    for (obl=op->sk_list;obl;obl=obl->next) {
      tmp = obl->ob;
#else
    for (tmp=op->inv;tmp;tmp=tmp->below) {
      if(tmp->type!=SKILL) continue;
#endif
      if(skills[tmp->stats.sp].category==i
   	&&(tmp->invisible||QUERY_FLAG(tmp,FLAG_APPLIED))) {
	  /* header info */
          if(is_first) {  
            is_first=0;
            new_draw_info(NDI_UNIQUE, 0,op," "); 
            if(tmp->exp_obj) { 
	      object *tmp_exp = tmp->exp_obj;
	      int k=(length-15-strlen(tmp_exp->name)); 
	      char tmpbuf[40];
	      strcpy(tmpbuf,tmp_exp->name);
	      while(k>0) {k--; strcat(tmpbuf,".");}
              if (settings.use_permanent_experience) {
                new_draw_info_format(NDI_UNIQUE,0,op,"%slvl:%3d (xp:%d/%d/%d%%)",
	             tmpbuf,tmp_exp->level,
                     tmp_exp->stats.exp,
                     level_exp(tmp_exp->level+1, op->expmul),
                     clipped_percent(tmp_exp->last_heal,tmp_exp->stats.exp));
              } else {
                new_draw_info_format(NDI_UNIQUE,0,op,"%slvl:%3d (xp:%d/%d)",
	             tmpbuf,tmp_exp->level,
                     tmp_exp->stats.exp,
                     level_exp(tmp_exp->level+1, op->expmul));
              }
	      if (strcmp(tmp_exp->name,"physique")==0)
		{
		  sprintf(Special,"You can handle %d weapon improvements.",tmp_exp->level/5+5);
		}
	      if (strcmp(tmp_exp->name,"wisdom")==0)
		{
		    char *cp = determine_god(op);

		    sprintf(Special,"You worship %s.", cp?cp:"no god at current time");
		}
            } else if(i==EXP_NONE) { 
              new_draw_info(NDI_UNIQUE,0,op,"misc.");
	    } 
	  }

	  /* print matched skills */
          if((!op || QUERY_FLAG(op, FLAG_WIZ)))
              (void) sprintf(buf,"%s%s %s (%5d)",in,
		QUERY_FLAG(tmp,FLAG_APPLIED)?"*":"-",
		skills[tmp->stats.sp].name,tmp->count);
          else
              (void) sprintf(buf,"%s%s %s",in,
		QUERY_FLAG(tmp,FLAG_APPLIED)?"*":"-",
		skills[tmp->stats.sp].name);

          new_draw_info(NDI_UNIQUE,0,op,buf);
      }
    }
    if (Special[0]!='\0')
      {
	new_draw_info(NDI_UNIQUE,0,op,Special);
      }
  }
}    

/* use_skill() - similar to invoke command, it executes the skill in the 
 * direction that the user is facing. Returns false if we are unable to 
 * change to the requested skill, or were unable to use the skill properly. 
 * -b.t.
 */

int use_skill(object *op, char *string) {
    int sknum=-1;

    /* the skill name appears at the begining of the string,
     * need to reset the string to next word, if it exists. */
    /* first eat name of skill and then eat any leading spaces */  

    if(string && (sknum=lookup_skill_by_name(string))>=0) {
	int len;

	if (sknum==-1) {
	    new_draw_info_format(NDI_UNIQUE, 0, op, 
		 "Unable to find skill by name %s", string);
	    return 0;
	}

	len=strlen(skills[sknum].name);

	/* All this logic goes and skips over the skill name to find any
	 * options given to the skill.
	 */
	if(len>=strlen(string)) {
	    *string=0x0;
	} else {
	    while(len--){ string++;}
	    while(*string==0x20){string++;}
	}
	if(strlen(string)==0) string = NULL;

    }
   
#ifdef SKILL_UTIL_DEBUG
  LOG(llevDebug,"use_skill() got skill: %s\n",sknum>-1?skills[sknum].name:"none");
#endif

   /* Change to the new skill, then execute it. */
   if(change_skill(op,sknum))
	if(do_skill(op,op->facing,string)) return 1;
 
    return 0;
}


/* change_skill() - returns true if we are able to change to the requested
 * skill. Ignore the 'pl' designation, this code is useful for players and
 * monsters.  -bt. thomas@astro.psu.edu
 *
 * sk_index == -1 means that old skill should be unapplied, and no new skill
 * applied.
 */

/* Sept 95. Got rid of nasty strcmp calls in here -b.t.*/

/* Dec 95 - cleaned up the code a bit, change_skill now passes an indexed
 * value for the skill rather than a character string. Added find_skill. 
 * -b.t. */
 
int change_skill (object *who, int sk_index)
{
    object *tmp;

    if (who->chosen_skill && who->chosen_skill->stats.sp == sk_index)
    {
        /* optimization for changing skill to current skill */
        if (who->type == PLAYER)
            who->contr->shoottype = range_skill;
        return 1;
    }

    if (sk_index >= 0 && sk_index < NROFSKILLS
        && (tmp = find_skill (who, sk_index)) != NULL)
    {
        if (apply_special (who, tmp, AP_APPLY)) {
            LOG (llevError, "BUG: change_skill(): can't apply new skill\n");
            return 0;
        }
        return 1;
    }

    if (who->chosen_skill)
        if (apply_special (who, who->chosen_skill, AP_UNAPPLY))
            LOG (llevError, "BUG: change_skill(): can't unapply old skill\n");
    if (sk_index >= 0)
        new_draw_info_format (NDI_UNIQUE, 0, who, "You have no knowledge "
                              "of %s.", skills[sk_index].name);
    return 0;
}

/* This is like change_skill above, but it is given that
 * skill is already in who's inventory - this saves us
 * time if the caller has already done the work for us.
 * return 0 on success, 1 on failure.
 */

int change_skill_to_skill (object *who, object *skill)
{
    if (!skill) return 1;	    /* Quick sanity check */

    if (who->chosen_skill == skill)
    {
        /* optimization for changing skill to current skill */
        if (who->type == PLAYER)
            who->contr->shoottype = range_skill;
        return 0;
    }

    if (skill->env != who) {
	LOG(llevError,"change_skill_to_skill: skill is not in players inventory\n");
	return 1;
    }

    if (apply_special (who, skill, AP_APPLY)) {
	LOG (llevError, "BUG: change_skill(): can't apply new skill\n");
            return 1;
    }
    return 0;
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

int attack_melee_weapon(object *op, int dir, char *string) {

  if(!QUERY_FLAG(op, FLAG_READY_WEAPON)) {
        if(op->type==PLAYER)
          new_draw_info(NDI_UNIQUE, 0,op,"You have no ready weapon to attack with!");
        return 0;
    }

  return skill_attack(NULL,op,dir,string);

}

/* attack_hth() - this handles all hand-to-hand attacks -b.t. */
 
/* July 5, 1995 - I broke up attack_hth() into 2 parts. In the first
 * (attack_hth) we check for weapon use, etc in the second (the new
 * function skill_attack() we actually attack.
 */

int attack_hth(object *pl, int dir, char *string) {
  object *enemy=NULL,*weapon;

  if(QUERY_FLAG(pl, FLAG_READY_WEAPON))
    for(weapon=pl->inv;weapon;weapon=weapon->below) {
        if(weapon->type!=WEAPON
          ||!QUERY_FLAG(weapon, FLAG_APPLIED)) continue;
        CLEAR_FLAG(weapon,FLAG_APPLIED);
        CLEAR_FLAG(pl,FLAG_READY_WEAPON);
        fix_player(pl);
        if(pl->type==PLAYER) {
	    new_draw_info(NDI_UNIQUE, 0,pl,"You unwield your weapon in order to attack.");
	    esrv_update_item(UPD_FLAGS, pl, weapon);
        }
        break;
    }   
 
  return skill_attack(enemy,pl,dir,string);
 
}

/* skill_attack() - Core routine for use when we attack using a skills
 * system. There are'nt too many changes from before, basically this is
 * a 'wrapper' for the old attack system. In essence, this code handles
 * all skill-based attacks, ie hth, missile and melee weapons should be
 * treated here. If an opponent is already supplied by move_player(),
 * we move right onto do_skill_attack(), otherwise we find if an
 * appropriate opponent exists.
 *
 * This is called by move_player() and attack_hth()
 *
 * Initial implementation by -bt thomas@astro.psu.edu
 */
 
int skill_attack (object *tmp, object *pl, int dir, char *string) {
  int dx,dy;
 
  if(!dir) dir=pl->facing;
  dx=freearr_x[dir],dy=freearr_y[dir];
 
/* If we don't yet have an opponent, find if one exists, and attack.
 * Legal opponents are the same as outlined in move_player_attack()
 */
 
  if(tmp==NULL) {
    if (out_of_map(pl->map, pl->x+dx,pl->y+dy)) return 0;
    for(tmp=get_map_ob(pl->map,pl->x+dx,pl->y+dy);tmp;tmp=tmp->above)
        if((QUERY_FLAG(tmp,FLAG_ALIVE) && tmp->stats.hp>=0)
           || QUERY_FLAG(tmp, FLAG_CAN_ROLL)
           || tmp->type==LOCKED_DOOR ) {
                if((pl->type==PLAYER && tmp->type==PLAYER) && (pl->contr->party_number!=-1
                       && pl->contr->party_number==tmp->contr->party_number))
                                return 0;
                break;
        }
  }
  if(tmp!=NULL) return do_skill_attack(tmp,pl,string);
 
  if(pl->type==PLAYER)
    new_draw_info(NDI_UNIQUE, 0,pl,"There is nothing to attack!");
 
  return 0;
}

/* do_skill_attack() - We have got an appropriate opponent from either
 * move_player_attack() or skill_attack(). In this part we get on with
 * attacking, take care of messages from the attack and changes in invisible.
 * Returns true if the attack damaged the opponent.
 * -b.t. thomas@astro.psu.edu
 */
 
int do_skill_attack(object *tmp, object *op, char *string) {
    int success; 
    object *pskill = op->chosen_skill;
    char buf[MAX_BUF], *name = query_name(tmp);
    static char *attack_hth_skill[] = {"skill_karate",	"skill_clawing",
	"skill_flame_touch", "skill_punching", NULL };
 
   /* For Players only: if there is no ready weapon, and no "attack" skill
    * is readied either then check if op has a hand-to-hand "attack skill" 
    * to use instead. We define this an "attack" skill as any skill 
    * that is linked to the strexpobj.
    * If we are using the wrong skill, change it (if possible) to
    * a more appropriate skill (in order from the attack_hth_skill[]
    * list). If the miserable slob still does'nt have any of the hth 
    * skills, give them the last attack_hth_skill on the list. There 
    * is probably a better way to do this...how?? 
    */

    if(op->type==PLAYER 
	&& !QUERY_FLAG(op,FLAG_READY_WEAPON)
	&& ((!pskill||!pskill->exp_obj)||!pskill->exp_obj->stats.Str)) { 
	    int i=0,got_one=0;
	    object *tmp2=NULL;
	    while(attack_hth_skill[i]!=NULL) { 
		for(tmp2=op->inv;tmp2;tmp2=tmp2->below) 
		    if(tmp2->type==SKILL 
			&& !strcmp(attack_hth_skill[i],tmp2->arch->name)) {
				got_one=1;
				break;
		    }
	 	if(got_one) break;
		i++;
	    }

	    if(!got_one) { /* Arrgh. The player has no hth attack skills in inventory. 
			    * Lets give them the last one in attack_hth_skill[] */ 
		archetype *skill;
		skill=find_archetype(attack_hth_skill[i-1]);
     	        if(!skill) { 
        	    LOG(llevError,"do_skill_attack() could'nt find attack skill for %s\n",
			op->name);
		    return 0;
		} 
		   
                tmp2=arch_to_object(skill);
		insert_ob_in_ob(tmp2,op);
		(void) link_player_skill(op,tmp2);
	    }
	    /* now try to ready the new skill */
	    if(change_skill_to_skill(op,tmp2)) {  /* oh oh, trouble! */
        	LOG(llevError,"do_skill_attack() could'nt give new hth skill to %s\n",
			op->name);
		return 0;
	    }
    }	 

#if 0
   /* check if op is using a hth attack, if so modify damage */
          if(QUERY_FLAG(op,FLAG_READY_SKILL) && op->chosen_skill
             && !QUERY_FLAG(op, FLAG_READY_WEAPON))
                 if((op->type!=PLAYER) || (op->type==PLAYER
                        && op->contr->shoottype==range_skill)) {
			   int dam=hth_damage(tmp,op); 
                           if(dam!=op->stats.dam) { 
				op->stats.dam=dam;
			   }
	  }
#endif

   /* if we have 'ready weapon' but no 'melee weapons' skill readied
    * this will flip to that skill. This is only window dressing for
    * the players--no need to do this for monsters.
    */
          if(op->type==PLAYER && QUERY_FLAG(op,FLAG_READY_WEAPON)
             && (!op->chosen_skill || op->chosen_skill->stats.sp!=SK_MELEE_WEAPON)) {
#ifdef NO_AUTO_SKILL_SWITCH
		rangetype oldrange=op->contr->shoottype;
#endif
                 (void) change_skill(op,SK_MELEE_WEAPON);

/* This is just a simple hack - would probably be cleaner to have change_skill
 * do the right thing, but this isn't that bad.
 */
#ifdef NO_AUTO_SKILL_SWITCH
		if (op->contr->shoottype!=oldrange) {
		    op->contr->shoottype=oldrange;
		}
#endif
	  }
 
    /* lose invisiblity/hiding status for running attacks */
 
          if(op->type==PLAYER && op->contr->tmp_invis) {
                 op->contr->tmp_invis=0;
                 op->invisible=0;
                 op->hide=0;
                 update_object(op);
          }
 
          success = attack_ob(tmp,op);
 
   /* print appropriate  messages to the player */
 
          if(success && string!=NULL) {
                 sprintf(buf, string);
                 if(op->type==PLAYER)
                        new_draw_info_format(NDI_UNIQUE, 0,op,
                          "You %s %s!",buf,name);
                 else if(tmp->type==PLAYER)
                        new_draw_info_format(NDI_UNIQUE, 0,tmp,
                           "%s %s you!",query_name(op),buf);
          }
 
          return success;

}                         

 
/* hth_damage() - returns the appropriate amount of hth damage, based on the
 * skill in use and the opponent's characteristics. This is generalized to
 * allow monster skill use too. Called only from do_skill_attack().  -b.t.
 */
 
int hth_damage(object *target, object *pl) {
  int base = pl->arch->clone.stats.dam + dam_bonus[pl->stats.Str];
  int damage = pl->chosen_skill->stats.dam * (1 + (SK_level(pl)/9));
 
  if(damage<1)                  /* Safety - not a hth skill per se */
        return base;
  else
        damage += base;

  /* are we using a skill?, if not return normal hth damage */
  if(pl->chosen_skill==NULL
        || !QUERY_FLAG(pl,FLAG_READY_SKILL)) return damage;

 /* Against some creatures the hth attack gets no special bonus.
  * Probably left out some cases here -b.t.
  */

  if((target->level>SK_level(pl))
     || QUERY_FLAG(target,FLAG_UNDEAD)
     || QUERY_FLAG(target,FLAG_SPLITTING)
     || QUERY_FLAG(target,FLAG_HITBACK)) {
         pl->attacktype=AT_PHYSICAL;
         return damage;
  }

  damage += (((SK_level(pl)/18)+1)*pl->chosen_skill->stats.dam) + 1;

  return damage;
}

/* This is in the same spirit as the similar routine for spells 
 * it should be used anytime a function needs to check the user's
 * level.
 */

int SK_level(object *op)
{
  object *head = op->head ? op->head : op;
  int level;

#ifdef ALLOW_SKILLS
  if(head->type==PLAYER && head->chosen_skill && head->chosen_skill->level!=0) {
	level = head->chosen_skill->level;
  } else {
	level = head->level;
  }
#else
  level = head->level;
#endif

  if(level<=0)
  {
    LOG (llevError, "BUG: SK_level(arch %s, name %s): level <= 0\n",
         op->arch->name, op->name);
    level = 1;	 /* safety */
  }

  return level;
}

/* returns the amount of time it takes to use a skill. 
 * We allow for stats, and level to modify the amount 
 * of time. Monsters have no skill use time because of
 * the random nature in which use_monster_skill is called
 * already simulates this. -b.t. 
 */

float get_skill_time(object *op, int skillnr) {
  float time = skills[skillnr].time;

  if(!time || op->type!=PLAYER) 
     return 0;
  else {
    int sum = get_weighted_skill_stat_sum (op,skillnr);
    int level = SK_level(op)/10; 

	time *= 1/(1+(sum/15)+level);
  }
  return FABS(time);
}

/* get_skill_stat1() - returns the value of the primary skill
 * stat. Used in various skills code. -b.t.
 */

int get_skill_stat1(object *op) { 
  int stat_value = 0, stat=NO_STAT_VAL;

  if((op->chosen_skill) && ((stat = skills[op->chosen_skill->stats.sp].stat1)!=NO_STAT_VAL) )  
    stat_value = get_attr_value(&(op->stats),stat);

  return stat_value;
}

/* get_skill_stat2() - returns the value of the secondary skill
 * stat. Used in various skills code. -b.t.
 */

int get_skill_stat2(object *op) {
  int stat_value = 0,stat = NO_STAT_VAL;

  if((op->chosen_skill) && ((stat = skills[op->chosen_skill->stats.sp].stat2)!=NO_STAT_VAL) )  
    stat_value = get_attr_value(&(op->stats),stat);

  return stat_value;
}

/* get_skill_stat3() - returns the value of the tertiary skill 
 * stat. Used in various skills code. -b.t. 
 */ 
 
int get_skill_stat3(object *op) { 
  int stat_value = 0,stat = NO_STAT_VAL; 
 
  if((op->chosen_skill) && ((stat = skills[op->chosen_skill->stats.sp].stat3)!=NO_STAT_VAL) )
    stat_value = get_attr_value(&(op->stats),stat);  
 
  return stat_value; 
} 

/*get_weighted_skill_stats() - */

int get_weighted_skill_stats(object *op) {
  int value=0;

  value = (get_skill_stat1(op)/2)+(get_skill_stat2(op)/4)+(get_skill_stat3(op)/4);

  return value;

}


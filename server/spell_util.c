/*
 * static char *rcsid_spell_util_c =
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
#include <spells.h>
#include <object.h>
#include <errno.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <sounds.h>

#ifdef sequent
/* stoopid sequent includes don't do this like they should */
extern char * sys_errlist[];
extern int sys_nerr;
#endif

char *range_name[range_size] = {
  "none", "bow", "magic", "wand", "rod", "scroll", "horn"
#ifdef ALLOW_SKILLS
	,"steal"
#endif
};

archetype *spellarch[NROFREALSPELLS];

void init_spells() {
  static int init_spells_done = 0;
  int i;

  if (init_spells_done)
    return;
  LOG(llevDebug, "Initializing spells...");
  init_spell_param(); /*  peterm:  read the spell parameter file--newspells.c */
  init_spells_done = 1;
  for (i = 0; i < NROFREALSPELLS; i++)
    if (spells[i].archname) {
      if ((spellarch[i] = find_archetype(spells[i].archname)) == NULL)
        LOG(llevError,
            "Spell %s needs arch %s, your archetype file is out of date.\n",
            spells[i].name,spells[i].archname);
    } else
      spellarch[i] = (archetype *) NULL;
  LOG(llevDebug, "done.\n");
}

void dump_spells()
{
    int i;

    for (i = 0; i < NROFREALSPELLS; i++) {
      char *name1 = NULL, *name2 = NULL;
      if (spellarch[i]) {
        name1 = spellarch[i]->name;

        if (spellarch[i]->clone.other_arch)
          name2 = spellarch[i]->clone.other_arch->name;
      }
      fprintf(stderr, "%s:%s:%s\n", spells[i].name, (name1?name1:"null"), 
	     (name2?name2:"null"));
    }
}

void spell_effect (int spell_type, int x, int y, mapstruct *map,
	object *originator)
{

  if (spellarch[spell_type] != (archetype *) NULL) {
    object *effect = arch_to_object(spellarch[spell_type]);

    effect->x = x;
    effect->y = y;

    insert_ob_in_map(effect, map, originator);
  }
}

spell *find_spell(int spelltype) {
  if(spelltype<0||spelltype>NROFREALSPELLS)
    return NULL;
  return &spells[spelltype];
}

/*
 * base_level: level before considering attuned/repelled paths
 * Returns modified level.
 */
int path_level_mod (object *caster, int base_level, int spell_type)
{
 spell *s = find_spell(spell_type);
 int new_level;

 if (caster->path_denied & s->path)
 {
   LOG (llevError, "BUG: path_level_mod (arch %s, name %s): casting denied "
        "spell\n", caster->arch->name, caster->name);
   return 1;
 }
 new_level = base_level
             + ((caster->path_repelled & s->path) ? -5 : 0)
             + ((caster->path_attuned & s->path) ? 5 : 0);
 return (new_level < 1) ? 1 : new_level;
}

int casting_level (object *caster, int spell_type)
{
  return path_level_mod (caster, SK_level (caster), spell_type);
}


int check_spell_known (object *op, int spell_type)
{
  int i;
  for(i=0; i < (int)op->contr->nrofknownspells; i++)
    if(op->contr->known_spells[i]==spell_type)
      return 1;
  return 0;
}


/*
 * cast_spell():
 * Fires spell "type" in direction "dir".
 * If "ability" is true, the spell is the innate ability of a monster.
 * (ie, don't check for blocks_magic(), and don't add AT_MAGIC to attacktype.
 *
 * op is the creature that is owner of the object that is casting the spell
 * caster is the actual object (wand, potion) casting the spell. can be
 *    same as op.
 * dir is the direction to cast in.
 * ability is true if it is an ability casting the spell.  These can be
 *    cast in no magic areas.
 * item is the type of object that is casting the spell.
 * stringarg is any options that are being used.
 */

/* Oct 95 - added cosmetic touches for MULTIPLE_GODS hack -b.t. */

int cast_spell(object *op,object *caster,int dir,int type,int ability,SpellTypeFrom item,char *stringarg) {
#ifdef MULTIPLE_GODS
  char *godname;
#endif
  spell *s=find_spell(type);
  int success=0,bonus;
  int duration=SP_PARAMETERS[type].bdur;  /*  get the base duration */

#ifdef MULTIPLE_GODS
  if(!strcmp((godname=determine_god(op)),"none")) godname="A random spirit"; 
#endif

  /* It looks like the only properties we ever care about from the casting
   * object (caster) is spell paths and level.
   */
  if (!caster && item==spellNormal)
	caster=op;

  if(s==NULL) {
    LOG(llevError,"Error, unknown spell: %d\n",type);
    return 0;
  }
  if(item==spellPotion) {  /*  if the potion casts an onself spell, 
		    don't use the facing direction (given by apply.c)*/
	 if( spells[type].onself) dir = 0;
  }

  if(!(QUERY_FLAG(op, FLAG_WIZ))&&
	  (op->type==PLAYER)&&
	  (op->contr->shoottype==range_magic)&&
     (item!=spellPotion)&& 
     (!(IS_SUMMON_SPELL(type)&&op->contr->golem!=NULL)))
  {
	if(!spells[type].cleric&& op->stats.sp<SP_level_spellpoint_cost(op,caster,type))
	{
	  new_draw_info(NDI_UNIQUE, 0,op,"You don't have enough mana.");
	  op->contr->count_left=0;
	  return 0;
	}
	if(spells[type].cleric&&op->stats.grace<SP_level_spellpoint_cost(op,caster,type))
	{
	  /* it's possible for grace to go negative */
	  /* Fine - let grace go negative, but how negative it is should really
	   * put a limit on things - in the old method, chance was the same
	   * no matter how negative it was.
	   *Instead of subtracting 10 from the roll, add in grace (which is
	   * negative).  This puts a real limit on things.
	   */
	  if( (RANDOM()%op->stats.Wis) +op->stats.grace -
		  10*SP_level_spellpoint_cost(op,caster,type)/op->stats.maxgrace >0) {
#ifdef MULTIPLE_GODS
			  new_draw_info_format(NDI_UNIQUE, 0,op, 
				"%s grants your prayer, though you are unworthy.",godname);
#else
			  new_draw_info(NDI_UNIQUE, 0,op, 
				"God grants your prayer, though you are unworthy.");
#endif
	  }
	  else
	  {
		  prayer_failure(op,op->stats.grace,SP_level_spellpoint_cost(op,caster,type));
#ifdef MULTIPLE_GODS
			  new_draw_info_format(NDI_UNIQUE, 0,op,"%s ignores your prayer.",godname);
#else
			  new_draw_info(NDI_UNIQUE, 0,op,"God ignores your prayer.");
#endif
		  op->contr->count_left=0;
			  return 0;
	  }
	}
  }

  if (caster->path_denied & s->path) {
    new_draw_info(NDI_UNIQUE, 0,op, "You are unable to cast that spell.");
    if(op->type==PLAYER) op->contr->count_left=0;
    return RANDOM()%(s->sp)+1;
  }
  /* If it is an ability, assume that the designer of the archetype knows
   * what they are doing.
   */
  if (item==spellNormal && !ability && SK_level(caster) < s->level && !QUERY_FLAG(op,FLAG_WIZ)) {
    new_draw_info(NDI_UNIQUE, 0,op, "You lack enough skill to cast that spell.");
    if(op->type==PLAYER) op->contr->count_left=0;
    return 0;
  }

#ifdef CASTING_TIME
  if (op->casting==-1) /* begin the casting */
   {
     if (item == spellNormal&&!ability){
       op->casting = s->time*PATH_TIME_MULT(op,s);
       op->spell   = s;  /* so no one cast a spell and switchs to get lower 
			    casting times!!! */
       op->spelltype = type;
       op->spell_state = 1;


	/*  put the stringarg into the object struct so that when the
	    spell is actually cast, it knows about the stringarg.
	    necessary for the invoke command spells.  */
       if(stringarg) {
          op->spellarg = strdup_local(stringarg);  
	}
	else op->spellarg=NULL;
       return 0;
     }
  } else if (op->casting != 0) {
    if (op->type == PLAYER )
      new_draw_info(NDI_UNIQUE, 0,op,"You are casting!");
    return 0;
  }
#endif
  /*  ban removed on clerical spells in no-magic areas */
  if (!ability && item != spellPotion && 
	( ((!s->cleric)&&blocks_magic(op->map,op->x,op->y))||
	  (( s->cleric)&&blocks_cleric(op->map,op->x,op->y)))) {
    if (op->type!=PLAYER)
      return 0;
    if(s->cleric) 
#ifdef MULTIPLE_GODS
      new_draw_info_format(NDI_UNIQUE, 0,op,"This ground is unholy!  %s ignores you.",godname);
#else
      new_draw_info(NDI_UNIQUE, 0,op,"This ground is unholy!  God ignores you.");
#endif
    else
    switch(op->contr->shoottype) {
    case range_magic:
      new_draw_info(NDI_UNIQUE, 0,op,"Something blocks your spellcasting.");
      break;
    case range_wand:
      new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of your wand.");
      break;
    case range_rod:
      new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of your rod.");
      break;
    case range_horn:
      new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of your horn.");
      break;
    case range_scroll:
      new_draw_info(NDI_UNIQUE, 0,op,"Something blocks the magic of your scroll.");
      break;
    default:
      break;
    }
    return 0;
  }
  if(item == spellNormal && op->type==PLAYER&&s->cleric&&
     RANDOM()%100< s->level*2 - op->level + cleric_chance[op->stats.Wis]-
	op->stats.luck*3) {
    play_sound_player_only(op->contr, SOUND_FUMBLE_SPELL,0,0);
    new_draw_info(NDI_UNIQUE, 0,op,"You fumble the spell.");
#ifdef CASTING_TIME
  op->casting = -1;
  op->spell_state = 1;
#endif    
    if(s->sp==0) /* Shouldn't happen... */
      return 0;
    return RANDOM()%(SP_level_spellpoint_cost(op,caster,type)+1)+1;
  }
#ifdef SPELL_ENCUMBRANCE
  if(item == spellNormal && op->type==PLAYER && (!s->cleric) ) {
    int failure = (RANDOM()%200) - op->contr->encumbrance +op->level -s->level +35;

    if( failure < 0) {
	new_draw_info(NDI_UNIQUE, 0,op,"You bungle the spell because you have too much heavy equipment in use.");
#ifdef SPELL_FAILURE_EFFECTS
        spell_failure(op,failure,SP_level_spellpoint_cost(op,caster,type));
#endif
	return RANDOM()%(SP_level_spellpoint_cost(op,caster,type)+ 1);
	}
   }
#endif /*SPELL_ENCUMBRANCE*/
	
/*
 * This is a simplification of the time it takes to cast a spell.
 * In the future, the time will have to be spent before the
 * spell takes effect, and the caster can possibly be disturbed.
 * (maybe that should depend upon the spell cast?)
 */
#ifdef CASTING_TIME
if (item == spellNormal && !ability ){
  op->casting = -1;
  op->spell_state = 1;
  s = op->spell; /* set s to the cast spell */
  type = op->spelltype;
  stringarg = op->spellarg;
}
#else
  /* It seems the that the patch that added spell casting times
   * increased the time value of most spells by about 10.  So divide
   * by 10 to get back to more normal use.
   */
  op->speed_left -= (s->time*PATH_TIME_MULT(op,s) / 10) * FABS(op->speed);
#endif
  switch((enum spellnrs) type) {
  case SP_BULLET:
  case SP_LARGE_BULLET:
    success = fire_arch(op,caster,dir,spellarch[type],type,1);
    break;
  case SP_HOLY_ORB:
    success = fire_arch(op,caster,dir,spellarch[type],type,0);
    break;
  case SP_S_FIREBALL:
  case SP_M_FIREBALL:
  case SP_L_FIREBALL:
  case SP_S_SNOWSTORM:
  case SP_M_SNOWSTORM:
  case SP_L_SNOWSTORM:
  case SP_HELLFIRE:
  case SP_POISON_CLOUD:
  case SP_M_MISSILE:
  case SP_S_MANABALL:
  case SP_M_MANABALL:
  case SP_L_MANABALL:
    success = fire_arch(op,caster,dir,spellarch[type],type, !ability);
    break;
  case SP_MASS_CONFUSION:
  case SP_SHOCKWAVE:
  case SP_COLOR_SPRAY:
  case SP_FACE_OF_DEATH:
  case SP_COUNTER_SPELL:
  case SP_BURNING_HANDS:
  case SP_PARALYZE:
  case SP_SLOW:
  case SP_ICESTORM:
  case SP_FIREBREATH:
  case SP_LARGE_ICESTORM:
  case SP_BANISHMENT:	
  case SP_MANA_BLAST:
    success = cast_cone(op,caster,dir,duration,type,spellarch[type],!ability);
    break;
  case SP_TURN_UNDEAD:
    if(QUERY_FLAG(op,FLAG_UNDEAD)) { /* the undead *don't* cast this */
       new_draw_info(NDI_UNIQUE, 0,op,"Your undead nature prevents you from turning undead!");
       success=0; break;
    }
  case SP_HOLY_WORD:
    success = cast_cone(op,caster,dir,duration+(turn_bonus[op->stats.Wis]/5),type,
	spellarch[type],0);
    break;
  case SP_HOLY_WRATH:
  case SP_INSECT_PLAGUE:
  case SP_RETRIBUTION:
    success = cast_smite_spell(op,caster,dir,type);
    break;
  case SP_SUNSPEAR:
  case SP_FIREBOLT:
  case SP_FROSTBOLT:
  case SP_S_LIGHTNING:
  case SP_L_LIGHTNING:
  case SP_STEAMBOLT:
  case SP_MANA_BOLT:
    success = fire_bolt(op,caster,dir,type,!ability);
    break;
  case SP_BOMB:
    success = create_bomb(op,caster,dir,type,"bomb");
    break;
  case SP_GOLEM:
  case SP_FIRE_ELEM:
  case SP_WATER_ELEM:
  case SP_EARTH_ELEM:
  case SP_AIR_ELEM:
    success = summon_monster(op,caster,dir,spellarch[type],type);
    break;
  case SP_FINGER_DEATH: 
    success = finger_of_death(op,caster,dir);
    break;
  case SP_SUMMON_AVATAR: 
  case SP_HOLY_SERVANT: {
    archetype *spat = find_archetype((type==SP_SUMMON_AVATAR)?"avatar":"holy_servant");
    success = summon_avatar(op,caster,dir,spat,type);
    break; } 
  case SP_CONSECRATE: 
    success = cast_consecrate(op); 
    break;
  case SP_SUMMON_CULT:
#ifdef MULTIPLE_GODS
    success = summon_cult_monsters(op,dir);
#else
    success = summon_pet(op,dir,item);
#endif
    break;
  case SP_PET:
    success = summon_pet(op,dir, item);
    break;
  case SP_D_DOOR:
    /* dimension door needs the actual caster, because that is what is
     * moved.
     */
    success = dimension_door(op,dir);
    break;
  case SP_DARKNESS:
  case SP_WALL_OF_THORNS:
  case SP_CHAOS_POOL:
  case SP_COUNTERWALL:
  case SP_FIRE_WALL:
  case SP_FROST_WALL:
  case SP_EARTH_WALL:
    success = magic_wall(op,caster,dir,type);
    break;
  case SP_MAGIC_MAPPING:
    if(op->type==PLAYER) {
      spell_effect(SP_MAGIC_MAPPING, op->x, op->y, op->map, op);
      draw_map(op);
      success=1;
    }
    break;
  case SP_FEAR:
    if(op->type==PLAYER)
      bonus=fear_bonus[op->stats.Cha];
    else
      bonus=caster->head==NULL?caster->level/3+1:caster->head->level/3+1;
    success = cast_cone(op,caster,dir,duration+bonus,SP_FEAR,spellarch[type],!ability);
    break;
  case SP_WOW:
    success = cast_wow(op,dir,ability, item);
    break;
  case SP_DESTRUCTION:
    success = cast_destruction(op,caster,5+op->stats.Int,AT_MAGIC);
    break;
  case SP_PERCEIVE:
    success = perceive_self(op);
    break;
  case SP_WOR:
    success = cast_wor(op,caster);
    break;
  case SP_INVIS:
  case SP_INVIS_UNDEAD:
  case SP_IMPROVED_INVIS:
    success = cast_invisible(op,caster,type);
    break;
  case SP_PROBE:
    success = probe(op,dir);
    break;
  case SP_CREATE_FOOD:
    success = cast_create_food(op,caster,dir,stringarg);
    break;
  case SP_EARTH_DUST:
    success = cast_earth2dust(op,caster);
    break;
  case SP_REGENERATION: 
  case SP_BLESS:
  case SP_CURSE:
  case SP_HOLY_POSSESSION:
  case SP_STRENGTH:
  case SP_DEXTERITY:
  case SP_CONSTITUTION:
  case SP_CHARISMA:
  case SP_ARMOUR:
  case SP_PROT_COLD:
  case SP_PROT_FIRE:
  case SP_PROT_ELEC:
  case SP_PROT_POISON:
  case SP_PROT_SLOW:
  case SP_PROT_DRAIN:
  case SP_PROT_PARALYZE:
  case SP_PROT_ATTACK:
  case SP_PROT_MAGIC:
  case SP_PROT_CONFUSE:
  case SP_PROT_CANCEL:
  case SP_PROT_DEPLETE:
  case SP_LEVITATE:
  case SP_HEROISM:
  case SP_CONFUSION:
  case SP_XRAY:
  case SP_DARK_VISION:
    success = cast_change_attr(op,caster,dir,type);
    break;
  case SP_RESTORATION:
  case SP_HEAL:
  case SP_MINOR_HEAL:
  case SP_MED_HEAL:
  case SP_MAJOR_HEAL:
  case SP_CURE_POISON:
  case SP_CURE_CONFUSION:
  case SP_CURE_BLINDNESS:
  case SP_CURE_DISEASE:
    success = cast_heal(op,dir,type);
    break;
  case SP_REGENERATE_SPELLPOINTS:
    success = cast_regenerate_spellpoints(op);
    break;
  case SP_SMALL_SPEEDBALL:
  case SP_LARGE_SPEEDBALL:
    success = cast_speedball(op,dir,type);
    break;
  case SP_POLYMORPH:
    success = cast_polymorph(op,dir);
    break;
  case SP_CHARGING:
    success = recharge(op);
    break;
  case SP_CANCELLATION:
    success = fire_cancellation(op,dir,spellarch[type],!ability);
    break;
  case SP_ALCHEMY:
    success = alchemy(op);
    break;
  case SP_REMOVE_CURSE:
  case SP_REMOVE_DAMNATION:
    success = remove_curse(op, type, item);
    break;
  case SP_IDENTIFY:
    success = cast_identify(op);
    break;
  case SP_DETECT_MAGIC:
  case SP_DETECT_MONSTER:
  case SP_DETECT_EVIL:
  case SP_DETECT_CURSE:
  case SP_SHOW_INVIS:
    success = cast_detection(op, type);
    break;
  case SP_AGGRAVATION:
    aggravate_monsters(op);
    success = 1;
    break;
/* peterm: following spells added */
  case SP_BALL_LIGHTNING:
    success = fire_arch(op,caster,dir,find_archetype("ball_lightning"),type,!ability);
    break;
  case SP_METEOR_SWARM: {
    int n;
    n=RANDOM()%3 + RANDOM()%3 + RANDOM()%3 +3 +
      SP_level_strength_adjust(op,caster, type);
    success = 1;
    fire_swarm(op,caster,dir,spellarch[type],SP_METEOR,n,0);
    break;
  }
  case SP_BULLET_SWARM: {
    int n;
    n=RANDOM()%3 + RANDOM()%3 + RANDOM()%3 +3 +
      SP_level_strength_adjust(op,caster, type);
    success = 1;
    fire_swarm(op,caster,dir,spellarch[type],SP_BULLET,n,1);
    break;
  }
  case SP_BULLET_STORM: {
    int n;
    n=RANDOM()%3 + RANDOM()%3 + RANDOM()%3 +3 +
      SP_level_strength_adjust(op,caster, type);
    success = 1;
    fire_swarm(op,caster,dir,spellarch[type],SP_LARGE_BULLET,n,1);
    break;
  }
  case SP_CAUSE_MANY: {
    int n;
    n=RANDOM()%3 + RANDOM()%3 + RANDOM()%3 +3 +
      SP_level_strength_adjust(op,caster, type);
    success = 1;
    fire_swarm(op,caster,dir,spellarch[type],SP_CAUSE_HEAVY,n,1);
    break;
  }
  case SP_METEOR:
    success = fire_arch(op,caster,dir,find_archetype("meteor"),type,0);
    break;
  case SP_MYSTIC_FIST:
    success = summon_monster(op,caster,dir,spellarch[type],type);
    break;
  case SP_RAISE_DEAD:
  case SP_RESURRECTION:
    success = cast_raise_dead_spell(op,dir,type, NULL);
    break;
  /* mlee */
  case SP_IMMUNE_COLD:
  case SP_IMMUNE_FIRE:
  case SP_IMMUNE_ELEC:
  case SP_IMMUNE_POISON:
  case SP_IMMUNE_SLOW:
  case SP_IMMUNE_DRAIN:
  case SP_IMMUNE_PARALYZE:
  case SP_IMMUNE_ATTACK:
  case SP_IMMUNE_MAGIC:
  case SP_INVULNERABILITY:
  case SP_PROTECTION:
  case SP_HASTE:
    success=cast_change_attr(op,caster,dir,type);
    break;
  /* peterm, additional spells added */
  case SP_BUILD_DIRECTOR:
  case SP_BUILD_BWALL:
  case SP_BUILD_LWALL:
  case SP_BUILD_FWALL:
    success=create_the_feature(op,caster,dir,type);
    break;
  case SP_RUNE_FIRE: 
  case SP_RUNE_FROST:
  case SP_RUNE_SHOCK: 
  case SP_RUNE_BLAST: 
  case SP_RUNE_DEATH: 
  case SP_RUNE_ANTIMAGIC:
    success = write_rune(op,dir,0,caster->level,s->archname);
    break;
  case SP_RUNE_DRAINSP:
    success = write_rune(op,dir,SP_MAGIC_DRAIN,caster->level,s->archname);
    break;
  case SP_RUNE_TRANSFER:
    success= write_rune(op,dir,SP_TRANSFER,caster->level,s->archname);
    break;
  case SP_TRANSFER:
    success = cast_transfer(op,dir);
    break;
  case SP_MAGIC_DRAIN:
    success= drain_magic(op,dir);
    break;
  case SP_DISPEL_RUNE:
    success = dispel_rune(op,dir,0);  /* 0 means no risk of detonating rune */
    break;
  case SP_SUMMON_EVIL_MONST:
	if(op->type==PLAYER) return 0;
    success = summon_hostile_monsters(op,op->stats.maxhp,op->race);
    break;

  case SP_REINCARNATION:
    {
      object * dummy;
      if(stringarg==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Reincarnate WHO?");
	success=0;
	break;
      }
      dummy = get_object();
      dummy->name = add_string(stringarg);
      success = cast_raise_dead_spell(op,dir,type, dummy);
      free_object(dummy);
    }
    break;
  case SP_RUNE_MAGIC:
    { int total_sp_cost, spellinrune;
      spellinrune=look_up_spell_by_name(op,stringarg);
      if(spellinrune!=-1) {
      total_sp_cost=SP_level_spellpoint_cost(op,caster,spellinrune)
			+spells[spellinrune].sp;
	if(op->stats.sp<total_sp_cost) {
	  new_draw_info(NDI_UNIQUE, 0,op,"Not enough spellpoints.");
#ifdef CASTING_TIME
	  /* free the spell arg */
	  if(stringarg) {free(stringarg);stringarg=NULL; };
#endif
	  return 0;
	}
	success=write_rune(op,dir,spellinrune,caster->level,stringarg);
	return (success ? total_sp_cost : 0);
      }
#ifdef CASTING_TIME
	  /* free the spell arg */
	  if(stringarg) {free(stringarg);stringarg=NULL; };
#endif
      return 0;
    }
    break;
  case SP_RUNE_MARK:
    success=write_rune(op,dir,0,-2,stringarg);
#ifdef CASTING_TIME
	  /* free the spell arg */
	  if(stringarg) {free(stringarg);stringarg=NULL; };
#endif


    break;
  case SP_LIGHT:
    success = cast_light(op,caster,dir);
    break;
  case SP_DAYLIGHT:
    success = cast_daylight(op); 
    break; 
  case SP_NIGHTFALL:
    success = cast_nightfall(op);
    break;
  case SP_FAERY_FIRE:
    success = cast_faery_fire(op,caster);
    break;
  case SP_CAUSE_LIGHT:
  case SP_CAUSE_HEAVY:
  case SP_CAUSE_MEDIUM:
  case SP_CAUSE_CRITICAL:
	success = fire_arch(op,caster,dir,spellarch[type],type,1); /* don't want to OR magic */
  break;
  case SP_SUMMON_FOG:
	success = summon_fog(op,caster,dir,type);
	break;
  case SP_PACIFY:
	cast_pacify(op,caster,spellarch[type],type);
	success = 1;
  	break;
  case SP_COMMAND_UNDEAD:
	cast_charm_undead(op,caster,spellarch[type],type);
	success = 1;
  	break;
  case SP_CHARM:
	cast_charm(op,caster,spellarch[type],type);
	success = 1;
	break;
  /* huma */
  case SP_CREATE_MISSILE:
    success = cast_create_missile(op,caster,dir,stringarg);
    break;

  case SP_CAUSE_EBOLA:
  case SP_CAUSE_FLU:
  case SP_CAUSE_PLAGUE:
  case SP_CAUSE_LEPROSY:
  case SP_CAUSE_SMALLPOX:
  case SP_CAUSE_PNEUMONIC_PLAGUE:
  case SP_CAUSE_ANTHRAX:
  case SP_CAUSE_TYPHOID:
    success = cast_cause_disease(op,caster,dir,spellarch[type],type);
    break;
    /* DAMN */
  case SP_DANCING_SWORD:
  case SP_STAFF_TO_SNAKE: 
  case SP_ANIMATE_WEAPON:
    success = animate_weapon(op,caster,dir,spellarch[type],type);
    break;
  }

  play_sound_map(op->map, op->x, op->y, SOUND_CAST_SPELL_0 + type);
#ifdef CASTING_TIME
	  /* free the spell arg */
	  if(stringarg) {free(stringarg);stringarg=NULL; };
#endif

#ifdef SPELLPOINT_LEVEL_DEPEND
  return success?SP_level_spellpoint_cost(op,caster,type):0;
#else
  return success?(s->sp*PATH_SP_MULT(op,s)):0;
#endif
}


int cast_create_obj(object *op,object *caster,object *new_op, int dir)
{
  if(dir && blocked(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) {
    new_draw_info(NDI_UNIQUE, 0,op,"Something is in the way.");
    new_draw_info(NDI_UNIQUE, 0,op,"You cast it at your feet.");
    dir = 0;
  }
  new_op->x=op->x+freearr_x[dir];
  new_op->y=op->y+freearr_y[dir];
  insert_ob_in_map(new_op,op->map,op);
  return dir;
}

int summon_monster(object *op,object *caster,int dir,archetype *at,int spellnum) {
  object *tmp;
  if(op->type==PLAYER)
    if(op->contr->golem!=NULL&&!QUERY_FLAG(op->contr->golem,FLAG_FREED)) {
      control_golem(op->contr->golem,dir);
      return 0;
    }
  if(!dir)
    dir=find_free_spot(NULL,op->map,op->x,op->y,1,9);
  if((dir==-1) || blocked(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir])) {
    new_draw_info(NDI_UNIQUE, 0,op,"There is something in the way.");
    if(op->type==PLAYER)
      op->contr->count_left=0;
    return 0;
  }
  tmp=arch_to_object(at);
  if(op->type==PLAYER) {
    CLEAR_FLAG(tmp, FLAG_MONSTER);
    SET_FLAG(tmp, FLAG_FRIENDLY);
    tmp->stats.exp=0;
    add_friendly_object(tmp);
    tmp->type=GOLEM;
/* Don't see any point in setting this when monsters summon monsters: */
    set_owner(tmp,op);
    op->contr->golem=tmp;
    /* give the player control of the golem */
    op->contr->shoottype=range_scroll;
  } else {
    if(QUERY_FLAG(op, FLAG_FRIENDLY)) {
      object *owner = get_owner(op);
      if (owner != NULL) { /* For now, we transfer ownership */
        set_owner (tmp, owner);
        tmp->move_type = PETMOVE;
        add_friendly_object (tmp);
        SET_FLAG (tmp, FLAG_FRIENDLY);
      }
    }
    SET_FLAG(tmp, FLAG_MONSTER);
  }
  /*  This sets the level dependencies on dam and hp for monsters */
  tmp->stats.hp = SP_PARAMETERS[spellnum].bdur +
			 10 * SP_level_strength_adjust(op,caster,spellnum);
  tmp->stats.dam= SP_PARAMETERS[spellnum].bdam +
			 2* SP_level_dam_adjust(op,caster,spellnum);
  if(tmp->stats.dam<0) tmp->stats.dam=127;  /*seen this go negative!*/
 /*  make experience increase in proportion to the strength of the summoned creature. */
  tmp->stats.exp *= SP_level_spellpoint_cost(op,caster,spellnum)/spells[spellnum].sp;
  tmp->speed_left= -1;
  tmp->x=op->x+freearr_x[dir],tmp->y=op->y+freearr_y[dir];
  tmp->direction=dir;
  insert_ob_in_map(tmp,op->map,op);
  return 1;
}


/* Returns true if it is ok to put spell *op on the space/may provided.
 * Not sure what immune_stop was supposed to do - the only functions that
 * current call this are move_cone and explosion, and both are just
 * passing op->attacktype on.
 * My guess judging from the old code is that if the space has a monster
 * immune to immune_stop, the spell should not be placed.  But that
 * doesn't make a lot of sense to me (a dragon immune to fire should not
 * cause a fireball not to explode
 *
 * 0.94.2 - rewrote this to include the wall check instead of the calling
 * function doing so.
 */

static int ok_to_put_more(mapstruct *m,int x,int y,object *op,int immune_stop) {
    object *tmp;

    /* No check for out of map was here before - probably caught when the
     * calling function tries to insert the object.  But a check here
     * should only be a minor performance hit, and is a good thing.
     */

    if (out_of_map(m,x,y)) return 0;

    /* if there is a wall, certainly can't put the spell there.  The blocks
     * view check is historic from the old calling functions - my personal
     * view is that blocks view should not affect spells in any way, but
     * I will leave it in for now.
     */
    if(wall(m,x,y) || blocks_view(m,x,y)) return 0;

    for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp->above) {
	/* If there is a counterspell on the space, and this
	 * object is using magic, don't progess.  I believe we could
	 * leave this out and let in progress, and other areas of the code
	 * will then remove it, but that would seem to to use more
	 * resources, and may not work as well if a player is standing
	 * on top of a counterwall spell (may hit the player before being
	 * removed.)  On the other hand, it may be more dramatic for the
	 * spell to actually hit the counterwall and be sucked up.
	 *
	 * We should probably check the type of the object, just in case
	 * a player somehow gets a counterspell attacktype.
	 */
	if ((tmp->attacktype & AT_COUNTERSPELL) &&
	    (tmp->type != PLAYER) && !QUERY_FLAG(tmp,FLAG_MONSTER) &&
	    (tmp->type != WEAPON) && (tmp->type != BOW) &&
	    (tmp->type != ARROW) && (tmp->type != GOLEM) &&
	    (immune_stop & AT_MAGIC)) return 0;

	/* This is to prevent 'out of control' spells.  Basically, this
	 * limits one spell effect per space per spell.  This is definately
	 * needed for performance reasons, and just for playability I believe.
	 * there are no such things as multispaced spells right now, so
	 * we don't need to worry about the head.
	 */
	if ((tmp->stats.maxhp == op->stats.maxhp) && (tmp->type == op->type))
	    return 0;

	/* Perhaps we should also put checks in for no magic and unholy
	 * ground to prevent it from moving along?
	 */
#if 0
	/* Remove code that determines if it stops because it hits
	 * a type of monster.  I don't think that should affect it.
	 */
    head=tmp->head==NULL?tmp:tmp->head;
    if((QUERY_FLAG(head, FLAG_ALIVE) && head->immune & immune_stop))
      return 0;
#endif
    }
    /* If it passes the above tests, it must be OK */
    return 1;
}






int fire_bolt(object *op,object *caster,int dir,int type,int magic) {
  object *tmp=NULL;
  if (!spellarch[type])
    return 0;
  tmp=arch_to_object(spellarch[type]);
  if(tmp==NULL)
    return 0;
  /*  peterm:  level dependency for bolts  */
  tmp->stats.dam = SP_PARAMETERS[type].bdam + SP_level_dam_adjust(op,caster,type);
  tmp->stats.hp = SP_PARAMETERS[type].bdur + SP_level_strength_adjust(op,caster,type);
  if(magic)
    tmp->attacktype|=AT_MAGIC;
  tmp->x=op->x,tmp->y=op->y;
  tmp->direction=dir;
  if(QUERY_FLAG(tmp, FLAG_IS_TURNABLE))
    SET_ANIMATION(tmp, dir);
  set_owner(tmp,op);
  tmp->level = SK_level (caster);
#if 0
  if(op->type==PLAYER)
    tmp->stats.wc=5+(op->contr->shootstrength-5)/5,
    tmp->stats.exp=(op->contr->shootstrength-5)/3+12,
    tmp->stats.hp=8+(op->contr->shootstrength-5)/8;
#endif
  tmp->x+=DIRX(tmp),tmp->y+=DIRY(tmp);
  if(wall(op->map,tmp->x,tmp->y)) {
    if(!QUERY_FLAG(tmp, FLAG_REFLECTING)) {
      free_object(tmp);
      return 0;
    }
    tmp->x=op->x,tmp->y=op->y;
    tmp->direction=absdir(tmp->direction+4);
  }
  if ((tmp = insert_ob_in_map(tmp,op->map,op)) != NULL)
    move_bolt (tmp);
  return 1;
}

/*  peterm  added a type field to fire_arch.  Needed it for making
    fireball etall level dependent.
    Later added a ball-lightning firing routine.
 * dir is direction, at is spell we are firing.  Type is index of spell
 * array.  If magic is 1, then add magical attacktype to spell.
 * op is either the owner of the spell (player who gets exp) or the
 * casting object owned by the owner.  caster is the casting object.
 */

int fire_arch (object *op, object *caster, int dir, archetype *at, int type,
	int magic)
{
	return fire_arch_from_position (op, caster, op->x, op->y, dir, at,
	                                type, magic);
}

int fire_arch_from_position (object *op, object *caster, sint16 x, sint16 y,
	int dir, archetype *at, int type, int magic)
{
  object *tmp, *env;
 
  if(at==NULL)
    return 0;
  for(env=op;env->env!=NULL;env=env->env);
  if (env->map == NULL)
    return 0;
  tmp=arch_to_object(at);
  if(tmp==NULL)
    return 0;
  tmp->stats.sp=type;
  tmp->stats.dam=SP_PARAMETERS[type].bdam+SP_level_dam_adjust(op,caster,type);
  tmp->stats.hp=SP_PARAMETERS[type].bdur+SP_level_strength_adjust(op,caster,type);
  tmp->x=x, tmp->y=y;
  tmp->direction=dir;
  if (get_owner (op) != NULL)
    copy_owner (tmp, op);
  else
    set_owner (tmp, op);
  tmp->level = casting_level (caster, type);
#ifdef MULTIPLE_GODS /* needed for AT_HOLYWORD,AT_GODPOWER stuff */
  if(tmp->attacktype&AT_HOLYWORD||tmp->attacktype&AT_GODPOWER) {
	      if(!tailor_god_spell(tmp,op)) return 0; 
  } else /* Ugly else going across endif */
#endif 
  if(magic)
    tmp->attacktype|=AT_MAGIC;
  if(QUERY_FLAG(tmp, FLAG_IS_TURNABLE))
    SET_ANIMATION(tmp, dir);
#if 0
  if(op->type==PLAYER)
    tmp->stats.hp=(op->contr->shootstrength-10)/10+10;
#endif
  if ((tmp = insert_ob_in_map (tmp, op->map, op)) == NULL)
    return 1;
  switch(type) {
    case SP_M_MISSILE:
    move_missile(tmp);
	break;
    case SP_BALL_LIGHTNING:
	tmp->stats.food=SP_PARAMETERS[type].bdur +
                      SP_level_strength_adjust(op,caster,type);
	move_ball_lightning(tmp);
	break;
    default:
    move_fired_arch(tmp);
  }
  return 1;
}

int
cast_cone(object *op, object *caster,int dir, int strength, int spell_type,archetype *spell_arch, int magic)
{
  object *tmp;
  int i,success=0,range_min= -1,range_max=1;

  if(!dir)
    range_min= -3,range_max=4,strength/=2;

  for(i=range_min;i<=range_max;i++) {
    int x=op->x+freearr_x[absdir(dir+i)],
        y=op->y+freearr_y[absdir(dir+i)];
    if(wall(op->map,x,y))
      continue;
    success=1;
    tmp=arch_to_object(spell_arch);
    set_owner(tmp,op);
    tmp->level = casting_level (caster, spell_type);
    tmp->x=x,tmp->y=y;
#ifdef MULTIPLE_GODS /* holy word stuff */                
    if((tmp->attacktype&AT_HOLYWORD)||(tmp->attacktype&AT_GODPOWER)) {
            if(!tailor_god_spell(tmp,op)) return 0;  
    } else /* god/holy word isnt really 'magic' */
#endif
    if(magic)
      tmp->attacktype|=AT_MAGIC;  /* JWI cone attacks should be considered
                                     magical in nature ;) */
    if(dir)
      tmp->stats.sp=dir;
    else
      tmp->stats.sp=i;
    tmp->stats.hp=strength+SP_level_strength_adjust(op,caster,spell_type);
    tmp->stats.dam=SP_PARAMETERS[spell_type].bdam +
                  SP_level_dam_adjust(op,caster,spell_type); 
    tmp->stats.maxhp=tmp->count;
    if ( ! QUERY_FLAG (tmp, FLAG_FLYING))
      LOG (llevDebug, "cast_cone(): arch %s doesn't have flying 1\n",
           spell_arch->name);
    if ( ! QUERY_FLAG (tmp, FLAG_WALK_ON) || ! QUERY_FLAG (tmp, FLAG_FLY_ON))
      LOG (llevDebug, "cast_cone(): arch %s doesn't have walk_on 1 and "
           "fly_on 1\n", spell_arch->name);
    insert_ob_in_map(tmp,op->map,op);
  }
  return success;
}

void move_cone(object *op) {
    int i;
    tag_t tag;

    if (op->env) {
        /* handle flowers in icecubes */
        op->speed = 0;
        update_ob_speed (op);
        return;
    }

    /* if no map then hit_map will crash so just ignore object */
    if (! op->map) {
	LOG(llevError,"Tried to move_cone object %s without a map.\n",
	    op->name ? op->name : "unknown");
	return;
    }

    /* lava saves it's life, but not yours  :) */
    if (QUERY_FLAG(op, FLAG_LIFESAVE)) {
	hit_map(op,0,op->attacktype);
	return;
    }

    /* If no owner left, the spell dies out. */
    if(get_owner(op)==NULL) {
	remove_ob(op);
	free_object(op);
	return;
    }

    /* Hit map returns 1 if it hits a monster.  If it does, set
     * food to 1, which will stop the cone from progressing.
     */
    tag = op->count;
    op->stats.food |= hit_map(op,0,op->attacktype);
    if (was_destroyed (op, tag))
        return;

    if((op->stats.hp-=2)<0) {
	if(op->stats.exp) {
	    op->speed = 0;
	    update_ob_speed(op);
	    op->stats.exp=0;
	    op->stats.sp=0; /* so they will join */
	} else {
	    remove_ob(op);
	    free_object(op);
	}
	return;
    }

    if(op->stats.food)   return;

    op->stats.food=1;

    for(i= -1;i<2;i++) {
	int x=op->x+freearr_x[absdir(op->stats.sp+i)],
	    y=op->y+freearr_y[absdir(op->stats.sp+i)];

	if(ok_to_put_more(op->map,x,y,op,op->attacktype)) {
	    object *tmp=arch_to_object(op->arch);
            copy_owner (tmp, op);
	    tmp->x=x, tmp->y=y;

	    /* added to make face of death work,and counterspell */
	    tmp->level = op->level;

#ifdef MULTIPLE_GODS /* holy word stuff */
	    if(tmp->attacktype&AT_HOLYWORD||tmp->attacktype&AT_GODPOWER) 
		if(!tailor_god_spell(tmp,op)) return;
#endif
	    tmp->stats.sp=op->stats.sp,tmp->stats.hp=op->stats.hp+1;
	    tmp->stats.maxhp=op->stats.maxhp;
	    tmp->stats.dam = op->stats.dam;
	    tmp->attacktype=op->attacktype;
	    insert_ob_in_map(tmp,op->map,op);
	}
    }
}

void fire_a_ball (object *op, int dir, int strength)
{
  object *tmp;

  if ( ! op->other_arch) {
    LOG (llevError, "BUG: fire_a_ball(): no other_arch\n");
    return;
  }
  if ( ! dir) {
    LOG (llevError, "BUG: fire_a_ball(): no direction\n");
    return;
  }
  tmp = arch_to_object (op->other_arch);
  set_owner(tmp,op);
  tmp->direction=dir;
  tmp->x=op->x,tmp->y=op->y;
  tmp->speed = 1;
  update_ob_speed(tmp);
  tmp->stats.hp=strength;
  tmp->level = op->level;
  SET_ANIMATION(tmp, dir);
  SET_FLAG(tmp, FLAG_FLYING);
  if ((tmp = insert_ob_in_map (tmp, op->map, op)) != NULL)
    move_fired_arch (tmp);
}

void explosion(object *op) {
  object *tmp;
  mapstruct *m=op->map; /* In case we free b */
  int i;

  if(--(op->stats.hp)<0) {
    remove_ob(op);
    free_object(op);
    return;
  }
  if(op->above!=NULL&&op->above->type!=PLAYER) {
    SET_FLAG (op, FLAG_NO_APPLY);
    remove_ob(op);
    insert_ob_in_map(op,op->map,op);
    CLEAR_FLAG (op, FLAG_NO_APPLY);
  }
  hit_map(op,0,op->attacktype);
  if(op->stats.hp>2&&!op->value) {
    op->value=1;
    for(i=1;i<9;i++) {
      int dx,dy;
      if(wall(op->map,dx=op->x+freearr_x[i],dy=op->y+freearr_y[i]))
        continue;
      if(blocks_view(op->map, dx, dy))
        continue;
      if(ok_to_put_more(op->map,dx,dy,op,op->attacktype)) {
        tmp=get_object();
        copy_object(op,tmp); /* This is probably overkill on slow computers.. */
        tmp->state=0;
        tmp->speed_left= -0.21;
        tmp->stats.hp--;
        tmp->value=0;
        tmp->x=dx,tmp->y=dy;
        insert_ob_in_map(tmp,m,op);
      }
    }
  }
}

int reflwall(mapstruct *m,int x,int y) {
  object *op;
  if(out_of_map(m,x,y)) return 0;
  for(op=get_map_ob(m,x,y);op!=NULL;op=op->above)
    if(QUERY_FLAG(op, FLAG_REFL_SPELL))
      return 1;
  return 0;
}

void move_bolt(object *op) {
  object *tmp;
  int w,r;
  if(--(op->stats.hp)<0) {
    remove_ob(op);
    free_object(op);
    return;
  }
  hit_map(op,0,op->attacktype);
  if(!op->value&&--(op->stats.exp)>0) {
    op->value=1;
    if(!op->direction)
      return;
    /*
     * The bolt stops if it hits someone who is immune to it.
     */
    tmp=get_map_ob(op->map,op->x,op->y);
    while(tmp!=NULL&&(!QUERY_FLAG(tmp, FLAG_ALIVE)||!(tmp->immune&op->attacktype)))
      tmp=tmp->above;
    if(tmp!=NULL) {
      remove_ob(op);
      free_object(op);
      return;
    }
    if(blocks_view(op->map,op->x+DIRX(op),op->y+DIRY(op)))
      return;
    w=wall(op->map,op->x+DIRX(op),op->y+DIRY(op));
    r=reflwall(op->map,op->x+DIRX(op),op->y+DIRY(op));
    if(w&&!QUERY_FLAG(op, FLAG_REFLECTING))
      return;
    if(w||r) { /* We're about to bounce */
      if(!QUERY_FLAG(op, FLAG_REFLECTING))
        return;
      op->value=0;
      if(op->direction&1)
        op->direction=absdir(op->direction+4);
      else {
        int left= wall(op->map,op->x+freearr_x[absdir(op->direction-1)],
                              op->y+freearr_y[absdir(op->direction-1)]),
            right=wall(op->map,op->x+freearr_x[absdir(op->direction+1)],
                              op->y+freearr_y[absdir(op->direction+1)]);
        if(left==right)
          op->direction=absdir(op->direction+4);
        else if(left)
          op->direction=absdir(op->direction+2);
        else if(right)
          op->direction=absdir(op->direction-2);
      }
      update_turn_face(op); /* A bolt *must* be IS_TURNABLE */
      return;
    }
    else { /* Create a copy of this object and put it ahead */
      tmp=get_object();
      copy_object(op,tmp);
      tmp->speed_left= -0.1;
      tmp->value=0;
      tmp->stats.hp++;
      tmp->x+=DIRX(tmp),tmp->y+=DIRY(tmp);
      tmp = insert_ob_in_map(tmp,op->map,op);
      if (tmp) {
        if ( ! tmp->stats.food) {
          tmp->stats.food = 1;
          move_bolt (tmp);
        } else {
          tmp->stats.food = 0;
        }
      }
    }
  }
}


/* updated this to allow more than the golem 'head' to attack */
/* op is the golem to be moved. */

void move_golem(object *op) {
    int made_attack=0;
    object *tmp;
    tag_t tag;

    if(QUERY_FLAG(op, FLAG_MONSTER))
	return; /* Has already been moved */

    if(get_owner(op)==NULL) {
	LOG(llevDebug,"Golem without owner destructed.\n");
	remove_ob(op);
	free_object(op);
	return;
    }
    /* It would be nice to have a cleaner way of what message to print
     * when the golem expires than these hard coded entries.
     */
    if(--op->stats.hp<0) {
	char buf[MAX_BUF];
	if(op->exp_obj && op->exp_obj->stats.Wis) {
	    if(op->inv) 
		strcpy(buf,"Your staff stops slithering around and lies still.");
	    else
		sprintf(buf,"Your %s departed this plane.",op->name);
	} else if (!strncmp(op->name,"animated ",9)) {
	    sprintf(buf,"Your %s falls to the ground.",op->name);
	} else {
	    sprintf(buf,"Your %s dissolved.",op->name);
	}
	new_draw_info(NDI_UNIQUE, 0,op->owner,buf);
	remove_friendly_object(op);
	op->owner->contr->golem=NULL;
	remove_ob(op);
	free_object(op);
	return;
    }

    /* Do golem attacks/movement for single & multisq golems. 
     * Assuming here that op is the 'head' object. Pass only op to 
     * move_ob (makes recursive calls to other parts) 
     * move_ob returns 0 if the creature was not able to move.
     */
    tag = op->count;
    if(move_ob(op,op->direction,op)) return;
    if (was_destroyed (op, tag))
        return;

    for(tmp=op;tmp;tmp=tmp->more) { 
	int x=tmp->x+freearr_x[op->direction],y=tmp->y+freearr_y[op->direction];
	object *victim;

	if (out_of_map(op->map,x,y)) continue;

	for(victim=get_map_ob(op->map,x,y);victim;victim=victim->above) 
	    if(QUERY_FLAG(victim,FLAG_ALIVE)) break;

	/* We used to call will_hit_self to make sure we don't
	 * hit ourselves, but that didn't work, and I don't really
	 * know if that was more efficient anyways than this.
	 * This at least works.  Note that victim->head can be NULL,
	 * but since we are not trying to dereferance that pointer,
	 * that isn't a problem.
	 */
	if(victim && victim!=op && victim->head!=op) {

	    /* for golems with race fields, we don't attack
	     * aligned races */

	    if(victim->race&&op->race&&strstr(op->race,victim->race)) {
		if(op->owner) new_draw_info_format(NDI_UNIQUE, 0,op->owner,
			"%s avoids damaging %s.",op->name,victim->name);
	    } else if (op->exp_obj && op->exp_obj->stats.Wis
		       && victim == op->owner) {
		if(op->owner) new_draw_info_format(NDI_UNIQUE, 0,op->owner,
				"%s avoids damaging you.",op->name);
	    } else {
		/* I think using hit_map here is just wrong -
		 * we are not attacking a space - we have a specific
		 * creature we are attacking, attack_ob seems more
		 * appropriate.
		 */
		
		attack_ob(victim,op);
/*		hit_map(tmp,op->direction,op->attacktype);*/
		made_attack=1;
	    }
	} /* If victim */
    }
    if(made_attack) update_object(op);
}

void control_golem(object *op,int dir) {
  op->direction=dir;
}


void move_missile(object *op) {
  int i;
  object *owner;
  sint16 new_x, new_y;

  owner = get_owner(op);
  if (owner == NULL) {
    remove_ob(op);
    free_object(op);
    return;
  }

  new_x = op->x + DIRX(op);
  new_y = op->y + DIRY(op);

  if (blocked (op->map, new_x, new_y)) {
    tag_t tag = op->count;
    hit_map (op, op->direction, AT_MAGIC);
    if ( ! was_destroyed (op, tag)) {
      remove_ob (op);
      free_object(op);
    }
    return;
  }

  remove_ob(op);
  if ( ! op->direction || wall (op->map, new_x, new_y)
      || blocks_view (op->map, new_x, new_y))
  {
    free_object(op);
    return;
  }
  op->x = new_x;
  op->y = new_y;
  i=find_dir(op->map,op->x,op->y,get_owner(op));
  if(i&&i!=op->direction){
    op->direction=absdir(op->direction+((op->direction-i+8)%8<4?-1:1));
    SET_ANIMATION(op, op->direction);
  }
  insert_ob_in_map(op,op->map,op);
}

int explode_object(object *op) {
  object *tmp, *victim, *env;

  if(out_of_map(op->map,op->x,op->y))  /*  peterm:  check for out of map obj's.*/
    {
      return 0;
    }
  for(env=op;env->env!=NULL;env=env->env);
  if (env->map == NULL)
    return 0;
  if(op->other_arch==NULL)
    return 0;
  tmp=arch_to_object(op->other_arch);

  /* peterm: Hack added to make objects be able to both hit for damage and
    then explode.  */
  if(op->attacktype){
      for(victim=get_map_ob(op->map,op->x,op->y);victim!=NULL;victim=victim->above)
        if(QUERY_FLAG(victim,FLAG_ALIVE))
          break;
      hit_map(op,0,op->attacktype);

#if 0
    /* Hit_map will also do a hit_player for us.  Leaving this call in
     * effectively doubles the amount of damage the bullet is doing.
     */
      /* Should hit_map also be doing this?  Why call hit_player
       * again?  Also, make sure victim has not been killed - it
       * is possible that hit_map killed the object.
       */
      if(victim!=NULL && !QUERY_FLAG(victim,FLAG_FREED))
	 hit_player(victim,op->stats.dam,op,op->attacktype);
#endif
    }

  /*  peterm:  hack added to make fireballs and other explosions level
   *  dependent:
   */

  /*  op->stats.sp stores the spell which made this object here. */
  tmp->stats.dam += SP_level_dam_adjust(op,op,op->stats.sp);
  if(op->attacktype&AT_MAGIC)
    tmp->attacktype|=AT_MAGIC;
  copy_owner (tmp, op);
  if(op->stats.hp)
    tmp->stats.hp=op->stats.hp;
  tmp->stats.maxhp=op->count; /* Unique ID */
  tmp->x=env->x,tmp->y=env->y;

#ifdef MULTIPLE_GODS /* needed for AT_HOLYWORD stuff -b.t. */
  if(tmp->attacktype&AT_HOLYWORD||tmp->attacktype&AT_GODPOWER) 
          if(!tailor_god_spell(tmp,op)) return 0;   
#endif

  if (wall(env->map,env->x,env->y))
    tmp->x-=DIRX(env),tmp->y-=DIRY(env);
  if (out_of_map(env->map, env->x, env->y))
    free_object(tmp);
  else
    insert_ob_in_map(tmp,env->map,op);
  free_object(op);
  return 1;
}

void check_fired_arch(object *op) {
  if(blocked(op->map,op->x,op->y)) {
    object *tmp;
    remove_ob(op);
    if(out_of_map(op->map,op->x,op->y)) {
	free_object(op);
	return;
    }
    if(explode_object(op))
      return;
    for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above)
      if(QUERY_FLAG(tmp, FLAG_ALIVE))
        break;
    if(tmp!=NULL)
      op->stats.dam-=hit_player(tmp,op->stats.dam,op,op->attacktype);
    if(blocked(op->map,op->x,op->y)) {
      free_object(op);
      return;
    }
    insert_ob_in_map(op,op->map,op);
  }
}

void move_fired_arch(object *op) {
    remove_ob(op);

    /* peterm:  added to make comet leave a trail of burnouts 
	it's an unadulterated hack, but the effect is cool.	*/
    if(op->stats.sp == SP_METEOR) {
	object * tmp1=arch_to_object(find_archetype("fire_trail"));

        tmp1->x = op->x; tmp1->y = op->y;
        insert_ob_in_map(tmp1,op->map,op);
    } /* end addition.  */

    op->x+=DIRX(op),op->y+=DIRY(op);
    if(!op->direction||wall(op->map,op->x,op->y)) {
	if(explode_object(op))
	    return;
	free_object(op);
	return;
    }

    if(reflwall(op->map,op->x,op->y)) {
	op->direction=absdir(op->direction+4);
	if ((op = insert_ob_in_map(op,op->map,op)) != NULL)
	    update_turn_face(op);
	return;
    }
    if(blocked(op->map,op->x,op->y)) {
	object *tmp;

	if(out_of_map(op->map,op->x,op->y)) {
	    free_object(op);
	    return;
	}

	if(explode_object(op))
	    return;

	for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above)
	    if(QUERY_FLAG(tmp, FLAG_ALIVE))
		break;

	if(tmp!=NULL) {
	    /* Certain items, like speedballs, have attacktype ghosthit.
	     * hit_player wants to remove the object after it hits the player.
	     * Since it is already removed, just don't make it ghosthit, and
	     * remove it here
	     */

	    if (op->attacktype & AT_GHOSTHIT) {
		hit_player(tmp,op->stats.dam,op,(op->attacktype & ~AT_GHOSTHIT));
		free_object(op);
		return;
	    }
	    else
		op->stats.dam-=hit_player(tmp,op->stats.dam,op,op->attacktype);
	}
	/* I guess this can be applicable if the object blocking the
	 * space was destroyed?
	 */
	if(blocked(op->map,op->x,op->y)) {
	    free_object(op);
	    return;
	}
    } /* if space is blocked */
    insert_ob_in_map(op,op->map,op);
}


void drain_rod_charge(object *rod) {
  rod->stats.hp -= spells[rod->stats.sp].sp;
  if (QUERY_FLAG(rod, FLAG_ANIMATE))
    fix_rod_speed(rod);
}

void fix_rod_speed(object *rod) {
  rod->speed = (FABS(rod->arch->clone.speed)*rod->stats.hp) /
               (float)rod->stats.maxhp;
  if (rod->speed < 0.02)
    rod->speed = 0.02;
  update_ob_speed(rod);
}


/*  this function is commonly used to find a friendly target for
spells such as heal or protection or armour  */

object *find_target_for_friendly_spell(object *op,int dir) 
{ object *tmp;
  if(op->type!=PLAYER&&op->type!=RUNE) {
    tmp=get_owner(op);
    /* If the owner does not exist, or is not a monster, than apply the spell
     * to the caster.
     */
    if(!tmp || !QUERY_FLAG(tmp,FLAG_MONSTER)) tmp=op;
  }
  else {
    if (out_of_map(op->map, op->x+freearr_x[dir],op->y+freearr_y[dir]))
	tmp=NULL;
    else  {
      for(tmp=get_map_ob(op->map,op->x+freearr_x[dir],op->y+freearr_y[dir]);
        tmp!=NULL;
        tmp=tmp->above)
      if(tmp->type==PLAYER)
        break;
    }
  }
  if(tmp==NULL)               /* didn't find a player there, look in current square for a player */
    for(tmp=get_map_ob(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above)
      if(tmp->type==PLAYER)
        break;
  return tmp;
}


/*  peterm: ball lightning mover.  */
/*  ball lightning automatically seeks out a victim, if
    it sees any monsters close enough.  */

void move_ball_lightning(object *op) {
    int i,nx,ny,tx,ty;
    remove_ob(op);
    nx=op->x+DIRX(op);
    ny=op->y+DIRY(op);
    ty=op->y;
    tx=op->x;  /*  the following logic makes sure that the ball
		    doesn't move into a wall, and makes
		    sure that it will move along a wall to try and
		    get at it's victim.  */
    if(!wall(op->map, nx, ny)&&!blocks_view(op->map,nx,ny)) {
	tx=nx;
	ty=ny;
    }
    else
    {  i=RANDOM()%2;
	if(i) {
	if(!wall(op->map,op->x,ny)&&!blocks_view(op->map,op->x,ny)) ty=ny;
	else if(!wall(op->map,nx,op->y)&&!blocks_view(op->map,nx,op->y)) tx=nx;
	}
	else {
	if(!wall(op->map,nx,op->y)&&!blocks_view(op->map,nx,op->y)) tx=nx;
	else if(!wall(op->map,op->x,ny)&&!blocks_view(op->map,op->x,ny)) ty=ny;
	}
    }
    op->y=ty;
    op->x=tx;

    if(blocked(op->map,op->x,op->y)) hit_map(op,0,op->attacktype);

    i=spell_find_dir(op->map,op->x,op->y,get_owner(op));

    if(i) op->direction=i;
    insert_ob_in_map(op,op->map,op);
}
	
/* raytrace:
 * spell_find_dir(map, x, y, exclude) will search first the center square
 * then some close squares in the given map at the given coordinates for
 * live objects.
 * It will not considered the object given as exlude among possible
 * live objects.
 * It returns the direction toward the first/closest live object if finds
 * any, otherwise 0.
 */

int spell_find_dir(mapstruct *m, int x, int y, object *exclude) {
  int i,max=SIZEOFFREE;
  object *tmp;
  if (exclude && exclude->head)
    exclude = exclude->head;

  for(i=(RANDOM()%8)+1;i<max;i++) {
    if(wall(m, x+freearr_x[i],y+freearr_y[i]))
      max=maxfree[i];
    else {
      tmp=get_map_ob(m,x+freearr_x[i],y+freearr_y[i]);
      while(tmp!=NULL && ((tmp!=NULL&&!QUERY_FLAG(tmp,FLAG_MONSTER)&&
        tmp->type!=PLAYER&&!QUERY_FLAG(tmp,FLAG_GENERATOR)) ||
	(tmp == exclude || (tmp->head && tmp->head == exclude))))
                tmp=tmp->above;
      if(tmp!=NULL)
        return freedir[i];
    }
  }
  return 0;
}


/* peterm:  */

/*  peterm:  the following defines the parameters for all the 
spells.  
    bdam:  base damage or hp of spell or summoned monster
  bdur:  base duration of spell or base range
  ldam:  levels you need over the min for the spell to gain one dam
  ldur:  levels you need over the min for the spell to gain one dur
*/


/*  The following adjustments to spell strength are done in the
philosophy that the longer one knows a spell, the better one
should get at it.  So the more experience levels you are above
the minimum for knowing a spell, the more effective it becomes. 
most of the following adjustments are for damage only, some are
for turning undead and whatnot.  

  The following functions assume that casting the spell is not
denied.  Denied spells have an undefined path level modifier.
There wouldn't be a meaningful result anyway.

  The arrays are defined in spells.h*/

/* July 1995 - I changed the next 3 functions slightly by replacing
 * the casters level (op->level) with the skill level (SK_level(op)) 
 * instead for when we have compiled with ALLOW_SKILLS - b.t. 
 */
/* now based on caster's level instead of on op's level and caster's	*
 * path modifiers. 	--DAMN						*/

int SP_level_dam_adjust(object *op, object *caster, int spell_type)
{
    int level = casting_level (caster, spell_type);
    int adj = (level-spells[spell_type].level);
    if(adj < 0) adj=0;
    if(SP_PARAMETERS[spell_type].ldam)
	adj/=SP_PARAMETERS[spell_type].ldam;
    else adj=0;
 return adj;
}

/* July 1995 - changed slightly (SK_level) for ALLOW_SKILLS - b.t. */
/* now based on caster's level instead of on op's level and caster's	*
 * path modifiers. 	--DAMN						*/
int SP_level_strength_adjust(object *op, object *caster, int spell_type)
{
    int level = casting_level (caster, spell_type);
    int adj = (level-spells[spell_type].level);
    if(adj < 0) adj=0;
    if(SP_PARAMETERS[spell_type].ldur)
	adj/=SP_PARAMETERS[spell_type].ldur;
    else adj=0;
 return adj;
}

/*  The following function scales the spellpoint cost of
a spell by it's increased effectiveness.  Some of the
lower level spells become incredibly vicious at high
levels.  Very cheap mass destruction.  This function is
intended to keep the sp cost related to the effectiveness. */

/* July 1995 - changed slightly (SK_level) for ALLOW_SKILLS - b.t. */

int SP_level_spellpoint_cost(object *op, object *caster, int spell_type)
{
  spell *s=find_spell(spell_type);
  int level = casting_level (caster, spell_type);
#ifdef SPELLPOINT_LEVEL_DEPEND
  int sp;
  if(SP_PARAMETERS[spell_type].spl)
   sp= (int) (spells[spell_type].sp * 
	       (1.0 + 
	(MAX(0,	(float)(level-spells[spell_type].level)/
	(float)SP_PARAMETERS[spell_type].spl ))));
  else sp= spells[spell_type].sp;
  sp *= PATH_SP_MULT(caster,s);
  return MIN(sp,(spells[spell_type].sp + 50));
#else
  return s->sp*PATH_SP_MULT(caster,s);
#endif /* SPELLPOINT_LEVEL_DEPEND */  
}



/*  move_swarm_spell:  peterm  */
/*  This is an implementation of the swarm spell.  It was written for
meteor swarm, but it could be used for any swarm.  A swarm spell
is a special type of object that casts swarms of other types
of spells.  Which spell it casts is flexible.  It fires the spells
from a set of squares surrounding the caster, in a given direction. */

void move_swarm_spell(object *op)
{
    sint16 x,y;
    int di;

    if(op->stats.hp == 0 || get_owner (op) == NULL) {
	remove_ob(op);
	free_object(op);
	return;
    }
    op->stats.hp--;

   if(op->stats.hp)
        di=RANDOM()%7-3;  /* get a random number of -3->3 */
   else
        di=0;  /* fire the last one from forward. */
   x = op->x + freearr_x[absdir(op->direction +di)];
   y = op->y + freearr_y[absdir(op->direction +di)];

   /*  for level dependence, we need to know what spell is fired.  */
   /*  that's stored in op->stats.sp  by fire_swarm  */
   if ( ! wall (op->map, x, y))
       fire_arch_from_position (op, op, x, y, op->direction, op->other_arch,
                                op->stats.sp, op->magic);
}



/*  fire_swarm:  peterm */
/*  The following routine creates a swarm of objects.  It actually
    sets up a specific swarm object, which then fires off all
    the parts of the swarm.  

  Interface:
    op:  the owner
    caster: the caster (owner, wand, rod, scroll)
    dir: the direction everything will be fired in
    swarm_type:  the archetype that will be fired
    spell_type:  the spell type of the archetype that's fired
    n:  the number to be fired.
*/
    

void fire_swarm (object *op, object *caster, int dir, archetype *swarm_type,
	int spell_type, int n, int magic)
{
  object *tmp;
  tmp=get_archetype("swarm_spell");
  tmp->x=op->x;
  tmp->y=op->y;	    
  set_owner(tmp,op);       /* needed so that if swarm elements kill, caster gets xp.*/
  tmp->level=casting_level(caster, spell_type);   /*needed later, to get level dep. right.*/
  tmp->stats.sp=spell_type;  /* needed later, see move_swarm_spell */
#ifdef MULTIPLE_GODS
  tmp->attacktype = swarm_type->clone.attacktype;
  if (tmp->attacktype & AT_HOLYWORD || tmp->attacktype & AT_GODPOWER) {
    if ( ! tailor_god_spell (tmp, op))
      return;
  }
#endif 
  tmp->magic = magic;
  tmp->stats.hp=n;	    /* n in swarm*/
  tmp->other_arch=swarm_type;  /* the archetype of the things to be fired*/
  tmp->direction=dir; 
  tmp->invisible=1;
  insert_ob_in_map(tmp,op->map,op);
}

	    
/*  look_up_spell_by_name:  peterm
    this function attempts to find the spell spname in spells[].
    if it doesn't exist, or if the op cannot cast that spname,
    -1 is returned.  */


int look_up_spell_by_name(object *op,char *spname) {
    int numknown;
    int spnum;
    int plen;
    int spellen;
    int i;

    if(spname==NULL) return -1;
    if(op==NULL) numknown=NROFREALSPELLS;
	else
	if(QUERY_FLAG(op, FLAG_WIZ)) numknown=NROFREALSPELLS;
	    else numknown = op->contr->nrofknownspells;
    plen=strlen(spname);
    for(i=0;i<numknown;i++) {
	if(op==NULL) spnum=i;
	else
	    if(QUERY_FLAG(op,FLAG_WIZ)) spnum=i;
		else  spnum = op->contr->known_spells[i];

	spellen=strlen(spells[spnum].name);

	if(strncmp(spname,spells[spnum].name,MIN(spellen,plen)) == 0 ) 
	    return spnum;
    }
    return -1;
}



void put_a_monster(object *op,char *monstername) {
  object *tmp,*head=NULL,*prev=NULL;
  archetype *at;
  int dir;
  
  /* find a free square nearby */
  /* first we check the closest square for free squares */
  if((at=find_archetype(monstername))==NULL) return;
  dir=find_first_free_spot(at,op->map,op->x,op->y);
  if(dir!=-1) {
    /* This is basically grabbed for generate monster.  Fixed 971225 to
     * insert multipart monsters properly
     */
    while (at!=NULL) {
	tmp=arch_to_object(at);
	tmp->x=op->x+freearr_x[dir]+at->clone.x;
	tmp->y=op->y+freearr_y[dir]+at->clone.y;
	if (head) {
	    tmp->head=head;
	    prev->more=tmp;
	}
	if (!head) head=tmp;
	prev=tmp;
	at=at->more;
    }

    insert_ob_in_map(head,op->map,op);

    /* thought it'd be cool to insert a burnout, too.*/
    tmp=get_archetype("burnout");
    tmp->map = op->map;
    tmp->x=op->x+freearr_x[dir];
    tmp->y=op->y+freearr_y[dir];
    insert_ob_in_map(tmp,op->map,op);
    }
}


/*  Some local definitions for shuffle-attack */
int color_array[20];
#define black 0
#define white 1
#define red 2
#define light_blue 3
#define blue 4
#define light_green 5
#define green 6
#define yellow 7
#define khaki 8
    struct {
	int attacktype;
	int face;
	int fg;
	int bg;
    } ATTACKS[22] = {
	{AT_PHYSICAL,0,0,12},
	{AT_PHYSICAL,0,0,12},  /*face = explosion*/
	{AT_PHYSICAL,0,0,12},
	{AT_MAGIC,1,1,12},
	{AT_MAGIC,1,1,12},   /* face = last-burnout */
	{AT_MAGIC,1,1,12},
	{AT_FIRE,2,3,12},
	{AT_FIRE,2,3,12},  /* face = fire....  */
	{AT_FIRE,2,3,12},
	{AT_ELECTRICITY,3,11,9},
	{AT_ELECTRICITY,3,11,9},  /* ball_lightning */
	{AT_ELECTRICITY,3,11,9},
	{AT_COLD,4,5,1},	
	{AT_COLD,4,5,1},  /* face=icestorm*/
	{AT_COLD,4,5,1},
	{AT_CONFUSION,5,0,12},
	{AT_POISON,7,0,12},
	{AT_POISON,7,0,12}, /* face = acid sphere.  generator */
	{AT_POISON,7,8,12},  /* poisoncloud face */
	{AT_SLOW,8,0,12},
	{AT_PARALYZE,9,11,9},
	{AT_FEAR,10,0,12}  };



/*  shuffle_attack:  peterm */
/*  This routine shuffles the attack of op to one of the 
   ones in the list.  It does this at random.  It also
   chooses a face appropriate to the attack that is
   being committed by that square at the moment.  
    right now it's being used by color spray and create pool of
    chaos.  */

void shuffle_attack(object *op,int change_face)
{
    int i;
    i=RANDOM()%22;
    op->attacktype|=ATTACKS[i].attacktype|AT_MAGIC;
    if(change_face) {
	SET_ANIMATION(op, ATTACKS[i].face);
    }
}


/*  the following function reads from the file 'spell_params' in	
the lib dir, and resets the array in memory to reflect the values
in spell_parameters.  The format in there MUST be:
spell name
spell_number bdam bdur ldam ldur
 for
base damage of spell, base duration of spell, level-dependency for damage
level-dependency for duration--examples
magic bullet
0 0 0 0
large icestorm
0 1 1 1
small fireball
1 0 0 8
....

The parameters have different effects for different spells.
Please refer to the documentation.
*/
void init_spell_param()
{
  FILE *spell_params;
  char fname[MAX_BUF];
  char spell_name[50];
  char spell_attrib[50];
  int bdam,bdur,ldam,ldur;
  int sp;
  int level;
  int spellindex;
  int spl;  /*  the spellpoint level dependency */

  /* This is hokey, but this function gets called everytime.
  I need these colors for shuffle-attack to work right, and
  they seem to change form implementation to implemention of
  crossfire.  So I'm making my own array here for use in
  shuffle-attack. it's global in scope to this file.*/
  
  color_array[black]=find_color("black");
  color_array[white]=find_color("white");
  color_array[red]=find_color("red");
  color_array[light_blue]=find_color("light_blue");
  color_array[blue]=find_color("blue");
  color_array[light_green]=find_color("light_green");
  color_array[green]=find_color("green");
  color_array[yellow]=find_color("yellow");
  color_array[khaki]=find_color("khaki");

  /*explosion--for physical*/
  ATTACKS[0].fg=color_array[black];
  ATTACKS[0].bg=color_array[khaki];
  ATTACKS[1].fg=color_array[black];
  ATTACKS[1].bg=color_array[khaki];
  ATTACKS[2].fg=color_array[black];
  ATTACKS[2].bg=color_array[khaki];  
  /*magic--burnout attack */
  ATTACKS[3].fg=color_array[light_blue];
  ATTACKS[3].bg=color_array[khaki];
  ATTACKS[4].fg=color_array[light_blue];
  ATTACKS[4].bg=color_array[khaki];
  ATTACKS[5].fg=color_array[light_blue];
  ATTACKS[5].bg=color_array[khaki];  
  /*fire--for fire*/
  ATTACKS[6].fg=color_array[red];
  ATTACKS[6].bg=color_array[khaki];
  ATTACKS[7].fg=color_array[red];
  ATTACKS[7].bg=color_array[khaki];
  ATTACKS[8].fg=color_array[red];
  ATTACKS[8].bg=color_array[khaki];  
  /*electricity--ball lightning face */
  ATTACKS[9].fg=color_array[yellow];
  ATTACKS[9].bg=color_array[khaki];
  ATTACKS[10].fg=color_array[yellow];
  ATTACKS[10].bg=color_array[khaki];
  ATTACKS[11].fg=color_array[yellow];
  ATTACKS[11].bg=color_array[khaki];  
  /*icestorm--for cold*/
  ATTACKS[12].fg=color_array[light_blue];
  ATTACKS[12].bg=color_array[white];
  ATTACKS[13].fg=color_array[light_blue];
  ATTACKS[13].bg=color_array[white];
  ATTACKS[14].fg=color_array[light_blue];
  ATTACKS[14].bg=color_array[white];
  /* madness--madness*/
  ATTACKS[15].fg=color_array[black];
  ATTACKS[15].bg=color_array[khaki];

  /* poison --  poisoncloud */
  ATTACKS[16].fg=color_array[white];
  ATTACKS[16].bg=color_array[light_green];
  ATTACKS[17].fg=color_array[white];
  ATTACKS[17].bg=color_array[light_green];
  ATTACKS[18].fg=color_array[white];
  ATTACKS[18].bg=color_array[light_green];
  

  /*  slow */
  
  ATTACKS[19].fg=color_array[black];
  ATTACKS[19].bg=color_array[khaki];
  /* paralize -- stars */
  ATTACKS[20].fg=color_array[yellow];
  ATTACKS[20].bg=color_array[khaki];  
  /* fear */

  ATTACKS[21].fg=color_array[black];
  ATTACKS[21].bg=color_array[khaki];  

  sprintf(fname,"%s/spell_params",settings.datadir);
  if(! (spell_params=fopen(fname,"r")))
    {
	perror(fname);
	return;
    }
	
  while(!feof(spell_params))
    {
	fgets(spell_name,49,spell_params);
	spellindex=look_up_spell_by_name(NULL,spell_name);
	if(spellindex==-1) {
	    fprintf(stderr,"\nUnrecognized spell: %s",spell_name);
	    continue;
	}
	fgets(spell_attrib,49,spell_params);
	sscanf(spell_attrib,"%d %d %d %d %d %d %d",&level,&sp,&bdam,&bdur,&ldam,&ldur,&spl);
	spells[spellindex].sp=sp;
	spells[spellindex].level=level;
	SP_PARAMETERS[spellindex].bdam=bdam;
	SP_PARAMETERS[spellindex].bdur=bdur;
        SP_PARAMETERS[spellindex].ldam=ldam;
        SP_PARAMETERS[spellindex].ldur=ldur;
	SP_PARAMETERS[spellindex].spl=spl;
    }
    fclose(spell_params);
}

/* get_pointed_target() - this is used by finger of death
 * and the 'smite' spells. Returns the pointer to the first
 * monster in the direction which is pointed to by op. b.t.
 */
 
object *get_pointed_target(object *op, int dir) {
  object *target;
  int x,y;

  if (dir==0) return NULL;
  for(x=op->x+freearr_x[dir],y=op->y+freearr_y[dir]
     ;!out_of_map(op->map,x,y)&&!blocks_view(op->map,x,y)
     &&!wall(op->map,x,y);x+=freearr_x[dir],y+=freearr_y[dir])
        for(target=get_map_ob(op->map,x,y);target;target=target->above) {
              if(QUERY_FLAG(target->head?target->head:target,FLAG_MONSTER)) {
                   if(!blocks_magic(op->map,x,y))
                          return target;
                   else break;
	      }
        }
 
  return ((object *) NULL);
}

/* cast_smite_arch() - the priest points to a creature and causes
 * a 'godly curse' to decend. I generalized this a bit so that several
 * spells will be possible to use w/ this code (eg fire_arch, cast_cone).
 * -b.t.
 */

int cast_smite_spell (object *op, object *caster,int dir, int type) {
   object *effect, *target = get_pointed_target(op,dir);
#ifdef MULTIPLE_GODS
   object *god = find_god(determine_god(op));
#endif

   if(!target || QUERY_FLAG(target,FLAG_REFL_SPELL)
#ifdef MULTIPLE_GODS /* if we don't worship a god, or target a creature
                      * of our god, the spell will fail.  */
      ||!god
      ||(target->title&&!strcmp(target->title,god->name))
      ||(target->race&&strstr(target->race,god->race))
#endif
   ) {
        new_draw_info(NDI_UNIQUE,0,op,"Your request is unheeded.");
        return 0;
   }

   if (spellarch[type] != (archetype *) NULL)
      effect = arch_to_object(spellarch[type]);
   else
      return 0;

  /* tailor the effect by priest level and worshipped God */
   effect->level = casting_level (caster, type);
#ifdef MULTIPLE_GODS
   if(effect->attacktype&AT_HOLYWORD||effect->attacktype&AT_GODPOWER) {
        if(tailor_god_spell(effect,op))
           new_draw_info_format(NDI_UNIQUE,0,op,
                "%s answers your call!",determine_god(op));
        else {
           new_draw_info(NDI_UNIQUE,0,op,"Your request is ignored.");
           return 0;
	}
   }
#endif 
 
   /* size of the area of destruction */
   effect->stats.hp=SP_PARAMETERS[type].bdur +
                SP_level_strength_adjust(op,caster,type);
   /* how much woe to inflict :) */
   effect->stats.dam=SP_PARAMETERS[type].bdam +
                SP_level_dam_adjust(op,caster,type);
   if(effect->stats.dam<0) effect->stats.dam = 127;
   effect->stats.maxhp=effect->count; /*??*/
   set_owner(effect,op);
 
   /* ok, tell it where to be, and insert! */
   effect->x=target->x;effect->y=target->y;
   insert_ob_in_map(effect,op->map,op);
 
   return 1;
}


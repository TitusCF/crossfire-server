/*
 * static char *rcsid_spells_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to mark@pyramid.com.
*/

#ifndef SPELLS_H_1
#define SPELLS_H_1

/* set constant definitions on the first pass, before object.h */
#define NRSPELLPATHS	20
#define NROFREALSPELLS	196	/* Number of different spells */

#define PATH_NULL	0x00000000
#define PATH_PROT	0x00000001 /*       1 */
#define PATH_FIRE	0x00000002 /*       2 */
#define PATH_FROST	0x00000004 /*       4 */
#define PATH_ELEC	0x00000008 /*       8 */
#define PATH_MISSILE	0x00000010 /*      16 */
#define PATH_SELF	0x00000020 /*      32 */
#define PATH_SUMMON	0x00000040 /*      64 */
#define PATH_ABJURE	0x00000080 /*     128 */
#define PATH_RESTORE	0x00000100 /*     256 */
#define PATH_DETONATE	0x00000200 /*     512 */
#define PATH_MIND	0x00000400 /*    1024 */
#define PATH_CREATE	0x00000800 /*    2048 */
#define PATH_TELE	0x00001000 /*    4096 */
#define PATH_INFO	0x00002000 /*    8192 */
#define PATH_TRANSMUTE	0x00004000 /*   16384 */
#define PATH_TRANSFER	0x00008000 /*   32768 */
#define PATH_TURNING	0x00010000 /*   65536 */
#define PATH_WOUNDING	0x00020000 /*  131072 */
#define PATH_DEATH	0x00040000 /*  262144 */
#define PATH_LIGHT	0x00080000 /*  524288 */
 
#define DESCRIBE_PATH(retbuf, variable, name) \
    if(variable) { \
      int i,j=0; \
      strcat(retbuf,"(" name ": "); \
      for(i=0; i<NRSPELLPATHS; i++) \
        if(variable & (1<<i)) { \
          if (j) \
            strcat(retbuf,", "); \
          else \
            j = 1; \
          strcat(retbuf, spellpathnames[i]); \
        } \
      strcat(retbuf,")"); \
    }

#define IS_SUMMON_SPELL(spell) (((spell) > SP_BOMB && (spell) < SP_D_DOOR) \
	|| ((spell) == SP_MYSTIC_FIST) || ((spell) == SP_SUMMON_AVATAR) \
	|| ((spell) == SP_HOLY_SERVANT))

#define PATH_SP_MULT(op,spell) (((op->path_attuned & s->path) ? 0.8 : 1) * \
				((op->path_repelled & s->path) ? 1.25 : 1))

#define PATH_TIME_MULT(op,spell) (((op->path_attuned & s->path) ? 0.8 : 1) * \
				((op->path_repelled & s->path) ? 1.25 : 1))

#else
/* set the types and globals on the second pass;  they need object.h */
#ifndef SPELLS_H_2
#define SPELLS_H_2

extern Fontindex fbface[];
extern Fontindex lface[];
extern Fontindex  mmface[];
extern int turn_bonus[];
extern int fear_bonus[];
extern int cleric_chance[];

typedef struct spell_struct {
  char name[BIG_NAME];
  short level;           /* Level required to cast this spell */
  short sp;              /* Spellpoint-cost to cast it */
  short charges;         /* If it can be used in wands, max # of charges */
  float time;            /* How many ticks it takes to cast the spell */
  short scrolls;         /* If it can be used from scrolls, max # of scrolls */
  short scroll_chance;   /* 1-10 probability of finding this as scroll */
  short books;           /* 1-10 probability of finding this as spellbook */
  unsigned range:1;      /* True if this is a range attack spell */
  unsigned defensive:1;  /* True if it is a defensive spell */
  unsigned cleric:1;     /* True if it is a cleric-spell (wis-chance failure)*/
  unsigned onself:1;     /* Should monsters cast this spell on themselves */
  uint32 path;           /* Path this spell belongs to */
  char *archname;	 /* Pointer to archetype used by spell */
} spell;

extern spell spells[NROFREALSPELLS];

typedef struct
{
  sint16 bdam;  /*  base damage  */
  sint16 bdur;  /*  base duration  */
  sint16 ldam;  /*  damage adjustment for level  */
  sint16 ldur;  /*  duration adjustment for level  */
  sint16 spl;	/*  number of levels to increase cost by multiples of base */
} spell_parameters;

/* When adding new spells, don't insert into the middle of the list - 
 * add to the end of the list.  Some archetypes and treasures require
 * the spell numbers to be as they are.
 */

enum spellnrs {
  SP_BULLET,		SP_S_FIREBALL,		SP_M_FIREBALL,		/*0*/
  SP_L_FIREBALL,	SP_BURNING_HANDS,

  SP_S_LIGHTNING,	SP_L_LIGHTNING,		SP_M_MISSILE,		/*5*/
  SP_BOMB,		SP_GOLEM,

  SP_FIRE_ELEM,		SP_EARTH_ELEM,		SP_WATER_ELEM,		/*10*/
  SP_AIR_ELEM,		SP_D_DOOR,	     

  SP_EARTH_WALL,	SP_PARALYZE,		SP_ICESTORM,		/*15*/
  SP_MAGIC_MAPPING,	SP_TURN_UNDEAD,

  SP_FEAR,		SP_POISON_CLOUD,	SP_WOW,			/*20*/
  SP_DESTRUCTION,	SP_PERCEIVE,	     

  SP_WOR,		SP_INVIS,		SP_INVIS_UNDEAD,	/*25*/
  SP_PROBE,		SP_LARGE_BULLET,	     

  SP_IMPROVED_INVIS,	SP_HOLY_WORD,		SP_MINOR_HEAL,		/*30*/
  SP_MED_HEAL,		SP_MAJOR_HEAL,

  SP_HEAL,		SP_CREATE_FOOD,		SP_EARTH_DUST,		/*35*/
  SP_ARMOUR,		SP_STRENGTH,

  SP_DEXTERITY,		SP_CONSTITUTION,	SP_CHARISMA,		/*40*/
  SP_FIRE_WALL,		SP_FROST_WALL,

  SP_PROT_COLD,		SP_PROT_ELEC,		SP_PROT_FIRE,		/*45*/
  SP_PROT_POISON,	SP_PROT_SLOW,

  SP_PROT_PARALYZE,	SP_PROT_DRAIN,		SP_PROT_MAGIC,		/*50*/
  SP_PROT_ATTACK,	SP_LEVITATE,

  SP_SMALL_SPEEDBALL,	SP_LARGE_SPEEDBALL,	SP_HELLFIRE,		/*55*/
  SP_FIREBREATH,	SP_LARGE_ICESTORM,

  SP_CHARGING,		SP_POLYMORPH,		SP_CANCELLATION,	/*60*/
  SP_CONFUSION,		SP_MASS_CONFUSION,

  SP_PET,		SP_SLOW,		SP_REGENERATE_SPELLPOINTS,/*65*/
  SP_CURE_POISON,	SP_PROT_CONFUSE,

  SP_PROT_CANCEL,	SP_PROT_DEPLETE,	SP_ALCHEMY,		/*70*/
  SP_REMOVE_CURSE,	SP_REMOVE_DAMNATION,	

  SP_IDENTIFY,		SP_DETECT_MAGIC,	SP_DETECT_MONSTER,	/*75*/
  SP_DETECT_EVIL,	SP_DETECT_CURSE,	

  SP_HEROISM,		SP_AGGRAVATION,		SP_FIREBOLT,		/*80*/
  SP_FROSTBOLT,		SP_SHOCKWAVE,

  SP_COLOR_SPRAY,	SP_HASTE,		SP_FACE_OF_DEATH,	/*85*/
  SP_BALL_LIGHTNING,	SP_METEOR_SWARM,

  SP_METEOR,		SP_MYSTIC_FIST,		SP_RAISE_DEAD,		/*90*/
  SP_RESURRECTION,	SP_REINCARNATION,     

/* mlee's spells*/	

  SP_IMMUNE_COLD,	SP_IMMUNE_ELEC,		SP_IMMUNE_FIRE,		/*95*/
  SP_IMMUNE_POISON,	SP_IMMUNE_SLOW,


  SP_IMMUNE_PARALYZE,	SP_IMMUNE_DRAIN,	SP_IMMUNE_MAGIC,	/*100*/
  SP_IMMUNE_ATTACK,	SP_INVULNERABILITY,

  SP_PROTECTION,							/*105*/
        /*Some more new spells by peterm */
			SP_RUNE_FIRE,		SP_RUNE_FROST,		
  SP_RUNE_SHOCK,	SP_RUNE_BLAST,

  SP_RUNE_DEATH,	SP_RUNE_MARK,		SP_BUILD_DIRECTOR,	/*110*/
  SP_CHAOS_POOL,	SP_BUILD_BWALL,

  SP_BUILD_LWALL,	SP_BUILD_FWALL,		SP_RUNE_MAGIC,		/*115*/
  SP_RUNE_DRAINSP,	SP_RUNE_ANTIMAGIC,

  SP_RUNE_TRANSFER,	SP_TRANSFER,		SP_MAGIC_DRAIN,		/*120*/
  SP_COUNTER_SPELL,	SP_DISPEL_RUNE,

  SP_CURE_CONFUSION,	SP_RESTORATION,		SP_SUMMON_EVIL_MONST,	/*125*/
  SP_COUNTERWALL,	SP_CAUSE_LIGHT,

  SP_CAUSE_MEDIUM,	SP_CAUSE_HEAVY,		SP_CHARM,		/*130*/
  SP_BANISHMENT,	SP_CREATE_MISSILE,

  SP_SHOW_INVIS,	SP_XRAY,		SP_PACIFY,		/*135*/
  SP_SUMMON_FOG,	SP_STEAMBOLT,

  /* lots of new cleric spells,many need MULTIPLE_GODS defined to be
   * very usefull - b.t. */
  SP_COMMAND_UNDEAD,	SP_HOLY_ORB,		SP_SUMMON_AVATAR,	/*140*/
  SP_HOLY_POSSESSION,	SP_BLESS,		

  SP_CURSE,		SP_REGENERATION,	SP_CONSECRATE,		/*145*/
  SP_SUMMON_CULT,	SP_CAUSE_CRITICAL, 

  SP_HOLY_WRATH,	SP_RETRIBUTION,		SP_FINGER_DEATH,	/*150*/
  SP_INSECT_PLAGUE,	SP_HOLY_SERVANT,
  
  SP_WALL_OF_THORNS,	SP_STAFF_TO_SNAKE,	SP_LIGHT,		/*155*/
  SP_DARKNESS,		SP_NIGHTFALL,

  SP_DAYLIGHT,		SP_SUNSPEAR,		SP_FAERY_FIRE,		/*160*/
  SP_CURE_BLINDNESS,	SP_DARK_VISION,

  SP_BULLET_SWARM,	SP_BULLET_STORM,	SP_CAUSE_MANY,		/*165*/
  SP_S_SNOWSTORM,	SP_M_SNOWSTORM,

  SP_L_SNOWSTORM,	SP_CURE_DISEASE,	SP_CAUSE_EBOLA,		/*170*/
  SP_CAUSE_FLU,		SP_CAUSE_PLAGUE,

  SP_CAUSE_LEPROSY,	SP_CAUSE_SMALLPOX,	SP_CAUSE_PNEUMONIC_PLAGUE,/*175*/
  SP_CAUSE_ANTHRAX,	SP_CAUSE_TYPHOID,

  SP_MANA_BLAST,	SP_S_MANABALL,		SP_M_MANABALL,		/*180*/
  SP_L_MANABALL,	SP_MANA_BOLT,

  SP_DANCING_SWORD,	SP_ANIMATE_WEAPON
};
	
extern spell_parameters SP_PARAMETERS[];
extern char *spellpathnames[NRSPELLPATHS];
extern archetype *spellarch[NROFREALSPELLS];

typedef enum SpellTypeFrom {
  spellNormal, spellWand, spellRod, spellHorn, spellScroll, spellPotion
} SpellTypeFrom;


#endif

#endif

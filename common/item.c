/*
 * static char *rcsid_item_c =
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

#include <global.h>
#include <funcpoint.h>
#include <living.h>
#include <spells.h>

/* the ordering of this is actually doesn't make a difference
 * However, for ease of use, new entries should go at the end
 * so those people that debug the code that get used to something
 * being in the location 4 don't get confused.
 *
 * The ordering in save_name, use_name, nonuse_name.
 * save_name is the name used to load/save it from files.  It should
 * match that of the doc/Developers/objects.  The only
 * real limitation is that it shouldn't have spaces or other characters
 * that may mess up the match code.  It must also start with body_
 * use_name is how we describe the location if we can use it.
 * nonuse_name is how we describe it if we can't use it.  I think
 * the values below will make it pretty clear how those work out
 * They are basically there to make life a little easier - if a character
 * examines an item and it says it goes on 'your arm', its pretty clear
 * they can use it.  See the last sample (commented out) for a dragon
 * Note that using the term 'human' may not be very accurate, humanoid
 * may be better.
 * Basically, for the use/nonuse, the code does something like:
 * "This item goes %s\n", with the use/nonuse values filling in the %s
 */
Body_Locations body_locations[NUM_BODY_LOCATIONS] = {
{"body_range",	"in your range slot",	"in a human's range slot"},
{"body_arm",	"on your arm",		"on a human's arm"},
{"body_torso",	"on your body",		"on a human's torso"},
{"body_head",	"on your head",		"on a human's head"},
{"body_neck",	"around your neck",	"around a humans neck"},
{"body_skill",	"in your skill slot",	"in a human's skill slot"},
{"body_finger",	"on your finger",	"on a human's finger"},
{"body_shoulder", "around your shoulders",  "around a human's shoulders"},
{"body_foot",	"on your feet",		"on a human's feet"},
{"body_hand",	"on your hands",	"on a human's hands"},
{"body_wrist",	"around your wrists",	"around a human's wrist"},
{"body_waist",	"around your waist",	"around a human's waist"},

/*{"body_dragon_torso", "your body", "a dragon's body"} */
};

static char numbers[21][20] = {
  "no","","two","three","four","five","six","seven","eight","nine","ten",
  "eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen",
  "eighteen","nineteen","twenty"
};

static char numbers_10[10][20] = {
  "zero","ten","twenty","thirty","fourty","fifty","sixty","seventy",
  "eighty","ninety"
};

static char levelnumbers[21][20] = {
  "zeroth","first", "second", "third", "fourth", "fifth", "sixth", "seventh",
  "eighth", "ninth", "tenth", "eleventh", "twelfth", "thirteenth",
  "fourteenth", "fifteenth", "sixteenth", "seventeenth", "eighteen",
  "nineteen", "twentieth"
};

static char levelnumbers_10[11][20] = {
  "zeroth","tenth","twentieth","thirtieth","fortieth","fiftieth","sixtieth",
  "seventieth","eightieth","ninetieth"
};

/*
materialtype material[NROFMATERIALS] = {
  *  		  P  M  F  E  C  C  A  D  W  G  P S P T F  C D D C C G H B  I *
   *		  H  A  I  L  O  O  C  R  E  H  O L A U E  A E E H O O O L  N *
   *		  Y  G  R  E  L  N  I  A  A  O  I O R R A  N P A A U D L I  T *
   *		  S  I  E  C  D  F  D  I  P  S  S W A N R  C L T O N   Y N  R *
   *		  I  C     T     U     N  O  T  O   L      E E H S T P   D  N *
  {"paper", 	{15,10,17, 9, 5, 7,13, 0,20,15, 0,0,0,0,0,10,0,0,0,0,0,0,0,0}},
  {"metal", 	{ 2,12, 3,12, 2,10, 7, 0,20,15, 0,0,0,0,0,10,0,0,0,0,0,0,0,0}},
  {"glass", 	{14,11, 8, 3,10, 5, 1, 0,20,15, 0,0,0,0,0, 0,0,0,0,0,0,0,0,0}},
  {"leather", 	{ 5,10,10, 3, 3,10,10, 0,20,15, 0,0,0,0,0,12,0,0,0,0,0,0,0,0}},
  {"wood", 	{10,11,13, 2, 2,10, 9, 0,20,15, 0,0,0,0,0,12,0,0,0,0,0,0,0,0}},
  {"organics", 	{ 3,12, 9,11, 3,10, 9, 0,20,15, 0,0,0,0,0, 0,0,0,0,0,0,0,0,0}},
  {"stone", 	{ 2, 5, 2, 2, 2, 2, 1, 0,20,15, 0,0,0,0,0, 5,0,0,0,0,0,0,0,0}},
  {"cloth", 	{14,11,13, 4, 4, 5,10, 0,20,15, 0,0,0,0,0, 5,0,0,0,0,0,0,0,0}},
  {"adamant", 	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0,0, 0,0,0,0,0,0,0,0,0}},
  {"liquid", 	{ 0, 8, 9, 6,17, 0,15, 0,20,15,12,0,0,0,0,11,0,0,0,0,0,0,0,0}},
  {"soft metal",{ 6,12, 6,14, 2,10, 1, 0,20,15, 0,0,0,0,0,10,0,0,0,0,0,0,0,0}},
  {"bone", 	{10, 9, 4, 5, 3,10,10, 0,20,15, 0,0,0,0,0, 2,0,0,0,0,0,0,0,0}},
  {"ice", 	{14,11,16, 5, 0, 5, 6, 0,20,15, 0,0,0,0,0, 7,0,0,0,0,0,0,0,0}}
};
*/

/* This curve may be too steep.  But the point is that there should
 * be tough choices - there is no real point to this if everyone can
 * wear whatever they want with no worries.  Perhaps having the steep
 * curve is good (maybe even steeper), but allowing players to
 * have 2 * level instead.  Ideally, top level characters should only be
 * able to use 2-3 of the most powerful items.
 * note that this table is only really used for program generated items -
 * custom objects can use whatever they want.
 */
static int enc_to_item_power[21] = {
0, 0, 1, 2, 3, 4,    /* 5 */
5, 7, 9, 11, 13,    /* 10 */
15, 18, 21, 24, 27, /* 15 */
30, 35, 40, 45, 50  /* 20 */
};

int get_power_from_ench(int ench)
{
    if (ench < 0) ench = 0;
    if (ench > 20) ench = 20;
    return enc_to_item_power[ench];
}

/* This takes an object 'op' and figures out what its item_power
 * rating should be.  This should only really be used by the treasure
 * generation code, and when loading legacy objects.  It returns
 * the item_power it calculates.
 * If flag is 1, we return the number of enchantment, and not the
 * the power.  This is used in the treasure code.
 */
int calc_item_power(object *op, int flag)
{
    int i, tmp, enc;

    enc = 0;
    for (i=0; i<NUM_STATS; i++)
	enc += get_attr_value(&op->stats, i);

    /* This protection logic is pretty flawed.  20% fire resistance
     * is much more valuable than 20% confusion, or 20% slow, or
     * several others.  Start at 1 - ignore physical - all that normal
     * armour shouldn't be counted against 
     */
    tmp = 0;
    for (i=1; i<NROFATTACKS; i++)
	tmp += op->resist[i];

    /* Add/substract 10 so that the rounding works out right */
    if (tmp>0) enc += (tmp+10)/20;
    else if (tmp<0) enc += (tmp - 10) / 20;

    enc += op->magic;

    /* For each attacktype a weapon has, one more encantment.  Start at 1 -
     * physical doesn't count against total.
     */
    if (op->type == WEAPON) {
	for (i=1; i<NROFATTACKS; i++)
	    if (op->attacktype & (1 << i)) enc++;
	if (op->slaying) enc += 2;	    /* What it slays is probably more relevent */
    }
    /* Items the player can equip */
    if ((op->type == WEAPON) || (op->type == ARMOUR)   || (op->type == HELMET) ||
        (op->type == SHIELD)   || (op->type == RING) ||
        (op->type == BOOTS)    || (op->type == GLOVES) ||
        (op->type == AMULET )  || (op->type == GIRDLE) ||
        (op->type == BRACERS ) || (op->type == CLOAK)) {
	enc += op->stats.food;	    /* sustenance */
	enc += op->stats.hp;	    /* hp regen */
	enc += op->stats.sp;	    /* mana regen */
	enc += op->stats.grace;	    /* grace regen */
	enc += op->stats.exp;	    /* speed bonus */
    }
    enc += op->stats.luck;

    /* Do spell paths now */
    for (i=1; i<NRSPELLPATHS; i++) {
	if (op->path_attuned& (1 << i)) enc++;
	else if (op->path_denied & (1 << i)) enc-=2;
	else if (op->path_repelled & (1 << i)) enc--;
    }

    if(QUERY_FLAG(op,FLAG_LIFESAVE))	    enc += 5;
    if(QUERY_FLAG(op,FLAG_REFL_SPELL))	    enc += 3;
    if(QUERY_FLAG(op,FLAG_REFL_MISSILE))    enc += 2;
    if(QUERY_FLAG(op,FLAG_STEALTH))	    enc += 1;
    if(QUERY_FLAG(op,FLAG_XRAYS))	    enc += 2;
    if(QUERY_FLAG(op,FLAG_SEE_IN_DARK))	    enc += 1;
    if(QUERY_FLAG(op,FLAG_MAKE_INVIS))	    enc += 1;

    return get_power_from_ench(enc);

}

/* describe_resistance generates the visible naming for resistances.
 * returns a static array of the description.  This can return
 * a big buffer.
 * if newline is true, we don't put parens around the description
 * but do put a newline at the end.  Useful when dumping to files
 */
char *describe_resistance(object *op, int newline)
{
    static char buf[VERY_BIG_BUF];
    char    buf1[VERY_BIG_BUF];
    int tmpvar;

    buf[0]=0;
    for (tmpvar=0; tmpvar<NROFATTACKS; tmpvar++) {
       if (op->resist[tmpvar] && (op->type != FLESH || atnr_is_dragon_enabled(tmpvar)==1)) {
	    if (!newline)
		sprintf(buf1,"(%s %+d)", resist_plus[tmpvar], op->resist[tmpvar]);
	    else
		sprintf(buf1,"%s %d\n", resist_plus[tmpvar], op->resist[tmpvar]);

	    strcat(buf, buf1);
       }
    }
    return buf;
}


/*
 * query_weight(object) returns a character pointer to a static buffer
 * containing the text-representation of the weight of the given object.
 * The buffer will be overwritten by the next call to query_weight().
 */

char *query_weight(object *op) {
  static char buf[10];
  int i=op->nrof?op->nrof*op->weight:op->weight+op->carrying;

  if(op->weight<0)
    return "      ";
  if(i%1000)
    sprintf(buf,"%6.1f",i/1000.0);
  else
    sprintf(buf,"%4d  ",i/1000);
  return buf;
}

/*
 * Returns the pointer to a static buffer containing
 * the number requested (of the form first, second, third...)
 */

char *get_levelnumber(int i) {
  static char buf[MAX_BUF];
  if (i > 99) {
    sprintf(buf, "%d.", i);
    return buf;
  }
  if(i < 21)
    return levelnumbers[i];
  if(!(i%10))
    return levelnumbers_10[i/10];
  strcpy(buf, numbers_10[i/10]);
  strcat(buf, levelnumbers[i%10]);
  return buf;
}


/*
 * get_number(integer) returns the text-representation of the given number
 * in a static buffer.  The buffer might be overwritten at the next
 * call to get_number().
 * It is currently only used by the query_name() function.
 */

char *get_number(int i) {
  if(i<=20)
    return numbers[i];
  else {
    static char buf[MAX_BUF];
    sprintf(buf,"%d",i);
    return buf;
  }
}

/*
 *  Returns pointer to static buffer containing ring's or amulet's
 *  abilities
 *  These are taken from old query_name(), but it would work better
 *  if describle_item() would be called to get this information and
 *  caller would handle FULL_RING_DESCRIPTION definition.
 *  Or make FULL_RING_DESCRIPTION standard part of a game and let
 *  client handle names.
 */
/* Aug 95 modified this slightly so that Skill tools don't have magic bonus
 * from stats.sp - b.t.
 */
char *ring_desc (object *op) 
{
    static char buf[VERY_BIG_BUF];
    int attr, val,len;
    
    buf[0] = 0;

    if (! QUERY_FLAG(op, FLAG_IDENTIFIED))
	return buf;

    for (attr=0; attr<7; attr++) {
	if ((val=get_attr_value(&(op->stats),attr))!=0) {
	    sprintf (buf+strlen(buf), "(%s%+d)", short_stat_name[attr], val);
	}
    }
    if(op->stats.exp)
	sprintf(buf+strlen(buf), "(speed %+lld)", op->stats.exp);
    if(op->stats.wc)
	sprintf(buf+strlen(buf), "(wc%+d)", op->stats.wc);
    if(op->stats.dam)
	sprintf(buf+strlen(buf), "(dam%+d)", op->stats.dam);
    if(op->stats.ac)
	sprintf(buf+strlen(buf), "(ac%+d)", op->stats.ac);

    strcat(buf,describe_resistance(op, 0));

    if (op->stats.food != 0)
	sprintf(buf+strlen(buf), "(sustenance%+d)", op->stats.food);
	 /*    else if (op->stats.food < 0)
			 sprintf(buf+strlen(buf), "(hunger%+d)", op->stats.food); */
    if(op->stats.grace)
	sprintf(buf+strlen(buf), "(grace%+d)", op->stats.grace);
    if(op->stats.sp && op->type!=SKILL)
	sprintf(buf+strlen(buf), "(magic%+d)", op->stats.sp);
    if(op->stats.hp)
	sprintf(buf+strlen(buf), "(regeneration%+d)", op->stats.hp);
    if(op->stats.luck)
	sprintf(buf+strlen(buf), "(luck%+d)", op->stats.luck);
    if(QUERY_FLAG(op,FLAG_LIFESAVE))
	strcat(buf,"(lifesaving)");
    if(QUERY_FLAG(op,FLAG_REFL_SPELL))
	strcat(buf,"(reflect spells)");
    if(QUERY_FLAG(op,FLAG_REFL_MISSILE))
	strcat(buf,"(reflect missiles)");
    if(QUERY_FLAG(op,FLAG_STEALTH))
	strcat(buf,"(stealth)");
    /* Shorten some of the names, so they appear better in the windows */
    len=strlen(buf);
    DESCRIBE_PATH_SAFE(buf, op->path_attuned, "Attuned", &len, VERY_BIG_BUF);
    DESCRIBE_PATH_SAFE(buf, op->path_repelled, "Repelled", &len, VERY_BIG_BUF);
    DESCRIBE_PATH_SAFE(buf, op->path_denied, "Denied", &len, VERY_BIG_BUF);

    /*    if(op->item_power)
	sprintf(buf+strlen(buf), "(item_power %+d)",op->item_power);
    */
    if(buf[0] == 0 && op->type!=SKILL)
	strcpy(buf,"of adornment");


    return buf;
}

/*
 * query_short_name(object) is similar to query_name, but doesn't 
 * contain any information about object status (worn/cursed/etc.)
 */
char *query_short_name(object *op) 
{
    static char buf[HUGE_BUF];
    char buf2[HUGE_BUF];
    int len=0;

    if(op->name == NULL)
	return "(null)";
    if(!op->nrof && !op->weight && !op->title && !is_magical(op)) 
	return op->name; /* To speed things up (or make things slower?) */

    if (op->nrof <= 1)
	safe_strcat(buf,op->name, &len, HUGE_BUF);
    else
	safe_strcat(buf,op->name_pl, &len, HUGE_BUF);

    if (op->title && QUERY_FLAG(op,FLAG_IDENTIFIED)) {
	safe_strcat(buf, " ", &len, HUGE_BUF);
	safe_strcat(buf, op->title, &len, HUGE_BUF);
    }

    switch(op->type) {
	case SPELLBOOK:
	case SCROLL:
	case WAND:
	case ROD:
	    if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
		if(!op->title) {
		    safe_strcat(buf," of ", &len, HUGE_BUF);
		    if (op->inv) 
			safe_strcat(buf,op->inv->name, &len, HUGE_BUF);
		    else
			LOG(llevError,"Spellbook %s lacks inventory\n", op->name);
		}
		if(op->type != SPELLBOOK) {
		    sprintf(buf2, " (lvl %d)", op->level);
		    safe_strcat(buf, buf2, &len, HUGE_BUF);
		}
	    }
	    break;

      case SKILL:
      case AMULET:
      case RING:
	if (!op->title) {
	    /* If ring has a title, full description isn't so useful */ 
	    char *s = ring_desc(op);
	    if (s[0]) {
		safe_strcat (buf, " ", &len, HUGE_BUF);
		safe_strcat(buf, s, &len, HUGE_BUF);
	    }
	}
	break;
      default:
	if(op->magic && ((QUERY_FLAG(op,FLAG_BEEN_APPLIED) && 
	   need_identify(op)) || QUERY_FLAG(op,FLAG_IDENTIFIED))) {
	    sprintf(buf2, " %+d", op->magic);
	    safe_strcat(buf, buf2, &len, HUGE_BUF);
	}
    }
    return buf;
}

/*
 * query_name(object) returns a character pointer pointing to a static
 * buffer which contains a verbose textual representation of the name
 * of the given object.
 * cf 0.92.6:  Put in 5 buffers that it will cycle through.  In this way,
 * you can make several calls to query_name before the bufs start getting
 * overwritten.  This may be a bad thing (it may be easier to assume the value
 * returned is good forever.)  However, it makes printing statements that
 * use several names much easier (don't need to store them to temp variables.)
 *
 */
char *query_name(object *op) {
    static char buf[5][HUGE_BUF];
    static int use_buf=0;
    int len=0;
#ifdef NEW_MATERIAL_CODE
    materialtype_t *mt;
#endif

    use_buf++;
    use_buf %=5;

#ifdef NEW_MATERIAL_CODE
    if ((IS_ARMOR(op) || IS_WEAPON(op)) && op->materialname) {
	mt = name_to_material(op->materialname);
	if (mt) {
	    safe_strcat(buf[use_buf], mt->description, &len, HUGE_BUF);
	    safe_strcat(buf[use_buf], " ", &len, HUGE_BUF);
	}
    }
#endif

    safe_strcat(buf[use_buf], query_short_name(op), &len, HUGE_BUF);

    if (QUERY_FLAG(op,FLAG_INV_LOCKED))
	safe_strcat(buf[use_buf], " *", &len, HUGE_BUF);
    if (op->type == CONTAINER && ((op->env && op->env->container == op) || 
	(!op->env && QUERY_FLAG(op,FLAG_APPLIED))))
	safe_strcat(buf[use_buf]," (open)", &len, HUGE_BUF);

    if (QUERY_FLAG(op,FLAG_KNOWN_CURSED)) {
	if(QUERY_FLAG(op,FLAG_DAMNED))
	    safe_strcat(buf[use_buf], " (damned)", &len, HUGE_BUF);
	else if(QUERY_FLAG(op,FLAG_CURSED))
	    safe_strcat(buf[use_buf], " (cursed)", &len, HUGE_BUF);
    }
    /* Basically, if the object is known magical (detect magic spell on it),
     * and it isn't identified,  print out the fact that
     * it is magical.  Assume that the detect magical spell will only set
     * KNOWN_MAGICAL if the item actually is magical.
     *
     * Changed in V 0.91.4 - still print that the object is magical even
     * if it has been applied.  Equipping an item does not tell full
     * abilities, especially for artifact items.
     */
    if (QUERY_FLAG(op,FLAG_KNOWN_MAGICAL) && !QUERY_FLAG(op,FLAG_IDENTIFIED))
	safe_strcat(buf[use_buf], " (magic)", &len, HUGE_BUF);

#if 0
    /* item_power will be returned in desribe_item - it shouldn't really
     * be returned in the name.
     */
    if(op->item_power)
	sprintf(buf[use_buf]+strlen(buf[use_buf]), "(item_power %+d)",
	    op->item_power);

#endif

    if (QUERY_FLAG(op,FLAG_APPLIED)) {
	switch(op->type) {
	  case BOW:
	  case WAND:
	  case ROD:
	  case HORN:
	    safe_strcat(buf[use_buf]," (readied)", &len, HUGE_BUF);
	    break;
	  case WEAPON:
	    safe_strcat(buf[use_buf]," (wielded)", &len, HUGE_BUF);
	    break;
	  case ARMOUR:
	  case HELMET:
	  case SHIELD:
	  case RING:
	  case BOOTS:
	  case GLOVES:
	  case AMULET:
	  case GIRDLE:
	  case BRACERS:
	  case CLOAK:
	    safe_strcat(buf[use_buf]," (worn)", &len, HUGE_BUF);
	    break;
	  case CONTAINER:
	    safe_strcat(buf[use_buf]," (active)", &len, HUGE_BUF);
	    break;
	  case SKILL:
	  default:
	    safe_strcat(buf[use_buf]," (applied)", &len, HUGE_BUF);
	}
    }
    if(QUERY_FLAG(op, FLAG_UNPAID))
	safe_strcat(buf[use_buf]," (unpaid)", &len, HUGE_BUF);

    return buf[use_buf];
}

/*
 * query_base_name(object) returns a character pointer pointing to a static
 * buffer which contains a verbose textual representation of the name
 * of the given object.  The buffer will be overwritten at the next
 * call to query_base_name().   This is a lot like query_name, but we
 * don't include the item count or item status.  Used for inventory sorting
 * and sending to client.
 * If plural is set, we generate the plural name of this.
 */
char *query_base_name(object *op, int plural) {
    static char buf[MAX_BUF], buf2[MAX_BUF];
    int len;
    materialtype_t *mt;

    if((!plural && !op->name) || (plural && !op->name_pl))
	return "(null)";

    if(!op->nrof && !op->weight && !op->title && !is_magical(op)) 
	return op->name; /* To speed things up (or make things slower?) */

    if ((IS_ARMOR(op) || IS_WEAPON(op)) && op->materialname)
	mt = name_to_material(op->materialname);

#ifdef NEW_MATERIAL_CODE
    if ((IS_ARMOR(op) || IS_WEAPON(op)) && op->materialname && mt &&
	op->arch->clone.materialname != mt->name &&
	!(op->material & M_SPECIAL)) {
	strcpy(buf, mt->description);
	len=strlen(buf);
	safe_strcat(buf, " ", &len, MAX_BUF);
	if (!plural)
	    safe_strcat(buf, op->name, &len, MAX_BUF);
	else
	    safe_strcat(buf, op->name_pl, &len, MAX_BUF);
    } else {
#endif
	if (!plural)
	    strcpy(buf, op->name);
	else
	    strcpy(buf, op->name_pl);
	len=strlen(buf);
#ifdef NEW_MATERIAL_CODE
    }
#endif

    if (op->title && QUERY_FLAG(op,FLAG_IDENTIFIED)) {
	safe_strcat(buf, " ", &len, MAX_BUF);
	safe_strcat(buf, op->title, &len, MAX_BUF);
    }

    switch(op->type) {
	case SPELLBOOK:
	case SCROLL:
	case WAND:
	case ROD:
	    if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
		if(!op->title) {
		    safe_strcat(buf," of ", &len, MAX_BUF);
		    if (op->inv) 
			safe_strcat(buf,op->inv->name, &len, MAX_BUF);
		    else
			LOG(llevError,"Spellbook %s lacks inventory\n", op->name);
		}
		if(op->type != SPELLBOOK) {
		    sprintf(buf2, " (lvl %d)", op->level);
		    safe_strcat(buf, buf2, &len, MAX_BUF);
		}
	    }
	    break;


      case SKILL:
      case AMULET:
      case RING:
	if (!op->title) {
	    /* If ring has a title, full description isn't so useful */ 
	    char *s = ring_desc(op);
	    if (s[0]) {
		safe_strcat (buf, " ", &len, MAX_BUF);
		safe_strcat (buf, s, &len, MAX_BUF);
	    }
	}
	break;
      default:
	if(op->magic && ((QUERY_FLAG(op,FLAG_BEEN_APPLIED) && 
	   need_identify(op)) || QUERY_FLAG(op,FLAG_IDENTIFIED))) {
	    sprintf(buf + strlen(buf), " %+d", op->magic);
	}
    }
    return buf;
}

/* Break this off from describe_item - that function was way
 * too long, making it difficult to read.  This function deals
 * with describing the monsters & players abilities.  It should only
 * be called with monster & player objects.  Returns a description
 * in a static buffer.
 */

static char *describe_monster(object *op) {
    char buf[MAX_BUF];
    static char retbuf[VERY_BIG_BUF];
    int i;

    retbuf[0]='\0';

    /* Note that the resolution this provides for players really isn't
     * very good.  Any player with a speed greater than .67 will
     * fall into the 'lightning fast movement' category.
     */
    if(FABS(op->speed)>MIN_ACTIVE_SPEED) {
	switch((int)((FABS(op->speed))*15)) {
	    case 0:
		strcat(retbuf,"(very slow movement)");
		break;
	    case 1:
		strcat(retbuf,"(slow movement)");
		break;
	    case 2:
		strcat(retbuf,"(normal movement)");
		break;
	    case 3:
	    case 4:
		strcat(retbuf,"(fast movement)");
		break;
	    case 5:
	    case 6:
		strcat(retbuf,"(very fast movement)");
		break;
	    case 7:
	    case 8:
	    case 9:
	    case 10:
		strcat(retbuf,"(extremely fast movement)");
		break;
	    default:
		strcat(retbuf,"(lightning fast movement)");
		break;
	}
    }
    if(QUERY_FLAG(op,FLAG_UNDEAD))
	strcat(retbuf,"(undead)");
    if(QUERY_FLAG(op,FLAG_CAN_PASS_THRU))
	strcat(retbuf,"(pass through doors)");
    if(QUERY_FLAG(op,FLAG_SEE_INVISIBLE))
	strcat(retbuf,"(see invisible)");
    if(QUERY_FLAG(op,FLAG_USE_WEAPON))
	strcat(retbuf,"(wield weapon)");
    if(QUERY_FLAG(op,FLAG_USE_BOW))
	strcat(retbuf,"(archer)");
    if(QUERY_FLAG(op,FLAG_USE_ARMOUR))
	strcat(retbuf,"(wear armour)");
    if(QUERY_FLAG(op,FLAG_USE_RING))
	strcat(retbuf,"(wear ring)");
    if(QUERY_FLAG(op,FLAG_USE_SCROLL))
	strcat(retbuf,"(read scroll)");
    if(QUERY_FLAG(op,FLAG_USE_RANGE))
	strcat(retbuf,"(fires wand/rod/horn)");
    if(QUERY_FLAG(op,FLAG_CAN_USE_SKILL))
	strcat(retbuf,"(skill user)");
    if(QUERY_FLAG(op,FLAG_CAST_SPELL))
	strcat(retbuf,"(spellcaster)");
    if(QUERY_FLAG(op,FLAG_FRIENDLY))
	strcat(retbuf,"(friendly)");
    if(QUERY_FLAG(op,FLAG_UNAGGRESSIVE))
	strcat(retbuf,"(unaggressive)");
    if(QUERY_FLAG(op,FLAG_HITBACK))
	strcat(retbuf,"(hitback)");
    if(QUERY_FLAG(op,FLAG_STEALTH))
	strcat(retbuf,"(stealthy)");
    if(op->randomitems != NULL) {
	treasure *t;
	int first = 1;
	for(t=op->randomitems->items; t != NULL; t=t->next)
	    if(t->item && (t->item->clone.type == SPELL)) {
		if(first) {
		    first = 0;
		    strcat(retbuf,"(Spell abilities:)");
		}
		strcat(retbuf,"(");
		strcat(retbuf,t->item->clone.name);
		strcat(retbuf,")");
	    }
    }
    if (op->type == PLAYER) {
	if(op->contr->digestion) {
	    if(op->contr->digestion!=0)
		sprintf(buf,"(sustenance%+d)",op->contr->digestion);
	    strcat(retbuf,buf);
	}
	if(op->contr->gen_grace) {
	    sprintf(buf,"(grace%+d)",op->contr->gen_grace);
	    strcat(retbuf,buf);
	}
	if(op->contr->gen_sp) {
	    sprintf(buf,"(magic%+d)",op->contr->gen_sp);
	    strcat(retbuf,buf);
	}
	if(op->contr->gen_hp) {
	    sprintf(buf,"(regeneration%+d)",op->contr->gen_hp);
	    strcat(retbuf,buf);
	}
	if(op->stats.luck) {
	    sprintf(buf,"(luck%+d)",op->stats.luck);
	    strcat(retbuf,buf);
	}
    }

    /* describe attacktypes */
    if (is_dragon_pl(op)) {
	/* for dragon players display the attacktypes from clawing skill
	 * Break apart the for loop - move the comparison checking down -
	 * this makes it more readable.
	 */
	object *tmp;
	
	for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
	    if (tmp->type == SKILL && !strcmp(tmp->name, "clawing")) break;

	if (tmp && tmp->attacktype!=0) {
		DESCRIBE_ABILITY(retbuf, tmp->attacktype, "Claws");
	}
	else {
		DESCRIBE_ABILITY(retbuf, op->attacktype, "Attacks");
	}
    } else {
	DESCRIBE_ABILITY(retbuf, op->attacktype, "Attacks");
    }
    DESCRIBE_PATH(retbuf, op->path_attuned, "Attuned");
    DESCRIBE_PATH(retbuf, op->path_repelled, "Repelled");
    DESCRIBE_PATH(retbuf, op->path_denied, "Denied");
    for (i=0; i < NROFATTACKS; i++) {
	if (op->resist[i]) {
	    sprintf(buf, "(%s %+d)", resist_plus[i], op->resist[i]);
	    strcat(retbuf, buf);
	}
    }
    return retbuf;
}


/*
 * Returns a pointer to a static buffer which contains a
 * description of the given object.
 * If it is a monster, lots of information about its abilities
 * will be returned.
 * If it is an item, lots of information about which abilities
 * will be gained about its user will be returned.
 * If it is a player, it writes out the current abilities
 * of the player, which is usually gained by the items applied.
 * It would be really handy to actually pass another object
 * pointer on who is examining this object.  Then, you could reveal
 * certain information depending on what the examiner knows, eg,
 * wouldn't need to use the SEE_INVISIBLE flag to know it is
 * a dragon player examining food.  Could have things like
 * a dwarven axe, in which the full abilities are only known to
 * dwarves, etc.
 *
 * This function is really much more complicated than it should
 * be, because different objects have different meanings
 * for the same field (eg, wands use 'food' for charges).  This
 * means these special cases need to be worked out.
 *
 * Add 'owner' who is the person examining this object.
 * owner can be null if no one is being associated with this
 * item (eg, debug dump or the like)
 */

char *describe_item(object *op, object *owner) {
    char buf[MAX_BUF];
    static char retbuf[VERY_BIG_BUF];
    int identified,i;

    retbuf[0]='\0';
    if(QUERY_FLAG(op,FLAG_MONSTER) || op->type==PLAYER) {
	return describe_monster(op);
    }
    /* figure this out once, instead of making multiple calls to need_identify.
     * also makes the code easier to read.
     */
    if (!need_identify(op) || QUERY_FLAG(op, FLAG_IDENTIFIED)) identified = 1;
    else {
	strcpy(retbuf,"(unidentified)");
	identified = 0;
    }

    switch(op->type) {
	case BOW:
	case ARROW:
	case WAND:
	case ROD:
	case HORN:
	case WEAPON:
	case ARMOUR:
	case HELMET:
	case SHIELD:
	case BOOTS:
	case GLOVES:
	case GIRDLE:
	case BRACERS:
	case CLOAK:
	case SKILL_TOOL:
	    break;  /* We have more information to do below this switch */

	case FOOD:
	case FLESH:
	case DRINK:
	    if(identified || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
		sprintf(buf,"(food+%d)", op->stats.food);
		strcat(retbuf, buf);
	
		if (op->type == FLESH && op->last_eat>0 && atnr_is_dragon_enabled(op->last_eat)) {
		    sprintf(buf, "(%s metabolism)", change_resist_msg[op->last_eat]);
		    strcat(retbuf, buf);
		}
	
		if (!QUERY_FLAG(op,FLAG_CURSED)) {
		    if (op->stats.hp)
			strcat(retbuf,"(heals)");
		    if (op->stats.sp)
			strcat(retbuf,"(spellpoint regen)");
		}
		else {
		    if (op->stats.hp)
			strcat(retbuf,"(damages)");
		    if (op->stats.sp)
			strcat(retbuf,"(spellpoint depletion)");
		}
	    }
	    break;


	case SKILL:
	case RING:
	case AMULET:
	    if(op->item_power) {
		sprintf(buf,"(item_power %+d)",op->item_power);
		strcat(retbuf,buf);
	    }
	    if (op->title)
		strcat (retbuf, ring_desc(op));
	    return retbuf;

	default:
	    return retbuf;
    }

    /* Down here, we more further describe equipment type items.
     * only describe them if they have been identified or the like.
     */
    if (identified || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
	int attr,val;

	for (attr=0; attr<7; attr++) {
	    if ((val=get_attr_value(&(op->stats),attr))!=0) {
		sprintf(buf, "(%s%+d)", short_stat_name[attr], val);
		strcat(retbuf,buf);
	    }
	}

	if(op->stats.exp) {
	    sprintf(buf,"(speed %+lld)",op->stats.exp);
	    strcat(retbuf,buf);
	}


	switch(op->type) {
	    case BOW:
	    case ARROW:
	    case GIRDLE:
	    case HELMET:
	    case SHIELD:
	    case BOOTS:
	    case GLOVES:
	    case WEAPON:
	    case SKILL:
	    case RING:
	    case AMULET:
	    case ARMOUR:
	    case BRACERS:
	    case FORCE:
	    case CLOAK:
		if(op->stats.wc) {
		    sprintf(buf,"(wc%+d)",op->stats.wc);
		    strcat(retbuf,buf);
		}
		if(op->stats.dam) {
		    sprintf(buf,"(dam%+d)",op->stats.dam);
		    strcat(retbuf,buf);
		}
		if(op->stats.ac) {
		    sprintf(buf,"(ac%+d)",op->stats.ac);
		    strcat(retbuf,buf);
		}
		if ((op->type==WEAPON || op->type == BOW) && op->level>0) {
		    sprintf(buf,"(improved %d/%d)",op->last_eat,op->level);
		    strcat(retbuf,buf);
		}
		break;

		default:
		    break;
	}
	if(QUERY_FLAG(op,FLAG_XRAYS))
	    strcat(retbuf,"(xray-vision)");
	if(QUERY_FLAG(op,FLAG_FLYING))
	    strcat(retbuf,"(levitate)");
	if(QUERY_FLAG(op,FLAG_SEE_IN_DARK))
	    strcat(retbuf,"(infravision)");

	if(op->item_power) {
	    sprintf(buf,"(item_power %+d)",op->item_power);
	    strcat(retbuf,buf);
	}
    } /* End if identified or applied */

    /* This blocks only deals with fully identified object.
     * it is intentional that this is not an 'else' from a above -
     * in this way, information is added.
      */
    if(identified) {
	int more_info = 0;

	switch(op->type) {
	    case ROD:  /* These use stats.sp for spell selection and stats.food */
	    case HORN: /* and stats.hp for spell-point regeneration... */
	    case BOW:
	    case ARROW:
	    case WAND:
	    case FOOD:
	    case FLESH:
	    case DRINK:
		more_info = 0;
		break;

	    /* Armor type objects */
	    case ARMOUR:
	    case HELMET:
	    case SHIELD:
	    case BOOTS:
	    case GLOVES:
	    case GIRDLE:
	    case BRACERS:
	    case CLOAK:
		if (ARMOUR_SPEED(op)) {
		    sprintf(buf,"(Max speed %1.2f)", ARMOUR_SPEED(op) / 10.0);
		    strcat(retbuf, buf);
		}
		if (ARMOUR_SPELLS(op)) {
		    sprintf(buf,"(Spell regen penalty %d)", ARMOUR_SPELLS(op));
		    strcat(retbuf, buf);
		}
		more_info=1;
		break;

	    case WEAPON:
		/* Calculate it the same way fix_player does so the results
		 * make sense.
		 */
		i = (WEAPON_SPEED(op)*2-op->magic)/2;
		if (i<0) i=0;

		sprintf(buf,"(weapon speed %d)", i);
		strcat(retbuf, buf);
		more_info=1;
		break;

	}
	if (more_info) {
	    if(op->stats.food) {
		if(op->stats.food!=0)
		    sprintf(buf,"(sustenance%+d)",op->stats.food);
		strcat(retbuf,buf);
	    }
	    if(op->stats.grace) {
		sprintf(buf,"(grace%+d)",op->stats.grace);
		strcat(retbuf,buf);
	    }
	    if(op->stats.sp) {
		sprintf(buf,"(magic%+d)",op->stats.sp);
		strcat(retbuf,buf);
	    }
	    if(op->stats.hp) {
		sprintf(buf,"(regeneration%+d)",op->stats.hp);
		strcat(retbuf,buf);
	    }
	}

	if(op->stats.luck) {
	    sprintf(buf,"(luck%+d)",op->stats.luck);
	    strcat(retbuf,buf);
	}
	if(QUERY_FLAG(op,FLAG_LIFESAVE))
	    strcat(retbuf,"(lifesaving)");
	if(QUERY_FLAG(op,FLAG_REFL_SPELL))
	    strcat(retbuf,"(reflect spells)");
	if(QUERY_FLAG(op,FLAG_REFL_MISSILE))
	    strcat(retbuf,"(reflect missiles)");
	if(QUERY_FLAG(op,FLAG_STEALTH))
	    strcat(retbuf,"(stealth)");
	if(op->slaying!=NULL) {
	    sprintf(buf,"(slay %s)",op->slaying);
	    strcat(retbuf,buf);
	}
	DESCRIBE_ABILITY(retbuf, op->attacktype, "Attacks");
	/* resistance on flesh is only visible for quetzals.  If
	 * non flesh, everyone can see its resistances
	 */
	if (op->type != FLESH || (owner && is_dragon_pl(owner)))
	    strcat(retbuf,describe_resistance(op, 0));
	DESCRIBE_PATH(retbuf, op->path_attuned, "Attuned");
	DESCRIBE_PATH(retbuf, op->path_repelled, "Repelled");
	DESCRIBE_PATH(retbuf, op->path_denied, "Denied");
    }

    return retbuf;
}

/* Return true if the item is magical.  A magical item is one that
 * increases/decreases any abilities, provides a resistance,
 * has a generic magical bonus, or is an artifact.
 * This function is used by detect_magic to determine if an item
 * should be marked as magical.
 */

int is_magical(object *op) {
    int i;

    /* living creatures are considered non magical */
    if (QUERY_FLAG(op, FLAG_ALIVE)) return 0;

    /* This is a test for it being an artifact, as artifacts have titles */
    if (op->title!=NULL) return 1;

    /* Handle rings and amulets specially.  If they change any of these
     * values, it means they are magical.
     */
    if ((op->type==AMULET || op->type==RING) && 
	(op->stats.ac || op->stats.food || op->stats.exp || op->stats.dam ||
	op->stats.wc || op->stats.sp || op->stats.hp || op->stats.luck))
		 return 1;

    /* Check for stealty, speed, flying, or just plain magic in the boots */
    if (op->type== BOOTS && 
	(QUERY_FLAG(op, FLAG_STEALTH) || QUERY_FLAG(op, FLAG_FLYING) ||
	 op->stats.exp))
		return 1;

    /* Take care of amulet/shield that reflects spells/missiles */
    if ((op->type==AMULET || op->type==SHIELD) && 
	(QUERY_FLAG(op, FLAG_REFL_SPELL) || QUERY_FLAG(op, FLAG_REFL_MISSILE)))
	    return 1;

    /* Take care of helmet of xrays */
    if (op->type==HELMET && QUERY_FLAG(op,FLAG_XRAYS)) return 1;

    /* Potions & rods are always magical.  Wands/staves are also magical,
     * assuming they still have any charges left.
     */
    if (op->type==POTION || op->type==ROD ||
	(op->type==WAND && op->stats.food))
	    return 1;

    /* if something gives a protection, either positive or negative, its magical */
    /* This is really a pretty bad hack - as of now, ATNR_PHYSICAL is 0,
     * so this always works out fine.
     */
    for (i=ATNR_PHYSICAL+1; i<NROFATTACKS; i++)
	if (op->resist[i]) return 1;

    /* Physical protection is expected on some item types, so they should
     * not be considered magical.
     */
    if (op->resist[ATNR_PHYSICAL] && op->type != HELMET && op->type != SHIELD &&
	op->type != BOOTS && op->type != GLOVES && op->type != ARMOUR) return 1;
	
   /* power crystal, spellbooks, and scrolls are always magical.  */
   if (op->magic || op->type==POWER_CRYSTAL || op->type==SPELLBOOK || 
	op->type==SCROLL || op->type==GIRDLE)
	    return 1;

    /* Check to see if it increases/decreases any stats */
    for (i=0; i<7; i++) 
	    if (get_attr_value(&(op->stats),i)!=0) return 1;

    /* If it doesn't fall into any of the above categories, must
     * be non magical.
     */
    return 0;
}

/* need_identify returns true if the item should be identified.  This 
 * function really should not exist - by default, any item not identified
 * should need it.
 */

int need_identify(object *op) {
    switch(op->type) {
	case RING:
	case WAND:
	case ROD:
	case HORN:
	case SCROLL:
	case SKILL:
	case SKILLSCROLL:
	case SPELLBOOK:
	case FOOD:
	case POTION:
	case BOW:
	case ARROW:
	case WEAPON:
	case ARMOUR:
	case SHIELD:
	case HELMET:
	case AMULET:
	case BOOTS:
	case GLOVES:
	case BRACERS:
	case GIRDLE:
	case CONTAINER:
	case DRINK:
	case FLESH:
	case INORGANIC:
	case CLOSE_CON:
	case CLOAK:
	case GEM:
	case POWER_CRYSTAL:
	case POISON:
	case BOOK:
	case SKILL_TOOL:
	    return 1;
    }
    /* Try to track down some stuff that may show up here.  Thus, the
     * archetype file can be updated, and this function removed.
     */
#if 0
    LOG(llevDebug,"need_identify: %s does not need to be id'd\n", op->name);
#endif
    return 0;
}


/*
 * Supposed to fix face-values as well here, but later.
 */

void identify(object *op) {
    object *pl;

    SET_FLAG(op,FLAG_IDENTIFIED);
    CLEAR_FLAG(op, FLAG_KNOWN_MAGICAL);
    CLEAR_FLAG(op, FLAG_NO_SKILL_IDENT);

    /*
     * We want autojoining of equal objects:
     */
    if (QUERY_FLAG(op,FLAG_CURSED) || QUERY_FLAG(op,FLAG_DAMNED))
	SET_FLAG(op,FLAG_KNOWN_CURSED);

    if (op->type == POTION) {
	if (op->inv && op->randomitems) {
	    if (op->title) free_string(op->title);
	    op->title = add_refcount(op->inv->name);
	} else if (op->arch) {
	    free_string(op->name);
	    op->name = add_refcount(op->arch->clone.name);
	    free_string(op->name_pl);
	    op->name_pl = add_refcount(op->arch->clone.name_pl);
	}
    }

    /* If the object is on a map, make sure we update its face */
    if (op->map)
	update_object(op,UP_OBJ_FACE);
    else {
	pl = is_player_inv(op->env);
	if (pl)
	    /* A lot of the values can change from an update - might as well send
	     * it all.
	     */
	    esrv_send_item_func(pl, op);
    }
}

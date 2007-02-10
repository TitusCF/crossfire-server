/*
 * static char *rcsid_item_c =
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

/**
 * @file common/item.c
 * Those functions deal with objects in general, including description, body location, and such.
 *
 * @todo
 * put const char* instead of char* when possible.
 */

#include <global.h>
#include <funcpoint.h>
#include <living.h>
#include <spells.h>

/**
 * The ordering of this is actually doesn't make a difference
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
body_locations_struct body_locations[NUM_BODY_LOCATIONS] = {
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

/** To write nice count instead of just the number. */
static const char numbers[21][20] = {
    "no","","two","three","four","five","six","seven","eight","nine","ten",
    "eleven","twelve","thirteen","fourteen","fifteen","sixteen","seventeen",
    "eighteen","nineteen","twenty"
};

/** Tens */
static const char numbers_10[10][20] = {
    "zero","ten","twenty","thirty","fourty","fifty","sixty","seventy",
    "eighty","ninety"
};

/** Levels as a full name and not a number. */
static const char levelnumbers[21][20] = {
    "zeroth","first", "second", "third", "fourth", "fifth", "sixth", "seventh",
    "eighth", "ninth", "tenth", "eleventh", "twelfth", "thirteenth",
    "fourteenth", "fifteenth", "sixteenth", "seventeenth", "eighteen",
    "nineteen", "twentieth"
};

/** Tens for levels */
static const char levelnumbers_10[11][20] = {
  "zeroth","tenth","twentieth","thirtieth","fortieth","fiftieth","sixtieth",
  "seventieth","eightieth","ninetieth"
};

/**
 * The following is a large table of item types, the fields are:
 * item number, item name, item name (plural), and two numbers that are the skills
 * used to identify them. Anytime a new item type is added or removed, this list
 * should be altered to reflect that. The defines for the numerical values are in
 * define.h
 */
static const typedata item_types[] = {
    {PLAYER, "player", "players", 0, 0},
    {ROD, "rod", "rods", SK_THAUMATURGY, 0},
    {TREASURE, "treasure", "treasure", 0, 0},
    {POTION, "potion", "potions", SK_ALCHEMY, 0},
    {FOOD, "food", "food", SK_WOODSMAN, 0},
    {POISON, "poison", "poisons", SK_ALCHEMY, 0},
    {BOOK, "book", "books", SK_LITERACY, 0},
    {CLOCK, "clock", "clocks", 0, 0},
    {LIGHTNING, "lightning", "lightning", 0, 0},
    {ARROW, "arrow", "arrows", SK_BOWYER, 0},
    {BOW, "bow", "bows", SK_BOWYER, 0},
    {WEAPON, "weapon", "weapons", SK_SMITHERY, 0},
    {ARMOUR, "armour", "armour", SK_SMITHERY, 0},
    {PEDESTAL, "pedestal", "pedestals", 0, 0},
    {ALTAR, "altar", "altars", 0, 0},
    {LOCKED_DOOR, "locked door", "locked doors", 0, 0},
    {SPECIAL_KEY, "special key", "special keys", 0, 0},
    {MAP, "map", "maps", 0, 0},
    {DOOR, "door", "doors", 0, 0},
    {KEY, "key", "keys", 0, 0},
    {TIMED_GATE, "timed_gate", "timed_gates", 0, 0},
    {TRIGGER, "trigger", "triggers", 0, 0},
    {GRIMREAPER, "grimreaper", "grimreapers", 0, 0},
    {MAGIC_EAR, "magic ear", "magic ears", 0, 0},
    {TRIGGER_BUTTON, "trigger button", "trigger buttons", 0, 0},
    {TRIGGER_ALTAR, "trigger altar", "trigger altars", 0, 0},
    {TRIGGER_PEDESTAL, "trigger pedestal", "trigger pedestals", 0, 0},
    {SHIELD, "shield", "shields", SK_SMITHERY, 0},
    {HELMET, "helmet", "helmets", SK_SMITHERY, 0},
    {HORN, "horn", "horns", SK_THAUMATURGY, 0},
    {MONEY, "money", "money", 0, 0},
    {CLASS, "class", "classes", 0, 0},
    {GRAVESTONE, "gravestone", "gravestones", 0, 0},
    {AMULET, "amulet", "amulets", SK_JEWELER, 0},
    {PLAYERMOVER, "player mover", "player movers", 0, 0},
    {TELEPORTER, "teleporter", "teleporters", 0, 0},
    {CREATOR, "creator", "creators", 0, 0},
    {SKILL, "skill", "skills", 0, 0},
    {EXPERIENCE, "experience", "experience", 0, 0},
    {EARTHWALL, "earthwall", "earthwalls", 0, 0},
    {GOLEM, "golem", "golems", 0, 0},
    {THROWN_OBJ, "projectile", "projectiles", 0, 0},
    {BLINDNESS, "blindness", "blindness", 0, 0},
    {GOD, "god", "gods", 0, 0},
    {DETECTOR, "detector", "detectors", 0, 0},
    {TRIGGER_MARKER, "trigger marker", "trigger markers", 0, 0},
    {DEAD_OBJECT, "dead object", "dead objects", 0, 0},
    {DRINK, "drink", "drinks", SK_WOODSMAN, SK_ALCHEMY},
    {MARKER, "marker", "markers", 0, 0},
    {HOLY_ALTAR, "holy altar", "holy altars", 0, 0},
    {PLAYER_CHANGER, "player changer", "player changers", 0, 0},
    {BATTLEGROUND, "battleground", "battlegrounds", 0, 0},
    {PEACEMAKER, "peacemaker", "peacemakers", 0, 0},
    {GEM, "gem", "gems", SK_JEWELER, 0},
    {FIREWALL, "firewall", "firewalls", 0, 0},
    {ANVIL, "anvil", "anvils", 0, 0},
    {CHECK_INV, "inventory checker", "inventory checkers", 0, 0},
    {MOOD_FLOOR, "mood floor", "mood floors", 0, 0},
    {EXIT, "exit", "exits", 0, 0},
    {ENCOUNTER, "encounter", "encounters", 0, 0},
    {SHOP_FLOOR, "shop floor", "shop floors", 0, 0},
    {SHOP_MAT, "shop mat", "shop mats", 0, 0},
    {RING, "ring", "rings", SK_JEWELER, 0},
    {FLOOR, "floor", "floors", 0, 0},
    {FLESH, "flesh", "flesh", SK_WOODSMAN, 0},
    {INORGANIC, "inorganic", "inorganics", SK_ALCHEMY, 0},
    {SKILL_TOOL, "skill tool", "skill tools", 0, 0},
    {LIGHTER, "lighter", "lighters", 0, 0},
    {TRAP_PART, "trap part", "trap parts", 0, 0},
    {WALL, "wall", "walls", 0, 0},
    {LIGHT_SOURCE, "light source", "light sources", 0, 0},
    {MISC_OBJECT, "bric-a-brac", "bric-a-brac", 0, 0},
    {MONSTER, "monster", "monsters", 0, 0},
    {SPAWN_GENERATOR, "monster generator", "monster generators", 0, 0},
    {LAMP, "lamp", "lamps", 0, 0},
    {DUPLICATOR, "duplicator", "duplicators", 0, 0},
    {TOOL, "tool", "tools", 0, 0},
    {SPELLBOOK, "spellbook", "spellbooks", SK_LITERACY, 0},
    {BUILDFAC, "building facility", "building facilities", 0, 0},
    {CLOAK, "cloak", "cloaks", SK_SMITHERY, 0},
    {SPINNER, "spinner", "spinners", 0, 0},
    {GATE, "gate", "gates", 0, 0},
    {BUTTON, "button", "buttons", 0, 0},
    {CF_HANDLE, "cf handle", "cf handles", 0, 0},
    {HOLE, "hole", "holes", 0, 0},
    {TRAPDOOR, "trapdoor", "trapdoors", 0, 0},
    {SIGN, "sign", "signs", 0, 0},
    {BOOTS, "boots", "boots", SK_SMITHERY, 0},
    {GLOVES, "gloves", "gloves", SK_SMITHERY, 0},
    {SPELL, "spell", "spells", 0, 0},
    {SPELL_EFFECT, "spell effect", "spell effects", 0, 0},
    {CONVERTER, "converter", "converters", 0, 0},
    {BRACERS, "bracers", "bracers", SK_SMITHERY, 0},
    {POISONING, "poisoning", "poisonings", 0, 0},
    {SAVEBED, "savebed", "savebeds", 0, 0},
    {POISONCLOUD, "poison cloud", "poison clouds", 0, 0},
    {FIREHOLES, "fireholes", "fireholes", 0, 0},
    {WAND, "wand", "wands", SK_THAUMATURGY, 0},
    {SCROLL, "scroll", "scrolls", SK_LITERACY, 0},
    {DIRECTOR, "director", "directors", 0, 0},
    {GIRDLE, "girdle", "girdles", SK_SMITHERY, 0},
    {FORCE, "force", "forces", 0, 0},
    {POTION_EFFECT, "potion effect", "potion effects", 0, 0},
    {CLOSE_CON, "closed container", "closed container", 0, 0},
    {CONTAINER, "container", "containers", SK_ALCHEMY, 0},
    {ARMOUR_IMPROVER, "armour improver", "armour improvers", 0, 0},
    {WEAPON_IMPROVER, "weapon improver", "weapon improvers", 0, 0},
    {SKILLSCROLL, "skillscroll", "skillscrolls", 0, 0},
    {DEEP_SWAMP, "deep swamp", "deep swamps", 0, 0},
    {IDENTIFY_ALTAR, "identify altar", "identify altars", 0, 0},
    {MENU, "inventory list", "inventory lists", 0, 0},
    {RUNE, "rune", "runes", 0, 0},
    {TRAP, "trap", "traps", 0, 0},
    {POWER_CRYSTAL, "power_crystal", "power_crystals", 0, 0},
    {CORPSE, "corpse", "corpses", 0, 0},
    {DISEASE, "disease", "diseases", 0, 0},
    {SYMPTOM, "symptom", "symptoms", 0, 0},
    {BUILDER, "item builder", "item builders", 0, 0},
    {MATERIAL, "building material", "building materials", 0, 0},
    {ITEM_TRANSFORMER, "item_transformer", "item_transformers", 0, 0},
};

/** Number of items in ::item_types array. */
const int item_types_size=sizeof(item_types)/sizeof(*item_types);

/** This curve may be too steep.  But the point is that there should
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
    if (ench < 0)  ench = 0;
    if (ench > 20) ench = 20;
    return enc_to_item_power[ench];
}

/**
 * This takes an object 'op' and figures out what its item_power
 * rating should be.  This should only really be used by the treasure
 * generation code, and when loading legacy objects.  It returns
 * the item_power it calculates.
 * If flag is 1, we return the number of enchantment, and not the
 * the power.  This is used in the treasure code.
 *
 * @param op
 * object of which to compute the item_power
 * @param flag
 * unused
 *
 * @todo
 * fix function, and remove unused flag variable.
 */
int calc_item_power(const object *op, int flag)
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

/**
 * @param itemtype
 * item type for which to return typedata.
 * @return
 * typedata that has a number equal to itemtype, if there
 * isn't one, returns NULL */
const typedata *get_typedata(int itemtype) {
    int i;
    for (i=0;i<item_types_size;i++)
        if (item_types[i].number==itemtype) return &item_types[i];
    return NULL;
}

/**
 * @param name
 * item name for which to return typedata. Singular form is preferred.
 * @return
 * typedata that has a name equal to itemtype, if there
 * isn't one, return the plural name that matches, if there still isn't
 * one return NULL
 *
 * @note
 * will emit an Info if found by plural form.
 */
const typedata *get_typedata_by_name(const char *name) {
    int i;
    for (i=0;i<item_types_size;i++)
        if (!strcmp(item_types[i].name, name)) return &item_types[i];
    for (i=0;i<item_types_size;i++)
        if (!strcmp(item_types[i].name_pl, name)) {
            LOG(llevInfo,
                "get_typedata_by_name: I have been sent the plural %s, the singular form %s is preffered\n",
                name, item_types[i].name);
            return &item_types[i];
        }
    return NULL;
}
/**
 * Generates the visible naming for resistances.
 *
 * @param op
 * object we want information about.
 * @param newline
 * If TRUE, we don't put parens around the description
 * but do put a newline at the end.  Useful when dumping to files
 * @param[out] buf
 * buffer that will receive the description.
 * @param size
 * buffer size.
 */
void describe_resistance(const object *op, int newline, char* buf, int size)
{
    char *p;
    int tmpvar;

    p = buf;
    for (tmpvar=0; tmpvar<NROFATTACKS; tmpvar++) {
        if (op->resist[tmpvar] && (op->type != FLESH || atnr_is_dragon_enabled(tmpvar)==1)) {
            if (!newline)
                snprintf(p, buf+size-p, "(%s %+d)", resist_plus[tmpvar], op->resist[tmpvar]);
            else
                snprintf(p, buf+size-p, "%s %d\n", resist_plus[tmpvar], op->resist[tmpvar]);
            p = strchr(p, '\0');
        }
    }
}

/**
 * Formats the item's weight.
 * @param op
 * object we want the weight of.
 * @param[out] buf
 * buffer to write to.
 * @param size
 * buffer size.
 */
void query_weight(const object *op, char* buf, int size) {
    sint32 i=(op->nrof?op->nrof:1)*op->weight+op->carrying;

    if(op->weight<0)
        snprintf(buf, size, "      ");
    else if(i%1000)
        snprintf(buf, size, "%6.1f",i/1000.0);
    else
        snprintf(buf, size, "%4d  ",i/1000);
}

/**
 * Formats a level.
 * @param i
 * level to format.
 * @param[out] buf
 * buffer which will contain the level.
 * @param size
 * size of the buffer.
 */
void get_levelnumber(int i, char* buf, int size) {
    if (i > 99 || i < 0) {
        snprintf(buf, size, "%d.", i);
        return;
    }
    if(i < 21) {
        snprintf(buf, size, "%s", levelnumbers[i]);
        return;
    }
    if(!(i%10)) {
        snprintf(buf, size, "%s", levelnumbers_10[i/10]);
        return;
    }
    snprintf(buf, size, "%s%s", numbers_10[i/10], levelnumbers[i%10]);
    return;
}

/**
 * Describes a ring or amulet.
 * @param op
 * item to describe.
 * @param buf
 * buffer that will contain the description.
 * @param size
 * size of buffer.
 *
 * @note
 * These are taken from old query_name(), but it would work better
 * if describle_item() would be called to get this information and
 * caller would handle FULL_RING_DESCRIPTION definition.
 * Or make FULL_RING_DESCRIPTION standard part of a game and let
 * client handle names.
 * @note
 * Aug 95 modified this slightly so that Skill tools don't have magic bonus
 * from stats.sp - b.t.
 *
 * @todo
 Use safe string functions. Check if really ring/amulet?
 */
static void ring_desc (const object *op, char* buf, int size)
{
    int attr, val,len;

    buf[0] = 0;

    if (! QUERY_FLAG(op, FLAG_IDENTIFIED))
        return;

    for (attr=0; attr<NUM_STATS; attr++) {
        if ((val=get_attr_value(&(op->stats),attr))!=0) {
            snprintf(buf+strlen(buf), size-strlen(buf), "(%s%+d)", short_stat_name[attr], val);
        }
    }
    if(op->stats.exp)
        snprintf(buf+strlen(buf), size-strlen(buf), "(speed %+" FMT64 ")", op->stats.exp);
    if(op->stats.wc)
        snprintf(buf+strlen(buf), size-strlen(buf), "(wc%+d)", op->stats.wc);
    if(op->stats.dam)
        snprintf(buf+strlen(buf), size-strlen(buf), "(dam%+d)", op->stats.dam);
    if(op->stats.ac)
        snprintf(buf+strlen(buf), size-strlen(buf), "(ac%+d)", op->stats.ac);

    describe_resistance(op, 0, buf+strlen(buf), size-strlen(buf));

    if (op->stats.food != 0)
        snprintf(buf+strlen(buf), size-strlen(buf), "(sustenance%+d)", op->stats.food);
    /* else if (op->stats.food < 0)
        snprintf(buf+strlen(buf), size-strlen(buf), "(hunger%+d)", op->stats.food); */
    if(op->stats.grace)
        snprintf(buf+strlen(buf), size-strlen(buf), "(grace%+d)", op->stats.grace);
    if(op->stats.sp && op->type!=SKILL)
        snprintf(buf+strlen(buf), size-strlen(buf), "(magic%+d)", op->stats.sp);
    if(op->stats.hp)
        snprintf(buf+strlen(buf), size-strlen(buf), "(regeneration%+d)", op->stats.hp);
    if(op->stats.luck)
        snprintf(buf+strlen(buf), size-strlen(buf), "(luck%+d)", op->stats.luck);
    if(QUERY_FLAG(op,FLAG_LIFESAVE))
        snprintf(buf+strlen(buf), size-strlen(buf), "(lifesaving)");
    if(QUERY_FLAG(op,FLAG_REFL_SPELL))
        snprintf(buf+strlen(buf), size-strlen(buf), "(reflect spells)");
    if(QUERY_FLAG(op,FLAG_REFL_MISSILE))
        snprintf(buf+strlen(buf), size-strlen(buf), "(reflect missiles)");
    if(QUERY_FLAG(op,FLAG_STEALTH))
        snprintf(buf+strlen(buf), size-strlen(buf), "(stealth)");
    /* Shorten some of the names, so they appear better in the windows */
    len=strlen(buf);
    DESCRIBE_PATH_SAFE(buf, op->path_attuned, "Attuned", &len, size);
    DESCRIBE_PATH_SAFE(buf, op->path_repelled, "Repelled", &len, size);
    DESCRIBE_PATH_SAFE(buf, op->path_denied, "Denied", &len, size);

    /*    if(op->item_power)
	sprintf(buf+strlen(buf), size-strlen(buf), "(item_power %+d)", op->item_power);
    */
    if(buf[0] == 0 && op->type!=SKILL)
        snprintf(buf, size, "of adornment");
}

/**
 * query_short_name(object) is similar to query_name(), but doesn't
 * contain any information about object status (worn/cursed/etc.)
 *
 * @param op
 * object to describe.
 * @param buf
 * buffer which will contain the name. Must not be NULL.
 * @param size
 * buffer length.
 */
void query_short_name(const object *op, char* buf, int size)
{
    int len=0;

    if(op->name == NULL) {
        snprintf(buf, size, "(null)");
        return;
    }
    if(!op->nrof && !op->weight && !op->title && !is_magical(op)) {
        snprintf(buf, size, "%s", op->name); /* To speed things up (or make things slower?) */
        return;
    }
    buf[0] = '\0';

    if (op->nrof <= 1)
        safe_strcat(buf, op->name, &len, size);
    else
        safe_strcat(buf, op->name_pl, &len, size);

    if (op->title && QUERY_FLAG(op,FLAG_IDENTIFIED)) {
        safe_strcat(buf, " ", &len, size);
        safe_strcat(buf, op->title, &len, size);
    }

    switch(op->type) {
        case SPELLBOOK:
        case SCROLL:
        case WAND:
        case ROD:
            if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
                if(!op->title) {
                    safe_strcat(buf," of ", &len, size);
                    if (op->inv)
                        safe_strcat(buf,op->inv->name, &len, size);
                    else
                        LOG(llevError,"Spellbook %s lacks inventory\n", op->name);
                }
                if(op->type != SPELLBOOK) {
                    snprintf(buf+len, size-len, " (lvl %d)", op->level);
                    len += strlen(buf+len);
                }
            }
            break;

        case SKILL:
        case AMULET:
        case RING:
            if (!op->title) {
                /* If ring has a title, full description isn't so useful */
                char desc[VERY_BIG_BUF];
                ring_desc(op, desc, VERY_BIG_BUF);
                if (desc[0]) {
                    safe_strcat(buf, " ", &len, size);
                    safe_strcat(buf, desc, &len, size);
                }
            }
            break;

        default:
            if(op->magic && ((QUERY_FLAG(op,FLAG_BEEN_APPLIED) &&
                need_identify(op)) || QUERY_FLAG(op,FLAG_IDENTIFIED))) {
                snprintf(buf+len, size-len, " %+d", op->magic);
                len += strlen(buf+len);
            }
    }
}

/**
 * Describes an item.
 *
 * @param op
 * item to describe. Must not be NULL.
 * @param buf
 * buffer that will contain the description.
 * @param size
 * size of buffer.
 */
void query_name(const object *op, char* buf, int size) {
    int len=0;
#ifdef NEW_MATERIAL_CODE
    materialtype_t *mt;
#endif

    buf[0] = '\0';

#ifdef NEW_MATERIAL_CODE
    if ((IS_ARMOR(op) || IS_WEAPON(op)) && op->materialname) {
        mt = name_to_material(op->materialname);
        if (mt) {
            safe_strcat(buf, mt->description, &len, size);
            safe_strcat(buf, " ", &len, size);
        }
    }
#endif

    query_short_name(op, buf+len, size-len);
    len += strlen(buf+len);

    if (QUERY_FLAG(op,FLAG_INV_LOCKED))
        safe_strcat(buf, " *", &len, size);
    if (op->type == CONTAINER && ((op->env && op->env->container == op) ||
        (!op->env && QUERY_FLAG(op,FLAG_APPLIED))))
        safe_strcat(buf," (open)", &len, size);

    if (QUERY_FLAG(op,FLAG_KNOWN_CURSED)) {
        if(QUERY_FLAG(op,FLAG_DAMNED))
            safe_strcat(buf, " (damned)", &len, size);
        else if(QUERY_FLAG(op,FLAG_CURSED))
            safe_strcat(buf, " (cursed)", &len, size);
    }
    if(QUERY_FLAG(op,FLAG_BLESSED) && QUERY_FLAG(op,FLAG_KNOWN_BLESSED))
            safe_strcat(buf, " (blessed)", &len, size);

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
        safe_strcat(buf, " (magic)", &len, size);

#if 0
    /* item_power will be returned in desribe_item - it shouldn't really
     * be returned in the name.
     */
    if(op->item_power)
        snprintf(buf+strlen(buf), size-strlen(buf), "(item_power %+d)",
            op->item_power);

#endif

    if (QUERY_FLAG(op,FLAG_APPLIED)) {
        switch(op->type) {
            case BOW:
            case WAND:
            case ROD:
            case HORN:
                safe_strcat(buf," (readied)", &len, size);
                break;
            case WEAPON:
                safe_strcat(buf," (wielded)", &len, size);
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
                safe_strcat(buf," (worn)", &len, size);
                break;
            case CONTAINER:
                safe_strcat(buf," (active)", &len, size);
                break;
            case SKILL:
            default:
                safe_strcat(buf," (applied)", &len, size);
        }
    }
    if(QUERY_FLAG(op, FLAG_UNPAID))
        safe_strcat(buf," (unpaid)", &len, size);
}

/**
 * Query a short name for the item.
 *
 * This is a lot like query_name(), but we
 * don't include the item count or item status.  Used for inventory sorting
 * and sending to client.
 * If plural is set, we generate the plural name of this.
 *
 * @param op
 * item we want the name of.
 * @param plural
 * whether to get the singular or plural name
 * @param buf
 * buffer that will contain the object's name. Must not be NULL.
 * @param size
 * buffer's length
 */
void query_base_name(const object *op, int plural, char* buf, int size) {
    int len;
#ifdef NEW_MATERIAL_CODE
    materialtype_t *mt;
#endif

    if((!plural && !op->name) || (plural && !op->name_pl)) {
        strncpy(buf, "(null)", size);
        return;
    }

    if(!op->nrof && !op->weight && !op->title && !is_magical(op)) {
        strncpy(buf, op->name, size); /* To speed things up (or make things slower?) */
        return;
    }

    buf[0] = '\0';

#ifdef NEW_MATERIAL_CODE
    if ((IS_ARMOR(op) || IS_WEAPON(op)) && op->materialname)
        mt = name_to_material(op->materialname);
    else
        mt = NULL;

    if (mt &&
        op->arch->clone.materialname != mt->name &&
        !(op->material & M_SPECIAL)) {
        snprintf(buf, size, "%s", mt->description);
        len=strlen(buf);
        safe_strcat(buf, " ", &len, size);
        safe_strcat(buf, plural ? op->name_pl : op->name, &len, size);
    } else {
#endif
    snprintf(buf, size, "%s", plural ? op->name_pl : op->name);
    len=strlen(buf);
#ifdef NEW_MATERIAL_CODE
    }
#endif

    if (op->title && QUERY_FLAG(op,FLAG_IDENTIFIED)) {
        safe_strcat(buf, " ", &len, size);
        safe_strcat(buf, op->title, &len, size);
    }

    switch(op->type) {
        case SPELLBOOK:
        case SCROLL:
        case WAND:
        case ROD:
            if (QUERY_FLAG(op,FLAG_IDENTIFIED)||QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
                if(!op->title) {
                    safe_strcat(buf," of ", &len, size);
                    if (op->inv)
                        safe_strcat(buf,op->inv->name, &len, size);
                    else
                        LOG(llevError,"Spellbook %s lacks inventory\n", op->name);
                }
                if(op->type != SPELLBOOK) {
                    snprintf(buf+len, size-len, " (lvl %d)", op->level);
                    len += strlen(buf+len);
                }
            }
            break;

        case SKILL:
        case AMULET:
        case RING:
            if (!op->title) {
            /* If ring has a title, full description isn't so useful */
                char s[MAX_BUF];
                ring_desc(op, s, MAX_BUF);
                if (s[0]) {
                    safe_strcat (buf, " ", &len, size);
                    safe_strcat (buf, s, &len, size);
                }
            }
            break;

        default:
            if(op->magic && ((QUERY_FLAG(op,FLAG_BEEN_APPLIED) &&
                need_identify(op)) || QUERY_FLAG(op,FLAG_IDENTIFIED))) {
                snprintf(buf+strlen(buf), size-strlen(buf), " %+d", op->magic);
            }
    }
}

/**
 * Describes a monster.
 *
 * @param op
 * monster to describe. Must not be NULL.
 * @param retbuf
 * buffer that will contain the description. Must not be NULL.
 * @param size
 * buffer size.
 *
 * @note
 * Break this off from describe_item - that function was way
 * too long, making it difficult to read.  This function deals
 * with describing the monsters & players abilities.  It should only
 * be called with monster & player objects.
 *
 * @todo
 * Rename to describe_living (or equivalent) since called for player too.
 * Use safe string functions. Fix weird sustenance logic.
 */
void describe_monster(const object *op, char* retbuf, int size) {
    int i;
    int len;

    retbuf[0]='\0';

    /* Note that the resolution this provides for players really isn't
     * very good.  Any player with a speed greater than .67 will
     * fall into the 'lightning fast movement' category.
     */
    if(FABS(op->speed)>MIN_ACTIVE_SPEED) {
        switch((int)((FABS(op->speed))*15)) {
            case 0:
                snprintf(retbuf, size,"(very slow movement)");
                break;
            case 1:
                snprintf(retbuf, size, "(slow movement)");
                break;
            case 2:
                snprintf(retbuf, size, "(normal movement)");
                break;
            case 3:
            case 4:
                snprintf(retbuf, size, "(fast movement)");
                break;
            case 5:
            case 6:
                snprintf(retbuf, size, "(very fast movement)");
                break;
            case 7:
            case 8:
            case 9:
            case 10:
                snprintf(retbuf, size, "(extremely fast movement)");
                break;
            default:
                snprintf(retbuf, size, "(lightning fast movement)");
            break;
        }
    }
    if(QUERY_FLAG(op,FLAG_UNDEAD))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(undead)");
    if(QUERY_FLAG(op,FLAG_SEE_INVISIBLE))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(see invisible)");
    if(QUERY_FLAG(op,FLAG_USE_WEAPON))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(wield weapon)");
    if(QUERY_FLAG(op,FLAG_USE_BOW))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(archer)");
    if(QUERY_FLAG(op,FLAG_USE_ARMOUR))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(wear armour)");
    if(QUERY_FLAG(op,FLAG_USE_RING))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(wear ring)");
    if(QUERY_FLAG(op,FLAG_USE_SCROLL))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(read scroll)");
    if(QUERY_FLAG(op,FLAG_USE_RANGE))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(fires wand/rod/horn)");
    if(QUERY_FLAG(op,FLAG_CAN_USE_SKILL))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(skill user)");
    if(QUERY_FLAG(op,FLAG_CAST_SPELL))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(spellcaster)");
    if(QUERY_FLAG(op,FLAG_FRIENDLY))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(friendly)");
    if(QUERY_FLAG(op,FLAG_UNAGGRESSIVE))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(unaggressive)");
    if(QUERY_FLAG(op,FLAG_HITBACK))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(hitback)");
    if(QUERY_FLAG(op,FLAG_STEALTH))
        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(stealthy)");
    if(op->randomitems != NULL) {
        treasure *t;
        int first = 1;
        for(t=op->randomitems->items; t != NULL; t=t->next)
            if(t->item && (t->item->clone.type == SPELL)) {
                if(first) {
                    first = 0;
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(Spell abilities:)");
                }
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(%s)",  t->item->clone.name);
            }
    }
    if (op->type == PLAYER) {
        if(op->contr->digestion) {
            if(op->contr->digestion!=0)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(sustenance%+d)", op->contr->digestion);
        }
        if(op->contr->gen_grace) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(grace%+d)", op->contr->gen_grace);
        }
        if(op->contr->gen_sp) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(magic%+d)", op->contr->gen_sp);
        }
        if(op->contr->gen_hp) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(regeneration%+d)", op->contr->gen_hp);
        }
        if(op->stats.luck) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(luck%+d)",op->stats.luck);
        }
    }

    /* describe attacktypes */
    len = strlen(retbuf);
    if (is_dragon_pl(op)) {
        /* for dragon players display the attacktypes from clawing skill
         * Break apart the for loop - move the comparison checking down -
         * this makes it more readable.
         */
        object *tmp;

        for (tmp=op->inv; tmp!=NULL; tmp=tmp->below)
            if (tmp->type == SKILL && !strcmp(tmp->name, "clawing")) break;

        if (tmp && tmp->attacktype!=0) {
            DESCRIBE_ABILITY_SAFE(retbuf, tmp->attacktype, "Claws", &len, size);
        }
        else {
            DESCRIBE_ABILITY_SAFE(retbuf, op->attacktype, "Attacks", &len, size);
        }
    } else {
        DESCRIBE_ABILITY_SAFE(retbuf, op->attacktype, "Attacks", &len, size);
    }
    DESCRIBE_PATH_SAFE(retbuf, op->path_attuned, "Attuned", &len, size);
    DESCRIBE_PATH_SAFE(retbuf, op->path_repelled, "Repelled", &len, size);
    DESCRIBE_PATH_SAFE(retbuf, op->path_denied, "Denied", &len, size);
    for (i=0; i < NROFATTACKS; i++) {
        if (op->resist[i]) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(%s %+d)", resist_plus[i], op->resist[i]);
        }
    }
}


/**
 * Describes an item, in all its details.
 *
 * \li If it is a monster, lots of information about its abilities
 * will be returned.
 * \li If it is an item, lots of information about which abilities
 * will be gained about its user will be returned.
 * \li If it is a player, it writes out the current abilities
 * of the player, which is usually gained by the items applied.
 *
 * It would be really handy to actually pass another object
 * pointer on who is examining this object.  Then, you could reveal
 * certain information depending on what the examiner knows, eg,
 * wouldn't need to use the SEE_INVISIBLE flag to know it is
 * a dragon player examining food.  Could have things like
 * a dwarven axe, in which the full abilities are only known to
 * dwarves, etc.
 *
 * Add 'owner' who is the person examining this object.
 * owner can be null if no one is being associated with this
 * item (eg, debug dump or the like)
 *
 * @param op
 * object to describe. Must not be NULL.
 * @param owner
 * player examining the object.
 * @param retbuf
 * buffer that will contain the description. Must not be NULL.
 * @param size
 * size of buffer.
 *
 * @note
 * This function is really much more complicated than it should
 * be, because different objects have different meanings
 * for the same field (eg, wands use 'food' for charges).  This
 * means these special cases need to be worked out.
 *
 * @todo
 * Check whether owner is really needed. Use safe string functions.
 * Check spurious food logic.
 */
void describe_item(const object *op, const object *owner, char* retbuf, int size) {
    int identified,i;

    retbuf[0]='\0';
    if(QUERY_FLAG(op,FLAG_MONSTER) || op->type==PLAYER) {
        describe_monster(op, retbuf, size);
        return;
    }
    /* figure this out once, instead of making multiple calls to need_identify.
     * also makes the code easier to read.
     */
    if (!need_identify(op) || QUERY_FLAG(op, FLAG_IDENTIFIED)) identified = 1;
    else {
        snprintf(retbuf, size, "(unidentified)");
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

        case POWER_CRYSTAL:
            if (op->stats.maxsp>1000){ /*higher capacity crystals*/
                i = (op->stats.maxsp%1000)/100;
                if (i)
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(capacity %d.%dk). It is ", op->stats.maxsp/1000,i);
                else
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(capacity %dk). It is ", op->stats.maxsp/1000);
            } else
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(capacity %d). It is ", op->stats.maxsp);
            i = (op->stats.sp*10)/op->stats.maxsp;
            if (op->stats.sp==0)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "empty.");
            else if (i==0)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "almost empty.");
            else if (i<3)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "partially filled.");
            else if (i<6)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "half full.");
            else if (i<9)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "well charged.");
            else if (op->stats.sp == op->stats.maxsp)
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "fully charged.");
            else
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "almost full.");
            break;

        case FOOD:
        case FLESH:
        case DRINK:
            if(identified || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(food+%d)", op->stats.food);

                if (op->type == FLESH && op->last_eat>0 && atnr_is_dragon_enabled(op->last_eat)) {
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(%s metabolism)", change_resist_msg[op->last_eat]);
                }

                if (!QUERY_FLAG(op,FLAG_CURSED)) {
                    if (op->stats.hp)
                        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(heals)");
                    if (op->stats.sp)
                        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(spellpoint regen)");
                }
                else {
                    if (op->stats.hp)
                        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(damages)");
                    if (op->stats.sp)
                        snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(spellpoint depletion)");
                }
            }
            break;

        case SKILL:
        case RING:
        case AMULET:
            if(op->item_power) {
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(item_power %+d)", op->item_power);
            }
            if (op->title) {
                ring_desc(op, retbuf+strlen(retbuf), size-strlen(retbuf));
            }
            return;

        default:
            return;
    }

    /* Down here, we more further describe equipment type items.
     * only describe them if they have been identified or the like.
     */
    if (identified || QUERY_FLAG(op,FLAG_BEEN_APPLIED)) {
        int attr,val;

        for (attr=0; attr<NUM_STATS; attr++) {
            if ((val=get_attr_value(&(op->stats),attr))!=0) {
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(%s%+d)", short_stat_name[attr], val);
            }
        }

        switch (op->type) {
            case FLESH:
                break;
            default:
                if(op->stats.exp) {
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(speed %+" FMT64 ")", op->stats.exp);
                }
                break;
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
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(wc%+d)", op->stats.wc);
                }
                if(op->stats.dam) {
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(dam%+d)", op->stats.dam);
                }
                if(op->stats.ac) {
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(ac%+d)", op->stats.ac);
                }
                if ((op->type==WEAPON || op->type == BOW) && op->level>0) {
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(improved %d/%d)", op->last_eat, op->level);
                }
                break;

            default:
                break;
        }
        if(QUERY_FLAG(op,FLAG_XRAYS))
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(xray-vision)");
        if(QUERY_FLAG(op,FLAG_SEE_IN_DARK))
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(infravision)");

        /* levitate was what is was before, so we'll keep it */
        if (op->move_type & MOVE_FLY_LOW)
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(levitate)");

        if (op->move_type & MOVE_FLY_HIGH)
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(fly)");

        if (op->move_type & MOVE_SWIM)
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(swim)");

        /* walking is presumed as 'normal', so doesn't need mentioning */

        if(op->item_power) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(item_power %+d)", op->item_power);
        }
    } /* End if identified or applied */

    /* This blocks only deals with fully identified object.
     * it is intentional that this is not an 'else' from a above -
     * in this way, information is added.
      */
    if(identified) {
        int more_info = 0;
        int len;

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
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(Max speed %1.2f)", ARMOUR_SPEED(op)/10.0);
                }
                if (ARMOUR_SPELLS(op)) {
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(Spell regen penalty %d)", ARMOUR_SPELLS(op));
                }
                more_info=1;
                break;

            case WEAPON:
                /* Calculate it the same way fix_object does so the results
                 * make sense.
                 */
                i = (WEAPON_SPEED(op)*2-op->magic)/2;
                if (i<0) i=0;

                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(weapon speed %d)", i);
                more_info=1;
                break;
        }
        if (more_info) {
            if(op->stats.food) {
                if(op->stats.food!=0)
                    snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(sustenance%+d)", op->stats.food);
            }
            if(op->stats.grace) {
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(grace%+d)", op->stats.grace);
            }
            if(op->stats.sp) {
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(magic%+d)", op->stats.sp);
            }
            if(op->stats.hp) {
                snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(regeneration%+d)", op->stats.hp);
            }
        }

        if(op->stats.luck) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(luck%+d)", op->stats.luck);
        }
        if(QUERY_FLAG(op,FLAG_LIFESAVE))
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(lifesaving)");
        if(QUERY_FLAG(op,FLAG_REFL_SPELL))
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(reflect spells)");
        if(QUERY_FLAG(op,FLAG_REFL_MISSILE))
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(reflect missiles)");
        if(QUERY_FLAG(op,FLAG_STEALTH))
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(stealth)");
        if(op->slaying!=NULL && op->type != FOOD) {
            snprintf(retbuf+strlen(retbuf), size-strlen(retbuf), "(slay %s)", op->slaying);
        }
        len = strlen(retbuf);
        DESCRIBE_ABILITY_SAFE(retbuf, op->attacktype, "Attacks", &len, size);
        /* resistance on flesh is only visible for dragons.  If
         * non flesh, everyone can see its resistances
         */
        if (op->type != FLESH || (owner && is_dragon_pl(owner))) {
            describe_resistance(op, 0, retbuf+strlen(retbuf), size-strlen(retbuf));
        }
        len = strlen(retbuf);
        DESCRIBE_PATH_SAFE(retbuf, op->path_attuned, "Attuned", &len, size);
        DESCRIBE_PATH_SAFE(retbuf, op->path_repelled, "Repelled", &len, size);
        DESCRIBE_PATH_SAFE(retbuf, op->path_denied, "Denied", &len, size);
    }
}

/**
 * Checks whether object is magical.
 *
 *  A magical item is one that
 * increases/decreases any abilities, provides a resistance,
 * has a generic magical bonus, or is an artifact.
 *
 * @param op
 * item to check.
 * @return
 * true if the item is magical.
 */
int is_magical(const object *op) {
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
    /* Presume any boots that have a move_type are special. */
    if (op->type== BOOTS &&
        ((QUERY_FLAG(op, FLAG_STEALTH) || op->move_type ||  op->stats.exp)))
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
        op->type != BOOTS && op->type != GLOVES && op->type != ARMOUR)
        return 1;

   /* power crystal, spellbooks, and scrolls are always magical.  */
   if (op->magic || op->type==POWER_CRYSTAL || op->type==SPELLBOOK ||
        op->type==SCROLL || op->type==GIRDLE)
        return 1;

    /* Check to see if it increases/decreases any stats */
    for (i=0; i<NUM_STATS; i++)
        if (get_attr_value(&(op->stats),i)!=0) return 1;

    /* If it doesn't fall into any of the above categories, must
     * be non magical.
     */
    return 0;
}

/**
 * This function really should not exist - by default, any item not identified
 * should need it.
 *
 * @param op
 * item to check.
 * @return
 * true if the item should be identified.
 * @todo
 * either remove this function, or fix comment above :)
 */
int need_identify(const object *op) {
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


/**
 * Identifies an item.
 * Supposed to fix face-values as well here, but later.
 *
 * @param op
 * item to identify. Can be already identified without ill effects.
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

    if (QUERY_FLAG(op,FLAG_BLESSED))
        SET_FLAG(op,FLAG_KNOWN_BLESSED);

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
        pl = get_player_container(op->env);
        if (pl)
            /* A lot of the values can change from an update - might as well send
             * it all.
             */
            esrv_send_item(pl, op);
    }
}

/*
 * static char *rcsid_attack_h =
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


#ifndef ATTACK_H
#define ATTACK_H

/* These are default values for converting from simple protected/immune/vuln
 * to thre partial resistances
 */
#define RESIST_IMMUNE	100
#define RESIST_PROT	30
#define RESIST_VULN	-100

/*
 * Attacktypes:
 * ATNR_... is the attack number that is indexed into the
 * the resist array in the object structure.
 */

#define NROFATTACKS		24

/* Note that the last ATNR_ should be one less than NROFATTACKS above
 * since the ATNR starts counting at zero.
 * For compatible loading, these MUST correspond to the same value
 * as the bitmasks below.
 */
#define ATNR_PHYSICAL		0
#define ATNR_MAGIC		1
#define ATNR_FIRE		2
#define ATNR_ELECTRICITY	3
#define ATNR_COLD		4
#define ATNR_CONFUSION		5
#define ATNR_ACID		6
#define ATNR_DRAIN		7
#define ATNR_WEAPONMAGIC	8
#define ATNR_GHOSTHIT		9
#define ATNR_POISON		10
#define ATNR_SLOW		11
#define ATNR_PARALYZE		12
#define ATNR_TURN_UNDEAD	13
#define ATNR_FEAR		14
#define ATNR_CANCELLATION	15
#define ATNR_DEPLETE		16
#define ATNR_DEATH		17
#define ATNR_CHAOS		18
#define ATNR_COUNTERSPELL	19
#define ATNR_GODPOWER		20
#define ATNR_HOLYWORD		21
#define ATNR_BLIND		22
#define ATNR_INTERNAL		23

#define AT_PHYSICAL	0x00000001 /*       1 */
#define AT_MAGIC	0x00000002 /*       2 */
#define AT_FIRE		0x00000004 /*       4 */
#define AT_ELECTRICITY	0x00000008 /*       8 */
#define AT_COLD		0x00000010 /*      16 */
#define AT_CONFUSION	0x00000020 /*      32 The spell will use this one */
#define AT_ACID		0x00000040 /*      64 Things might corrode when hit */
#define AT_DRAIN	0x00000080 /*     128 */
#define AT_WEAPONMAGIC	0x00000100 /*     256 Very special, use with care */
#define AT_GHOSTHIT	0x00000200 /*     512 Attacker dissolves */
#define AT_POISON	0x00000400 /*    1024 */
#define AT_SLOW		0x00000800 /*    2048 */
#define AT_PARALYZE	0x00001000 /*    4096 */
#define AT_TURN_UNDEAD	0x00002000 /*    8192 */
#define AT_FEAR		0x00004000 /*   16384 */
#define AT_CANCELLATION 0x00008000 /*   32768 ylitalo@student.docs.uu.se */
#define AT_DEPLETE      0x00010000 /*   65536 vick@bern.docs.uu.se */
#define AT_DEATH        0x00020000 /*  131072 peterm@soda.berkeley.edu */
#define AT_CHAOS        0x00040000 /*  262144 peterm@soda.berkeley.edu*/
#define AT_COUNTERSPELL 0x00080000 /*  524288 peterm@soda.berkeley.edu*/
#define AT_GODPOWER	0x00100000 /* 1048576  peterm@soda.berkeley.edu */
#define AT_HOLYWORD	0x00200000 /* 2097152 race selective attack thomas@astro.psu.edu */ 
#define AT_BLIND	0x00400000 /* 4194304 thomas@astro.psu.edu */ 
#define AT_INTERNAL	0x00800000 /* Only used for internal calculations */

/* attacktypes_load is suffixed to resist_ when saving objects.
 * (so the line may be 'resist_fire' 20 for example).  These are never
 * seen by the player.  loader.l uses the same names, but it doesn't look
 * like you can use these values, so in that function they are hard coded.
 */

/* Note that internal should not ever be referanced in the last two
 * tables.  however, other resisttypes may be added, and if through some
 * bug these do get used somehow, might as well make it more easier to notice
 * and not have mystery values appear.
 */
#ifndef INIT_C
EXTERN int resist_table[];
EXTERN char *resist_change[NROFATTACKS][4];
EXTERN char *resist_plus[NROFATTACKS];
EXTERN char *attacktype_desc[NROFATTACKS];
EXTERN char *resist_save[NROFATTACKS];

#else
EXTERN char *resist_save[NROFATTACKS] = {
"physical", "magic", "fire", "electricity", "cold", "confusion", "acid",
"drain", "weaponmagic", "ghosthit", "poison", "slow", "paralyze",
"turn_undead", "fear", "cancellation", "deplete", "death", "chaos",
"counterspell", "godpower", "holyword", "blind" ,
"internal"
};

/* Short description of names of the attacktypes */
EXTERN char *attacktype_desc[NROFATTACKS] = {
"physical", "magic", "fire", "electricity", "cold", "confusion", "acid",
"drain", "weapon magic", "ghost hit", "poison", "slow", "paralyze",
"turn undead", "fear", "cancellation", "deplete", "death", "chaos",
"counterspell", "god power", "holy word", "blind" ,
"internal"
};

/* This is the array that is what the player sees. */

EXTERN char *resist_plus[NROFATTACKS] = {
"armour", "resist magic", "resist fire", "resist electricity", "resist cold", 
"resist confusion", "resist acid", "resist drain", 
"resist weaponmagic", "resist ghosthit", "resist poison", "resist slow",
"resist paralyzation", "resist turn undead", "resist fear", 
"resist cancellation", "resist depletion", "resist death", "resist chaos",
"resist counterspell", "resist god power", "resist holy word",
"resist blindness" , 
"resist internal"
};

/* These are the message displayed when a player puts on/takes off an item
 * that gives them some resistance.  These are triples - immune (100),
 * protection (positive) and negative, with the message of when you add
 * and subtract the entry.
 * Index 0 is what you see when you add an immunity.
 * Index 1 is what you see when you remove an immuity.
 * Index 2 is what you see when you gain protective value
 * Index 3 is what you see when you lose protective value.
 */

EXTERN char *resist_change[NROFATTACKS][4] = {
    /* Physical isn't used right now */
    {"You feel impervious to attack.", "You no longer feel impervious to attack.",
     "You feel more armoured.", "You feel less armoured."},

    {"You feel immune to magic.",  "You feel less immune to magic.",
     "You feel more resistant to magic.", "You feel less resistant to magic."},

    {"You feel immune to fire.", "You feel less immune to fire.",
     "You feel resistant to fire.", "You feel less resistant to fire."},

    {"You feel immune to electricity.", "You feel less immune to electricity.",
     "You feel more resistant to electricity.", "You feel less resistant to electricity."},

    {"You feel immune to cold.","You feel less immune to cold.",
	"You feel resistant to cold.", "You feel less resistant to cold."},

    {"Your mind is warded against disruption .", "Your mind is no longer warded against disruption.", 
	"You feel more resistant to confusion.", "You feel less resistant to confusion."},

    {"You feel immune to acid.",
	    "You feel less immune to acid.", 
	    "You feel more resistant to acid.",
	    "You feel less resistant to acid."
    },

    {"You feel very full of life.",
	    "You shiver, everything seems so bleak.",
	    "You feel more resistant to draining.",
	    "You feel less resistant to draining."
    },
    /* Player shouldn't see these */
    {"You feel immune to weapon magic.",  
	"You feel less immune to weapon magic.",
	"You feel more resistant to weapon magic.", 
	"You feel less resistant to weapon magic."
    },

    /* More than just ghosts have ghosthit, but we will
     * use ghosts as a general term here.
     */
    {"You feel immune to ghosts.",  
	"You feel less immune to ghosts.",
	"You feel more resistant to ghosts.", 
	"You feel less resistant to ghosts."
    },
    {"You feel immune to poison.",
	"You feel less immune to poision.",
	"You feel more resistant to poison.",
	"You feel less resistant to poison."
    },
    {"You feel in sync with time.", /* SLOW */
	"You feel out of sync with time.",
	"You feel more in sync with time.",
	"You feel less in sync with time."
    },
    {"You feel very unrestrained.",
	"You feel more restrained.",
	"You feel more resistant to paralyzation.",
	"You feel less resistant to paralyzation."
    },
    {"You feel immune to clerical turning",
	"You feel less immune to clerical turning",
	"You feel more resistant to clerical turning",
	"You feel less resistant to clerical turning"
    },
    {"You feel extremely brave.",
	"You feel less brave.",
	"You feel more resistant to fear.",
	"You feel less resistant to fear."
    },
    {"", "", "", ""},	/* Cancellation - not attainable by players */

    {"You feel immune to depletion.",
	    "You feel less immune to depletion",
	    "You feel more resistant to depletion.",
	    "You feel less resistant to depletion."
    },

    {"You feel immune to death magic.",
	    "You feel less immune to death magic",
	    "You feel more resistant to death magic.",
	    "You feel less resistant to death magic."
    },
    {"", "", "", ""},	/* Chaos - not attainable by players */
    {"", "", "", ""},	/* Counterspell - not attainable by players */
    {"", "", "", ""},	/* God Power - not attainable by players */
    {"You feel immune to the power of holy words",
	"You feel less immune to the power of holy words",
	"You feel more resistant to the power of holy words",
	"You feel less resistant to the power of holy words"
    },
    {"You feel immune to blinding",
	"You feel less immune to blinding",
	"You feel more resistant to blinding",
	"You feel less resistant to blinding"
    },
    {"", "", "", ""}	/* Internal - not attainable by players */
};

/* If you want to weight things so certain resistances show up more often than
 * others, just add more entries in the table for the protections you want to
 * show up.
 */
EXTERN int resist_table[] = {ATNR_PHYSICAL, ATNR_MAGIC, ATNR_FIRE, 
    ATNR_ELECTRICITY,ATNR_COLD, ATNR_CONFUSION, ATNR_ACID, ATNR_DRAIN,
    ATNR_GHOSTHIT, ATNR_POISON, ATNR_SLOW, ATNR_PARALYZE, ATNR_TURN_UNDEAD,
    ATNR_FEAR, ATNR_DEPLETE, ATNR_DEATH, ATNR_HOLYWORD, ATNR_BLIND};

#endif

#define num_resist_table 18

#endif

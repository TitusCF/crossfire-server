/*
 * static char *rcsid_spellist_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel

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

    The author can be reached via e-mail to mark@pyramid.com
*/

#include "spells.h"

spell spells[NROFREALSPELLS]={
/* name,lvl,sp,charges,time,scrolls[nr,chance],book-chance,rng,defensive,cleric,self
   path, other arch*/
{"magic bullet",       		 1, 1, 99, 2, 0, 0,  10, 1, 0, 0, 0,
 PATH_MISSILE, "bullet",},						/* 0 */
{"small fireball",     		 1, 6, 40, 5, 0, 0,  8,  1, 0, 0, 0,
 PATH_FIRE, "firebullet_s",},
{"medium fireball",    		 3,10, 20, 10, 0, 0,  6,  1, 0, 0, 0,
 PATH_FIRE, "firebullet_m",},
{"large fireball",     		 5,16, 10, 15, 0, 0,  2,  1, 0, 0, 0,
 PATH_FIRE, "firebullet_l",},
{"burning hands",      		 1, 5, 20, 5, 0, 0,  8,  1, 0, 0, 0,
 PATH_FIRE, "firebreath",},
{"small lightning",		 1, 6, 40, 5, 0, 0,  8,  1, 0, 0, 0,
 PATH_ELEC, "lightning_s",},
{"large lightning",		 4, 13, 20, 12, 0, 0,  3,  1, 0, 0, 0,
 PATH_ELEC, "lightning_l",},
{"magic missile",        	 1, 1, 75, 3, 0, 0,  8,  1, 0, 0, 0,
 PATH_MISSILE, "magic_missile",},
{"create bomb",          	 6,10, 5, 20, 1, 1,  3,  1, 1, 0, 0,
 PATH_DETONATE, "bomb",},
{"summon golem",         	 2, 5, 10, 30, 1, 1,  8,  1, 1, 0, 0,
 PATH_SUMMON, "golem",},
{"summon fire elemental",	 7,25, 4,  40, 1, 1,  2,  1, 1, 0, 0,
 PATH_SUMMON, "fire_elemental",},					/* 10 */
{"summon earth elemental",	 4,15, 10, 40, 1, 1,  3,  1, 1, 0, 0,
 PATH_SUMMON, "earth_elemental",},
{"summon water elemental",	 5,15, 8,  40, 1, 1,  4,  1, 1, 0, 0,
 PATH_SUMMON, "water_elemental",},
{"summon air elemental",	 6,20, 6,  40, 1, 1,  5,  1, 1, 0, 0,
 PATH_SUMMON, "air_elemental",},
{"dimension door",   	   	10,25, 8,  1, 0, 0,  1,  1, 0, 0, 0,
 PATH_TELE, "enchantment",},
{"create earth wall",    	 4, 6, 12, 30, 0, 0,  6,  1, 1, 0, 0,
 PATH_CREATE, "earthwall",},
{"paralyze",			 2, 5, 40, 8, 0, 0,  8,  1, 0, 0, 0,
 PATH_NULL, "paralyze",},
{"icestorm",			 1, 5, 15, 8, 0, 0,  4,  1, 0, 0, 0,
 PATH_FROST, "icestorm",},
{"magic mapping",		 5,15, 20, 1, 2, 8,  5,  0, 0, 0, 1,
 PATH_INFO, "enchantment",},
{"turn undead",			 1, 2, 40, 5, 0, 0,  8,  1, 0, 1, 0,
 PATH_TURNING, "turn_undead",},
{"fear",			 4, 6, 25, 5, 0, 0,  5,  1, 0, 0, 0,
 PATH_MIND, "fear",},							/* 20 */
{"poison cloud",		 2, 5, 30, 10, 0, 0,  6,  1, 0, 0, 0,
 PATH_MISSILE, "spellball",},
{"wonder",			 3,10, 20, 0, 0, 0,  0,  1, 0, 0, 0,
 PATH_TRANSMUTE, "flowers",},
{"destruction",			18,30,  0, 20, 3, 10, 1,  1, 0, 0, 1,
 PATH_NULL, "destruction",},
{"perceive self",		 2, 5, 20, 0, 2, 2,  0,  0, 0, 1, 1,
 PATH_INFO, "enchantment",},
{"word of recall",	        10,40,  3, 50, 1, 2,  1,  0, 0, 1, 1,
 PATH_TELE, "enchantment",},
{"invisible",			 6,15,  0, 5, 3, 2,  4,  1, 1, 0, 1,
 PATH_NULL, "enchantment",},
{"invisible to undead",		 6,25,  0, 5, 1, 2,  2,  1, 1, 1, 1,
 PATH_NULL, "enchantment",},
{"probe",			 1, 3, 40, 2, 0, 0,  6,  1, 0, 0, 0,
 PATH_INFO, NULL,},
{"large bullet",		 4, 3, 33, 6, 0, 0,  4,  1, 0, 0, 0,
 PATH_MISSILE, "lbullet",},
{"improved invisibility",	 8,25,  0, 10, 1, 1,  1,  1, 1, 0, 1,
 PATH_NULL, "enchantment",},						/* 30 */
{"holy word",			 1, 4,  0, 1, 0, 0,  4,  1, 0, 1, 0,
 PATH_TURNING, "holy_word",},
{"minor healing",		 1, 4, 40, 3, 0, 0,  9,  1, 1, 1, 1,
 PATH_RESTORE, "healing",},
{"medium healing",		 4, 7, 20, 6, 0, 0,  5,  1, 1, 1, 1,
 PATH_RESTORE, "healing",},
{"major healing",		 8,10, 12, 9, 0, 0,  3,  1, 1, 1, 1,
 PATH_RESTORE, "healing",},
{"heal",			10,50,  5, 12, 0, 0,  1,  1, 1, 1, 1,
 PATH_RESTORE, "healing",},
{"create food",	 6, 10,  0, 20, 0, 0,  4,  1, 1, 1, 0,
 PATH_CREATE, "food",},
{"earth to dust",		 2, 5,  0, 30, 0, 0,  2,  1, 1, 0, 0,
 PATH_NULL, "destruction",},
{"armour",			 1, 8,  0, 20, 3, 2,  8,  1, 1, 0, 1,
 PATH_SELF, "enchantment",},
{"strength",			 2,10,  0, 20, 3, 2,  6,  1, 0, 0, 1,
 PATH_SELF, "enchantment",},
{"dexterity",			 3,12,  0, 20, 3, 2,  4,  1, 0, 0, 1,
 PATH_SELF, "enchantment",},						/* 40 */
{"constitution",		 4,15,  0, 20, 3, 2,  4,  1, 1, 0, 1,
 PATH_SELF, "enchantment",},
{"charisma",			 3,12,  0, 20, 0, 0,  4,  1, 0, 0, 1,
 PATH_SELF, "enchantment",},
{"create fire wall",		 6, 5,  0, 10, 0, 0,  3,  1, 1, 0, 0,
 PATH_CREATE, "firebreath",},
{"create frost wall",		 8, 8,  0, 10, 0, 0,  2,  1, 1, 0, 0,
 PATH_CREATE, "icestorm",},
{"protection from cold",	 3,15,  0, 10, 1, 1,  3,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from electricity",	 4,15,  0, 10, 1, 1,  3,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from fire",	 5,20,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from poison",	 6,20,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from slow",	 7,20,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from paralysis",	 8,20,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},						/* 50 */
{"protection from draining",	 9,25,  0, 30, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from magic",	10,30,  0, 30, 1, 1,  1,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from attack",	13,50,  0, 50, 1, 1,  1,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"levitate",			 6,10,  0, 10, 1, 1,  2,  0, 0, 0, 1,
 PATH_NULL, "enchantment",},
{"small speedball",              3, 3,  0, 20, 0, 0,  0,  1, 0, 0, 0,
 PATH_MISSILE, "speedball",},
{"large speedball",		 6, 6,  0, 40, 0, 0,  0,  1, 0, 0, 0,
 PATH_MISSILE, "speedball",},
{"hellfire",			 8,13,  0, 30, 0, 0,  0,  1, 0, 0, 0,
 PATH_FIRE, "hellfire",},
{"dragonbreath",		12, 13,  0, 30, 0, 0,  0,  1, 0, 0, 0,
 PATH_FIRE, "firebreath",},
{"large icestorm",		12,13,  0, 40, 0, 0,  0,  1, 0, 0, 0,
 PATH_FROST, "icestorm",},
{"charging",			10,200, 0, 75, 1, 1,  0,  0, 0, 0, 1,
 PATH_TRANSFER, "enchantment",},					/* 60 */
{"polymorph",			 6,20, 10, 30, 0, 0,  0,  1, 0, 0, 0,
 PATH_TRANSMUTE, "polymorph",},
{"cancellation",		10,30, 10, 10, 0, 0,  1,  1, 0, 0, 0,
 PATH_ABJURE, "cancellation",},
{"confusion",			 2,10,  0, 10, 3, 2,  7,  0, 0, 0, 0,
 PATH_MIND, "confuse",},
{"mass confusion",		 7,20, 15, 20, 0, 0,  3,  1, 0, 0, 0,
 PATH_MIND, "confuse",},
{"summon pet monster",		 2, 5, 15, 40, 3, 1,  8,  1, 0, 0, 0,
 PATH_SUMMON, NULL,},
{"slow",			 1, 5, 30, 5, 0, 0,  7,  1, 0, 0, 0,
 PATH_NULL, "slow",},
{"regenerate spellpoints",      99, 0,  0, 0, 0, 0,  0,  0, 0, 0, 1,
 PATH_RESTORE, NULL,},
{"cure poison",                  4, 7,  0, 10, 0, 0,  5,  1, 1, 1, 1,
 PATH_RESTORE, "healing",},
{"protection from confusion",	 7,20,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"protection from cancellation",11,30,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},						/* 70 */
{"protection from depletion",	 7,20,  0, 10, 1, 1,  2,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"alchemy",			 3, 5,  0, 15, 3, 2,  7,  1, 0, 0, 1,
 PATH_TRANSMUTE, "enchantment",},
{"remove curse",		 8,80,  0,100, 1, 3,  1,  1, 0, 1, 1,
 PATH_RESTORE, "protection",},
{"remove damnation",		15,200, 0,200, 1, 1,  0,  1, 0, 1, 1,
 PATH_RESTORE, "protection",},
{"identify",			 8,60,  0,200, 9,10,  5,  1, 0, 0, 1,
 PATH_INFO, "enchantment",},
{"detect magic",		 1, 1, 50, 13, 3, 8, 10,  1, 0, 0, 1,
 PATH_INFO, "detect_magic",},
{"detect monster",		 2, 2,  0, 15, 3, 6,  8,  1, 1, 0, 1,
 PATH_INFO, "detect_magic",},
{"detect evil",			 3, 3,  0, 15, 3, 5,  8,  1, 1, 1, 1,
 PATH_INFO, "detect_magic",},
{"detect curse",		5,10,  0, 20, 3, 5,  1,  1, 0, 1, 1,
 PATH_INFO, "detect_magic",},
{"heroism",			10,50,  0, 10, 0, 0,  0,  1, 0, 0, 1,
 PATH_SELF, "enchantment",},						/* 80 */
{"aggravation",			 1, 0,  0, 1, 0, 0,  0,  0, 0, 0, 0,
 PATH_NULL, NULL,},
{"firebolt",			 2, 9, 35, 10, 0, 0,  4,  1, 0, 0, 0,
 PATH_FIRE, "firebolt",},
{"frostbolt",			 3,12, 30, 10, 0, 0,  3,  1, 0, 0, 0,
 PATH_FROST, "frostbolt",},
{"shockwave",                   14,26,  0, 20, 0, 0,  0,  1, 0, 0, 0,
 PATH_NULL, "shockwave",},
{"color spray",                 13,35,  0, 15, 0, 0,  0,  1, 0, 0, 0,
 PATH_NULL, "color_spray",},
{"haste",                       12,50,  0, 10, 0, 0,  0,  1, 0, 0, 1,
 PATH_SELF, "enchantment",},
{"face of death",               22, 80,  0, 15, 0, 0,  0,  1, 0, 1, 0,
 PATH_DEATH, "face_of_death",},
{"ball lightning",               9,10, 30, 30, 1, 9,  0,  1, 0, 0, 0,
 PATH_ELEC, "ball_lightning",},
{"meteor swarm",                12,30,  0, 30, 0, 0,  0,  1, 0, 0, 0,
 PATH_MISSILE, "meteor",},
{"comet",                        8,15,  0, 20, 0, 0,  0,  1, 0, 0, 0,
 PATH_MISSILE, NULL,},							/* 90 */
{"mystic fist",                  5,10,  0, 15, 0, 0,  1,  1, 0, 0, 0,
 PATH_SUMMON, "mystic_fist",}, 
{"raise dead",                  10,150,  0, 60, 0, 0,  0,  1, 0, 1, 0,
 PATH_RESTORE, "enchantment",},
{"resurrection",                20,250, 0, 180, 0, 0,  0,  0, 0, 1, 0,
 PATH_RESTORE, "enchantment",},
{"reincarnation",               25,350, 0,100, 0, 0,  0,  0, 0, 1, 0,
 PATH_RESTORE, "enchantment",},
/* mlee - Keep these spells 0 book chance, as they are low level quest items.*/
/* raised the grace value on some immuntity spells -b.t. */
  /* keep them path_null, that prevents them being given out by praying*/
{"immunity to cold",            6, 60,  0, 10, 0, 0,  0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to electricity",     8, 65,  0, 10, 0, 0,  0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to fire",            10,70,  0, 10, 0, 0,  0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to poison",          12,60,  0, 10, 0, 0,  0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to slow",            14,60,  0, 10, 0, 0,  0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to paralysis",        16,60,  0, 10, 0, 0, 0,  1, 1, 1, 1,
 PATH_NULL, "protection",},						/*100*/
{"immunity to draining",         18,75,  0, 10, 0, 0, 0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to magic",           20,150,  0, 30, 0, 0,  0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"immunity to attack",          26,170,  0, 50, 0, 0, 0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
/* set the path to PATH_NULL so god_intervention wont hand out this spell */
{"invulnerability",             80,225,  0, 30, 0, 0, 0,  1, 1, 1, 1,
 PATH_NULL, "protection",},
{"defense",                     40,75,   0, 30, 0, 0, 0,  1, 1, 1, 1,
 PATH_PROT, "protection",},
{"rune of fire",                4,10,    0, 30, 0, 0, 5,  1, 0, 0, 0,
 PATH_FIRE, "rune_fire",},
{"rune of frost",               6,12,    0, 30, 0, 0, 4,  1, 0, 0, 0,
 PATH_FROST, "rune_frost",},
{"rune of shocking",            8,14,    0, 30, 0, 0, 3,  1, 0, 0, 0,
 PATH_ELEC, "rune_shock",},
{"rune of blasting",           10,18,    0, 30, 0, 0, 2,  1, 0, 0, 0,
 PATH_DETONATE, "rune_blast",},
{"rune of death",              17,20,    0, 40, 0, 0, 1,  1, 0, 0, 0,
 PATH_DEATH, "rune_death",},						/*110*/
{"marking rune",                1,2,     0, 10, 0, 0, 5,  0, 0, 0, 0,
 PATH_NULL, "rune_mark",},
{"build director",             10,30,    0, 30, 0, 0, 1,  1, 0, 0, 0,
 PATH_CREATE, NULL,},
{"create pool of chaos",       10,10,   10, 15, 0, 0, 1,  1, 0, 0, 0,
 PATH_CREATE, "color_spray",},
{"build bullet wall",          12,35,    0, 35, 0, 0, 1,  1, 0, 0, 0,
 PATH_CREATE, NULL,},
{"build lightning wall",       14,40,    0, 40, 0, 0, 1,  1, 0, 0, 0,
 PATH_CREATE, NULL,},
{"build fireball wall",        16,45,    0, 45, 0, 0, 1,  1, 0, 0, 0,
 PATH_CREATE, NULL,},
{"magic rune",                  12,5,    0, 30, 0, 0, 1,  0, 0, 0, 0,
 PATH_CREATE, "generic_rune",},
{"rune of magic drain",        14,30,    0, 30, 0, 0, 0,  1, 0, 0, 0,
 PATH_TRANSFER, "rune_drain_magic",},
{"antimagic rune",              7,5,     0, 20, 0, 0, 1,  1, 0, 0, 0,
 PATH_ABJURE, "rune_antimagic",},
{"rune of transferrence",       6,12,    0, 40, 0, 0, 1,  1, 0, 0, 0,
 PATH_TRANSFER, "rune_transferrence",},					/*120*/
{"transferrence",               5,10,    0, 20, 0, 0, 1,  1, 0, 0, 0,
 PATH_TRANSFER, "enchantment",},
{"magic drain",                12,20,    0, 1, 0, 0, 1,  1, 0, 0, 0,
 PATH_TRANSFER, "enchantment",},
{"counterspell",                3,10,   20, 0, 0, 0, 1,  1, 0, 0, 0,
 PATH_ABJURE, "counterspell",},
{"disarm",                  4,7,     0, 30, 0, 0, 1,  1, 0, 0, 0,
 PATH_ABJURE, "enchantment",},
{"cure confusion",              7,8,     0, 15, 1, 4, 1,  1, 0, 1, 1,
 PATH_RESTORE, "healing",},
{"restoration",                 13,80,   0, 30, 1, 1, 1,  1, 0, 1, 1,
 PATH_RESTORE, "healing",},
{"summon evil monster",         8,8,     0, 30, 0, 0, 0,  0, 0, 0, 0,
 PATH_SUMMON, NULL,},
{"counterwall",                 8, 8,   30, 30, 0, 0, 1,  1, 0, 0, 0,
 PATH_RESTORE, "counterspell",},
{"cause light wounds",		1, 4,    0,  5, 0, 0, 2,  1, 0, 1, 0,
 PATH_WOUNDING,"cause_wounds",},
{"cause medium wounds",		3, 8,    0,  5, 0, 0, 2,  1, 0, 1, 0,
PATH_WOUNDING,"cause_wounds",},					/* 130 */
{"cause serious wounds",	5, 16,   0,  5, 0, 0, 2,  1, 0, 1, 0,
PATH_WOUNDING,"cause_wounds",},
{"charm monsters",		5, 20,    0, 10, 0, 0, 1,  1, 0, 0, 0,
PATH_MIND,"enchantment",},
{"banishment",			5, 10,   3, 10, 1, 1, 1,  1, 0, 1, 0,
PATH_TURNING,"banishment",},
{"create missile",		1,5,	0, 20, 1, 1, 1,  1, 0, 0, 0,
PATH_CREATE,"enchantment",},
{"show invisible",		7,10,	4, 20, 1, 1, 1,  1, 1, 1, 0,
PATH_INFO,"enchantment",},
{"xray",			10,20,	0, 20, 1, 1, 1,  1, 0, 0, 0,
PATH_INFO,"enchantment",},
{"pacify",			4, 10,	1, 2, 0, 0, 3,  1, 0, 1, 0,
PATH_MIND,"enchantment",}, 
{"summon fog",                   2, 5,  10, 10, 0, 0, 2,  1, 0, 0, 0,
PATH_CREATE,"fog",},
{"steambolt",                    5, 10, 10, 10, 0, 0,  1,  1, 0, 0, 0,
PATH_FIRE, "steambolt",},
{"command undead",               4, 12,  0, 10, 0, 0, 3,  1, 0, 1, 0,
PATH_MIND,"enchantment",},					/* 140 */ 
{"holy orb",                    7, 12,  0, 5, 0, 0, 3,  1, 0, 1, 0,
PATH_TURNING,"holy_orb",}, 
/* most of the next 13 spells work best if MULTIPLE_GODS is defined. -b.t. */
{"summon avatar",               10, 60, 0, 15, 0, 0, 1,  1, 0, 1, 0,
 PATH_SUMMON, "avatar",},
{"holy possession",             9, 30,  0, 10, 0, 0, 1,  1, 0, 1, 0,
 PATH_ABJURE, "enchantment",},
{"bless",                        2, 8,  0, 5, 0, 0,  3,  1, 0, 1, 0,
 PATH_ABJURE, "enchantment",},
{"curse",                        2, 8,  0, 5, 0, 0,  2,  1, 0, 1, 0,
 PATH_ABJURE, "enchantment",},
{"regeneration",                 7, 15,  0, 10, 0, 0, 1,  1, 0, 1, 0,
 PATH_ABJURE, "enchantment",}, 
/* consecrate spell has no use w/o MULTIPLE_GODS defined. We make book=0 
 * in that case -b.t. */
#ifdef MULTIPLE_GODS
{"consecrate",                    4, 35,  0, 50, 0, 0, 1,  1, 0, 1, 0,
 PATH_ABJURE, "enchantment",},
#else
{"consecrate",                    4, 35,  0, 50, 0, 0, 0,  1, 0, 1, 0,
 PATH_ABJURE, "enchantment",},
#endif
{"summon cult monsters",          3, 12,  0, 10, 0, 0, 2,  1, 0, 1, 0,
 PATH_SUMMON, NULL,},
{"cause critical wounds",	  7, 25,   0,  5, 0, 0, 0,  1, 0, 1, 0,
 PATH_WOUNDING,"cause_wounds",}, 
{"holy wrath",	  		  14, 40,   0,  5, 0, 0, 1,  1, 0, 1, 0,
 PATH_TURNING,"holy_wrath",},                                /* 150 */
{"retributive strike",		  26, 150,   0, 15, 0, 0, 0,  1, 0, 1, 0,
 PATH_WOUNDING,"god_power",}, 
{"finger of death",	  	  15, 50,   0,  5, 0, 0, 0,  1, 0, 1, 0,
 PATH_DEATH, NULL,}, 
{"insect plague",	  	  12, 40,   0,  5, 0, 0, 0,  1, 0, 1, 0,
 PATH_SUMMON,"insect_plague",}, 
{"call holy servant",	  	  5, 30,   0,  5, 0, 0, 3,  1, 0, 1, 0,
 PATH_SUMMON, "holy_servant",}, 
{"wall of thorns",	  	  6, 20,   0, 5, 0, 0, 0,  0, 0, 1, 0,
 PATH_CREATE, "thorns"}, 
{"staff to snake",	  	  2, 8,   0, 5, 0, 0, 1,  1, 0, 1, 0,
 PATH_CREATE, "snake_golem"},
{"light",              	 	1, 8, 20, 5, 0, 0, 3,  1, 0, 1, 0,
 PATH_LIGHT, "light"},
{"darkness",          		5, 15, 10, 5, 0, 0, 1,  1, 0, 1, 0,
 PATH_LIGHT, "darkness"},
{"nightfall",	         	16, 120,  0, 15, 0, 0, 0,  1, 0, 1, 0,
 PATH_LIGHT, NULL},
{"daylight",            	18, 120,  0, 15, 0, 0, 0,  1, 0, 1, 0,
  PATH_LIGHT, NULL},                                            /* 160 */
{"sunspear",                    6, 8, 35, 8, 0, 0,  0,  1, 0, 1, 0,
 PATH_LIGHT, "sunspear"},
{"faery fire",                	4, 10,  0, 15, 3, 2, 2,  1, 0, 0, 0,
 PATH_LIGHT, NULL},
{"cure blindness",              9, 30,  0, 10, 1, 1, 2,  1, 1, 1, 1,
 PATH_RESTORE, "healing",},
{"dark vision",                 5, 10,  0, 12, 3, 2, 2,  1, 0, 0, 0,
 PATH_INFO, NULL},
{"bullet swarm",		7,  6,  0,  5, 0, 0, 1,  1, 0, 0, 0,
   PATH_MISSILE,"bullet"},
{"bullet storm",	       10,  8,  0,  5, 0, 0, 1,  1, 0, 0, 0,
   PATH_MISSILE,"lbullet"},
{"cause many wounds",	       12,  30,  0,  5, 0, 0, 0,  1, 0, 1, 0,
   PATH_WOUNDING,"cause_wounds"},
{"small snowstorm",     		 1, 6, 40, 5, 0, 0,  8,  1, 0, 0, 0,
 PATH_FROST, "snowball_s",},
{"medium snowstorm",    		 3,10, 20, 10, 0, 0,  6,  1, 0, 0, 0,
 PATH_FROST, "snowball_m",},
{"large snowstorm",     		 5,16, 10, 15, 0, 0,  2,  1, 0, 0, 0,
 PATH_FROST, "snowball_l",},					/* 170 */
{"cure disease",               5,30,  0, 10, 3, 5,  1,  0, 0, 1, 1,
 PATH_RESTORE,NULL},
{"cause red death",           12,100, 0, 10, 0, 0,  0,  1, 0, 1, 0,
 PATH_WOUNDING,"ebola"},
{"cause flu",                  2, 10, 5, 10, 3, 2,  5,  1, 0, 1, 0,
 PATH_WOUNDING,"flu"},
{"cause black death",         15, 120, 0, 10, 0, 0,  0,  1, 0, 1, 0,
 PATH_NULL,"bubonic_plague"},
{"cause leprosy",              5, 20, 0, 10, 1, 1,  5,  1, 0, 1, 0,
 PATH_WOUNDING,"leprosy"},
{"cause smallpox",            10, 85, 0, 10, 0, 0,  0,  1, 0, 1, 0,
 PATH_WOUNDING,"smallpox"},
{"cause white death",              85,350, 0, 10, 0, 0,  0,  1, 0, 1, 0,
 PATH_WOUNDING,"pneumonic_plague"},
{"cause anthrax",             12, 50, 0, 10, 1, 1,  1,  1, 0, 1, 0,
 PATH_WOUNDING,"anthrax"},
{"cause typhoid",             8, 60, 0, 10, 1, 1,  1,  1, 0, 1, 0,
   PATH_WOUNDING,"typhoid"},
{"mana blast",		       8, 10, 0, 15, 0, 0,  2,  1, 0, 0, 0,
   PATH_TRANSFER, "manablast", },					/* 180 */
{"small manaball",	       4, 12, 0,  9, 0, 0,  3,  1, 0, 0, 0,
   PATH_TRANSFER, "manabullet_s", },
{"medium manaball",	       7, 20, 0, 18, 0, 0,  2,  1, 0, 0, 0,
   PATH_TRANSFER, "manabullet_m", },
{"large manaball",	      10, 32, 0, 27, 0, 0,  1,  1, 0, 0, 0,
   PATH_TRANSFER, "manabullet_l", },
{"mana bolt",		       5, 18, 0,  9, 0, 0,  2,  1, 0, 0, 0,
   PATH_TRANSFER, "manabolt", },
{"dancing sword",	      11, 25, 0, 10, 0, 0,  1,  0, 0, 0, 0,    /* 185 */
   PATH_CREATE, "dancingsword", },
{"animate weapon",	       7, 25, 0, 10, 0, 0,  4,  0, 0, 0, 0,
   PATH_TELE, "dancingsword", },
{"cause cold",                  2, 10, 5, 10, 3, 2,  5,  1, 0, 1, 0,   /* 187 */
   PATH_WOUNDING,"disease_cold"},
{"divine shock",              1, 3, 0, 10, 0, 0, 0, 1, 0, 1, 0, 
   PATH_WOUNDING,"divine_shock"},
{"windstorm",                   3,3,  0, 10, 0, 0,  0,  1, 0, 1, 0,
 PATH_NULL, "windstorm",},
{"sanctuary",                 7, 30,  0, 10,  0,  0,  0,  0,  1,  1,  1,  /* 190 */
   PATH_PROT,"sanctuary"},
{"peace",                 20, 80,  0, 10,  0,  0,  0,  1,  0,  1,  0,
   PATH_PROT,"peace"},
{"spiderweb",              4, 10,  0, 10,  0,  0,  0,  1,  0,  1,  0,
   PATH_CREATE,"spiderweb_spell"},
{"conflict",              10, 50,  0, 10,  0,  0,  0,  1,  0,  1,  0,
   PATH_MIND, "enchantment"},
{"rage",                   1,  5,  0, 10,  0,  0,  0,  1,  0,  1,  1,
   PATH_WOUNDING, "enchantment"},
{"forked lightning",       5, 15,  0, 10,  0,  0,  0,  1,  0,  1,  0,   /* 195 */
   PATH_ELEC, "forked_lightning"},
{"poison fog",             5, 15,  0, 10,  0,  0,  0,  1,  0,  1,  0,
   PATH_WOUNDING, "poison_fog"},
{"flaming aura",           1,  5,  0, 10,  0,  0,  0,  0,  1,  1,  1,
   PATH_DETONATE, "flaming_aura"},
{"vitriol",                5, 15,  0, 10,  0,  0,  0,  0,  1,  1,  1,
   PATH_DETONATE, "vitriol"},  /* not for people to cast. */
{"vitriol splash",                5, 15,  0, 10,  0,  0,  0,  0,  1,  1,  1,
   PATH_DETONATE, "vitriol_splash"},  /* not for people to cast. */
{"ironwood skin",	 1, 8,  0, 20, 3, 2,  0,  1, 1, 1, 1,   /* 200 */
   PATH_SELF, "enchantment",},
{"wrathful eye",	 5, 30,  0, 20, 0, 0,  0,  1, 0, 1, 0,   
   PATH_SELF, "wrathful_eye",}
};

/*  peterm:  the following defines the parameters for all the
spells. 
  bdam:  base damage or hp of spell or summoned monster
  bdur:  base duration of spell or base range
  ldam:  levels you need over the min for the spell to gain one dam
  ldur:  levels you need over the min for the spell to gain one dur
  spl:   number of levels beyond minimum for spell point cost to
	 increase by amount equal to base cost.  i.e. if base cost
	 is 10 at level 2 and spl is 5, cost will increase by 2 per
	 level.  if base cost is 5 and spl is 10, cost increases by
	 1 every 2 levels.
    the information here is default only.  It is over-ridden by
    entries in LIBDIR/spell_params of the form.  Please
    see spell_params.doc

*/

spell_parameters SP_PARAMETERS[NROFREALSPELLS] =
{
/*  bdam    bdur    ldam    ldur	spl	spell  */
{   10,     0,      1,      0,		6},	/*magic bullet*/         /* 0 */
{   8,      6,      3,      0,		24},	/*small fireball*/       
{   8,      10,     3,      0,		24},	/*medium fireball*/      
{   8,      14,     4,      0,		32},	/*large fireball*/       
{   4,      5,      4,      4,		9},	/*burning hands*/        
{   8,      8,      3,      0,		24},	/*small lightning*/      
{   8,      16,     3,      0,		24},	/*large lightning*/      
{   9,      0,      1,      0,		10},	/*magic missile*/        
{   12,     8,      6,      0,		72},	/*create bomb*/          
{   8,      75,     1,      1,		15},	/*summon golem*/         
{   20,     200,    1,      1,		15},	/*summon fire elemental*/   /* 10 */
{   50,     280,    1,      1,		15},	/*summon earth elemental*/
{   40,     140,    1,      1,		15},	/*summon water elemental*/
{   10,     160,    1,      1,		15},	/*summon air elemental*/ 
{   0,      0,      0,      0,		0},	/*dimension door*/       
{   0,      32,     0,      1,		0},	/*create earth wall*/    
{   25,     5,      3,      3,		0},	/*paralyze*/             
{   4,      6,      3,      5,		9},	/*icestorm*/             
{   0,      0,      0,      0,		0},	/*magic mapping*/        
{   0,      3,      0,      4,		12},	/*turn undead*/ /* wis bonus on bdur*/
{   5,      4,      0,      3,		12},	/*fear*/ /*cha bonus on bdur */  /*20*/
{   0,      0,      3,      0,		8},	/*poison cloud*/         
{   0,      0,      0,      0,		0},	/*wonder*/               
{   10,     5,      3,      6,		15},	/*destruction*/ /* dam=10+int */
{   0,      0,      0,      0,		0},	/*perceive self*/
{   0,      5,      0,      1,		40},	/*word of recall*/       
{   0,      300,    20,     1,		15},	/*invisible*/            
{   0,      300,    20,     1,		15},	/*invisible to undead*/  
{   0,      0,      0,      0,		0},	/*probe*/                
{   25,     0,      1,      0,		25},	/*large bullet*/         
{   0,      200,    20,     1,		10},	/*improved invisibility*/   /*30*/
{   6,      2,      3,      5,		7},	/*holy word*/   /* bdur+=turn bonus */
{   0,      0,      0,      0,		0},	/*minor healing*/        
{   0,      0,      0,      0,		0},	/*medium healing*/       
{   0,      0,      0,      0,		0},	/*major healing*/        
{   0,      0,      0,      0,		0},	/*heal*/                 
{   100,    0,      1,      0,		10},	/*create food*/          
{   0,      1,      0,      3,		3},	/*earth to dust*/        
{   0,      0,      5,      4,		8},	/*armour*/               
{   0,      0,      0,      4,		0},	/*strength*/             
{   0,      0,      0,      4,		0},	/*dexterity*/               /*40*/
{   0,      0,      0,      4,		0},	/*constitution*/         
{   0,      0,      0,      4,		0},	/*charisma*/             
{   4,      260,    3,      1,		8},	/*create fire wall*/     
{   0,      240,    3,      1,		8},	/*create frost wall*/    
{   0,      0,      0,      4,		40},	/*protection from cold*/ 
{   0,      0,      0,      4,		40},	/*protection from electricity*/
{   0,      0,      0,      4,		40},	/*protection from fire*/ 
{   0,      0,      0,      4,		40},	/*protection from poison*/
{   0,      0,      0,      4,		40},	/*protection from slow*/ 
{   0,      0,      0,      4,		40},	/*protection from paralysis*//*50*/
{   0,      0,      0,      4,		40},	/*protection from draining*/
{   0,      0,      0,      4,		40},	/*protection from magic*/
{   0,      0,      0,      4,		40},	/*protection from attack*/
{   0,      0,      0,      4,		40},	/*levitate*/             
{   0,      0,      0,      0,		0},	/*small speedball*/      
{   0,      0,      0,      0,		0},	/*large speedball*/      
{   8,      12,     4,      0,		24},	/*hellfire*/             
{   4,      14,     3,      5,		11},	/*dragonbreath*/         
{   4,      14,     3,      5,		11},	/*large icestorm*/       
{   0,      0,      0,      0,		0},	/*charging*/                /* 60 */
{   0,      0,      0,      0,		0},	/*polymorph*/            
{   0,      0,      0,      0,		0},	/*cancellation*/         
{   0,      0,      0,      0,		0},	/*confusion*/            
{   0,      5,      0,      5,		25},	/*mass confusion*/       
{   0,      0,      0,      0,		0},	/*summon pet monster*/   
{   5,      5,      0,      4,		20},	/*slow*/                 
{   0,      0,      0,      0,		0},	/*regenerate spellpoints*/
{   0,      0,      0,      0,		0},	/*cure poison*/          
{   0,      0,      0,      4,		40},	/*protection from confusion*/
{   0,      0,      0,      4,		40},	/*protection from cancellation*/ /*70*/
{   0,      0,      0,      4,		40},	/*protection from depletion*/
{   0,      0,      0,      0,		0},	/*alchemy*/              
{   0,      0,      0,      0,		16},	/*remove curse*/              
{   0,      0,      0,      0,		30},	/*remove damnation*/              
{   0,      0,      0,      0,		0},	/*identify*/              
{   0,      0,      0,      0,		0},	/*detect magic*/              
{   0,      0,      0,      0,		0},	/*detect monster*/              
{   0,      0,      0,      0,		0},	/*detect evil*/              
{   0,      0,      0,      0,		0},	/*detect curse*/              
{   0,      0,      0,      0,		0},	/*heroism */			/*80*/
{   0,      0,      0,      0,		0},	/*aggravation*/              
{   10,     9,      3,      0,		30},	/*firebolt*/              
{   12,     11,     4,      0,		48},	/*frostbolt*/              
{   10,     18,     3,      5,		25},	/*shockwave*/            
{   8,      10,     3,      5,		16},	/*color spray*/          
{   0,      0,      5,      2,		0},	/*haste*/
{   0,      4,      0,      9,		36},	/*face of death*/        
{   8,      40,     3,      1,		15},	/*ball lightning*/       
{   0,      0,      0,      4,		12},	/*meteor swarm*/         
{   75,     0,      1,      0,		18},	/*comet*/			/*90*/ 
{   8,      50,     1,      1,		15},	/*mystic fist*/
{   0,      0,      0,      0,		0},	/*raise dead*/
{   0,      0,      0,      0,		0},	/*resurrection*/
{   0,      0,      0,      0,		0},	/*reincarnation*/
{   0,      0,      0,      5,		50},	/*immunity to cold*/
{   0,      0,      0,      5,		50},	/*immunity to electricity*/
{   0,      0,      0,      5,		50},	/*immunity to fire*/
{   0,      0,      0,      5,		50},	/*immunity to poison*/
{   0,      0,      0,      5,		50},	/*immunity to slow*/
{   0,      0,      0,      5,		50},	/*immunity to paralysis*/	/*100*/
{   0,      0,      0,      5,		50},	/*immunity to draining*/
{   0,      0,      0,      5,		50},	/*immunity to magic*/
{   0,      0,      0,      5,		50},	/*immunity to attack*/
{   0,      0,      0,      5,		50},	/*invulnerability*/
{   0,      0,      0,      5,		50},	/*protection*/
{   0,      0,      0,      0,		0},	/* fire rune */
{   0,      0,      0,      0,		0},	/* frost rune */
{   0,      0,      0,      0,		0},	/* shocking rune */
{   0,      0,      0,      0,		0},	/* blasting rune */
{   0,      0,      0,      0,		0},	/* death rune */		/*110*/
{   0,      0,      0,      0,		0},	/* marking rune */
{   80,     200,    1,      1,		10},	/* build director */
{   4,      240,    3,      1,		0},	/* chaos pool */
{   80,     100,    1,      1,		10},	/* build bullet wall */
{   80,     100,    1,      1,		10},	/* build lightning wall */
{   80,     100,    1,      1,		10},	/* build fireball wall */
{   0,      0,      0,      0,		0},	/* magic rune */
{   0,      0,      0,      0,		0},	/* rune of magic drain */
{   0,      0,      0,      0,		0},	/* rune of barring magic */
{   8,      0,      2,      0,		24},	/* rune of transferrence */	/*120*/
{   8,      0,      2,      0,		20},	/* transferrence */
{   75,     30,     2,      1,		10},	/* magic drain */
{   0,      6,      0,      3,		18},	/* counter-spell */
{   0,      0,      0,      0,		0},	/* dispel rune */
{   0,      0,      0,      0,		0},	/* cure madness */
{   0,      0,      0,      0,		0},	/* restoration */
{   0,      0,      0,      0,		0},	/* summon hostile monsters */
{   0,     100,     0,      1,		20},	/* counterwall */
{   6,      0,	    1,	    0,		6},	/* cause light wounds */
{  24,      0,	    1,	    0,		24},	/* cause medium wounds */	/* 130 */ 
{  50,      0,	    1,	    0,		50},	/* cause serious wounds */
{   0,      0,     0,      3 ,      27},     /*  Charm monster */
{   0,      4,	    0,      9, 		36},	/* Banishment */
{   0,	   5,     6,      2,       5},    /* create missile */
{   0,      4,	    0,      0,		30},	/* show invisible */
{   0,      4,	    0,      9,		36},	/* xray vision */
{   0,	    0,      0,	    10,		10},    /* pacify */
{   0,	    0,      0,	   10,		 0},    /* summon fog */
{   20,     8,     2,      5,		40}, 	/* steambolt*/              
{   0,      0,     0,      3 ,      	27},    /* command undead */		/* 140 */ 
{   25,     8,      4,     0,           10},	/* holy orb */ 
{   50,     300,    1,      1,		15},	/* summon avatar */  
{   1,	    0,      4,	    4,		10},    /* holy possesion */
{   1,	    0,      8,	    6,		10},    /* bless */
{   1,	    0,      8,	    6,		10},    /* curse */
{   1,	    0,      5,	    6,		10},    /* regeneration */
{   0,	    0,      0,	    0,		0},    /* consecrate  */
{   0,      0,      0,      0,		0},	/* summon cult monster*/   
{  100,      0,	    1,	    0,	        50},	/* cause critical wounds */
{    0,      0,     1,      0,		30},	/* holy wrath */		/* 150 */  
{    0,      0,	    1,	    0,	        30}, 	/* finger of retribution */
{    0,      0,     1,      0,		20},	/* finger of death */   
{    0,      0,     1,      0,		30},	/* insect plague */   
{   10,     50,     2,      2,		50},	/* holy servant */   
{    4,    300,     3,      1,		50},	/* wall of thorns */   
{    8,     30,     3,      3,		40},	/* staff to snake */   
{    2,   1000,    20,      1,		10},	/* light */   
{   10,     50,     2,      2,		50},	/* darkness */   
{   10,     50,     2,      2,		50},	/* nightfall */  		
{   10,     50,     2,      2,		50},	/* daylight */     /* 160 */ 
{    6,      8,     3,      9,          35},    /* sunspear */
{    1,      5,     9,      6,          15},	/* faery fire */
{    0,      0,     0,      0,		0},	/* cure blindness */   
{    0,      4,     0,      5,		30},	/* dark vision */   
{    0,      0,     0,      4,		12},    /* bullet swarm */
{    0,      0,     0,      4,		12},	/* bullet storm */
{    0,      0,     0,      4,		12},	/* cause many wounds  */
{   8,      6,      3,      0,		24},	/*small snowstorm*/       
{   8,      10,     3,      0,		24},	/*medium snowstorm*/      
{   8,      14,     4,      0,		32},	/*large snowstorm*/     /* 170 */ 
{   0,       0,     0,      0,		0},	/*cure disease*/      
{   0,       0,     1,      1,		24},	/*cause red death*/      
{   0,       0,     3,      5,		10},	/*cause flu*/      
{   0,       0,     3,      10,		20},	/*cause black death*/      
{   0,       0,     1,      1,		10},	/*cause leprosy*/      
{   0,       0,     1,      4,		10},	/*cause smallpox*/      
{   0,       0,     1,      5,		24},	/*cause plague*/      
{   0,       0,     1,      10,		10},	/*cause anthrax*/      
{   0,       0,     1,      10,		24},	/*cause typhoid*/      
{	4,	5,	4,	4,	9},	/* mana blast */	/* 180 */ 
{	8,	6,	3,	0,	24},	/* small manaball */
{	8,	10,	3,	0,	24},	/* medium manaball */
{	8,	14,	4,	0,	32},	/* large manaball */
{	10,	9,	3,	0,	30},	/* mana bolt */
{	2,	20,	2,	1,	10},	/* dancing sword */     /* 185 */ 
{	2,	20,	2,	1,	20},	/* animate weapon */
{   0,       0,     3,      5,		10},	/*cause cold*/      
{   1,      1,     9,      3,		7},	/*divine shock*/      
{   0,      20,     25,      4,		8},	/*windstorm*/      
{   0,     100,      0,      1,		20},	/*sanctuary*/    /* 190 */     
{   0,      4,       0,      9,		36},	/*peace*/      
{   0,      10,     0,       5,		50},	/*spiderweb*/      
{   0,      5,      0,      10,		10},	/*conflict*/      
{   0,       0,     5,      4,		4},	/*rage*/      
{   10,     16,     3,      0,		24},	/*forked lightning*/      /* 195*/
{   10,     5,     5,      4,		10},	/*poison fog*/      
{   6,      100,     25,      4,	20},	/*flame aura*/      
{   100,    20,     25,      4,		6},	/*vitriol*/      
{   40,      5,      4,      40,	10},	/*vitriol splash*/        
{   0,      0,      5,      4,		6},	/*ironwood skin*/ 
{   20,      5,    3,      5,		7},	/*wrathful eye*/ 
};

char *spellpathnames[NRSPELLPATHS] = {
 "Protection",
 "Fire",
 "Frost",
 "Electricity",
 "Missiles",
 "Self",
 "Summoning",
 "Abjuration",
 "Restoration",
 "Detonation",
 "Mind",
 "Creation",
 "Teleportation",
 "Information",
 "Transmutation",
 "Transferrence",
 "Turning",
 "Wounding",
 "Death",
 "Light"
};


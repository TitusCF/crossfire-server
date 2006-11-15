/*
 * static char *rcsid_newclient_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002,2006 Mark Wedel & Crossfire Development Team
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

/* This file defines various flags that both the new client and
 * newserver uses.  These should never be changed, only expanded.
 * Changing them will likely cause all old clients to not work properly.
 * While called newclient, it is really used by both the client and
 * server to keep some values the same.
 *
 * Name format is CS_(command)_(flag)
 * CS = Client/Server.
 * (command) is protocol command, ie ITEM
 * (flag) is the flag name
 */

/* Ideally, this file should be the same between the server and
 * the client.  However, this often drifts apart because of a value
 * that is only useful on the client and server.  Generally, it isn't
 * a problem to have a few extra #defines if this lets them stay
 * in sync.
 *
 * Given this file contains the constants that dictate what is sent
 * between the server and client, keeping them in sync makes doing changes
 * easier - modify this file in one place, copy it over.
 *
 */


#ifndef NEWCLIENT_H
#define NEWCLIENT_H

/* MAXSOCKRECVBUF and MAXSOCKSENDBUF are used on the server
 * MAXSOCKBUF is used by the client.
 */

/* Maximum size of any packet we expect. This number includes both the length
 * bytes (2 bytes) at the start of each packet and the trailing '\0' (1 byte)
 * at the end of each packet.
 */
#define MAXSOCKRECVBUF (2+65535+1)

/* Maximum size of any packet we send. This number does not include the length
 * bytes at the start of each packet. The value is chosen to not overflow the
 * input buffer of old clients (2006-05-21).
 */
#define MAXSOCKSENDBUF 10239

/* Maximum size of any packet we expect.  Using this makes it so we don't need to
 * allocate and deallocate the same buffer over and over again and the price
 * of using a bit of extra memory.  It also makes the code simpler.
 * The size is big enough to receive any valid packet: 2 bytes for length,
 * 65535 for max. packet size, 1 for appended trailing '\0'.
 */
#define MAXSOCKBUF (2+65535+1)


/* How much the x,y coordinates in the map2 are off from
 * actual upper left corner.  Necessary for light sources
 * that may be off the edge of the visible map.
 */
#define MAP2_COORD_OFFSET   15


#define CS_QUERY_YESNO	0x1	/* Yes/no question */
#define CS_QUERY_SINGLECHAR 0x2	/* Single character response expected */
#define CS_QUERY_HIDEINPUT 0x4	/* Hide input being entered */

#define CS_SAY_NORMAL	0x1	/* Normal say command */
#define CS_SAY_SHOUT	0x2	/* Text is shouted. */
#define CS_SAY_GSAY	0x4	/* Text is group say command */

/* These are multiplication values that should be used when changing 
 * floats to ints, and vice version.  MULTI is integer representatin
 * (float to int), MULTF is float, for going from int to float.
 */
#define FLOAT_MULTI	100000
#define FLOAT_MULTF	100000.0

/* ID's for the various stats that get sent across. */

#define CS_STAT_HP	 1
#define CS_STAT_MAXHP	 2
#define CS_STAT_SP	 3
#define CS_STAT_MAXSP	 4
#define CS_STAT_STR	 5
#define CS_STAT_INT	 6
#define CS_STAT_WIS	 7
#define CS_STAT_DEX	 8
#define CS_STAT_CON	 9
#define CS_STAT_CHA	10
#define CS_STAT_EXP	11
#define CS_STAT_LEVEL	12
#define CS_STAT_WC	13
#define CS_STAT_AC	14
#define CS_STAT_DAM	15
#define CS_STAT_ARMOUR	16
#define CS_STAT_SPEED	17
#define CS_STAT_FOOD	18
#define CS_STAT_WEAP_SP 19
#define CS_STAT_RANGE	20
#define CS_STAT_TITLE	21
#define CS_STAT_POW	22
#define CS_STAT_GRACE	23
#define CS_STAT_MAXGRACE	24
#define CS_STAT_FLAGS	25
#define CS_STAT_WEIGHT_LIM	26
#define CS_STAT_EXP64	28
#define CS_STAT_SPELL_ATTUNE	29
#define CS_STAT_SPELL_REPEL	30
#define CS_STAT_SPELL_DENY	31

/* Start & end of resistances, inclusive. */
#define CS_STAT_RESIST_START	100
#define CS_STAT_RESIST_END	117
#define CS_STAT_RES_PHYS	100
#define CS_STAT_RES_MAG		101
#define CS_STAT_RES_FIRE	102
#define CS_STAT_RES_ELEC	103
#define CS_STAT_RES_COLD	104
#define CS_STAT_RES_CONF	105
#define CS_STAT_RES_ACID	106
#define CS_STAT_RES_DRAIN	107
#define CS_STAT_RES_GHOSTHIT	108
#define CS_STAT_RES_POISON	109
#define CS_STAT_RES_SLOW	110
#define CS_STAT_RES_PARA	111
#define CS_STAT_TURN_UNDEAD	112
#define CS_STAT_RES_FEAR	113
#define CS_STAT_RES_DEPLETE	114
#define CS_STAT_RES_DEATH	115
#define CS_STAT_RES_HOLYWORD	116
#define CS_STAT_RES_BLIND	117

/* Start & end of skill experience + skill level, inclusive. */
#define CS_STAT_SKILLEXP_START 118
#define CS_STAT_SKILLEXP_END 129
#define CS_STAT_SKILLEXP_AGILITY 118
#define CS_STAT_SKILLEXP_AGLEVEL 119
#define CS_STAT_SKILLEXP_PERSONAL 120
#define CS_STAT_SKILLEXP_PELEVEL 121
#define CS_STAT_SKILLEXP_MENTAL 122
#define CS_STAT_SKILLEXP_MELEVEL 123
#define CS_STAT_SKILLEXP_PHYSIQUE 124
#define CS_STAT_SKILLEXP_PHLEVEL 125
#define CS_STAT_SKILLEXP_MAGIC 126
#define CS_STAT_SKILLEXP_MALEVEL 127
#define CS_STAT_SKILLEXP_WISDOM 128
#define CS_STAT_SKILLEXP_WILEVEL 129

/* CS_STAT_SKILLINFO is used as the starting index point.  Skill number->name
 * map is generated dynamically for the client, so a bunch of entries will
 * be used here.  CS_NUM_SKILLS does not match how many skills there really
 * are - instead, it is used as a range of values so that the client
 * can have some idea how many skill categories there may be.
 */
#define CS_STAT_SKILLINFO   140
#define CS_NUM_SKILLS	     50

/* These are used with CS_STAT_FLAGS above to communicate S->C what the
 * server thinks the fireon & runon states are.
 */
#define SF_FIREON   0x01
#define SF_RUNON    0x02

/* The following are the color flags passed to new_draw_info.
 *
 * We also set up some control flags
 *
 * NDI = New Draw Info
 */

/* Color specifications - note these match the order in xutil.c */
/* Note 2:  Black, the default color, is 0.  Thus, it does not need to
 * be implicitly specified.
 */
#define NDI_BLACK	0
#define NDI_WHITE	1
#define NDI_NAVY	2
#define NDI_RED		3
#define NDI_ORANGE	4
#define NDI_BLUE	5	/* Actually, it is Dodger Blue */
#define NDI_DK_ORANGE	6	/* DarkOrange2 */
#define NDI_GREEN	7	/* SeaGreen */
#define NDI_LT_GREEN	8	/* DarkSeaGreen,  which is actually paler */
				/* Than seagreen - also background color */
#define NDI_GREY	9
#define NDI_BROWN	10	/* Sienna */
#define NDI_GOLD	11
#define NDI_TAN		12	/* Khaki */

#define NDI_MAX_COLOR	12	/* Last value in */
#define NDI_COLOR_MASK	0xff	/* Gives lots of room for expansion - we are */
				/* using an int anyways, so we have the space */
				/* to still do all the flags */


#define NDI_UNIQUE	0x100	/* Print this out immediately, don't buffer */
#define NDI_ALL		0x200	/* Inform all players of this message */

/* Flags for the item command */
enum {a_none, a_readied, a_wielded, a_worn, a_active, a_applied};
#define F_APPLIED       0x000F
#define F_LOCATION      0x00F0
#define F_UNPAID        0x0200
#define F_MAGIC         0x0400
#define F_CURSED        0x0800
#define F_DAMNED        0x1000
#define F_OPEN          0x2000
#define F_NOPICK        0x4000
#define F_LOCKED        0x8000

#define CF_FACE_NONE	0
#define CF_FACE_BITMAP	1
#define CF_FACE_XPM	2
#define CF_FACE_PNG	3
#define CF_FACE_CACHE	0x10

/* Used in the new_face structure on the magicmap field.  Low bits
 * are color informatin.  For now, only high bit information we need
 * is floor information.
 */
#define FACE_FLOOR	0x80
#define FACE_WALL	0x40	/* Or'd into the color value by the server
				 * right before sending.
				 */
#define FACE_COLOR_MASK	0xf

#define UPD_LOCATION	0x01
#define UPD_FLAGS	0x02
#define UPD_WEIGHT	0x04
#define UPD_FACE	0x08
#define UPD_NAME	0x10
#define UPD_ANIM	0x20
#define UPD_ANIMSPEED	0x40
#define UPD_NROF	0x80

/* The flag values sent by the updspell command */
#define UPD_SP_MANA	0x01
#define UPD_SP_GRACE	0x02
#define UPD_SP_DAMAGE	0x04

#define SOUND_NORMAL	0
#define SOUND_SPELL	1

#define FACE_IS_ANIM    1<<15
#define ANIM_RANDOM	1<<13
#define ANIM_SYNC	2<<13

/* ANIM_FLAGS_MASK and ANIM_MASK are only used by the client */
#define ANIM_FLAGS_MASK	0x6000

/* AND'ing this with data from server gets us just the animation id */
#define ANIM_MASK	0x1fff

 
/* Constants in the form EMI_ is for extended map infos.
 * Even if the client select the additionnal infos it wants
 * on the map, there may exist cases where this whole info
 * is not given in one buch but in separate bunches. This 
 * is done performance reasons (imagine some info related to
 * a visible object and another info related to a 4 square
 * width and height area). At the begin of an extended info packet
 * is a bit field. A bit is activated for each extended info
 * present in the data 
 */
/* Meanings:
 * EMI_NOREDRAW  Take extended infos into account but don't redraw,
 *               some additionnal datas will follow in a new packet
 * EMI_SMOOTH    Datas about smoothing  
 */ 
#define EMI_NOREDRAW        0x01  
#define EMI_SMOOTH          0x02

/* this last one says the bitfield continue un next byte
 * There may be several on contiguous bytes. So there is 7
 * actual bits used per byte, and the number of bytes
 * is not fixed in protocol
 */
#define EMI_HASMOREBITS     0x80

 
/*
 * Note!
 * If you add message types here, don't forget
 * to keep the client up to date too!
 */
 
  
/* message types */
#define MSG_TYPE_BOOK		    1
#define MSG_TYPE_CARD		    2
#define MSG_TYPE_PAPER		    3
#define MSG_TYPE_SIGN		    4
#define MSG_TYPE_MONUMENT	    5
#define MSG_TYPE_DIALOG		    6
#define MSG_TYPE_MOTD		    7
#define MSG_TYPE_ADMIN		    8
#define MSG_TYPE_SHOP		    9
#define MSG_TYPE_COMMAND	    10	/* Responses to commands, eg, who */
#define MSG_TYPE_ATTRIBUTE	    11	/* Changes to attributes (stats, */
					/* resistances, etc) */
#define MSG_TYPE_SKILL		    12	/* Messages related to using skills */
#define MSG_TYPE_APPLY		    13	/* Applying objects */
#define MSG_TYPE_ATTACK		    14	/* Attack related messges */
#define MSG_TYPE_COMMUNICATION	    15	/* Communication between players */
#define MSG_TYPE_SPELL		    16	/* Spell related info */
#define MSG_TYPE_ITEM		    17	/* Item related information */
#define MSG_TYPE_MISC		    18	/* Messages that don't go anyplace else */
#define MSG_TYPE_VICTIM		    19	/* Something bad is happening to the player */
#define MSG_TYPE_LAST		    20

#define MSG_SUBTYPE_NONE         0

/* book messages subtypes */
#define MSG_TYPE_BOOK_CLASP_1    1
#define MSG_TYPE_BOOK_CLASP_2    2
#define MSG_TYPE_BOOK_ELEGANT_1  3
#define MSG_TYPE_BOOK_ELEGANT_2  4
#define MSG_TYPE_BOOK_QUARTO_1   5
#define MSG_TYPE_BOOK_QUARTO_2   6
#define MSG_TYPE_BOOK_SPELL_EVOKER    7
#define MSG_TYPE_BOOK_SPELL_PRAYER    8
#define MSG_TYPE_BOOK_SPELL_PYRO      9
#define MSG_TYPE_BOOK_SPELL_SORCERER  10
#define MSG_TYPE_BOOK_SPELL_SUMMONER  11

/* card messages subtypes*/
#define MSG_TYPE_CARD_SIMPLE_1    1
#define MSG_TYPE_CARD_SIMPLE_2    2
#define MSG_TYPE_CARD_SIMPLE_3    3
#define MSG_TYPE_CARD_ELEGANT_1   4
#define MSG_TYPE_CARD_ELEGANT_2   5
#define MSG_TYPE_CARD_ELEGANT_3   6
#define MSG_TYPE_CARD_STRANGE_1   7
#define MSG_TYPE_CARD_STRANGE_2   8
#define MSG_TYPE_CARD_STRANGE_3   9
#define MSG_TYPE_CARD_MONEY_1     10
#define MSG_TYPE_CARD_MONEY_2     11
#define MSG_TYPE_CARD_MONEY_3     12

/* Paper messages subtypes */
#define MSG_TYPE_PAPER_NOTE_1       1
#define MSG_TYPE_PAPER_NOTE_2       2
#define MSG_TYPE_PAPER_NOTE_3       3
#define MSG_TYPE_PAPER_LETTER_OLD_1 4
#define MSG_TYPE_PAPER_LETTER_OLD_2 5
#define MSG_TYPE_PAPER_LETTER_NEW_1 6
#define MSG_TYPE_PAPER_LETTER_NEW_2 7
#define MSG_TYPE_PAPER_ENVELOPE_1   8
#define MSG_TYPE_PAPER_ENVELOPE_2   9
#define MSG_TYPE_PAPER_SCROLL_OLD_1 10
#define MSG_TYPE_PAPER_SCROLL_OLD_2 11
#define MSG_TYPE_PAPER_SCROLL_NEW_1 12
#define MSG_TYPE_PAPER_SCROLL_NEW_2 13
#define MSG_TYPE_PAPER_SCROLL_MAGIC 14

/* road signs messages subtypes */
#define MSG_TYPE_SIGN_BASIC         1
#define MSG_TYPE_SIGN_DIR_LEFT      2
#define MSG_TYPE_SIGN_DIR_RIGHT     3
#define MSG_TYPE_SIGN_DIR_BOTH      4

/* stones and monument messages */
#define MSG_TYPE_MONUMENT_STONE_1      1
#define MSG_TYPE_MONUMENT_STONE_2      2
#define MSG_TYPE_MONUMENT_STONE_3      3
#define MSG_TYPE_MONUMENT_STATUE_1     4
#define MSG_TYPE_MONUMENT_STATUE_2     5
#define MSG_TYPE_MONUMENT_STATUE_3     6
#define MSG_TYPE_MONUMENT_GRAVESTONE_1 7
#define MSG_TYPE_MONUMENT_GRAVESTONE_2 8
#define MSG_TYPE_MONUMENT_GRAVESTONE_3 9
#define MSG_TYPE_MONUMENT_WALL_1       10
#define MSG_TYPE_MONUMENT_WALL_2       11
#define MSG_TYPE_MONUMENT_WALL_3       12

/* dialog messsage */
#define MSG_TYPE_DIALOG_NPC		1   /* A message from the npc */
#define MSG_TYPE_DIALOG_ALTAR		2   /* A message from an altar */
#define MSG_TYPE_DIALOG_MAGIC_MOUTH	3   /* Magic Mouth/Magic Ear */

/* MOTD doesn't have any subtypes */

/* admin/global messages */
#define MSG_TYPE_ADMIN_RULES	    1
#define MSG_TYPE_ADMIN_NEWS	    2
#define MSG_TYPE_ADMIN_PLAYER	    3	    /* Player coming/going/death */
#define MSG_TYPE_ADMIN_DM	    4	    /* DM related admin actions */
#define MSG_TYPE_ADMIN_HISCORE	    5	    /* Hiscore list */
#define MSG_TYPE_ADMIN_LOADSAVE	    6	    /* load/save operations */
#define MSG_TYPE_ADMIN_LOGIN	    7	    /* login messages/errors */
#define MSG_TYPE_ADMIN_VERSION	    8	    /* version info */


/* I'm not actually expecting anything to make much use of the MSG_TYPE_SHOP
 * values However, to use the media tags, need to use draw_ext_info, and need
 * to have a type/subtype, so figured might as well put in real values here.
 */
#define MSG_TYPE_SHOP_LISTING		1   /* Shop listings - inventory, */
					    /* what it deals in */
#define MSG_TYPE_SHOP_PAYMENT		2   /* Messages about payment, lack */
					    /* of funds */
#define MSG_TYPE_SHOP_SELL		3   /* Messages about selling items */
#define MSG_TYPE_SHOP_MISC		4   /* Random messages */

/* Basically, 1 subtype/command.  Like shops, not expecting much
 * to be done, but by having different subtypes, it makes it easier for
 * client to store way information (eg, who output)
 */
#define MSG_TYPE_COMMAND_WHO	    1
#define MSG_TYPE_COMMAND_MAPS	    2
#define MSG_TYPE_COMMAND_BODY	    3
#define MSG_TYPE_COMMAND_MALLOC	    4
#define MSG_TYPE_COMMAND_WEATHER    5
#define MSG_TYPE_COMMAND_STATISTICS 6
#define MSG_TYPE_COMMAND_CONFIG	    7	/* bowmode, petmode, applymode */
#define MSG_TYPE_COMMAND_INFO	    8	/* Generic info - resistances, etc */
#define MSG_TYPE_COMMAND_QUESTS	    9	/* Quest info */
#define MSG_TYPE_COMMAND_DEBUG	    10	/* Various debug type commands */
#define MSG_TYPE_COMMAND_ERROR	    11	/* Bad syntax/can't use command */
#define MSG_TYPE_COMMAND_SUCCESS    12	/* Successful result from command */
#define MSG_TYPE_COMMAND_FAILURE    13	/* Failed result from command */
#define MSG_TYPE_COMMAND_EXAMINE    14	/* Player examining something */
#define MSG_TYPE_COMMAND_INVENTORY  15	/* Inventory listing */
#define MSG_TYPE_COMMAND_HELP	    16	/* Help related information */
#define MSG_TYPE_COMMAND_DM	    17	/* DM related commands */
#define MSG_TYPE_COMMAND_NEWPLAYER  18	/* Create a new character - not */
					/* really a command, but is responding */
					/* to player input */

/* This is somewhat verbose.  If the client ends up being able to
 * choose various attributes based on message type, I think it is important
 * for the client to know if this is a benefit or detriment to the player.
 * In the case of losing a bonus, this typically indicates a spell has
 * ended, which is probably more important (and should be displayed more
 * prominently) than when you cast the spell
 */

#define MSG_TYPE_ATTRIBUTE_ATTACKTYPE_GAIN  1	/* Atacktypes here refer to */
#define MSG_TYPE_ATTRIBUTE_ATTACKTYPE_LOSS  2	/* the player gaining or */
						/* losing these attacktypes */
						/* not being a victim of an */
						/* attacktype. */
#define MSG_TYPE_ATTRIBUTE_PROTECTION_GAIN  3	/* Protections in this */
#define MSG_TYPE_ATTRIBUTE_PROTECTION_LOSS  4	/* context  are pretty */
						/* generic - things like */
						/* reflection or lifesave */
						/* are also under the */
						/* protection category. */
#define MSG_TYPE_ATTRIBUTE_MOVE		    5	/* A change in the movement */
						/* type of the player */
#define MSG_TYPE_ATTRIBUTE_RACE		    6	/* Racial related changes */
#define MSG_TYPE_ATTRIBUTE_BAD_EFFECT_START 7	/* The start/end of bad */
#define MSG_TYPE_ATTRIBUTE_BAD_EFFECT_END   8	/* effects to the player */
#define MSG_TYPE_ATTRIBUTE_STAT_GAIN	    9	/* Start/end of stat changes */
#define MSG_TYPE_ATTRIBUTE_STAT_LOSS	    10
#define MSG_TYPE_ATTRIBUTE_LEVEL_GAIN	    11	/* Gaining/losing of levels */
#define MSG_TYPE_ATTRIBUTE_LEVEL_LOSS	    12
#define MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_START 13	/* Like bad effects, but for */
#define MSG_TYPE_ATTRIBUTE_GOOD_EFFECT_END  14	/* good effects to the player */
#define MSG_TYPE_ATTRIBUTE_GOD		    15	/* changing god info */


/* I think one type/skill is overkill, so instead, use broader categories
 * for these messages.
 * The difference in ERROR vs FAILURE is basically this:  ERROR indicates
 * something wasn't right to even attempt to use the skill (don't have
 * needed object, or haven't marked objects, etc).
 * FAILURE indicates that player attempted to use the skill, but it
 * didn't work.
 * PRAY is listed out because praying over altars can generate some
 * messages not really related to the skill itself.
 */
#define MSG_TYPE_SKILL_MISSING	1	/* Don't have the skill */
#define MSG_TYPE_SKILL_ERROR	2	/* Doing something wrong */
#define MSG_TYPE_SKILL_SUCCESS	3	/* Successfully used skill */
#define MSG_TYPE_SKILL_FAILURE	4	/* Failure in using skill */
#define MSG_TYPE_SKILL_PRAY	5	/* Praying related messages */
#define MSG_TYPE_SKILL_LIST	6	/* List of skills */


/* Messages related to applying objects.  Note that applying many
 * objects may generate MSG_TYPE_ATTRIBUTE messages - the APPLY here
 * more directly related to the direct messages related to applying
 * them (you put on your armor, you apply scroll, etc).
 * The ERROR is like that for SKILLS - something prevent even trying
 * to apply the object.  FAILURE indicates result wasn't successful.
 */
#define MSG_TYPE_APPLY_ERROR	1
#define MSG_TYPE_APPLY_UNAPPLY	2   /* Unapply an object */
#define MSG_TYPE_APPLY_SUCCESS	3   /* Was able to apply object */
#define MSG_TYPE_APPLY_FAILURE	4   /* Apply OK, but no/bad result */
#define MSG_TYPE_APPLY_CURSED	5   /* Applied a cursed object (BAD) */
#define MSG_TYPE_APPLY_TRAP	6   /* Have activated a trap */
#define MSG_TYPE_APPLY_BADBODY	7   /* Don't have body to use object */
#define MSG_TYPE_APPLY_PROHIBITION 8   /* Class/god prohibiiton on obj */
#define MSG_TYPE_APPLY_BUILD	9   /* Build related actions */

/* attack related messages */
#define MSG_TYPE_ATTACK_DID_HIT	    1   /* Player hit something else */
#define MSG_TYPE_ATTACK_PET_HIT	    2   /* Players pet hit something else */
#define MSG_TYPE_ATTACK_FUMBLE	    3   /* Player fumbled attack */
#define MSG_TYPE_ATTACK_DID_KILL    4	/* Player killed something */
#define MSG_TYPE_ATTACK_PET_DIED    5	/* Pet was killed */
#define MSG_TYPE_ATTACK_NOKEY	    6	/* Keys are like attacks, so... */
#define MSG_TYPE_ATTACK_NOATTACK    7	/* You avoid attacking */
#define MSG_TYPE_ATTACK_PUSHED	    8	/* Pushed a friendly player */

#define	MSG_TYPE_COMMUNICATION_RANDOM	1   /* Random event (coin toss) */
#define	MSG_TYPE_COMMUNICATION_SAY	2   /* Player says something */
#define	MSG_TYPE_COMMUNICATION_ME	3   /* Player me's a message */
#define	MSG_TYPE_COMMUNICATION_TELL	4   /* Player tells something */
#define	MSG_TYPE_COMMUNICATION_EMOTE	5   /* Player emotes */
#define	MSG_TYPE_COMMUNICATION_PARTY	6   /* Party message */

#define MSG_TYPE_SPELL_HEAL		1   /* Healing related spells */
#define MSG_TYPE_SPELL_PET		2   /* Pet related messages */
#define MSG_TYPE_SPELL_FAILURE		3   /* Spell failure messages */
#define MSG_TYPE_SPELL_END		4   /* A spell ends */
#define MSG_TYPE_SPELL_SUCCESS		5   /* Spell succeeded messages */
#define MSG_TYPE_SPELL_ERROR		6   /* Spell failure messages */
#define MSG_TYPE_SPELL_PERCEIVE_SELF	7   /* Perceive self messages */
#define MSG_TYPE_SPELL_TARGET		8   /* Target of non attack spell */
#define MSG_TYPE_SPELL_INFO		9   /* random info about spell, not */
					    /* related to failure/success */

#define MSG_TYPE_ITEM_REMOVE		1   /* Item removed from inv */
#define MSG_TYPE_ITEM_ADD		2   /* Item added to inv */
#define MSG_TYPE_ITEM_CHANGE		3   /* Item has changed in some way */
#define MSG_TYPE_ITEM_INFO		3   /* Information related to an item */

/* MSG_TYPE_MISC, by its very nature, doesn't really have subtypes.  It is
 * used for messages that really don't belong anyplace else
 */

#define MSG_TYPE_VICTIM_SWAMP		1   /* Player is sinking in a swamp */
#define MSG_TYPE_VICTIM_WAS_HIT		2   /* Player was hit by something */
#define MSG_TYPE_VICTIM_STEAL		3   /* Someone tried to steal from the player */
#define MSG_TYPE_VICTIM_SPELL		4   /* Someone cast a bad spell on the player */
#define MSG_TYPE_VICTIM_DIED		5   /* Player died! */
#define MSG_TYPE_VICTIM_WAS_PUSHED	6   /* Player was pushed or attempted pushed */

/* Contains the base information we use to make up a packet we want to send. */
typedef struct SockList {
    int len;
    unsigned char *buf;
} SockList;

typedef struct CS_Stats {
    int	    ibytes;	/* ibytes, obytes are bytes in, out */
    int	    obytes;
    short   max_conn;	/* Maximum connections received */
    time_t  time_start;	/* When we started logging this */
} CS_Stats;


extern CS_Stats cst_tot, cst_lst;

#endif /* NEWCLIENT_H */

/*
 * static char *rcsid_player_h =
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

#define NUM_OUTPUT_BUFS	5
typedef struct {
    char *buf;			/* Actual string pointer */
    uint32 first_update;	/* First time this message was stored  */
    uint16 count;		/* How many times we got this message */
} Output_Buf;


/* wand/rod/horn rolled into range_misc.  They all use the same body location
 * anyways.
 */
typedef enum rangetype {
    range_bottom    = -1, 
    range_none	    = 0, 
    range_bow	    = 1,
    range_magic	    = 2,
    range_misc	    = 3, 
    range_golem     = 4,
    range_skill	    = 5,
    range_size	    = 6
} rangetype;


typedef enum usekeytype {
    key_inventory=0,
    keyrings=1,
    containers=2,
} usekeytype;

/* This is used to control what to do when we need to unapply
 * an object before we can apply another one.
 */
typedef enum unapplymode {
    unapply_nochoice=0,	    /* Will unapply objects when there no choice to unapply */
    unapply_never=1,	    /* will not unapply objects automatically */
    unapply_always=2,	    /* Will unapply whatever is necessary - this goes beyond */
			    /* no choice - if there are multiple ojbect of the same type */
			    /* that need to be unapplied, there is no way for the player */
			    /* to control which of these will be unapplied. */
} unapplymode;

typedef struct pl {
    struct pl	*next;		    /* Pointer to next player, NULL if this is last */
    NewSocket	socket;		    /* Socket information for this player */
    char	maplevel[MAX_BUF];  /* On which level is the player? */
    struct mapdef *loading;	    /* When entering a map in progress of loading, not really used */
    sint16	new_x,new_y;	    /* After entering a map being loaded */
    char	savebed_map[MAX_BUF];  /* map where player will respawn after death */
    sint16	bed_x, bed_y;	    /* x,y - coordinates of respawn (savebed) */
    sint16	known_spells[NROFREALSPELLS]; /* Spells known by the player */
    uint16	nrofknownspells;    /* Index in the above array */
    rangetype	shoottype;	    /* Which range-attack is being used by player */
    object	*ranges[range_size];/* object for each range.  Set up in fix player.  Note */
				    /* That not all are used, it is just nice to map this 1:1 */
				    /* With the range names */
    usekeytype	usekeys;	    /* Method for finding keys for doors */
    unapplymode	unapply;	    /* Method for auto unapply */
    sint16	chosen_spell;	    /* Type of readied spell */
    uint32	count;		    /* Any numbers typed before a command */
    uint32	mode;		    /* Mode of player for pickup. */

    sint8	digestion;	    /* Any bonuses/penalties to digestion */
    sint8	gen_hp;		    /* Bonuses to regeneration speed of hp */
    sint8	gen_sp;		    /* Bonuses to regeneration speed of sp */
    sint8	gen_sp_armour;	    /* Penalty to sp regen from armour */
    sint8	gen_grace;	    /* Bonuses to regeneration speed of grace */
    sint16	item_power;	    /* Total item power of objects equipped */
    uint8	state;		    /* Input state of the player (name, password, etc */
    uint8	listening;	    /* Which priority will be used in info_all */
    sint8	last_level;	    /* Last level we sent to client */

    /* Try to put all the bitfields together - saves some small amount of memory */
    uint32	known_spell:1;	    /* True if you know the spell of the wand */
    uint32	braced:1;	    /* Will not move if braced, only attack */
    uint32	tmp_invis:1;	    /* Will invis go away when we attack ? */
    uint32	do_los:1;	    /* If true, need to call update_los() in draw(), and clear */
    uint32	fire_on:1;	    /* Player should fire object, not move */
    uint32	run_on:1;	    /* Player should keep moving in dir until run is off */
    uint32	has_hit:1;	    /* If set, weapon_sp instead of speed will count */
    uint32	name_changed:1;	    /* If true, the player has set a name. */
    uint32	peaceful:1;	    /* If set, won't attack friendly creatures */
    uint32	hidden:1;	    /* If True, player (DM) is hidden from view */
    uint32	explore:1;	    /* if True, player is in explore mode */

    sint32	last_skill_index;   /* this is init from init_player_exp() */
    object	*last_skill_ob[MAX_EXP_CAT];	/* the exp object */
    sint32	last_skill_exp[MAX_EXP_CAT];	/* shadow register. if != exp. obj update client */
    sint32	last_skill_level[MAX_EXP_CAT];	/* same for level */
    sint32	last_skill_id[MAX_EXP_CAT];	/* Thats the CS_STATS_ id for client STATS cmd*/


    float	weapon_sp;	    /* Penalties to speed when fighting w speed >ws/10*/
    float	last_weapon_sp;	    /* if diff than weapon_sp, update client */
    uint16	last_flags;	    /* fire/run on flags for last tick */
    uint32	last_weight_limit;  /* Last weight limit transmitted to client */
    object	*golem;		    /* Which golem is controlled */
    uint32	golem_count;	    /* To track the golem */
    living	orig_stats;	    /* Permanent real stats of player */
    living	last_stats;	    /* Last stats as sent to client */
    float	last_speed;	    /* Last speed as sent to client */
    sint16	last_resist[NROFATTACKS];	/* last resist values sent to client */
    int		Swap_First;	    /* First stat player has selected to swap */
    object	*ob;		    /* The object representing the player */
    object	*last_used;	    /* Pointer to object last picked or applied */
    uint32	last_used_id;	    /* Safety measures to be sure it's the same */
    sint8	blocked_los[MAP_CLIENT_X][MAP_CLIENT_Y]; /* array showing what spaces */
				    /* the player can see.  For maps smaller than */
				    /* MAP_CLIENT_.., the upper left is used */

    char	own_title[MAX_NAME];	/* Title the player has chosen for themself */
				    /* Note that for dragon players, this is filled in for them */
    char	title[BIG_NAME];    /* Default title, like fighter, wizard, etc */
  
    sint8	levhp[11];	    /* What the player gained on that level */
    sint8	levsp[11];	    /* Same for sp */
    sint8	levgrace[11];	    /* And same for grace */

    char	killer[BIG_NAME];   /* Who killed this player. */
    char	last_tell[MAX_NAME];   /* last player that told you something [mids 01/14/2002] */

    char	write_buf[MAX_BUF]; /* Holds arbitrary input from client */
    char	input_buf[MAX_BUF]; /* Holds command to run */
    char	password[16];	    /* 2 (seed) + 11 (crypted) + 1 (EOS) + 2 (safety) = 16 */

#ifdef SAVE_INTERVAL
    time_t	last_save_time;
#endif /* SAVE_INTERVAL */
#ifdef AUTOSAVE
    uint32	last_save_tick;
#endif
    sint16    party_number;	    /* Party number this player is part of */
    sint16    party_number_to_join; /* used when player wants to join a party */
				    /* but we will have to get password first */
				    /* so we have to remember which party to */
				    /* join */
    char    search_str[MAX_BUF];    /* Item we are looking for */
    sint16  encumbrance;	    /* How much our player is encumbered  */
    Output_Buf	outputs[NUM_OUTPUT_BUFS];   /* holds output strings to client */
    uint16	outputs_sync;	    /* How often to print, no matter what */
    uint16	outputs_count;	    /* Print if this count is exceeded */
    object	*mark;		    /* marked object */
    uint32	mark_count;	    /* count of mark object */
} player;


/* not really the player, but tied pretty closely */  
typedef struct party_struct {
  sint16 partyid;
  char * partyleader;
  char passwd[9];
  struct party_struct *next;
  char *partyname;
#ifdef PARTY_KILL_LOG
  struct party_kill
  {
    char killer[MAX_NAME+1],dead[MAX_NAME+1];
    uint32 exp;
  } party_kills[PARTY_KILL_LOG];
#endif
  uint32 total_exp,kills;
} partylist;




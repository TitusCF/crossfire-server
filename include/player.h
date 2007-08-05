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

/**
 * @file
 * Player-specific structures.
 */

#ifndef PLAYER_H
#define PLAYER_H

/** How many output buffers a player has. */
#define NUM_OUTPUT_BUFS	5
/** One output buffer. */
typedef struct {
    const char *buf;        /**< Actual string pointer. */
    uint32 first_update;    /**< First time this message was stored. */
    uint16 count;           /**< How many times we got this message. */
} Output_Buf;


/**
 * What range is currntly selected by the player.
 *
 * Wand/rod/horn rolled into range_misc.  They all use the same body location
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
    range_builder   = 6,
    range_size	    = 7
} rangetype;

/** Bow firing mode. @todo document. */
typedef enum _bowtype {
    bow_normal = 0,
    bow_threewide = 1,
    bow_spreadshot = 2,
    bow_n = 3, /* must stay at 3 */
    bow_ne = 4,
    bow_e = 5,
    bow_se = 6,
    bow_s = 7,
    bow_sw = 8,
    bow_w = 9,
    bow_nw = 10, /* must stay at 10 */
	bow_bestarrow = 11
} bowtype_t;

/** Petmode. @todo document. */
typedef enum _petmode {
    pet_normal = 0,
    pet_sad = 1,
    pet_defend = 2,
    pet_arena = 3
} petmode_t;

/** How to use keys. @todo document. */
typedef enum usekeytype {
    key_inventory=0,
    keyrings=1,
    containers=2
} usekeytype;

/**
 * This is used to control what to do when we need to unapply
 * an object before we can apply another one.
 */
typedef enum unapplymode {
    unapply_nochoice=0,     /**< Will unapply objects when there no choice to unapply. */
    unapply_never=1,        /**< Will not unapply objects automatically. */
    unapply_always=2        /**< Will unapply whatever is necessary - this goes beyond
                * no choice - if there are multiple ojbect of the same type
                * that need to be unapplied, there is no way for the player
                * to control which of these will be unapplied. */
} unapplymode;

/**
 * This stores, for a spell a player knows, the last sp/gr/dam information sent to client.
 */
typedef struct client_spell {
    object *spell;
    sint16 last_sp;
    sint16 last_grace;
    sint16 last_dam;
    struct client_spell *next;
} client_spell;

/** One party. First item is ::firstparty. */
typedef struct party_struct {
    char * partyleader;     /**< Who is the leader. */
    char passwd[9];         /**< Party password. */
    struct party_struct *next;  /**< Next party in list. */
    char *partyname;        /**< Party name. */

#ifdef PARTY_KILL_LOG
    struct party_kill {
	char killer[MAX_NAME+1],dead[MAX_NAME+1];
	sint64 exp;
    } party_kills[PARTY_KILL_LOG];
#endif
    sint64 total_exp;
    uint32  kills;
} partylist;

/**
 * Whether to rejoin party at login or not.
 */
typedef enum party_rejoin_mode {
    party_rejoin_no = 0,        /**< Don't rejoin. */
    party_rejoin_if_exists = 1, /**< Rejoin if party exists. */
    party_rejoin_always = 2    /**< If party doesn't exist, form it. */
} party_rejoin_mode;

/** One player. */
typedef struct pl {
    struct pl	*next;		    /**< Pointer to next player, NULL if this is last. */
    socket_struct	socket;		    /**< Socket information for this player. */
    char	maplevel[MAX_BUF];  /**< On which level is the player? */
    char	savebed_map[MAX_BUF];  /**< Map where player will respawn after death. */
    sint16	bed_x, bed_y;	    /**< x,y - coordinates of respawn (savebed). */
    rangetype	shoottype;	    /**< Which range-attack is being used by player. */
    char	spellparam[MAX_BUF];	/**< What param to add to spells. */
    bowtype_t   bowtype;	    /**< Which firemode? */
    petmode_t   petmode;	    /**< Which petmode? */
    object	*ranges[range_size];/**< Object for each range.  Set up in fix_object().  Note
				    * That not all are used, it is just nice to map this 1:1
				    * With the range names. */
    uint32	golem_count;	    /**< To track the golem. */
    usekeytype	usekeys;	    /**< Method for finding keys for doors. */
    unapplymode	unapply;	    /**< Method for auto unapply. */
    uint32	count;		    /**< Any numbers typed before a command. */
    uint32	mode;		    /**< Mode of player for pickup. */

    sint16	digestion;	    /**< Any bonuses/penalties to digestion. */
    sint16	gen_hp;		    /**< Bonuses to regeneration speed of hp. */
    sint16	gen_sp;		    /**< Bonuses to regeneration speed of sp. */
    sint16	gen_sp_armour;	    /**< Penalty to sp regen from armour. */
    sint16	gen_grace;	    /**< Bonuses to regeneration speed of grace. */
    sint16	item_power;	    /**< Total item power of objects equipped. */
    uint8	state;		    /**< Input state of the player (name, password, etc.) @todo document. */
    uint8	listening;	    /**< Which priority will be used in info_all. */
    sint8	last_level;	    /**< Last level we sent to client. */

    /* Try to put all the bitfields together - saves some small amount of memory */
    uint32	braced:1;	    /**< Will not move if braced, only attack. */
    uint32	tmp_invis:1;	    /**< Will invis go away when we attack? */
    const char	*invis_race;	    /**< What race invisible to? */
    uint32	do_los:1;	    /**< If true, need to call update_los() in draw(), and clear. */
    uint32	fire_on:1;	    /**< Player should fire object, not move. */
    uint32	run_on:1;	    /**< Player should keep moving in dir until run is off. */
    uint32	has_hit:1;	    /**< If set, weapon_sp instead of speed will count. */
    uint32	name_changed:1;	    /**< If true, the player has set a name. */
    uint32	peaceful:1;	    /**< If set, won't attack friendly creatures. */
    uint32	hidden:1;	    /**< If True, player (DM) is hidden from view. */
    uint32	explore:1;	    /**< if True, player is in explore mode. */
    uint32	no_shout:1;	    /**< if True, player is *not* able to use shout command. */

    object	*last_skill_ob[NUM_SKILLS];	/**< Exp objects sent to client. */
    sint64	last_skill_exp[NUM_SKILLS];	/**< Last exp sent to client. If != exp. obj update client. */

    float	weapon_sp;	    /**< Penalties to speed when fighting w speed >ws/10. */
    float	last_weapon_sp;	    /**< if diff than weapon_sp, update client. */
    uint16	last_flags;	    /**< Fire/run on flags for last tick. */
    sint32	last_weight;	    /**< Last weight as sent to client; -1 means do not send weight. */
    sint32	last_weight_limit;  /**< Last weight limit transmitted to client. */
    uint32	last_path_attuned;  /**< Last spell attunment sent to client. */
    uint32	last_path_repelled; /**< Last spell repelled sent to client. */
    uint32	last_path_denied;   /**< Last spell denied sent to client. */
    living	orig_stats;	    /**< Permanent real stats of player. */
    living	last_stats;	    /**< Last stats as sent to client. */
    float	last_speed;	    /**< Last speed as sent to client. */
    sint16	last_resist[NROFATTACKS];	/**< last resist values sent to client. */
    int		Swap_First;	    /**< First stat player has selected to swap. */
    object	*ob;		    /**< The object representing the player. */
    object	*last_used;	    /**< Pointer to object last picked or applied. */
    uint32	last_used_id;	    /**< Safety measures to be sure it's the same. */
    sint8	blocked_los[MAP_CLIENT_X][MAP_CLIENT_Y]; /**< Array showing what spaces
				    * the player can see.  For maps smaller than
				    * MAP_CLIENT_.., the upper left is used. */

    char	own_title[MAX_NAME];	/**< Title the player has chosen for themself.
				    * Note that for dragon players, this is filled in for them. */
    char	title[BIG_NAME];    /**< Default title, like fighter, wizard, etc. */

    sint8	levhp[11];	    /**< What hp bonus the player gained on that level. */
    sint8	levsp[11];	    /**< What sp bonus the player gained on that level. */
    sint8	levgrace[11];	/**< What grace bonus the player gained on that level. */

    char	killer[BIG_NAME];   /**< Who killed this player. */
    char	last_tell[MAX_NAME];   /**< last player that told you something [mids 01/14/2002]. */

    char	write_buf[MAX_BUF]; /**< Holds arbitrary input from client. */
    char	input_buf[MAX_BUF]; /**< Holds command to run. */
    char	password[16];	    /**< 2 (seed) + 11 (crypted) + 1 (EOS) + 2 (safety) = 16 */
    char	new_password[16];   /**< 2 (seed) + 11 (crypted) + 1 (EOS) + 2 (safety) = 16 */

#ifdef SAVE_INTERVAL
    time_t	last_save_time;     /**< Last time the player was saved. */
#endif /* SAVE_INTERVAL */
#ifdef AUTOSAVE
    uint32	last_save_tick;     /**< Last tick the player was saved. */
#endif
    partylist    *party;	    /**< Party this player is part of. */
    partylist    *party_to_join; /**< Used when player wants to join a party
				    * but we will have to get password first
				    * so we have to remember which party to
				    * join. */
    party_rejoin_mode rejoin_party; /**< Whether to rejoin or not party at login. */
    char    search_str[MAX_BUF];    /**< Item we are looking for. */
    sint16  encumbrance;	    /**< How much our player is encumbered. */
    Output_Buf	outputs[NUM_OUTPUT_BUFS];   /**< Holds output strings to send to client. */
    uint16	outputs_sync;	    /**< How often to print, no matter what. */
    uint16	outputs_count;	    /**< Print if this count is exceeded. */
    object	*mark;		    /**< Marked object. */
    uint32	mark_count;	    /**< Count of marked object. */
    object	*transport;	    /**< Transport the player is in. */
    client_spell* spell_state; /**< Spell information sent to client. */
    /* Special DM fields */
    tag_t*  stack_items;    /**< Item stack for patch/dump/... commands. */
    int     stack_position; /**< Current stack position, 0 for no item. */
    sstring followed_player; /**< Player the DM is following. */
} player;

#endif /* PLAYER_H */

/*
 * static char *rcsid_player_h =
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

#define NUM_OUTPUT_BUFS	5
typedef struct {
  char *buf;			/* Actual string pointer */
  uint32 first_update;		/* First time this message was stored  */
  uint16 count;			/* How many times we got this message */
} Output_Buf;

/* this is already in define.h */
/*#define MAX_NAME 16*/   /* Max length of character name */


typedef enum rangetype {
  range_bottom = -1, range_none = 0, range_bow = 1, range_magic = 2,
  range_wand = 3, range_rod = 4, range_scroll = 5, range_horn = 6,
  range_skill = 7,
  range_size = 8
} rangetype;


typedef enum usekeytype {
    key_inventory=0,
    keyrings=1,
    containers=2,
} usekeytype;
    

typedef struct pl {
  struct pl *next;	      /* Pointer to next player, NULL if this is last */
  NewSocket socket;	    /* Socket information for this player */
  char maplevel[MAX_BUF];     /* On which level is the player? */
  struct mapdef *loading;     /* When entering a map in progress of loading */
  int new_x,new_y;            /* After entering a map being loaded */
  char savebed_map[MAX_BUF];  /* map where player will respawn after death */
  int bed_x, bed_y;           /* x,y - coordinates of respawn (savebed) */
  int removed;                /* Flag telling if ob is to be inserted */
  sint16 known_spells[NROFREALSPELLS]; /* Spells known by the player */
  uint16 nrofknownspells; 	/* Index in the above array */
  unsigned known_spell:1;     /* True if you know the spell of the wand */
  unsigned last_known_spell:1;/* What was last updated with draw_stats() */
  rangetype shoottype;	      /* Which range-attack is being used by player */
  rangetype last_shoot;	      /* What was last updated with draw_stats() */
  usekeytype usekeys;	      /* Method for finding keys for doors */
  sint16 chosen_spell;		/* Type of readied spell */
  sint16 last_spell;		/* What spell draw_stats() last displayed */
  sint16 chosen_item_spell;	/* Type of spell that the item fires */
  uint32 count;       /* Any numbers typed before a command */
  uint32 count_left;  /* How many instances of prev_cmd are left */
			      /* to execute */
  unsigned char prev_cmd;     /* Previous command executed */
  unsigned char prev_fire_on;
  unsigned char prev_keycode; /* Previous command executed */
  unsigned char key_down;     /* Last move-key still held down */
  uint16 mode;			/* Mode of player for pickup. */
  signed char digestion;	/* Any bonuses/penalties to digestion */
  signed char gen_hp;		/* Bonuses to regeneration speed of hp */
  signed char gen_sp;		/* Bonuses to regeneration speed of sp */
  signed char gen_sp_armour;	/* Penalty to sp regen from armour */
  signed char gen_grace;	/* Bonuses to regeneration speed of grace */
  uint32 braced:1;          /* Will not move if braced, only attack */
  uint32 tmp_invis:1;       /* Will invis go away when we attack ? */
  uint32 do_los:1;	      /* If true, update_los() in draw(), and clear */
#ifdef EXPLORE_MODE
  uint32 explore:1;         /* if True, player is in explore mode */
#endif
  sint32  last_skill_index;	       /* this is init from init_player_exp() */
  object *last_skill_ob[MAX_EXP_CAT];  /* the exp object */
  sint32  last_skill_exp[MAX_EXP_CAT]; /* shadow register. if != exp. obj update client */
  sint32  last_skill_level[MAX_EXP_CAT]; /* same for level */
  sint32  last_skill_id[MAX_EXP_CAT]; /* Thats the CS_STATS_ id for client STATS cmd*/

  unsigned char state;
  unsigned char listening; /* Which priority will be used in info_all */

  unsigned char fire_on;
  unsigned char run_on;
  unsigned char idle;      /* How long this player has been idle */
  unsigned char has_hit;   /* If set, weapon_sp instead of speed will count */
  float weapon_sp;         /* Penalties to speed when fighting w speed >ws/10*/
  float last_weapon_sp;    /* Last turn */
  uint16 last_flags;	    /* fire/run on flags for last tick */
  uint32  last_weight_limit;	/* Last weight limit transmitted to client */
  object *golem;           /* Which golem is controlled */
  living orig_stats;       /* Can be less in case of poisoning */
  living last_stats;       /* Last stats drawn with draw_stats() */
  float last_speed;
  signed long last_value;  /* Same usage as last_stats */
  long last_weight;
  sint16    last_resist[NROFATTACKS];	/* last resist values sent to client */

#ifdef USE_SWAP_STATS
  int Swap_First;
#endif
  
  int use_pixmaps;	/* If set, use pixmaps instead of fonts */
  int color_pixmaps;	/* if set, use color pixmaps (XPM) */

  object *ob;            /* The object representing the player */
  object *last_used;     /* Pointer to object last picked or applied */
  long last_used_id;     /* Safety measures to be sure it's the same */


  /* As the names suggest, these are how various aspects of the map
   * look like.  This is basically used to compare what we last
   * sent to the client so we only send what changed.
   */
  New_Face  *drawn[MAP_CLIENT_X][MAP_CLIENT_Y]; /* Last displayed */
  New_Face  *floor[MAP_CLIENT_X][MAP_CLIENT_Y];
  New_Face  *floor2[MAP_CLIENT_X][MAP_CLIENT_Y];
  int	    darkmask[MAP_CLIENT_X][MAP_CLIENT_Y];
  sint8	    blocked_los[MAP_CLIENT_X][MAP_CLIENT_Y];

  
/* Eneq(@csd.uu.se): Since there is no direct link between characters title and
   his level I figure that it doesn't matter what title he has so I have made
   it possible for him/her to define this in the resources. */
  
  char own_title[MAX_NAME];
  char title[MAX_NAME];
  
  signed char levhp[11]; /* What the player gained on that level */
  signed char levsp[11];
  signed char levgrace[11];
  signed char last_level;

  uint32 name_changed:1;      /* If true, the player has set a name. */
  char killer[BIG_NAME];  /* Who killed this player. */
  char last_cmd;
  int peaceful;

  char write_buf[MAX_BUF];
  char input_buf[MAX_BUF];
  char password[16]; /* 2 (seed) + 11 (crypted) + 1 (EOS) + 2 (safety) = 16 */
#ifdef SAVE_INTERVAL
  time_t last_save_time;
#endif /* SAVE_INTERVAL */
#ifdef AUTOSAVE
  long last_save_tick;
#endif
  sint16    party_number;
  sint16    party_number_to_join; /* used when player wants to join a party
				 but we will have to get password first
				 so we have to remember which party to
				 join */
#ifdef SEARCH_ITEMS
  char search_str[MAX_BUF];
#endif /* SEARCH_ITEMS */

#ifdef SPELL_ENCUMBRANCE
  short encumbrance;  /*  How much our player is encumbered  */
#endif
  Output_Buf	outputs[NUM_OUTPUT_BUFS];
  uint16	outputs_sync;	/* How often to print, no matter what */
  uint16	outputs_count;	/* Print if this count is exceeded */
  object	*mark;		/* marked object */
  uint32	mark_count;	/* count or mark object */
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




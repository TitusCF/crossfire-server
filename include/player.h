/**
 * @file
 * Player-specific structures.
 */

#ifndef PLAYER_H
#define PLAYER_H

/**
 * What range is currently selected by the player.
 *
 * Wand/rod/horn rolled into range_misc.  They all use the same body location
 * anyways.
 */
typedef enum rangetype {
    range_bottom    = -1, /**< Minimum, exclusive, value. */
    range_none      = 0,  /**< No range selected. */
    range_bow       = 1,  /**< Bow. */
    range_magic     = 2,  /**< Spells. */
    range_misc      = 3,  /**< Misc items. */
    range_golem     = 4,  /**< Control golem. */
    range_skill     = 5,  /**< Use skill. */
    range_builder   = 6,  /**< Map builder. */
    range_size      = 7   /**< Maximum, exclusive, value. */
} rangetype;

/** Bow firing mode.*/
typedef enum _bowtype {
    bow_normal = 0, /**< Standard mode, one random arrow. */
    bow_threewide = 1,  /**< Fire three arrows in the same direction. */
    bow_spreadshot = 2, /**< Fire three arrows in a cone. */
    bow_n = 3,          /**< Fire north whatever the facing direction. Must stay at 3. */
    bow_ne = 4,         /**< Fire north-east whatever the facing direction. */
    bow_e = 5,          /**< Fire east whatever the facing direction. */
    bow_se = 6,         /**< Fire south-east whatever the facing direction. */
    bow_s = 7,          /**< Fire south whatever the facing direction. */
    bow_sw = 8,         /**< Fire south-west whatever the facing direction. */
    bow_w = 9,          /**< Fire west whatever the facing direction. */
    bow_nw = 10,        /**< Fire north-west whatever the facing direction. */
    bow_bestarrow = 11  /**< Try to find an arrow matching the target. */
} bowtype_t;

/** Petmode. */
typedef enum _petmode {
    pet_normal = 0, /**< Standard mode/ */
    pet_sad = 1,    /**< Try to find enemies. */
    pet_defend = 2, /**< Stay close to the owner. */
    pet_arena = 3   /**< Attack other players in arena. */
} petmode_t;

/** How to use keys. */
typedef enum usekeytype {
    key_inventory = 0,  /**< Only use keys in inventory. */
    keyrings = 1,       /**< Use keys in inventory and active key rings. */
    containers = 2      /**< Use keys in inventory and active containers. */
} usekeytype;

/**
 * This is used to control what to do when we need to unapply
 * an object before we can apply another one.
 */
typedef enum unapplymode {
    unapply_nochoice = 0,   /**< Will unapply objects when there no choice to unapply. */
    unapply_never = 1,      /**< Will not unapply objects automatically. */
    unapply_always = 2      /**< Will unapply whatever is necessary - this goes beyond
                             * no choice - if there are multiple ojbect of the same type
                             * that need to be unapplied, there is no way for the player
                             * to control which of these will be unapplied. */
} unapplymode;

/**
 * This stores, for a spell a player knows, the last sp/gr/dam information sent to client.
 */
typedef struct client_spell {
    object *spell;              /**< Spell object this structure is about. */
    int16_t last_sp;             /**< Last spell cost. */
    int16_t last_grace;          /**< Last grace cost. */
    int16_t last_dam;            /**< Last damage. */
    struct client_spell *next;  /**< Next spell information. */
} client_spell;

/**
 * Whether to rejoin party at login or not.
 */
typedef enum party_rejoin_mode {
    party_rejoin_no = 0,        /**< Don't rejoin. */
    party_rejoin_if_exists = 1, /**< Rejoin if party exists. */
    party_rejoin_always = 2     /**< If party doesn't exist, form it. */
} party_rejoin_mode;

/** One player. */
typedef struct pl {
    struct pl   *next;                  /**< Pointer to next player, NULL if this is last. */
    socket_struct socket;               /**< Socket information for this player.  See the page on
                                         * @ref page_connection "the login process" for a description of its use. */
    char        maplevel[MAX_BUF];      /**< On which level is the player? */
    char        savebed_map[MAX_BUF];   /**< Map where player will respawn after death. */
    int16_t      bed_x, bed_y;           /**< x,y - coordinates of respawn (savebed). */
    rangetype   shoottype;              /**< Which range-attack is being used by player. */
    char        spellparam[MAX_BUF];    /**< What param to add to spells. */
    bowtype_t   bowtype;                /**< Which firemode? */
    petmode_t   petmode;                /**< Which petmode? */
    object      *ranges[range_size];    /**< Object for each range.  Set up in fix_object().  Note
                                         * That not all are used, it is just nice to map this 1:1
                                         * With the range names. */
    uint32_t      golem_count;            /**< To track the golem. */
    usekeytype  usekeys;                /**< Method for finding keys for doors. */
    unapplymode unapply;                /**< Method for auto unapply. */
    uint32_t      count;                  /**< Any numbers typed before a command. */
    uint32_t      mode;                   /**< Mode of player for pickup. */

    int16_t      digestion;              /**< Any bonuses/penalties to digestion. */
    int16_t      gen_hp;                 /**< Bonuses to regeneration speed of hp. */
    int16_t      gen_sp;                 /**< Bonuses to regeneration speed of sp. */
    int16_t      gen_sp_armour;          /**< Penalty to sp regen from armour. */
    int16_t      gen_grace;              /**< Bonuses to regeneration speed of grace. */
    int16_t      item_power;             /**< Total item power of objects equipped. */
    uint8_t       state;                  /**< Input state of the player (name, password, etc). See the page on
                                         * @ref page_connection "the login process" for a description of its use. */
    uint8_t       listening;              /**< Which priority will be used in info_all. */
    int8_t       last_level;             /**< Last level we sent to client. */
    float        character_load;         /**< Value between 0 and 1 indicating how much the character is overloaded. */
    float        last_character_load;    /**< Last value sent to the client. */

    /* Try to put all the bitfields together - saves some small amount of memory */
    uint32_t      braced:1;            /**< Will not move if braced, only attack. */
    uint32_t      tmp_invis:1;         /**< Will invis go away when we attack? */
    uint32_t      do_los:1;            /**< If true, need to call update_los() in draw(), and clear. */
    uint32_t      fire_on:1;           /**< Player should fire object, not move. */
    uint32_t      run_on:1;            /**< Player should keep moving in dir until run is off. */
    uint32_t      has_hit:1;           /**< If set, weapon_sp instead of speed will count. */
    uint32_t      name_changed:1;      /**< If true, the player has set a name. */
    uint32_t      peaceful:1;          /**< If set, won't attack friendly creatures. */
    uint32_t      hidden:1;            /**< If True, player (DM) is hidden from view. */
    uint32_t      no_shout:1;          /**< if True, player is *not *able to use shout command. */
    uint32_t      has_directory:1;     /**< If 0, the player was not yet saved, its directory doesn't exist. */
    const char  *invis_race;         /**< What race invisible to? */

    object      *last_skill_ob[MAX_SKILLS];     /**< Exp objects sent to client. */
    int64_t      last_skill_exp[MAX_SKILLS];     /**< Last exp sent to client. If != exp. obj update client. */

    float       last_weapon_sp;      /**< if diff than weapon_sp, update client. */
    uint16_t      last_flags;          /**< Fire/run on flags for last tick. */
    int32_t      last_weight;         /**< Last weight as sent to client; -1 means do not send weight. */
    int32_t      last_weight_limit;   /**< Last weight limit transmitted to client. */
    uint32_t      last_path_attuned;   /**< Last spell attunment sent to client. */
    uint32_t      last_path_repelled;  /**< Last spell repelled sent to client. */
    uint32_t      last_path_denied;    /**< Last spell denied sent to client. */
    uint32_t      last_character_flags; /**< Last character flags (CS_STAT_CHARACTER_FLAGS) sent to client. */
    uint16_t      last_item_power;      /**< Last value for item_power. */
    int         swap_first;          /**< First stat player has selected to swap. */
    living      orig_stats;          /**< Permanent real stats of player. */
    living      last_stats;          /**< Last stats as sent to client. */
    living      last_orig_stats;     /**< Last permanent stats sent to client. */
    living      last_race_stats;     /**< Last race stats sent to the client. */
    living      applied_stats;       /**< Stat changes due to gear or skills. */
    living      last_applied_stats;  /**< Last applied stats sent to the client. */
    float       last_speed;          /**< Last speed as sent to client. */
    int16_t      last_resist[NROFATTACKS];       /**< last resist values sent to client. */
    int16_t      last_golem_hp;       /**< Last golem hp value sent to the client. */
    int16_t      last_golem_maxhp;    /**< Last golem max hp value sent to the client. */
    object      *ob;                 /**< The object representing the player. */
    int8_t       blocked_los[MAP_CLIENT_X][MAP_CLIENT_Y]; /**< Array showing what spaces
                                                          * the player can see.  For maps smaller than
                                                          * MAP_CLIENT_.., the upper left is used. */

    char        own_title[MAX_NAME]; /**< Title the player has chosen for themself.
                                      * Note that for dragon players, this is filled in for them. */
    char        title[BIG_NAME];     /**< Default title, like fighter, wizard, etc. */

    int8_t       levhp[11];           /**< What hp bonus the player gained on that level. */
    int8_t       levsp[11];           /**< What sp bonus the player gained on that level. */
    int8_t       levgrace[11];        /**< What grace bonus the player gained on that level. */

    char        killer[BIG_NAME];    /**< Who killed this player. */
    char        last_tell[MAX_NAME]; /**< last player that told you something [mids 01/14/2002]. */

    char        password[16];        /**< 2 (seed) + 11 (crypted) + 1 (EOS) + 2 (safety) = 16 */
    char        new_password[16];    /**< 2 (seed) + 11 (crypted) + 1 (EOS) + 2 (safety) = 16 */

    int16_t      encumbrance;         /**< How much our player is encumbered. */
#ifdef SAVE_INTERVAL
    time_t      last_save_time;      /**< Last time the player was saved. */
#endif /* SAVE_INTERVAL */
#ifdef AUTOSAVE
    uint32_t      last_save_tick;      /**< Last tick the player was saved. */
#endif
    partylist   *party;              /**< Party this player is part of. */
    partylist   *party_to_join;      /**< Used when player wants to join a party
                                      * but we will have to get password first
                                      * so we have to remember which party to
                                      * join. */
    struct obj *last_exit;           /**< Last exit used by player or NULL */

    party_rejoin_mode rejoin_party;  /**< Whether to rejoin or not party at login. */
    char        search_str[MAX_BUF]; /**< Item we are looking for. */
    uint32_t      mark_count;          /**< Count of marked object. */
    object      *mark;               /**< Marked object. */
    object      *transport;          /**< Transport the player is in. */
    client_spell *spell_state;       /**< Spell information sent to client. */
    /* Special DM fields */
    tag_t       *stack_items;        /**< Item stack for patch/dump/... commands. */
    sstring     followed_player;     /**< Player the DM is following. */
    int         stack_position;      /**< Current stack position, 0 for no item. */
    int         language;            /**< The language the player wishes to use. */
    const char  *unarmed_skill;      /**< Prefered skill to use in unarmed combat */
    uint32_t      ticks_played;        /**< How many ticks this player has played */

    uint8_t delayed_buffers_allocated;  /**< Number of items in delayed_buffers_used. */
    uint8_t delayed_buffers_used;       /**< Used items in delayed_buffers_used. */
    SockList **delayed_buffers;         /**< Buffers which will be sent after the player's tick completes. */
} player;

/**
 * @defgroup FIND_PLAYER_xxx Flags to search players through find_player().
 * Those flag control what player should be returned.
 */
/*@{*/

#define FIND_PLAYER_PARTIAL_NAME    0x1 /**< Find on partial name. */
#define FIND_PLAYER_NO_HIDDEN_DM    0x2 /**< Don't find hidden DMs. */

/*@}*/

/**
 * @defgroup ADD_PLAYER_xxx Flags - flags passed to add_player()
 * to control behavior
 */
/*@{*/
#define ADD_PLAYER_NEW              0x1 /**< Name/password provided, so skip to roll stats */
#define ADD_PLAYER_NO_MAP           0x2 /**< Do not set the first map */
#define ADD_PLAYER_NO_STATS_ROLL    0x4 /**< Stats provided from client */
/*@}*/

#endif /* PLAYER_H */

/*
 * static char *rcsid_define_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2005 Mark Wedel & Crossfire Development Team
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
 * This file contains basic map-related structures and macros.
 */

/*
 * The mapstruct is allocated each time a new map is opened.
 * It contains pointers (very indirectly) to all objects on the map.
 */

#ifndef MAP_H
#define MAP_H

/* We set this size - this is to make magic map work properly on
 * tiled maps.  There is no requirement that this matches the
 * tiled maps size - it just seemed like a reasonable value.
 * Magic map code now always starts out putting the player in the
 * center of the map - this makes the most sense when dealing
 * with tiled maps.
 * We also figure out the magicmap color to use as we process the
 * spaces - this is more efficient as we already have up to date
 * map pointers.
 */
#define MAGIC_MAP_SIZE  50
#define MAGIC_MAP_HALF  MAGIC_MAP_SIZE/2


/* This correspondes to the map layers in the map2 protocol.
 * The MAP_LAYER_... correspond to what goes on what layer -
 * this removes the need for hardcoding, and also makes sure
 * we don't overstep the number of layers.
 */
#define MAP_LAYERS		10

extern const char *map_layer_name[MAP_LAYERS];


#define MAP_LAYER_FLOOR		0
#define MAP_LAYER_NO_PICK1	1   /**< Non pickable ground objects */
#define MAP_LAYER_NO_PICK2	2   /**< Non pickable ground objects */
#define MAP_LAYER_ITEM1		3   /**< Items that can be picked up */
#define MAP_LAYER_ITEM2		4
#define MAP_LAYER_ITEM3		5
#define MAP_LAYER_LIVING1	6   /**< Living creatures */
#define MAP_LAYER_LIVING2	7
#define MAP_LAYER_FLY1		8   /**< Flying objects - creatures, spells */
#define MAP_LAYER_FLY2		9   /**< Arrows, etc */

/** This is when the map will reset */
#define MAP_WHEN_RESET(m)	((m)->reset_time)

#define MAP_RESET_TIMEOUT(m)	((m)->reset_timeout)
#define MAP_DIFFICULTY(m)	((m)->difficulty)
#define MAP_TIMEOUT(m)		((m)->timeout)
#define MAP_SWAP_TIME(m)	((m)->swap_time)
#define MAP_OUTDOORS(m)		((m)->outdoor)

/**
 * Map darkness used to enforce the MAX_DARKNESS value.
 * but IMO, if it is beyond max value, that should be fixed
 * on the map or in the code.
 */
#define MAP_DARKNESS(m)	   	(m)->darkness

/** Map width. */
#define MAP_WIDTH(m)		(m)->width
/** Map height. */
#define MAP_HEIGHT(m)		(m)->height
/** Convenient function - total number of spaces is used in many places. */
#define MAP_SIZE(m)		((m)->width * (m)->height)

/** Default X coordinate for map enter. */
#define MAP_ENTER_X(m)		(m)->enter_x
/** Default Y coordinate for map enter. */
#define MAP_ENTER_Y(m)		(m)->enter_y

#define MAP_TEMP(m)		(m)->temp
#define MAP_PRESSURE(m)		(m)->pressure
#define MAP_HUMID(m)		(m)->humid
#define MAP_WINDSPEED(m)	(m)->windspeed
#define MAP_WINDDIRECTION(m)	(m)->winddir
#define MAP_SKYCOND(m)		(m)->sky
#define MAP_WORLDPARTX(m)	(m)->wpartx
#define MAP_WORLDPARTY(m)	(m)->wparty
#define MAP_NOSMOOTH(m)		(m)->nosmooth

/**
 * Options passed to ready_map_name() and load_original_map().
 * @anchor MAP_xxx
 * @todo remove map_block
 */
enum {
    MAP_FLUSH =         0x1,    /**< Always load map from the map directory, and don't do unique items or the like. */
    MAP_PLAYER_UNIQUE = 0x2,    /**< This map is player-specific. Don't do any more name translation on it. */
    MAP_BLOCK =         0x4,    /**< Unused. */
    MAP_STYLE =         0x8,    /**< Active objects shouldn't be put on active list. */
    MAP_OVERLAY =       0x10    /**< Map to load is an overlay. Always put items above floor. */
};

/**
 * Flags for save_object() and save_objects().
 * Can be combined for various effects.
 *
 * @anchor SAVE_FLAG_xxx
 */
enum {
    SAVE_FLAG_SAVE_UNPAID = 1,  /**< If set, unpaid items will be saved. */
    SAVE_FLAG_NO_REMOVE = 2    /**< If set, objects are not removed while saving. */
};

/**
 * How save_map() should save the map. Can't be combined.
 *
 * @anchor SAVE_MODE_xxx
 */
enum {
    SAVE_MODE_NORMAL = 0,   /**< No special handling. */
    SAVE_MODE_INPLACE = 1,  /**< Map is saved from where it was loaded.*/
    SAVE_MODE_OVERLAY = 2  /**< Map is persisted as an overlay. */
};

/**
 * Values for mapdef->in_memory field.
 * @anchor IN_MEMORY
 */
enum {
    MAP_IN_MEMORY = 1,  /**< Map is fully loaded. */
    MAP_SWAPPED = 2,    /**< Map spaces have been saved to disk. */
    MAP_LOADING = 3,    /**< This map is being loaded. */
    MAP_SAVING = 4      /**< Map being saved. Will stop remove_ob() from some processing. */
};

/* GET_MAP_FLAGS really shouldn't be used very often - get_map_flags should
 * really be used, as it is multi tile aware.  However, there are some cases
 * where it is known the map is not tiled or the values are known
 * consistent (eg, op->map, op->x, op->y)
 */
/** Gets map flags. Not multitile aware. */
#define GET_MAP_FLAGS(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].flags )
/** Sets map flags. Not multitile aware. */
#define SET_MAP_FLAGS(M,X,Y,C)	( (M)->spaces[(X) + (M)->width * (Y)].flags = C )
/** Gets map light. Not multitile aware. */
#define GET_MAP_LIGHT(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].light )
/** Sets map lighe. Not multitile aware. */
#define SET_MAP_LIGHT(M,X,Y,L)	( (M)->spaces[(X) + (M)->width * (Y)].light = L )
#define GET_MAP_PLAYER(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].pl )
#define SET_MAP_PLAYER(M,X,Y,C)	( (M)->spaces[(X) + (M)->width * (Y)].pl = C )

/** Gets the bottom object on a map. Not multitile aware. */
#define GET_MAP_OB(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].bottom )
/** Gets the top object on a map. Not multitile aware. */
#define GET_MAP_TOP(M,X,Y)	( (M)->spaces[(X) + (M)->width * (Y)].top )
/** legacy @todo remove */
#define get_map_ob		GET_MAP_OB

/** Sets the bottom object on a map. Not multitile aware. */
#define SET_MAP_OB(M,X,Y,tmp)	( (M)->spaces[(X) + (M)->width * (Y)].bottom = (tmp) )
/** Sets the top object on a map. Not multitile aware. */
#define SET_MAP_TOP(M,X,Y,tmp)	( (M)->spaces[(X) + (M)->width * (Y)].top = (tmp) )
/** legacy @todo remove */
#define set_map_ob SET_MAP_OB

/** Sets the layer face of specified square. Not multitile aware. */
#define SET_MAP_FACE_OBJ(M,X,Y,C,L) ( (M)->spaces[(X) + (M)->width * (Y)].faces_obj[L] = C )
/** Gets the layer face of specified square. Not multitile aware. */
#define GET_MAP_FACE_OBJ(M,X,Y,L)   ( (M)->spaces[(X) + (M)->width * (Y)].faces_obj[L]  )
/**
 * Returns the layers array so update_position can just copy
 * the entire array over.
 */
#define GET_MAP_FACE_OBJS(M,X,Y)   ( (M)->spaces[(X) + (M)->width * (Y)].faces_obj  )

/** Gets the blocking state of a square. Not multitile aware. */
#define GET_MAP_MOVE_BLOCK(M,X,Y)   ( (M)->spaces[(X) + (M)->width * (Y)].move_block )
/** Sets the blocking state of a square. Not multitile aware. */
#define SET_MAP_MOVE_BLOCK(M,X,Y,C) ( (M)->spaces[(X) + (M)->width * (Y)].move_block = C )

/** Gets the slowing state of a square. Not multitile aware. */
#define GET_MAP_MOVE_SLOW(M,X,Y)    ( (M)->spaces[(X) + (M)->width * (Y)].move_slow )
/** Sets the slowing state of a square. Not multitile aware. */
#define SET_MAP_MOVE_SLOW(M,X,Y,C)  ( (M)->spaces[(X) + (M)->width * (Y)].move_slow = C )

/** Gets the move_on state of a square. Not multitile aware. */
#define GET_MAP_MOVE_ON(M,X,Y)      ( (M)->spaces[(X) + (M)->width * (Y)].move_on )
/** Sets the move_on state of a square. Not multitile aware. */
#define SET_MAP_MOVE_ON(M,X,Y,C)    ( (M)->spaces[(X) + (M)->width * (Y)].move_on = C )

/** Gets the move_off state of a square. Not multitile aware. */
#define GET_MAP_MOVE_OFF(M,X,Y)      ( (M)->spaces[(X) + (M)->width * (Y)].move_off )
/** Sets the move_off state of a square. Not multitile aware. */
#define SET_MAP_MOVE_OFF(M,X,Y,C)    ( (M)->spaces[(X) + (M)->width * (Y)].move_off = C )

/**
 * Checks if a square is out of the map.
 * You should really know what you are doing before using this - you
 * should almost always be using out_of_map() instead, which takes into account
 * map tiling.
 */
#define OUT_OF_REAL_MAP(M,X,Y) ((X)<0 || (Y)<0 || (X)>=(M)->width || (Y)>=(M)->height)

/* These are used in the MapLook flags element.  They are not used in
 * in the object flags structure.
 */

#define P_BLOCKSVIEW    0x01    /**< This spot blocks the player's view. */
#define P_NO_MAGIC      0x02    /**< Spells (some) can't pass this object */

/* AB_NO_PASS is used for arch_blocked() return value.  It needs
 * to be here to make sure the bits don't match with anything.
 * Changed name to have AB_ prefix just to make sure no one
 * is using the P_NO_PASS.  AB_.. should only be used for
 * arch_blocked and functions that examine the return value.
 */

#define AB_NO_PASS       0x04
#define P_PLAYER	0x08	/* There is a player on this space */
#define P_IS_ALIVE      0x10	/**< Something alive is on this space. */
#define P_NO_CLERIC     0x20	/**< No clerical spells cast here. */
#define P_NEED_UPDATE	0x40	/**< This space is out of date. */
#define P_NO_ERROR      0x80	/**< Purely temporary - if set, update_position
                                 * does not complain if the flags are different.
                                 */
/* The following two values are not stored in the MapLook flags, but instead
 * used in the get_map_flags value - that function is used to return
 * the flag value, as well as other conditions - using a more general
 * function that does more of the work can hopefully be used to replace
 * lots of duplicate checks currently in the code.
 */
#define P_OUT_OF_MAP	0x100   /**< This space is outside the map. */
#define	P_NEW_MAP	0x200       /**< Coordinates passed result in a new tiled map.  */

/**
 * This structure contains all information related to one map square.
 */
typedef struct MapSpace {
    object	*bottom;	/**< Lowest object on this space. */
    object	*top;		/**< Highest object on this space. */
    object	*faces_obj[MAP_LAYERS];	/**< Face objects for the layers. */
    uint8	flags;		/**< Flags about this space (see the P_ values above). */
    sint8	light;		/**< How much light this space provides. */
    MoveType	move_block;	/**< What movement types this space blocks. */
    MoveType	move_slow;	/**< What movement types this space slows. */
    MoveType	move_on;	/**< What movement types are activated. */
    MoveType	move_off;	/**< What movement types are activated. */
    object	*pl;		/* Player that is on this space */
} MapSpace;

/**
 * This is an overlay structure of the whole world.  It exists as a simple
 * high level map, which doesn't contain the full data of the underlying map.
 * in this map, only things such as weather are recorded.  By doing so, we
 * can keep the entire world parameters in memory, and act as a whole on
 * them at once.  We can then, in a separate loop, update the actual world
 * with the new values we have assigned.
 */

typedef struct wmapdef {
    sint16	temp;		/**< Base temperature of this tile (F). */
    sint16	pressure;	/**< Barometric pressure (mb). */
    sint8	humid;		/**< Humitidy of this tile. */
    sint8	windspeed;	/**< Windspeed of this tile. */
    sint8	winddir;	/**< Direction of wind. */
    sint8	sky;		/**< Sky conditions. */
    sint32	avgelev;	/**< Average elevation. */
    uint32	rainfall;	/**< Cumulative rainfall. */
    uint8 	darkness;	/**< Indicates level of darkness of map. */
    uint8	water;		/**< 0-100 percentage of water tiles. */
    /*Dynamic parts*/
    sint16  realtemp;		/**< Temperature at a given calculation step for this tile. */
} weathermap_t;

/**
 * This is a game region.
 * Each map is in a given region of the game world and links to a region definiton, so
 * they have to appear here in the headers, before the mapdef
 */
typedef struct regiondef {
    struct	regiondef *next; /**< Pointer to next region, NULL for the last one */
    const char	*name;		 /**< Shortend name of the region as maps refer to it */
    const char	*parent_name;	 /**<
    				  * So that parent and child regions can be defined in
    				  * any order, we keep hold of the parent_name during
				  * initialisation, and the children get assigned to their
				  * parents later. (before runtime on the server though)
				  * nothing outside the init code should ever use this value.
				  */
    struct     regiondef *parent;/**<
    				  * Pointer to the region that is a parent of the current
    				  * region, if a value isn't defined in the current region
				  * we traverse this series of pointers until it is.
				  */
    const char	*longname;	 /**< Official title of the region, this might be defined
    				  * to be the same as name*/
    const char	*msg;		 /**< The description of the region */
    uint32	counter;	 /**< A generic counter for holding temporary data. */
    sint8	fallback;	 /**< Whether, in the event of a region not existing,
    				  * this should be the one we fall back on as the default. */
    char	*jailmap;        /**< Where a player that is arrested in this region should be imprisoned. */
    sint16	jailx, jaily;	 /**< The coodinates in jailmap to which the player should be sent. */
} region;

/**
 * Shop-related information for a map. This is one item type the shop will deal in, and
 * the price-chance.
 */
typedef struct shopitem {
    const char *name;     /**< Name of the item in question, null if it is the default item. */
    const char *name_pl;  /**< Plural name. */
    int typenum;    /**< Itemtype number we need to match, 0 if it is the default price. */
    sint8 strength; /**< The degree of specialisation the shop has in this item,
		     * as a percentage from -100 to 100. */
    int index;      /**< Being the size of the shopitems array.*/
} shopitems;

/**
 * This is a game-map.
 *
 * In general, code should always use the macros
 * above (or functions in map.c) to access many of the
 * values in the map structure.  Failure to do this will
 * almost certainly break various features.  You may think
 * it is safe to look at width and height values directly
 * (or even through the macros), but doing so will completely
 * break map tiling.
 */
typedef struct mapdef {
    struct mapdef *next;	/**< Next map, linked list. */
    char *tmpname;	/**< Name of temporary file. */
    char *name;		/**< Name of map as given by its creator. */
    region *region;	/**< What jurisdiction in the game world this map is ruled by
    			 * points to the struct containing all the properties of
			 * the region. */
    uint32 reset_time;	/**< When this map should reset. */
    uint32 reset_timeout;  /**< How many seconds must elapse before this map
			    * should be reset. */
    uint32 fixed_resettime:1;	/**< If true, reset time is not affected by
				 * players entering/exiting map. */
    uint32 unique:1;	/**< If set, this is a per player unique map. */
    uint32 template:1;	/**< If set, this is a template map. */
    uint32 nosmooth:1;  /**< If set the content of this map has smoothlevel=0 forced. */
    sint32 timeout;	/**< Swapout is set to this. */
    sint32 swap_time;	/**< When it reaches 0, the map will be swapped out. */
    sint16 players;	/**< How many players are on this level right now. Automatically updated by the object handling functions. */
    uint32 in_memory;	/**< Combination of @ref IN_MEMORY flags. */
    uint8 compressed;	/**< Compression method used. */
    uint16 difficulty;	/**< What level the player should be to play here. */

    uint8 darkness;    	/**< Indicates level of darkness of map. */
    uint16 width,height;	/**< Width and height of map. */
    uint16 enter_x;	/**< Enter_x and enter_y are default entrance location. */
    uint16 enter_y;	/**< on the map if none are set in the exit. */
    uint32  outdoor:1;	/**< True if an outdoor map. */
    oblinkpt *buttons;	/**< Linked list of linked lists of buttons. */
    MapSpace	*spaces;    /**< Array of spaces on this map. */
    sint16	temp;	/**< Base temperature of this tile (F). */
    sint16    pressure;  /**< Barometric pressure (mb). */
    sint8	humid;	/**< Humidity of this tile. */
    sint8     windspeed; /**< Windspeed of this tile. */
    sint8     winddir;   /**< Direction of wind. */
    sint8     sky;	/**< Sky conditions. */
    int     wpartx,wparty; /**< Highly fasten conversion between worldmap and weathermap. */
    shopitems   *shopitems; /**< List of item-types the map's shop will trade in. */
    char    *shoprace;  /**< The preffered race of the local shopkeeper. */
    double  shopgreed;  /**< How much our shopkeeper overcharges. */
    uint64  shopmin;    /**< Minimum price a shop will trade for. */
    uint64  shopmax;    /**< MMaximum price a shop will offer. */
    char    *msg;	/**< Message map creator may have left. */
    char    *maplore;	/**< Map lore information. */
    char    *tile_path[4];  /**< Path to adjoining maps. */
    struct mapdef *tile_map[4];	/**< Adjoining maps. */
    char path[HUGE_BUF];	/**< Filename of the map. */
    struct timeval last_reset_time; /**< A timestamp of the last original map loading. */
    char* background_music; /**< Background music to use for this map. */
} mapstruct;

/**
 * This is used by get_rangevector to determine where the other
 * creature is.  get_rangevector takes into account map tiling,
 * so you just can not look the the map coordinates and get the
 * righte value.  distance_x/y are distance away, which
 * can be negativbe.  direction is the crossfire direction scheme
 * that the creature should head.  part is the part of the
 * monster that is closest.
 * Note: distance should be always >=0. I changed it to UINT. MT
 */
typedef struct rv_vector {
    unsigned int distance;  /**< Distance, in squares. */
    int	    distance_x;     /**< X delta. */
    int	    distance_y;     /**< Y delta. */
    int	    direction;      /**< General direction to the targer. */
    object  *part;          /**< Part we found. */
} rv_vector;

#endif /* MAP_H */

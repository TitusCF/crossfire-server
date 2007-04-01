/*
 * static char *rcsid_map_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
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
 * Random map related variables.
 * @todo
 * replace defines with enum so can be linked from other places.
 */

#ifndef RANDOM_MAP_H
#define RANDOM_MAP_H

#define RM_SIZE		512

/**
 * Random map parameters.
 */
typedef struct {
    char wallstyle[RM_SIZE];
    char wall_name[RM_SIZE];
    char floorstyle[RM_SIZE];
    char monsterstyle[RM_SIZE];
    char treasurestyle[RM_SIZE];
    char layoutstyle[RM_SIZE];
    char doorstyle[RM_SIZE];
    char decorstyle[RM_SIZE];
    char origin_map[RM_SIZE];
    char final_map[RM_SIZE];
    char exitstyle[RM_SIZE];
    char this_map[RM_SIZE];
    char exit_on_final_map[RM_SIZE];
    char dungeon_name[RM_SIZE];

    int Xsize;
    int Ysize;
    int expand2x;
    int layoutoptions1;
    int layoutoptions2;
    int layoutoptions3;
    int symmetry;
    int difficulty;
    int difficulty_given;
    float difficulty_increase;
    int dungeon_level;
    int dungeon_depth;
    int decoroptions;
    int orientation;
    int origin_y;
    int origin_x;
    int random_seed;
    long unsigned int total_map_hp;
    int map_layout_style;
    int treasureoptions;
    int symmetry_used;
    region *region;
} RMParms;

int load_parameters(FILE *fp, int bufstate,RMParms *RP);


#define ONION_LAYOUT 1
#define MAZE_LAYOUT 2
#define SPIRAL_LAYOUT 3
#define ROGUELIKE_LAYOUT 4
#define SNAKE_LAYOUT 5
#define SQUARE_SPIRAL_LAYOUT 6
#define NROFLAYOUTS 6

/**
 * Move these defines out of room_gen_onion.c to here, as
 * other files (like square_spiral) also uses them.
 */
#define OPT_RANDOM    0     /**< Random option. */
#define OPT_CENTERED  1     /**< Centered. */
#define OPT_LINEAR    2     /**< Linear doors (default is nonlinear). */
#define OPT_BOTTOM_C  4     /**< Bottom-centered. */
#define OPT_BOTTOM_R  8     /**< Bottom-right centered. */
#define OPT_IRR_SPACE 16    /**< Irregularly/randomly spaced layers (default: regular). */
#define OPT_WALL_OFF 32     /**< No outer wall. */
#define OPT_WALLS_ONLY 64   /**< Only walls. */
#define OPT_NO_DOORS 256    /**< Place walls insead of doors.  Produces broken map. */


/**
 * Symmetry definitions -- used in this file AND in @ref random_maps/treasure.c "treasures.c",
 * the numerical values matter so don't change them.
 * @anchor SYM_xxx
 */
enum {
    RANDOM_SYM = 0, /**< Random symmetry. */
    NO_SYM = 1,     /**< No symmetry. */
    X_SYM = 2,      /**< Vertical symmetry. */
    Y_SYM = 3,      /**< Horizontal symmetry. */
    XY_SYM = 4      /**< Reflection. */
};

/** Minimal size a random should have to actually be generated. */
#define MIN_RANDOM_MAP_SIZE 10

/**
 * Macro to get a strongly centered random distribution,
 * from 0 to x, centered at x/2 */
#define BC_RANDOM(x) ((int) ((RANDOM() % (x)+RANDOM()%(x)+RANDOM()%(x))/3.))

int set_random_map_variable(RMParms *rp,const char *buf);

#endif

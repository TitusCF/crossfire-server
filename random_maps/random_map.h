

extern char wallstyle[];
extern char wall_name[];
extern char floorstyle[];
extern char monsterstyle[];
extern char treasurestyle[];
extern char layoutstyle[];
extern char doorstyle[];
extern char decorstyle[];
extern char origin_map[];
extern char final_map[];
extern char exitstyle[];
extern char this_map[];

extern int Xsize;
extern int Ysize;
extern int layoutoptions1;
extern int layoutoptions2;
extern int layoutoptions3;
extern int symmetry;
extern int difficulty;
extern int dungeon_level;
extern int dungeon_depth;
extern int decoroptions;
extern int orientation;
extern int origin_y;
extern int origin_x;
extern int random_seed;
extern long unsigned int total_map_hp;
extern int map_layout_style;
extern int treasureoptions;
extern int symmetry_used;
extern int generate_treasure_now;

int load_parameters(FILE *fp, int bufstate);

#include <rproto.h>

#define ONION_LAYOUT 1
#define MAZE_LAYOUT 2
#define NROFLAYOUTS 2

#define OPT_WALLS_ONLY 64

/* symmetry definitions--used in this file AND in treasure.c:
the numerical values matter so don't change them. */
#define RANDOM_SYM 0
#define NO_SYM 1
#define X_SYM 2
#define Y_SYM 3
#define XY_SYM 4


/*  a macro to get a strongly centered random distribution,
    from 0 to x, centered at x/2 */
#define BC_RANDOM(x) ((int) ((RANDOM() % (x)+RANDOM()%(x)+RANDOM()%(x))/3.))

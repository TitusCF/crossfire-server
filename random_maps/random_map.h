
#ifndef RANDOM_MAP_H
#define RANDOM_MAP_H

typedef struct {
  char wallstyle[512];
  char wall_name[512];
  char floorstyle[512];
  char monsterstyle[512];
  char treasurestyle[512];
  char layoutstyle[512];
  char doorstyle[512];
  char decorstyle[512];
  char origin_map[512];
  char final_map[512];
  char exitstyle[512];
  char this_map[512];

  int Xsize;
  int Ysize;
  int layoutoptions1;
  int layoutoptions2;
  int layoutoptions3;
  int symmetry;
  int difficulty;
  int difficulty_given;
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
  int generate_treasure_now;
} RMParms;

int load_parameters(FILE *fp, int bufstate,RMParms *RP);


#define ONION_LAYOUT 1
#define MAZE_LAYOUT 2
#define SPIRAL_LAYOUT 3
#define ROGUELIKE_LAYOUT 4
#define NROFLAYOUTS 4

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

#endif

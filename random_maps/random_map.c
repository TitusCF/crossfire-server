/*
 * static char *rcsid_random_map_c =
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

#include <time.h>
#include <stdio.h>
#include <global.h>
#include <maze_gen.h>
#include <room_gen.h>
#include <random_map.h>
#include <rproto.h>
#include <sproto.h>

void dump_layout(char **layout,RMParms *RP) {
 { int i,j;
  for(i=0;i<RP->Xsize;i++) {
    for(j=0;j<RP->Ysize;j++) {
      if(layout[i][j]==0) layout[i][j]=' ';
      printf("%c",layout[i][j]);
      if(layout[i][j]==' ') layout[i][j]=0;
    }
    printf("\n");
    }}
    printf("\n");
}
EXTERN FILE *logfile;
mapstruct *generate_random_map(char *OutFileName, RMParms *RP) {
    char **layout, buf[HUGE_BUF];
    mapstruct *theMap;
    int i;

    /* pick a random seed, or use the one from the input file */
    if(RP->random_seed == 0)
	RP->random_seed=time(0);

    SRANDOM(RP->random_seed);

    write_map_parameters_to_string(buf, RP);

    if(RP->difficulty==0) {
	RP->difficulty = RP->dungeon_level; /* use this instead of a map difficulty  */
	if (RP->difficulty_increase > 0.001) {
	    RP->difficulty = (int)((float)RP->dungeon_level * RP->difficulty_increase);
	    if (RP->difficulty < 1) RP->difficulty=1;
	}
    }
    else
	RP->difficulty_given=1;

    if(RP->Xsize<MIN_RANDOM_MAP_SIZE) RP->Xsize = MIN_RANDOM_MAP_SIZE + RANDOM()%25 + 5;
    if(RP->Ysize<MIN_RANDOM_MAP_SIZE) RP->Ysize = MIN_RANDOM_MAP_SIZE + RANDOM()%25 + 5;

    if(RP->expand2x > 0) {
	RP->Xsize /=2;
	RP->Ysize /=2;
    }

    layout = layoutgen(RP);

#ifdef RMAP_DEBUG
    dump_layout(layout,RP);
#endif

    /* increment these for the current map */
    RP->dungeon_level+=1;
    /* allow constant-difficulty maps. */
    /*  difficulty+=1; */

    /*  rotate the layout randomly */
    layout=rotate_layout(layout,RANDOM()%4,RP);
#ifdef RMAP_DEBUG
    dump_layout(layout,RP);
#endif

    /* allocate the map and set the floor */
    theMap = make_map_floor(layout,RP->floorstyle,RP); 

    /* set the name of the map. */
    strcpy(theMap->path,OutFileName);

    /* set region */
    theMap->region=RP->region;
    
    make_map_walls(theMap,layout,RP->wallstyle,RP);

    put_doors(theMap,layout,RP->doorstyle,RP);

    place_exits(theMap,layout,RP->exitstyle,RP->orientation,RP);

    place_specials_in_map(theMap,layout,RP);

    place_monsters(theMap,RP->monsterstyle,RP->difficulty,RP);

    /* treasures needs to have a proper difficulty set for the map. */
    theMap->difficulty=calculate_difficulty(theMap);

    place_treasure(theMap,layout,RP->treasurestyle,RP->treasureoptions,RP);

    put_decor(theMap,layout,RP->decorstyle,RP->decoroptions,RP);

    /* generate treasures, etc. */
    fix_auto_apply(theMap);

    unblock_exits(theMap,layout,RP);

    /* free the layout */
    for(i=0;i<RP->Xsize;i++)
	free(layout[i]);
    free(layout);

    theMap->msg = strdup_local(buf);
  
    return theMap;
}


/*  function selects the layout function and gives it whatever
    arguments it needs.  */
char **layoutgen(RMParms *RP) {
    char **maze=0;
    int oxsize= RP->Xsize, oysize=RP->Ysize;

    if(RP->symmetry == RANDOM_SYM) 
	RP->symmetry_used = (RANDOM() % ( XY_SYM))+1;
    else RP->symmetry_used = RP->symmetry;

    if(RP->symmetry_used==Y_SYM||RP->symmetry_used==XY_SYM) RP->Ysize = RP->Ysize/2+1;
    if(RP->symmetry_used==X_SYM||RP->symmetry_used==XY_SYM) RP->Xsize = RP->Xsize/2+1;

    if(RP->Xsize<MIN_RANDOM_MAP_SIZE) RP->Xsize = MIN_RANDOM_MAP_SIZE + RANDOM()%5;
    if(RP->Ysize<MIN_RANDOM_MAP_SIZE) RP->Ysize = MIN_RANDOM_MAP_SIZE + RANDOM()%5;
    RP->map_layout_style = 0;

    /* Redo this - there was a lot of redundant code of checking for preset
     * layout style and then random layout style.  Instead, figure out
     * the numeric layoutstyle, so there is only one area that actually
     * calls the code to make the maps.
     */
    if(strstr(RP->layoutstyle,"onion")) {
	RP->map_layout_style = ONION_LAYOUT;
    }

    if(strstr(RP->layoutstyle,"maze")) {
	RP->map_layout_style = MAZE_LAYOUT;
    }

    if(strstr(RP->layoutstyle,"spiral")) {
	RP->map_layout_style = SPIRAL_LAYOUT;
    }

    if(strstr(RP->layoutstyle,"rogue")) {
	RP->map_layout_style = ROGUELIKE_LAYOUT;
    }

    if(strstr(RP->layoutstyle,"snake")) {
	RP->map_layout_style = SNAKE_LAYOUT;
    }

    if(strstr(RP->layoutstyle,"squarespiral")) {
	RP->map_layout_style = SQUARE_SPIRAL_LAYOUT;
    }
    /* No style found - choose one ranomdly */
    if (RP->map_layout_style == 0) {
	RP->map_layout_style = (RANDOM() % NROFLAYOUTS) + 1;
    }

    switch(RP->map_layout_style) {

	    case ONION_LAYOUT:
		maze = map_gen_onion(RP->Xsize,RP->Ysize,RP->layoutoptions1,RP->layoutoptions2);
		if(!(RANDOM()%3)&& !(RP->layoutoptions1 & OPT_WALLS_ONLY)) roomify_layout(maze,RP);
		break;

	    case MAZE_LAYOUT:
		maze = maze_gen(RP->Xsize,RP->Ysize,RANDOM()%2);
		if(!(RANDOM()%2)) doorify_layout(maze,RP);
		break;

	    case SPIRAL_LAYOUT:
		maze = map_gen_spiral(RP->Xsize,RP->Ysize,RP->layoutoptions1);
		if(!(RANDOM()%2)) doorify_layout(maze,RP);
		break;

	    case ROGUELIKE_LAYOUT:
		/* Don't put symmetry in rogue maps.  There isn't much reason to
		 * do so in the first place (doesn't make it any more interesting),
		 * but more importantly, the symmetry code presumes we are symmetrizing
		 * spirals, or maps with lots of passages - making a symmetric rogue
		 * map fails because its likely that the passages the symmetry process
		 * creates may not connect the rooms.
		 */
		RP->symmetry_used = NO_SYM;
		RP->Ysize = oysize;
		RP->Xsize = oxsize;
		maze = roguelike_layout_gen(RP->Xsize,RP->Ysize,RP->layoutoptions1);
		/* no doorifying...  done already */
		break;

	    case SNAKE_LAYOUT:
		maze = make_snake_layout(RP->Xsize,RP->Ysize,RP->layoutoptions1);
		if(RANDOM()%2) roomify_layout(maze,RP);
		break;

	    case SQUARE_SPIRAL_LAYOUT:
		maze = make_square_spiral_layout(RP->Xsize,RP->Ysize,RP->layoutoptions1);
		if(RANDOM()%2) roomify_layout(maze,RP);
		break;
	}

    maze = symmetrize_layout(maze, RP->symmetry_used,RP);
#ifdef RMAP_DEBUG
    dump_layout(maze,RP);
#endif
    if(RP->expand2x) {
	maze = expand2x(maze,RP->Xsize,RP->Ysize);
	RP->Xsize = RP->Xsize * 2 -1;
	RP->Ysize = RP->Ysize * 2 -1;
    }
    return maze; 
}


/*  takes a map and makes it symmetric:  adjusts Xsize and
Ysize to produce a symmetric map. */

char **symmetrize_layout(char **maze, int sym,RMParms *RP) {
  int i,j;
  char **sym_maze;
  int Xsize_orig,Ysize_orig;
  Xsize_orig = RP->Xsize;
  Ysize_orig = RP->Ysize;
  RP->symmetry_used = sym;  /* tell everyone else what sort of symmetry is used.*/
  if(sym == NO_SYM) {
    RP->Xsize = Xsize_orig;
    RP->Ysize = Ysize_orig;
    return maze;
  }
  /* pick new sizes */
  RP->Xsize = ((sym==X_SYM||sym==XY_SYM)?RP->Xsize*2-3:RP->Xsize);
  RP->Ysize = ((sym==Y_SYM||sym==XY_SYM)?RP->Ysize*2-3:RP->Ysize);

  sym_maze = (char **)calloc(sizeof(char*),RP->Xsize);
  for(i=0;i<RP->Xsize;i++)
    sym_maze[i] = (char *)calloc(sizeof(char),RP->Ysize);

  if(sym==X_SYM)
    for(i=0;i<RP->Xsize/2+1;i++)
      for(j=0;j<RP->Ysize;j++) {
        sym_maze[i][j] = maze[i][j];
        sym_maze[RP->Xsize - i-1][j] = maze[i][j];
      };
  if(sym==Y_SYM)
    for(i=0;i<RP->Xsize;i++)
      for(j=0;j<RP->Ysize/2+1;j++) {
        sym_maze[i][j] = maze[i][j];
        sym_maze[i][RP->Ysize-j-1] = maze[i][j];
      }
  if(sym==XY_SYM)
    for(i=0;i<RP->Xsize/2+1;i++)
      for(j=0;j<RP->Ysize/2+1;j++) {
        sym_maze[i][j] = maze[i][j];
        sym_maze[i][RP->Ysize-j-1] = maze[i][j];
        sym_maze[RP->Xsize - i-1][j] = maze[i][j];
        sym_maze[RP->Xsize - i-1][RP->Ysize-j-1] = maze[i][j];
      }
  /* delete the old maze */
  for(i=0;i<Xsize_orig;i++)
    free(maze[i]);
  free(maze);
  /* reconnect disjointed spirals */
  if(RP->map_layout_style==SPIRAL_LAYOUT) 
    connect_spirals(RP->Xsize,RP->Ysize,sym,sym_maze);
  /* reconnect disjointed nethackmazes:  the routine for
   spirals will do the trick?*/
  if(RP->map_layout_style==ROGUELIKE_LAYOUT) 
    connect_spirals(RP->Xsize,RP->Ysize,sym,sym_maze);
  
  return sym_maze;
}


/*  takes  a map and rotates it.  This completes the
    onion layouts, making them possibly centered on any wall. 
    It'll modify Xsize and Ysize if they're swapped.
*/

char ** rotate_layout(char **maze,int rotation,RMParms *RP) {
  char **new_maze;
  int i,j;

  switch(rotation) {
  case 0:
    return maze;
    break;
  case 2:  /* a reflection */
    {
      char *new=malloc(sizeof(char) * RP->Xsize*RP->Ysize);
      for(i=0;i<RP->Xsize;i++) {  /* make a copy */
        for(j=0;j<RP->Ysize;j++) {
          new[i * RP->Ysize + j] = maze[i][j];
        }
      }
      for(i=0;i<RP->Xsize;i++) { /* copy a reflection back */
        for(j=0;j<RP->Ysize;j++) {
          maze[i][j]= new[(RP->Xsize-i-1)*RP->Ysize + RP->Ysize-j-1];
        }
      }
      free(new);
      return maze;
      break;
    }
  case 1:
  case 3:
    { int swap;
    new_maze = (char **) calloc(sizeof(char *),RP->Ysize);
    for(i=0;i<RP->Ysize;i++) {
      new_maze[i] = (char *) calloc(sizeof(char),RP->Xsize);
    }
    if(rotation == 1) /* swap x and y */
      for(i=0;i<RP->Xsize;i++)
        for(j=0;j<RP->Ysize;j++)
          new_maze[j][i] = maze[i][j];

    if(rotation == 3) { /* swap x and y */
      for(i=0;i<RP->Xsize;i++)
        for(j=0;j<RP->Ysize;j++)
          new_maze[j][i] = maze[RP->Xsize -i-1][RP->Ysize - j-1];
    }

    /* delete the old layout */
    for(i=0;i<RP->Xsize;i++)
      free(maze[i]);
    free(maze);

    swap = RP->Ysize;
    RP->Ysize = RP->Xsize;
    RP->Xsize = swap;
    return new_maze;
    break;
    }
  }
  return NULL;
}

/*  take a layout and make some rooms in it. 
    --works best on onions.*/
void roomify_layout(char **maze,RMParms *RP) {
  int tries = RP->Xsize*RP->Ysize/30;
  int ti;

  for(ti=0;ti<tries;ti++) {
    int dx,dy;  /* starting location for looking at creating a door */
    int cx,cy;  /* results of checking on creating walls. */
    dx = RANDOM() % RP->Xsize;
    dy = RANDOM() % RP->Ysize;
    cx = can_make_wall(maze,dx,dy,0,RP);  /* horizontal */
    cy = can_make_wall(maze,dx,dy,1,RP);  /* vertical */
    if(cx == -1) {
      if(cy != -1)
        make_wall(maze,dx,dy,1);
      continue;
    }
    if(cy == -1) {
      make_wall(maze,dx,dy,0);
      continue;
    }
    if(cx < cy) make_wall(maze,dx,dy,0);
    else make_wall(maze,dx,dy,1);
  }
}

/*  checks the layout to see if I can stick a horizontal(dir = 0) wall
    (or vertical, dir == 1)
    here which ends up on other walls sensibly.  */
    
int can_make_wall(char **maze,int dx,int dy,int dir,RMParms *RP) {
  int i1;
  int length=0;

  /* dont make walls if we're on the edge. */
  if(dx == 0 || dx == (RP->Xsize -1) || dy == 0 || dy == (RP->Ysize-1)) return -1;

  /* don't make walls if we're ON a wall. */
  if(maze[dx][dy]!=0) return -1;

  if(dir==0) /* horizontal */
    {
      int y = dy;
      for(i1=dx-1;i1>0;i1--) {
        int sindex=surround_flag2(maze,i1,y,RP);
        if(sindex == 1) break;  
        if(sindex != 0) return -1;  /* can't make horiz.  wall here */
        if(maze[i1][y]!=0) return -1;  /* can't make horiz.  wall here */
        length++;
      }
	
      for(i1=dx+1;i1<RP->Xsize-1;i1++) {
        int sindex=surround_flag2(maze,i1,y,RP);
        if(sindex == 2) break;  
        if(sindex != 0) return -1;   /* can't make horiz.  wall here */
        if(maze[i1][y]!=0) return -1;  /* can't make horiz.  wall here */
        length++;
      }
      return length;
    }
  else {  /* vertical */
    int x = dx;
    for(i1=dy-1;i1>0;i1--) {
      int sindex=surround_flag2(maze,x,i1,RP);
      if(sindex == 4) break;  
      if(sindex != 0) return -1;  /* can't make vert. wall here */
      if(maze[x][i1]!=0) return -1;  /* can't make horiz.  wall here */
      length++;
    }
	
    for(i1=dy+1;i1<RP->Ysize-1;i1++) {
      int sindex=surround_flag2(maze,x,i1,RP);
      if(sindex == 8) break;  
      if(sindex != 0) return -1;  /* can't make verti. wall here */
      if(maze[x][i1]!=0) return -1;  /* can't make horiz.  wall here */
      length++;
    }
    return length;
  }
  return -1;   
}


int make_wall(char **maze,int x, int y, int dir){
  maze[x][y] = 'D'; /* mark a door */
  switch(dir) {
  case 0: /* horizontal */
    {
      int i1;
      for(i1 = x-1;maze[i1][y]==0;i1--) 
        maze[i1][y]='#';
      for(i1 = x+1;maze[i1][y]==0;i1++)
        maze[i1][y]='#';
      break;
    }
  case 1: /* vertical */
    {
      int i1;
      for(i1 = y-1;maze[x][i1]==0;i1--) 
        maze[x][i1]='#';
      for(i1 = y+1;maze[x][i1]==0;i1++)
        maze[x][i1]='#';
      break;
    }
  }      

  return 0;
}

/*  puts doors at appropriate locations in a layout. */

void doorify_layout(char **maze,RMParms *RP) {
  int ndoors = RP->Xsize*RP->Ysize/60;  /* reasonable number of doors. */
  int *doorlist_x;
  int *doorlist_y;
  int doorlocs = 0;  /* # of available doorlocations */
  int i,j;
  
  doorlist_x = malloc(sizeof(int) * RP->Xsize*RP->Ysize);
  doorlist_y = malloc(sizeof(int) * RP->Xsize*RP->Ysize);


  /* make a list of possible door locations */
  for(i=1;i<RP->Xsize-1;i++)
    for(j=1;j<RP->Ysize-1;j++) {
      int sindex = surround_flag(maze,i,j,RP);
      if(sindex == 3 || sindex == 12) /* these are possible door sindex*/
        {
          doorlist_x[doorlocs]=i;
          doorlist_y[doorlocs]=j;
          doorlocs++;
        }
    }
  while(ndoors > 0 && doorlocs > 0) {
    int di;
    int sindex;
    di = RANDOM() % doorlocs;
    i=doorlist_x[di];
    j=doorlist_y[di];
    sindex= surround_flag(maze,i,j,RP);
    if(sindex == 3 || sindex == 12) /* these are possible door sindex*/
      {
        maze[i][j] = 'D';
        ndoors--;
      }
    /* reduce the size of the list */
    doorlocs--;
    doorlist_x[di]=doorlist_x[doorlocs];
    doorlist_y[di]=doorlist_y[doorlocs];
  }
  free(doorlist_x);
  free(doorlist_y);
}


void write_map_parameters_to_string(char *buf,RMParms *RP) {

  char small_buf[256];
  sprintf(buf,"xsize %d\nysize %d\n",RP->Xsize,RP->Ysize);

  if(RP->wallstyle[0]) {
    sprintf(small_buf,"wallstyle %s\n",RP->wallstyle);
    strcat(buf,small_buf);
  }

  if(RP->floorstyle[0]) {
    sprintf(small_buf,"floorstyle %s\n",RP->floorstyle);
    strcat(buf,small_buf);
  }

  if(RP->monsterstyle[0]) {
    sprintf(small_buf,"monsterstyle %s\n",RP->monsterstyle);
    strcat(buf,small_buf);
  }

  if(RP->treasurestyle[0]) {
    sprintf(small_buf,"treasurestyle %s\n",RP->treasurestyle);
    strcat(buf,small_buf);
  }

  if(RP->layoutstyle[0]) {
    sprintf(small_buf,"layoutstyle %s\n",RP->layoutstyle);
    strcat(buf,small_buf);
  }

  if(RP->decorstyle[0]) {
    sprintf(small_buf,"decorstyle %s\n",RP->decorstyle);
    strcat(buf,small_buf);
  }

  if(RP->doorstyle[0]) {
    sprintf(small_buf,"doorstyle %s\n",RP->doorstyle);
    strcat(buf,small_buf);
  }

  if(RP->exitstyle[0]) {
    sprintf(small_buf,"exitstyle %s\n",RP->exitstyle);
    strcat(buf,small_buf);
  }

  if(RP->final_map[0]) {
    sprintf(small_buf,"final_map %s\n",RP->final_map);
    strcat(buf,small_buf);
  }
  
  if(RP->exit_on_final_map[0]) { 
    sprintf(small_buf,"exit_on_final_map %s\n",RP->exit_on_final_map);
    strcat(buf,small_buf);
  }
  
  if(RP->this_map[0]) {
    sprintf(small_buf,"origin_map %s\n",RP->this_map);
    strcat(buf,small_buf);
  }

  if(RP->expand2x) {
    sprintf(small_buf,"expand2x %d\n",RP->expand2x);
    strcat(buf,small_buf);
  }

  if(RP->layoutoptions1) {
    sprintf(small_buf,"layoutoptions1 %d\n",RP->layoutoptions1);
    strcat(buf,small_buf);
  }


  if(RP->layoutoptions2) {
    sprintf(small_buf,"layoutoptions2 %d\n",RP->layoutoptions2);
    strcat(buf,small_buf);
  }


  if(RP->layoutoptions3) {
    sprintf(small_buf,"layoutoptions3 %d\n",RP->layoutoptions3);
    strcat(buf,small_buf);
  }

  if(RP->symmetry) {
    sprintf(small_buf,"symmetry %d\n",RP->symmetry);
    strcat(buf,small_buf);
  }


  if(RP->difficulty && RP->difficulty_given ) {
    sprintf(small_buf,"difficulty %d\n",RP->difficulty);
    strcat(buf,small_buf);
  }

  if(RP->difficulty_increase != 1.0 ) {
    sprintf(small_buf,"difficulty_increase %f\n",RP->difficulty_increase);
    strcat(buf,small_buf);
  }

  sprintf(small_buf,"dungeon_level %d\n",RP->dungeon_level);
  strcat(buf,small_buf);

  if(RP->dungeon_depth) {
    sprintf(small_buf,"dungeon_depth %d\n",RP->dungeon_depth);
    strcat(buf,small_buf);
  }

  if(RP->decoroptions) {
    sprintf(small_buf,"decoroptions %d\n",RP->decoroptions);
    strcat(buf,small_buf);
  }

  if(RP->orientation) {
    sprintf(small_buf,"orientation %d\n",RP->orientation);
    strcat(buf,small_buf);
  }

  if(RP->origin_x) {
    sprintf(small_buf,"origin_x %d\n",RP->origin_x);
    strcat(buf,small_buf);
  }

  if(RP->origin_y) {
    sprintf(small_buf,"origin_y %d\n",RP->origin_y);
    strcat(buf,small_buf);
  }
  if(RP->random_seed) {
    /* Add one so that the next map is a bit different */
    sprintf(small_buf,"random_seed %d\n",RP->random_seed + 1);
    strcat(buf,small_buf);
  }

  if(RP->treasureoptions) {
    sprintf(small_buf,"treasureoptions %d\n",RP->treasureoptions);
    strcat(buf,small_buf);
  }


}

void write_parameters_to_string(char *buf,
                                int xsize_n,
                                int ysize_n,
                                char *wallstyle_n,
                                char *floorstyle_n,
                                char *monsterstyle_n,
                                char *treasurestyle_n,
                                char *layoutstyle_n,
                                char *decorstyle_n,
                                char *doorstyle_n,
                                char *exitstyle_n,
                                char *final_map_n,
                                char *exit_on_final_map_n,
                                char *this_map_n,
                                int layoutoptions1_n,
                                int layoutoptions2_n,
                                int layoutoptions3_n,
                                int symmetry_n,
                                int dungeon_depth_n,
                                int dungeon_level_n,
                                int difficulty_n,
                                int difficulty_given_n,
                                int decoroptions_n,
                                int orientation_n,
                                int origin_x_n,
                                int origin_y_n,
                                int random_seed_n,
                                int treasureoptions_n,
				float difficulty_increase) 
{

  char small_buf[256];
  sprintf(buf,"xsize %d\nysize %d\n",xsize_n,ysize_n);

  if(wallstyle_n && wallstyle_n[0]) {
    sprintf(small_buf,"wallstyle %s\n",wallstyle_n);
    strcat(buf,small_buf);
  }

  if(floorstyle_n && floorstyle_n[0]) {
    sprintf(small_buf,"floorstyle %s\n",floorstyle_n);
    strcat(buf,small_buf);
  }

  if(monsterstyle_n && monsterstyle_n[0]) {
    sprintf(small_buf,"monsterstyle %s\n",monsterstyle_n);
    strcat(buf,small_buf);
  }

  if(treasurestyle_n && treasurestyle_n[0]) {
    sprintf(small_buf,"treasurestyle %s\n",treasurestyle_n);
    strcat(buf,small_buf);
  }

  if(layoutstyle_n &&layoutstyle_n[0]) {
    sprintf(small_buf,"layoutstyle %s\n",layoutstyle_n);
    strcat(buf,small_buf);
  }

  if(decorstyle_n && decorstyle_n[0]) {
    sprintf(small_buf,"decorstyle %s\n",decorstyle_n);
    strcat(buf,small_buf);
  }

  if(doorstyle_n && doorstyle_n[0]) {
    sprintf(small_buf,"doorstyle %s\n",doorstyle_n);
    strcat(buf,small_buf);
  }

  if(exitstyle_n && exitstyle_n[0]) {
    sprintf(small_buf,"exitstyle %s\n",exitstyle_n);
    strcat(buf,small_buf);
  }

  if(final_map_n && final_map_n[0]) {
    sprintf(small_buf,"final_map %s\n",final_map_n);
    strcat(buf,small_buf);
  }

  if(exit_on_final_map_n && exit_on_final_map_n[0]) {
    sprintf(small_buf,"exit_on_final_map %s\n",exit_on_final_map_n);
    strcat(buf,small_buf);
  }
  
  if(this_map_n && this_map_n[0]) {
    sprintf(small_buf,"origin_map %s\n",this_map_n);
    strcat(buf,small_buf);
  }

  if(layoutoptions1_n) {
    sprintf(small_buf,"layoutoptions1 %d\n",layoutoptions1_n);
    strcat(buf,small_buf);
  }


  if(layoutoptions2_n) {
    sprintf(small_buf,"layoutoptions2 %d\n",layoutoptions2_n);
    strcat(buf,small_buf);
  }


  if(layoutoptions3_n) {
    sprintf(small_buf,"layoutoptions3 %d\n",layoutoptions3_n);
    strcat(buf,small_buf);
  }

  if(symmetry_n) {
    sprintf(small_buf,"symmetry %d\n",symmetry_n);
    strcat(buf,small_buf);
  }


  if(difficulty_n && difficulty_given_n ) {
    sprintf(small_buf,"difficulty %d\n",difficulty_n);
    strcat(buf,small_buf);
  }

  if(difficulty_increase > 0.001 ) {
    sprintf(small_buf,"difficulty_increase %f\n",difficulty_increase);
    strcat(buf,small_buf);
  }

  sprintf(small_buf,"dungeon_level %d\n",dungeon_level_n);
  strcat(buf,small_buf);

  if(dungeon_depth_n) {
    sprintf(small_buf,"dungeon_depth %d\n",dungeon_depth_n);
    strcat(buf,small_buf);
  }

  if(decoroptions_n) {
    sprintf(small_buf,"decoroptions %d\n",decoroptions_n);
    strcat(buf,small_buf);
  }

  if(orientation_n) {
    sprintf(small_buf,"orientation %d\n",orientation_n);
    strcat(buf,small_buf);
  }

  if(origin_x_n) {
    sprintf(small_buf,"origin_x %d\n",origin_x_n);
    strcat(buf,small_buf);
  }

  if(origin_y_n) {
    sprintf(small_buf,"origin_y %d\n",origin_y_n);
    strcat(buf,small_buf);
  }
  if(random_seed_n) {
    /* Add one so that the next map is a bit different */
    sprintf(small_buf,"random_seed %d\n",random_seed_n + 1);
    strcat(buf,small_buf);
  }

  if(treasureoptions_n) {
    sprintf(small_buf,"treasureoptions %d\n",treasureoptions_n);
    strcat(buf,small_buf);
  }


}

/* copy an object with an inventory...  i.e., duplicate the inv too. */
void copy_object_with_inv(object *src_ob, object *dest_ob) {
  object *walk,*tmp;

  copy_object(src_ob,dest_ob);

  for(walk=src_ob->inv;walk!=NULL;walk=walk->below) {
    tmp=get_object();
    copy_object(walk,tmp);
    insert_ob_in_ob(tmp,dest_ob);
  }
}

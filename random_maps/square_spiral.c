

/* peterm@langmuir.eecs.berkeley.edu:  this function generates a random
snake-type layout.

input:  xsize, ysize;
output:  a char** array with # and . for closed and open respectively.

a char value of 0 represents a blank space:  a '#' is
a wall.

*/


#include <stdio.h>
#include <global.h>
#include <time.h>

#if 0

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

#endif

char **map_gen_onion(int xsize, int ysize, int option, int layers);


/* These are some helper functions which help with
   manipulating a centered onion and turning it into
   a square spiral */

/* this starts from within a centered onion layer (or between two layers),
   and looks up until it finds a wall, and then looks right until it
   finds a vertical wall, i.e., the corner.  It sets cx and cy to that.
   it also starts from cx and cy. */

void find_top_left_corner(char **maze,int *cx, int *cy) {

  (*cy)--;
  /* find the top wall. */
  while(maze[*cx][*cy]==0) (*cy)--;
  /* proceed right until a corner is detected */
  while(maze[*cx][*cy+1]==0) (*cx)++;
  
  /* cx and cy should now be the top-right corner of the onion layer */
}


char **make_square_spiral_layout(int xsize, int ysize,int options) {
  int i,j;
  int cx,cy;
  int tx,ty;

  /* generate and allocate a doorless, centered onion */
  char **maze = map_gen_onion(xsize,ysize,257,0);

  /* find the layout center.  */
  for(i=0;i<xsize;i++)
    for(j=0;j<ysize;j++) {
      if(maze[i][j]=='C' ) {
        cx = i; cy=j;
      }
    }
  tx = cx; ty = cy;
  while(1) {
    find_top_left_corner(maze,&tx,&ty);

    if(ty < 2 || tx < 2 || tx > xsize -2 || ty > ysize-2 ) break;
    make_wall(maze,tx,ty-1,1);  /* make a vertical wall with a door */

    maze[tx][ty-1]='#'; /* convert the door that make_wall puts here to a wall */
    maze[tx-1][ty]='D';/* make a doorway out of this layer */

    /* walk left until we find the top-left corner */
    while(maze[tx-1][ty]) tx--;

    make_wall(maze,tx-1,ty,0);     /* make a horizontal wall with a door */

    /* walk down until we find the bottom-left corner */
    while(maze[tx][ty+1]) ty++;

    make_wall(maze,tx,ty+1,1);    /* make a vertical wall with a door */

    /* walk rightuntil we find the bottom-right corner */
    while(maze[tx+1][ty]) tx++;
    
    make_wall(maze,tx+1,ty,0);   /* make a horizontal wall with a door */
    tx++;  /* set up for next layer. */
  }
      
  /* place the exits.  */

  if(RANDOM() %2) {
    maze[cx][cy]='>';
    maze[xsize-2][1]='<';
  }
  else {
    maze[cx][cy]='<';
    maze[xsize-2][1]='>';
  }
    
  return maze;
}




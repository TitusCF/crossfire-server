

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




char **make_snake_layout(int xsize, int ysize,int options) {
  int i,j;

  /* allocate that array, set it up */
  char **maze = (char **)calloc(sizeof(char*),xsize);
  for(i=0;i<xsize;i++) {
    maze[i] = (char *) calloc(sizeof(char),ysize);
  }

  /* write the outer walls */
  for(i=0;i<xsize;i++) 
    maze[i][0] = maze[i][ysize-1] = '#';
  for(j=0;j<ysize;j++)
    maze[0][j] = maze[xsize-1][j] = '#';

  /* Bail out if the size is too small to make a snake. */
  if(xsize < 8 || ysize < 8) return maze;

  /* decide snake orientation--vertical or horizontal , and
     make the walls and place the doors. */

  if(RANDOM()%2) { /* vertical orientation */
    int n_walls = RANDOM() % ((xsize - 5)/3) +1;
    int spacing = xsize / (n_walls+1);
    int orientation=1;
    for(i=spacing;i<xsize-3;i+=spacing) {
      if(orientation) {
        for(j=1;j<ysize-2;j++) {
          maze[i][j] = '#';
        }
        maze[i][j] = 'D';
      }
      else {
       for(j=2;j<ysize;j++) {
          maze[i][j] = '#';
        }
        maze[i][1] = 'D';
      }
      orientation ^= 1; /* toggle the value of orientation */
    }
  }
  else {  /* horizontal orientation */
    int n_walls = RANDOM() % ((ysize - 5)/3) +1;
    int spacing = ysize / (n_walls+1);
    int orientation=1;
    for(i=spacing;i<ysize-3;i+=spacing) {
      if(orientation) {
        for(j=1;j<xsize-2;j++) {
          maze[j][i] = '#';
        }
        maze[j][i] = 'D';
      }
      else {
       for(j=2;j<xsize;j++) {
          maze[j][i] = '#';
        }
        maze[1][i] = 'D';
      }
      orientation ^= 1; /* toggle the value of orientation */
    }
  }
  
  /* place the exit up/down */
  if(RANDOM() %2)
    { maze[1][1] = '<'; maze[xsize-2][ysize-2]='>'; }
  else
    { maze[1][1] = '>'; maze[xsize-2][ysize-2]='<'; }
    
  
  return maze;
}




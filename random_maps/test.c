#include <stdio.h>
#include <global.h>
#include <expand2x.h>


/* this is a testing program for layouts.  It's
   included here for convenience only.  */
char **map_gen_spiral(int,int,int);
char **roguelike_layout_gen(int xsize, int ysize, int options);
char **make_snake_layout(int xsize, int ysize, int options );
char **make_square_spiral_layout(int xsize, int ysize, int options );
char **gen_corridor_rooms(int, int, int);

void dump_layout(char **layout, int Xsize, int Ysize) {
  int i,j;

  for(j=0;j<Ysize;j++) {
    for(i=0;i<Xsize;i++) {
      if(layout[i][j]==0) layout[i][j]=' ';
      printf("%c",layout[i][j]);
    }
    printf("\n");
  }
}

main() {
  int Xsize, Ysize;
  char **layout, **biglayout;
  SRANDOM(time(0));

  Xsize= RANDOM() %30 + 10;
  Ysize= RANDOM() %20 + 10;


  /* put your layout here */
  layout = roguelike_layout_gen(Xsize,Ysize,0);
  /*layout = make_snake_layout(Xsize,Ysize,0); */
  /*layout = make_square_spiral_layout(Xsize,Ysize,0); */
  /*layout = gen_corridor_rooms(Xsize, Ysize, 1); */
  /*layout = maze_gen(Xsize,Ysize,0); */
  /*layout = map_gen_onion(Xsize,Ysize,0,0);*/

  dump_layout(layout, Xsize, Ysize);
  printf("\nExpanding layout...\n");

  biglayout = expand2x(layout, Xsize, Ysize);
  dump_layout(biglayout, Xsize*2-1, Ysize*2-1);
}


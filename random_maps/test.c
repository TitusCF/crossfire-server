#include <stdio.h>
#include <global.h>

/* this is a testing program for layouts.  It's
   included here for convenience only.  */
char **map_gen_spiral(int,int,int);
char **roguelike_layout_gen(int xsize, int ysize, int options);
char **make_snake_layout(int xsize, int ysize, int options );
char **make_square_spiral_layout(int xsize, int ysize, int options );

main() {
  int Xsize, Ysize;
  int i,j,k;
  char **layout;
  SRANDOM(time(0));

  Xsize= RANDOM() %30 + 20;
  Ysize= RANDOM() %30 + 20;
  /* put your layout here */
  /*layout = roguelike_layout_gen(Xsize,Ysize,0);*/
  /*layout = make_snake_layout(Xsize,Ysize,0); */
  layout = make_square_spiral_layout(Xsize,Ysize,0);

  for(i=0;i<Xsize;i++) {
      for(j=0;j<Ysize;j++) {
        if(layout[i][j]==0) layout[i][j]=' ';
        printf("%c",layout[i][j]);
      }
      printf("\n");
  }
}

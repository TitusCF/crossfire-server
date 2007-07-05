

/*#include <random_map.h> */
char **maze_gen(int xsize, int ysize,int option);  /* the outside interface routine */
void fill_maze_full(char **maze,int x,int y,int xsize,int ysize);
void fill_maze_sparse(char **maze,int x,int y,int xsize,int ysize);
void make_wall_free_list(int xsize,int ysize);
void pop_wall_point(int *x, int *y);
int find_free_point(char **maze,int *x,int *y,int xc, int yc,int xsize,int ysize);

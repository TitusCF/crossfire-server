/*
 * static char *rcsid_style_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
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

    The author can be reached via e-mail to mwedel@scruz.net
*/


#include <global.h>
#include <random_map.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/autoconf.h"

#ifndef HAVE_SCANDIR

/* The scandir is grabbed from the gnulibc and modified slightly to remove
 * special gnu libc constructs/error conditions.
 */

/* Copyright (C) 1992, 1993, 1994, 1995, 1996 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

int alphasort( struct dirent **a, struct dirent **b)
{
  return strcmp ((*a)->d_name, (*b)->d_name);
}

extern int errno;

int
scandir (dir, namelist, select, cmp)
     const char *dir;
     struct dirent ***namelist;
     int (*select)(struct dirent *);
     int (*cmp)(const void *, const void *);
{
  DIR *dp = opendir (dir);
  struct dirent **v = NULL;
  size_t vsize = 0, i;
  struct dirent *d;
  int save;

  if (dp == NULL)
    return -1;

  errno=0;

  i = 0;
  while ((d = readdir (dp)) != NULL)
    if (select == NULL || (*select) (d))
      {
	size_t dsize;

	if (i == vsize)
	  {
	    struct dirent **new;
	    if (vsize == 0)
	      vsize = 10;
	    else
	      vsize *= 2;
	    new = (struct dirent **) realloc (v, vsize * sizeof (*v));
	    if (new == NULL)
	      {
	      lose:
		errno=ENOMEM;

		break;
	      }
	    v = new;
	  }

	/*	dsize = &d->d_name[sizeof(d->d_name)] - (char *) d; */
	v[i] = (struct dirent *) malloc (d->d_reclen);
	if (v[i] == NULL)
	  goto lose;

	memcpy (v[i++], d, d->d_reclen);
      }

  if (errno != 0)
    {
      save = errno;
      (void) closedir (dp);
      while (i > 0)
	free (v[--i]);
      free (v);
      errno=save;
      return -1;
    }

  (void) closedir (dp);
  errno=save;

  /* Sort the list if we have a comparison function to sort with.  */
  if (cmp != NULL)
    qsort (v, i, sizeof (*v), cmp);
  *namelist = v;
  return i;
}

#endif

object *style_map_object_list[2048];
int nrofstyle_map_objects;


/* the warning here is because I've declared it "const", the
   .h file in linux allows non-const.  */
int select_regular_files(const struct dirent *the_entry) {
  if(the_entry->d_name[0]=='.') return 0;
  if(strstr(the_entry->d_name,"CVS")) return 0;
  return 1;
}
  
/* this function loads and returns the map requested.
  dirname, for example, is "/styles/wallstyles", stylename, is,
for example, "castle", difficulty is -1 when difficulty is
irrelevant to the style.  If dirname is given, but stylename
isn't, and difficult is -1, it returns a random style map.
Otherwise, it tries to match the difficulty given with a style
file, named style_name_# where # is an integer */


mapstruct *find_style(char *dirname,char *stylename,int difficulty) {
  char style_file_path[256];
  char style_file_full_path[256];
  mapstruct *style_map = 0;
  struct stat file_stat;

  
  /* if stylename exists, set style_file_path to that file.*/
  if(stylename && strlen(stylename)>0)
    sprintf(style_file_path,"%s/%s",dirname,stylename);
  else /* otherwise, just use the dirname.  We'll pick a random stylefile.*/
    sprintf(style_file_path,"%s",dirname);

  /* is what we were given a directory, or a file? */
  sprintf(style_file_full_path,"%s/maps%s",settings.datadir,style_file_path);
  stat(style_file_full_path,&file_stat);


  if(! (S_ISDIR(file_stat.st_mode))) {
    style_map = load_original_map(style_file_path,0);
  }
  if(style_map == NULL)  /* maybe we were given a directory! */
    {
	 struct dirent **namelist;
	 int n;
	 char style_dir_full_path[256];

	 /* get the names of all the files in that directory */
	 sprintf(style_dir_full_path,"%s/maps%s",settings.datadir,style_file_path);
	 n = scandir(style_dir_full_path,&namelist,select_regular_files,alphasort);

	 if(n<=0) return 0; /* nothing to load.  Bye. */

	 if(difficulty==-1) {  /* pick a random style from this dir. */
	   strcat(style_file_path,"/");
	   strcat(style_file_path,namelist[RANDOM()%n]->d_name);
	   style_map = load_original_map(style_file_path,0);
	 }
	 else {  /* find the map closest in difficulty */
	   int min_dist=32000,min_index=-1;
	   int i;
	   for(i=0;i<n;i++) {
	     int dist;
	     char *mfile_name = strrchr(namelist[i]->d_name,'_')+1;
	     if((mfile_name-1) == NULL) { /* since there isn't a sequence, */
	       /*pick one at random to recurse */
	       return find_style(style_file_path,
				 namelist[RANDOM()%n]->d_name,difficulty);
	     }
	       
	     dist = abs(difficulty-atoi(mfile_name));
	     if(dist<min_dist) {
	       min_dist = dist;
	       min_index = i;
	     }
	   }
	   /* presumably now we've found the "best" match for the
		 difficulty. */
	   strcat(style_file_path,"/");
	   strcat(style_file_path,namelist[min_index]->d_name);
	   style_map = load_original_map(style_file_path,0);

	 }
	
    }

  return style_map;

}

/* picks a random object from a style map.
	it maintains a data stucture so that if the same stylemap is
	called several times, it uses the old datastucture. */

object *pick_random_object(mapstruct *style) {
  static mapstruct *laststyle=0;
  
  /* if this isn't the current style, make the style_map_object_list array
     for easy searching */
  if(laststyle!=style)
	 {
		int i,j;
		int maxx,maxy;
		object *new_obj;

		laststyle=style;
		maxx = style->map_object->x;
		maxy = style->map_object->y;
		nrofstyle_map_objects = 0;
		for(i=0;i<maxx;i++) {
		  for(j=0;j<maxy;j++) {
			 new_obj = get_map_ob(style,i,j);
			 if(new_obj) { /* make sure it's the head: if it is, add it */
				if(!new_obj->head ) {
				  style_map_object_list[nrofstyle_map_objects] = new_obj;
				  nrofstyle_map_objects++;
				}
			 }
		  }
		}
	 }
  if(nrofstyle_map_objects > 0)
  return style_map_object_list[RANDOM() % nrofstyle_map_objects];
  else return NULL;
}
				
			 
  

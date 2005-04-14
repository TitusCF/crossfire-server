/*
 * static char *rcsid_style_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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


#include <global.h>
#include <random_map.h>
#ifndef WIN32 /* ---win32 exclude headers */
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/autoconf.h"
#endif /* win32 */


/* This is our own version of scandir/select_regular_files/sort.
 * To support having subdirectories in styles, we need to know
 * if in fact the directory we read is a subdirectory.  However,
 * we can't get that through the normal dirent entry.  So
 * instead, we do our own where we do have the full directory
 * path so can do stat calls to see if in fact it is a directory.
 * dir is the name of the director to scan.
 * namelist is the array of file names returned.  IT needs to be
 * freed by the caller.
 * skip_dirs controls our behavioru - if nonzero, we don't
 * skip any subdirectories - if zero, we store those away,
 * since there are cases where we want to choose a random
 * directory. 
 */
int load_dir (const char *dir, char ***namelist, int skip_dirs)
{
    DIR *dp;
    struct dirent *d;
    int entries=0, entry_size=0;
    char name[NAME_MAX+1], **rn=NULL;
    struct stat sb;

    dp = opendir (dir);
    if (dp == NULL)
	return -1;

    while ((d = readdir (dp)) != NULL) {
	sprintf(name, "%s/%s", dir, d->d_name);
	if (skip_dirs) {
	    stat(name, &sb);
	    if (S_ISDIR(sb.st_mode)) {
		continue;
	    }
	}

	if (entries == entry_size) {
	    entry_size+=10;
	    rn = realloc(rn, sizeof(char*) * entry_size);
	}
	rn[entries] = strdup_local(d->d_name);
	entries++;

    }
    (void) closedir (dp);

    qsort(rn, entries, sizeof(char*), (int(*)())strcmp);

    *namelist = rn;
    return entries;
}




  
/* this function loads and returns the map requested.
 * dirname, for example, is "/styles/wallstyles", stylename, is,
 * for example, "castle", difficulty is -1 when difficulty is
 * irrelevant to the style.  If dirname is given, but stylename
 * isn't, and difficult is -1, it returns a random style map.
 * Otherwise, it tries to match the difficulty given with a style
 * file, named style_name_# where # is an integer 
 */

/* remove extern, so visible to command_style_map_info function */
mapstruct *styles=NULL;


mapstruct *load_style_map(char *style_name)
{
    mapstruct *style_map;

    /* Given a file.  See if its in memory */
    for (style_map = styles; style_map!=NULL; style_map=style_map->next) {
	if (!strcmp(style_name, style_map->path)) return style_map;
    }
    style_map = load_original_map(style_name,MAP_STYLE);
    /* Remove it from global list, put it on our local list */
    if (style_map) {
	mapstruct *tmp;

	if (style_map == first_map)
	    first_map = style_map->next;
	else {
	    for (tmp = first_map; tmp && tmp->next != style_map; tmp = tmp->next);
	    if(tmp)
		tmp->next = style_map->next;
	}
	style_map->next = styles;
	styles = style_map;
    }
    return style_map;
}

mapstruct *find_style(char *dirname,char *stylename,int difficulty) {
    char style_file_path[256];
    char style_file_full_path[256];
    mapstruct *style_map = NULL;
    struct stat file_stat;
    int i, only_subdirs=0;
  
    /* if stylename exists, set style_file_path to that file.*/
    if(stylename && strlen(stylename)>0)
	sprintf(style_file_path,"%s/%s",dirname,stylename);
    else /* otherwise, just use the dirname.  We'll pick a random stylefile.*/
	sprintf(style_file_path,"%s",dirname);

    /* is what we were given a directory, or a file? */
    sprintf(style_file_full_path,"%s/maps%s",settings.datadir,style_file_path);
    stat(style_file_full_path,&file_stat);

    if(! (S_ISDIR(file_stat.st_mode))) {
	style_map=load_style_map(style_file_path);
    }
    if(style_map == NULL)  /* maybe we were given a directory! */
    {
	char **namelist;
	int n;
	char style_dir_full_path[256];

	/* get the names of all the files in that directory */
	sprintf(style_dir_full_path,"%s/maps%s",settings.datadir,style_file_path);

	/* First, skip subdirectories.  If we don't find anything, then try again
	 * without skipping subdirs.
	 */
	n = load_dir(style_dir_full_path, &namelist, 1);

	if (n<=0) {
	    n = load_dir(style_dir_full_path, &namelist, 0);
	    only_subdirs=1;
	}

	if (n<=0) return 0; /* nothing to load.  Bye. */

	/* Picks a random map.  Note that if this is all directories,
	 * we know it won't be able to load, so save a few ticks.   
	 * the door handling checks for this failure and handles
	 * it properly.
	 */
	if(difficulty==-1) {  /* pick a random style from this dir. */
	    if (only_subdirs)
		style_map=NULL;
	    else {
		strcat(style_file_path,"/");
		strcat(style_file_path,namelist[RANDOM()%n]);
		style_map = load_style_map(style_file_path);
	    }
	}
	else {  /* find the map closest in difficulty */
	    int min_dist=32000,min_index=-1;

	    for(i=0;i<n;i++) {
		int dist;
		char *mfile_name = strrchr(namelist[i],'_')+1;

		if((mfile_name-1) == NULL) { /* since there isn't a sequence, */
		    int q;
		    /*pick one at random to recurse */
		    style_map= find_style(style_file_path,
				 namelist[RANDOM()%n],difficulty);
		    for (q=0; q<n; q++)
			free(namelist[q]);
		    free(namelist);
		    return style_map;
		} else {
		    dist = abs(difficulty-atoi(mfile_name));
		    if(dist<min_dist) {
			min_dist = dist;
			min_index = i;
		    }
		}
	    }
	    /* presumably now we've found the "best" match for the
		    difficulty. */
	    strcat(style_file_path,"/");
	    strcat(style_file_path,namelist[min_index]);
	    style_map = load_style_map(style_file_path);
	}
	for (i=0; i<n; i++)
	    free(namelist[i]);
	free(namelist);
    }
  return style_map;

}


/* picks a random object from a style map.
 * Redone by MSW so it should be faster and not use static
 * variables to generate tables.
 */
object *pick_random_object(mapstruct *style) {
    int x,y, limit=0;
    object *new_obj;

    /* while returning a null object will result in a crash, that
     * is actually preferable to an infinite loop.  That is because
     * most servers will automatically restart in case of crash.
     * Change the logic on getting the random space - shouldn't make
     * any difference, but this seems clearer to me.
     */
    do {
	limit++;
	x = RANDOM() % MAP_WIDTH(style);
	y = RANDOM() % MAP_HEIGHT(style);
	new_obj = get_map_ob(style,x,y);
    } while (new_obj == NULL && limit<1000);
    if (new_obj->head) return new_obj->head;
    else return new_obj;
}
				

void free_style_maps()			 
{
    mapstruct *next;
    int  style_maps=0;

    /* delete_map will try to free it from the linked list,
     * but won't find it, so we need to do it ourselves
     */
    while (styles) {
	next = styles->next;
	delete_map(styles);
	styles=next;
	style_maps++;
    }
    LOG(llevDebug,"free_style_maps: Freed %d maps\n", style_maps);
}


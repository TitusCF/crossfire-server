/*
 * static char *rcsid_map_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1992 Frank Tore Johansen
    Copyright (C) 2000 Mark Wedel

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
#include <funcpoint.h>

#include <loader.h>

#include <unistd.h>

extern int nrofallocobjects,nroffreeobjects;

#if defined(sgi)
/* popen_local is defined in porting.c */
#define popen popen_local
#endif

#if defined (MACH) || defined (NeXT) || defined (__MACH__)
#ifndef S_ISGID
#define S_ISGID 0002000
#endif
#ifndef S_IWOTH
#define S_IWOTH 0000200
#endif
#ifndef S_IWGRP
#define S_IWGRP 0000020
#endif
#ifndef S_IWUSR
#define S_IWUSR 0000002
#endif
#ifndef S_IROTH
#define S_IROTH 0000400
#endif
#ifndef S_IRGRP
#define S_IRGRP 0000040
#endif
#ifndef S_IRUSR
#define S_IRUSR 0000004
#endif
#endif
#if defined(MACH) || defined(vax) || defined(ibm032) || defined(NeXT) || defined(__MACH__)
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif
#endif

/* This is a list of the suffix, uncompress and compress functions.  Thus,
 * if you have some other compress program you want to use, the only thing
 * that needs to be done is to extended this.
 * The first entry must be NULL - this is what is used for non
 * compressed files.
 */
#define NROF_COMPRESS_METHODS 4
char *uncomp[4][3] = {
  {NULL, NULL, NULL},
  {".Z", UNCOMPRESS, COMPRESS},
  {".gz", GUNZIP, GZIP},
  {".bz2", BUNZIP, BZIP}
};


/*
 * Returns the mapstruct which has a name matching the given argument.
 */

mapstruct *has_been_loaded (char *name) {
  mapstruct *map;

  if (!name || !*name) 
	return 0;
  for (map = first_map; map; map = map->next)
      if (!strcmp (name, map->path))
	  break;
  return (map);
}

/*
 * This makes a path absolute outside the world of Crossfire.
 * In other words, it prepends LIBDIR/MAPDIR/ to the given path
 * and returns the pointer to a static array containing the result.
 * it really should be called create_mapname
 */

char *create_pathname (char *name) {
    static char buf[MAX_BUF];

    /* Why?  having extra / doesn't confuse unix anyplace?  Dependancies
     * someplace else in the code? msw 2-17-97
     */
    if (*name == '/')
      sprintf (buf, "%s/%s%s", settings.datadir, settings.mapdir, name);
    else
      sprintf (buf, "%s/%s/%s", settings.datadir, settings.mapdir, name);
    return (buf);
}

/*
 * This makes absolute path to the itemfile where unique objects
 * will be saved. Converts '/' to '@'. I think it's essier maintain
 * files than full directory structure, but if this is problem it can 
 * be changed.
 */
static char *create_items_path (char *s) {
    static char buf[MAX_BUF];
    char *t;
    
    if (*s == '/')
      s++;

    sprintf (buf, "%s/%s/", settings.localdir, settings.uniquedir);

    for (t=buf+strlen(buf); *s; s++,t++)
      if (*s == '/')
          *t = '@';
      else 
          *t = *s;
    *t = 0;
    return (buf);  
}


/*
 * This function checks if a file with the given path exists.
 * -1 is returned if it fails, otherwise the mode of the file
 * is returned.
 * It tries out all the compression suffixes listed in the uncomp[] array.
 *
 * If prepend_dir is set, then we call create_pathname (which prepends
 * libdir & mapdir).  Otherwise, we assume the name given is fully
 * complete.
 */

int check_path (char *name, int prepend_dir)
{
    char buf[MAX_BUF], *endbuf;
    struct stat statbuf;
    int mode = 0, i;

    if (prepend_dir)
	strcpy (buf, create_pathname(name));
    else
	strcpy(buf, name);

    if ((endbuf = strchr(buf, '\0')) == NULL)
      return (-1);
   
    for (i = 0; i < NROF_COMPRESS_METHODS; i++) {
      if (uncomp[i][0])
        strcpy(endbuf, uncomp[i][0]);
      else
        *endbuf = '\0';
      if (!stat (buf, &statbuf))
        break;
    }
    if (i == NROF_COMPRESS_METHODS)
      return (-1);
    if (!S_ISREG (statbuf.st_mode))
	return (0);

    if (((statbuf.st_mode & S_IRGRP) && getegid() == statbuf.st_gid) ||
	((statbuf.st_mode & S_IRUSR) && geteuid() == statbuf.st_uid) ||
	(statbuf.st_mode & S_IROTH))
	mode |= 4;

    if ((statbuf.st_mode & S_IWGRP && getegid() == statbuf.st_gid) ||
	(statbuf.st_mode & S_IWUSR && geteuid() == statbuf.st_uid) ||
	(statbuf.st_mode & S_IWOTH))
	mode |= 2;
    
    return (mode);
}

void dump_map_lights(mapstruct *m) {
  objectlink *obl=m->light;
  object *tmp=NULL;
  while(obl) {
     if((tmp=obl->ob)) 
       LOG(llevDebug,"%s (%d) radius:%d\n",tmp->name,tmp->count,tmp->glow_radius); 
     obl=obl->next;
  } 
}

/*
 * Prints out debug-information about a map.
 */

void dump_map(mapstruct *m) {
  LOG(llevError,"Map %s status: %d.\n",m->path,m->in_memory);
  LOG(llevError,"Size: %dx%d Start: %d,%d\n",
          m->mapx,m->mapy,
	  EXIT_X(m->map_object), EXIT_Y(m->map_object));
  if(m->map_object->msg!=NULL)
    LOG(llevError,"Message:\n%s",m->map_object->msg);
  if(m->tmpname!=NULL)
    LOG(llevError,"Tmpname: %s\n",m->tmpname);
  LOG(llevError,"Difficulty: %d\n",m->difficulty); 
#ifdef USE_LIGHTING
  LOG(llevError,"Darkness: %d\n",m->darkness); 
#endif
}

/*
 * Prints out debug-information about all maps.
 */

void dump_all_maps() {
  mapstruct *m;
  for(m=first_map;m!=NULL;m=m->next) {
    LOG(llevError,"Map %s status %d.\n",m->path,m->in_memory);
    LOG(llevError,"Tmpname: %s\n",m->tmpname);
  }
}

/*
 * Returns true if a wall is present in a given location.
 */

int wall(mapstruct *m, int x,int y) {

	if (out_of_map(m,x,y))
		return 1;
	return (get_map(m,x,y))->flags & P_NO_PASS;
}

/*
 * Returns true if it's impossible to see through the given coordinate
 * in the given map.
 */

int blocks_view(mapstruct *m, int x, int y) {
  if(out_of_map(m,x,y))
    return 1;
  return (get_map(m,x,y))->flags & P_BLOCKSVIEW;
}

/*
 * Returns true if the given coordinate in the given map blocks magic.
 */

int blocks_magic(mapstruct *m, int x, int y) {
    if(out_of_map(m,x,y))
	return 1;
    return (get_map(m,x,y))->flags & P_NO_MAGIC;

}

/*
 * Returns true if clerical spells cannot work here
 */
int blocks_cleric(mapstruct *m, int x, int y) {
    if(out_of_map(m,x,y))
	return 1;
    return (get_map(m,x,y))->flags & P_NO_CLERIC;
}

/*
 * Returns true if the given coordinate in the given map blocks passage.
 */

int blocked(mapstruct *m, int x, int y) {
    int r;
    MapLook *f;
    if(out_of_map(m,x,y))
	return 1;
    f = get_map(m,x,y);
    r = f->flags & (P_NO_PASS | P_IS_ALIVE);
    return r;
}

/*
 * Returns true if the given coordinate in the map where the given object
 * is, blocks the given object (which may be multi-part)
 */

int blocked_link(object *ob, int x, int y) {
  object *tmp;
  if(out_of_map(ob->map,x,y))
    return 1;
  if (!blocked(ob->map,x,y)) /* no need to go further */
      return 0; 
  if(ob->head != NULL)
    ob=ob->head;
  for(tmp = get_map_ob(ob->map,x,y); tmp!= NULL; tmp = tmp->above)
    if (QUERY_FLAG(tmp,FLAG_NO_PASS) || (QUERY_FLAG(tmp,FLAG_ALIVE) &&
       tmp->head != ob && tmp != ob))
      	return 1;
  return 0;
}

/*
 * Eneq(@csd.uu.se): This is a new version of blocked, this one handles objects
 * that can be passed through by monsters with the CAN_PASS_THRU defined.
 * Returns 1 if the object can not pass through that space.
 * This code really needs to be rewritten so it would at least be somewhat
 * reasonable to read.
 */

int blocked_two(object *op, int x,int y) {
  object *tmp;
  if(out_of_map(op->map,x,y))
    return 1;
  tmp=get_map_ob(op->map,x,y);
  for(tmp=get_map_ob(op->map,x,y);tmp!=NULL;tmp=tmp->above){
    if((QUERY_FLAG(tmp,FLAG_ALIVE)&&tmp->type!=DOOR)||
	(QUERY_FLAG(tmp,FLAG_NO_PASS)&&!QUERY_FLAG(tmp,FLAG_PASS_THRU)&&tmp->type!=CHECK_INV)||
        (QUERY_FLAG(tmp,FLAG_NO_PASS)&&QUERY_FLAG(tmp,FLAG_PASS_THRU)&&
	!QUERY_FLAG(op,FLAG_CAN_PASS_THRU)&&tmp->type!=CHECK_INV))
      return 1;
    /* check inv doors by Florian Beck */
    /* if check_inv has last_sp=1, the player can only move if he has the 
     * object 
     */
    if (tmp->type==CHECK_INV&&QUERY_FLAG(tmp,FLAG_NO_PASS)) {
      if (tmp->last_sp){ /* player needs object */
	if (check_inv_recursive(op,tmp)==NULL) /* player doesn't have object 
						  so no pass */
	  return 1;
      } else
	/* player must *not* have object */
	if (check_inv_recursive(op,tmp)!=NULL) /* player has object */
	  return 1;
    }
  }
  /* end fb */
  return 0;
}

/*
 * Returns true if the given archetype can't fit in the given spot.
 */

int arch_blocked(archetype *at,mapstruct *m,int x,int y) {
  archetype *tmp;
  if(at==NULL)
    return blocked(m,x,y);
  for(tmp=at;tmp!=NULL;tmp=tmp->more)
    if(blocked(m,x+tmp->clone.x,y+tmp->clone.y))
      return 1;
  return 0;
}

/*
 * Returns true if the given archetype can't fit into the map at the
 * given spot (some part of it is outside the map-boundaries).
 */

int arch_out_of_map(archetype *at,mapstruct *m,int x,int y) {
  archetype *tmp;
  if(at==NULL)
    return out_of_map(m,x,y);
  for(tmp=at;tmp!=NULL;tmp=tmp->more)
    if(out_of_map(m,x+tmp->clone.x,y+tmp->clone.y))
      return 1;
  return 0;
}

/*
 * Goes through all objects in the given map, and does a sanity-check
 * on all pointers.
 */

void refresh_map(mapstruct *m) {
  int x,y;
  object *tmp,*tmp2=NULL,*active=NULL;

  if(m==NULL || m->in_memory != MAP_IN_MEMORY)
    return;
  for(x=0;x<m->mapx;x++)
    for(y=0;y<m->mapy;y++) {
/* Eneq(@csd.uu.se): Hunting down inappropriate objects in the map. The game
   sometime hangs and tries to remove_removed objects etc. */
      for(active=tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=tmp2)
      { /* how can we get objects with pointer<1024, mta */
        if (tmp < (object *) 1024) /* map is uninitialized? */
          break;
        tmp2=tmp->above;
        active=tmp;
        if (QUERY_FLAG(tmp,FLAG_REMOVED)) {
          LOG(llevError,"Crossfire: Found removed object in map.\n");
          active=(tmp->above==NULL?tmp->below:tmp->above);
          if (tmp->below==NULL&&tmp2==NULL)
            m->map_ob[x+m->mapx*y]=NULL;
          else if (tmp->below==NULL&&tmp2!=NULL) {
            tmp2->below=NULL;
            m->map_ob[x+m->mapx*y]=tmp2;
          } else if (tmp->below!=NULL&&tmp2==NULL)
            tmp->below->above=NULL;
          else if (tmp->below!=NULL&&tmp2!=NULL) {
            tmp->below->above=tmp2;
            tmp2->below=tmp->below;
          }
          if (!QUERY_FLAG(tmp,FLAG_FREED))
            free_object(tmp);
        }
        if (QUERY_FLAG(tmp,FLAG_FREED)&&!QUERY_FLAG(tmp,FLAG_REMOVED)) {
          LOG(llevError, "Crossfire: Found freed object in map.\n");
          remove_ob(tmp);
        }
      }

      if(active != NULL)
        update_object(active);
    }
}

/*
 * open_and_uncompress() first searches for the original filename.
 * if it exist, then it opens it and returns the file-pointer.
 * if not, it does two things depending on the flag.  If the flag
 * is set, it tries to create the original file by uncompressing a .Z file.
 * If the flag is not set, it creates a pipe that is used for
 * reading the file (NOTE - you can not use fseek on pipes)
 *
 * The compressed pointer is set to nonzero if the file is
 * compressed (and thus,  fp is actually a pipe.)  It returns 0
 * if it is a normal file
 *
 * (Note, the COMPRESS_SUFFIX is used instead of ".Z", thus it can easily
 * be changed in the config file.)
 */

FILE *open_and_uncompress(char *name,int flag, int *compressed) {
  FILE *fp;
  char buf[MAX_BUF],buf2[MAX_BUF], *bufend;
  int try_once = 0;

  strcpy(buf, name);
  bufend = buf + strlen(buf);

/*  LOG(llevDebug, "open_and_uncompress(%s)\n", name);
*/

  /* strip off any compression prefixes that may exist */
  for (*compressed = 0; *compressed < NROF_COMPRESS_METHODS; (*compressed)++) {
    if ((uncomp[*compressed][0]) &&
      (!strcmp(uncomp[*compressed][0], bufend - strlen(uncomp[*compressed][0])))) {
	buf[strlen(buf) - strlen(uncomp[*compressed][0])] = '\0';
	bufend = buf + strlen(buf);
    }
  }
  for (*compressed = 0; *compressed < NROF_COMPRESS_METHODS; (*compressed)++) {
    struct stat statbuf;

    if (uncomp[*compressed][0])
        strcpy(bufend, uncomp[*compressed][0]);
    if (stat(buf, &statbuf)) {

/*      LOG(llevDebug, "Failed to stat %s\n", buf);
*/
      continue;
    }
/*    LOG(llevDebug, "Found file %s\n", buf);
*/
    if (uncomp[*compressed][0]) {
      strcpy(buf2, uncomp[*compressed][1]);
      strcat(buf2, " < ");
      strcat(buf2, buf);
      if (flag) {
        int i;
        if (try_once) {
          LOG(llevError, "Failed to open %s after decompression.\n", name);
          return NULL;
        }
        try_once = 1;
        strcat(buf2, " > ");
        strcat(buf2, name);
        LOG(llevDebug, "system(%s)\n", buf2);
        if ((i=system(buf2))) {
          LOG(llevError, "system(%s) returned %d\n", buf2, i);
          return NULL;
        }
        unlink(buf);		/* Delete the original */
        *compressed = '\0';	/* Restart the loop from the beginning */
        chmod(name, statbuf.st_mode);
        continue;
      }
      if ((fp = popen(buf2, "r")) != NULL)
        return fp;
    } else if((fp=fopen(name,"r"))!=NULL)
      return fp;
  }
  LOG(llevDebug, "Can't open %s\n", name);
  return NULL;
}

/*
 * See open_and_uncompress().
 */

void close_and_delete(FILE *fp, int compressed) {
  if (compressed)
    pclose(fp);
  else
    fclose(fp);
}

/* When the map is loaded, load_object does not actually insert objects
 * into inventory, but just links them.  What this does is go through
 * and insert them properly.
 * The object 'container' is the object that contains the inventory.
 * This is needed so that we can update the containers weight.
 */


void fix_container(object *container)
{
    object *tmp=container->inv, *next;

    container->inv=NULL;
    while (tmp!=NULL) {
	next = tmp->below;
	if (tmp->inv)
	    fix_container(tmp);
	(void) insert_ob_in_ob(tmp,container);
	tmp = next;
    }
    /* sum_weight will go through and calculate what all the containers are
     * carrying.
     */
    sum_weight(container);
}

/*
 * Loads (ands parses) the objects into a given map from the specified
 * file pointer.
 * If block is true, the game will be blocked until all objects have
 * been loaded (this is needed to avoid certain critical regions).
 */

void load_objects (mapstruct *m, FILE *fp, int block) {
    int i;
#if defined(PROCESS_WHILE_LOADING) && CHECK_ACTIVE_MAPS
    int  j = 0;
#endif

    object *op, *prev=NULL,*last_more=NULL;
/*    MapLook	maplook;*/

    op=get_object();
    op->map = m; /* To handle buttons correctly */

    while((i=load_object(fp,op,LO_REPEAT))) {
	/* if the archetype for the object is null, means that we
	 * got an invalid object.  Don't do anythign with it - the game
	 * or editor will not be able to do anything with it either.
	 */
	if (op->arch==NULL) {
	    if (op->name!=NULL)
		LOG(llevDebug,"Discarded object %s - invalid archetype.\n",op->name);
	    continue;
	}
	/* this hack allows for 'unused' altars (see below) to have a 
 	 * random god (see the 'god table' in god.c) associated w/ it -b.t. 
	 */
#ifdef MULTIPLE_GODS
	if(op->type==ALTAR && !op->value && !editor && !op->stats.sp)
	   if(baptize_altar(op))
		LOG(llevDebug,"Baptized altar to %s\n",op->title);
#endif
	switch(i) {
	  case LL_NORMAL:
	    insert_ob_in_map(op,m);
#if 1
	    if (op->inv) sum_weight(op);
#else
	    if (op->inv) fix_container(op);
#endif
	    prev=op,last_more=op;
	    break;
#if 0	/* internal inventories should be handled by load routine */
	  case 2:
	    op = insert_ob_in_ob(op,prev);
	    break;
#endif
	  case LL_MORE:
	    insert_ob_in_map(op,m);
	    op->head=prev,last_more->more=op,last_more=op;
	    break;
	}

	/* Small hack to set the floor object for the maps.  This
	 * increases the performance for color pixmaps, as not the floor
	 * and top object are easily accessible.  Before, it had to
	 * search through the objects for the map space to find the
	 * floor.
	 *
	 * This should probably be someplace else, but I could not
	 * figure out the set_map_ob function, which then calls the
	 * set_map function, in where the floor is determined.
	 * I am also assuming that the floor can not change.  As such,
	 * the floor only needs to be determined once.
	 * Mark Wedel (master@rahul.net)
	 */
#if 0
	if (QUERY_FLAG(op,FLAG_IS_FLOOR)) {
		maplook.face = op->face;
		maplook.flags = 0;
		m->floor[op->x+ m->mapx*op->y] = maplook;
	}
#endif
	op=get_object();
        op->map = m;
#if defined(PROCESS_WHILE_LOADING) && CHECK_ACTIVE_MAPS
	if(!block && j++>CHECK_ACTIVE_MAPS) {
	    j=0;
	    /* We don't want to freeze the whole game too long */
	    (*process_active_maps_func)(); 
	}
#endif
    }
    free_object(op);
}

void save_objects (mapstruct *m, FILE *fp, FILE *fp2) {
    int i, j = 0,unique=0;
    object *op, *tmp, *otmp;
#if 0 /*defined(PROCESS_WHILE_LOADING) && CHECK_ACTIVE_MAPS*/
    int  elapsed = 0;
#endif
    /* first pass - save one-part objects */
    for(i = 0; i < m->mapx; i++)
	for (j = 0; j < m->mapy; j++) {
	    unique=0;
	    for(op = get_map_ob (m, i, j); op; op = otmp) {
		otmp = op->above;

		if (QUERY_FLAG(op,FLAG_IS_FLOOR) && QUERY_FLAG(op, FLAG_UNIQUE))
		    unique=1;

		if(op->type == PLAYER) {
			LOG(llevDebug, "Player on map that is being saved\n");
			continue;
		}

		if (op->head || op->more || op->owner)
		    continue;

		if (unique || QUERY_FLAG(op, FLAG_UNIQUE))
		    save_object( fp2 , op, 3);
		else
		    save_object(fp, op, 3);

/* Would be nice to let players still do stuff while a map is saving.
 * The problem is, having a player enter a map that is in the process
 * of being saved.  The maps needs to finish saving, and then be
 * re-loaded.
 */
#if 0 /*defined(PROCESS_WHILE_LOADING) && CHECK_ACTIVE_MAPS*/
	      if(elapsed++>CHECK_ACTIVE_MAPS) {
		elapsed=0;
		/* We don't want to freeze the whole game too long */
		(*process_active_maps_func)(); 
	      }
#endif
	    }
	}
    /* second pass - save multi-part objects */
    for(i = 0; i < m->mapx; i++)
        for (j = 0; j < m->mapy; j++) {
	    unique=0;
            for(op = get_map_ob (m, i, j); op; op = otmp) {
                otmp = op->above;

		if (QUERY_FLAG(op,FLAG_IS_FLOOR) && QUERY_FLAG(op, FLAG_UNIQUE))
		    unique=1;

		if(op->type == PLAYER) {
			LOG(llevDebug, "Player on map that is being saved\n");
			continue;
		}

		if (op->head || !op->more || op->owner)
		    continue;

		if (unique || QUERY_FLAG(op, FLAG_UNIQUE))
		    save_object( fp2 , op, 3);
		else
		    save_object(fp, op, 3);

		for (tmp = op->more; tmp; tmp = tmp->more) {
		    if (unique || QUERY_FLAG(op, FLAG_UNIQUE)) {
			fprintf ( fp2, "More\n");
			save_object( fp2 , tmp, 3);
		    } else {
			fprintf ( fp, "More\n");
			save_object(fp, tmp, 3);
		    }
		}
#if 0 /*defined(PROCESS_WHILE_LOADING) && CHECK_ACTIVE_MAPS*/
	      if(elapsed++>CHECK_ACTIVE_MAPS) {
		elapsed=0;
		/* We don't want to freeze the whole game too long */
		(*process_active_maps_func)(); 
	      }
#endif
	    }
	}
}

/*
 * Opens the file "filename" and reads information about the map
 * from the given file, and stores it in a newly allocated
 * mapstruct.  A pointer to this structure is returned, or NULL on failure.
 * flags correspond to those in map.h.  Main ones used are
 * MAP_PLAYER_UNIQUE, in which case we don't do any name changes, and
 * MAP_BLOCK, in which case we block on this load.
 */

mapstruct *load_original_map(char *filename, int flags) {
    FILE *fp;
    mapstruct *m;
    object *op;
    int comp;
    char pathname[MAX_BUF];
    
    LOG(llevDebug, "load_original_map: %s\n", filename);
    if (flags & MAP_PLAYER_UNIQUE) 
	strcpy(pathname, filename);
    else
	strcpy(pathname, create_pathname(filename));

    if((fp=open_and_uncompress(pathname, 0, &comp))==NULL) {
	LOG(llevError,"Can't open %s\n", pathname);
	perror("Can't read map file");
	return (NULL);
    }
        
    op = get_object();

    if (!load_object(fp, op,LO_NEWFILE) || op->type != MAP) {
	LOG(llevError,"Error in map (%s) - map object not found\n", filename);
        close_and_delete(fp, comp);
	return (NULL);
    }
    if (op->arch == NULL) {
      LOG(llevDebug, "Mapobject had no archetype\n");
      op->arch = find_archetype("map");
      if (op->arch == NULL)
        LOG(llevError, "Archetype map lacks.");
      if (op->arch->clone.type != MAP)
        LOG(llevDebug, "Archetype map has wrong type.");
    }
    m = get_empty_map (op->x, op->y);

    strcpy (m->path, filename);
    m->compressed = comp;

    if (m->map_object)
	free_object (m->map_object);
    m->map_object = op;
    op->map = m;

    m->in_memory=MAP_LOADING;
    load_objects (m, fp, (flags & MAP_BLOCK?1:0));
    close_and_delete(fp, comp);
    m->in_memory=MAP_IN_MEMORY;
    m->difficulty=calculate_difficulty(m);
    m->darkness=m->map_object->invisible;
    m->do_los=0;
    set_map_reset_time(m);

    return (m);
}

/*
 * Loads a map, which has been loaded earlier, from file.
 * Return the map object we load into (this can change from the passed
 * option if we can't find the original map)
 */

static mapstruct *load_temporary_map(mapstruct *m) {
    FILE *fp;
    object *op;
    int comp;
    char buf[MAX_BUF];
    
    if (!m->tmpname) {
	LOG(llevError, "No temporary filename for map %s\n", m->path);
	strcpy(buf, m->path);
	delete_map(m);
        m = load_original_map(buf,0);
	(*fix_auto_apply_func)(m); /* Chests which open as default */
	return m;
    }

    if((fp=open_and_uncompress(m->tmpname,0, &comp))==NULL) {
	LOG(llevError,"Can't open %s\n",m->tmpname);
	perror("Can't read map file");
	strcpy(buf, m->path);
	delete_map(m);
        m = load_original_map(buf,0);
	(*fix_auto_apply_func)(m); /* Chests which open as default */
	return m;
    }
    
    op = get_object();

    load_object(fp,op,LO_NEWFILE);
    if (op->arch == NULL || op->type != MAP) {
	LOG(llevError,"Error in temporary map '%s'\n", m->path);
        m = load_original_map(m->path,0);
	return m;
    }

    m->compressed = comp;


    if (m->map_object)
	free_object (m->map_object);
    m->map_object = op;

    allocate_map(m);
    clear_map(m);

    m->in_memory=MAP_LOADING;
    load_objects (m, fp, 0);
    close_and_delete(fp, comp);
    m->in_memory=MAP_IN_MEMORY;
    return m;
}

/******************************************************************************
 * This is the start of unique map handling code
 *****************************************************************************/

/* This goes through map 'm' and removed any unique items on the map. */
static void delete_unique_items(mapstruct *m)
{
    int i,j,unique=0;
    object *op, *next;

    for(i=0; i<m->map_object->x; i++)
	for(j=0; j<m->map_object->y; j++) {
	    unique=0;
	    for (op=get_map_ob(m, i, j); op; op=next) {
		next = op->above;
		if (QUERY_FLAG(op, FLAG_IS_FLOOR) && QUERY_FLAG(op, FLAG_UNIQUE))
		    unique=1;
		if(op->head == NULL && (QUERY_FLAG(op, FLAG_UNIQUE) || unique)) {
		    clean_object(op);
		    if (QUERY_FLAG(op, FLAG_IS_LINKED))
			remove_button_link(op);
		    remove_ob(op);
		    free_object(op);
		}
	    }
	}
}


/*
 * Loads unique objects from file(s) into the map which is in memory
 * m is the map to load unique items into.
 */
static void load_unique_objects(mapstruct *m) {
    FILE *fp;
    int comp,count;
    char firstname[MAX_BUF];

    for (count=0; count<10; count++) {
	sprintf(firstname, "%s.v%02d", create_items_path(m->path), count);
	if (!access(firstname, R_OK)) break;
    }
    /* If we get here, we did not find any map */
    if (count==10) return;

    if ((fp=open_and_uncompress(firstname, 0, &comp))==NULL) {
	/* There is no expectation that every map will have unique items, but this
	 * is debug output, so leave it in.
	 */
	LOG(llevDebug, "Can't open unique items file for %s\n", create_items_path(m->path));
      return;
    }

    m->in_memory=MAP_LOADING;
    if (m->tmpname == NULL)    /* if we have loaded unique items from */
      delete_unique_items(m); /* original map before, don't duplicate them */
    load_object(fp, NULL, LO_NOREAD);
    load_objects (m, fp, 0);
    close_and_delete(fp, comp);
    m->in_memory=MAP_IN_MEMORY;
}


/*
 * Saves a map to file.  If flag is set, it is saved into the same
 * file it was (originally) loaded from.  Otherwise a temporary
 * filename will be genarated, and the file will be stored there.
 * The temporary filename will be stored in the mapstructure.
 * If the map is unique, we also save to the filename in the map
 * (this should have been updated when first loaded)
 */

int new_save_map(mapstruct *m, int flag) {
    FILE *fp, *fp2;
    char filename[MAX_BUF],buf[MAX_BUF];
    
    if (flag && !*m->path) {
	LOG(llevError,"Tried to save map without path.\n");
	return -1;
    }
    
    if (flag || QUERY_FLAG(m->map_object, FLAG_UNIQUE)) {
	if (!QUERY_FLAG(m->map_object, FLAG_UNIQUE))
	    strcpy (filename, create_pathname (m->path));
	else
	    strcpy (filename, m->path);

	/* If the compression suffix already exists on the filename, don't
	 * put it on again.  This nasty looking strcmp checks to see if the
	 * compression suffix is at the end of the filename already.
	 */
        if (m->compressed &&
	  strcmp((filename + strlen(filename)-strlen(uncomp[m->compressed][0])),
	     uncomp[m->compressed][0]))
	          strcat(filename, uncomp[m->compressed][0]);
	make_path_to_file (filename);
    } else {
	if (!m->tmpname)
	    m->tmpname = tempnam_local(settings.tmpdir,NULL);
	strcpy(filename, m->tmpname);
    }
    LOG(llevDebug,"Saving map %s\n",m->path);
    m->in_memory = MAP_SAVING;

    if (!m->map_object) {
	LOG(llevError, "no map object for map %s!\n", m->path);
	return -1;
    }

    /* Compress if it isn't a temporary save.  Do compress if unique */
    if (m->compressed && ((QUERY_FLAG(m->map_object, FLAG_UNIQUE) || flag)) {
	char buf[MAX_BUF];
	strcpy(buf, uncomp[m->compressed][2]);
	strcat(buf, " > ");
	strcat(buf, filename);
	fp = popen(buf, "w");
    } else
	fp = fopen(filename, "w");

    if(fp == NULL) {
	perror("Can't open file for saving");
	return -1;
    }
    
    save_object (fp, m->map_object, 3);

    /* In the game save unique items in the different file, but
     * in the editor save them to the normal map file.
     * If unique map, save files in the proper destination (set by
     * player)
     */
    fp2 = fp; /* save unique items into fp2 */
    if (flag == 0 && !QUERY_FLAG(m->map_object, FLAG_UNIQUE)) {
	sprintf (buf,"%s.v00",create_items_path (m->path));
	if ((fp2 = fopen (buf, "w")) == NULL) {
	    LOG(llevError, "Can't open unique items file %s\n", buf);
	}
	save_objects (m, fp, fp2);
	if (fp2 != NULL) {
	    if (ftell (fp2) == 0) {
		fclose (fp2);
		unlink (buf);
	    } else {
		fclose (fp2);
		chmod (buf, SAVE_MODE);
	    }
	}
    } else { /* save same file when not playing, like in editor */
	save_objects (m, fp, fp);
    }

    if (m->compressed && !flag)
	pclose(fp);
    else
	fclose(fp);

    chmod (filename, SAVE_MODE);
    return 0;
}

/*
 * If any directories in the given path doesn't exist, they are created.
 */

void make_path_to_file (char *filename)
{
    char buf[MAX_BUF], *cp = buf;
    struct stat statbuf;

    if (!filename || !*filename)
	return;
    strcpy (buf, filename);
    LOG(llevDebug, "make_path_tofile %s...", filename);
    while ((cp = strchr (cp + 1, (int) '/'))) {
	*cp = '\0';
#if 0
	LOG(llevDebug, "\n Checking %s...", buf);
#endif
	if (stat(buf, &statbuf) || !S_ISDIR (statbuf.st_mode)) {
	    LOG(llevDebug, "Was not dir...");
	    if (mkdir (buf, 0777)) {
		perror ("Couldn't make path to file");
		return;
	    }
#if 0
	    LOG(llevDebug, "Made dir.");
	} else
	    LOG(llevDebug, "Was dir");
#else
	}
#endif
	*cp = '/';
    }
    LOG(llevDebug,"\n");
}


/*
 * Clears the arrays containing object-pointers and outlook of a map.
 */

void clear_map(mapstruct *m) {
    MapLook *aptr, *endptr;

    endptr=m->map+m->mapx*m->mapy;
    for(aptr=m->map; aptr < endptr; ++aptr)
	*aptr = blank_look;
    endptr=m->floor+m->mapx*m->mapy;
    for(aptr=m->floor; aptr < endptr; ++aptr)
	*aptr = blank_look;
    endptr=m->floor2+m->mapx*m->mapy;
    for(aptr=m->floor2; aptr < endptr; ++aptr)
	*aptr = blank_look;
    memset(m->map_ob, 0,sizeof(object *)*m->mapx*m->mapy);
}

/*
 * This function relinks all _pointers_ to the objects from
 * one map to another.
 * Note: You can _not_ free the objects in the original map
 * after this function has been called.
 * (What happened to this function? It no longer copies the pointers! -Frank)
 */

void copy_map(mapstruct *m1, mapstruct *m2) {
  int x,y;

  strncpy(m2->path,m1->path,BIG_NAME);
  x = m2->mapx;
  y = m2->mapy;
  copy_object (m1->map_object, m2->map_object);
  m2->mapx = x;
  m2->mapy = y;  

  for(x=0;x<m1->mapx&&x<m2->mapx;x++)
    for(y=0;y<m1->mapy&&y<m2->mapy;y++) {
      set_map(m2,x,y,get_map(m1,x,y));
    }
}

/*
 * This function, like copy_map(), relinks all _pointers_ from
 * one map to the other map.
 */

void relink_objs_offset(mapstruct *m1, mapstruct *m2, int dx, int dy) {
  int x,y;
  object *ob;

  for (x = 0; x < m1->mapx && x < m2->mapx + dx; x++)
    for (y = 0; y < m1->mapy && y < m2->mapy + dy; y++) {
      set_map(m2, x + dx, y + dy, get_map(m1, x, y));
      set_map_ob(m2, x + dx, y + dy, get_map_ob(m1, x, y));
      set_map_ob(m1, x, y, (object *) NULL);
      for (ob = get_map_ob(m2, x + dx, y + dy); ob; ob = ob->above)
      {
        ob->x = x + dx;
        ob->y = y + dy;
        ob->map = m2;
      }
    }
}

/*
 * Remove and free all objects in the inventory of the given object.
 * object.c ?
 */

void clean_object(object *op)
{
    object *tmp, *next;

    for(tmp = op->inv; tmp; tmp = next)
    {
	next = tmp->below;
	clean_object(tmp);
	if (QUERY_FLAG(tmp, FLAG_IS_LINKED))
	    remove_button_link(tmp);
	remove_ob(tmp);
	free_object(tmp);
    }
}

/*
 * Remove and free all objects in the given map.
 */

void free_all_objects(mapstruct *m) {
  int i,j;
  object *op;

  for(i=0;i<m->mapx;i++)
    for(j=0;j<m->mapy;j++) {
      object *previous_obj=NULL;
        while((op=get_map_ob(m,i,j))!=NULL) {
          if (op==previous_obj)
          {
            LOG(llevDebug, "free_all_objects: Link error, bailing out.\n");
            break;
          }
          previous_obj=op;
          if(op->head!=NULL)
          op = op->head;

	  /* If the map isn't in memory, free_object will remove and
	   * free objects in op's inventory.  So let it do the job.
	   */
	  if (m->in_memory==MAP_IN_MEMORY)
	    clean_object(op);
          remove_ob(op);
          free_object(op);
        }
      }
}

/*
 * This function moves all objects from one map to another.
 *
 * move_all_objects(): Only used from the editor(s?)
 * Yes. -Frank
 */

void move_all_objects(mapstruct *m1, mapstruct *m2) {
  int i,j;
  object *op;

  for(i=0;i<m1->mapx&&i<m2->mapx;i++)
    for(j=0;j<m1->mapy&&j<m2->mapy;j++) {
      while((op=get_map_ob(m1,i,j))!=NULL&&op->head==NULL) {
        remove_ob(op);
        op->x=i,op->y=j; /* Not really needed */
        insert_ob_in_map(op,m2);
      }
    }
  free_all_objects(m1);
}

/*
 * function: vanish mapstruct
 * m       : pointer to mapstruct, if NULL no action
 */

void delete_map(mapstruct *m) {
    mapstruct *tmp;

    if (!m)
      return;
    if (m->in_memory == MAP_IN_MEMORY)
	free_map (m, 1);
    /* move this out of free_map, since tmpname can still be needed if
     * the map is swapped out.
     */
    if (m->tmpname) {
	free(m->tmpname);
	m->tmpname=NULL;
    }
    if (m == first_map)
	first_map = m->next;
    else {
	for (tmp = first_map; tmp && tmp->next != m; tmp = tmp->next);
        if(tmp)
	  tmp->next = m->next;
    }
    free (m);
}

/*
 * Frees everything allocated by the given mapstructure.
 */

void free_map(mapstruct *m,int flag) {
  if (!m->in_memory) {
    LOG(llevError,"Trying to free freed map.\n");
    return;
  }
  if(flag)
    free_all_objects(m);
  m->in_memory=MAP_SWAPPED;
  if (m->map != NULL)
    CFREE(m->map);
  if (m->map_ob != NULL)
    CFREE(m->map_ob);
  if (m->floor !=NULL)
    CFREE(m->floor);
  if (m->floor2 !=NULL)
    CFREE(m->floor2);
  m->map = NULL;/* It's being accessed again, incorrectly.  Maybe this will */
		/* make it get loaded back in. */
  m->map_ob = NULL;
  m->floor = NULL;
  m->floor2 = NULL;
  if (m->map_object)
      free_object (m->map_object);
  m->map_object=NULL;
  if (m->buttons)
    free_objectlinkpt(m->buttons);
  m->buttons = NULL;
}

/*
 * Allocates, initialises, and returns a pointer to a mapstruct.
 * takes a path argument it doesn't look like it uses.
 */

mapstruct *get_linked_map(char *path) {
  mapstruct *map=(mapstruct *) CALLOC(1,sizeof(mapstruct));
  mapstruct *mp;

  if(map==NULL)
    fatal(OUT_OF_MEMORY);
  for(mp=first_map;mp!=NULL&&mp->next!=NULL;mp=mp->next);
  if(mp==NULL)
    first_map=map;
  else
    mp->next=map;
  map->next=NULL;
  map->path[0]='\0';
  map->tmpname=NULL;
  map->encounter = 0;
  map->players=0;
  map->in_memory=MAP_SWAPPED;
/*  map->read_only=0;*/
  map->timeout=0;
  map->light = (objectlink *) NULL;
  map->darkness = 0;
  map->do_los=0;
  map->pending = (objectlink *) NULL;
  map->map_object = NULL;
  map->buttons = NULL;
  map->compressed = 0;

  return map;
}

/*
 * Allocates the arrays contained in a mapstruct.
 */

void allocate_map(mapstruct *m) {
  if(m->in_memory != MAP_SWAPPED )
    return;
  m->in_memory = MAP_IN_MEMORY;
  m->map=(MapLook *) CALLOC(m->mapx*m->mapy,sizeof(MapLook));
  m->floor=(MapLook *) CALLOC(m->mapx*m->mapy,sizeof(MapLook));
  m->map_ob=(object **) CALLOC(m->mapx*m->mapy,sizeof(object *));
  m->floor2=(MapLook *) CALLOC(m->mapx*m->mapy,sizeof(MapLook));
  if(m->map==NULL||m->map_ob==NULL || m->floor==NULL || m->floor2==NULL)
    fatal(OUT_OF_MEMORY);
}

/*
 * Creates an empty map of the given size, and returns a pointer to it.
 */

mapstruct *get_empty_map (int sizex, int sizey) {
    mapstruct *m = get_linked_map (NULL);
    archetype *tmp;

    for (tmp = first_archetype; tmp; tmp = tmp->next)
	if (tmp->clone.type == MAP)
	    break;
    if (tmp) {
	m->map_object = ObjectCreateArch (tmp);
	m->mapx = sizex;
	m->mapy = sizey;
	allocate_map (m);
	clear_map (m);
    } else {
	m->map_object = NULL;
	LOG (llevError, "no map archetypes\n");
    }
    return m;
}

/*
 * Makes sure the given map is loaded and swapped in.
 * name is path name of the map.
 * flags meaning:
 * 0x1 (MAP_FLUSH): flush the map - always load from the map directory,
 *   and don't do unique items or the like.
 * 0x2 (MAP_PLAYER_UNIQUE) - this is a unique map for each player.
 *   dont do any more name translation on it.
 *
 * Returns a pointer to the given map.
 */

mapstruct *ready_map_name(char *name, int flags) {
    mapstruct *m;

    if (!name)
       return (NULL);

    /* Have we been at this level before? */
    m = has_been_loaded (name);

    if (m && (m->in_memory == MAP_LOADING || m->in_memory == MAP_IN_MEMORY))
	return m;

    if ((flags & (MAP_FLUSH|MAP_PLAYER_UNIQUE)) || !m || (m->reset_time != -1 && seconds() > m->reset_time)) {

	/* first visit or time to reset */
	if (m) {
	    clean_tmp_map(m);	/* Doesn't make much difference */
	    delete_map(m);
	}

	/* create and load a map */
	if (flags & MAP_PLAYER_UNIQUE)
	    LOG(llevDebug, "Trying to load map %s.\n", name);
	else
	    LOG(llevDebug, "Trying to load map %s.\n", create_pathname(name));

	if (!(m = load_original_map(name, (flags & MAP_PLAYER_UNIQUE))))
	    return (NULL);

	(*fix_auto_apply_func)(m); /* Chests which open as default */

	/* If a player unique map, no extra unique object file to load.
	 * if from the editor, likewise.
	 */
	if (! (flags & (MAP_FLUSH|MAP_PLAYER_UNIQUE))) 
	    load_unique_objects(m);

    } else {
	/* If in this loop, we found a temporary map, so load it up. */

	m=load_temporary_map (m);
	load_unique_objects(m);

	clean_tmp_map(m);
	m->in_memory = MAP_IN_MEMORY;
	/* tempnam() on sun systems (probably others) uses malloc
	 * to allocated space for the string.  Free it here.
	 * In some cases, load_temporary_map above won't find the
	 * temporary map, and so has reloaded a new map.  If that
	 * is the case, tmpname is now null
	 */
	if (m->tmpname) free(m->tmpname);
	m->tmpname = NULL;
	/* It's going to be saved anew anyway */
    } 

    /* Below here is stuff common to both first time loaded maps and
     * temp maps.
     */

    /* In case other objects press some buttons down */
    (*update_buttons_func)(m);

    return m;
}



void no_maps_file(char *filename) {
  LOG(llevError,"Can't open the %s file.\n",filename);
  LOG(llevError,"If you have no maps, you must either make them yourself\n");
  LOG(llevError,"with the mapeditor, or fetch them from ftp.ifi.uio.no\n");
  LOG(llevError,"in the /pub/crossfire directory.  Be sure not to fetch\n");
  LOG(llevError,"maps belonging to a later version than this version.\n");
  exit(-1);
}


void set_map_reset_time(mapstruct *map) {
#ifdef MAP_RESET
#ifdef MAP_MAXRESET
    if (MAP_RESETTIME(map)>MAP_MAXRESET)
	map->reset_time = seconds() + MAP_MAXRESET;
    else
#endif /* MAP_MAXRESET */
    map->reset_time = seconds() + MAP_RESETTIME (map);
#else
    map->reset_time = (-1); /* Will never be reset */
#endif
}

/*
 * This routine is supposed to find out which level the players should have
 * before visiting this map.  It is used to calculate which bonuses to put
 * on magic items.
 *
 * Difficulty can now be set by the map creature.  If the value stored
 * in the map is zero, then use this routine.
 */

int calculate_difficulty(mapstruct *m) {
  object *op;
  archetype *at;
  int x,y;
  int diff=0;
  int total_exp=0,exp_pr_sq;
  int i;

  if (MAP_DIFFICULTY(m)) {
	LOG(llevDebug, "Using stored map difficulty: %d\n", MAP_DIFFICULTY(m));
	return MAP_DIFFICULTY(m);
  }
  for(x=0;x<m->mapx;x++)
    for(y=0;y<m->mapy;y++)
      for(op=get_map_ob(m,x,y);op!=NULL;op=op->above) {
        if(QUERY_FLAG(op,FLAG_MONSTER))
          total_exp+=op->stats.exp;
        if(QUERY_FLAG(op,FLAG_GENERATOR)) {
          total_exp+=op->stats.exp;
          at=type_to_archetype(GENERATE_TYPE(op));
          if(at!=NULL)
            total_exp+=at->clone.stats.exp*8;
        }
      }
#ifdef NEWCALC
  (int)exp_pr_sq=((double)1000*total_exp)/(m->mapx*m->mapy+1);
  for(i=20;i>0;i--)
    if(exp_pr_sq>level_exp(i,1.0)) {
      diff=i;
      break;
    }
#else
  exp_pr_sq=((double)1000*total_exp)/(m->mapx*m->mapy+1);
  diff=20;
  for(i=1;i<20;i++)
    if(exp_pr_sq<=level_exp(i,1.0)) {
      diff=i;
      break;
    }
#endif
  return diff;
}

void clean_tmp_map(mapstruct *m) {
  if(m->tmpname == NULL)
    return;
  (void) unlink(m->tmpname);
}

/*
 * member: copy by translate objects from source to a new map
 * source: -map
 * width : width of target map
 * height: height of target map
 * dx    : positive translate to right
 * dy    : positive translate to down
 */
mapstruct *MapMoveScrollResize(mapstruct *source, 
				int width, int height, int dx, int dy) 
{
    mapstruct *target;
    object *obj,*prt; /* PaRT of obj */
    int x,y,sx = source->mapx, sy = source->mapy;
    int linked = 0, link=0;

    if (!width) width = sx;
    if (!height) height = sy;
    target = get_empty_map (width, height);

    strncpy (target->path, source->path, BIG_NAME);

    copy_object (source->map_object, target->map_object);
    target->mapx = width;
    target->mapy = height;  

    if(dx < 0) dx += target->mapx;
    if(dy < 0) dy += target->mapy;

    for(y=0; y < sy && y < target->mapy; y++)
	for(x=0; x < sx && x < target->mapx; x++)
	    while((obj = get_map_ob(source,x,y)) && !obj->head) {
		if ((linked = QUERY_FLAG (obj,FLAG_IS_LINKED))) {
		    link = get_button_value (obj);
		    remove_button_link (obj);
		}
		remove_ob(obj);
		for(prt = obj; prt; prt = prt->more) {
		    prt->x += dx;
		    prt->x %= target->mapx; /* it can be split by edge */
		    prt->y += dy;           /* designers problem to fix */
		    prt->y %= target->mapy;
		}
		insert_ob_in_map(obj,target);
		if (linked)
		    add_button_link(obj, target, link);
	    }
    /*free_all_objects(source);*/
    free_map (source, 1);
    delete_map (source);
    return target;
}

/*
 * member: copy by translate objects from source to target
 * target: -map
 * source: -map
 * dx    : positive translate to right
 * dy    : positive translate to down
 */
void MapMoveScroll(mapstruct *target, mapstruct *source, int dx, int dy) 
{
    object *obj,*prt; /* PaRT of obj */
    int x,y;

    if(dx < 0) dx += target->mapx;
    if(dy < 0) dy += target->mapy;

    for(y=0; y < source->mapy && y < target->mapy; y++)
	for(x=0; x < source->mapx && x < target->mapx; x++)
	    while((obj = get_map_ob(source,x,y)) && !obj->head) {
		remove_ob(obj);
		for(prt = obj; prt; prt = prt->more) {
		    prt->x += dx;
		    prt->x %= target->mapx; /* it can be split by edge */
		    prt->y += dy;           /* designers problem to fix */
		    prt->y %= target->mapy;
		}
		insert_ob_in_map(obj,target);
	    }
    free_all_objects(source);
}

object * MapGetRealObject (mapstruct * emap, int x, int y, int z)
{
    object *tmp = MapGetObjectZ (emap, x, y, z);
    return tmp ? (tmp->head ? tmp->head : tmp) : tmp;
}

int MapInsertObjectZ(mapstruct *emap,object *o,int x, int y, int z)
{
    object *op, *above, *below;

    if (o->more)
        MapInsertObjectZ (emap,o->more, x, y, z);

    o->x += x;
    o->y += y;
    o->map = emap;
    CLEAR_FLAG(o,FLAG_REMOVED);

    op = get_map_ob (emap, o->x, o->y);
    if (z < 0) {
	above = op;
	below = NULL;
    } else {
	while (op && op->above)
	    op = op->above;
    
	above = NULL;
	below = op;
	while (op && z-- > 0) {
	    above = op;
	    below = op = op->below;
	}
    }
    o->below = below;
    o->above = above;

    if (above)
        above->below = o;
    else {
	MapLook f;
	f.flags = 0;
	f.face = o->face;
	set_map (emap, o->x, o->y, &f);
    }
    if (below)
        below->above = o;
    else
        set_map_ob (emap, o->x, o->y, o);
    
    return (0);
}

int MapObjectOut (mapstruct *target, object *obj, int x, int y) {
    object *tmp;
    for(tmp = obj; tmp; tmp = tmp->more)
        if(out_of_map(target,x + tmp->x,y + tmp->y)) return 1;
    return 0;
}

object * MapGetObjectZ (mapstruct * emap, int x, int y, int z)
{
    object *op;

    if (!emap || out_of_map (emap, x, y))
        return (NULL);
    op = get_map_ob (emap, x, y);
    while (op && op->above)
        op = op->above;
    while (op && z-- > 0)
        op = op->below;
    return (op);
}


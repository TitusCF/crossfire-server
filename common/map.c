/*
 * static char *rcsid_map_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001-2003 Mark Wedel & Crossfire Development Team
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
#include <funcpoint.h>

#include <loader.h>
#ifndef WIN32 /* ---win32 exclude header */
#include <unistd.h>
#endif /* win32 */

extern int nrofallocobjects,nroffreeobjects;

#if 0
/* If 0 this block because I don't know if it is still needed.
 * if it is, it really should be done via autoconf now days
 * and not by specific machine checks.
 */

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
#endif



/*
 * Returns the mapstruct which has a name matching the given argument.
 * return NULL if no match is found.
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
 * same as create_pathname, but for the overlay maps.
 */

char *create_overlay_pathname (char *name) {
    static char buf[MAX_BUF];

    /* Why?  having extra / doesn't confuse unix anyplace?  Dependancies
     * someplace else in the code? msw 2-17-97
     */
    if (*name == '/')
      sprintf (buf, "%s/%s%s", settings.localdir, settings.mapdir, name);
    else
      sprintf (buf, "%s/%s/%s", settings.localdir, settings.mapdir, name);
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
 * Only the editor actually cares about the writablity of this -
 * the rest of the code only cares that the file is readable.
 * when the editor goes away, the call to stat should probably be
 * replaced by an access instead (similar to the windows one, but
 * that seems to be missing the prepend_dir processing
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
#ifdef WIN32 /* ***win32: check this sucker in windows style. */
	return(_access(name,0));
#else

    /* old method (strchr(buf, '\0')) seemd very odd to me -
     * this method should be equivalant and is clearer.
     * Can not use strcat because we need to cycle through
     * all the names.
     */
    endbuf = buf + strlen(buf);
   
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
	return (-1);

    if (((statbuf.st_mode & S_IRGRP) && getegid() == statbuf.st_gid) ||
	((statbuf.st_mode & S_IRUSR) && geteuid() == statbuf.st_uid) ||
	(statbuf.st_mode & S_IROTH))
	mode |= 4;

    if ((statbuf.st_mode & S_IWGRP && getegid() == statbuf.st_gid) ||
	(statbuf.st_mode & S_IWUSR && geteuid() == statbuf.st_uid) ||
	(statbuf.st_mode & S_IWOTH))
	mode |= 2;
    
    return (mode);
#endif
}

/*
 * Prints out debug-information about a map.
 * Dumping these at llevError doesn't seem right, but is
 * necessary to make sure the information is in fact logged.
 */

void dump_map(mapstruct *m) {
    LOG(llevError,"Map %s status: %d.\n",m->path,m->in_memory);
    LOG(llevError,"Size: %dx%d Start: %d,%d\n",
	MAP_WIDTH(m), MAP_HEIGHT(m),
	MAP_ENTER_X(m), MAP_ENTER_Y(m));

    if(m->msg!=NULL)
	LOG(llevError,"Message:\n%s",m->msg);

    if(m->tmpname!=NULL)
	LOG(llevError,"Tmpname: %s\n",m->tmpname);

    LOG(llevError,"Difficulty: %d\n",m->difficulty); 
    LOG(llevError,"Darkness: %d\n",m->darkness); 
}

/*
 * Prints out debug-information about all maps.
 * This basically just goes through all the maps and calls
 * dump_map on each one.
 */

void dump_all_maps() {
    mapstruct *m;
    for(m=first_map;m!=NULL;m=m->next) {
	dump_map(m);
  }
}

/* This rolls up wall, blocks_magic, blocks_view, etc, all into
 * one function that just returns a P_.. value (see map.h)
 * it will also do map translation for tiled maps, returning
 * new values into newmap, nx, and ny.  Any and all of those
 * values can be null, in which case if a new map is needed (returned
 * by a P_NEW_MAP value, another call to get_map_from_coord
 * is needed.  The case of not passing values is if we're just
 * checking for the existence of something on those spaces, but
 * don't expect to insert/remove anything from those spaces.
 */
int get_map_flags(mapstruct *oldmap, mapstruct **newmap, sint16 x, sint16 y, sint16 *nx, sint16 *ny)
{
    int newx, newy, retval=0;
    mapstruct *mp;

    if (out_of_map(oldmap, x, y)) return P_OUT_OF_MAP;
    newx = x;
    newy = y;
    mp = get_map_from_coord(oldmap, &newx, &newy);
    if (mp != oldmap)
	retval |= P_NEW_MAP;
    if (newmap) *newmap = mp;
    if (nx) *nx = newx;
    if (ny) *ny = newy;
    retval |= mp->spaces[newx + mp->width * newy].flags;
    return retval;
}


/*
 * Returns true if the given coordinate is blocked by the
 * object passed is not blocking.  This is used with 
 * multipart monsters - if we want to see if a 2x2 monster
 * can move 1 space to the left, we don't want its own area
 * to block it from moving there.
 * Returns TRUE if the space is blocked by something other than the
 * monster.
 */

int blocked_link(object *ob, int x, int y) {
    object *tmp;
    mapstruct	*m;
    sint16  sx, sy;
    int mflags;

    sx = x;
    sy = y;
    m = ob->map;

    mflags = get_map_flags(m, &m, sx, sy, &sx, &sy);

    if (mflags & P_OUT_OF_MAP)
	return 1;


    /* If space is currently not blocked by anything, no need to
     * go further.  Not true for players - all sorts of special
     * things we need to do for players.
     */
    if (ob->type != PLAYER && ! (mflags & (P_NO_PASS | P_IS_ALIVE))) return 0;


    if(ob->head != NULL)
	ob=ob->head;

    /* We basically go through the stack of objects, and if there is
     * some other object that has NO_PASS or FLAG_ALIVE set, return
     * true.  If we get through the entire stack, that must mean
     * ob is blocking it, so return 0.
     */
    for(tmp = GET_MAP_OB(m,sx,sy); tmp!= NULL; tmp = tmp->above) {

	/* This must be before the checks below.  Code for inventory checkers. */
	if (tmp->type==CHECK_INV && QUERY_FLAG(tmp,FLAG_NO_PASS)) {
	    /* If last_sp is set, the player/monster needs an object,
	     * so we check for it.  If they don't have it, they can't
	     * pass through this space.
	     */
	    if (tmp->last_sp) {
		if (check_inv_recursive(ob,tmp)==NULL)
		    return 1;
		else
		    continue;
	    } else {
		/* In this case, the player must not have the object -
		 * if they do, they can't pass through.
		 */
		if (check_inv_recursive(ob,tmp)!=NULL) /* player has object */
		    return 1;
		else
		    continue;
	    }
	} /* if check_inv */
	else if (QUERY_FLAG(tmp,FLAG_NO_PASS) || (QUERY_FLAG(tmp,FLAG_ALIVE) &&
		     tmp->head != ob && tmp != ob && tmp->type != DOOR))
			return 1;

    }
    return 0;
}


/*
 * Returns true if the given archetype can't fit in the given spot.
 * This is meant for multi space objects - for single space objecs,
 * just calling get_map_flags and checking the P_BLOCKED is
 * sufficient.  This function goes through all the parts of the
 * multipart object and makes sure they can be inserted.
 *
 * While this doesn't call out of map, the get_map_flags does.
 *
 * This function has been used to deprecate arch_out_of_map -
 * this function also does that check, and since in most cases,
 * a call to one would follow the other, doesn't make a lot of sense to
 * have two seperate functions for this.
 *
 * This returns nonzero if this arch can not go on the space provided,
 * 0 otherwise.  the return value will contain the P_.. value
 * so the caller can know why this object can't go on the map.
 * Note that callers should not expect P_NEW_MAP to be set
 * in return codes - since the object is multispace - if
 * we did return values, what do you return if half the object
 * is one map, half on another.
 */

int arch_blocked(archetype *at,mapstruct *m,int x,int y) {
    archetype *tmp;
    int flag;

    if(at==NULL)
	return get_map_flags(m,NULL, x,y, NULL, NULL) & (P_BLOCKED | P_OUT_OF_MAP);

    for(tmp=at;tmp!=NULL;tmp=tmp->more) {
	flag = get_map_flags(m, NULL, x+tmp->clone.x,y+tmp->clone.y, NULL, NULL);
	if (flag & (P_BLOCKED | P_OUT_OF_MAP))
	    return (flag & (P_BLOCKED | P_OUT_OF_MAP));
    }
    return 0;
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

/* link_multipart_objects go through all the objects on the map looking
 * for objects whose arch says they are multipart yet according to the
 * info we have, they only have the head (as would be expected when
 * they are saved).  We do have to look for the old maps that did save
 * the more sections and not re-add sections for them.
 */

static void link_multipart_objects(mapstruct *m)
{
    int x,y;
    object *tmp, *op, *last, *above;
    archetype	*at;

    for(x=0;x<MAP_WIDTH(m);x++)
	for(y=0;y<MAP_HEIGHT(m);y++)
	    for(tmp=get_map_ob(m,x,y);tmp!=NULL;tmp=above) {
		above=tmp->above;

		/* already multipart - don't do anything more */
		if (tmp->head || tmp->more) continue;

		/* If there is nothing more to this object, this for loop
		 * won't do anything.
		 */
		for (at = tmp->arch->more, last=tmp; at != NULL; at=at->more, last=op) {
		    op = arch_to_object(at);

		    /* update x,y coordinates */
		    op->x += tmp->x;
		    op->y += tmp->y;
		    op->head = tmp;
		    op->map = m;
		    last->more = op;
		    if (tmp->name != op->name) {
			if (op->name) free_string(op->name);
			op->name = add_string(tmp->name);
		    }
		    if (tmp->title != op->title) {
			if (op->title) free_string(op->title);
			op->title = add_string(tmp->title);
		    }
		    /* we could link all the parts onto tmp, and then just
		     * call insert_ob_in_map once, but the effect is the same,
		     * as insert_ob_in_map will call itself with each part, and
		     * the coding is simpler to just to it here with each part.
		     */
		    insert_ob_in_map(op, op->map, tmp,INS_NO_MERGE|INS_ABOVE_FLOOR_ONLY|INS_NO_WALK_ON);
		} /* for at = tmp->arch->more */
	    } /* for objects on this space */
}
		    
		

/*
 * Loads (ands parses) the objects into a given map from the specified
 * file pointer.
 * mapflags is the same as we get with load_original_map
 */

void load_objects (mapstruct *m, FILE *fp, int mapflags) {
    int i,bufstate=LO_NEWFILE;
    int unique;
    object *op, *prev=NULL,*last_more=NULL, *otmp, *ootmp;;

    op=get_object();
    op->map = m; /* To handle buttons correctly */

    while((i=load_object(fp,op,bufstate, mapflags))) {
	/* Since the loading of the map header does not load an object
	 * anymore, we need to pass LO_NEWFILE for the first object loaded,
	 * and then switch to LO_REPEAT for faster loading.
	 */
	bufstate = LO_REPEAT;

	/* if the archetype for the object is null, means that we
	 * got an invalid object.  Don't do anythign with it - the game
	 * or editor will not be able to do anything with it either.
	 */
	if (op->arch==NULL) {
	    if (op->name!=NULL)
		LOG(llevDebug,"Discarded object %s - invalid archetype.\n",op->name);
	    continue;
	}

	/* check for unique items, or unique squares */
	unique = 0;
	for (otmp = get_map_ob(m, op->x, op->y); otmp; otmp = ootmp) {
	    ootmp = otmp->above;
	    if (QUERY_FLAG(otmp, FLAG_UNIQUE))
		unique = 1;
	}
	if (QUERY_FLAG(op, FLAG_UNIQUE) || QUERY_FLAG(op, FLAG_OBJ_SAVE_ON_OVL))
	    unique = 1;
	if (!(mapflags & (MAP_OVERLAY|MAP_PLAYER_UNIQUE) || unique))
	   SET_FLAG(op, FLAG_OBJ_ORIGINAL);

	switch(i) {
	  case LL_NORMAL:
	    /* if we are loading an overlay, put the floors on the bottom */
	    if ((QUERY_FLAG(op, FLAG_IS_FLOOR) ||
		 QUERY_FLAG(op, FLAG_OVERLAY_FLOOR)) && mapflags & MAP_OVERLAY)
		insert_ob_in_map(op,m,op,INS_NO_MERGE | INS_NO_WALK_ON | INS_ABOVE_FLOOR_ONLY);
	    else
		insert_ob_in_map(op,m,op,INS_NO_MERGE | INS_NO_WALK_ON | INS_ON_TOP);
	    if (op->inv) sum_weight(op);
	    prev=op,last_more=op;
	    break;

	  case LL_MORE:
	    insert_ob_in_map(op,m, op, INS_NO_MERGE | INS_NO_WALK_ON | INS_ABOVE_FLOOR_ONLY);
	    op->head=prev,last_more->more=op,last_more=op;
	    break;
	}
	if (mapflags & MAP_STYLE) {
	    remove_from_active_list(op);
	}
	op=get_object();
        op->map = m;
    }
    free_object(op);
    link_multipart_objects(m);
}

/* This saves all the objects on the map in a non destructive fashion.
 * Modified by MSW 2001-07-01 to do in a single pass - reduces code,
 * and we only save the head of multi part objects - this is needed
 * in order to do map tiling properly.
 */
void save_objects (mapstruct *m, FILE *fp, FILE *fp2, int flag) {
    int i, j = 0,unique=0;
    object *op,  *otmp;
    /* first pass - save one-part objects */
    for(i = 0; i < MAP_WIDTH(m); i++)
	for (j = 0; j < MAP_HEIGHT(m); j++) {
	    unique=0;
	    for(op = get_map_ob (m, i, j); op; op = otmp) {
		otmp = op->above;

		if (QUERY_FLAG(op,FLAG_IS_FLOOR) && QUERY_FLAG(op, FLAG_UNIQUE))
		    unique=1;

		if(op->type == PLAYER) {
			LOG(llevDebug, "Player on map that is being saved\n");
			continue;
		}

		if (op->head || op->owner)
		    continue;

		if (unique || QUERY_FLAG(op, FLAG_UNIQUE))
		    save_object( fp2 , op, 3);
		else
		    if (flag == 0 ||
			(flag == 2 && (!QUERY_FLAG(op, FLAG_OBJ_ORIGINAL) &&
			 !QUERY_FLAG(op, FLAG_UNPAID))))
		    	save_object(fp, op, 3);

#if 0
		for (tmp = op->more; tmp; tmp = tmp->more) {
		    if (unique || QUERY_FLAG(op, FLAG_UNIQUE)) {
			fprintf ( fp2, "More\n");
			save_object( fp2 , tmp, 3);
		    } else {
			fprintf ( fp, "More\n");
			save_object(fp, tmp, 3);
		    }
		}
#endif
	    } /* for this space */
	} /* for this j */
}

/*
 * Allocates, initialises, and returns a pointer to a mapstruct.
 * Modified to no longer take a path option which was not being
 * used anyways.  MSW 2001-07-01
 */

mapstruct *get_linked_map() {
    mapstruct *map=(mapstruct *) calloc(1,sizeof(mapstruct));
    mapstruct *mp;

    if(map==NULL)
	fatal(OUT_OF_MEMORY);

    for(mp=first_map;mp!=NULL&&mp->next!=NULL;mp=mp->next);
    if(mp==NULL)
	first_map=map;
    else
	mp->next=map;

    map->in_memory=MAP_SWAPPED;
    /* The maps used to pick up default x and y values from the
     * map archetype.  Mimic that behaviour.
     */
    MAP_WIDTH(map)=16;
    MAP_HEIGHT(map)=16;
    MAP_RESET_TIMEOUT(map)=7200;
    MAP_TIMEOUT(map)=300;
    /* Gah - these should really have a zero default! */
    MAP_ENTER_X(map)=1;
    MAP_ENTER_Y(map)=1;
	/*set part to -1 indicating conversion to weather map not yet done*/
	MAP_WORLDPARTX(map)=-1;
	MAP_WORLDPARTY(map)=-1;
    return map;
}

/*
 * Allocates the arrays contained in a mapstruct.
 * This basically allocates the dynamic array of spaces for the
 * map.
 */

void allocate_map(mapstruct *m) {
    m->in_memory = MAP_IN_MEMORY;
    /* Log this condition and free the storage.  We could I suppose
     * realloc, but if the caller is presuming the data will be intact,
     * that is their poor assumption.
     */
    if (m->spaces) {
	LOG(llevError,"allocate_map callled with already allocated map (%s)\n", m->path);
	free(m->spaces);
    }

    m->spaces = calloc(1, MAP_WIDTH(m) * MAP_HEIGHT(m) * sizeof(MapSpace));

    if(m->spaces==NULL)
	fatal(OUT_OF_MEMORY);
}

/* Creatures and returns a map of the specific size.  Used
 * in random map code and the editor.
 */
mapstruct *get_empty_map(int sizex, int sizey) {
    mapstruct *m = get_linked_map();
    m->width = sizex;
    m->height = sizey;
    m->in_memory = MAP_SWAPPED;
    allocate_map(m);
    return m;
}

/* This loads the header information of the map.  The header
 * contains things like difficulty, size, timeout, etc.
 * this used to be stored in the map object, but with the
 * addition of tiling, fields beyond that easily named in an
 * object structure were needed, so it just made sense to
 * put all the stuff in the map object so that names actually make
 * sense.
 * This could be done in lex (like the object loader), but I think
 * currently, there are few enough fields this is not a big deal.
 * MSW 2001-07-01
 * return 0 on success, 1 on failure.
 */

static int load_map_header(FILE *fp, mapstruct *m)
{
    char buf[HUGE_BUF], msgbuf[HUGE_BUF], *key, *value, *end;
    int msgpos=0;

    while (fgets(buf, HUGE_BUF-1, fp)!=NULL) {
	buf[HUGE_BUF-1] = 0;
	key = buf;
	while (isspace(*key)) key++;
	if (*key == 0) continue;    /* empty line */
	value = strchr(key, ' ');
	if (!value) {
	    end = strchr(key, '\n');
	    *end=0;
	} else {
	    *value = 0;
	    value++;
	    while (isspace(*value)) value++;
	    end = strchr(value, '\n');
	}

	/* key is the field name, value is what it should be set
	 * to.  We've already done the work to null terminate key,
	 * and strip off any leading spaces for both of these.
	 * We have not touched the newline at the end of the line -
	 * these are needed for some values.  the end pointer 
	 * points to the first of the newlines.
	 * value could be NULL!  It would be easy enough to just point
	 * this to "" to prevent cores, but that would let more errors slide
	 * through.
	 */

	if (!strcmp(key, "arch")) {
	    /* This is an oddity, but not something we care about much. */
	    if (strcmp(value,"map\n")) 
		LOG(llevError,"loading map and got a non 'arch map' line(%s %s)?\n",key,value);
	}
	else if (!strcmp(key,"name")) {
	    *end=0;
	    m->name = strdup_local(value);
	} else if (!strcmp(key,"msg")) {
	    while (fgets(buf, HUGE_BUF-1, fp)!=NULL) {
		if (!strcmp(buf,"endmsg\n")) break;
		else {
		    /* slightly more efficient than strcat */
		    strcpy(msgbuf+msgpos, buf);
		    msgpos += strlen(buf);
		}
	    }
	    /* There are lots of maps that have empty messages (eg, msg/endmsg
	     * with nothing between).  There is no reason in those cases to
             * keep the empty message.  Also, msgbuf contains garbage data
	     * when msgpos is zero, so copying it results in crashes
	     */
	    if (msgpos != 0)
		m->msg = strdup_local(msgbuf);
	} 
	/* first strcmp value on these are old names supported
	 * for compatibility reasons.  The new values (second) are
	 * what really should be used.
	 */
	else if (!strcmp(key,"hp") || !strcmp(key, "enter_x")) {
	    m->enter_x = atoi(value);
	} else if (!strcmp(key,"sp") || !strcmp(key, "enter_y")) {
	    m->enter_y = atoi(value);
	} else if (!strcmp(key,"x") || !strcmp(key, "width")) {
	    m->width = atoi(value);
	} else if (!strcmp(key,"y") || !strcmp(key, "height")) {
	    m->height = atoi(value);
	} else if (!strcmp(key,"weight") || !strcmp(key, "reset_timeout")) {
	    m->reset_timeout = atoi(value);
	} else if (!strcmp(key,"value") || !strcmp(key, "swap_time")) {
	    m->timeout = atoi(value);
	} else if (!strcmp(key,"level") || !strcmp(key, "difficulty")) {
	    m->difficulty = atoi(value);
	} else if (!strcmp(key,"invisible") || !strcmp(key, "darkness")) {
	    m->darkness = atoi(value);
	} else if (!strcmp(key,"stand_still") || !strcmp(key, "fixed_resettime")) {
	    m->fixed_resettime = atoi(value);
	} else if (!strcmp(key,"unique")) {
	    m->unique = atoi(value);
	} else if (!strcmp(key,"outdoor")) {
	    m->outdoor = atoi(value);
	} else if (!strcmp(key, "temp")) {
	    m->temp = atoi(value);
	} else if (!strcmp(key, "pressure")) {
	    m->pressure = atoi(value);
	} else if (!strcmp(key, "humid")) {
	    m->humid = atoi(value);
	} else if (!strcmp(key, "windspeed")) {
	    m->windspeed = atoi(value);
	} else if (!strcmp(key, "winddir")) {
	    m->winddir = atoi(value);
	} else if (!strcmp(key, "sky")) {
	    m->sky = atoi(value);
	}
	else if (!strncmp(key,"tile_path_", 10)) {
	    int tile=atoi(key+10);

	    if (tile<1 || tile>4) {
		LOG(llevError,"load_map_header: tile location %d out of bounds (%s)\n",
		    tile, m->path);
	    } else {
		*end = 0;
		if (m->tile_path[tile-1]) {
		    LOG(llevError,"load_map_header: tile location %d duplicated (%s)\n",
			tile, m->path);
		    free(m->tile_path[tile-1]);
		}
		/* This is sort of broken.  IF we don't normalize the path,
		 * the editor will die when it gets a multipart object that
		 * spans map.  However, if we do this, then the tile_paths
		 * get changed because we normalize them - the normalized paths
		 * may not be terrible, but not ideal either.	
		 */
#if 0
		if (!editor) {
#else
		{
#endif
		    if (check_path(value, 1)==-1) {
			int i;
			/* Need to try and normalize the path.  msgbuf is safe to use,
			 * as that is all read in at one time.
			 */
			strcpy(msgbuf, m->path);
			for (i=0; msgbuf[i]!=0; i++)
			    if (msgbuf[i] == '/') end = msgbuf + i;
			if (!end) {
			    LOG(llevError,"get_map_header: Can not normalize tile path %s %s\n",
				m->path, value);
			    value = NULL;
			} else {
			    strcpy(end+1, value);
			    if (check_path(msgbuf,1)==-1) {
				LOG(llevError,"get_map_header: Can not normalize tile path %s %s\n",
				    m->path, value);
				value = NULL;
			    } else
				value = msgbuf;
			}
		    } /* if unable to load path as given */
		} /* if not editor */
		if (value)
		    m->tile_path[tile-1] = strdup_local(value);
	    }
	}
	else if (!strcmp(key,"end")) break;
	else {
	    LOG(llevError,"Got unknown value in map header: %s %s\n", key, value);
	}
    }
    if (strcmp(key,"end")) {
	LOG(llevError,"Got premature eof on map header!\n");
	return 1;
    }
    return 0;
}

/*
 * Opens the file "filename" and reads information about the map
 * from the given file, and stores it in a newly allocated
 * mapstruct.  A pointer to this structure is returned, or NULL on failure.
 * flags correspond to those in map.h.  Main ones used are
 * MAP_PLAYER_UNIQUE, in which case we don't do any name changes, and
 * MAP_BLOCK, in which case we block on this load.  This happens in all
 *   cases, no matter if this flag is set or not.
 * MAP_STYLE: style map - don't add active objects, don't add to server
 *		managed map list.
 */

mapstruct *load_original_map(char *filename, int flags) {
    FILE *fp;
    mapstruct *m;
    int comp;
    char pathname[MAX_BUF];
    
    LOG(llevDebug, "load_original_map: %s (%x)\n", filename,flags);
    if (flags & MAP_PLAYER_UNIQUE) 
	strcpy(pathname, filename);
    else if (flags & MAP_OVERLAY)
	strcpy(pathname, create_overlay_pathname(filename));
    else
	strcpy(pathname, create_pathname(filename));

    if((fp=open_and_uncompress(pathname, 0, &comp))==NULL) {
	LOG(llevError,"Can't open %s\n", pathname);
	perror("Can't read map file");
	return (NULL);
    }

    m = get_linked_map();

    strcpy (m->path, filename);
    if (load_map_header(fp, m)) {
	LOG(llevError,"Error loading map header for %s, flags=%d\n",
	    filename, flags);
	delete_map(m);
	return NULL;
    }

    allocate_map(m);
    m->compressed = comp;

    m->in_memory=MAP_LOADING;
    load_objects (m, fp, flags & (MAP_BLOCK|MAP_STYLE));
    close_and_delete(fp, comp);
    m->in_memory=MAP_IN_MEMORY;
    if (!MAP_DIFFICULTY(m)) 
	MAP_DIFFICULTY(m)=calculate_difficulty(m);
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
    int comp;
    char buf[MAX_BUF];
    
    if (!m->tmpname) {
	LOG(llevError, "No temporary filename for map %s\n", m->path);
	strcpy(buf, m->path);
	delete_map(m);
        m = load_original_map(buf, 0);
	if(m==NULL) return NULL;
	(*fix_auto_apply_func)(m); /* Chests which open as default */
	return m;
    }

    if((fp=open_and_uncompress(m->tmpname,0, &comp))==NULL) {
	LOG(llevError,"Can't open %s\n",m->tmpname);
	perror("Can't read map file");
	strcpy(buf, m->path);
	delete_map(m);
        m = load_original_map(buf, 0);
	if(m==NULL) return NULL;
	(*fix_auto_apply_func)(m); /* Chests which open as default */
	return m;
    }
    

    if (load_map_header(fp, m)) {
	LOG(llevError,"Error loading map header for %s (%s)\n",
	    m->path, m->tmpname);
	delete_map(m);
        m = load_original_map(m->path, 0);
	return NULL;
    }
    m->compressed = comp;
    allocate_map(m);

    m->in_memory=MAP_LOADING;
    load_objects (m, fp, 0);
    close_and_delete(fp, comp);
    m->in_memory=MAP_IN_MEMORY;
    return m;
}

/*
 * Loads a map, which has been loaded earlier, from file.
 * Return the map object we load into (this can change from the passed
 * option if we can't find the original map)
 */

mapstruct *load_overlay_map(char *filename, mapstruct *m) {
    FILE *fp;
    int comp;
    char pathname[MAX_BUF];

    strcpy(pathname, create_overlay_pathname(filename));

    if((fp=open_and_uncompress(pathname, 0, &comp))==NULL) {
/*	LOG(llevDebug,"Can't open overlay %s\n", pathname);*/
	return m;
    }
    
    if (load_map_header(fp, m)) {
	LOG(llevError,"Error loading map header for overlay %s (%s)\n",
	    m->path, pathname);
	delete_map(m);
        m = load_original_map(m->path, 0);
	return NULL;
    }
    m->compressed = comp;
    /*allocate_map(m);*/

    m->in_memory=MAP_LOADING;
    load_objects (m, fp, MAP_OVERLAY);
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

    for(i=0; i<MAP_WIDTH(m); i++)
	for(j=0; j<MAP_HEIGHT(m); j++) {
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
    load_object(fp, NULL, LO_NOREAD,0);
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
    int i;
    
    if (flag && !*m->path) {
	LOG(llevError,"Tried to save map without path.\n");
	return -1;
    }
    
    if (flag || (m->unique)) {
	if (!m->unique) { /* flag is set */
	    if (flag == 2)
		strcpy(filename, create_overlay_pathname(m->path));
	    else
		strcpy (filename, create_pathname (m->path));
	} else 
	    strcpy (filename, m->path);

	/* If the compression suffix already exists on the filename, don't
	 * put it on again.  This nasty looking strcmp checks to see if the
	 * compression suffix is at the end of the filename already.
	 */
        if (m->compressed &&
	  strcmp((filename + strlen(filename)-strlen(uncomp[m->compressed][0])),
	     uncomp[m->compressed][0]))
	          strcat(filename, uncomp[m->compressed][0]);
	make_path_to_file(filename);
    } else {
	if (!m->tmpname)
	    m->tmpname = tempnam_local(settings.tmpdir,NULL);
	strcpy(filename, m->tmpname);
    }
    LOG(llevDebug,"Saving map %s\n",m->path);
    m->in_memory = MAP_SAVING;

    /* Compress if it isn't a temporary save.  Do compress if unique */
    if (m->compressed && (m->unique || flag)) {
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
    
    /* legacy */
    fprintf(fp,"arch map\n");
    if (m->name) fprintf(fp,"name %s\n", m->name);
    if (!flag) fprintf(fp,"swap_time %d\n", m->swap_time);
    if (m->reset_timeout) fprintf(fp,"reset_timeout %d\n", m->reset_timeout);
    if (m->fixed_resettime) fprintf(fp,"fixed_resettime %d\n", m->fixed_resettime);
    /* we unfortunately have no idea if this is a value the creator set
     * or a difficulty value we generated when the map was first loaded
     */
    if (m->difficulty) fprintf(fp,"difficulty %d\n", m->difficulty);
    if (m->darkness) fprintf(fp,"darkness %d\n", m->darkness);
    if (m->width) fprintf(fp,"width %d\n", m->width);
    if (m->height) fprintf(fp,"height %d\n", m->height);
    if (m->enter_x) fprintf(fp,"enter_x %d\n", m->enter_x);
    if (m->enter_y) fprintf(fp,"enter_y %d\n", m->enter_y);
    if (m->msg) fprintf(fp,"msg\n%sendmsg\n", m->msg);
    if (m->unique) fprintf(fp,"unique %d\n", m->unique);
    if (m->outdoor) fprintf(fp,"outdoor %d\n", m->outdoor);
    if (m->temp) fprintf(fp, "temp %d\n", m->temp);
    if (m->pressure) fprintf(fp, "pressure %d\n", m->pressure);
    if (m->humid) fprintf(fp, "humid %d\n", m->humid);
    if (m->windspeed) fprintf(fp, "windspeed %d\n", m->windspeed);
    if (m->winddir) fprintf(fp, "winddir %d\n", m->winddir);
    if (m->sky) fprintf(fp, "sky %d\n", m->sky);

    /* Save any tiling information, except on overlays */
    if (flag != 2)
	for (i=0; i<4; i++)
	    if (m->tile_path[i])
	        fprintf(fp,"tile_path_%d %s\n", i+1, m->tile_path[i]);

    fprintf(fp,"end\n");

    /* In the game save unique items in the different file, but
     * in the editor save them to the normal map file.
     * If unique map, save files in the proper destination (set by
     * player)
     */
    fp2 = fp; /* save unique items into fp2 */
    if ((flag == 0 || flag == 2) && !m->unique) {
	sprintf (buf,"%s.v00",create_items_path (m->path));
	if ((fp2 = fopen (buf, "w")) == NULL) {
	    LOG(llevError, "Can't open unique items file %s\n", buf);
	}
	if (flag == 2)
	    save_objects(m, fp, fp2, 2);
	else
	    save_objects (m, fp, fp2, 0);
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
	save_objects(m, fp, fp, 0);
    }

    if (m->compressed && (m->unique || flag))
	pclose(fp);
    else
	fclose(fp);

    chmod (filename, SAVE_MODE);
    return 0;
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

    for(i=0;i<MAP_WIDTH(m);i++)
	for(j=0;j<MAP_HEIGHT(m);j++) {
	    object *previous_obj=NULL;
	    while((op=GET_MAP_OB(m,i,j))!=NULL) {
		if (op==previous_obj) {
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
#ifdef MANY_CORES
    /* I see periodic cores on metalforge where a map has been swapped out, but apparantly
     * an item on that map was not saved - look for that condition and die as appropriate -
     * this leaves more of the map data intact for better debugging.
     */
    for (op=objects; op!=NULL; op=op->next) {
	if (!QUERY_FLAG(op, FLAG_REMOVED) && op->map == m) {
	    LOG(llevDebug,"free_all_objects: object %s still on map after it should have been freed", op->name);
	    abort();
	}
    }
#endif
}

/*
 * Frees everything allocated by the given mapstructure.
 * don't free tmpname - our caller is left to do that
 */

void free_map(mapstruct *m,int flag) {
    int i;

    if (!m->in_memory) {
	LOG(llevError,"Trying to free freed map.\n");
	return;
    }
    if (flag && m->spaces) free_all_objects(m);
    if (m->name) FREE_AND_CLEAR(m->name);
    if (m->spaces) FREE_AND_CLEAR(m->spaces);
    if (m->msg) FREE_AND_CLEAR(m->msg);
    if (m->buttons)
	free_objectlinkpt(m->buttons);
    m->buttons = NULL;
    for (i=0; i<4; i++) {
	if (m->tile_path[i]) FREE_AND_CLEAR(m->tile_path[i]);
	m->tile_map[i] = NULL;
    }
    m->in_memory = MAP_SWAPPED;
}

/*
 * function: vanish mapstruct
 * m       : pointer to mapstruct, if NULL no action
 * this deletes all the data on the map (freeing pointers)
 * and then removes this map from the global linked list of maps.
 */

void delete_map(mapstruct *m) {
    mapstruct *tmp, *last;
    int i;

    if (!m)
      return;
    if (m->in_memory == MAP_IN_MEMORY) {
	/* change to MAP_SAVING, even though we are not,
	 * so that remove_ob doesn't do as much work.
	 */
	m->in_memory = MAP_SAVING;
	free_map (m, 1);
    }
    /* move this out of free_map, since tmpname can still be needed if
     * the map is swapped out.
     */
    if (m->tmpname) {
	free(m->tmpname);
	m->tmpname=NULL;
    }
    last = NULL;
    /* We need to look through all the maps and see if any maps
     * are pointing at this one for tiling information.  Since
     * tiling can be assymetric, we just can not look to see which
     * maps this map tiles with and clears those.
     */
    for (tmp = first_map; tmp != NULL; tmp = tmp->next) {
	if (tmp->next == m) last = tmp;

	/* This should hopefully get unrolled on a decent compiler */
	for (i=0; i<4; i++)
	    if (tmp->tile_map[i] == m) tmp->tile_map[i]=NULL;
    }

    /* If last is null, then this should be the first map in the list */
    if (!last) {
	if (m == first_map)
	    first_map = m->next;
	else
	    /* m->path is a static char, so should hopefully still have
	     * some useful data in it.
	     */
	    LOG(llevError,"delete_map: Unable to find map %s in list\n",
		m->path);
    }
    else
	last->next = m->next;

    free (m);
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

    /* Map is good to go, so just return it */
    if (m && (m->in_memory == MAP_LOADING || m->in_memory == MAP_IN_MEMORY)) {
	return m;
    }

    /* unique maps always get loaded from their original location, and never
     * a temp location.  Likewise, if map_flush is set, or we have never loaded
     * this map, load it now.  I removed the reset checking from here -
     * it seems the probability of a player trying to enter a map that should
     * reset but hasn't yet is quite low, and removing that makes this function
     * a bit cleaner (and players probably shouldn't rely on exact timing for
     * resets in any case - if they really care, they should use the 'maps command.
     */
    if ((flags & (MAP_FLUSH|MAP_PLAYER_UNIQUE)) || !m) {

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

	if (! (flags & (MAP_FLUSH|MAP_PLAYER_UNIQUE|MAP_OVERLAY))) {
	    m=load_overlay_map(name, m);
	    if (m==NULL)
		return NULL;
	}

    } else {
	/* If in this loop, we found a temporary map, so load it up. */

	m=load_temporary_map (m);
        if(m==NULL) return NULL;
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

    decay_objects(m); /* start the decay */
    /* In case other objects press some buttons down */
    update_buttons(m);
    if (m->outdoor)
	(*set_darkness_map_func)(m);
    /* run the weather over this map */
    (*weather_effect_func)(name);
    return m;
}


/*
 * This routine is supposed to find out the difficulty of the map.
 * difficulty does not have a lot to do with character level,
 * but does have a lot to do with treasure on the map.
 *
 * Difficulty can now be set by the map creature.  If the value stored
 * in the map is zero, then use this routine.  Maps should really
 * have a difficulty set than using this function - human calculation
 * is much better than this functions guesswork.
 */

int calculate_difficulty(mapstruct *m) {
    object *op;
    archetype *at;
    int x,y;
    int diff=0;
    int i;
    sint64 exp_pr_sq, total_exp=0;

    if (MAP_DIFFICULTY(m)) {
	LOG(llevDebug, "Using stored map difficulty: %d\n", MAP_DIFFICULTY(m));
	return MAP_DIFFICULTY(m);
    }

    for(x=0;x<MAP_WIDTH(m);x++)
	for(y=0;y<MAP_HEIGHT(m);y++)
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
    (int)exp_pr_sq=((double)1000*total_exp)/(m->map_object->x*m->map_object->y+1);
    for(i=20;i>0;i--)
	if(exp_pr_sq>level_exp(i,1.0)) {
	    diff=i;
	    break;
	}
#else
    exp_pr_sq=((double)1000*total_exp)/(MAP_WIDTH(m)*MAP_HEIGHT(m)+1);
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

void free_all_maps()
{
    int real_maps=0;

    while (first_map) {
	/* I think some of the callers above before it gets here set this to be 
	 * saving, but we still want to free this data
	 */
	if (first_map->in_memory == MAP_SAVING) first_map->in_memory = MAP_IN_MEMORY;
	delete_map(first_map);
	real_maps++;
    }
    LOG(llevDebug,"free_all_maps: Freed %d maps\n", real_maps);
}

/* change_map_light() - used to change map light level (darkness)
 * up or down. Returns true if successful.   It should now be
 * possible to change a value by more than 1.
 * Move this from los.c to map.c since this is more related
 * to maps than los.
 */
 
int change_map_light(mapstruct *m, int change) {
    int new_level = m->darkness + change;
 
    /* Nothing to do */
    if(!change || (new_level <= 0 && m->darkness == 0) || 
       (new_level >= MAX_DARKNESS && m->darkness >=MAX_DARKNESS)) {
        return 0;
    }

    /* inform all players on the map */
    if (change>0) 
	(info_map_func)(NDI_BLACK, m,"It becomes darker.");
    else
	(info_map_func)(NDI_BLACK, m,"It becomes brighter.");

    /* Do extra checking.  since m->darkness is a unsigned value,
     * we need to be extra careful about negative values.
     * In general, the checks below are only needed if change
     * is not +/-1
     */
    if (new_level < 0) m->darkness = 0;
    else if (new_level >=MAX_DARKNESS) m->darkness = MAX_DARKNESS;
    else m->darkness=new_level;

    /* All clients need to get re-updated for the change */
    update_all_map_los(m);
    return 1;
}


/* 
 * This function updates various attributes about a specific space
 * on the map (what it looks like, whether it blocks magic,
 * has a living creatures, prevents people from passing
 * through, etc)
 */
void update_position (mapstruct *m, int x, int y) {
    object *tmp, *last = NULL;
    uint8 flags = 0, oldflags, light=0, anywhere=0;
    New_Face *top,*floor, *middle;
    object *top_obj, *floor_obj, *middle_obj;

    oldflags = GET_MAP_FLAGS(m,x,y);
    if (!(oldflags & P_NEED_UPDATE)) {
	LOG(llevDebug,"update_position called with P_NEED_UPDATE not set: %s (%d, %d)\n",
	    m->path, x, y);
	return;
    }

    middle=blank_face;
    top=blank_face;
    floor=blank_face;

    for (tmp = get_map_ob (m, x, y); tmp; last = tmp, tmp = tmp->above) {

	/* This could be made additive I guess (two lights better than
	 * one).  But if so, it shouldn't be a simple additive - 2
	 * light bulbs do not illuminate twice as far as once since
	 * it is a disapation factor that is squared (or is it cubed?)
	 */
	if (tmp->glow_radius > light) light = tmp->glow_radius;

	/* This call is needed in order to update objects the player
	 * is standing in that have animations (ie, grass, fire, etc).
	 * However, it also causes the look window to be re-drawn
	 * 3 times each time the player moves, because many of the
	 * functions the move_player calls eventualy call this.
	 *
	 * Always put the player down for drawing.
	 */
	if (!tmp->invisible) {
	    if ((tmp->type==PLAYER || QUERY_FLAG(tmp, FLAG_MONSTER))) {
		top = tmp->face;
		top_obj = tmp;
	    }
	    else if (QUERY_FLAG(tmp,FLAG_IS_FLOOR)) {
		/* If we got a floor, that means middle and top were below it,
		* so should not be visible, so we clear them.
		*/
		middle=blank_face;
		top=blank_face;
		floor = tmp->face;
		floor_obj = tmp;
	    }
	    /* Flag anywhere have high priority */
	    else if (QUERY_FLAG(tmp, FLAG_SEE_ANYWHERE)) {
		middle = tmp->face;

		middle_obj = tmp;
		anywhere =1;
	    }
	    /* Find the highest visible face around.  If equal
	     * visibilities, we still want the one nearer to the
	     * top
	     */
	    else if (middle == blank_face || (tmp->face->visibility > middle->visibility && !anywhere)) {
    		middle = tmp->face;
		middle_obj = tmp;
	    }
	}
	if (tmp==tmp->above) {
	    LOG(llevError, "Error in structure of map\n");
	    exit (-1);
	}
      
	if (QUERY_FLAG(tmp,FLAG_ALIVE))
	    flags |= P_IS_ALIVE;
	if (QUERY_FLAG(tmp,FLAG_NO_PASS))
	    flags |= P_NO_PASS;
	if (QUERY_FLAG(tmp,FLAG_NO_MAGIC))
	    flags |= P_NO_MAGIC;
	if (QUERY_FLAG(tmp,FLAG_DAMNED))
	    flags |= P_NO_CLERIC;
#if 0
	/* P_PASS_THRU doesn't seem to do anything so commented out for now */
	if (QUERY_FLAG(tmp,FLAG_PASS_THRU))
	    flags |= P_PASS_THRU;
#endif
	if (QUERY_FLAG(tmp,FLAG_BLOCKSVIEW))
	    flags |= P_BLOCKSVIEW;
    } /* for stack of objects */

    /* we don't want to rely on this function to have accurate flags, but
     * since we're already doing the work, we calculate them here.
     * if they don't match, logic is broken someplace.
     */
    if (((oldflags & ~(P_NEED_UPDATE|P_NO_ERROR)) != flags) &&
        (!(oldflags & P_NO_ERROR))) {
        LOG(llevDebug,"update_position: updated flags do not match old flags: %s (old=%d,new=%d) %x != %x\n",
	    m->path, x, y,
            (oldflags & ~P_NEED_UPDATE), flags);
    }
    SET_MAP_FLAGS(m, x, y, flags);

    /* At this point, we have a floor face (if there is a floor),
     * and the floor is set - we are not going to touch it at
     * this point.
     * middle contains the highest visibility face. 
     * top contains a player/monster face, if there is one.
     *
     * We now need to fill in top.face and/or middle.face.
     */

    /* If the top face also happens to be high visibility, re-do our
     * middle face.  This should not happen, as we already have the
     * else statement above so middle should not get set.  OTOH, it 
     * may be possible for the faces to match but be different objects.
     */
    if (top == middle) middle=blank_face;

    /* There are three posibilities at this point:
     * 1) top face is set, need middle to be set.
     * 2) middle is set, need to set top.
     * 3) neither middle or top is set - need to set both.
     */

    for (tmp=last; tmp; tmp=tmp->below) {
	/* Once we get to a floor, stop, since we already have a floor object */
	if (QUERY_FLAG(tmp,FLAG_IS_FLOOR)) break;

	/* If two top faces are already set, quit processing */
	if ((top != blank_face) && (middle != blank_face)) break;

	/* Only show visible faces, unless its the editor - show all */
	if (!tmp->invisible || editor) {
	    /* Fill in top if needed */
	    if (top == blank_face) {
		top = tmp->face;
		top_obj = tmp;
		if (top == middle) middle=blank_face;
	    } else {
		/* top is already set - we should only get here if
		 * middle is not set
		 *
		 * Set the middle face and break out, since there is nothing
		 * more to fill in.  We don't check visiblity here, since
		 * 
		 */
		if (tmp->face  != top ) {
		    middle = tmp->face;
		    middle_obj = tmp;
		    break;
		}
	    }
	}
    }
    if (middle == floor) middle = blank_face;
    if (top == middle) middle = blank_face;
    SET_MAP_FACE(m,x,y,top,0);
    if(top != blank_face)
        SET_MAP_FACE_OBJ(m,x,y,top_obj,0);
    else
        SET_MAP_FACE_OBJ(m,x,y,NULL,0);
    SET_MAP_FACE(m,x,y,middle,1);
    if(middle != blank_face)
        SET_MAP_FACE_OBJ(m,x,y,middle_obj,1);
    else
        SET_MAP_FACE_OBJ(m,x,y,NULL,1);
    SET_MAP_FACE(m,x,y,floor,2);
    if(floor != blank_face)
        SET_MAP_FACE_OBJ(m,x,y,floor_obj,2);
    else
        SET_MAP_FACE_OBJ(m,x,y,NULL,2);
    SET_MAP_LIGHT(m,x,y,light);
}


void set_map_reset_time(mapstruct *map) {
    if (MAP_RESET_TIMEOUT(map)>MAP_MAXRESET)
        MAP_WHEN_RESET(map) = seconds() + MAP_MAXRESET;
    else
    MAP_WHEN_RESET(map) = seconds() + MAP_RESET_TIMEOUT (map);
}

/* this updates the orig_map->tile_map[tile_num] value after loading
 * the map.  It also takes care of linking back the freshly loaded
 * maps tile_map values if it tiles back to this one.  It returns
 * the value of orig_map->tile_map[tile_num].  It really only does this
 * so that it is easier for calling functions to verify success.
 */

static mapstruct *load_and_link_tiled_map(mapstruct *orig_map, int tile_num)
{
    int dest_tile = (tile_num +2) % 4;

    orig_map->tile_map[tile_num] = ready_map_name(orig_map->tile_path[tile_num], 0);

    /* need to do a strcmp here as the orig_map->path is not a shared string */
    if (!strcmp(orig_map->tile_map[tile_num]->tile_path[dest_tile], orig_map->path))
	orig_map->tile_map[tile_num]->tile_map[dest_tile] = orig_map;

    return orig_map->tile_map[tile_num];
}

/* this returns TRUE if the coordinates (x,y) are out of
 * map m.  This function also takes into account any
 * tiling considerations, loading adjacant maps as needed.
 * This is the function should always be used when it
 * necessary to check for valid coordinates.
 * This function will recursively call itself for the 
 * tiled maps.
 *
 * 
 */
int out_of_map(mapstruct *m, int x, int y)
{

    /* If we get passed a null map, this is obviously the
     * case.  This generally shouldn't happen, but if the
     * map loads fail below, it could happen.
     */
    if (!m) return 0;

    /* Simple case - coordinates are within this local
     * map.
     */
    if ( x>=0 && x<MAP_WIDTH(m) && y>=0 && y < MAP_HEIGHT(m))
	return 0;

    if (x<0) {
	if (!m->tile_path[3]) return 1;
	if (!m->tile_map[3] || m->tile_map[3]->in_memory != MAP_IN_MEMORY) {
	    load_and_link_tiled_map(m, 3);
	}
	return (out_of_map(m->tile_map[3], x + MAP_WIDTH(m->tile_map[3]), y));
    }
    if (x>=MAP_WIDTH(m)) {
	if (!m->tile_path[1]) return 1;
	if (!m->tile_map[1] || m->tile_map[1]->in_memory != MAP_IN_MEMORY) {
	    load_and_link_tiled_map(m, 1);
	}
	return (out_of_map(m->tile_map[1], x - MAP_WIDTH(m), y));
    }
    if (y<0) {
	if (!m->tile_path[0]) return 1;
	if (!m->tile_map[0] || m->tile_map[0]->in_memory != MAP_IN_MEMORY) {
	    load_and_link_tiled_map(m, 0);
	}
	return (out_of_map(m->tile_map[0], x, y + MAP_HEIGHT(m->tile_map[0])));
    }
    if (y>=MAP_HEIGHT(m)) {
	if (!m->tile_path[2]) return 1;
	if (!m->tile_map[2] || m->tile_map[2]->in_memory != MAP_IN_MEMORY) {
	    load_and_link_tiled_map(m, 2);
	}
	return (out_of_map(m->tile_map[2], x, y - MAP_HEIGHT(m)));
    }
    return 1;
}

/* This is basically the same as out_of_map above, but
 * instead we return NULL if no map is valid (coordinates
 * out of bounds and no tiled map), otherwise it returns
 * the map as that the coordinates are really on, and 
 * updates x and y to be the localized coordinates.
 * Using this is more efficient of calling out_of_map
 * and then figuring out what the real map is
 */
mapstruct *get_map_from_coord(mapstruct *m, int *x, int *y)
{

    /* Simple case - coordinates are within this local
     * map.
     */

    if (*x>=0 && *x<MAP_WIDTH(m) && *y>=0 && *y < MAP_HEIGHT(m))
	return m;

    if (*x<0) {
	if (!m->tile_path[3]) return NULL;
	if (!m->tile_map[3] || m->tile_map[3]->in_memory != MAP_IN_MEMORY)
	    load_and_link_tiled_map(m, 3);

	*x += MAP_WIDTH(m->tile_map[3]);
	return (get_map_from_coord(m->tile_map[3], x, y));
    }
    if (*x>=MAP_WIDTH(m)) {
	if (!m->tile_path[1]) return NULL;
	if (!m->tile_map[1] || m->tile_map[1]->in_memory != MAP_IN_MEMORY)
	    load_and_link_tiled_map(m, 1);

	*x -= MAP_WIDTH(m);
	return (get_map_from_coord(m->tile_map[1], x, y));
    }
    if (*y<0) {
	if (!m->tile_path[0]) return NULL;
	if (!m->tile_map[0] || m->tile_map[0]->in_memory != MAP_IN_MEMORY)
	    load_and_link_tiled_map(m, 0);

	*y += MAP_HEIGHT(m->tile_map[0]);
	return (get_map_from_coord(m->tile_map[0], x, y));
    }
    if (*y>=MAP_HEIGHT(m)) {
	if (!m->tile_path[2]) return NULL;
	if (!m->tile_map[2] || m->tile_map[2]->in_memory != MAP_IN_MEMORY)
	    load_and_link_tiled_map(m, 2);

	*y -= MAP_HEIGHT(m);
	return (get_map_from_coord(m->tile_map[2], x, y));
    }
    return NULL;    /* Shouldn't get here */
}

/* From map.c
 * This is used by get_player to determine where the other
 * creature is.  get_rangevector takes into account map tiling,
 * so you just can not look the the map coordinates and get the
 * righte value.  distance_x/y are distance away, which
 * can be negativbe.  direction is the crossfire direction scheme
 * that the creature should head.  part is the part of the
 * monster that is closest.
 * 
 * get_rangevector looks at op1 and op2, and fills in the
 * structure for op1 to get to op2.
 * We already trust that the caller has verified that the
 * two objects are at least on adjacent maps.  If not,
 * results are not likely to be what is desired.
 * if the objects are not on maps, results are also likely to
 * be unexpected
 *
 * currently, the only flag supported (0x1) is don't translate for
 * closest body part of 'op1'
 */

void get_rangevector(object *op1, object *op2, rv_vector *retval, int flags)
{
    object	*best;

    if (op1->map->tile_map[0] == op2->map) {
	retval->distance_x = op2->x - op1->x;
	retval->distance_y = -(op1->y +(MAP_HEIGHT(op2->map)- op2->y));

    }
    else if (op1->map->tile_map[1] == op2->map) {
	retval->distance_y = op2->y - op1->y;
	retval->distance_x = (MAP_WIDTH(op1->map) - op1->x) + op2->x;
    }
    else if (op1->map->tile_map[2] == op2->map) {
	retval->distance_x = op2->x - op1->x;
	retval->distance_y = (MAP_HEIGHT(op1->map) - op1->y) +op2->y;

    }
    else if (op1->map->tile_map[3] == op2->map) {
	retval->distance_y = op2->y - op1->y;
	retval->distance_x = -(op1->x +(MAP_WIDTH(op2->map)- op2->y));
    }
    else if (op1->map == op2->map) {
	retval->distance_x = op2->x - op1->x;
	retval->distance_y = op2->y - op1->y;

    }
    best = op1;
    /* If this is multipart, find the closest part now */
    if (!(flags & 0x1) && op1->more) {
	object *tmp;
	int best_distance = retval->distance_x * retval->distance_x +
		    retval->distance_y * retval->distance_y, tmpi;

	/* we just tkae the offset of the piece to head to figure
	 * distance instead of doing all that work above again
	 * since the distance fields we set above are positive in the
	 * same axis as is used for multipart objects, the simply arithemetic
	 * below works.
	 */
	for (tmp=op1->more; tmp; tmp=tmp->more) {
	    tmpi = (op1->x - tmp->x + retval->distance_x) * (op1->x - tmp->x + retval->distance_x) +
		(op1->y - tmp->y + retval->distance_y) * (op1->y - tmp->y + retval->distance_y);
	    if (tmpi < best_distance) {
		best_distance = tmpi;
		best = tmp;
	    }
	}
	if (best != op1) {
	    retval->distance_x += op1->x - best->x;
	    retval->distance_y += op1->y - best->y;
	}
    }
    retval->part = best;
    retval->distance = isqrt(retval->distance_x*retval->distance_x + retval->distance_y*retval->distance_y);
    retval->direction = find_dir_2(-retval->distance_x, -retval->distance_y);
}

/* this is basically the same as get_rangevector above, but instead of 
 * the first parameter being an object, it instead is the map
 * and x,y coordinates - this is used for path to player - 
 * since the object is not infact moving but we are trying to traverse
 * the path, we need this.
 * flags has no meaning for this function at this time - I kept it in to
 * be more consistant with the above function and also in case they are needed
 * for something in the future.  Also, since no object is pasted, the best
 * field of the rv_vector is set to NULL.
 */

void get_rangevector_from_mapcoord(mapstruct  *m, int x, int y, object *op2, rv_vector *retval,int flags)
{
    if (m->tile_map[0] == op2->map) {
	retval->distance_x = op2->x - x;
	retval->distance_y = -(y +(MAP_HEIGHT(op2->map)- op2->y));

    }
    else if (m->tile_map[1] == op2->map) {
	retval->distance_y = op2->y - y;
	retval->distance_x = (MAP_WIDTH(m) - x) + op2->x;
    }
    else if (m->tile_map[2] == op2->map) {
	retval->distance_x = op2->x - x;
	retval->distance_y = (MAP_HEIGHT(m) - y) +op2->y;

    }
    else if (m->tile_map[3] == op2->map) {
	retval->distance_y = op2->y - y;
	retval->distance_x = -(x +(MAP_WIDTH(op2->map)- op2->y));
    }
    else if (m == op2->map) {
	retval->distance_x = op2->x - x;
	retval->distance_y = op2->y - y;

    }
    retval->part = NULL;
    retval->distance = isqrt(retval->distance_x*retval->distance_x + retval->distance_y*retval->distance_y);
    retval->direction = find_dir_2(-retval->distance_x, -retval->distance_y);
}

/* Returns true of op1 and op2 are effectively on the same map
 * (as related to map tiling).  Note that this looks for a path from
 * op1 to op2, so if the tiled maps are assymetric and op2 has a path
 * to op1, this will still return false.
 * Note we only look one map out to keep the processing simple
 * and efficient.  This could probably be a macro.
 * MSW 2001-08-05
 */
int on_same_map(object *op1, object *op2)
{
    /* If the object isn't on a map, can't be on the same map, now can it?
     * this check also prevents crashes below.
     */
    if (op1->map == NULL || op2->map == NULL) return FALSE;

    if (op1->map == op2->map || op1->map->tile_map[0] == op2->map ||
	op1->map->tile_map[1] == op2->map ||
	op1->map->tile_map[2] == op2->map ||
	op1->map->tile_map[3] == op2->map) return TRUE;
    return FALSE;
}

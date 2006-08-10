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

#ifndef WIN32 /* ---win32 exclude header */
#include <unistd.h>
#endif /* win32 */
/* 
 * Pass a char array, returns a pointer to the region of the same name.
 * if it can't find a region of the same name it returns the first region
 * with the 'fallback' property set.
 * if it can't find a matching name /or/ a fallback region it logs an info message 
 * message and returns NULL
 * used by the map parsing code. 
 */
region *get_region_by_name(const char *region_name) {
    region *reg;
    char *p = strchr(region_name, '\n');
    if (p) *p = '\0';
    for (reg=first_region;reg!=NULL;reg=reg->next)
	if (!strcmp(reg->name, region_name)) return reg;
    
    for (reg=first_region;reg!=NULL;reg=reg->next) {
	if (reg->fallback) {
	    LOG(llevDebug,"region called %s requested, but not found, fallback used.\n", region_name);
	    return reg;
	}
    }
    LOG(llevInfo,"Got no region or fallback for region %s.\n", region_name);
    return NULL;
}

/* This might need optimising at some point. */
region *get_region_by_map(mapstruct *m) {
	return get_region_by_name(get_name_of_region_for_map(m));
}

/*
 * Since we won't assume all maps have a region set properly, we need an
 * explicit check that it is, this is much nicer here than scattered throughout
 * the map code.
 */

const char *get_name_of_region_for_map(const mapstruct  *m) {
	region *reg;
	if (m->region!=NULL) return m->region->name;
	for (reg=first_region;reg!=NULL;reg=reg->next) {
		if (reg->fallback) return reg->name;
	}
  	LOG(llevInfo,"map %s had no region and I couldn't find a fallback to use.\n", m->name);
	return "unknown";
}

/*
 * Tries to find a region that 'name' corresponds to.
 * It looks, in order, for:
 * an exact match to region name (case insensitive)
 * an exact match to longname (case insensitive)
 * a substring that matches to the longname (eg Kingdom)
 * a substring that matches to the region name (eg nav)
 * if it can find none of these it returns the first parentless region 
 * (there should be only one of these - the top level one)
 * If we got a NULL, then just return the top level region
 * 
 */
region *get_region_from_string(const char *name) {
    region *reg;
    char *substr;
    char *p;
    
    if (name==NULL) {
    	for (reg=first_region;reg->parent!=NULL;reg=reg->parent);
    	return reg;
    }
    p = strchr(name, '\n');
    if (p) *p = '\0';
    for (reg=first_region;reg!=NULL;reg=reg->next)
	if (!strcasecmp(reg->name, name)) return reg;
    
    for (reg=first_region;reg!=NULL;reg=reg->next)
    	if (reg->longname != NULL) {
	    if (!strcasecmp(reg->longname, name)) return reg;
	}
    
    substr=NULL;
    for (reg=first_region;reg!=NULL;reg=reg->next)
    	if (reg->longname != NULL) {
    	    substr=strstr(reg->longname, name);
	    if (substr != NULL) return reg;
	}
    for (reg=first_region;reg!=NULL;reg=reg->next)
    	if (reg->longname != NULL) { 
	/* 
	 * This is not a bug, we want the region that is  most identifiably a discrete 
	 * area in the game, eg if we have 'scor', we want to return 'scorn' and not 
	 * 'scornarena', regardless of their order on the list so we only look at those 
	 * regions with a longname set.
	 */
    	    substr=strstr(reg->name, name);
	    if (substr != NULL) return reg;
	}
    for (reg=first_region;reg!=NULL;reg=reg->next) { 
    	    substr=strstr(reg->name, name);
	    if (substr != NULL) return reg;
    }
    /* if we are still here, we are going to have to give up, and give the top level region */
    for (reg=first_region;reg->parent!=NULL;reg=reg->parent);
    return reg;
}

/* 
 * returns 1 if the player is in the region reg, or a child region thereof
 * otherwise returns 0
 * if passed a NULL region returns -1
 */

int region_is_child_of_region(const region *child, const region *r) {
    
    if (r==NULL)
    	return -1; 
    if (child == NULL)
	return 0;
    if (!strcmp(child->name, r->name))
	return 1;
    else if(child->parent!=NULL)
    	return region_is_child_of_region(child->parent,r);	
    else return 0;
}

/* 
 * the longname of a region is not a required field, any given region
 * may want to not set it and use the parent's one instead. so, we:
 * 1. check if a longname is set and if so return it.
 * 2. check if there is a parent and try and call the function against that
 * 3. return a obviously wrong string if we can't get a longname, this should
 *    never happen. We also log a debug message.
 */
const char *get_region_longname(const region *r) {
    
    if (r->longname!=NULL) 
    	return r->longname;
    else if(r->parent!=NULL)
    	return get_region_longname(r->parent);
    else {
    	LOG(llevDebug,"NOTICE region %s has no parent and no longname.\n", r->name);
    	return "no name can be found for the current region";
    }
}

const char *get_region_msg(const region *r) {
    if (r->msg!=NULL) 
    	return r->msg;
    else if(r->parent!=NULL)
    	return get_region_msg(r->parent);
    else {
    	LOG(llevDebug,"NOTICE region %s has no parent and no msg.\n", r->name);
    	return "no description can be found for the current region";
    }
}

/** Returns an object which is an exit through which the player represented by op should be
 *  sent in order to be imprisoned. If there is no suitable place to which an exit can be 
 *  constructed, then NULL will be returned. The caller is responsible for freeing the object
 *  created by this function.
 */
object *get_jail_exit(object *op) {
    region *reg;
    object *exit;
    if (op->type != PLAYER) {
	LOG(llevError, "region.c: get_jail_exit called against non-player object.\n");
	return NULL;
    }
    reg=get_region_by_map(op->map);
    while (reg!=NULL) {
	if (reg->jailmap) {
	    exit=get_object();
	    EXIT_PATH(exit)=add_string(reg->jailmap);
	    /* damned exits reset savebed and remove teleports, so the prisoner can't escape */
	    SET_FLAG(exit, FLAG_DAMNED);
	    EXIT_X(exit) = reg->jailx;
	    EXIT_Y(exit) = reg->jaily;
	    return exit;
	}
	else reg=reg->parent;
    }
    LOG(llevDebug,"No suitable jailmap for region %s was found.\n", reg->name);
    return NULL;
}

/*
 * First initialises the archtype hash-table (init_archetable()).
 * Reads and parses the archetype file (with the first and second-pass
 * functions).
 * Then initialises treasures by calling load_treasures().
 */
void init_regions(void) {
    FILE *fp;
    char filename[MAX_BUF];
    int comp;
    
    if(first_region!=NULL) /* Only do this once */
    return;
        
    sprintf(filename,"%s/%s/%s",settings.datadir,settings.mapdir,settings.regions);
    LOG(llevDebug,"Reading regions from %s...\n",filename);
    if((fp=open_and_uncompress(filename,0,&comp))==NULL) {
	LOG(llevError," Can't open regions file %s in init_regions.\n", filename);
	return;
    }
    parse_regions(fp);
    assign_region_parents();
	LOG(llevDebug," done\n");

    close_and_delete(fp, comp);
}

/* 
 * Allocates and zeros a region struct, this isn't free()'d anywhere, so might
 * be a memory leak, but it shouldn't matter too much since it isn't called that
 * often.... 
 */

region *get_region_struct(void) {
  
    region *new;

    new=(region *)CALLOC(1,sizeof(region));
    if(new==NULL)
        fatal(OUT_OF_MEMORY);

    memset(new, '\0', sizeof(region));
  
    return new;
}

/*
 * Reads/parses the region file, and copies into a linked list
 * of region structs.
 */
void parse_regions(FILE *fp) {
    region *new;
    region *reg;
    
    char buf[HUGE_BUF], msgbuf[HUGE_BUF], *key=NULL, *value, *end;
    int msgpos=0;

    new = NULL;
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
	
	/* 
	 * This is a bizzare mutated form of the map and archetype parser 
	 * rolled into one. Key is the field name, value is what it should 
	 * be set to. 
	 * We've already done the work to null terminate key,
	 * and strip off any leading spaces for both of these.
	 * We have not touched the newline at the end of the line -
	 * these might be needed for some values. the end pointer 
	 * points to the first of the newlines.
	 * value could be NULL!  It would be easy enough to just point
	 * this to "" to prevent cores, but that would let more errors slide
	 * through.
	 */
	if (!strcmp(key,"region")) {
	    *end=0;
	    new=get_region_struct();
	    new->name = strdup_local(value);
	} 
	else if (!strcmp(key,"parent")) {
	    /* 
	     * Note that this is in the initialisation code, so we don't actually
	     * assign the pointer to the parent yet, because it might not have been
	     * parsed.
	     */
	    *end=0;
	    new->parent_name = strdup_local(value);
	}
	else if (!strcmp(key,"longname")) {
	    *end=0;
	    new->longname = strdup_local(value);
	}
	else if (!strcmp(key,"jail")) {
	    /* jail entries are of the form: /path/to/map x y */
	    char path[MAX_BUF];
	    int x,y;
	    if (sscanf(value, "%[^ ] %d %d\n", path, &x, &y) != 3) {
		LOG(llevError, "region.c: malformated regions entry: jail %s\n", value);
		continue;
	    }
	    new->jailmap = strdup_local(path);
	    new->jailx = x;
	    new->jaily = y;
	}
	else if (!strcmp(key,"msg")) {
	    while (fgets(buf, HUGE_BUF-1, fp)!=NULL) {
		if (!strcmp(buf,"endmsg\n")) break;
		else {
		    strcpy(msgbuf+msgpos, buf);
		    msgpos += strlen(buf);
		}
	    }
	    /* 
	     * There may be regions with empty messages (eg, msg/endmsg
	     * with nothing between). When maps are loaded, this is done
	     * so better do it here too...
	     */
	    if (msgpos != 0)
		new->msg = strdup_local(msgbuf);
	    
	    /* we have to reset msgpos, or the next region will store both msg blocks.*/	
	    msgpos=0;
	}
	else if (!strcmp(key,"fallback")) {
	    *end=0;
	    new->fallback = atoi(value);
	}
	else if (!strcmp(key,"end")) {
	/* Place this new region last on the list, if the list is empty put it first */
	    for (reg=first_region;reg!=NULL&&reg->next!=NULL;reg=reg->next);
	    
	    if (reg==NULL) first_region=new;
	    else reg->next=new;	
	    new = NULL;
	}
	else if (!strcmp(key,"nomore")) {
	/* we have reached the end of the region specs....*/
	break;
	}
	else {
	/* we should never get here, if we have, then something is wrong */
	    LOG(llevError, "Got unknown value in region file: %s %s\n", key, value);
	}
    } 
    if (!key || strcmp(key,"nomore")) 
	LOG(llevError, "Got premature eof on regions file!\n");
}

void assign_region_parents(void) {
    region *reg;
    uint32 parent_count=0;
    uint32 region_count=0;
    for (reg=first_region;reg!=NULL&&reg->next!=NULL;reg=reg->next) {
	if (reg->parent_name!=NULL) {
	    reg->parent=get_region_by_name(reg->parent_name);
	    parent_count++;
	}
	region_count++;
    }
    LOG(llevDebug, "Assigned %u regions with %u parents.", region_count, parent_count);
}

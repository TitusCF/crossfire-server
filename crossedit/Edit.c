/*
 *  CrossEdit - game world editor
 *  Copyright (C) 1993 Jarkko Sonninen & Petri Heinila
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  The authors can be reached via e-mail to Jarkko.Sonninen@lut.fi
 *  or Petri.Heinila@lut.fi .
 */

#include "Defines.h"
#include "Edit.h"

#include "X11.h"
#include "Ansi.h"

#include "debug.h"

#include "CrList.h"
#include "CrEdit.h"

#include "Cnv.h"
/* HACK Make sure we know how big CnvPromptMax is. */
#include "Cnv/config.h"
#include "App.h"
#include "Attr.h"
#include "MapAttr.h"
#include "Str.h"
#include "Bitmaps.h"

#include "libproto.h"
#include "proto.h"

/*
 * This function relinks all _pointers_ to the objects from
 * one map to another.
 * Note: You can _not_ free the objects in the original map
 * after this function has been called.
 * (What happened to this function? It no longer copies the pointers! -Frank)
 * moved to Edit.c from common/map.c since Edit.c is the only file that uses it.
 */

static void copy_map(mapstruct *m1, mapstruct *m2) {
    int x,y;

    memcpy(m2, m1, sizeof(mapstruct));

#if 0
    /* I'm really not sure how this is used - in any
     * case, we need to copy the objects now, and
     * I think we'd really need to make a copy of the objects,
     * but I'm not sure of the callback structure of this and
     * where this is used.  Once that is figured out,
     * this can probably be fixed to do the right thing.
     * MSW 2006-03-14
     */
    for(x=0;x<MAP_WIDTH(m1)&&x<MAP_WIDTH(m2);x++)
	for(y=0;y<MAP_HEIGHT(m1)&&y<MAP_HEIGHT(m2);y++) {
	    SET_MAP_FACE(m2,x,y,GET_MAP_FACE(m1,x,y,0),0);
	    SET_MAP_FACE(m2,x,y,GET_MAP_FACE(m1,x,y,1),1);
	    SET_MAP_FACE(m2,x,y,GET_MAP_FACE(m1,x,y,2),2);
	}
#endif
}

/*
 * member: copy by translate objects from source to a new map
 * source: -map
 * width : width of target map
 * height: height of target map
 * dx    : positive translate to right
 * dy    : positive translate to down
 */
static mapstruct *MapMoveScrollResize(mapstruct *source, 
				int width, int height, int dx, int dy) 
{
    mapstruct *target;
    object *obj,*prt; /* PaRT of obj */
    int x,y,sx = MAP_WIDTH(source), sy = MAP_HEIGHT(source);
    int linked = 0, link=0;
    int i;

    if (!width) width = sx;
    if (!height) height = sy;
    target = get_empty_map (width, height);

    if(dx < 0) dx += MAP_WIDTH(target);
    if(dy < 0) dy += MAP_HEIGHT(target);

    /*
     * Copy the map-headers from source to target.
     *
     * The name, msg and tiling paths are allocated for
     * each map (see map.c::load_map_header()). Copy
     * the pointer, then NULL the pointer in source so
     * the strings aren't freed.
     *
     * Statements below in the order the fields appear
     * in map.h.
     */

#define MOVE_AND_CLEAR(to, from) to = from; from = NULL;
    strncpy (target->path, source->path, HUGE_BUF - 1);
    target->path[HUGE_BUF - 1] = '\0';    
    MOVE_AND_CLEAR(target->tmpname, source->tmpname);    
    MOVE_AND_CLEAR(target->name, source->name);    
    target->region = source->region;
    MAP_WHEN_RESET(target) = MAP_WHEN_RESET(source);
    MAP_RESET_TIMEOUT(target) = MAP_RESET_TIMEOUT(source);
    target->fixed_resettime = source->fixed_resettime;
    target->unique = source->unique;
    MAP_NOSMOOTH(target) = MAP_NOSMOOTH(source);
    MAP_TIMEOUT(target) = MAP_TIMEOUT(source);
    MAP_SWAP_TIME(target) = MAP_SWAP_TIME(source);
    /* fields players/in_memory not copied */
    target->compressed = source->compressed;
    MAP_DIFFICULTY(target) = MAP_DIFFICULTY(source);    
    MAP_DARKNESS(target) = MAP_DARKNESS(source);
    MAP_WIDTH(target) = width;
    MAP_HEIGHT(target) = height;  
    MAP_ENTER_X(target) = (MAP_ENTER_X(source) + dx) % width;
    MAP_ENTER_Y(target) = (MAP_ENTER_Y(source) + dy) % height;
    target->outdoor = MAP_OUTDOORS(source);     
    MAP_TEMP(target) = MAP_TEMP(source);
    MAP_PRESSURE(target) = MAP_PRESSURE(source);
    MAP_HUMID(target) = MAP_HUMID(source);
    MAP_WINDSPEED(target) = MAP_WINDSPEED(source);
    MAP_WINDDIRECTION(target) = MAP_WINDDIRECTION(source);
    MAP_SKYCOND(target) = MAP_SKYCOND(source);
    /* fields wpartx/wparty not copied */
    MOVE_AND_CLEAR(target->msg, source->msg)
    /* Tiling paths. */
    for (i = 0; i < 4; i++) { 
        MOVE_AND_CLEAR(target->tile_path[i], source->tile_path[i]);
    }    
#undef MOVE_AND_CLEAR    

    for(y=0; y < sy && y < MAP_HEIGHT(target); y++)
	for(x=0; x < sx && x < MAP_WIDTH(target); x++)
	    while((obj = get_map_ob(source,x,y)) && !obj->head) {
		if ((linked = QUERY_FLAG (obj,FLAG_IS_LINKED))) {
		    link = get_button_value (obj);
		    remove_button_link (obj);
		}
		remove_ob(obj);
		for(prt = obj; prt; prt = prt->more) {
		    prt->x += dx;
		    prt->x %= MAP_WIDTH(target); /* it can be split by edge */
		    prt->y += dy;           /* designers problem to fix */
		    prt->y %= MAP_HEIGHT(target);
		}
		insert_ob_in_map(obj,target,obj,INS_NO_MERGE | INS_NO_WALK_ON);
		if (linked)
		    add_button_link(obj, target, link);
	    }
    /*free_all_objects(source);*/
    free_map (source, 1);
    delete_map (source);
    return target;
}


static object * MapGetRealObject (mapstruct * emap, int x, int y, int z)
{
    object *tmp = MapGetObjectZ (emap, x, y, z);
    return tmp ? (tmp->head ? tmp->head : tmp) : tmp;
}

static int MapInsertObjectZ(mapstruct *emap,object *o,int x, int y, int z)
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
	SET_MAP_FACE_OBJ (emap, o->x, o->y, o, 0);
    }
    if (below)
        below->above = o;
    else
        set_map_ob (emap, o->x, o->y, o);
    
    return (0);
}

static int MapObjectOut (mapstruct *target, object *obj, int x, int y) {
    object *tmp;
    for(tmp = obj; tmp; tmp = tmp->more)
        if(OUT_OF_REAL_MAP(target,x + tmp->x,y + tmp->y)) return 1;
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




/**********************************************************************
 * inner declarations
 **********************************************************************/

/*
static void CbEditSetPath (Widget w, XtPointer client, XtPointer call);
static void EditRefreshCb (Widget w, XtPointer client, XtPointer call);
static void CbEditStart (Widget w, XtPointer client, XtPointer call);

static void CbEditToggleRead (Widget w, XtPointer client, XtPointer call);
static void CbEditToggleOver (Widget w, XtPointer client, XtPointer call);
static void CbEditToggleAuto (Widget w, XtPointer client, XtPointer call);
 */
/*
static Boolean EdFreeMap ( Edit *);
static Boolean Load ( Edit *, char *name );
static int EdSelectItem ( Edit *, int x, int y );
*/
static Boolean EdSaveMap (Edit self, char *name);
/*
static void EditResizeScroll(Edit self,int width,int height,int dx,int dy);
*/
static void EditInsertArch (Edit self, int x, int y, int i, archetype * at);

object *EditCloneInsert (Edit self,object *obj,int x, int y, int z);
Boolean EditObjectDelete (Edit self, int x, int y, int z);

/*
 * to all refresh  
 */
const XRectangle EditRectAll = {
    0,0,
    10000,10000
};

/**********************************************************************
 * privates
 **********************************************************************/


/*
 *
 */
static Boolean EdFreeMap (Edit self)
{
    if (!self) return False;
    
    /*** no item from self map anymore ***/
    if (AppItemGetEdit(self->app) == self) {
	AppItemSet (self->app, NULL,NULL,0);
    }
    if(self->app->look.edit == self)
	AppSelectUnset(self->app);
    if (self->emap) {
	*self->emap->path = '\0';
	free_map (self->emap, 1);
	delete_map (self->emap);
	self->emap = NULL;
    }
    return True;
}

/*
 * member   : load map from file to memory
 *            there should no other map loading function for editor
 *            than self
 * name     : filename of map, relative ( level number )  
 * return   : True, if map is loaded
 */
static Boolean Load(Edit self, char *name)
{
    int mask;
    mapstruct *tmp;
    char path[PATH_MAX+1],save[PATH_MAX+1];
    char buf[BUFSIZ];
    
    strcpy(path, name);
    if((mask = check_path (path,1)) < 4) {
	sprintf(buf,"You can't access %s",path);
	CnvNotify(buf,"Continue",NULL);
	return False;
    }

    /* new map, no refs */
    if(self->app->look.edit == self) {
	AppSelectUnset(self->app);
	AttrChange(self->app->attr,NULL, 0, 0);
    }
    /*** remove old mapstructure ***/
    if(self->emap)
	strcpy(save,self->emap->path); /* klydge */
    EdFreeMap (self);
    if(!(tmp = ready_map_name (path, MAP_FLUSH))) {
	/* Make the map null, since right now we don't have a valid map
	 * in memory.  The CnfNotify call will force a redraw - we
	 * can check for Null map data pretty easy, but can't really check
	 * for unknown map data.
	 */
	if (self->w) /* only when already a window, open otherwhise coredump */
	    XtVaSetValues(self->w, XtNmap, NULL, NULL);
	sprintf(buf,"Cannot load map %s",path);
	CnvNotify(buf,"Continue",NULL);
	if(!(tmp = ready_map_name(save,MAP_FLUSH))) return False;
    }

    /*** initialiaze map ***/
    debug1("Edit-Load() %s\n",path);
    if(self->view) XawViewportSetLocation(self->view,0,0);
    self->emap = tmp;
    self->read_only = mask & 2 ? 0 : 1;
    strcpy (self->emap->path, path);
    EditUnmodified(self);
    EditUpdate(self);
    CrEditRefresh(self->w,EditRectAll);
    return True;
}

/**********************************************************************
 * wall handling
 **********************************************************************/

static int direction_x[] = {0, -1, 0, 1};
static int direction_y[] = {1, 0, -1, 0};

/*
 *
 */
static int find_draw_arch (mapstruct * emap, int line, archetype * at)
{
    int i;
    object *tmp;

    for (i = 0; i < 16; i++)
	if ((tmp = get_map_ob (emap, i, line)) &&
	  tmp->arch == at)
	    return (i);
    return (-1);
}

static void EditInsertArch (Edit self, int x, int y, int i, archetype * at)
{
    Window w;
    mapstruct *emap;
    object *op,*tmp;
    int j = i;
    XRectangle rect;

    emap = self->emap;
    w = XtWindow(self->w);
    op = get_map_ob (emap, x, y);

    /* check for duplicate object */
    while (op && op->above)
        op = op->above;
    while (op && j-- > 0)
        op = op->below;
    if (op && op->arch == at)
        return;

    if (ob_blocked(&at->clone, self->emap, x, y) & P_OUT_OF_MAP) {
        debug("Out of Map\n");
        return;
    }
    op = object_create_arch (at);
    if (op) {
        MapInsertObjectZ(emap,op,x,y,i);
        /*debug3 ("Inserting %s %d %d\n", op->name, op->x, op->y);*/

        for (tmp = op; tmp; tmp = tmp->more) {
            if (self->app->look.rect.x == tmp->x && 
		self->app->look.rect.y == tmp->y)
                AppUpdate(self->app);
        }
	rect.x = x;
	rect.y = y;
	rect.width = rect.height = 1;
	CrEditRefresh(self->w,rect);
    } else {
        free_object (op);
        debug0 ("Inserting failed\n");
    }

    return;
}

/*
 * member: hmm
 */
static int update_wall (Edit self, int x, int y, int add, int rem)
{
    int i;
    object *tmp, *op;
    Window w;
    mapstruct * emap;
    
    emap = self->emap;
    w = XtWindow(self->w);

    if (out_of_map (emap, x, y) || !(op = get_map_ob (emap, x, y)))
	return False;

    while (op->above)
	op = op->above;

    /*
    i = find_draw_arch (self->app->item.wall_map, 
			self->app->item.wall_set, 
			op->arch);
			*/
    i = find_draw_arch (AppItemGetMap(self->app), 
			AppItemGetWall(self->app), 
			op->arch);
    if (i >= 0) {
	/* debug4 ("%d | %d & ~%d = %d\n", i, add, rem, (i | add) & ~rem); */
	remove_ob (op);
	free_object (op);
	tmp = get_map_ob (AppItemGetMap(self->app), ((i | add) & ~rem),
	  AppItemGetWall(self->app));
	if (tmp)
	    EditInsertArch (self, x, y, 0, tmp->arch);
	return True;
    }
    return False;
}

/*
 *
 */
static void draw_add (Edit self, int x, int y)
{
    int i, mask = 0;

    if (!self->app->item.wall_map)
	CnvDie(self->shell,"No Wall map");

    if (MAP_WIDTH(self->app->item.wall_map) < 16)
	CnvDie(self->shell,"Wall Map has wrong width\n");

    for (i = 0; i < 4; i++)
	if (update_wall (self, x + direction_x[i], y + direction_y[i], 
			 (1 << i), 0))
	    mask |= (1 << (i ^ 2));

    update_wall (self, x, y, mask, 0);
}

/*
 *
 */
static void draw_remove (Edit self, int x, int y)
{
    if (!self->app->item.wall_map)
	CnvDie(self->shell,"No Wall map");

    if (MAP_WIDTH(self->app->item.wall_map) < 16)
	CnvDie(self->shell,"Wall Map has wrong width\n");

    update_wall (self, x, y + 1, 0, 1);
    update_wall (self, x - 1, y, 0, 2);
    update_wall (self, x, y - 1, 0, 4);
    update_wall (self, x + 1, y, 0, 8);
}


/*
 * member: resize and scroll mao of editor
 * width : new width, if zero, no resize
 * height: new height, if zero, no resize
 * dx    : amount to scroll, neg. to left, pos. to right
 * dy    : amount to scroll, neg, to up, pos. to down
 */
void EditResizeScroll(Edit self,int width,int height,int dx,int dy)
{
    mapstruct *tmp;
    char buf[BUFSIZ];

    if(!self || self->read_only) return;
    if(self->type != ClipBoard &&  
       (width < MapMinWidth || width > MapMaxWidth ||
	height < MapMinHeight || height > MapMaxHeight)) {
	sprintf(buf,"Map cannot be smaller than %dx%d",
		MapMinWidth,MapMinHeight);
	CnvNotify(buf,"Continue",NULL);
	return;
    }


    /*
     * SelectUnset before EdFreeMap, since, it tries to 
     *  do that, and there's no mapstuct at that moment 
     */
    if(self->app->look.edit == self)
	AppSelectUnset(self->app);
    
    tmp = MapMoveScrollResize(self->emap, width, height, dx, dy);
    self->emap = NULL; /* MapMoveScrollResize destroys the old map */
    EdFreeMap(self);
    self->emap = tmp;
    EditModified(self);
    EditUpdate(self);

    CrEditRefresh(self->w,EditRectAll);

    debug5("EditResizeScroll() %s %dx%d+%d+%d\n",
	   EditGetPath(self),width,height,dx,dy);
}

/**********************************************************************
 * File-menu callbacks
 **********************************************************************/

/*
 * callback: save current map, ask name if not given
 */
static void SaveCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    EditSave(self);
}

/*
 * callback: save map as name
 */
static void SaveAsCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    char path[PATH_MAX];

    if(CnvPathSelect(self->app->path) == CnvPathCancel) return;
    sprintf(path,"%s/%s",
	    self->app->path->current,
	    self->app->path->filename);
    EdSaveMap(self,path);
}

/*
 * callback: load current map again
 */
static void ClearCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    Map tmp;

    debug1("ClearCb() %s\n",self->emap->path);
    if(self->read_only) return;
    tmp = get_empty_map(MAP_WIDTH(self->emap), MAP_HEIGHT(self->emap));
    copy_map (self->emap, tmp); 
    EdFreeMap(self);
    self->emap = tmp;
    EditUnmodified(self);
    EditUpdate(self);
}

/*
 * callback: load new map
 */
static void LoadCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    char path[PATH_MAX+1],buf[BUFSIZ];

    /*** save, if modified ***/
    if (self->modified) {
	switch (CnvNotify ("Map modified, save",
			   "OK","Cancel","No",NULL)) {
	  case 1:
	    EditSave(self);
	    break;
	  case 2:
	    return; 
	  default:
	    break;
	}
    }
    /*** load map ***/
    if(CnvPathSelect(self->app->path) == CnvPathCancel) return;
    sprintf(path,"%s/%s",self->app->path->current,self->app->path->filename);
    /*** check out if map can load ***/
    if (has_been_loaded (path)) {
      sprintf(buf,"%s already in memory",path);
      CnvNotify(buf,"Continue",NULL);
      return;
    }
    Load(self,path);
}

/*
 * callback: load current map again
 */
static void ReloadCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    debug1("ReloadCb() %s\n",self->emap->path);
    Load(self,self->emap->path);
}

/* 
 * callback: enter to map in Look window 
 */
static void EnterCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    object *obj = NULL, *tmp;
    char buf[BUFSIZ];
    char loadPath[PATH_MAX+1];
    const char *path;
    XRectangle rect;
    
    /*** find object ***/
    if(self->app->look.edit)
	obj = MapGetObjectZ
	    (self->app->look.edit->emap,
	     self->app->look.rect.x,
	     self->app->look.rect.y,0);
    if (!obj || !self->app->look.edit) {
	CnvNotify("Select exit to enter","Continue",NULL);
	return;
    }
    /*** foreach object in list, if exit load it ***/
    tmp = obj;
    do {
	obj = tmp->head ? tmp->head : tmp;
	if (obj->type == PLAYER_CHANGER || obj->type == TELEPORTER || obj->type==EXIT) {
	    /*** check out if exit leads ***/
	    if(!EXIT_PATH(obj)) {
		CnvNotify("Exit leads nowhere",
			  "OK",NULL);
		return;
	    }
	    path = EXIT_PATH(obj);
	    rect.x = EXIT_X(obj);
	    rect.y = EXIT_Y(obj);
	    /*** save current map if modified ***/
	    if (self->modified) {
		switch (CnvNotify ("Map modified, save",
				   "Save","Cancel","Forget",NULL)) {
		  case 1:
		    EditSave(self);
		    break;
		  case 2:
		    return;
		  default:
		    break;
		}
	    }
	    /*** load & update ***/
	    StrPathCd(strcpy(loadPath,EditGetPath(self)),path);
	    debug1("EnterCb() %s\n",loadPath);

	    /*** check out if map can load ***/
	    if (has_been_loaded (loadPath)) {
		sprintf(buf,"%s already in memory",loadPath);
		CnvNotify(buf,"Continue",NULL);
		return;
	    }
	    if(!Load(self,loadPath)) return;
	    if (rect.x == 0 && rect.y == 0) {
		rect.x = MAP_ENTER_X(self->emap);
		rect.y = MAP_ENTER_Y(self->emap);
	    }
	    rect.width = rect.height = 0;
	    AppSelectSet(self->app, self, rect);
	    CrEditSelect(self->w, rect);
	    return;
	}
	tmp = tmp->below;
    } while(tmp);
    CnvNotify("That's not exit","Continue",NULL);
}

/*
 * callback: reset this map from crossfire 
 */
static void ResetCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    char cmd[ARG_MAX+1],buf[MAX_INPUT+1];

    debug0("ResetCb()\n");
    if(!self->app->res.cmdReset) {
	CnvNotify("no defined *cmdReset:","Continue",NULL);
	return;
    }
    sprintf(cmd,self->app->res.cmdReset,EditGetPath(self));
    if(system(cmd)) {
	sprintf(buf,"Command failed \"%s\"",cmd);
	CnvNotify(buf,"Continue",NULL);
    }
}

/*
 * callback: quit map editor
 */
static void CloseCb (Widget w, XtPointer client, XtPointer call)
{
    char buf[BUFSIZ];
    Edit self = (Edit) client;
    if (EditIsModified(self)) {
	sprintf (buf, "Quitting, save %s ?", self->emap->path);
	switch (CnvNotify (buf,"Save","Cancel", "Forget",NULL)) {
	  case 1:		/* ok */
	    EditSave(self);
	    break;
	  case 2:
	    return;
	  default:
	    EditUnmodified(self); /* to fool EditDestroy */
	    break;
	}
    }
    EditDestroy(self);
}

static CnvMenuRec fileMenu[] = {
    {"save",   SaveCb},
    {"saveAs", SaveAsCb},
    {"----",   NULL},
    {"load",   LoadCb},
    {"reload", ReloadCb},
    {"----",   NULL},
    {"clear",  ClearCb},
    {"reset",  ResetCb},
    {"----",   NULL},
    {"enter",  EnterCb},
    {"----",   NULL},
    {"close",  CloseCb},
    {"",       NULL },
};

/**********************************************************************
 * Auto Creating stuff
 **********************************************************************/

static char * MapMessageCreate (App self) {
    char buf[MAX_BUF];
    time_t t = time(NULL);
    sprintf (buf, "Creator: %s\nEmail: %s\nDate: %s",
	     self->res.creator,self->res.email, ctime (&t));
    return strdup_local (buf);
}

#if 0
static char * MapNameCreate (String path)
{
    char name[PATH_MAX+1];
    int i,j;

    strncpy(name,path,PATH_MAX);
    name[PATH_MAX] = '\0';
    for(i = strlen(name)-1; i && name[i] != '/';i--);
    i++;
    for(j = i; name[j] && name[j] != '.';j++);
    if(name[j] == '.') name[j] = 0;
    return strdup_local (&name[i]);
}
#endif
/**********************************************************************
 * Options - menu
 **********************************************************************/


#if 0
/*
 * callback: set new path of map
 */
static void SetPathCb (Widget w, XtPointer client, XtPointer call)
{
    char path[CnvPromptMax+1];
    Edit self = (Edit) client;
    
    switch (CnvPrompt ("Set path", self->emap->path, path, 
		       "OK","Cancel",NULL)) {
    case 1:
	EditSetPath(self,path);
    default:
	break;
    }
}

/*
 * callback: resize and scroll map
 */
static void ResizeCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    char buf[1025], path[CnvPromptMax+1];
    unsigned x, y;
    int sx = 0, sy = 0, res;

    if (self->read_only)
	return;

    sprintf (buf, "%dx%d+0+0", 
	     MAP_WIDTH(self->emap), MAP_HEIGHT(self->emap));
    switch (CnvPrompt ("ResizeScroll",
		       buf, path,"OK",NULL)) {
    case 1:
	res = XParseGeometry (path, &sx, &sy, &x, &y);
	
	if (!(res & WidthValue))
	    x = MAP_WIDTH(self->emap);
	if (!(res & HeightValue))
	    y = MAP_HEIGHT(self->emap);
	
	if (!(res & XValue))
	    sx = 0;
	if (!(res & YValue))
	    sy = 0;

	if (x < 3 || y < 3 || x > 100 || y > 100) {
	    CnvNotify ("Illegal size","OK",NULL);
	    return;
	}
	EditResizeScroll (self, x,y,sx, sy);
	break;
    default:
	return;
    }
}

/*
 * callback: set start point
 */
static void StartCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit) client;
    char buf[BUFSIZ], reply[CnvPromptMax+1];
    int x, y, flags;
    int width, height;

    sprintf (buf, "%dx%d", 
	     MAP_ENTER_X(self->emap),
	     MAP_ENTER_Y(self->emap));
    switch (CnvPrompt ("Start of map", buf,reply,
	"OK","Cancel",NULL)) {
    case 1:			/* ok */
	flags = XParseGeometry (reply, &x, &y, &width, &height);
	if ((flags != (WidthValue | HeightValue))) {
	    CnvNotify ( "Illegal value","Ok",NULL);
	    return;
	}
	if (out_of_map(self->emap,width,height)) {
	    sprintf (buf, "Point  %dx%d out of map", width, height);
	    CnvNotify (buf,"OK",NULL);
	    return;
	} else {
	    MAP_ENTER_X(self->emap) = width;
	    MAP_ENTER_Y(self->emap) = height;
	    EditModified(self);
	}
	EditUpdate(self);
	return;
    default:
	return;
    }
}
#endif



/*
 * Edit attributes of the Map
 */
static void AttributesCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit)client;
	
    debug0("Edit::AttributesCb()\n");

    if (self->mapattr == NULL) {
        self->mapattr = MapAttrCreate(self->emap, self->app, self);
    } else {
	MapAttrDestroy(self->mapattr);
    }
}

/* 
 * callback: refresh map
 */
static void RefreshCb (Widget w, XtPointer client, XtPointer call)
{  
  Edit self = (Edit)client;
  CrEditRefresh(self->w,EditRectAll);
  EditUpdate(self);
}

#if 0
/* 
 * callback: toggle write accces 
 */
static void ToggleReadCb (Widget w, XtPointer client, XtPointer call)
{
  Edit self = (Edit)client;

  if(self->type == Pick ||
     self->type == Wall) return;
  ((Edit) client)->read_only++;
  EditUpdate(self);
  debug ("CbEditToggleRead()\n");
}
#endif

/* 
 * callback: Toggle overwrite mode 
 */
static void ToggleOverCb (Widget w, XtPointer client, XtPointer call)
{
  Edit self = (Edit)client;

  if(self->type == Pick ||
     self->type == Wall) return;
  ((Edit) client)->overwrite++;
  EditUpdate(self);
  debug ("CbEditToggleOver()\n");
}

/* 
 * callback: Toggle auto choose 
 */
static void ToggleAutoCb (Widget w, XtPointer client, XtPointer call)
{
  Edit self = (Edit)client;

  if(self->type == Pick ||
     self->type == Wall) return;
  ((Edit) client)->auto_choose++;
  EditUpdate(self);
  debug ("CbEditToggleAuto()\n");
}

/* 
 * callback: Toggle showing weak walls
 */
static void ToggleWeakCb (Widget w, XtPointer client, XtPointer call)
{
    Edit self = (Edit)client;
    Cardinal weak_walls;
  
    XtVaGetValues(self->w, XtNshow_weak_walls, &weak_walls, NULL);
    XtVaSetValues(self->w, XtNshow_weak_walls, !weak_walls, NULL);

    EditUpdate(self);
    debug("CbEditToggleWeak()\n");
}

/*
 * callback: Adjust stacking size
 */
static void SetStackingCb(Widget w, XtPointer client, XtPointer call)
{
    char buf[MAX_BUF];
    char retbuf[CnvPromptMax+1];
    Edit self = (Edit)client;
    Cardinal stacking;
    int ret;
    
    XtVaGetValues(self->w, XtNstacking, &stacking, NULL);
    
    snprintf(buf, sizeof(buf), "%d", stacking);
    
    ret = CnvPrompt("Set stacking (0 = off)", buf, retbuf, "OK", "Cancel", NULL);
        
    if (ret != 1)
        return;
    
    stacking = atoi(retbuf);
    
    debug2("SetStackingCb %s %d\n", retbuf, stacking);

    /* TODO How did they come up with this magic number? */
    if (stacking > 48) {
        CnvNotify("Illegal space", "Ok", NULL);
        return;
    }
    XtVaSetValues(self->w, XtNstacking, stacking, NULL);

    EditUpdate(self);
} 

static Widget OptionMenu(String name,Edit self,Widget parent)
{
    Widget shell,refresh, use;

    shell = XtVaCreatePopupShell 
      (name, simpleMenuWidgetClass, parent, NULL);

    use = XtVaCreateManagedWidget
	("attributes",smeBSBObjectClass, shell,
	 NULL);
    XtAddCallback(use,XtNcallback,AttributesCb,(XtPointer)self);

    /*** refresh ***/
    XtVaCreateManagedWidget ("line", smeLineObjectClass, shell, NULL);
    refresh = XtVaCreateManagedWidget
	("refresh",smeBSBObjectClass,shell,
	 NULL);
    XtAddCallback
	(refresh,XtNcallback,RefreshCb,(XtPointer)self);
    /*** toggles ***/
    XtVaCreateManagedWidget ("line", smeLineObjectClass, shell, NULL);
    self->iw.overwrite = XtVaCreateManagedWidget 
	("overWrite", smeBSBObjectClass, shell, NULL);
    XtAddCallback (self->iw.overwrite,
		   XtNcallback,ToggleOverCb, (XtPointer) self);
    self->iw.auto_choose = XtVaCreateManagedWidget 
	("autoChoose", smeBSBObjectClass, shell, NULL);
    XtAddCallback (self->iw.auto_choose,
		   XtNcallback,ToggleAutoCb, (XtPointer) self);
    self->iw.weak_walls = XtVaCreateManagedWidget 
	("weakWalls", smeBSBObjectClass, shell, NULL);
    XtAddCallback (self->iw.weak_walls,
		   XtNcallback,ToggleWeakCb, (XtPointer) self);
    self->iw.stacking = XtVaCreateManagedWidget 
	("sparse", smeBSBObjectClass, shell, NULL);
    XtAddCallback (self->iw.stacking,
		   XtNcallback,SetStackingCb, (XtPointer) self);

    return shell;
}

/**********************************************************************
 * edit routines - CrEdit callbacks
 *   insert
 *   select
 *   delete
 *   align
 *   feed
 **********************************************************************/

static void InsertCb(Widget w,XtPointer clientData,XtPointer callData)
{
    Edit self = (Edit)clientData;
    CrEditCall call = (CrEditCall)callData;
    AppSelectUnset(self->app);
    EditInsert(self,call->rect.x,call->rect.y,call->z);
}

static void SelectCb(Widget w,XtPointer clientData,XtPointer callData)
{
    Edit self = (Edit)clientData;
    CrEditCall call = (CrEditCall)callData;
    if(self->auto_choose) {
	if(self->type == Wall && call->rect.x == 0) {
	    AppItemSet(self->app,self,NULL,call->rect.y);
	} else {
	    object *obj;
	    obj = MapGetRealObject(self->emap,call->rect.x,call->rect.y,0);
	    AppItemSet(self->app,self,obj,AppItemNoWall);
	}
    } else {
	AppSelectSet(self->app,self,call->rect);
    }
}

static void PropsCb(Widget w,XtPointer clientData,XtPointer callData)
{
    Edit self = (Edit)clientData;
    CrEditCall call = (CrEditCall)callData;
    debug0 ("PropsCb()\n");
    if(!self->auto_choose) {
	object *ob = MapGetRealObject
	    (self->emap, call->rect.x, call->rect.y, call->z);
	if(!self->app->attr) {
	    self->app->attr = AttrCreate 
		("attr", self->app, ob, AttrDescription, GetType(ob), self);
	} else {
	    AttrChange (self->app->attr,ob, GetType(ob), self);
	}
    }
    return;
}

static void DeleteCb(Widget w,XtPointer clientData,XtPointer callData)
{
    Edit self = (Edit)clientData;
    CrEditCall call = (CrEditCall)callData;
    AppSelectUnset(self->app);
    EditDelete(self,call->rect.x,call->rect.y,call->z);
}

static void AlignCb(Widget w,XtPointer clientData,XtPointer callData)
{
    Edit self = (Edit)clientData;
    CrEditCall call = (CrEditCall)callData;
    debug4("Edit::AlignCb() by %dx%d+%d+%d\n",call->rect.width,
	   call->rect.height,call->rect.x,call->rect.y);
    EditResizeScroll(self,call->rect.width,
		     call->rect.height,call->rect.x,call->rect.y);
}

static void FeedCb(Widget w,XtPointer clientData,XtPointer callData)
{
    Edit self = (Edit)clientData;
    CrEditCall call = (CrEditCall)callData;
    char buf[BUFSIZ],*str;
    
    debug("FeedCb()\n");
    /*** path, modified for game, not clean thing  ***/
    str = buf;
    if (!self->app->attr || !self->app->attr->op)
	return;
    if(!strcmp(self->app->attr->op->map->path,call->map->path)) {
	StrBasename(buf,self->app->attr->op->map->path);
    } else {
	strcpy(buf,self->app->attr->op->map->path);
	StrPathGenCd(buf,call->map->path);
	if(strlen(buf) > 2) str = &buf[2];
    }
    XtVaSetValues (self->app->attr->tags[I_Path].value, 
		   XtNstring, str, 
		   NULL);
    /*** x ***/
    sprintf (buf, "%d", call->rect.x);
    XtVaSetValues (self->app->attr->tags[I_X].value,
		   XtNstring, buf, 
		   NULL);
    /*** y ***/
    sprintf (buf, "%d", call->rect.y);
    XtVaSetValues (self->app->attr->tags[I_Y].value,
		   XtNstring, buf, 
		   NULL);
}

/**********************************************************************
 * layout
 **********************************************************************/

static void Layout(Edit self,Widget parent,Cardinal stacking)
{
    Widget vbox, hbox,use;
    Widget editMenu, optionMenu;

    /*** shell ***/
    self->shell = XtVaCreatePopupShell 
      ("edit",topLevelShellWidgetClass, self->app->shell,
       XtNwidth,
       (MAP_WIDTH(self->emap) > self->app->res.mapWidth ?
	self->app->res.mapWidth * FontSize :
	MAP_WIDTH(self->emap) * FontSize ) + 16,/* kludge */
       XtNheight,
       (MAP_HEIGHT(self->emap) > self->app->res.mapHeight ?
	self->app->res.mapHeight * FontSize :
	MAP_HEIGHT(self->emap) * FontSize ) + 46,/* kludge */
       XtNiconPixmap,bitmaps.edit,
       NULL);
    vbox = XtVaCreateManagedWidget ("vbox", panedWidgetClass,
      self->shell, NULL);

    /*** menu bar ***/
    hbox = XtVaCreateManagedWidget ("hbox", boxWidgetClass,
      vbox, NULL);
    
    if(self->type != ClipBoard) {
	use = XtVaCreateManagedWidget 
	    ("mapFileButton", menuButtonWidgetClass, hbox,
	     XtNmenuName,"fileMenu",
	     NULL);
	CnvMenu("fileMenu",use,fileMenu,(XtPointer)self);

	editMenu = XtVaCreateManagedWidget 
	    ("mapEditButton", menuButtonWidgetClass, hbox,
	     XtNmenuName,"mapEdit",
	     NULL);

	optionMenu = XtVaCreateManagedWidget 
	    ("mapOptionButton", menuButtonWidgetClass, hbox,
	     XtNmenuName,"optionMenu",
	     NULL);
	OptionMenu("optionMenu",self,optionMenu);
    }


    /*** editor-widget ***/
    self->view = XtVaCreateManagedWidget 
	("veivi",viewportWidgetClass, vbox, 
	 XtNallowResize,True,
	 XtNforceBars,True,
	 XtNallowHoriz,True,
	 XtNallowVert,True,
	 NULL);
    self->w = XtVaCreateManagedWidget 
	("cross",crEditWidgetClass,self->view,
	 XtNresizable,True,
	 XtNmap,self->emap,
	 XtNstacking,stacking,
	 XtNselectArea,True,
	 NULL);
    XtAddCallback(self->w,XtNinsertCallback,InsertCb,(XtPointer)self);
    XtAddCallback(self->w,XtNselectCallback,SelectCb,(XtPointer)self);
    XtAddCallback(self->w,XtNpropsCallback,PropsCb,(XtPointer)self);
    XtAddCallback(self->w,XtNdeleteCallback,DeleteCb,(XtPointer)self);
    XtAddCallback(self->w,XtNalignCallback,AlignCb,(XtPointer)self);
    XtAddCallback(self->w,XtNfeedCallback,FeedCb,(XtPointer)self);
}

/**********************************************************************
 * public
 **********************************************************************/

/*
 * create editing window for map for crossfire-abs-path file-name
 */
Edit EditCreate(App app,EditType type,String path)
{
    Edit self;

    /*** initialising ***/
    self = (Edit)XtMalloc(sizeof(struct _Edit));
    self->app = app;
    self->shell = NULL;
    self->w = NULL;
    self->view = NULL;
    self->next = NULL;
    self->emap = NULL;
    self->type = type;
    self->overwrite = 0;
    self->mapattr = NULL;
    self->modified = False;

    /*** load or create map ***/
    if (type == ClipBoard) {
	if (!self->emap)
	    self->emap = get_empty_map(MapMinWidth,MapMinHeight);
	strcpy (self->emap->path, "/ClipBoard");

	if (!self->emap->msg) {
	    self->emap->msg = MapMessageCreate (app);
	}
    } else if(path && *path != '\0') {
	if (!Load(self,path)) return 0;
    } else {
	self->emap = get_empty_map(16,16);
	strcpy (self->emap->path, "/Noname");

	if (!self->emap->msg) {
	    self->emap->msg = MapMessageCreate (app);
	}
    }
    
    if (!self->emap)
	return 0;
    
    /*** creating ***/
    Layout(self,self->app->shell,0);
    EditUnmodified(self);

    /*** type specific ***/
    switch (self->type) {
    case Regular:
	self->read_only = 0;
	self->auto_choose = 0;
	break;
    case Pick:
    case Wall:
	self->read_only = 1;
	self->auto_choose = 1;
	XtVaSetValues(self->w,
		      XtNstacking,1,
		      XtNselectArea,False,
		      NULL);
	break;
    case ClipBoard:
	self->read_only = 0;
	self->auto_choose = 0;
	break;
    default:
        fprintf(stderr,"unknown MapType");
	exit(EXIT_FAILURE);
    }
    EditUpdate(self);
    /*** show-map-editor ***/
    if(type != ClipBoard)
	XtPopup (self->shell, XtGrabNone);

    return self;
}

/*
 * member: destroy edit
 */
void EditDestroy(Edit self)
{
    char buf[BUFSIZ];
    
    if(!self) return;
    debug1 ("EditDestroy() %s\n",EditGetPath(self));
    
    /*** reply to save if designed ***/
    if (EditIsModified(self)) {
	sprintf (buf, "Quitting, save %s ?", self->emap->path);
	switch (CnvNotify (buf,"Save","Forget",NULL)) {
	case 1:		/* ok */
	    EditSave(self);
	    break;
	default:
	    break;
	}
    }
    /*** outer coonections ***/
    if (self->mapattr) {
	MapAttrDestroy(self->mapattr);
    }
    if (self->app->attr && self->app->attr->op &&
	self->app->attr->op->map == self->emap) {
	AttrChange(self->app->attr, NULL, 0, (Edit)0);
    }
    if (AppItemGetEdit(self->app) == self) {
	AppItemSet (self->app, NULL,NULL,0);
    }
    AppUpdate(self->app);
    AppEditDeattach(self->app,self);
    EdFreeMap (self);

    /*** inner remove ***/
    XtDestroyWidget (self->shell);
    XtFree((char*)self);
}

/*
 * member: update Edit values
 */
void EditUpdate(Edit self)
{
    char buf[BUFSIZ];

    if(!(self && self->shell && self->emap)) return;

    /*** map title ***/
    snprintf(buf, sizeof(buf),
	    "Edit %s %s",
	    self->emap->path,
	    self->modified ? "*" : "");
    XtVaSetValues(self->shell,
		  XtNtitle,buf,
		  XtNiconName,buf,
		  NULL);
    
    /*** info ***/
    if (self->mapattr != NULL)
	MapAttrChange(self->mapattr, self->emap);

    /*** toggles ***/
    if(self->type != ClipBoard) {
        Cardinal weak_walls;
        Cardinal stacking;
    
#if 0
	XtVaSetValues 
	    (self->iw.read_only,
	     XtNleftBitmap, self->read_only ? bitmaps.mark : None, NULL);
#endif
	XtVaSetValues 
	    (self->iw.overwrite, 
	     XtNleftBitmap, self->overwrite ? bitmaps.mark : None, NULL);
	XtVaSetValues 
	    (self->iw.auto_choose, 
	     XtNleftBitmap, self->auto_choose ? bitmaps.mark : None, 
	     NULL);
        
	XtVaGetValues(self->w, XtNshow_weak_walls, &weak_walls, NULL);
	XtVaSetValues 
	    (self->iw.weak_walls, 
	     XtNleftBitmap, weak_walls ? bitmaps.mark : None, 
	     NULL);

	XtVaGetValues(self->w, XtNstacking, &stacking, NULL);
	XtVaSetValues 
	    (self->iw.stacking, 
	     XtNleftBitmap, (stacking != 0) ? bitmaps.mark : None, 
	     NULL);
    }
    /*** map area ***/
    if(self->emap && self->w)
	XtVaSetValues(self->w,XtNmap,self->emap,NULL);

    if (self->app->look.edit == self)
	CrEditRefresh (self->w, self->app->look.rect);
}

/*
 * member : save map from memory to file, there should be no other
 *            map saving routine than self
 * self   : pointer to current Edit structure
 * name   : filename of map, relative ( level number )  ???
 * return : True, map is saved
 */
static Boolean EdSaveMap (Edit self, char *name)
{
    mapstruct *m = self->emap;
    char buf[BUFSIZ];

    debug0 ("Saving Map by name\n");

    strcpy (m->path, name);

    /*** updating map name here ***/
    EditSetPath(self,name);
    if(save_map (m, 1)) {
	sprintf(buf,"Error in saving map %s",EditGetPath(self));
	CnvNotify ("Error in saving map !","OK",NULL);
	return False;
    }
    EditUnmodified(self);
    return True;
}

/*
 * member: save contents of map to file
 * return: status
 */
EditReturn EditSave(Edit self)
{
    char path[PATH_MAX+1];  

    if (self->read_only) {
	CnvNotify ( "Map is read-only!","OK",NULL);
	return EditOk;
    }
    if (!self->modified) {
	CnvNotify ( "No changes to save", "OK",NULL);
	return EditOk;
    }
    /*** if empty path, ask it ***/
    if (!*self->emap->path) {
	if(CnvPathSelect(self->app->path) == CnvPathCancel) return EditOk;
	sprintf(path,"%s/%s",self->app->path->current,
		self->app->path->filename);
	strcpy (self->emap->path, path);
    }
    if(!EdSaveMap(self,self->emap->path)) return EditOk;
    EditUpdate(self);
    return EditOk;
}

/*
 * member: load map from file
 * self   : pointer to current Edit structure
 * return : status
 */
EditReturn EditLoad(Edit self)
{
  if(Load(self, self->emap->path))
    return EditOk;
  else
    return EditError;
}

/*
 * member: make fill
 * x,y   : point to start fill
 */
void EditPerformFill(Edit self, int x, int y)
{
    int dx, dy;
    if (!self->emap) return;

    for (dx=x; dx< x+self->app->look.rect.width; dx++) {
	for (dy=y; dy < y+self->app->look.rect.height; dy++) {
	    if (!OUT_OF_REAL_MAP (self->emap, dx, dy))
		EditInsert(self, dx, dy, 0);
	}
    }
}

/*
 * member: make fill
 * x,y   : point to start fill
 */
void EditPerformFillBelow(Edit self, int x, int y)
{
    int dx, dy;
    if (!self->emap) return;

    for (dx=x; dx< x+self->app->look.rect.width; dx++) {
	for (dy=y; dy < y+self->app->look.rect.height; dy++) {
	    if (!OUT_OF_REAL_MAP (self->emap, dx, dy))
		EditInsert(self, dx, dy, -1);
	}
    }
}

/*
 * member: fill
 * x,y   : point to start fill
 */
void EditFillRectangle(Edit self, XRectangle rec)
{
    int x,y;

    if (!self->emap || self->read_only)
	return;

    for(x = 0; x < rec.width; x++)
	for(y = 0; y < rec.height; y++)
	    EditInsert (self, x + rec.x, y + rec.y, 0);

    return;
}


/*
 * member: wipe
 * x,y   : point to start fill
 */
void EditWipeRectangle(Edit self, XRectangle rec)
{
    int x,y;

    if (!self || !self->emap || self->read_only)
	return;

    for(x=0; x < rec.width; x++)
	for(y=0; y < rec.height; y++)
	    while(get_map_ob(self->emap,x + rec.x, y + rec.y))
		EditObjectDelete (self, x + rec.x, y + rec.y, 0);
    CrEditRefresh (self->w, rec);
    return;
}

/*
 * member: shave
 * x,y   : point to start fill
 */
void EditShaveRectangle(Edit self, XRectangle rec)
{
    int x,y;

    if (!self || !self->emap || self->read_only)
	return;

    for(x=0; x < rec.width; x++)
	for(y=0; y < rec.height; y++)
	    EditObjectDelete (self, x + rec.x, y + rec.y, 0);

    CrEditRefresh (self->w, rec);
    return;
}

void EditShaveRectangleBelow(Edit self, XRectangle rec)
{
    int x,y;

    if (!self || !self->emap || self->read_only)
	return;

    for(x=0; x < rec.width; x++)
	for(y=0; y < rec.height; y++)
	    EditObjectDelete (self, x + rec.x, y + rec.y, -1);

    CrEditRefresh (self->w, rec);
    return;
}

/*
 * member: copy rectangle area (x,y,width,height) from src to
 *         self to point (x,y)
 */
void EditCopyRectangle(Edit self,Edit src,XRectangle rect,int sx,int sy)
{
    int x, y, z;
    object *obj,*tmp;

    if(!self || 
       !src ||
       self->read_only)
	return;

    if (rect.width > MAP_WIDTH(self->emap) - sx)
	rect.width = MAP_WIDTH(self->emap) - sx;
    if (rect.width > MAP_WIDTH(src->emap))
	rect.width = MAP_WIDTH(src->emap);

    if (rect.height > MAP_HEIGHT(self->emap) - sy)
	rect.height = MAP_HEIGHT(self->emap);
    if (rect.height > MAP_HEIGHT(src->emap))
	rect.height = MAP_HEIGHT(src->emap);

    debug2("EditCopyRectangle() %s -> %s\n",EditGetPath(src),
	  EditGetPath(self));
    if(self->overwrite) 
	for(x=0; x < rect.width; x++)
	    for(y=0; y < rect.height; y++)
		EditDeletePoint(self,(sx+x),(sy+y));
		
    for(x=0; x < rect.width; x++)
	for(y=0; y < rect.height; y++) {
	    obj = MapGetObjectZ(src->emap,(rect.x + x),(rect.y + y),0);
	    for(tmp = obj,z=0; tmp; tmp = tmp->below,z++) {
		if(tmp->head) {continue;}
		/*EditObjectInsert(self,tmp,(sx+x),(sy+y),z);*/
		EditCloneInsert(self,tmp,(sx+x),(sy+y),z);
	    }
	}
    rect.x = sx;
    rect.y = sy;
    CrEditRefresh (self->w, rect);
    EditModified(self);
}

/*
 *
 */
void EditDeletePoint(Edit self,int x,int y)
{
    while(get_map_ob(self->emap,x,y))
	EditDelete(self,x,y,0);
}

/*
 * Member: set Edit to modified state
 */
void EditModified(Edit self)
{
    if (self && !self->modified) {
      self->modified = True;
      EditUpdate(self);
    }
}

/*
 * member: set Edit to modified state
 */
void EditUnmodified(Edit self)
{
    if (self && self->modified) {
      self->modified = False;
      EditUpdate(self);
    }
}

static void EditObjectCalc(Edit self,object *obj,XRectangle *rect)
{
    object *tmp;

    rect->x = 10000;
    rect->y = 10000;
    rect->width = 0;
    rect->height = 0;
    for (tmp = obj; tmp; tmp = tmp->more) {
	if (self->app->look.rect.x == tmp->x && 
	    self->app->look.rect.y == tmp->y)
	    AppUpdate(self->app);
	if(tmp->x < rect->x) rect->x = tmp->x;
	if(tmp->y < rect->y) rect->y = tmp->y;
	if(tmp->x - rect->x + 1> rect->width) 
	    rect->width = tmp->x - rect->x + 1;
	if(tmp->y - rect->y + 1> rect->height) 
	    rect->height = tmp->y - rect->y + 1;
    }
}

/*
 * member: delete object from map of editor
 * x,y,z : delete object from self point
 * return: True, if object deleted
 */
Boolean EditDelete (Edit self, int x, int y, int z)
{
    object *obj;
    XRectangle rect;

    obj = MapGetObjectZ(self->emap, x, y, z);
    if (self->read_only || z < 0 || !obj)
      return False;
    if (obj->head)
	obj = obj->head;

    EditObjectCalc (self,obj,&rect);
    if (!EditObjectDelete (self, x, y, z))
      return False;
    CrEditRefresh (self->w,rect);

    if (self->app->item.wall_map)
	draw_remove (self, x, y);
    
    EditModified(self);
    return True;
}

/*
 * member: delete object from map of editor
 * x,y,z : delete object from self point
 * return: True, if object deleted
 */
Boolean EditObjectDelete (Edit self, int x, int y, int z)
{
    object *obj;

    obj = MapGetObjectZ(self->emap, x, y, z);
    if (self->read_only || !obj)
      return False;

    if (obj->head)
		obj = obj->head;

    if (z < 0) {
	if (obj->below)
	    obj = obj->below;
	else
	    return False;
    }
    /* This is just plain wrong 
    **                    -- deletes object above the selected object!
    ** if (z > 0)
    **    obj = obj->above;
    */
    if (self->app->attr && self->app->attr->op == obj)
	AttrChange(self->app->attr, NULL, 0, 0);
    if (AppItemGetEdit(self->app) == self)
      AppItemSet (self->app, NULL,NULL,0);

    debug4("EditDelete() %s in %dx%dx%d\n",obj->name,x, y, z);

    remove_ob (obj);
    free_object (obj);
    /* remove_ob should do this for us */
    /* But it doesnt - ds */
    SET_MAP_FLAGS(self->emap, x, y, P_NEED_UPDATE);
    update_position (self->emap, x, y);
    return True;
}

/*
 * member: insert object to point of map of editor
 * object:
 * x,y,z : point to insert
 * return: True, if obejct inserted
 */
object *EditCloneInsert (Edit self,object *obj,int x, int y, int z)
{
    object *op;
    int button_value;

    /*** create & insert ***/
    if ((op = object_create_clone(obj))) {
	/*** object do not fit ***/
	if(!MapObjectOut(self->emap,op,x,y)) {
	    MapInsertObjectZ (self->emap, op, x, y, z);
	    if (QUERY_FLAG(obj, FLAG_IS_LINKED) && 
	      (button_value = get_button_value(obj))) {
		add_button_link(op, self->emap, button_value);
	      }
	    return op;
	}
    } 
    debug0 ("Inserting failed\n");
    return NULL;
}

/*
 * member: insert object to point of map of editor
 * x,y,z : point to insert
 * return: True, if obejct inserted
 */
Boolean EditInsert (Edit self,int x, int y, int z)
{
    int zi = z;
    object *op,*tmp;
    XRectangle rect;

    /*** ***/
    if (!self ||
	self->read_only ||
	!self->emap || 
	out_of_map (self->emap, x, y))
	return False;

    /*** something to insert ***/
    if (!AppItemGetObject(self->app) && !AppItemGetMap(self->app)) {
	CnvNotify ( "Select item to insert.", "OK", NULL);
	return False;
    }
    /*** check for duplicate object ***/
    if (z >= 0) {
	op = get_map_ob (self->emap, x, y);
	while (op && op->above)
	    op = op->above;
	while (op && zi-- > 0)
	    op = op->below;
	if (op && op->arch == self->app->item.clone->arch)
	    return False;
    }
    /* Hopefully this doesn't break anything */
    else {
	op = get_map_ob (self->emap, x, y);
	if (op && op->arch == self->app->item.clone->arch)
	    return False;
    }

    debug3("EditInsert() %dx%dx%d\n",x,y,z);

    /*** handle background ***/
    if (self->overwrite)
	while (get_map_ob (self->emap, x, y))
	    EditDelete(self, x, y, 0);
    else {
	tmp = MapGetObjectZ (self->emap, x, y, z);
	if (tmp && 
	    (tmp->arch == self->app->item.clone->arch ||
	    (self->app->item.wall_map &&
	      find_draw_arch (AppItemGetMap(self->app),
		AppItemGetWall(self->app),
		tmp->arch) >= 0)))
	    return False;
    }

    /*** create & insert ***/
    op = EditCloneInsert (self, self->app->item.clone, x, y, z);

    if (self->app->item.wall_map) 
	draw_add (self, x, y);

    EditObjectCalc(self,op,&rect);
    SET_MAP_FLAGS(self->emap, x, y, P_NEED_UPDATE);
    update_position (self->emap, x, y);
    CrEditRefresh(self->w,rect);

    EditModified(self);
    return True;
}


/*
 * member: insert object to point of map of editor
 * object:
 * x,y,z : point to insert
 * return: True, if obejct inserted
 */
Boolean EditObjectInsert (Edit self,object *obj,int x, int y, int z)
{
    int zi = z;
    object *op,*tmp;
    XRectangle rect;

    /*** ***/
    if (!self ||
	self->read_only ||
	!self->emap || 
	out_of_map (self->emap, x, y) ||
	!obj)
	return False;

    /*** check for duplicate object ***/
    op = get_map_ob (self->emap, x, y);
    while (op && op->above)
	op = op->above;
    while (op && zi-- > 0)
	op = op->below;
    if (op && op->arch == obj->arch)
	return False;

    debug3("EditInsert() %dx%dx%d\n",x,y,z);

    /*** handle background ***/
    tmp = MapGetObjectZ (self->emap, x, y, z);
    if (tmp && (tmp->arch == obj->arch))
	return False;
    
    /*** create & insert ***/
    op = EditCloneInsert (self, op, x, y, z);

    EditObjectCalc(self,op,&rect);
    CrEditRefresh(self->w,rect);

    EditModified(self);
    return True;
}

/*
 * member: set path name of map
 * path  : path of file representing map
 */
void EditSetPath(Edit self,String path)
{
    if(!self) return;
    strcpy(self->emap->path,path);
    EditUpdate(self);
}

/*** end of Edit.c ***/

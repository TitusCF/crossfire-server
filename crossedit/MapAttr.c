/*
 *  CrossEdit - game world editor
 *  Copyright (C) 2005 Mark Wedel & Crossfire Development Team
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
 *  The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

#include <assert.h>

#include "MapAttr.h"
#include "CrFace.h" /* XtNadjust */
#include "debug.h" /* debug0, etc */

/*
 * getValue functions
 */

/* Buffer size is defined in MapAttrChange(). */
#define CPY_INT(x) snprintf(str, BUFSIZ, "%d", (x));
#define CPY_STR(x) snprintf(str, BUFSIZ, "%s", (x));

static void getEnterX(mapstruct *map, char *str) {
    CPY_INT(MAP_ENTER_X(map));
}

static void getEnterY(mapstruct *map, char *str) {
    CPY_INT(MAP_ENTER_Y(map));
}

static void getWidth(mapstruct *map, char *str) {
    CPY_INT(MAP_WIDTH(map));
}

static void getHeight(mapstruct *map, char *str) {
    CPY_INT(MAP_HEIGHT(map));
}

static void getTileMapn(mapstruct *map, char *str, int n) {
    if (map->tile_path[n] == NULL) {
        str[0] = '\0';
    } else {
        CPY_STR(map->tile_path[n]);
    }
}

static void getTileMap0(mapstruct *map, char *str) {
    getTileMapn(map, str, 0);
}

static void getTileMap1(mapstruct *map, char *str) {
    getTileMapn(map, str, 1);
}

static void getTileMap2(mapstruct *map, char *str) {
    getTileMapn(map, str, 2);
}

static void getTileMap3(mapstruct *map, char *str) {
    getTileMapn(map, str, 3);
}

static void getTimeout(mapstruct *map, char *str) {
    CPY_INT(MAP_TIMEOUT(map));
}

static void getReset(mapstruct *map, char *str) {
    CPY_INT(MAP_RESET_TIMEOUT(map));
}

static void getDarkness(mapstruct *map, char *str) {
    CPY_INT(MAP_DARKNESS(map));
}

static void getDifficulty(mapstruct *map, char *str) {
    CPY_INT(MAP_DIFFICULTY(map));
}

static void getNoReset(mapstruct *map, char *str) {
    *str = map->fixed_resettime;
}

static void getOutdoor(mapstruct *map, char *str) {
    *str = MAP_OUTDOORS(map);
}

static void getRegion(mapstruct *map, char *str) {
    CPY_STR(get_name_of_region_for_map(map));
}

static void getNoSmooth(mapstruct *map, char *str) {
    *str = MAP_NOSMOOTH(map);
}

#undef CPY_STR
#undef CPY_INT

/*
 * putValue functions
 */

static void putEnterX(mapstruct *map, const char *str) {
    MAP_ENTER_X(map) = atoi(str);
}

static void putEnterY(mapstruct *map, const char *str) {
    MAP_ENTER_Y(map) = atoi(str);
}

static void putTileMapn(mapstruct *map, const char *str, int n) {
    if (map->tile_path[n] != NULL) {
        free(map->tile_path[n]);
        map->tile_path[n] = NULL;
    }
    if (str != NULL && strcmp(str, "") != 0) {
        map->tile_path[n] = strdup_local(str);
    }
}

static void putTileMap0(mapstruct *map, const char *str) {
    putTileMapn(map, str, 0);
}

static void putTileMap1(mapstruct *map, const char *str) {
    putTileMapn(map, str, 1);
}

static void putTileMap2(mapstruct *map, const char *str) {
    putTileMapn(map, str, 2);
}

static void putTileMap3(mapstruct *map, const char *str) {
    putTileMapn(map, str, 3);
}

/* Rather than resize the map twice, record the new size here,
   and apply it once (see MapAttrApply()). */
static int new_width, new_height;

static void putWidth(mapstruct *map, const char *str) {
    new_width = atoi(str);
}
static void putHeight(mapstruct *map, const char *str) {
    new_height = atoi(str);
}

static void putTimeout(mapstruct *map, const char *str) {
    MAP_TIMEOUT(map) = atoi(str);
}

static void putReset(mapstruct *map, const char *str) {
    MAP_RESET_TIMEOUT(map) = atoi(str);
}

static void putDarkness(mapstruct *map, const char *str) {
    MAP_DARKNESS(map) = atoi(str);
}

static void putDifficulty(mapstruct *map, const char *str) {
    MAP_DIFFICULTY(map) = atoi(str);
}

static void putOutdoor(mapstruct *map, const char *str) {
    if (*str)
        MAP_OUTDOORS(map) = 1;
    else
        MAP_OUTDOORS(map) = 0;
}

static void putRegion (mapstruct *map, const char *str) {
    map->region = get_region_by_name(str);
}

static void putNoSmooth(mapstruct *map, const char *str) {
    if (*str)
        MAP_NOSMOOTH(map) = 1;
    else
        MAP_NOSMOOTH(map) = 0;
}

static void putNoReset(mapstruct *map, const char *str) {
    if (*str)
        map->fixed_resettime = 1;
    else
        map->fixed_resettime = 0;
}



/**********************************************************************
 * tags
 **********************************************************************/

static const MapAttrDef MapDescription[] = {
    {"Start X",         MATypeString, getEnterX,        putEnterX},
    {"Start Y",         MATypeString, getEnterY,        putEnterY},
    {"Width",           MATypeString, getWidth,         putWidth},
    {"Height",          MATypeString, getHeight,        putHeight},

    {"Swap Time",       MATypeString, getTimeout,       putTimeout},
    {"Reset Timeout",   MATypeString, getReset,         putReset},

    {"Difficulty",      MATypeString, getDifficulty,    putDifficulty},
    {"Darkness",        MATypeString, getDarkness,      putDarkness},
    {"Region",          MATypeString, getRegion,        putRegion},

    {"East Map",        MATypeString, getTileMap1,      putTileMap1},
    {"West Map",        MATypeString, getTileMap3,      putTileMap3},
    {"North Map",       MATypeString, getTileMap0,      putTileMap0},
    {"South Map",       MATypeString, getTileMap2,      putTileMap2},

    {"Outdoor",         MATypeToggle, getOutdoor,       putOutdoor},
    {"No Smoothing",    MATypeToggle, getNoSmooth,      putNoSmooth},
    {"FixedReset",      MATypeToggle, getNoReset,       putNoReset},
};

static const int NUM_ATTRS = sizeof(MapDescription)/sizeof(MapAttrDef);


/* Build the widgets for the above list of attributes. */
static void MapAttrTagsCreate(MapAttr self, Widget parent)
{
    Widget cont;
    int i;

    self->tags = (Widget *)XtCalloc(NUM_ATTRS, sizeof(Widget));

    /* First, those that use a textbox, */
    for (i = 0; i < NUM_ATTRS; i++) {
        if (MapDescription[i].type == MATypeString) {

            /* A layout composite widget to keep the
               label to the left of the entry box. */
            cont = XtVaCreateManagedWidget
                ("box", boxWidgetClass, parent,
                 XtNorientation, XtorientHorizontal,
                 NULL);
            XtVaCreateManagedWidget
                ("label", labelWidgetClass, cont,
                 XtNlabel, MapDescription[i].label,
                 NULL);
            self->tags[i] = XtVaCreateManagedWidget
                ("value", asciiTextWidgetClass, cont,
                 XtNtype, XawAsciiString,
                 XtNeditType, XawtextEdit,
                 NULL);
        }
    }

    /* Then those that toggle. */
    for (i = 0; i < NUM_ATTRS; i++) {
        if (MapDescription[i].type == MATypeToggle) {

            /* Also, Crossedit.ad puts a few pixels' spacing
               around each list entry by putting a width
               on the *box's* border. */
            cont = XtVaCreateManagedWidget
                ("box", boxWidgetClass, parent,
                 XtNorientation, XtorientHorizontal,
                 NULL);

            self->tags[i] = XtVaCreateManagedWidget
                ("toggle", toggleWidgetClass, cont,
                 XtNlabel, MapDescription[i].label,
                 NULL);
        }
    }
}

/**
 * Write the values from our widgets into self->map. Sort of the reverse of
 * MapAttrChange().
 */
static void MapAttrApply(MapAttr self)
{
    String str;
    int len;
    int i;

    if (self->map == NULL) {
        return;
    }

    /*** name ***/
    XtVaGetValues(self->iw.name,
                  XtNstring, &str,
                  NULL);
    free(self->map->name);
    self->map->name = strdup_local(str);

    /*** message ***/
    XtVaGetValues(self->iw.msg,
                  XtNstring, &str,
                  NULL);

    free(self->map->msg);
    len = strlen(str);
    if (len > 0) {
        if (str[len-1] == '\n') {
            self->map->msg = strdup_local(str);
        } else {
            char *newmsg;

            /* The msg doesn't end with a newline. Append one, otherwise
               the headers can't be read properly. */

            newmsg = malloc(len+2);
            memcpy(newmsg, str, len);
            newmsg[len] = '\n';
            newmsg[len+1] = '\0';

            self->map->msg = newmsg;
        }
    } else {
        self->map->msg = NULL;
    }

    /* For each attribute, extract the state of the input widget,
       and write it to the map. */
    for (i = 0; i < NUM_ATTRS; i++) {
        const MapAttrDef *which = &MapDescription[i];
        if (which->type == MATypeString) {
            XtVaGetValues(self->tags[i], XtNstring, &str, NULL);
            which->putValue(self->map, str);
        } else if (which->type == MATypeToggle) {
            Boolean tmp;
            XtVaGetValues(self->tags[i], XtNstate, &tmp, NULL);
            *str = tmp ? ~0 : 0;
            which->putValue(self->map, str);
        }
    }

    /* The entries for the width and height store in new_width and new_height,
       so we only resize once. */
    if (new_width != MAP_WIDTH(self->map) ||
        new_height != MAP_HEIGHT(self->map)) {
        EditResizeScroll(self->client, new_width, new_height, 0, 0);
    } else {
        EditUpdate(self->client);
    }

    /*** update ***/
    AppUpdate(self->app);

    /* "Dirty" the map. */
    EditModified(self->client);
}

/*
 * callback: receive ok
 */
static void OkCb(Widget w, XtPointer client, XtPointer call)
{
    MapAttr self = (MapAttr)client;

    MapAttrApply(self);
    MapAttrDestroy(self);
}

/*
 * callback: receive apply
 */
static void ApplyCb(Widget w, XtPointer client, XtPointer call)
{
    MapAttr self = (MapAttr)client;

    MapAttrApply(self);
}

/*
 * callback: receive cancel
 */
static void CancelCb(Widget w, XtPointer client, XtPointer call) {
    MapAttr self = (MapAttr)client;

    MapAttrDestroy(self);
}

static void MapAttrLayout(MapAttr self, Widget parent) {
    Widget form, view1, pane, ok, apply, cancel;

    /* This widget gets decorated by the window manager.
       ('Shell' is possibly the Intrinsics name for such widgets.) */
    self->shell = XtVaCreatePopupShell
        ("mapattr", topLevelShellWidgetClass, parent, NULL);

    /* A "composite" widget, for laying out the other widgets. */
    form = XtVaCreateManagedWidget("form", formWidgetClass, self->shell, NULL);

    /* One line for the name,
       (the dimensions of widgets are given in Crossedit.ad. */
    self->iw.name = XtVaCreateManagedWidget
        ("name", asciiTextWidgetClass, form,
         XtNtype, XawAsciiString,
         XtNeditType, XawtextEdit,
         XtNresize, False,
         NULL);

    /* Below the name, several lines for the msg; */
    self->iw.msg = XtVaCreateManagedWidget
        ("msg", asciiTextWidgetClass, form,
         XtNtype, XawAsciiString,
         XtNeditType, XawtextEdit,
         XtNfromVert, self->iw.name,
         NULL);

    /* Below the message window,
       a widget that doesn't display all of its one child;
       instead, it displays scrollbars. */
    view1 = XtVaCreateManagedWidget
        ("view", viewportWidgetClass, form,
         /*XtNallowVert, True,*/
         XtNforceBars, True,
         XtNfromVert, self->iw.msg,
         NULL);

    /* Inside view1, a widget that packs its many children
       as tightly as possible. We've told this one to be tall
       and narrow, so we have a vertically scrolling list, with
       the attribute widgets stacked on top of one-another. */
    pane = XtVaCreateManagedWidget
        ("pane", boxWidgetClass, view1,
         XtNorientation, XtorientVertical,
         NULL);

    /* Below the attribute pane, view1, three button-type
       widgets. */
    ok = XtVaCreateManagedWidget
      ("ok", commandWidgetClass, form,
       XtNfromVert, view1,
       NULL);
    XtAddCallback(ok, XtNcallback, OkCb, (XtPointer)self);
    apply = XtVaCreateManagedWidget
      ("apply", commandWidgetClass, form,
       XtNfromVert, view1,
       XtNfromHoriz, ok,
       NULL);
    XtAddCallback(apply, XtNcallback, ApplyCb, (XtPointer)self);
    cancel = XtVaCreateManagedWidget
      ("cancel", commandWidgetClass, form,
       XtNfromVert, view1,
       XtNfromHoriz, apply,
       NULL);
    XtAddCallback(cancel, XtNcallback, CancelCb, (XtPointer)self);

    /* Add the individual attribute widgets to
       their pane. */
    MapAttrTagsCreate(self, pane);
}

/* Destroy the widget tree and free the internal data (not the map!). */
void MapAttrDestroy(MapAttr self)
{
    XtDestroyWidget(self->shell);
    XtFree((char*)self->tags);

    /* Clear the one pointer to self. */
    assert(self->client->mapattr == self);
    self->client->mapattr = NULL;

    XtFree((char*)self);
}

/* Set self's widgets to reflect the state of the given map.
   The new map is the one subsequent changes will be written to
   (with MapAttrApply()).

   This is sort of MapAttrApply()'s reverse. */
void MapAttrChange(MapAttr self, mapstruct * map) {
    int i;
    char buf[BUFSIZ];

    self->map = map;

    /* Give the name and msg boxes their new values. */
    XtVaSetValues(self->iw.name, XtNstring, self->map->name, NULL);
    XtVaSetValues(self->iw.msg, XtNstring, self->map->msg, NULL);

    /* For each attribute, */
    for (i = 0; i < NUM_ATTRS; i++) {
        /* fetch the value from the map,
           and give it to the matching widget. */

        const MapAttrDef *which = &MapDescription[i];
        which->getValue(map, buf);
        if (which->type == MATypeString) {
              XtVaSetValues(self->tags[i],
                            XtNstring, buf,
                            NULL);

              /* Put the cursor at the end of the line. */
              XtVaSetValues(self->tags[i],
                            XtNinsertPosition, strlen(buf),
                            NULL);
        } else if (which->type == MATypeToggle) {
              XtVaSetValues(self->tags[i],
                            XtNstate, *buf ? TRUE : FALSE,
                            NULL);
        }
    }

    snprintf(buf, sizeof(buf), "Map: %s", self->map->name);
    XtVaSetValues(self->shell,
                  XtNtitle, buf,
                  XtNiconName, buf,
                  NULL);
}

MapAttr MapAttrCreate(mapstruct * map, App app, Edit client)
{
    MapAttr self = (MapAttr)XtMalloc(sizeof(struct _MapAttr));
    self->map = map;
    self->app = client->app;
    self->client = client;

    /* Build the widgets, */
    MapAttrLayout(self, self->app->shell);

    /* fill them with data, */
    MapAttrChange(self, map);

    /* and display the window. */
    XtPopup(self->shell, XtGrabNone);

    return self;
}

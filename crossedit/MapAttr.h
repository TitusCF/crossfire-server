#ifndef MAPATTR_H
#define MAPATTR_H

#include "App.h"
#include "Cnv.h"
#include "Defines.h"

/**********************************************************************/

/**
 * Get a value from the mapstruct and put it in the string; the string is
 * BUFSIZ long.
 */
typedef void (*MapAttrGetFunc)(mapstruct *, char *);

/**
 * Write to the mapstruct based on the string given.
 */
typedef void (*MapAttrPutFunc)(mapstruct *, const char *);

typedef enum _MapAttrType {
    MATypeString,
    MATypeToggle
} MapAttrType;

/* Toggle types store a nonzero for true in the first octet of the buffer, and
 * read the first octet of whatever they're passed.
 */

typedef struct {
    String label;            /* Human-readable name of property. */
    MapAttrType type;
    MapAttrGetFunc getValue;
    MapAttrPutFunc putValue;
} MapAttrDef;

struct _MapAttr {
    App app;
    mapstruct *map;          /* map being edited */
    Edit client;             /* Edit-window that created this */
    Widget shell;            /* popup-shell */
    struct {
        Widget name;         /* asciiText, one line */
        Widget msg;          /* ditto */
    } iw;
    Widget *tags;            /* asciiText or toggle For each MapAttrDef */
};

/* MapAttr is defined in Defines.h. */

/*
 * function inteface
 */
MapAttr MapAttrCreate(mapstruct *map, App app, Edit client);
void MapAttrDestroy(MapAttr self);
void MapAttrChange(MapAttr self, mapstruct *map);

#endif /* MAPATTR_H */

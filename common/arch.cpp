/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file arch.cpp
 * All archetype-related functions.
 * @note
 * The naming of these functions is really poor - they are all
 * pretty much named '.._arch_...', but they may more may not
 * return archetypes.  Some make the arch_to_object call, and thus
 * return an object.  Perhaps those should be called 'archob' functions
 * to denote they return an object derived from the archetype.
 * MSW 2003-04-29
 * @todo
 * make the functions use the same order for parameters (type first, then name, or the opposite).
 */

extern "C" {
#include "global.h"
}

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"

#include <string>
#include <map>
#include "assets.h"
#include "AssetsManager.h"

/**
 * This function retrieves an archetype given the name that appears
 * during the game (for example, "writing pen" instead of "stylus").
 * It does not use the hashtable system, but browse the whole archlist each time.
 * I suggest not to use it unless you really need it because of performance issue.
 * It is currently used by scripting extensions (create-object).
 * Params:
 * @param name
 * the name we're searching for (ex: "writing pen")
 * @return
 * the archetype found or NULL if nothing was found
 */
archetype *find_archetype_by_object_name(const char *name) {
    return getManager()->archetypes()->findByObjectName(name);
}

/**
 * This function retrieves an archetype by type and name that appears during
 * the game. It is basically the same as find_archetype_by_object_name()
 * except that it considers only items of the given type.
 * @param type
 * item type we're searching
 * @param name
 * the name we're searching for (ex: "writing pen")
 */
archetype *find_archetype_by_object_type_name(int type, const char *name) {
    return getManager()->archetypes()->findByObjectTypeName(type, name);
}

/**
 * Retrieves an archetype by skill name and type.
 * This is a lot like the other get_archetype_ functions, with different options.
 * @param skill
 * skill to search for. Must not be NULL.
 * @param type
 * item type to search for. -1 means that it doesn't matter.
 * @return
 * matching archetype, or NULL if none found.
 */
archetype *get_archetype_by_skill_name(const char *skill, int type) {
    return getManager()->archetypes()->findBySkillNameAndType(skill, type);
}

/**
 * Retrieves an archetype by type and subtype.
 * Similiar to other get_archetype_ functions. This returns the first archetype
 * that matches both the type and subtype.  type and subtype
 * can be -1 to say ignore, but in this case, the match it does
 * may not be very useful.  This function is most useful when
 * subtypes are known to be unique for a particular type
 * (eg, skills)
 * @param type
 * object type to search for. -1 means any
 * @param subtype
 * object subtype to search for. -1 means any
 * @return
 * matching archetype, or NULL if none found.
 */
archetype *get_archetype_by_type_subtype(int type, int subtype) {
    return getManager()->archetypes()->findByTypeSubtype(type, subtype);
}

/**
 * Creates an object given the name that appears during the game
 * (for example, "writing pen" instead of "stylus").
 * @param name
 * the name we're searching for (ex: "writing pen"), must not be null
 * @return
 * a corresponding object if found; a singularity object if not found.
 * @note
 * Note by MSW - it appears that it takes the full name and keeps
 * shortening it until it finds a match.  I re-wrote this so that it
 * doesn't malloc it each time - not that this function is used much,
 * but it otherwise had a big memory leak.
 */
object *create_archetype_by_object_name(const char *name) {
    archetype *at;
    char tmpname[MAX_BUF];
    size_t i;

    strncpy(tmpname, name, MAX_BUF-1);
    tmpname[MAX_BUF-1] = 0;
    for (i = strlen(tmpname); i > 0; i--) {
        tmpname[i] = 0;
        at = getManager()->archetypes()->findByObjectName(tmpname);
        if (at != NULL) {
            return arch_to_object(at);
        }
    }
    return create_singularity(name);
}

/**
 * Dumps an archetype to buffer.
 *
 * @param at
 * archetype to dump. Must not be NULL.
 * @param sb
 * buffer that will contain dumped information.
 */
void dump_arch(archetype *at, StringBuffer *sb) {
    object_dump(&at->clone, sb);
}

/**
 * Dumps _all_ archetypes to debug-level output.
 * If you run crossfire with debug, and enter DM-mode, you can trigger
 * this with the "dumpallarchetypes" command.
 */
void dump_all_archetypes(void) {
    for (auto arch = get_next_archetype(NULL); arch != NULL; arch = get_next_archetype(arch)) {

        StringBuffer *sb;
        char *diff;

        sb = stringbuffer_new();
        dump_arch(arch, sb);
        diff = stringbuffer_finish(sb);
        LOG(llevDebug, "%s\n", diff);
        free(diff);
    }
}

/**
 * Frees archetype.
 *
 * @param at
 * archetype to free. Pointer becomes invalid after the call.
 */
void free_arch(archetype *at) {
    if (at->name)
        free_string(at->name);
    if (at->clone.name)
        free_string(at->clone.name);
    if (at->clone.name_pl)
        free_string(at->clone.name_pl);
    if (at->clone.title)
        free_string(at->clone.title);
    if (at->clone.race)
        free_string(at->clone.race);
    if (at->clone.slaying)
        free_string(at->clone.slaying);
    if (at->clone.msg)
        free_string(at->clone.msg);
    object_free_key_values(&at->clone);
    free(at);
}

/**
 * Allocates, initialises and returns the pointer to an archetype structure.
 * @return
 * new archetype structure, will never be NULL.
 * @note
 * this function will call fatal() if it can't allocate memory.
 */
archetype *get_archetype_struct(void) {
    archetype *arch;

    arch = (archetype *)CALLOC(1, sizeof(archetype));
    if (arch == NULL)
        fatal(OUT_OF_MEMORY);
    arch->name = NULL;
    arch->clone.other_arch = NULL;
    arch->clone.name = NULL;
    arch->clone.name_pl = NULL;
    arch->clone.title = NULL;
    arch->clone.race = NULL;
    arch->clone.slaying = NULL;
    arch->clone.msg = NULL;
    object_clear(&arch->clone);  /* to initial state other also */
    CLEAR_FLAG((&arch->clone), FLAG_FREED); /* This shouldn't matter, since object_copy() */
    SET_FLAG((&arch->clone), FLAG_REMOVED); /* doesn't copy these flags... */
    arch->head = NULL;
    arch->more = NULL;
    arch->clone.arch = arch;
    return arch;
}

/**
 * Creates and returns a new object which is a copy of the given archetype.
 * This function returns NULL if given a NULL pointer, else an object.
 * @param at
 * archetype from which to get an object.
 * @return
 * object of specified type.
 * @note
 * object_new() will either allocate memory or call fatal(), so returned value
 * is never NULL.
 */
object *arch_to_object(archetype *at) {
    object *op;

    if (at == NULL) {
        LOG(llevError, "Couldn't find archetype.\n");
        return NULL;
    }
    op = object_new();
    object_copy_with_inv(&at->clone, op);
    op->arch = at;
    return op;
}

/**
 * Creates a dummy object. This function is called by get_archetype()
 * if it fails to find the appropriate archetype.
 * Thus get_archetype() will be guaranteed to always return
 * an object, and never NULL.
 * @param name
 * name to give the dummy object.
 * @return
 * object of specified name. It fill have the ::FLAG_NO_PICK flag set.
 * @note
 * object_new() will either allocate memory or call fatal(), so returned value
 * is never NULL.
 */
object *create_singularity(const char *name) {
    object *op;
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), "%s (%s)", ARCH_SINGULARITY, name);
    op = object_new();
    op->arch = empty_archetype;
    op->name = add_string(buf);
    op->name_pl = add_string(buf);
    SET_FLAG(op, FLAG_NO_PICK);
    return op;
}

/**
 * Finds which archetype matches the given name, and returns a new
 * object containing a copy of the archetype.
 * @param name
 * archetype name
 * @return
 * object of specified archetype, or a singularity. Will never be NULL.
 * @todo
 * replace with object_create_arch() which is multi-part aware.
 */
object *create_archetype(const char *name) {
    archetype *at;

    at = try_find_archetype(name);
    if (at == NULL)
        return create_singularity(name);
    return arch_to_object(at);
}

/**
 * Create a full object using the given archetype.
 * This instanciate not only the archetype but also
 * all linked archetypes in case of multisquare archetype.
 * @param at
 * archetype to instanciate. Must not be NULL.
 * @return
 * pointer to head of instance.
 * @note
 * will never return NULL.
 */
object *object_create_arch(archetype *at) {
    object *op, *prev = NULL, *head = NULL;

    while (at) {
        op = arch_to_object(at);
        op->x = at->clone.x;
        op->y = at->clone.y;
        if (head)
            op->head = head, prev->more = op;
        if (!head)
            head = op;
        prev = op;
        at = at->more;
    }
    return (head);
}

/*** end of arch.cpp ***/

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2010 Crossfire Development Team

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

    The authors can be reached via e-mail at crossfire@metalforge.org
*/

/**
@file

Handling of player knowledge of various things.

Right now, the following items are considered:
- alchemy formulea
- monster information

Knowledge is specific for a player, and organized in 'items'.
One item is a specific formulae or monster, whatever is coded.

Note that some things are done through functions in readable.c, especially item marking.

Knowledge is persistent and without errors. Maybe that could be changed in the future?



Only use non static functions when using this file.

Loading is done on a per-demand basis, writing at each change.

Knowledge is stored in a "player.knowledge" file in the player's directory.

Knowledge relies on key/values. Key is "knowledge_marker", value is specific to the
item class considered and must have a left-side part, delimited by a double dot, specifying a type.

The value should be enough to determine a unique formulae or monster etc.

A type is linked to various functions for displaying, checking the key value.

During loading, obsolete items (eg formula changed) are discarded.



@todo
- make knowledge shareable between players
- more things to keep trace of
- client/server protocol to query
*/

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

struct knowledge_player;
struct knowledge_type;

/** Function to fill the StringBuffer with the short description of an item. */
typedef void (*knowledge_summary)(const char *, StringBuffer *);
/** Function to fill the StringBuffer with a detailed description of an item. */
typedef void (*knowledge_detail)(const char *, StringBuffer *);
/** Function to check if the specified item is valid. */
typedef int (*knowledge_is_valid_item)(const char *);
/** Function to add the specified item, should return how many actually written. */
typedef int (*knowledge_add_item)(struct knowledge_player *, const char *, const struct knowledge_type *);
/** One item type that may be known to the player. */
typedef struct knowledge_type {
    const char *type;                   /**< Type internal code, musn't have a double dot, must be unique ingame. */
    knowledge_summary summary;          /**< Display the short description. */
    knowledge_detail detail;            /**< Display the detailed description. */
    knowledge_is_valid_item validate;   /**< Validate the specific value. */
    knowledge_add_item add;             /**< Add the item to the knowledge store. */
    const char *name;                   /**< Type name for player, to use with 'list'. */
} knowledge_type;


/** One known item for a player. */
typedef struct knowledge_item {
    sstring item;                   /**< Internal item code. */
    struct knowledge_item *next;    /**< Next item on the list. */
    const knowledge_type* handler;  /**< How to handle this item. */
} knowledge_item;

/** Information about a player. */
typedef struct knowledge_player {
    sstring player_name;            /**< Player's name. */
    struct knowledge_item *items;   /**< Known knowledge. */
    struct knowledge_player *next;  /**< Next player on the list. */
} knowledge_player;

/** All known loaded knowledge for a player. */
static knowledge_player *knowledge_global = NULL;


/**
 * Get a recipe from the internal code.
 * @param value code to get value for.
 * @return recipe, NULL if not found.
 */
static const recipe* knowledge_alchemy_get_recipe(const char *value) {
    const recipe *rec;
    recipelist *rl;
    int count, index;
    const char *dot;

    dot = strchr(value, ':');
    if (!dot) {
        /* warn? */
        return NULL;
    }

    count = atoi(value);
    index = atoi(dot + 1);
    dot = strchr(dot + 1, ':');
    if (!dot)
        /* warn? */
        return NULL;
    dot++;

    rl = get_formulalist(count);
    rec = rl->items;
    while (rec) {
        if (rec->index == index && strcmp(rec->title, dot) == 0)
            return rec;

        rec = rec->next;
    }

    return NULL;
}

/**
 * Give the title of the alchemy recpie.
 * @param value recipe internal value.
 * @param buf where to put the information.
 * @todo merge with stuff in readable.c
 */
static void knowledge_alchemy_summary(const char *value, StringBuffer *buf) {
    const recipe *rec = knowledge_alchemy_get_recipe(value);
    const archetype *arch;

    if (!rec)
        /* warn? */
        return;

    arch = find_archetype(rec->arch_name[RANDOM()%rec->arch_names]);
    if (!arch)
        /* not supposed to happen */
        return;

    if (strcmp(rec->title, "NONE"))
        stringbuffer_append_printf(buf, "%s of %s", arch->clone.name, rec->title);
    else {
        if (arch->clone.title != NULL) {
            stringbuffer_append_printf(buf, "%s %s", arch->clone.name, arch->clone.title);
        }
        else
            stringbuffer_append_printf(buf, "%s", arch->clone.name);
    }
}

/**
 * Give the full description of the alchemy recpie.
 * @param value recipe internal value.
 * @param buf where to store the detail.
 * @todo merge with stuff in readable.c
 */
static void knowledge_alchemy_detail(const char *value, StringBuffer *buf) {
    const recipe *rec = knowledge_alchemy_get_recipe(value);
    const linked_char *next;
    const archetype *arch;

    if (!rec)
        /* warn? */
        return;

    arch = find_archetype(rec->arch_name[RANDOM()%rec->arch_names]);
    if (!arch)
        /* not supposed to happen */
        return;

    if (strcmp(rec->title, "NONE"))
        stringbuffer_append_printf(buf, "Recipe for %s of %s:", arch->clone.name, rec->title);
    else {
        if (arch->clone.title != NULL) {
            stringbuffer_append_printf(buf, "Recipe for %s %s:", arch->clone.name, arch->clone.title);
        }
        else
            stringbuffer_append_printf(buf, "Recipe for %s:", arch->clone.name);
    }

    for (next = rec->ingred; next != NULL; next = next->next) {
        stringbuffer_append_printf(buf, "\n - %s", next->name);
    }

}

/**
 * Check if an alchemy recipe is still ok.
 * @param item what to check for
 * @return 0 if non valid, 1 else.
 */
static int knowledge_achemy_validate(const char *item) {
    return knowledge_alchemy_get_recipe(item) != NULL;
}

/**
 * Check whether a player already knows a knowledge item or not.
 * @param current player to check.
 * @param item what to check for.
 * @param kt the knowledge type for item.
 * @return 0 if item is known, 1 else.
 */
static int knowledge_known(const knowledge_player *current, const char *item, const knowledge_type *kt) {
    const knowledge_item *check = current->items;

    while (check) {
        if (strcmp(kt->type, check->handler->type) == 0 && strcmp(item, check->item) == 0) {
            /* already known, bailout */
            return 1;
        }
        check = check->next;
    }
    return 0;
}

/**
 * Add a knowledge item to a player's store if not found yet.
 * @param current where to look for the knowledge.
 * @param item internal value of the item to give, considered atomic.
 * @param kt how to handle the knowledge type.
 * @return 0 if item was already known, 1 else.
 */
static int knowledge_add(knowledge_player *current, const char *item, const knowledge_type *kt) {
    knowledge_item *check;

    if (knowledge_known(current, item, kt)) {
        return 0;
    }

    /* keep the knowledge */
    check = calloc(1, sizeof(knowledge_item));
    check->item = add_string(item);
    check->next = current->items;
    check->handler = kt;
    current->items = check;
    return 1;
}

/**
 * Monster information summary.
 * @param item knowledge item.
 * @param buf where to put the information.
 * @todo merge with stuff in readable.c
 */
static void knowledge_monster_summary(const char *item, StringBuffer *buf) {
    archetype *monster = find_archetype(item);
    if (!monster)
        return;

    stringbuffer_append_printf(buf, "%s", monster->clone.name);
}

/**
 * Describe in detail a monster.
 * @param item knowledge item for the monster (archetype name).
 * @param buf where to put the description.
 * @todo merge with stuff in readable.c
 */
static void knowledge_monster_detail(const char *item, StringBuffer *buf) {
    char buf2[HUGE_BUF];
    archetype *monster = find_archetype(item);

    if (!monster)
        return;

    stringbuffer_append_printf(buf, " *** %s ***\n", monster->clone.name);
    describe_item(&monster->clone, NULL, buf2, sizeof(buf2));
    stringbuffer_append_string(buf, buf2);
}

/**
 * Check if a monster knowledge item is still valid.
 * @param item monster to check
 * @return 0 if invalid, 1 if valid.
 */
static int knowledge_monster_validate(const char *item) {
    return try_find_archetype(item) != NULL;
}

/**
 * Add monster information to the player's knowledge, handling the multiple monster case.
 * @param current where to add the information to.
 * @param item monster to add, can be separated by dots for multiple ones.
 * @param type pointer of the monster type.
 * @return count of actually added items.
 */
static int knowledge_monster_add(struct knowledge_player *current, const char *item, const struct knowledge_type *type) {
    char *dup = strdup_local(item);
    char *pos, *first = dup;
    int added = 0;

    while (first) {
        pos = strchr(first, ':');
        if (pos)
            *pos = '\0';
        added += knowledge_add(current, first, type);
        first = pos ? pos + 1 : NULL;
    }

    free(dup);
    return added;
}

/**
 * God information summary.
 * @param item knowledge item.
 * @param buf where to put the information.
 * @todo merge with stuff in readable.c
 */
static void knowledge_god_summary(const char *item, StringBuffer *buf) {
    char *dup = strdup_local(item), *pos = strchr(dup, ':');

    if (pos)
        *pos = '\0';

    stringbuffer_append_printf(buf, "%s [god]", dup);
    free(dup);
}

/**
 * Describe in detail a god.
 * @param item knowledge item for the god (object name and what is known).
 * @param buf where to put the description.
 * @todo merge with stuff in readable.c
 */
static void knowledge_god_detail(const char *item, StringBuffer *buf) {
    char *dup = strdup_local(item), *pos = strchr(dup, ':'), *final;
    const archetype *god;
    int what;

    if (!pos) {
        LOG(llevError, "knowledge_god_detail: invalid god item %s\n", item);
        free(dup);
        return;
    }

    *pos = '\0';
    what = atoi(pos + 1);
    god = find_archetype_by_object_name(dup);
    free(dup);

    if (!god) {
        LOG(llevError, "knowledge_god_detail: couldn't find god %s?\n", item);
        return;
    }

    describe_god(&god->clone, what, buf, 0);
}

/**
 * Check if a god knowledge item is still valid.
 * @param item monster to check
 * @return 0 if invalid, 1 if valid.
 */
static int knowledge_god_validate(const char *item) {
    char *dup = strdup_local(item), *pos = strchr(dup, ':');
    int valid;

    if (!pos) {
        LOG(llevError, "knowledge_god_validate: invalid god item %s\n", item);
        free(dup);
        return 0;
    }
    *pos = '\0';
    valid = find_archetype_by_object_name(dup) != NULL;
    free(dup);
    return valid;
}

/**
 * Add god information to the player's knowledge, handling the multiple monster case.
 * @param current where to add the information to.
 * @param item god to add, a dot separating the exact knowledge.
 * @param type pointer of the monster type.
 * @return count of actually added items.
 */
static int knowledge_god_add(struct knowledge_player *current, const char *item, const struct knowledge_type *type) {
    char *dup = strdup_local(item), *pos = strchr(dup, ':');
    StringBuffer *buf;
    int what;
    knowledge_item* check;

    if (!pos) {
        LOG(llevError, "knowledge_god_add: invalid god item %s\n", item);
        free(dup);
        return 0;
    }

    *pos = '\0';
    what = atoi(pos + 1);

    for (check = current->items; check; check = check->next) {
        if (check->handler != type)
            /* Only consider our own type. */
            continue;
        if (strncmp(check->item, dup, strlen(dup)) == 0) {
            /* Already known, update information. */
            int known;
            pos = strchr(check->item, ':');
            known = atoi(pos + 1);
            known |= what;
            buf = stringbuffer_new();
            stringbuffer_append_printf(buf, "%s:%d", dup, known);
            free_string(check->item);
            check->item = stringbuffer_finish_shared(buf);
            free(dup);
            return (what != known);
        }
    }

    free(dup);

    /* Not known, so just add it regularly. */
    return knowledge_add(current, item, type);
}

/** All handled knowledge items. */
static const knowledge_type const knowledges[] = {
    { "alchemy", knowledge_alchemy_summary, knowledge_alchemy_detail, knowledge_achemy_validate, knowledge_add, "recipes" },
    { "monster", knowledge_monster_summary, knowledge_monster_detail, knowledge_monster_validate, knowledge_monster_add, "monsters" },
    { "god", knowledge_god_summary, knowledge_god_detail, knowledge_god_validate, knowledge_god_add, "gods" },
    { NULL, 0, 0, 0, 0 }
};


/**
 * Find a knowledge handler from its type.
 * @param type internal type name.
 * @return handler, NULL if not found.
 */
static const knowledge_type *knowledge_find(const char *type) {
    int i = 0;
    while (knowledges[i].type != NULL) {
        if (strcmp(knowledges[i].type, type) == 0)
            return &knowledges[i];
        i++;
    }

    return NULL;
}

/**
 * Store all knowledge data for a player to disk.
 * @param kp player data to store.
 */
static void knowledge_write_player_data(const knowledge_player *kp) {
    FILE *file;
    char write[MAX_BUF], final[MAX_BUF];
    const knowledge_item *item;

    snprintf(final, sizeof(final), "%s/%s/%s/%s.knowledge", settings.localdir, settings.playerdir, kp->player_name, kp->player_name);
    snprintf(write, sizeof(write), "%s.new", final);

    file = fopen(write, "w+");
    if (!file) {
        LOG(llevError, "knowledge: couldn't open player knowledge file %s!", write);
        draw_ext_info(NDI_UNIQUE | NDI_ALL_DMS, 0, NULL, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE, "File write error on server!");
        return;
    }

    item = kp->items;

    while (item) {
        fprintf(file, "%s:%s\n", item->handler->type, item->item);
        item = item->next;
    }

    fclose(file);
    /** @todo rename/backup, stuff like that */
    unlink(final);
    rename(write, final);
}

/**
 * Read all knowledge data for a player from disk.
 * @param kp player structure to load data into.
 */
static void knowledge_read_player_data(knowledge_player *kp) {
    FILE *file;
    char final[MAX_BUF], read[MAX_BUF], *dot;
    knowledge_item *item;
    const knowledge_type *type;

    snprintf(final, sizeof(final), "%s/%s/%s/%s.knowledge", settings.localdir, settings.playerdir, kp->player_name, kp->player_name);

    file = fopen(final, "r");
    if (!file) {
        /* no knowledge yet, no big deal */
        return;
    }

    while (fgets(read, sizeof(read), file) != NULL) {
        if (strlen(read) > 0)
            read[strlen(read) - 1] = '\0';

        dot = strchr(read, ':');
        if (!dot) {
            LOG(llevError, "knowledge: invalid line in file %s\n", final);
            continue;
        }

        *dot = '\0';

        type = knowledge_find(read);
        if (!type) {
            LOG(llevError, "knowledge: invalid type %s in file %s\n", read, final);
            continue;
        }
        if (!type->validate(dot + 1)) {
            LOG(llevDebug, "knowledge: ignoring now invalid %s in file %s\n", read, final);
            continue;
        }

        item = calloc(1, sizeof(knowledge_item));
        item->item = add_string(dot + 1);
        item->next = kp->items;
        item->handler = type;
        kp->items = item;
    }
    fclose(file);
}

/**
 * Find or create the knowledge store for a player. Will load data if required. fatal() will be called if memory failure.
 * @param pl who to find data for.
 * @return data store, never NULL.
 */
static knowledge_player *knowledge_get_or_create(const player *pl) {
    knowledge_player *cur = knowledge_global;

    while (cur) {
        if (cur->player_name == pl->ob->name)
            return cur;
        cur = cur->next;
    }

    cur = calloc(1, sizeof(knowledge_player));
    if (!cur)
        fatal(OUT_OF_MEMORY);
    cur->player_name = add_refcount(pl->ob->name);
    cur->next = knowledge_global;
    knowledge_global = cur;
    /* read knowledge for this player */
    knowledge_read_player_data(cur);
    return cur;
}

/**
 * Player is reading a book, give knowledge if needed, warn player, and such.
 * @param pl who is reading.
 * @param book what is read.
 */
void knowledge_read(player *pl, object *book) {
    sstring marker = object_get_value(book, "knowledge_marker");
    char *dot, *copy;
    const knowledge_type *type;
    int none, added = 0;
    knowledge_player *current = knowledge_get_or_create(pl);

    if (!marker)
        return;

    /* process marker, find if already known */
    dot = strchr(marker, ':');
    if (dot == NULL)
        return;

    copy = calloc(1, strlen(marker) + 1);
    strncpy(copy, marker, strlen(marker) + 1);

    dot = strchr(copy, ':');
    *dot = '\0';
    dot++;

    type = knowledge_find(copy);
    if (!type) {
        LOG(llevError, "knowledge: invalid marker type %s in %s\n", copy, book->name);
        free(copy);
        return;
    }

    none = (current->items == NULL);
    added = type->add(current, dot, type);
    free(copy);

    if (added) {
        draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "You write that down for future reference.");
        if (none) {
            /* first information ever written down, be nice and give hint to recover it. */
            draw_ext_info(NDI_UNIQUE, 0, pl->ob, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "Use the 'knowledge' command to browse what you write down (this message will not appear anymore).");
        }
    }
    knowledge_write_player_data(current);
}

/**
 * Display all a player's knowledge.
 * @param pl who to display knowledge of.
 * @param params additionnal parameters.
 */
static void knowledge_display(object *pl, const char *params) {
    knowledge_player *kp;
    knowledge_item *item;
    int index = 1, header = 0;
    const knowledge_type *show_only = NULL;
    StringBuffer *summary;
    char *final;

    if (params && params[0] == ' ') {
        const char *type = params + 1;
        int idx = 0;
        for (; knowledges[idx].type != NULL; idx++) {
            if (strcmp(knowledges[idx].name, type) == 0) {
                show_only = &knowledges[idx];
                break;
            }
        }

        if (show_only == NULL) {
            draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "Invalid type, valid types are:");
            for (idx = 0; knowledges[idx].type != NULL; idx++) {
                draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "- %s", knowledges[idx].name);
            }
            return;
        }
    }

    kp = knowledge_get_or_create(pl->contr);
    item = kp->items;
    while (item) {
        if (show_only != NULL && item->handler != show_only) {
            item = item->next;
            index++;
            continue;
        }

        if (header == 0) {
            if (show_only != NULL)
                draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "You have knowledge of those %s:", show_only->name);
            else
                draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "You have knowledge of:");
            header = 1;
        }

        summary = stringbuffer_new();
        item->handler->summary(item->item, summary);
        final = stringbuffer_finish(summary);
        draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "(%3d) %s", index, final);
        free(final);
        item = item->next;
        index++;
    }

    if (header == 0) {
        if (show_only != NULL)
            draw_ext_info_format(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "You don't know yet any relevant information about %s.", show_only->name);
        else
            draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, "You don't know yet any relevant information.");
    }
}

/**
 * Show the details of a knowledge item.
 * @param pl who is asking for details.
 * @param params additional parameters, should contain the knowledge item number.
 */
static void knowledge_show(object *pl, const char *params) {
    knowledge_player *kp;
    knowledge_item *item;
    int count = atoi(params);

    if (count <= 0) {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Invalid knowledge number");
        return;
    }

    kp = knowledge_get_or_create(pl->contr);
    item = kp->items;
    while (item) {
        if (count == 1) {
            StringBuffer *buf = stringbuffer_new();
            char *final;

            item->handler->detail(item->item, buf);
            final = stringbuffer_finish(buf);
            draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_MISC, MSG_TYPE_CLIENT_NOTICE, final);
            free(final);
            return;
        }
        item = item->next;
        count--;
    }

    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "Invalid knowledge number");
}

/**
 * Handle the 'knowledge' for a player.
 * @param pl who is using the command.
 * @param params additional parameters.
 * @return 0
 */
int command_knowledge(object *pl, char *params) {

    if (!pl->contr) {
        LOG(llevError, "command_knowledge: called for %s not a player!\n", pl->name);
        return 0;
    }

    if (!params || *params == '\0') {
        command_help(pl, "knowledge");
        return 0;
    }

    if (strncmp(params, "list", 4) == 0) {
        knowledge_display(pl, params + 4);
        return 0;
    }

    if (strncmp(params, "show ", 5) == 0) {
        knowledge_show(pl, params + 5);
        return 0;
    }

    command_help(pl, "knowledge");
    return 0;
}

/**
 * Free all knowledge items for a player.
 * @param kp what to free.
 */
static void free_knowledge_items(knowledge_player *kp) {
    knowledge_item *item = kp->items, *next;

    while (item) {
        free_string(item->item);
        next = item->next;
        free(item);
        item = next;
    }
    kp->items = NULL;
}

/**
 * Free all knowledge data.
 */
void free_knowledge(void) {
    knowledge_player *kp, *next;

    kp = knowledge_global;
    while (kp) {
        next = kp->next;
        free_knowledge_items(kp);
        free_string(kp->player_name);
        free(kp);
        kp = next;
    }
    knowledge_global = NULL;
}

int knowledge_player_knows(const player *pl, const char *knowledge) {
    const knowledge_type *type;
    char copy[MAX_BUF], *pos;
    const knowledge_player *current;

    if (strlen(knowledge) >= MAX_BUF - 1) {
        LOG(llevError, "knowledge_player_knows: too long knowledge %s\n", knowledge);
        return 0;
    }

    strncpy(copy, knowledge, MAX_BUF);
    pos = strchr(copy, ':');
    if (pos == NULL) {
        LOG(llevError, "knowledge_player_knows: invalid knowledge item %s\n", knowledge);
        return 0;
    }

    *pos = '\0';
    pos++;

    type = knowledge_find(copy);
    if (type == NULL) {
        LOG(llevError, "knowledge_player_knows: invalid knowledge type %s\n", knowledge);
        return 0;
    }

    current = knowledge_get_or_create(pl);

    return knowledge_known(current, pos, type);
}

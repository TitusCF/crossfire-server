
/*
 * static char *rcs_treasure_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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

/**
 * @file
 * Everything concerning treasures and artifacts.
 * @see page_treasure_list
 */

#define ALLOWED_COMBINATION

/**
 * TREASURE_DEBUG does some checking on the treasurelists after loading.
 * It is useful for finding bugs in the treasures file.  Since it only
 * slows the startup some (and not actual game play), it is by default
 * left on
 */
#define TREASURE_DEBUG

/* TREASURE_VERBOSE enables copious output concerning artifact generation */
/* #define TREASURE_VERBOSE */

#include <stdlib.h>
#include <global.h>
#include <treasure.h>
#include <loader.h>
#include <sproto.h>


static void change_treasure(treasure *t, object *op); /* overrule default values */
static int special_potion(object *op);
static void fix_flesh_item(object *item, object *donor);

extern const char *const spell_mapping[];
int artifact_init;  /**< 1 if doing archetypes initialization */

/**
 * Initialize global archtype pointers:
 */
void init_archetype_pointers(void) {
    int prev_warn = warn_archetypes;

    warn_archetypes = 1;
    if (ring_arch == NULL)
        ring_arch = find_archetype("ring");
    if (amulet_arch == NULL)
        amulet_arch = find_archetype("amulet");
    if (staff_arch == NULL)
        staff_arch = find_archetype("staff");
    if (crown_arch == NULL)
        crown_arch = find_archetype("crown");
    warn_archetypes = prev_warn;
}

/**
 * Allocate and return the pointer to an empty treasurelist structure.
 *
 * @return
 * new structure, blanked, never NULL.
 *
 * @note
 * will call fatal() if memory allocation error.
 * @ingroup page_treasure_list
 */
static treasurelist *get_empty_treasurelist(void) {
    treasurelist *tl = (treasurelist *)malloc(sizeof(treasurelist));
    if (tl == NULL)
        fatal(OUT_OF_MEMORY);
    memset(tl, 0, sizeof(treasurelist));
    return tl;
}

/**
 * Allocate and return the pointer to an empty treasure structure.
 *
 * @return
 * new structure, blanked, never NULL.
 *
 * @note
 * will call fatal() if memory allocation error.
 * @ingroup page_treasure_list
 */
static treasure *get_empty_treasure(void) {
    treasure *t = (treasure *)calloc(1, sizeof(treasure));
    if (t == NULL)
        fatal(OUT_OF_MEMORY);
    t->item = NULL;
    t->name = NULL;
    t->next = NULL;
    t->next_yes = NULL;
    t->next_no = NULL;
    t->chance = 100;
    t->magic = 0;
    t->nrof = 0;
    return t;
}

/**
 * Reads one treasure from the file, including the 'yes', 'no' and 'more' options.
 *
 * @param fp
 * file to read from.
 * @param[out] line
 * position in file.
 * @return
 * read structure, never NULL.
 *
 * @todo
 * check if change_name is still used, and remove it if no.
 * @ingroup page_treasure_list
 */
static treasure *load_treasure(FILE *fp, int *line) {
    char buf[MAX_BUF], *cp, variable[MAX_BUF];
    treasure *t = get_empty_treasure();
    int value;

    nroftreasures++;
    while (fgets(buf, MAX_BUF, fp) != NULL) {
        (*line)++;

        if (*buf == '#')
            continue;
        if ((cp = strchr(buf, '\n')) != NULL)
            *cp = '\0';
        cp = buf;
        while (isspace(*cp)) /* Skip blanks */
            cp++;

        if (sscanf(cp, "arch %s", variable)) {
            if ((t->item = find_archetype(variable)) == NULL)
                LOG(llevError, "Treasure lacks archetype: %s\n", variable);
        } else if (sscanf(cp, "list %s", variable))
            t->name = add_string(variable);
        else if (sscanf(cp, "change_name %s", variable))
            t->change_arch.name = add_string(variable);
        else if (sscanf(cp, "change_title %s", variable))
            t->change_arch.title = add_string(variable);
        else if (sscanf(cp, "change_slaying %s", variable))
            t->change_arch.slaying = add_string(variable);
        else if (sscanf(cp, "chance %d", &value))
            t->chance = (uint8)value;
        else if (sscanf(cp, "nrof %d", &value))
            t->nrof = (uint16)value;
        else if (sscanf(cp, "magic %d", &value))
            t->magic = (uint8)value;
        else if (!strcmp(cp, "yes"))
            t->next_yes = load_treasure(fp, line);
        else if (!strcmp(cp, "no"))
            t->next_no = load_treasure(fp, line);
        else if (!strcmp(cp, "end"))
            return t;
        else if (!strcmp(cp, "more")) {
            t->next = load_treasure(fp, line);
            return t;
        } else
            LOG(llevError, "Unknown treasure-command: '%s', last entry %s, line %d\n", cp, t->name ? t->name : "null", *line);
    }
    LOG(llevError, "treasure lacks 'end'.\n");
    return t;
}

#ifdef TREASURE_DEBUG
/**
 * Checks if a treasure if valid. Will also check its yes and no options.
 *
 * Will LOG() to error.
 *
 * @param t
 * treasure to check.
 * @param tl
 * needed only so that the treasure name can be printed out.
 * @ingroup page_treasure_list
 */
static void check_treasurelist(const treasure *t, const treasurelist *tl) {
    if (t->item == NULL && t->name == NULL)
        LOG(llevError, "Treasurelist %s has element with no name or archetype\n", tl->name);
    if (t->chance >= 100 && t->next_yes && (t->next || t->next_no))
        LOG(llevError, "Treasurelist %s has element that has 100%% generation, next_yes field as well as next or next_no\n", tl->name);
    /* find_treasurelist will print out its own error message */
    if (t->name && strcmp(t->name, "NONE"))
        find_treasurelist(t->name);
    if (t->next)
        check_treasurelist(t->next, tl);
    if (t->next_yes)
        check_treasurelist(t->next_yes, tl);
    if (t->next_no)
        check_treasurelist(t->next_no, tl);
}
#endif

/**
 * Opens LIBDIR/treasure and reads all treasure-declarations from it.
 * Each treasure is parsed with the help of load_treasure().
 *
 * Will LOG() if file can't be accessed.
 * @ingroup page_treasure_list
 */
void load_treasures(void) {
    FILE *fp;
    char filename[MAX_BUF], buf[MAX_BUF], name[MAX_BUF];
    treasurelist *previous = NULL;
    treasure *t;
    int comp, line = 0;

    snprintf(filename, sizeof(filename), "%s/%s", settings.datadir, settings.treasures);
    if ((fp = open_and_uncompress(filename, 0, &comp)) == NULL) {
        LOG(llevError, "Can't open treasure file.\n");
        return;
    }
    while (fgets(buf, MAX_BUF, fp) != NULL) {
        line++;
        if (*buf == '#')
            continue;

        if (sscanf(buf, "treasureone %s\n", name) || sscanf(buf, "treasure %s\n", name)) {
            treasurelist *tl = get_empty_treasurelist();

            tl->name = add_string(name);
            if (previous == NULL)
                first_treasurelist = tl;
            else
                previous->next = tl;
            previous = tl;
            tl->items = load_treasure(fp, &line);

            /* This is a one of the many items on the list should be generated.
             * Add up the chance total, and check to make sure the yes & no
             * fields of the treasures are not being used.
             */
            if (!strncmp(buf, "treasureone", 11)) {
                for (t = tl->items; t != NULL; t = t->next) {
#ifdef TREASURE_DEBUG
                    if (t->next_yes || t->next_no) {
                        LOG(llevError, "Treasure %s is one item, but on treasure %s\n", tl->name, t->item ? t->item->name : t->name);
                        LOG(llevError, "  the next_yes or next_no field is set\n");
                    }
#endif
                    tl->total_chance += t->chance;
                }
            }
        } else
            LOG(llevError, "Treasure-list didn't understand: %s, line %d\n", buf, line);
    }
    close_and_delete(fp, comp);

#ifdef TREASURE_DEBUG
    /* Perform some checks on how valid the treasure data actually is.
     * verify that list transitions work (ie, the list that it is supposed
     * to transition to exists).  Also, verify that at least the name
     * or archetype is set for each treasure element.
     */
    for (previous = first_treasurelist; previous != NULL; previous = previous->next)
        check_treasurelist(previous->items, previous);
#endif
}

/**
 * Searches for the given treasurelist in the globally linked list
 * of treasurelists which has been built by load_treasures().
 *
 * Will LOG() to error if not found.
 *
 * @param name
 * treasure list to search.
 * @return
 * match, or NULL if treasurelist doesn't exist or is 'none'.
 * @ingroup page_treasure_list
 */
treasurelist *find_treasurelist(const char *name) {
    const char *tmp = find_string(name);
    treasurelist *tl;

    /* Special cases - randomitems of none is to override default.  If
     * first_treasurelist is null, it means we are on the first pass of
     * of loading archetyps, so for now, just return - second pass will
     * init these values.
     */
    if (!strcmp(name, "none") || (!first_treasurelist))
        return NULL;
    if (tmp != NULL)
        for (tl = first_treasurelist; tl != NULL; tl = tl->next)
            if (tmp == tl->name)
                return tl;
    LOG(llevError, "Couldn't find treasurelist %s\n", name);
    return NULL;
}


/**
 * Inserts generated treasure where it should go.
 *
 * @param op
 * treasure just generated.
 * @param creator
 * for which object the treasure is being generated.
 * @param flags
 * combination of @ref GT_xxx values.
 * @ingroup page_treasure_list
 */
static void put_treasure(object *op, object *creator, int flags) {
    /* Bit of a hack - spells should never be put onto the map.  The entire
     * treasure stuff is a problem - there is no clear idea of knowing
     * this is the original object, or if this is an object that should be created
     * by another object.
     */
    if (flags&GT_ENVIRONMENT && op->type != SPELL) {
        SET_FLAG(op, FLAG_OBJ_ORIGINAL);
        object_insert_in_map_at(op, creator->map, op, INS_NO_MERGE|INS_NO_WALK_ON, creator->x, creator->y);
    } else
        op = object_insert_in_ob(op, creator);
}

/**
 * if there are change_xxx commands in the treasure, we include the changes
 * in the generated object
 *
 * @param t
 * treasure.
 * @param op
 * actual generated treasure.
 * @ingroup page_treasure_list
 */
static void change_treasure(treasure *t, object *op) {
    /* CMD: change_name xxxx */
    if (t->change_arch.name) {
        FREE_AND_COPY(op->name, t->change_arch.name);
        /* not great, but better than something that is completely wrong */
        FREE_AND_COPY(op->name_pl, t->change_arch.name);
    }

    if (t->change_arch.title) {
        if (op->title)
            free_string(op->title);
        op->title = add_string(t->change_arch.title);
    }

    if (t->change_arch.slaying) {
        if (op->slaying)
            free_string(op->slaying);
        op->slaying = add_string(t->change_arch.slaying);
    }
}

/**
 * Creates all the treasures.
 *
 * @param t
 * what to generate.
 * @param op
 * for who to generate the treasure.
 * @param flag
 * combination of @ref GT_xxx values.
 * @param difficulty
 * map difficulty.
 * @param tries
 * to avoid infinite recursion.
 * @ingroup page_treasure_list
 */
static void create_all_treasures(treasure *t, object *op, int flag, int difficulty, int tries) {
    object *tmp;


    if ((int)t->chance >= 100 || (RANDOM()%100+1) < (int)t->chance) {
        if (t->name) {
            if (strcmp(t->name, "NONE") && difficulty >= t->magic)
                create_treasure(find_treasurelist(t->name), op, flag, difficulty, tries);
        } else {
            if (t->item->clone.invisible != 0 || !(flag&GT_INVISIBLE)) {
                tmp = arch_to_object(t->item);
                if (t->nrof && tmp->nrof <= 1)
                    tmp->nrof = RANDOM()%((int)t->nrof)+1;
                fix_generated_item(tmp, op, difficulty, t->magic, flag);
                change_treasure(t, tmp);
                put_treasure(tmp, op, flag);
            }
        }
        if (t->next_yes != NULL)
            create_all_treasures(t->next_yes, op, flag, difficulty, tries);
    } else
        if (t->next_no != NULL)
            create_all_treasures(t->next_no, op, flag, difficulty, tries);
    if (t->next != NULL)
        create_all_treasures(t->next, op, flag, difficulty, tries);
}

/**
 * Creates one treasure from the list.
 *
 * @param tl
 * what to generate.
 * @param op
 * for who to generate the treasure.
 * @param flag
 * combination of @ref GT_xxx values.
 * @param difficulty
 * map difficulty.
 * @param tries
 * to avoid infinite recursion.
 *
 * @note
 * can abort() if treasure has errors.
 * @ingroup page_treasure_list
 */
static void create_one_treasure(treasurelist *tl, object *op, int flag, int difficulty, int tries) {
    int value = RANDOM()%tl->total_chance;
    treasure *t;

    if (tries++ > 100) {
        LOG(llevDebug, "create_one_treasure: tries exceeded 100, returning without making treasure\n");
        return;
    }

    for (t = tl->items; t != NULL; t = t->next) {
        value -= t->chance;
        if (value < 0)
            break;
    }

    if (!t || value >= 0) {
        LOG(llevError, "create_one_treasure: got null object or not able to find treasure\n");
        abort();
        return;
    }
    if (t->name) {
        if (!strcmp(t->name, "NONE"))
            return;
        if (difficulty >= t->magic)
            create_treasure(find_treasurelist(t->name), op, flag, difficulty, tries);
        else if (t->nrof)
            create_one_treasure(tl, op, flag, difficulty, tries);
        return;
    }
    if ((t->item) && (flag&GT_ONLY_GOOD)) { /* Generate only good items, damnit !*/
        if (QUERY_FLAG(&(t->item->clone), FLAG_CURSED)
        || QUERY_FLAG(&(t->item->clone), FLAG_DAMNED)) {
            create_one_treasure(tl, op, flag, difficulty, tries+1);
            return;
        }
    }
    if ((t->item && t->item->clone.invisible != 0) || flag != GT_INVISIBLE) {
        object *tmp = arch_to_object(t->item);

        if (!tmp)
            return;
        if (t->nrof && tmp->nrof <= 1)
            tmp->nrof = RANDOM()%((int)t->nrof)+1;
        fix_generated_item(tmp, op, difficulty, t->magic, flag);
        change_treasure(t, tmp);
        put_treasure(tmp, op, flag);
    }
}

/**
 * This calls the appropriate treasure creation function.
 *
 * @param t
 * what to generate.
 * @param op
 * for who to generate the treasure.
 * @param flag
 * combination of @ref GT_xxx values.
 * @param difficulty
 * map difficulty.
 * @param tries
 * to avoid infinite recursion.
 * @ingroup page_treasure_list
 */
void create_treasure(treasurelist *t, object *op, int flag, int difficulty, int tries) {
    if (tries++ > 100) {
        LOG(llevDebug, "createtreasure: tries exceeded 100, returning without making treasure\n");
        return;
    }
    if (t->total_chance)
        create_one_treasure(t, op, flag, difficulty, tries);
    else
        create_all_treasures(t->items, op, flag, difficulty, tries);
}

/**
 * This is similar to the old generate treasure function.  However,
 * it instead takes a treasurelist.  It is really just a wrapper around
 * create_treasure().  We create a dummy object that the treasure gets
 * inserted into, and then return that treasure.
 *
 * @param t
 * treasure list to generate from.
 * @param difficulty
 * treasure difficulty.
 * @return
 * generated treasure. Can be NULL if no suitable treasure was found.
 * @ingroup page_treasure_list
 */
object *generate_treasure(treasurelist *t, int difficulty) {
    object *ob = object_new(), *tmp;

    create_treasure(t, ob, 0, difficulty, 0);

    /* Don't want to free the object we are about to return */
    tmp = ob->inv;
    if (tmp != NULL)
        object_remove(tmp);
    if (ob->inv) {
        LOG(llevError, "In generate treasure, created multiple objects.\n");
    }
    object_free(ob);
    return tmp;
}

/**
 * Calculate the appropriate level for wands staves and scrolls.
 *
 * This code presumes that op has had its spell object created (in op->inv)
 *
 * @param op
 * item we're trying to get the level of
 * @param difficulty
 * map difficulty.
 * @param retmult
 * if 1, return the multiplier, not the level, for computing value
 * @return
 * generated level, 0 if invalid item.
 */
static int level_for_item(const object *op, int difficulty, int retmult) {
    int level, mult, olevel;

    mult = 0;
    if (!op->inv) {
        LOG(llevError, "level_for_item: Object %s has no inventory!\n", op->name);
        return 0;
    }
    level = op->inv->level;

    /* Basically, we set mult to the lowest spell increase attribute that is
     * not zero - zero's mean no modification is done, so we don't want those.
     * given we want non zero results, we can't just use a few MIN's here.
     */
    mult = op->inv->dam_modifier;
    if (op->inv->range_modifier && (op->inv->range_modifier < mult || mult == 0))
        mult = op->inv->range_modifier;
    if (op->inv->duration_modifier && (op->inv->duration_modifier < mult || mult == 0))
        mult = op->inv->duration_modifier;

    if (mult == 0)
        mult = 5;

    if (retmult)
        return mult;

    olevel = mult*rndm(0, difficulty)+level;
    if (olevel > MAX_SPELLITEM_LEVEL)
        olevel = MAX_SPELLITEM_LEVEL;

    return olevel;
}

/**
 * Based upon the specified difficulty and upon the difftomagic_list array,
 * a random magical bonus is returned.  This is a new way of calculating the chance for an item to have
 * a specific magical bonus
 * The array has two arguments, the difficulty of the level, and the
 * magical bonus "wanted".
 */
static const int difftomagic_list[DIFFLEVELS][MAXMAGIC+1] = {
/*chance of magic    difficulty*/
/* +0  +1 +2 +3 +4 */
    { 94, 3, 2, 1, 0 }, /*1*/
    { 94, 3, 2, 1, 0 }, /*2*/
    { 94, 3, 2, 1, 0 }, /*3*/
    { 94, 3, 2, 1, 0 }, /*4*/
    { 94, 3, 2, 1, 0 }, /*5*/
    { 90, 4, 3, 2, 1 }, /*6*/
    { 90, 4, 3, 2, 1 }, /*7*/
    { 90, 4, 3, 2, 1 }, /*8*/
    { 90, 4, 3, 2, 1 }, /*9*/
    { 90, 4, 3, 2, 1 }, /*10*/
    { 85, 6, 4, 3, 2 }, /*11*/
    { 85, 6, 4, 3, 2 }, /*12*/
    { 85, 6, 4, 3, 2 }, /*13*/
    { 85, 6, 4, 3, 2 }, /*14*/
    { 85, 6, 4, 3, 2 }, /*15*/
    { 81, 8, 5, 4, 3 }, /*16*/
    { 81, 8, 5, 4, 3 }, /*17*/
    { 81, 8, 5, 4, 3 }, /*18*/
    { 81, 8, 5, 4, 3 }, /*19*/
    { 81, 8, 5, 4, 3 }, /*20*/
    { 75, 10, 6, 5, 4 }, /*21*/
    { 75, 10, 6, 5, 4 }, /*22*/
    { 75, 10, 6, 5, 4 }, /*23*/
    { 75, 10, 6, 5, 4 }, /*24*/
    { 75, 10, 6, 5, 4 }, /*25*/
    { 70, 12, 7, 6, 5 }, /*26*/
    { 70, 12, 7, 6, 5 }, /*27*/
    { 70, 12, 7, 6, 5 }, /*28*/
    { 70, 12, 7, 6, 5 }, /*29*/
    { 70, 12, 7, 6, 5 }, /*30*/
    { 70, 9, 8, 7, 6 }, /*31*/
    { 70, 9, 8, 7, 6 }, /*32*/
    { 70, 9, 8, 7, 6 }, /*33*/
    { 70, 9, 8, 7, 6 }, /*34*/
    { 70, 9, 8, 7, 6 }, /*35*/
    { 70, 6, 9, 8, 7 }, /*36*/
    { 70, 6, 9, 8, 7 }, /*37*/
    { 70, 6, 9, 8, 7 }, /*38*/
    { 70, 6, 9, 8, 7 }, /*39*/
    { 70, 6, 9, 8, 7 }, /*40*/
    { 70, 3, 10, 9, 8 }, /*41*/
    { 70, 3, 10, 9, 8 }, /*42*/
    { 70, 3, 10, 9, 8 }, /*43*/
    { 70, 3, 10, 9, 8 }, /*44*/
    { 70, 3, 10, 9, 8 }, /*45*/
    { 70, 2, 9, 10, 9 }, /*46*/
    { 70, 2, 9, 10, 9 }, /*47*/
    { 70, 2, 9, 10, 9 }, /*48*/
    { 70, 2, 9, 10, 9 }, /*49*/
    { 70, 2, 9, 10, 9 }, /*50*/
    { 70, 2, 7, 11, 10 }, /*51*/
    { 70, 2, 7, 11, 10 }, /*52*/
    { 70, 2, 7, 11, 10 }, /*53*/
    { 70, 2, 7, 11, 10 }, /*54*/
    { 70, 2, 7, 11, 10 }, /*55*/
    { 70, 2, 5, 12, 11 }, /*56*/
    { 70, 2, 5, 12, 11 }, /*57*/
    { 70, 2, 5, 12, 11 }, /*58*/
    { 70, 2, 5, 12, 11 }, /*59*/
    { 70, 2, 5, 12, 11 }, /*60*/
    { 70, 2, 3, 13, 12 }, /*61*/
    { 70, 2, 3, 13, 12 }, /*62*/
    { 70, 2, 3, 13, 12 }, /*63*/
    { 70, 2, 3, 13, 12 }, /*64*/
    { 70, 2, 3, 13, 12 }, /*65*/
    { 70, 2, 3, 12, 13 }, /*66*/
    { 70, 2, 3, 12, 13 }, /*67*/
    { 70, 2, 3, 12, 13 }, /*68*/
    { 70, 2, 3, 12, 13 }, /*69*/
    { 70, 2, 3, 12, 13 }, /*70*/
    { 70, 2, 3, 11, 14 }, /*71*/
    { 70, 2, 3, 11, 14 }, /*72*/
    { 70, 2, 3, 11, 14 }, /*73*/
    { 70, 2, 3, 11, 14 }, /*74*/
    { 70, 2, 3, 11, 14 }, /*75*/
    { 70, 2, 3, 10, 15 }, /*76*/
    { 70, 2, 3, 10, 15 }, /*77*/
    { 70, 2, 3, 10, 15 }, /*78*/
    { 70, 2, 3, 10, 15 }, /*79*/
    { 70, 2, 3, 10, 15 }, /*80*/
    { 70, 2, 3, 9, 16  }, /*81*/
    { 70, 2, 3, 9, 16  }, /*82*/
    { 70, 2, 3, 9, 16  }, /*83*/
    { 70, 2, 3, 9, 16  }, /*84*/
    { 70, 2, 3, 9, 16  }, /*85*/
    { 70, 2, 3, 8, 17  }, /*86*/
    { 70, 2, 3, 8, 17  }, /*87*/
    { 70, 2, 3, 8, 17  }, /*88*/
    { 70, 2, 3, 8, 17  }, /*89*/
    { 70, 2, 3, 8, 17  }, /*90*/
    { 70, 2, 3, 7, 18  }, /*91*/
    { 70, 2, 3, 7, 18  }, /*92*/
    { 70, 2, 3, 7, 18  }, /*93*/
    { 70, 2, 3, 7, 18  }, /*94*/
    { 70, 2, 3, 7, 18  }, /*95*/
    { 70, 2, 3, 6, 19  }, /*96*/
    { 70, 2, 3, 6, 19  }, /*97*/
    { 70, 2, 3, 6, 19  }, /*98*/
    { 70, 2, 3, 6, 19  }, /*99*/
    { 70, 2, 3, 6, 19  }, /*100*/
    { 70, 2, 3, 6, 19  }, /*101*/
    { 70, 2, 3, 6, 19  }, /*101*/
    { 70, 2, 3, 6, 19  }, /*102*/
    { 70, 2, 3, 6, 19  }, /*103*/
    { 70, 2, 3, 6, 19  }, /*104*/
    { 70, 2, 3, 6, 19  }, /*105*/
    { 70, 2, 3, 6, 19  }, /*106*/
    { 70, 2, 3, 6, 19  }, /*107*/
    { 70, 2, 3, 6, 19  }, /*108*/
    { 70, 2, 3, 6, 19  }, /*109*/
    { 70, 2, 3, 6, 19  }, /*110*/
    { 70, 2, 3, 6, 19  }, /*111*/
    { 70, 2, 3, 6, 19  }, /*112*/
    { 70, 2, 3, 6, 19  }, /*113*/
    { 70, 2, 3, 6, 19  }, /*114*/
    { 70, 2, 3, 6, 19  }, /*115*/
    { 70, 2, 3, 6, 19  }, /*116*/
    { 70, 2, 3, 6, 19  }, /*117*/
    { 70, 2, 3, 6, 19  }, /*118*/
    { 70, 2, 3, 6, 19  }, /*119*/
    { 70, 2, 3, 6, 19  }, /*120*/
    { 70, 2, 3, 6, 19  }, /*121*/
    { 70, 2, 3, 6, 19  }, /*122*/
    { 70, 2, 3, 6, 19  }, /*123*/
    { 70, 2, 3, 6, 19  }, /*124*/
    { 70, 2, 3, 6, 19  }, /*125*/
    { 70, 2, 3, 6, 19  }, /*126*/
    { 70, 2, 3, 6, 19  }, /*127*/
    { 70, 2, 3, 6, 19  }, /*128*/
    { 70, 2, 3, 6, 19  }, /*129*/
    { 70, 2, 3, 6, 19  }, /*130*/
    { 70, 2, 3, 6, 19  }, /*131*/
    { 70, 2, 3, 6, 19  }, /*132*/
    { 70, 2, 3, 6, 19  }, /*133*/
    { 70, 2, 3, 6, 19  }, /*134*/
    { 70, 2, 3, 6, 19  }, /*135*/
    { 70, 2, 3, 6, 19  }, /*136*/
    { 70, 2, 3, 6, 19  }, /*137*/
    { 70, 2, 3, 6, 19  }, /*138*/
    { 70, 2, 3, 6, 19  }, /*139*/
    { 70, 2, 3, 6, 19  }, /*140*/
    { 70, 2, 3, 6, 19  }, /*141*/
    { 70, 2, 3, 6, 19  }, /*142*/
    { 70, 2, 3, 6, 19  }, /*143*/
    { 70, 2, 3, 6, 19  }, /*144*/
    { 70, 2, 3, 6, 19  }, /*145*/
    { 70, 2, 3, 6, 19  }, /*146*/
    { 70, 2, 3, 6, 19  }, /*147*/
    { 70, 2, 3, 6, 19  }, /*148*/
    { 70, 2, 3, 6, 19  }, /*149*/
    { 70, 2, 3, 6, 19  }, /*150*/
    { 70, 2, 3, 6, 19  }, /*151*/
    { 70, 2, 3, 6, 19  }, /*152*/
    { 70, 2, 3, 6, 19  }, /*153*/
    { 70, 2, 3, 6, 19  }, /*154*/
    { 70, 2, 3, 6, 19  }, /*155*/
    { 70, 2, 3, 6, 19  }, /*156*/
    { 70, 2, 3, 6, 19  }, /*157*/
    { 70, 2, 3, 6, 19  }, /*158*/
    { 70, 2, 3, 6, 19  }, /*159*/
    { 70, 2, 3, 6, 19  }, /*160*/
    { 70, 2, 3, 6, 19  }, /*161*/
    { 70, 2, 3, 6, 19  }, /*162*/
    { 70, 2, 3, 6, 19  }, /*163*/
    { 70, 2, 3, 6, 19  }, /*164*/
    { 70, 2, 3, 6, 19  }, /*165*/
    { 70, 2, 3, 6, 19  }, /*166*/
    { 70, 2, 3, 6, 19  }, /*167*/
    { 70, 2, 3, 6, 19  }, /*168*/
    { 70, 2, 3, 6, 19  }, /*169*/
    { 70, 2, 3, 6, 19  }, /*170*/
    { 70, 2, 3, 6, 19  }, /*171*/
    { 70, 2, 3, 6, 19  }, /*172*/
    { 70, 2, 3, 6, 19  }, /*173*/
    { 70, 2, 3, 6, 19  }, /*174*/
    { 70, 2, 3, 6, 19  }, /*175*/
    { 70, 2, 3, 6, 19  }, /*176*/
    { 70, 2, 3, 6, 19  }, /*177*/
    { 70, 2, 3, 6, 19  }, /*178*/
    { 70, 2, 3, 6, 19  }, /*179*/
    { 70, 2, 3, 6, 19  }, /*180*/
    { 70, 2, 3, 6, 19  }, /*181*/
    { 70, 2, 3, 6, 19  }, /*182*/
    { 70, 2, 3, 6, 19  }, /*183*/
    { 70, 2, 3, 6, 19  }, /*184*/
    { 70, 2, 3, 6, 19  }, /*185*/
    { 70, 2, 3, 6, 19  }, /*186*/
    { 70, 2, 3, 6, 19  }, /*187*/
    { 70, 2, 3, 6, 19  }, /*188*/
    { 70, 2, 3, 6, 19  }, /*189*/
    { 70, 2, 3, 6, 19  }, /*190*/
    { 70, 2, 3, 6, 19  }, /*191*/
    { 70, 2, 3, 6, 19  }, /*192*/
    { 70, 2, 3, 6, 19  }, /*193*/
    { 70, 2, 3, 6, 19  }, /*194*/
    { 70, 2, 3, 6, 19  }, /*195*/
    { 70, 2, 3, 6, 19  }, /*196*/
    { 70, 2, 3, 6, 19  }, /*197*/
    { 70, 2, 3, 6, 19  }, /*198*/
    { 70, 2, 3, 6, 19  }, /*199*/
    { 70, 2, 3, 6, 19  }, /*200*/
};

/** This is used when determining the magical bonus created on specific maps.
 *
 * @param difficulty
 * difficulty.
 * @return
 * random magic bonus.
 */
static int magic_from_difficulty(int difficulty) {
    int percent, loop;

    difficulty--;
    if (difficulty < 0)
        difficulty = 0;

    if (difficulty >= DIFFLEVELS)
        difficulty = DIFFLEVELS-1;

    percent = RANDOM()%100;

    for (loop = 0; loop < (MAXMAGIC+1); ++loop) {
        percent -= difftomagic_list[difficulty][loop];
        if (percent < 0)
            break;
    }
    if (loop == (MAXMAGIC+1)) {
        LOG(llevError, "Warning, table for difficulty %d bad.\n", difficulty);
        loop = 0;
    }
    /*  LOG(llevDebug, "Chose magic %d for difficulty %d\n", loop, difficulty);*/
    return (RANDOM()%3) ? loop : -loop;
}

/**
 * Sets magical bonus in an object, and recalculates the effect on
 * the armour variable, and the effect on speed of armour.
 * This function doesn't work properly, should add use of archetypes
 * to make it truly absolute.
 *
 * @param op
 * object we're modifying.
 * @param magic
 * magic modifier.
 */
void set_abs_magic(object *op, int magic) {
    if (!magic)
        return;

    op->magic = magic;
    if (op->arch) {
        if (op->type == ARMOUR)
            ARMOUR_SPEED(op) = (ARMOUR_SPEED(&op->arch->clone)*(100+magic*10))/100;

        if (magic < 0 && !(RANDOM()%3)) /* You can't just check the weight always */
            magic = (-magic);
        op->weight = (op->arch->clone.weight*(100-magic*10))/100;
    } else {
        if (op->type == ARMOUR)
            ARMOUR_SPEED(op) = (ARMOUR_SPEED(op)*(100+magic*10))/100;
        if (magic < 0 && !(RANDOM()%3)) /* You can't just check the weight always */
            magic = (-magic);
        op->weight = (op->weight*(100-magic*10))/100;
    }
}

/**
 * Sets a random magical bonus in the given object based upon
 * the given difficulty, and the given max possible bonus.
 *
 * Item will be cursed if magic is megative.
 *
 * @param difficulty
 * difficulty we want the item to be.
 * @param op
 * the object.
 * @param max_magic
 * what should be the maximum magic of the item.
 * @param flags
 * combination of @ref GT_xxx flags.
 */
static void set_magic(int difficulty, object *op, int max_magic, int flags) {
    int i;

    i = magic_from_difficulty(difficulty);
    if ((flags&GT_ONLY_GOOD) && i < 0)
        i = -i;
    if (i > max_magic)
        i = max_magic;
    set_abs_magic(op, i);
    if (i < 0)
        SET_FLAG(op, FLAG_CURSED);
}

/**
 * Randomly adds one magical ability to the given object.
 *
 * Modified for Partial Resistance in many ways:
 * 1) Since rings can have multiple bonuses, if the same bonus
 *  is rolled again, increase it - the bonuses now stack with
 *  other bonuses previously rolled and ones the item might natively have.
 * 2) Add code to deal with new PR method.
 *
 * Changes the item's value.
 *
 * @param op
 * ring or amulet to change.
 * @param bonus
 * bonus to add to item.
 */
static void set_ring_bonus(object *op, int bonus) {
    int r = RANDOM()%(bonus > 0 ? 25 : 11);

    if (op->type == AMULET) {
        if (!(RANDOM()%21))
            r = 20+RANDOM()%2;
        else {
            if (RANDOM()&2)
                r = 10;
            else
                r = 11+RANDOM()%9;
        }
    }

    switch (r) {
        /* Redone by MSW 2000-11-26 to have much less code.  Also,
         * bonuses and penalties will stack and add to existing values.
         * of the item.
         */
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        set_attr_value(&op->stats, r, (signed char)(bonus+get_attr_value(&op->stats, r)));
        break;

    case 7:
        op->stats.dam += bonus;
        break;

    case 8:
        op->stats.wc += bonus;
        break;

    case 9:
        op->stats.food += bonus; /* hunger/sustenance */
        break;

    case 10:
        op->stats.ac += bonus;
        break;

        /* Item that gives protections/vulnerabilities */
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19: {
            int b = 5+FABS(bonus), val, resist = RANDOM()%num_resist_table;

            /* Roughly generate a bonus between 100 and 35 (depending on the bonus) */
            val = 10+RANDOM()%b+RANDOM()%b+RANDOM()%b+RANDOM()%b;

            /* Cursed items need to have higher negative values to equal out with
             * positive values for how protections work out.  Put another
             * little random element in since that they don't always end up with
             * even values.
             */
            if (bonus < 0)
                val = 2*-val-RANDOM()%b;
            if (val > 35)
                val = 35; /* Upper limit */
            b = 0;
            while (op->resist[resist_table[resist]] != 0 && b < 4) {
                resist = RANDOM()%num_resist_table;
            }
            if (b == 4)
                return; /* Not able to find a free resistance */
            op->resist[resist_table[resist]] = val;
            /* We should probably do something more clever here to adjust value
             * based on how good a resistance we gave.
             */
            break;
        }

    case 20:
        if (op->type == AMULET) {
            SET_FLAG(op, FLAG_REFL_SPELL);
            op->value *= 11;
        } else {
            op->stats.hp = 1; /* regenerate hit points */
            op->value *= 4;
        }
        break;

    case 21:
        if (op->type == AMULET) {
            SET_FLAG(op, FLAG_REFL_MISSILE);
            op->value *= 9;
        } else {
            op->stats.sp = 1; /* regenerate spell points */
            op->value *= 3;
        }
        break;

    case 22:
        op->stats.exp += bonus; /* Speed! */
        op->value = (op->value*2)/3;
        break;
    }
    if (bonus > 0)
        op->value *= 2*bonus;
    else
        op->value = -(op->value*2*bonus)/3;
}

/**
 * get_magic(diff) will return a random number between 0 and 4.
 * diff can be any value above 2.  The higher the diff-variable, the
 * higher is the chance of returning a low number.
 * It is only used in fix_generated_treasure() to set bonuses on
 * rings and amulets.
 * Another scheme is used to calculate the magic of weapons and armours.
 */
static int get_magic(int diff) {
    int i;

    if (diff < 3)
        diff = 3;
    for (i = 0; i < 4; i++)
        if (RANDOM()%diff)
            return i;
    return 4;
}

#define DICE2 (get_magic(2) == 2 ? 2 : 1)
#define DICESPELL (RANDOM()%3+RANDOM()%3+RANDOM()%3+RANDOM()%3+RANDOM()%3)

/**
 * fix_generated_item():  This is called after an item is generated, in
 * order to set it up right.  This produced magical bonuses, puts spells
 * into scrolls/books/wands, makes it unidentified, hides the value, etc.
 *
 * 4/28/96 added creator object from which op may now inherit properties based on
 * op->type. Right now, which stuff the creator passes on is object type
 * dependant. I know this is a spagetti manuever, but is there a cleaner
 * way to do this? b.t.
 *
 * @param op
 * object to fix.
 * @param creator
 * for who op was created. Can be NULL.
 * @param difficulty
 * difficulty level.
 * @param max_magic
 * maximum magic for the item.
 * @param flags
 * @li ::GT_ENVIRONMENT: if not set, calls fix_flesh_item().
 * @li ::GT_STARTEQUIP: Sets ::FLAG_STARTEQIUP on item if appropriate, or clears the item's
 *  value.
 * @li ::GT_MINIMAL: Does minimal processing on the object - just enough to make it
 * a working object - don't change magic, value, etc, but set it material
 *  type as appropriate, for objects that need spell objects, set those, etc
 */
void fix_generated_item(object *op, object *creator, int difficulty, int max_magic, int flags) {
    int was_magic = op->magic, num_enchantments = 0, save_item_power;

    if (!creator || creator->type == op->type)
        creator = op; /* safety & to prevent polymorphed objects giving attributes */

    /* If we make an artifact, this information will be destroyed */
    save_item_power = op->item_power;
    op->item_power = 0;

    if (op->randomitems && op->type != SPELL) {
        create_treasure(op->randomitems, op, flags, difficulty, 0);
        if (!op->inv)
            LOG(llevDebug, "fix_generated_item: Unable to generate treasure for %s\n", op->name);
        /* So the treasure doesn't get created again */
        op->randomitems = NULL;
    }

    if (difficulty < 1)
        difficulty = 1;
    if (!(flags&GT_MINIMAL)) {
        if (op->arch == crown_arch) {
            set_magic(difficulty > 25 ? 30 : difficulty+5, op, max_magic, flags);
            num_enchantments = calc_item_power(op, 1);
            generate_artifact(op, difficulty);
        } else {
            if (!op->magic && max_magic)
                set_magic(difficulty, op, max_magic, flags);
            num_enchantments = calc_item_power(op, 1);
            if ((!was_magic && !(RANDOM()%CHANCE_FOR_ARTIFACT))
            || op->type == ROD
            || difficulty >= 999)
                generate_artifact(op, difficulty);
        }

        /* Object was made an artifact.  Calculate its item_power rating.
         * the item_power in the object is what the artifact adds.
         */
        if (op->title) {
            /* if save_item_power is set, then most likely we started with an
             * artifact and have added new abilities to it - this is rare, but
             * but I have seen things like 'strange rings of fire'.  So just
             * figure out the power from the base power plus what this one adds.
             * Note that since item_power is not quite linear, this actually
             * ends up being somewhat of a bonus.
             */
            if (save_item_power)
                op->item_power = save_item_power+get_power_from_ench(op->item_power);
            else
                op->item_power += get_power_from_ench(num_enchantments);
        } else if (save_item_power) {
            /* restore the item_power field to the object if we haven't changed
             * it. we don't care about num_enchantments - that will basically
             * just have calculated some value from the base attributes of the
             * archetype.
             */
            op->item_power = save_item_power;
        } else {
            /* item_power was zero. This is suspicious, as it may be because it
             * was never previously calculated. Let's compute a value and see if
             * it is non-zero. If it indeed is, then assign it as the new
             * item_power value.
             * - gros, 21th of July 2006.
             */
            op->item_power = calc_item_power(op, 0);
            save_item_power = op->item_power; /* Just in case it would get used
                                               * again below */
        }
    } else {
        /* If flag is GT_MINIMAL, we want to restore item power */
        op->item_power = save_item_power;
    }

    /* materialtype modifications.  Note we allow this on artifacts. */

    set_materialname(op, difficulty, NULL);

    if (flags&GT_MINIMAL) {
        if (op->type == POTION)
            /* Handle healing and magic power potions */
            if (op->stats.sp && !op->randomitems) {
                object *tmp;

                tmp = create_archetype(spell_mapping[op->stats.sp]);
                object_insert_in_ob(tmp, op);
                op->stats.sp = 0;
            }
    } else if (!op->title) { /* Only modify object if not special */
        switch (op->type) {
        case WEAPON:
        case ARMOUR:
        case SHIELD:
        case HELMET:
        case CLOAK:
            if (QUERY_FLAG(op, FLAG_CURSED) && !(RANDOM()%4))
                set_ring_bonus(op, -DICE2);
            break;

        case BRACERS:
            if (!(RANDOM()%(QUERY_FLAG(op, FLAG_CURSED) ? 5 : 20))) {
                set_ring_bonus(op, QUERY_FLAG(op, FLAG_CURSED) ? -DICE2 : DICE2);
                if (!QUERY_FLAG(op, FLAG_CURSED))
                    op->value *= 3;
            }
            break;

        case POTION: {
                int too_many_tries = 0, is_special = 0;

                /* Handle healing and magic power potions */
                if (op->stats.sp && !op->randomitems) {
                    object *tmp;

                    tmp = create_archetype(spell_mapping[op->stats.sp]);
                    object_insert_in_ob(tmp, op);
                    op->stats.sp = 0;
                }

                while (!(is_special = special_potion(op)) && !op->inv) {
                    generate_artifact(op, difficulty);
                    if (too_many_tries++ > 10)
                        break;
                }
                /* don't want to change value for healing/magic power potions,
                 * since the value set on those is already correct.
                 */
                if (op->inv && op->randomitems) {
                    /* value multiplier is same as for scrolls */
                    op->value = (op->value*op->inv->value);
                    op->level = op->inv->level/2+RANDOM()%difficulty+RANDOM()%difficulty;
                } else {
                    FREE_AND_COPY(op->name, "potion");
                    FREE_AND_COPY(op->name_pl, "potions");
                }
                if (!(flags&GT_ONLY_GOOD) && RANDOM()%2)
                    SET_FLAG(op, FLAG_CURSED);
                break;
            }

        case AMULET:
                if (op->arch == amulet_arch)
                    op->value *= 5; /* Since it's not just decoration */
        case RING:
            if (op->arch == NULL) {
                object_remove(op);
                object_free(op);
                op = NULL;
                break;
            }
            if (op->arch != ring_arch && op->arch != amulet_arch)
                /* It's a special artifact!*/
                break;

            if (!(flags&GT_ONLY_GOOD) && !(RANDOM()%3))
                SET_FLAG(op, FLAG_CURSED);
            set_ring_bonus(op, QUERY_FLAG(op, FLAG_CURSED) ? -DICE2 : DICE2);
            if (op->type != RING) /* Amulets have only one ability */
                break;
            if (!(RANDOM()%4)) {
                int d = (RANDOM()%2 || QUERY_FLAG(op, FLAG_CURSED)) ? -DICE2 : DICE2;
                if (d > 0)
                    op->value *= 3;
                set_ring_bonus(op, d);
                if (!(RANDOM()%4)) {
                    int d = (RANDOM()%3 || QUERY_FLAG(op, FLAG_CURSED)) ? -DICE2 : DICE2;
                    if (d > 0)
                        op->value *= 5;
                    set_ring_bonus(op, d);
                }
            }
            if (GET_ANIM_ID(op))
                SET_ANIMATION(op, RANDOM()%((int)NUM_ANIMATIONS(op)));
            break;

        case BOOK:
            /* Is it an empty book?, if yes lets make a special
             * msg for it, and tailor its properties based on the
             * creator and/or map level we found it on.
             */
            if (!op->msg && RANDOM()%10) {
                /* set the book level properly */
                if (creator->level == 0 || QUERY_FLAG(creator, FLAG_ALIVE)) {
                    if (op->map && op->map->difficulty)
                        op->level = RANDOM()%(op->map->difficulty)+RANDOM()%10+1;
                    else
                        op->level = RANDOM()%20+1;
                } else
                    op->level = RANDOM()%creator->level;

                tailor_readable_ob(op, -1);
                /* books w/ info are worth more! */
                op->value *= ((op->level > 10 ? op->level : (op->level+1)/2)*((strlen(op->msg)/250)+1));
                /* creator related stuff */

                /* for library, chained books.  Note that some monsters have
                 * no_pick set - we don't want to set no pick in that case.
                 */
                if (QUERY_FLAG(creator, FLAG_NO_PICK)
                && !QUERY_FLAG(creator, FLAG_MONSTER))
                    SET_FLAG(op, FLAG_NO_PICK);
                if (creator->slaying && !op->slaying) /* for check_inv floors */
                        op->slaying = add_string(creator->slaying);

                /* add exp so reading it gives xp (once)*/
                op->stats.exp = op->value > 10000 ? op->value/5 : op->value/10;
            }
            break;

        case SPELLBOOK:
            op->value = op->value*op->inv->value;
            /* add exp so learning gives xp */
            op->level = op->inv->level;
            op->stats.exp = op->value;
            /* some more fun */
            if (!(flags&GT_ONLY_GOOD) && rndm(1, 100) <= 5) {
                if (rndm(1, 6) <= 1)
                    SET_FLAG(op, FLAG_DAMNED);
                else
                    SET_FLAG(op, FLAG_CURSED);
            } else if (rndm(1, 100) <= 1) {
                SET_FLAG(op, FLAG_BLESSED);
            }
            break;

        case WAND:
            /* nrof in the treasure list is number of charges,
             * not number of wands.  So copy that into food (charges),
             * and reset nrof.
             */
            op->stats.food = op->inv->nrof;
            op->nrof = 1;
            /* If the spell changes by level, choose a random level
             * for it, and adjust price.  If the spell doesn't
             * change by level, just set the wand to the level of
             * the spell, and value calculation is simpler.
             */
            if (op->inv->duration_modifier
            || op->inv->dam_modifier
            || op->inv->range_modifier) {
                op->level = level_for_item(op, difficulty, 0);
                op->value = op->value*op->inv->value*(op->level+50)/(op->inv->level+50);
            } else {
                op->level = op->inv->level;
                op->value = op->value*op->inv->value;
            }
            break;

        case ROD:
            op->level = level_for_item(op, difficulty, 0);
            rod_adjust(op);
            break;

        case SCROLL:
            op->level = level_for_item(op, difficulty, 0);
            op->value = op->value*op->inv->value*(op->level+50)/(op->inv->level+50);
            /* add exp so reading them properly gives xp */
            op->stats.exp = op->value/5;
            op->nrof = op->inv->nrof;
            /* some more fun */
            if (!(flags&GT_ONLY_GOOD) && rndm(1, 100) <= 20) {
                if (rndm(1, 6) <= 1)
                    SET_FLAG(op, FLAG_DAMNED);
                else
                    SET_FLAG(op, FLAG_CURSED);
            } else if (rndm(1, 100) <= 2) {
                SET_FLAG(op, FLAG_BLESSED);
            }
            break;

        case RUNE:
            trap_adjust(op, difficulty);
            break;

        case TRAP:
            trap_adjust(op, difficulty);
            break;
        } /* switch type */
    }
    if (flags&GT_STARTEQUIP) {
        if (op->nrof < 2
        && op->type != CONTAINER
        && op->type != MONEY
        && !QUERY_FLAG(op, FLAG_IS_THROWN))
            SET_FLAG(op, FLAG_STARTEQUIP);
        else if (op->type != MONEY)
            op->value = 0;
    }

    if (!(flags&GT_ENVIRONMENT))
        fix_flesh_item(op, creator);
}

/*
 *
 *
 * CODE DEALING WITH ARTIFACTS STARTS HERE
 *
 *
 */

/**
 * Allocate and return the pointer to an empty artifactlist structure.
 *
 * @return
 * new structure blanked, never NULL.
 *
 * @note
 * will fatal() if memory error.
 */
static artifactlist *get_empty_artifactlist(void) {
    artifactlist *tl = (artifactlist *)malloc(sizeof(artifactlist));
    if (tl == NULL)
        fatal(OUT_OF_MEMORY);
    tl->next = NULL;
    tl->items = NULL;
    tl->total_chance = 0;
    return tl;
}

/**
 * Allocate and return the pointer to an empty artifact structure.
 *
 * @return
 * new structure blanked, never NULL.
 *
 * @note
 * will fatal() if memory error.
 */
static artifact *get_empty_artifact(void) {
    artifact *t = (artifact *)malloc(sizeof(artifact));
    if (t == NULL)
        fatal(OUT_OF_MEMORY);
    t->item = NULL;
    t->next = NULL;
    t->chance = 0;
    t->difficulty = 0;
    t->allowed = NULL;
    return t;
}

/**
 * Searches the artifact lists and returns one that has the same type
 * of objects on it.
 *
 * @return
 * NULL if no suitable list found.
 */
artifactlist *find_artifactlist(int type) {
    artifactlist *al;

    for (al = first_artifactlist; al != NULL; al = al->next)
        if (al->type == type)
            return al;
    return NULL;
}

/**
 * For debugging purposes.  Dumps all tables.
 *
 * @todo
 * use LOG() instead of fprintf.
 */
void dump_artifacts(void) {
    artifactlist *al;
    artifact *art;
    linked_char *next;

    fprintf(logfile, "\n");
    for (al = first_artifactlist; al != NULL; al = al->next) {
        fprintf(logfile, "Artifact has type %d, total_chance=%d\n", al->type, al->total_chance);
        for (art = al->items; art != NULL; art = art->next) {
            fprintf(logfile, "Artifact %-30s Difficulty %3d Chance %5d\n", art->item->name, art->difficulty, art->chance);
            if (art->allowed != NULL) {
                fprintf(logfile, "\tAllowed combinations:");
                for (next = art->allowed; next != NULL; next = next->next)
                    fprintf(logfile, "%s,", next->name);
                fprintf(logfile, "\n");
            }
        }
    }
    fprintf(logfile, "\n");
}

/**
 * For debugging purposes.  Dumps all treasures recursively (see below).
 */
static void dump_monster_treasure_rec(const char *name, treasure *t, int depth) {
    treasurelist *tl;
    int i;

    if (depth > 100)
        return;

    while (t != NULL) {
        if (t->name != NULL) {
            for (i = 0; i < depth; i++)
                fprintf(logfile, "  ");
            fprintf(logfile, "{   (list: %s)\n", t->name);
            tl = find_treasurelist(t->name);
            dump_monster_treasure_rec(name, tl->items, depth+2);
            for (i = 0; i < depth; i++)
                fprintf(logfile, "  ");
            fprintf(logfile, "}   (end of list: %s)\n", t->name);
        } else {
            for (i = 0; i < depth; i++)
                fprintf(logfile, "  ");
            if (t->item->clone.type == FLESH)
                fprintf(logfile, "%s's %s\n", name, t->item->clone.name);
            else
                fprintf(logfile, "%s\n", t->item->clone.name);
        }
        if (t->next_yes != NULL) {
            for (i = 0; i < depth; i++)
                fprintf(logfile, "  ");
            fprintf(logfile, " (if yes)\n");
            dump_monster_treasure_rec(name, t->next_yes, depth+1);
        }
        if (t->next_no != NULL) {
            for (i = 0; i < depth; i++)
                fprintf(logfile, "  ");
            fprintf(logfile, " (if no)\n");
            dump_monster_treasure_rec(name, t->next_no, depth+1);
        }
        t = t->next;
    }
}

/**
 * For debugging purposes.  Dumps all treasures for a given monster.
 * Created originally by Raphael Quinet for debugging the alchemy code.
 */
void dump_monster_treasure(const char *name) {
    archetype *at;
    int found;

    found = 0;
    fprintf(logfile, "\n");
    for (at = first_archetype; at != NULL; at = at->next)
        if (!strcasecmp(at->clone.name, name) && at->clone.title == NULL) {
            fprintf(logfile, "treasures for %s (arch: %s)\n", at->clone.name, at->name);
            if (at->clone.randomitems != NULL)
                dump_monster_treasure_rec(at->clone.name, at->clone.randomitems->items, 1);
            else
                fprintf(logfile, "(nothing)\n");
            fprintf(logfile, "\n");
            found++;
        }
    if (found == 0)
        fprintf(logfile, "No objects have the name %s!\n\n", name);
}

/**
 * Builds up the lists of artifacts from the file in the libdir.
 * Can be called multiple times without ill effects.
 */
void init_artifacts(void) {
    static int has_been_inited = 0;
    FILE *fp;
    char filename[MAX_BUF], buf[HUGE_BUF], *cp, *next;
    artifact *art = NULL;
    linked_char *tmp;
    int value, comp;
    artifactlist *al;
    archetype dummy_archetype;

    memset(&dummy_archetype, 0, sizeof(archetype));

    if (has_been_inited)
        return;
    else
        has_been_inited = 1;

    artifact_init = 1;

    snprintf(filename, sizeof(filename), "%s/artifacts", settings.datadir);
    LOG(llevDebug, "Reading artifacts from %s...\n", filename);
    if ((fp = open_and_uncompress(filename, 0, &comp)) == NULL) {
        LOG(llevError, "Can't open %s.\n", filename);
        return;
    }

    while (fgets(buf, HUGE_BUF, fp) != NULL) {
        if (*buf == '#')
            continue;
        if ((cp = strchr(buf, '\n')) != NULL)
            *cp = '\0';
        cp = buf;
        while (*cp == ' ') /* Skip blanks */
            cp++;
        if (*cp == '\0')
            continue;

        if (!strncmp(cp, "Allowed", 7)) {
            if (art == NULL) {
                art = get_empty_artifact();
                nrofartifacts++;
            }

            cp = strchr(cp, ' ')+1;
            while (*(cp+strlen(cp)-1) == ' ')
                cp[strlen(cp)-1] = '\0';

            if (!strcmp(cp, "all"))
                continue;

            do {
                while (*cp == ' ')
                    cp++;
                nrofallowedstr++;
                if ((next = strchr(cp, ',')) != NULL)
                    *(next++) = '\0';
                tmp = (linked_char *)malloc(sizeof(linked_char));
                tmp->name = add_string(cp);
                tmp->next = art->allowed;
                art->allowed = tmp;
            } while ((cp = next) != NULL);
        } else if (sscanf(cp, "chance %d", &value))
            art->chance = (uint16)value;
        else if (sscanf(cp, "difficulty %d", &value))
            art->difficulty = (uint8)value;
        else if (!strncmp(cp, "Object", 6)) {
            art->item = (object *)calloc(1, sizeof(object));
            if (art->item == NULL) {
                LOG(llevError, "init_artifacts: memory allocation failure.\n");
                abort();
            }
            object_reset(art->item);
            art->item->arch = &dummy_archetype;
            if (!load_object(fp, art->item, LO_LINEMODE, 0))
                LOG(llevError, "Init_Artifacts: Could not load object.\n");
            art->item->arch = NULL;
            art->item->name = add_string((strchr(cp, ' ')+1));
            al = find_artifactlist(art->item->type);
            if (al == NULL) {
                al = get_empty_artifactlist();
                al->type = art->item->type;
                al->next = first_artifactlist;
                first_artifactlist = al;
            }
            art->next = al->items;
            al->items = art;
            art = NULL;
        } else
            LOG(llevError, "Unknown input in artifact file: %s\n", buf);
    }

    close_and_delete(fp, comp);

    for (al = first_artifactlist; al != NULL; al = al->next) {
        for (art = al->items; art != NULL; art = art->next) {
            if (!art->chance)
                LOG(llevError, "Warning: artifact with no chance: %s\n", art->item->name);
            else
                al->total_chance += art->chance;
        }
#if 0
        LOG(llevDebug, "Artifact list type %d has %d total chance\n", al->type, al->total_chance);
#endif
    }

    LOG(llevDebug, "done artifacts.\n");
    artifact_init = 0;
}


/**
 * Used in artifact generation.  The bonuses of the first object
 * is modified by the bonuses of the second object.
 */
void add_abilities(object *op, object *change) {
    int i, tmp;

    if (change->face != blank_face) {
#ifdef TREASURE_VERBOSE
        LOG(llevDebug, "FACE: %d\n", change->face->number);
#endif
        op->face = change->face;
    }
    if (change->animation_id != 0) {
        op->animation_id = change->animation_id;
        SET_FLAG(op, FLAG_ANIMATE);
        animate_object(op, op->facing);
    }

    for (i = 0; i < NUM_STATS; i++)
        change_attr_value(&(op->stats), i, get_attr_value(&(change->stats), i));

    op->attacktype |= change->attacktype;
    op->path_attuned |= change->path_attuned;
    op->path_repelled |= change->path_repelled;
    op->path_denied |= change->path_denied;
    op->move_type |= change->move_type;
    op->stats.luck += change->stats.luck;

    if (QUERY_FLAG(change, FLAG_CURSED))
        SET_FLAG(op, FLAG_CURSED);
    if (QUERY_FLAG(change, FLAG_DAMNED))
        SET_FLAG(op, FLAG_DAMNED);
    if ((QUERY_FLAG(change, FLAG_CURSED) || QUERY_FLAG(change, FLAG_DAMNED))
    && op->magic > 0)
        set_abs_magic(op, -op->magic);

    if (QUERY_FLAG(change, FLAG_LIFESAVE))
        SET_FLAG(op, FLAG_LIFESAVE);
    if (QUERY_FLAG(change, FLAG_REFL_SPELL))
        SET_FLAG(op, FLAG_REFL_SPELL);
    if (QUERY_FLAG(change, FLAG_STEALTH))
        SET_FLAG(op, FLAG_STEALTH);
    if (QUERY_FLAG(change, FLAG_XRAYS))
        SET_FLAG(op, FLAG_XRAYS);
    if (QUERY_FLAG(change, FLAG_BLIND))
        SET_FLAG(op, FLAG_BLIND);
    if (QUERY_FLAG(change, FLAG_SEE_IN_DARK))
        SET_FLAG(op, FLAG_SEE_IN_DARK);
    if (QUERY_FLAG(change, FLAG_REFL_MISSILE))
        SET_FLAG(op, FLAG_REFL_MISSILE);
    if (QUERY_FLAG(change, FLAG_MAKE_INVIS))
        SET_FLAG(op, FLAG_MAKE_INVIS);

    if (QUERY_FLAG(change, FLAG_STAND_STILL)) {
        CLEAR_FLAG(op, FLAG_ANIMATE);
        /* so artifacts will join */
        if (!QUERY_FLAG(op, FLAG_ALIVE))
            op->speed = 0.0;
        object_update_speed(op);
    }
    if (change->nrof)
        op->nrof = RANDOM()%((int)change->nrof)+1;
    op->stats.exp += change->stats.exp; /* Speed modifier */
    op->stats.wc += change->stats.wc;
    op->stats.ac += change->stats.ac;

    if (change->other_arch) {
        /* Basically, for horns & potions, the other_arch field is the spell
         * to cast.  So convert that to into a spell and put it into
         * this object.
         */
        if (op->type == ROD || op->type == POTION) {
            object *tmp_obj;

            /* Remove any spells this object currently has in it */
            while (op->inv) {
                tmp_obj = op->inv;
                object_remove(tmp_obj);
                object_free(tmp_obj);
            }
            tmp_obj = arch_to_object(change->other_arch);
            object_insert_in_ob(tmp_obj, op);
        }
        /* No harm setting this for potions/horns */
        op->other_arch = change->other_arch;
    }

    if (change->stats.hp < 0)
        op->stats.hp = -change->stats.hp;
    else
        op->stats.hp += change->stats.hp;
    if (change->stats.maxhp < 0)
        op->stats.maxhp = -change->stats.maxhp;
    else
        op->stats.maxhp += change->stats.maxhp;
    if (change->stats.sp < 0)
        op->stats.sp = -change->stats.sp;
    else
        op->stats.sp += change->stats.sp;
    if (change->stats.maxsp < 0)
        op->stats.maxsp = -change->stats.maxsp;
    else
        op->stats.maxsp += change->stats.maxsp;
    if (change->stats.food < 0)
        op->stats.food = -(change->stats.food);
    else
        op->stats.food += change->stats.food;
    if (change->level < 0)
        op->level = -(change->level);
    else
        op->level += change->level;

    if (change->gen_sp_armour < 0)
        op->gen_sp_armour = -(change->gen_sp_armour);
    else
        op->gen_sp_armour = (op->gen_sp_armour*(change->gen_sp_armour))/100;

    op->item_power = change->item_power;

    for (i = 0; i < NROFATTACKS; i++) {
        if (change->resist[i]) {
            op->resist[i] += change->resist[i];
        }
    }
    if (change->stats.dam) {
        if (change->stats.dam < 0)
            op->stats.dam = (-change->stats.dam);
        else if (op->stats.dam) {
            tmp = (signed char)(((int)op->stats.dam*(int)change->stats.dam)/10);
            if (tmp == op->stats.dam) {
                if (change->stats.dam < 10)
                    op->stats.dam--;
                else
                    op->stats.dam++;
            } else
                op->stats.dam = tmp;
        }
    }
    if (change->weight) {
        if (change->weight < 0)
            op->weight = (-change->weight);
        else
            op->weight = (op->weight*(change->weight))/100;
    }
    if (change->last_sp) {
        if (change->last_sp < 0)
            op->last_sp = (-change->last_sp);
        else
            op->last_sp = (signed char)(((int)op->last_sp*(int)change->last_sp)/(int)100);
    }
    if (change->gen_sp_armour) {
        if (change->gen_sp_armour < 0)
            op->gen_sp_armour = (-change->gen_sp_armour);
        else
            op->gen_sp_armour = (signed char)(((int)op->gen_sp_armour*((int)change->gen_sp_armour))/(int)100);
    }
    op->value *= change->value;

    if (change->material)
        op->material = change->material;

    if (change->materialname) {
        if (op->materialname)
            free_string(op->materialname);
        op->materialname = add_refcount(change->materialname);
    }

    if (change->slaying) {
        if (op->slaying)
            free_string(op->slaying);
        op->slaying = add_refcount(change->slaying);
    }
    if (change->race) {
        if (op->race)
            free_string(op->race);
        op->race = add_refcount(change->race);
    }
    if (change->msg)
        object_set_msg(op, change->msg);

    if (change->inv) {
        object *copy;

        FOR_INV_PREPARE(change, inv) {
            copy = object_new();
            object_copy(inv, copy);
            object_insert_in_ob(copy, op);
        } FOR_INV_FINISH();
    }
}

/**
 * Checks if op can be combined with art.
 */
int legal_artifact_combination(object *op, artifact *art) {
    int neg, success = 0;
    linked_char *tmp;
    const char *name;

    if (art->allowed == (linked_char *)NULL)
        return 1; /* Ie, "all" */
    for (tmp = art->allowed; tmp; tmp = tmp->next) {
#ifdef TREASURE_VERBOSE
        LOG(llevDebug, "legal_art: %s\n", tmp->name);
#endif
        if (*tmp->name == '!')
            name = tmp->name+1,
            neg = 1;
        else
            name = tmp->name,
            neg = 0;

        /* If we match name, then return the opposite of 'neg' */
        if (!strcmp(name, op->name) || (op->arch && !strcmp(name, op->arch->name)))
            return !neg;

        /* Set success as true, since if the match was an inverse, it means
         * everything is allowed except what we match
         */
        else if (neg)
            success = 1;
    }
    return success;
}

/**
 * Fixes the given object, giving it the abilities and titles
 * it should have due to the second artifact-template.
 */
void give_artifact_abilities(object *op, object *artifact) {
    char new_name[MAX_BUF];

    snprintf(new_name, sizeof(new_name), "of %s", artifact->name);
    if (op->title)
        free_string(op->title);
    op->title = add_string(new_name);
    add_abilities(op, artifact); /* Give out the bonuses */

    return;
}

/** Give 1 re-roll attempt per artifact */
#define ARTIFACT_TRIES 2

/**
 * Decides randomly which artifact the object should be
 * turned into.  Makes sure that the item can become that
 * artifact (means magic, difficulty, and Allowed fields properly).
 * Then calls give_artifact_abilities in order to actually create
 * the artifact.
 */
void generate_artifact(object *op, int difficulty) {
    artifactlist *al;
    artifact *art;
    int i;

    al = find_artifactlist(op->type);

    if (al == NULL) {
        return;
    }

    for (i = 0; i < ARTIFACT_TRIES; i++) {
        int roll = RANDOM()%al->total_chance;

        for (art = al->items; art != NULL; art = art->next) {
            roll -= art->chance;
            if (roll < 0)
                break;
        }

        if (art == NULL || roll >= 0) {
            LOG(llevError, "Got null entry and non zero roll in generate_artifact, type %d\n", op->type);
            return;
        }
        if (!strcmp(art->item->name, "NONE"))
            return;
        if (FABS(op->magic) < art->item->magic)
            continue; /* Not magic enough to be this item */

        /* Map difficulty not high enough */
        if (difficulty < art->difficulty)
            continue;

        if (!legal_artifact_combination(op, art)) {
#ifdef TREASURE_VERBOSE
            LOG(llevDebug, "%s of %s was not a legal combination.\n", op->name, art->item->name);
#endif
            continue;
        }
        give_artifact_abilities(op, art->item);
        return;
    }
}

/**
 * fix_flesh_item() - objects of type FLESH are similar to type
 * FOOD, except they inherit properties (name, food value, etc).
 * based on the original owner (or 'donor' if you like). -b.t.
 */
static void fix_flesh_item(object *item, object *donor) {
    char tmpbuf[MAX_BUF];
    int i;

    if (item->type == FLESH && donor && QUERY_FLAG(donor, FLAG_MONSTER)) {
        /* change the name */
        snprintf(tmpbuf, sizeof(tmpbuf), "%s's %s", donor->name, item->name);
        FREE_AND_COPY(item->name, tmpbuf);
        snprintf(tmpbuf, sizeof(tmpbuf), "%s's %s", donor->name, item->name_pl);
        FREE_AND_COPY(item->name_pl, tmpbuf);

        /* store original arch in other_arch */
        if (!item->other_arch) {
            if (!donor->arch->reference_count) {
                item->other_arch = donor->arch;
            } else {
                /* If dealing with custom monsters, other_arch still needs to
                 * point back to the original.  Otherwise what happens
                 * is that other_arch points at the custom archetype, but
                 * that can be freed.  Reference count doesn't work because
                 * the loader will not be able to resolve the other_arch at
                 * load time (server may has restarted, etc.)
                 */
                archetype *original = find_archetype(donor->arch->name);

                if (original)
                    item->other_arch = original;
                else {
                    LOG(llevError, "could not find original archetype %s for custom monster!\n", donor->arch->name);
                    abort();
                }
            }
        }

        /* weight is FLESH weight/100 * donor */
        if ((item->weight = (signed long)(((double)item->weight/(double)100.0)*(double)donor->weight)) == 0)
            item->weight = 1;

        /* value is multiplied by level of donor */
        item->value *= isqrt(donor->level*2);

        /* food value */
        item->stats.food += (donor->stats.hp/100)+donor->stats.Con;

        /* flesh items inherit some abilities of donor, but not full effect. */
        for (i = 0; i < NROFATTACKS; i++)
            item->resist[i] = donor->resist[i]/2;

        /* item inherits donor's level and exp (important for dragons) */
        item->level = donor->level;
        item->stats.exp = donor->stats.exp;

        /* if donor has some attacktypes, the flesh is poisonous */
        if (donor->attacktype&AT_POISON)
            item->type = POISON;
        if (donor->attacktype&AT_ACID)
            item->stats.hp = -1*item->stats.food;
        SET_FLAG(item, FLAG_NO_STEAL);
    }
}

/**
 * Check if object is a special potion.
 *
 * @param op
 * object to check.
 * @return
 * 1 if op is a special potion (resistance, attribute, ...), 0 else.
 */
static int special_potion(object *op) {
    int i;

    if (op->attacktype)
        return 1;

    if (op->stats.Str
    || op->stats.Dex
    || op->stats.Con
    || op->stats.Pow
    || op->stats.Wis
    || op->stats.Int
    || op->stats.Cha)
        return 1;

    for (i = 0; i < NROFATTACKS; i++)
        if (op->resist[i])
            return 1;

    return 0;
}

/**
 * Frees a treasure, including its yes, no and next items.
 *
 * @param t
 * treasure to free. Pointer is free()d too, so becomes invalid.
 */
static void free_treasurestruct(treasure *t) {
    if (t->next)
        free_treasurestruct(t->next);
    if (t->next_yes)
        free_treasurestruct(t->next_yes);
    if (t->next_no)
        free_treasurestruct(t->next_no);
    free(t);
}

/**
 * Frees a link structure and its next items.
 *
 * @param lc
 * item to free. Pointer is free()d too, so becomes invalid.
 */
static void free_charlinks(linked_char *lc) {
    if (lc->next)
        free_charlinks(lc->next);
    free(lc);
}

/**
 * Totally frees an artifact, its next items, and such.
 *
 * @param at
 * artifact to free. Pointer is free()d too, so becomes invalid.
 *
 * @note
 * Objects at->item are malloc()ed by init_artifacts(), so can simply be free()d.
 *
 * But artifact inventory is a 'real' object, that may be created for 'old' objects. So should be
 * destroyed through object_free(). Note that it isn't on the usual item list, so some tweaking is required.
 */
static void free_artifact(artifact *at) {
    object *next;

    if (at->next)
        free_artifact(at->next);
    if (at->allowed)
        free_charlinks(at->allowed);
    while (at->item) {
        next = at->item->next;
        if (at->item->name)
            free_string(at->item->name);
        if (at->item->name_pl)
            free_string(at->item->name_pl);
        if (at->item->msg)
            free_string(at->item->msg);
        if (at->item->title)
            free_string(at->item->title);
        object_free_key_values(at->item);
        free(at->item);
        at->item = next;
    }
    free(at);
}

/**
 * Free specified list and its items.
 *
 * @param al
 * list to free. Pointer is free()d too, so becomes invalid.
 */
static void free_artifactlist(artifactlist *al) {
    artifactlist *nextal;

    for (; al != NULL; al = nextal) {
        nextal = al->next;
        if (al->items) {
            free_artifact(al->items);
        }
        free(al);
    }
}

/**
 * Free all treasure-related memory.
 */
void free_all_treasures(void) {
    treasurelist *tl, *next;

    for (tl = first_treasurelist; tl != NULL; tl = next) {
        next = tl->next;
        if (tl->name)
            free_string(tl->name);
        if (tl->items)
            free_treasurestruct(tl->items);
        free(tl);
    }
    free_artifactlist(first_artifactlist);
    first_artifactlist = NULL;
}

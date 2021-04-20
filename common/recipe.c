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
 * @file recipe.c
 * Basic stuff for use with the alchemy code. Clearly some of this stuff
 * could go into server/alchemy, but I left it here just in case it proves
 * more generally useful.
 *
 * Nov 1995 - file created by b.t. thomas@astro.psu.edu
 *
 * Our definition of 'formula' is any product of an alchemical process.
 * Ingredients are just comma delimited list of archetype (or object)
 * names.
 *
 * Example 'formula' entry in libdir/formulae:
 *      Object transparency
 *      chance 10
 *      ingred dust of beholdereye,gem
 *      arch potion_generic
 *
 * An ingredient is a name, which can contain an initial number for how many are needed.
 */

#include "global.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "assets.h"

static void build_stringlist(const char *str, char ***result_list, size_t *result_size);

/** Pointer to first recipelist. */
static recipelist *formulalist;

/**
 * Allocates a new recipelist.
 *
 * Will call fatal() if memory allocation error.
 *
 * @return
 * new structure initialized. Never NULL.
 */
static recipelist *init_recipelist(void) {
    recipelist *tl = (recipelist *)malloc(sizeof(recipelist));
    if (tl == NULL)
        fatal(OUT_OF_MEMORY);
    tl->total_chance = 0;
    tl->number = 0;
    tl->items = NULL;
    tl->next = NULL;
    return tl;
}

/**
 * Allocates a new recipe.
 *
 * Will call fatal() if memory allocation error.
 *
 * @return
 * new structure initialized. Never NULL.
 */
static recipe *get_empty_formula(void) {
    // This used to be a malloc followed by setting everything to zero.
    // So just use calloc to make it faster.
    // SilverNexus -- 2018-10-22
    recipe *t = (recipe *)calloc(1, sizeof(recipe));
    if (t == NULL)
        fatal(OUT_OF_MEMORY);
    return t;
}

/**
 * Gets a formula list by ingredients count.
 *
 * @param i
 * number of ingredients.
 *
 * @return
 * pointer to the formula list, or NULL if it doesn't exist.
 */
recipelist *get_formulalist(int i) {
    recipelist *fl = formulalist;
    int number = i;

    while (fl && number > 1) {
        if (!(fl = fl->next))
            break;
        number--;
    }
    return fl;
}

/**
 * Makes sure we actually have the requested artifact
 * and archetype.
 *
 * @param rp
 * recipe we want to check.
 *
 * @return
 * 1 if recipe is ok, 0 if missing something (and LOG() to error).
 */
static int check_recipe(const recipe *rp) {
    size_t i;
    int result;

    result = 1;
    for (i = 0; i < rp->arch_names; i++) {
        if (try_find_archetype(rp->arch_name[i]) != NULL) {
            const artifact *art = locate_recipe_artifact(rp, i);

            if (!art && strcmp(rp->title, "NONE") != 0) {
                LOG(llevError, "\nWARNING: Formula %s of %s has no artifact.\n", rp->arch_name[i], rp->title);
                result = 0;
            }
        } else {
            LOG(llevError, "\nWARNING: Can't find archetype %s for formula %s\n", rp->arch_name[i], rp->title);
            result = 0;
        }
    }

    return result;
}

/**
 * Builds up the lists of formula from the file in  the libdir. -b.t.
 */
void init_formulae(BufferReader *reader, const char *filename) {
    char *buf, *cp, *next;
    recipe *formula = NULL;
    recipelist *fl;
    linked_char *tmp;
    int value;

    if (!formulalist)
        formulalist = init_recipelist();

    while ((buf = bufferreader_next_line(reader)) != NULL) {
        if (*buf == '#' || *buf == '\0')
            continue;
        cp = buf;
        while (*cp == ' ') /* Skip blanks */
            cp++;

        if (!strncmp(cp, "Remove ", 7)) {
            if (strcmp(cp + 7, "*") == 0) {
                free_all_recipes();
                formulalist = init_recipelist();
            } else {
                LOG(llevError, "Recipes: only '*' is accepted for 'Remove' at %s:%d\n", filename, bufferreader_current_line(reader));
            }
            continue;
        }
        if (!strncmp(cp, "Object", 6)) {
            formula = get_empty_formula();
            formula->title = add_string(strchr(cp, ' ')+1);
        } else if (formula == NULL) {
            LOG(llevError, "recipe.c: First key in formulae file %s is not \"Object\".\n", filename);
            fatal(SEE_LAST_ERROR);
        } else if (!strncmp(cp, "keycode", 7)) {
            formula->keycode = add_string(strchr(cp, ' ')+1);
        } else if (sscanf(cp, "trans %d", &value)) {
            formula->transmute = value;
        } else if (sscanf(cp, "yield %d", &value)) {
            formula->yield = value;
        } else if (sscanf(cp, "chance %d", &value)) {
            formula->chance = value;
        } else if (sscanf(cp, "exp %d", &value)) {
            formula->exp = value;
        } else if (sscanf(cp, "diff %d", &value)) {
            formula->diff = value;
        } else if (!strncmp(cp, "ingred", 6)) {
            int numb_ingred;
            formula->ingred_count = 1;
            cp = strchr(cp, ' ')+1;
            do {
                if ((next = strchr(cp, ',')) != NULL) {
                    *(next++) = '\0';
                    formula->ingred_count++;
                }
                tmp = (linked_char *)malloc(sizeof(linked_char));
                /* trim the string */
                while (*cp == ' ')
                  cp++;
                while (*cp != '\0' && cp[strlen(cp) - 1] == ' ')
                  cp[strlen(cp) - 1] = '\0';
                tmp->name = add_string(cp);
                tmp->next = formula->ingred;
                formula->ingred = tmp;
                /* each ingredient's ASCII value is coadded. Later on this
                 * value will be used allow us to search the formula lists
                 * quickly for the right recipe.
                 */
                formula->index += strtoint(cp);
            } while ((cp = next) != NULL);
            /* now find the correct (# of ingred ordered) formulalist */
            numb_ingred = formula->ingred_count;
            fl = formulalist;
            while (numb_ingred != 1) {
                if (!fl->next)
                    fl->next = init_recipelist();
                fl = fl->next;
                numb_ingred--;
            }
            fl->total_chance += formula->chance;
            fl->number++;
            formula->next = fl->items;
            fl->items = formula;
        } else if (!strncmp(cp, "arch", 4)) {
            build_stringlist(strchr(cp, ' ')+1, &formula->arch_name, &formula->arch_names);
        } else if (!strncmp(cp, "skill", 5)) {
            formula->skill = add_string(strchr(cp, ' ')+1);
        } else if (!strncmp(cp, "cauldron", 8)) {
            formula->cauldron = add_string(strchr(cp, ' ')+1);
        } else if (!strncmp(cp, "failure_arch ", 13)) {
            formula->failure_arch = add_string(strchr(cp, ' ')+1);
        } else if (!strncmp(cp, "failure_message ", 16)) {
            formula->failure_message = add_string(strchr(cp, ' ')+1);
        } else if (sscanf(cp, "min_level %d", &value)) {
            formula->min_level = value;
        } else if (!strncmp(cp, "tool ", 5)) {
            build_stringlist(strchr(cp, ' ')+1, &formula->tool, &formula->tool_size);
        } else if (sscanf(cp, "combination %d", &value)) {
            formula->is_combination = value ? 1 : 0;
        } else
            LOG(llevError, "Unknown input in file %s: %s\n", filename, buf);
    }
    LOG(llevDebug, "done.\n");
}

/**
 * Check if formula don't have the same index.
 *
 * Since we are doing a squential search on the
 * formulae lists now, we have to be carefull that we dont have 2
 * formula with the exact same index value. Under the new nbatches
 * code, it is possible to have multiples of ingredients in a cauldron
 * which could result in an index formula mismatch. We *don't *check for
 * that possibility here. -b.t.
 *
 * LOG() to error level.
 *
 * @todo check archetypes exist, check coherence (skill present, cauldron ok, and such things), set chance to 0 for combinations
 */
void check_formulae(void) {
    recipelist *fl;
    recipe *check, *formula;
    int numb = 1, tool_match;
    size_t tool_i,tool_j;

    LOG(llevDebug, "Checking formulae lists...\n");

    for (fl = formulalist; fl != NULL; fl = fl->next) {
        for (formula = fl->items; formula != NULL; formula = formula->next) {
            for (check = formula->next; check != NULL; check = check->next)
                /* If one recipe has a tool and another a caudron, we should be able to handle it */
                if (check->index == formula->index &&
                        ((check->cauldron && formula->cauldron && strcmp(check->cauldron, formula->cauldron) == 0) ||
                        (check->tool_size == formula->tool_size && check->tool_size > 0))) {
                    /* Check the tool list to make sure they have no matches */
                    if (check->tool && formula->tool)
                    {
                        tool_match = 0;
                        for (tool_i = 0; tool_i < formula->tool_size; ++tool_i)
                            /* If it turns out these lists are sorted, then we could optimize this better. */
                            for (tool_j = 0; tool_j < check->tool_size; ++tool_j)
                                if (strcmp(formula->tool[tool_i], check->tool[tool_j]) == 0) {
                                    tool_match = 1;
                                    break; /* TODO: break out of the double loop */
                                }
                    }
                    else
                        tool_match = 1; /* If we get here, we matched on the cauldron */
                    /* Check to see if we have a denoted match */
                    if (tool_match) {
                        /* if the recipes don't have the same facility, then no issue anyway. */
                        LOG(llevError, " ERROR: On %d ingred list:\n", numb);
                        LOG(llevError, "Formulae [%s] of %s and [%s] of %s have matching index id (%d)\n",
                            formula->arch_name[0], formula->title, check->arch_name[0], check->title, formula->index);
                    }
                }
            for (size_t idx = 0; idx < formula->arch_names; idx++) {
                if (try_find_archetype(formula->arch_name[idx]) == NULL) {
                    LOG(llevError, "Formulae %s of %s (%d ingredients) references non existent archetype %s\n",
                        formula->arch_name[0], formula->title, numb, formula->arch_name[idx]);
                }
            }
        }
        numb++;
    }

    LOG(llevDebug, "done checking.\n");
}

/**
 * Dumps alchemy recipes to output.
 * Borrowed (again) from the artifacts code for this.
 *
 * @todo
 * use LOG() instead of fprintf?
 */
void dump_alchemy(void) {
    recipelist *fl = formulalist;
    recipe *formula = NULL;
    linked_char *next;
    int num_ingred = 1;

    fprintf(logfile, "\n");
    while (fl) {
        fprintf(logfile, "\n Formulae with %d ingredient%s  %d Formulae with total_chance=%d\n", num_ingred, num_ingred > 1 ? "s." : ".", fl->number, fl->total_chance);
        for (formula = fl->items; formula != NULL; formula = formula->next) {
            const artifact *art = NULL;
            char buf[MAX_BUF];
            size_t i;

            for (i = 0; i < formula->arch_names; i++) {
                const char *string = formula->arch_name[i];

                if (try_find_archetype(string) != NULL) {
                    art = locate_recipe_artifact(formula, i);
                    if (!art && strcmp(formula->title, "NONE"))
                        LOG(llevError, "Formula %s has no artifact\n", formula->title);
                    else {
                        if (strcmp(formula->title, "NONE"))
                            snprintf(buf, sizeof(buf), "%s of %s", string, formula->title);
                        else
                            strlcpy(buf, string, sizeof(buf));
                        fprintf(logfile, "%-30s(%d) bookchance %3d  ", buf, formula->index, formula->chance);
                        fprintf(logfile, "skill %s", formula->skill);
                        fprintf(logfile, "\n");
                        if (formula->ingred != NULL) {
                            int nval = 0, tval = 0;
                            fprintf(logfile, "\tIngred: ");
                            for (next = formula->ingred; next != NULL; next = next->next) {
                                if (nval != 0)
                                    fprintf(logfile, ",");
                                fprintf(logfile, "%s(%d)", next->name, (nval = strtoint(next->name)));
                                tval += nval;
                            }
                            fprintf(logfile, "\n");
                            if (tval != formula->index)
                                fprintf(logfile, "WARNING:ingredient list and formula values not equal.\n");
                        }
                        if (formula->skill != NULL)
                            fprintf(logfile, "\tSkill Required: %s", formula->skill);
                        if (formula->cauldron != NULL)
                            fprintf(logfile, "\tCauldron: %s\n", formula->cauldron);
                        fprintf(logfile, "\tDifficulty: %d\t Exp: %d\n", formula->diff, formula->exp);
                    }
                } else
                    LOG(llevError, "Can't find archetype:%s for formula %s\n", string, formula->title);
            }
        }
        fprintf(logfile, "\n");
        fl = fl->next;
        num_ingred++;
    }
}

/**
 * Find a treasure with a matching name.  The 'depth' parameter is
 * only there to prevent infinite loops in treasure lists (a list
 * referencing another list pointing back to the first one).
 *
 * @param t
 * item of treasure list to search from
 * @param name
 * name we're trying to find. Doesn't need to be a shared string.
 * @param depth
 * current depth. Code will exit if greater than 10.
 * @return
 * archetype with name, or NULL if nothing found.
 */
archetype *find_treasure_by_name(const treasure *t, const char *name, int depth) {
    treasurelist *tl;
    archetype *at;

    if (depth > 10)
        return NULL;

    while (t != NULL) {
        if (t->name != NULL) {
            tl = find_treasurelist(t->name);
            at = find_treasure_by_name(tl->items, name, depth+1);
            if (at != NULL)
                return at;
        } else {
            if (!strcasecmp(t->item->clone.name, name))
                return t->item;
        }
        if (t->next_yes != NULL) {
            at = find_treasure_by_name(t->next_yes, name, depth);
            if (at != NULL)
                return at;
        }
        if (t->next_no != NULL) {
            at = find_treasure_by_name(t->next_no, name, depth);
            if (at != NULL)
                return at;
        }
        t = t->next;
    }
    return NULL;
}

/**
 * Dumps to output all costs of recipes.
 *
 * Code copied from dump_alchemy() and modified by Raphael Quinet
 *
 * @todo
 * should use LOG()
 */
void dump_alchemy_costs(void) {
    recipelist *fl = formulalist;
    recipe *formula = NULL;
    linked_char *next;
    int num_ingred = 1;
    int num_errors = 0;
    long cost;
    long tcost;

    fprintf(logfile, "\n");
    while (fl) {
        fprintf(logfile, "\n Formulae with %d ingredient%s  %d Formulae with total_chance=%d\n", num_ingred, num_ingred > 1 ? "s." : ".", fl->number, fl->total_chance);
        for (formula = fl->items; formula != NULL; formula = formula->next) {
            const artifact *art = NULL;
            const archetype *at = NULL;
            char buf[MAX_BUF];
            size_t i;

            for (i = 0; i < formula->arch_names; i++) {
                const char *string = formula->arch_name[i];

                if ((at = try_find_archetype(string)) != NULL) {
                    art = locate_recipe_artifact(formula, i);
                    if (!art && strcmp(formula->title, "NONE"))
                        LOG(llevError, "Formula %s has no artifact\n", formula->title);
                    else {
                        if (!strcmp(formula->title, "NONE"))
                            strlcpy(buf, string, sizeof(buf));
                        else
                            snprintf(buf, sizeof(buf), "%s of %s", string, formula->title);
                        fprintf(logfile, "\n%-40s bookchance %3d  skill %s\n", buf, formula->chance, formula->skill);
                        if (formula->ingred != NULL) {
                            tcost = 0;
                            for (next = formula->ingred; next != NULL; next = next->next) {
                                cost = recipe_find_ingredient_cost(next->name);
                                if (cost < 0)
                                    num_errors++;
                                fprintf(logfile, "\t%-33s%5ld\n", next->name, cost);
                                if (cost < 0 || tcost < 0)
                                    tcost = -1;
                                else
                                    tcost += cost;
                            }
                            if (art != NULL && art->item != NULL)
                                cost = at->clone.value*art->item->value;
                            else
                                cost = at->clone.value;
                            fprintf(logfile, "\t\tBuying result costs: %5ld", cost);
                            if (formula->yield > 1) {
                                fprintf(logfile, " to %ld (max %d items)\n", cost*formula->yield, formula->yield);
                                cost = cost*(formula->yield+1L)/2L;
                            } else
                                fprintf(logfile, "\n");
                            fprintf(logfile, "\t\tIngredients cost:    %5ld\n\t\tComment: ", tcost);
                            if (tcost < 0)
                                fprintf(logfile, "Could not find some ingredients.  Check the formula!\n");
                            else if (tcost > cost)
                                fprintf(logfile, "Ingredients are much too expensive.  Useless formula.\n");
                            else if (tcost*2L > cost)
                                fprintf(logfile, "Ingredients are too expensive.\n");
                            else if (tcost*10L < cost)
                                fprintf(logfile, "Ingredients are too cheap.\n");
                            else
                                fprintf(logfile, "OK.\n");
                        }
                    }
                } else
                    LOG(llevError, "Can't find archetype:%s for formula %s\n", string, formula->title);
            }
        }
        fprintf(logfile, "\n");
        fl = fl->next;
        num_ingred++;
    }
    if (num_errors > 0)
        fprintf(logfile, "WARNING: %d objects required by the formulae do not exist in the game.\n", num_errors);
}

/**
 * Extracts the name from an ingredient.
 *
 * @param name
 * ingredient to extract from. Can contain a number at start.
 * @return
 * pointer in name to the first character of the ingredient's name.
 */
static const char *ingred_name(const char *name) {
    const char *cp = name;

    if (atoi(cp))
        cp = strchr(cp, ' ')+1;
    return cp;
}

/**
 * Extracts the number part of an ingredient.
 *
 * @param buf
 * ingredient.
 * @return
 * number part of an ingredient.
 */
static int numb_ingred(const char *buf) {
    int numb;

    if ((numb = atoi(buf)))
        return numb;
    else
        return 1;
}

/**
 * Convert buf into an integer equal to the coadded sum of the (lowercase) character
 *
 * ASCII values in buf (times prepended integers).
 *
 * @param buf
 * buffer we want to convert. Can contain an initial number.
 * @return
 * sum of lowercase characters of the ingredient's name.
 */
int strtoint(const char *buf) {
    const char *cp = ingred_name(buf);
    int val = 0, len = strlen(cp), mult = numb_ingred(buf);

    while (len) {
        val += tolower(*cp);
        cp++; len--;
    }
    return val*mult;
}

/**
 * Finds an artifact for a recipe.
 *
 * @param rp
 * recipe
 * @param idx
 * index of ingredient in recipe.
 * @return
 * artifact, or NULL if not found.
 */
const artifact *locate_recipe_artifact(const recipe *rp, size_t idx) {
    object *item = create_archetype(rp->arch_name[idx]);
    const artifactlist *at = NULL;
    const artifact *art = NULL;

    if (!item)
        return (artifact *)NULL;

    if ((at = find_artifactlist(item->type)))
        for (art = at->items; art; art = art->next)
            if (!strcmp(art->item->name, rp->title) && legal_artifact_combination(item, art))
                break;

    object_free_drop_inventory(item);

    return art;
}

/**
 * Gets a random recipe list.
 *
 * @return
 * random recipe list.
 */
static recipelist *get_random_recipelist(void) {
    recipelist *fl = NULL;
    int number = 0, roll = 0;

    /* first, determine # of recipelist we have */
    for (fl = get_formulalist(1); fl; fl = fl->next)
        number++;

    /* now, randomly choose one */
    if (number > 0)
        roll = RANDOM()%number;

    fl = get_formulalist(1);
    while (roll && fl) {
        if (fl->next)
            fl = fl->next;
        else
            break;
        roll--;
    }
    if (!fl) /* failed! */
        LOG(llevError, "get_random_recipelist(): no recipelists found!\n");
    else if (fl->total_chance == 0)
        fl = get_random_recipelist();

    return fl;
}

/**
 * Gets a random recipe from a list, based on chance.
 *
 * @param rpl
 * recipelist we want a recipe from. Can be NULL in which case a random one is selected.
 * @return
 * random recipe. Can be NULL if recipelist has a total_chance of 0.
 */
recipe *get_random_recipe(recipelist *rpl) {
    recipelist *fl = rpl;
    recipe *rp = NULL;
    int r = 0;

    /* looks like we have to choose a random one */
    if (fl == NULL)
        if ((fl = get_random_recipelist()) == NULL)
            return rp;

    if (fl->total_chance > 0) {
        r = RANDOM()%fl->total_chance;
        for (rp = fl->items; rp; rp = rp->next) {
            r -= rp->chance;
            if (r < 0)
                break;
        }
    }
    return rp;
}

/**
 * Frees all memory allocated to recipes and recipes lists.
 */
void free_all_recipes(void) {
    recipelist *fl, *flnext;
    recipe *formula = NULL, *next;
    linked_char *lchar, *charnext;

    LOG(llevDebug, "Freeing all the recipes\n");
    for (fl = formulalist; fl != NULL; fl = flnext) {
        flnext = fl->next;

        for (formula = fl->items; formula != NULL; formula = next) {
            next = formula->next;

            free(formula->arch_name[0]);
            free(formula->arch_name);
            if (formula->title)
                free_string(formula->title);
            if (formula->skill)
                free_string(formula->skill);
            if (formula->cauldron)
                free_string(formula->cauldron);
            if (formula->failure_arch)
                free_string(formula->failure_arch);
            if (formula->failure_message)
                free_string(formula->failure_message);
            for (lchar = formula->ingred; lchar; lchar = charnext) {
                charnext = lchar->next;
                free_string(lchar->name);
                free(lchar);
            }
            if (formula->tool)
                free(formula->tool[0]);
            free(formula->tool);
            free(formula);
        }
        free(fl);
    }
    formulalist = NULL;
}

/**
 * Split a comma separated string list into words.
 *
 * @param str
 * the string to split
 * @param[out] result_list
 * pointer to return value for the newly created list; the
 * caller is responsible for freeing both *result_list and **result_list.
 * @param[out] result_size
 * pointer to return value for the size of the newly created list
 */
static void build_stringlist(const char *str, char ***result_list, size_t *result_size) {
    char *dup;
    char *p;
    size_t size;
    size_t i;

    dup = strdup_local(str);
    if (dup == NULL)
        fatal(OUT_OF_MEMORY);

    size = 0;
    for (p = strtok(dup, ","); p != NULL; p = strtok(NULL, ","))
        size++;

    assert(size > 0);
    *result_list = malloc(sizeof(**result_list) * size);
    if (*result_list == NULL)
        fatal(OUT_OF_MEMORY);
    *result_size = size;

    for (i = 0; i < size; i++) {
        (*result_list)[i] = dup;
        dup = dup+strlen(dup)+1;
    }
}

/**
 * Find a recipe for a specified tool. This function can be called multiple times to browse the whole list,
 * using the 'from' parameter.
 * @param tool tool's archetype name.
 * @param from formula to search from, if NULL from the first one.
 * @return matching formula, NULL if none matching.
 */
recipe *find_recipe_for_tool(const char *tool, recipe *from) {
    size_t t;
    recipelist *list = from ? get_formulalist(from->ingred_count) : formulalist;
    recipe *test = from ? from->next : list->items;

    while (list) {
        while (test) {
            for (t = 0; t < test->tool_size; t++) {
                if (strcmp(test->tool[t], tool) == 0) {
                    return test;
                }
            }
            test = test->next;
        }

        list = list->next;
    }

    return NULL;
}

/**
 * @file
 * Alchemy recipe structures.
 */

#ifndef RECIPE_H
#define RECIPE_H

/** One alchemy recipe. */
typedef struct recipestruct {
    const char *title;          /**< Distinguishing name of product. */
    size_t arch_names;          /**< Size of the arch_name[] array. */
    char **arch_name;           /**< Possible archetypes of the final product made. */
    int chance;                 /**< Chance that recipe for this item will appear
                                  * in an alchemical grimore. */
    int diff;                   /**< Alchemical dfficulty level. */
    int exp;                    /**< How much exp to give for this formulae. */
    int index;                  /**< Index value derived from formula ingredients. */
    int transmute;              /**< If defined, one of the formula ingredients is
                                  * used as the basis for the product object. */
    int yield;                  /**< Maximum number of items produced by the recipe. */
    linked_char *ingred;        /**< List of ingredients. */
    struct recipestruct *next;  /**< Next recipe with the same number of ingredients. */
    const char *keycode;        /**< Optional keycode needed to use the recipe. */
    const char *skill;          /** Skill name used to make this recipe. */
    const char *cauldron;       /**< Arch of the cauldron/workbench used to house the formulae. */
} recipe;

/** List of recipes with a certain number of ingredients. */
typedef struct recipeliststruct {
    int total_chance;               /**< Total chance of the recipes in this list. */
    int number;                     /**< Number of recipes in this list. */
    struct recipestruct *items;     /**< Pointer to first recipe in this list. */
    struct recipeliststruct *next;  /**< Pointer to next recipe list. */
} recipelist;

#endif /* RECIPE_H */

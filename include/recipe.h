#ifndef RECIPE_H
#define RECIPE_H

/* 'recipe' and 'recipelist' are used by the alchemy code */
typedef struct recipestruct {
    const char *title; /* distinguishing name of product */
    size_t arch_names; /* the size of the arch_name[] array */
    char **arch_name;  /* the possible archetypes of the final product made */
    int chance;        /* chance that recipe for this item will appear
		        * in an alchemical grimore */
    int diff;	       /* alchemical dfficulty level */
    int exp;	       /* how much exp to give for this formulae */
    int index;	       /* an index value derived from formula ingredients */
    int transmute;     /* if defined, one of the formula ingredients is
			* used as the basis for the product object */
    int yield;         /* The maximum number of items produced by the recipe */
    linked_char *ingred; /* list of ingredients */
    struct recipestruct *next;
    const char *keycode;     /* keycode needed to use the recipe */
    const char *skill;       /* skill name used to make this recipe */
    const char *cauldron;    /* the arch of the cauldron/workbench used to house the
			* formulae. */
} recipe;

typedef struct recipeliststruct {
    int total_chance;
    int number;			    /* number of recipes in this list */ 
    struct recipestruct *items;	    /* pointer to first recipe in this list */ 
    struct recipeliststruct *next;  /* pointer to next recipe list */ 
} recipelist;

#endif /* RECIPE_H */

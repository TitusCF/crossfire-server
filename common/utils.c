/*
 * static char *rcsid_utils_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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
 * @file utils.c
 * General convenience functions for crossfire.
 *
 * The random functions here take luck into account when rolling random
 * dice or numbers.  This function has less of an impact the larger the
 * difference becomes in the random numbers.  IE, the effect is lessened
 * on a 1-1000 roll, vs a 1-6 roll.  This can be used by crafty programmers,
 * to specifically disable luck in certain rolls, simply by making the
 * numbers larger (ie, 1d1000 > 500 vs 1d6 > 3)
 */

#include <stdlib.h>
#include <global.h>

/**
 * Roll a random number between min and max.  Uses op to determine luck,
 * and if goodbad is non-zero, luck increases the roll, if zero, it decreases.
 *
 * Generally, op should be the player/caster/hitter requesting the roll,
 * not the recipient (ie, the poor slob getting hit). [garbled 20010916]
 */
int random_roll(int min, int max, const object *op, int goodbad) {
    int omin, diff, luck, base, ran;

    omin = min;
    diff = max - min + 1;
    ((diff > 2) ? (base = 20) : (base = 50)); /* d2 and d3 are corner cases */

    if (max < 1 || diff < 1) {
      LOG(llevError, "Calling random_roll with min=%d max=%d\n", min, max);
      return(min); /* avoids a float exception */
    }

    ran = RANDOM();

    if (op->type != PLAYER)
        return((ran%diff)+min);

    luck = op->stats.luck;
    if (RANDOM()%base < MIN(10, abs(luck))) {
        /* we have a winner */
        ((luck > 0) ? (luck = 1) : (luck = -1));
        diff -= luck;
        if (diff < 1)
            return(omin); /*check again*/
        ((goodbad) ? (min += luck) : (diff));

        return(MAX(omin, MIN(max, (ran%diff)+min)));
    }
    return((ran%diff)+min);
}

/**
 * This is a 64 bit version of random_roll() above.  This is needed
 * for exp loss calculations for players changing religions.
 */
sint64 random_roll64(sint64 min, sint64 max, const object *op, int goodbad) {
    sint64 omin, diff, luck, ran;
    int base;

    omin = min;
    diff = max - min + 1;
    ((diff > 2) ? (base = 20) : (base = 50)); /* d2 and d3 are corner cases */

    if (max < 1 || diff < 1) {
#ifndef WIN32
        LOG(llevError, "Calling random_roll with min=%lld max=%lld\n", min, max);
#else
        LOG(llevError, "Calling random_roll with min=%I64d max=%I64d\n", min, max);
#endif
      return(min); /* avoids a float exception */
    }

    /* Don't know of a portable call to get 64 bit random values.
     * So make a call to get two 32 bit random numbers, and just to
     * a little byteshifting.  Do make sure the first one is only
     * 32 bit, so we don't get skewed results
     */
    ran = (RANDOM() & 0xffffffff) | ((sint64)RANDOM() << 32);

    if (op->type != PLAYER)
        return((ran%diff)+min);

    luck = op->stats.luck;
    if (RANDOM()%base < MIN(10, abs(luck))) {
        /* we have a winner */
        ((luck > 0) ? (luck = 1) : (luck = -1));
        diff -= luck;
        if (diff < 1)
            return(omin); /*check again*/
        ((goodbad) ? (min += luck) : (diff));

        return(MAX(omin, MIN(max, (ran%diff)+min)));
    }
    return((ran%diff)+min);
}

/**
 * Roll a number of dice (2d3, 4d6).  Uses op to determine luck,
 * If goodbad is non-zero, luck increases the roll, if zero, it decreases.
 * Generally, op should be the player/caster/hitter requesting the roll,
 * not the recipient (ie, the poor slob getting hit).
 * The args are num D size (ie 4d6)  [garbled 20010916]
 */
int die_roll(int num, int size, const object *op, int goodbad) {
    int min, diff, luck, total, i, gotlucky, base, ran;

    diff = size;
    min = 1;
    luck = total = gotlucky = 0;
    ((diff > 2) ? (base = 20) : (base = 50)); /* d2 and d3 are corner cases */
    if (size < 2 || diff < 1) {
      LOG(llevError, "Calling die_roll with num=%d size=%d\n", num, size);
      return(num); /* avoids a float exception */
    }

    if (op->type == PLAYER)
        luck = op->stats.luck;

    for (i = 0; i < num; i++) {
        if (RANDOM()%base < MIN(10, abs(luck)) && !gotlucky) {
            /* we have a winner */
            gotlucky++;
            ((luck > 0) ? (luck = 1) : (luck = -1));
            diff -= luck;
            if (diff < 1)
                return(num); /*check again*/
            ((goodbad) ? (min += luck) : (diff));
            ran = RANDOM();
            total += MAX(1, MIN(size, (ran%diff)+min));
        } else {
            total += RANDOM()%size+1;
        }
    }
    return(total);
}

/**
 * Returns a number between min and max.
 *
 * It is suggested one use these functions rather than RANDOM()%, as it
 * would appear that a number of off-by-one-errors exist due to improper
 * use of %.  This should also prevent SIGFPE.
 */
int rndm(int min, int max)
{
    int diff;

    diff = max - min + 1;
    if (max < 1 || diff < 1)
        return(min);

    return(RANDOM()%diff+min);
}

/**
 * Decay and destroy persihable items in a map
 */
void decay_objects(mapstruct *m)
{
    int x, y, destroy;
    object *op, *otmp;

    if (m->unique)
        return;

    for (x=0; x < MAP_WIDTH(m); x++)
        for (y=0; y < MAP_HEIGHT(m); y++)
            for (op = get_map_ob(m, x, y); op; op = otmp) {
                destroy = 0;
                otmp = op->above;
                if (QUERY_FLAG(op,FLAG_IS_FLOOR) && QUERY_FLAG(op, FLAG_UNIQUE))
                    break;
                if (QUERY_FLAG(op, FLAG_IS_FLOOR) ||
                  QUERY_FLAG(op, FLAG_OBJ_ORIGINAL) ||
                  QUERY_FLAG(op, FLAG_OBJ_SAVE_ON_OVL) ||
                  QUERY_FLAG(op, FLAG_UNIQUE) ||
                  QUERY_FLAG(op, FLAG_OVERLAY_FLOOR) ||
                  QUERY_FLAG(op, FLAG_UNPAID) || IS_LIVE(op))
                    continue;
                /* otherwise, we decay and destroy */
                if (IS_WEAPON(op)) {
                    op->stats.dam--;
                    if (op->stats.dam < 0)
                    destroy = 1;
                } else if (IS_ARMOR(op) || IS_SHIELD(op) || op->type == GIRDLE || op->type == GLOVES || op->type == CLOAK) {
                    op->stats.ac--;
                    if (op->stats.ac < 0)
                    destroy = 1;
                } else if (op->type == FOOD) {
                    op->stats.food -= rndm(5,20);
                    if (op->stats.food < 0)
                    destroy = 1;
                } else {
                    if (op->material & M_PAPER || op->material & M_LEATHER ||
                      op->material & M_WOOD || op->material & M_ORGANIC ||
                      op->material & M_CLOTH || op->material & M_LIQUID)
                        destroy = 1;
                    if (op->material & M_IRON && rndm(1,5) == 1)
                        destroy = 1;
                    if (op->material & M_GLASS && rndm(1,2) == 1)
                        destroy = 1;
                    if ((op->material & M_STONE || op->material & M_ADAMANT) &&
                      rndm(1,10) == 1)
                        destroy = 1;
                    if ((op->material & M_SOFT_METAL || op->material & M_BONE) &&
                      rndm(1,3) == 1)
                        destroy = 1;
                    if (op->material & M_ICE && MAP_TEMP(m) > 32)
                        destroy = 1;
                }
                /* adjust overall chance below */
                if (destroy && rndm(0, 1)) {
                    remove_ob(op);
                    free_object(op);
                }
            }
}

/**
 * Convert materialname to materialtype_t
 *
 * @todo
 * why use a break?
 */
materialtype_t *name_to_material(const char *name)
{
    materialtype_t *mt, *nmt;

    mt = NULL;
    for (nmt = materialt; nmt != NULL && nmt->next != NULL; nmt=nmt->next) {
        if (strcmp(name, nmt->name) == 0) {
            mt = nmt;
            break;
        }
    }
    return mt;
}

/**
 * When doing transmutation of objects, we have to recheck the resistances,
 * as some that did not apply previously, may apply now.
 *
 * Only works on armors.
 */
void transmute_materialname(object *op, const object *change)
{
    materialtype_t *mt;
    int j;

    if (op->materialname == NULL)
        return;

    if (change->materialname != NULL &&
      strcmp(op->materialname, change->materialname))
        return;

    if (!(IS_ARMOR(op)  || IS_SHIELD(op) || op->type == GIRDLE || op->type == GLOVES || op->type == CLOAK))
        return;

    mt = name_to_material(op->materialname);
    if (!mt) {
        LOG(llevError, "archetype '%s>%s' uses nonexistent material '%s'\n", op->arch->name, op->name, op->materialname);
        return;
    }

    for (j=0; j < NROFATTACKS; j++)
        if (op->resist[j] == 0 && change->resist[j] != 0) {
            op->resist[j] += mt->mod[j];
            if (op->resist[j] > 100)
                op->resist[j] = 100;
            if (op->resist[j] < -100)
                op->resist[j] = -100;
        }
}

/**
 * Set the materialname and type for an item
 */
void set_materialname(object *op, int difficulty, materialtype_t *nmt)
{
    materialtype_t *mt, *lmt;

    if (op->materialname != NULL)
        return;



    if (nmt == NULL) {
        lmt = NULL;
#ifndef NEW_MATERIAL_CODE
        for (mt = materialt; mt != NULL && mt->next != NULL; mt=mt->next) {
            if (op->material & mt->material) {
                lmt = mt;
                break;
            }
        }

#else
        for (mt = materialt; mt != NULL && mt->next != NULL; mt=mt->next) {
            if (op->material & mt->material && rndm(1, 100) <= mt->chance &&
              difficulty >= mt->difficulty &&
              (op->magic >= mt->magic || mt->magic == 0)) {
                lmt = mt;
                if (!(IS_WEAPON(op) || IS_ARMOR(op) || IS_SHIELD(op) || op->type == GIRDLE || op->type == GLOVES || op->type == CLOAK))
                    break;
            }
        }
#endif
    } else {
        lmt = nmt;
    }

    if (lmt != NULL) {
#ifndef NEW_MATERIAL_CODE
        op->materialname = add_string(lmt->name);
        return;
#else

        if (op->stats.dam && IS_WEAPON(op)) {
            op->stats.dam += lmt->damage;
            if (op->stats.dam < 1)
                op->stats.dam = 1;
        }
        if (op->stats.sp && op->type == BOW)
            op->stats.sp += lmt->sp;
        if (op->stats.wc && IS_WEAPON(op))
            op->stats.wc += lmt->wc;
        if (IS_ARMOR(op) || IS_SHIELD(op) || op->type == GIRDLE || op->type == GLOVES || op->type == CLOAK) {
            int j;
            if (op->stats.ac)
                op->stats.ac += lmt->ac;
            for (j=0; j < NROFATTACKS; j++)
                if (op->resist[j] != 0) {
                    op->resist[j] += lmt->mod[j];
                    if (op->resist[j] > 100)
                    op->resist[j] = 100;
                    if (op->resist[j] < -100)
                    op->resist[j] = -100;
                }
        }
        op->materialname = add_string(lmt->name);
        /* dont make it unstackable if it doesn't need to be */
        if (IS_WEAPON(op) || IS_ARMOR(op) || IS_SHIELD(op) || op->type == GIRDLE || op->type == GLOVES || op->type == CLOAK) {
            op->weight = (op->weight * lmt->weight)/100;
            op->value = (op->value * lmt->value)/100;
        }
#endif
    }
}

/**
 * Strip out the media tags from a String.
 * Warning the input string will contain the result string
 */
void strip_media_tag(char *message){
    int in_tag=0;
    char* dest;
    char* src;
    src=dest=message;
    while (*src!='\0'){
        if (*src=='['){
            in_tag=1;
        } else if (in_tag && (*src==']'))
            in_tag=0;
        else if (!in_tag){
            *dest=*src;
            dest++;
        }
        src++;
    }
    *dest='\0';
}

/**
 * Finds a string in a string.
 *
 * @todo
 * isn't there another function (porting.c?) for that?
 */
const char* strrstr(const char* haystack, const char* needle){
    const char* lastneedle;
    lastneedle=NULL;
    while((haystack=strstr(haystack,needle))!=NULL){
        lastneedle=haystack;
        haystack++;
    }
    return lastneedle;

}
#define EOL_SIZE (sizeof("\n")-1)
/**
 * Removes endline from buffer (modified in place).
 */
void strip_endline(char* buf){
    if (strlen(buf)<sizeof("\n")){
        return;  
    }
    if (!strcmp(buf+strlen(buf)-EOL_SIZE,"\n"))
        buf[strlen(buf)-EOL_SIZE]='\0';    
}

/**
 * Replace in string src all occurrences of key by replacement. The resulting
 * string is put into result; at most resultsize characters (including the
 * terminating null character) will be written to result.
 */
void replace(const char *src, const char *key, const char *replacement, char *result, size_t resultsize)
{
    size_t resultlen;
    size_t keylen;

    /* special case to prevent infinite loop if key==replacement=="" */
    if(strcmp(key, replacement) == 0)
    {
        snprintf(result, resultsize, "%s", src);
        return;
    }

    keylen = strlen(key);

    resultlen = 0;
    while(*src != '\0' && resultlen+1 < resultsize)
    {
        if(strncmp(src, key, keylen) == 0)
        {
            snprintf(result+resultlen, resultsize-resultlen, "%s", replacement);
            resultlen += strlen(result+resultlen);
            src += keylen;
        }
        else
        {
            result[resultlen++] = *src++;
        }
    }
    result[resultlen] = '\0';
}

/**
 * Taking a string as an argument, mutate it into a string that looks like a list.
 *
 * A 'list' for the purposes here is a string of items, seperated by commas, except
 * for the last entry, which has an 'and' before it, and a full stop (period) after it.
 *
 * This function will also strip all trailing non alphanumeric characters.
 *
 * It does not insert an oxford comma.
 *
 * @param input
 * string to transform. Will be overwritten. Must be long enough to contain the modified string.
 *
 * @todo
 * use safe string functions.
 */
void make_list_like(char *input) {
    char *p, tmp[MAX_BUF];
    int i;
    if (!input || strlen(input) > MAX_BUF-5) return; 
    /* bad stuff would happen if we continued here, the -5 is to make space for ' and ' */

    strncpy(tmp, input, MAX_BUF-5);
    /*trim all trailing commas, spaces etc.*/
    for (i=strlen(tmp); !isalnum(tmp[i]) && i >= 0; i--) 
        tmp[i]='\0';
    strcat(tmp, ".");

    p=strrchr(tmp, ',');
    if (p) {
        *p='\0';
        strcpy(input, tmp);
        p++;
        strcat(input, " and");
        strcat(input, p);
    }
    else strcpy(input, tmp);
    return;
}

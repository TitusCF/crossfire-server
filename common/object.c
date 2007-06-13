/*
 * static char *rcsid_object_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2001 Mark Wedel & Crossfire Development Team
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
 * @file common/object.c
 * Everything related to objects, in their basic form.
 */

/* Eneq(@csd.uu.se): Added weight-modifiers in environment of objects.
   sub/add_weight will transcend the environment updating the carrying
   variable. */

#include <stdlib.h>
#include <string.h>
#include <global.h>
#ifndef WIN32 /* ---win32 exclude headers */
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#endif /* win32 */
#include <object.h>
#include <skills.h>
#include <loader.h>

static int compare_ob_value_lists_one(const object *, const object *);
static int compare_ob_value_lists(const object *, const object *);
static void expand_objects(void);
static void free_object2(object *, int);
static void permute(int *, int, int);
static int set_ob_key_value_s(object *, const char *, const char *, int);
static void get_multi_size(object *, int *, int *, int *, int *);



#ifdef MEMORY_DEBUG
int nroffreeobjects = 0;  /**< Number of free objects. */
int nrofallocobjects = 0; /**< Number of allocated objects. */
#undef OBJ_EXPAND
#define OBJ_EXPAND 1
#else
object objarray[STARTMAX]; /**< All objects, allocated this way at first */
int nroffreeobjects = STARTMAX;  /**< How many OBs allocated and free (free) */
int nrofallocobjects = STARTMAX; /**< How many OBs allocated (free + used) */
#endif

object *objects;           /**< Pointer to the list of used objects */
object *free_objects;      /**< Pointer to the list of unused objects */
object *active_objects;    /**< List of active objects that need to be processed */

/** X offset when searching around a spot. */
short freearr_x[SIZEOFFREE]=
    {0,0,1,1,1,0,-1,-1,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-1,
     0,1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-3,-2,-1};

/** Y offset when searching around a spot. */
short freearr_y[SIZEOFFREE]=
    {0,-1,-1,0,1,1,1,0,-1,-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,
     -3,-3,-3,-3,-2,-1,0,1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-3,-3};

/** Number of spots around a location, including that location (except for 0) */
int maxfree[SIZEOFFREE]=
    {0,9,10,13,14,17,18,21,22,25,26,27,30,31,32,33,36,37,39,39,42,43,44,45,
     48,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49,49};

/** Direction we're pointing on this spot. */
int freedir[SIZEOFFREE]= {
  0,1,2,3,4,5,6,7,8,1,2,2,2,3,4,4,4,5,6,6,6,7,8,8,8,
  1,2,2,2,2,2,3,4,4,4,4,4,5,6,6,6,6,6,7,8,8,8,8,8};


/**
 * Compares value lists.
 *
 * @param wants
 * what to search
 * @param has
 * where to search.
 * @return
 * TRUE if every key_values in wants has a partner with the same value in has.
 */
static int compare_ob_value_lists_one(const object * wants, const object * has) {
    key_value * wants_field;

    /* n-squared behaviour (see get_ob_key_link()), but I'm hoping both
     * objects with lists are rare, and lists stay short. If not, use a
     * different structure or at least keep the lists sorted... 
     */

    /* For each field in wants, */
    for (wants_field = wants->key_values; wants_field != NULL; wants_field = wants_field->next) {
        key_value * has_field;

        /* Look for a field in has with the same key. */
        has_field = get_ob_key_link(has, wants_field->key);

        if (has_field == NULL) {
            /* No field with that name. */
            return FALSE;
        }

        /* Found the matching field. */
        if (has_field->value != wants_field->value) {
            /* Values don't match, so this half of the comparison is false. */
            return FALSE;
        }

        /* If we get here, we found a match. Now for the next field in wants. */
    }

    /* If we get here, every field in wants has a matching field in has. */
    return TRUE;
}

/**
 * Compares two object lists.
 * @param ob1
 * @param ob2
 * objects to compare.
 * @return
 * TRUE if ob1 has the same key_values as ob2.
 */
static int compare_ob_value_lists(const object * ob1, const object * ob2) {
    /* However, there may be fields in has which aren't partnered in wants,
     * so we need to run the comparison *twice*. :(
     */
    return compare_ob_value_lists_one(ob1, ob2) && compare_ob_value_lists_one(ob2, ob1);
}

/**
 * Examines the 2 objects given to it, and returns true if
 * they can be merged together, including inventory.
 *
 * Note that this function appears a lot longer than the macro it
 * replaces - this is mostly for clarity - a decent compiler should hopefully
 * reduce this to the same efficiency.
 *
 * Check nrof variable *before* calling can_merge()
 *
 * Improvements made with merge:  Better checking on potion, and also
 * check weight
 *
 * @param ob1
 * @param ob2
 * objects to try to merge.
 * @return
 * TRUE if they can be merged, FALSE else.
 *
 * @todo
 * check the function at places marked.
 */
int can_merge(object *ob1, object *ob2) {

    /* A couple quicksanity checks */
    if ((ob1 == ob2) || (ob1->type != ob2->type)) return 0;

    if (ob1->speed != ob2->speed) return 0;
    /* Note sure why the following is the case - either the object has to
     * be animated or have a very low speed.  Is this an attempted monster
     * check?
     */
    /*TODO is this check really needed?*/
    if (!QUERY_FLAG(ob1,FLAG_ANIMATE) && FABS((ob1)->speed) > MIN_ACTIVE_SPEED)
        return 0;

    /* Do not merge objects if nrof would overflow. We use 1UL<<31 since that
     * value could not be stored in a sint32 (which unfortunately sometimes is
     * used to store nrof).
     */
    if (ob1->nrof+ob2->nrof >= 1UL<<31)
        return 0;

    /* This is really a spellbook check - really, we should
     * check all objects in the inventory.
    */
    /*TODO is this check really needed?*/
    if (ob1->inv || ob2->inv) {
        /* if one object has inventory but the other doesn't, not equiv */
        if ((ob1->inv && !ob2->inv) || (ob2->inv && !ob1->inv)) return 0;

        /* Now check to see if the two inventory objects could merge */
        if (!can_merge(ob1->inv, ob2->inv)) return 0;

        /* inventory ok - still need to check rest of this object to see
         * if it is valid.
         */
    }

    /* If the objects have been identified, set the BEEN_APPLIED flag.
     * This is to the comparison of the flags below will be OK.  We
     * just can't ignore the been applied or identified flags, as they
     * are not equal - just if it has been identified, the been_applied
     * flags lose any meaning.
     */

    /*TODO is this hack on BEEN_APPLIED really needed? */
    if (QUERY_FLAG(ob1, FLAG_IDENTIFIED))
        SET_FLAG(ob1, FLAG_BEEN_APPLIED);

    if (QUERY_FLAG(ob2, FLAG_IDENTIFIED))
        SET_FLAG(ob2, FLAG_BEEN_APPLIED);


    /* Note: FLAG_INV_LOCKED is ignored for merging purposes */
    if ((ob1->arch != ob2->arch) ||
        (ob1->flags[0] != ob2->flags[0]) ||
        (ob1->flags[1] != ob2->flags[1]) ||
        (ob1->flags[2] != ob2->flags[2]) ||
        ((ob1->flags[3] & ~0x4) != (ob2->flags[3] & ~0x4)) || /* ignore CLIENT_SENT */
        (ob1->name != ob2->name) || 
        (ob1->title != ob2->title) ||
        (ob1->msg != ob2->msg) || 
        (ob1->weight != ob2->weight) ||
        (memcmp(&ob1->resist, &ob2->resist, sizeof(ob1->resist))!=0) ||
        (memcmp(&ob1->stats, &ob2->stats, sizeof(ob1->stats))!=0) ||
        (ob1->attacktype != ob2->attacktype) ||
        (ob1->magic != ob2->magic) ||
        (ob1->slaying != ob2->slaying) ||
        (ob1->skill != ob2->skill) ||
        (ob1->value != ob2->value) ||
        (ob1->animation_id != ob2->animation_id) ||
        (ob1->client_type != ob2->client_type) ||
        (ob1->materialname != ob2->materialname) ||
        (ob1->lore != ob2->lore) ||
        (ob1->subtype != ob2->subtype) ||
        (ob1->move_type != ob2->move_type) ||
        (ob1->move_block != ob2->move_block) ||
        (ob1->move_allow != ob2->move_allow) ||
        (ob1->move_on != ob2->move_on) ||
        (ob1->move_off != ob2->move_off) ||
        (ob1->move_slow != ob2->move_slow) ||
        (ob1->move_slow_penalty != ob2->move_slow_penalty) ||
        (ob1->map_layer != ob2->map_layer)
    )
        return 0;

    /* Don't merge objects that are applied.  With the new 'body' code,
     * it is possible for most any character to have more than one of
     * some items equipped, and we don't want those to merge.
     */
    if (QUERY_FLAG(ob1, FLAG_APPLIED) || QUERY_FLAG(ob2, FLAG_APPLIED))
        return 0;

    if (ob1->key_values != NULL || ob2->key_values != NULL) {
        /* At least one of these has key_values. */
        if ((ob1->key_values == NULL) != (ob2->key_values == NULL)) {
            /* One has fields, but the other one doesn't. */
            return 0;
        } else {
            return compare_ob_value_lists(ob1, ob2);
        }
    }

    /*TODO should this really be limited to scrolls?*/
    switch (ob1->type) {
        case SCROLL:
            if (ob1->level != ob2->level) return 0;
            break;

    }

    /* Don't merge items with differing custom names. */
    if (ob1->custom_name != ob2->custom_name) return 0;

    /* Everything passes, must be OK. */
    return 1;
}

/**
 * sum_weight() is a recursive function which calculates the weight
 * an object is carrying.  It goes through in figures out how much
 * containers are carrying, and sums it up.
 *
 * This takes into account the container's weight reduction.
 *
 * @param op
 * object we want the weight of.
 * @return
 * weight of this item and all its inventory.
 *
 * @note
 * The object's carrying field is updated.
 */
 /* TODO should check call this this are made a place where we really need reevaluaton of whole tree */
signed long sum_weight(object *op) {
    signed long sum;
    object *inv;
    for(sum = 0, inv = op->inv; inv != NULL; inv = inv->below) {
        if (inv->inv)
            sum_weight(inv);
        sum += inv->carrying + inv->weight * (inv->nrof ? inv->nrof : 1);
    }
    if (op->type == CONTAINER && op->stats.Str)
        sum = (sum * (100 - op->stats.Str))/100;
    op->carrying = sum;
    return sum;
}

/**
 * Utility function.
 * @param op
 * object we want the environment of. Can't be NULL.
 * @return
 * the outermost environment object for a given object. Will not be NULL.
 */
object *object_get_env_recursive (object *op) {
    while (op->env != NULL)
        op = op->env;
    return op;
}

/**
 * Finds the player carrying an object.
 *
 * @param op
 * item for which we want the carrier (player).
 * @return
 * the player, or NULL if not in an inventory.
 *
 * @todo
 * this function is badly named. Fix patching on the fly.
 */
object *get_player_container(object *op) { 
    for (;op!=NULL&&op->type!=PLAYER; op=op->env)
      /*TODO this is patching the structure on the flight as side effect. Shoudln't be needed in clean code */
      if (op->env==op)
        op->env = NULL;
    return op;
}

/**
 * Dumps an object.
 *
 * @param op
 * object to dump. Can be NULL.
 * @param buf
 * buffer that will contain object information. Must not be NULL.
 * @param size
 * buf's size.
 */
void dump_object(object *op, char* buf, int size) {
    char cp[HUGE_BUF];

    if(op==NULL) {
        snprintf(buf, size, "[NULL pointer]");
        return;
    }

    /*  object *tmp;*/

    if(op->arch!=NULL) {
        snprintf(buf, size, "arch %s\n", op->arch->name?op->arch->name:"(null)");

        get_ob_diff(op,&empty_archetype->clone, cp, HUGE_BUF);
        if(cp[0]!='\0')
            snprintf(buf + strlen(buf) , size - strlen(buf), cp);
        if (op->more) {
            snprintf(buf + strlen(buf), size - strlen(buf), "more %s\n", ltostr10(op->more->count));
        }
        if (op->head) {
            snprintf(buf + strlen(buf), size - strlen(buf), "head %s\n", ltostr10(op->head->count));
        }
        if (op->env) {
            snprintf(buf + strlen(buf), size - strlen(buf), "env %s\n", ltostr10(op->env->count));
        }
        if (op->inv) {
            snprintf(buf + strlen(buf), size - strlen(buf), "inv %s\n", ltostr10(op->inv->count));
        }
        if (op->owner) {
            snprintf(buf + strlen(buf), size - strlen(buf), "owner %s\n", ltostr10(op->owner->count));
        }
        snprintf(buf + strlen(buf), size - strlen(buf), "end\n");
    } else {
        snprintf(buf + strlen(buf), size - strlen(buf), "Object %s\nend\n", op->name==NULL ? "(null)" : op->name);
    }
}

/**
 * Dumps all objects to console.
 *
 * This is really verbose...Can be triggered by the dumpallobjects command while in DM mode.
 *
 * All objects are dumped to stderr (or alternate logfile, if in server-mode)
 */
void dump_all_objects(void) {
    object *op;
    char buf[HUGE_BUF];
    for(op=objects;op!=NULL;op=op->next) {
        dump_object(op, buf, sizeof(buf));
        LOG(llevDebug, "Object %d\n:%s\n", op->count, buf);
    }
}


/**
 * Returns the object which has the count-variable equal to the argument.
 *
 * @param i
 * tag.
 * @return
 * matching object, NULL if not found.
 */
object *find_object(tag_t i) {
    object *op;
    for(op=objects;op!=NULL;op=op->next)
        if(op->count==i)
            break;
    return op;
}

/**
 * Finds an object by name.
 *
 * Used only by the patch command, but not all that useful.
 * Enables features like "patch name-of-other-player food 999"
 *
 * @param str
 * name to search for. Must not be allocated by add_string().
 * @return
 * the first object which has a name equal to the argument.
 */
object *find_object_name(const char *str) {
    const char *name=add_string(str);
    object *op;
    for(op=objects;op!=NULL;op=op->next)
        if(op->name==name)
            break;
    free_string(name);
    return op;
}

/**
 * Destroys all allocated objects.
 *
 * @note
 * free() is called instead of free_object() as the object's memory has already by cleaned.
 *
 * @warning
 * this should be the last method called.
 */
void free_all_object_data(void) {
#ifdef MEMORY_DEBUG
    object *op, *next;

    for (op=free_objects; op!=NULL; ) {
        next=op->next;
        free(op);
        nrofallocobjects--;
        nroffreeobjects--;
        op=next;
    }

    for (op=objects; op!= NULL;) {
        next=op->next;
        if (!QUERY_FLAG(op, FLAG_FREED)) {
            LOG(llevDebug, "non freed object: %s", op->name);
        }
        op=next;
    }
#endif

    LOG(llevDebug,"%d allocated objects, %d free objects, STARMAX=%d\n", 
        nrofallocobjects, nroffreeobjects,STARTMAX);
}

/**
 * Returns the object which this object marks as being the owner.
 *
 * A id-scheme is used to avoid pointing to objects which have been
 * freed and are now reused.  If this is detected, the owner is
 * set to NULL, and NULL is returned.
 *
 * Changed 2004-02-12 - if the player is setting at the play again
 * prompt, he is removed, and we don't want to treat him as an owner of
 * anything, so check removed flag.  I don't expect that this should break
 * anything - once an object is removed, it is basically dead anyways.
 *
 * @param op
 * item to search owner of.
 * @return
 * owner, or NULL if not found.
 *
 * @todo
 * a side effect of this function is to clean owner chain for not existing anymore owner.
 * This is not the place to do such a cleaning
 */
object *get_owner(object *op) {
    if(op->owner==NULL)
        return NULL;

    if (!QUERY_FLAG(op->owner,FLAG_FREED) && !QUERY_FLAG(op->owner, FLAG_REMOVED) &&
        op->owner->count==op->ownercount)
        return op->owner;
    LOG(llevError,"I had to clean an owner when in get_owner, this isn't my job.\n");
    op->owner=NULL;
    op->ownercount=0;
    return NULL;
}

/**
 * Clears the owner of specified object.
 *
 * @param op
 * object we want to clear the owner of. Can be NULL.
 */
void clear_owner(object *op)
{
    if (!op) return;

    op->owner = NULL;
    op->ownercount = 0;
}



/**
 * Sets the owner and sets the skill and exp pointers to owner's current
 * skill and experience objects.
 *
 * @param op
 * object of which to set the owner
 * @param owner
 * new owner for object. Can be NULL, in which case it's equivalent of calling clear_owner(op)
 *
 * @todo
 * replace owner serching loop with a call to get_owner()?
 */
void set_owner (object *op, object *owner)
{
    if(op==NULL)
        return;
    if (owner == NULL) {
        clear_owner(op);
        return;
    }

    /* next line added to allow objects which own objects */ 
    /* Add a check for ownercounts in here, as I got into an endless loop
     * with the fireball owning a poison cloud which then owned the
     * fireball.  I believe that was caused by one of the objects getting
     * freed and then another object replacing it.  Since the ownercounts
     * didn't match, this check is valid and I believe that cause is valid.
     */
    while (owner->owner && owner!=owner->owner && 
        owner->ownercount==owner->owner->count) owner=owner->owner;

    /* IF the owner still has an owner, we did not resolve to a final owner.
     * so lets not add to that.
     */
    if (owner->owner) {
        LOG(llevError,"owner id %d could not be resolved to a parent owner in set_owner(). This is bad!"
            "owner=%p owner->owner=%p owner->ownercount=%d owner->owner->count=%d\n",
            owner,owner->owner,owner->ownercount, owner->owner->count);
        return;
    }

    op->owner=owner;

    op->ownercount=owner->count;
}

/**
 * Set the owner to clone's current owner and set the skill and experience
 * objects to clone's objects (typically those objects that where the owner's
 * current skill and experience objects at the time when clone's owner was
 * set - not the owner's current skill and experience objects).
 *
 * Use this function if player created an object (e.g. fire bullet, swarm
 * spell), and this object creates further objects whose kills should be
 * accounted for the player's original skill, even if player has changed
 * skills meanwhile.
 *
 * @param op
 * object to update.
 * @param clone
 * object from which to get the owner.
 */
void copy_owner (object *op, object *clone)
{
    object *owner = get_owner (clone);
    if (owner == NULL) {
        /* players don't have owners - they own themselves.  Update
         * as appropriate.
         */
        /*TODO owner=self is dangerous and should be avoided*/
        if (clone->type == PLAYER) owner=clone;
        else return;
        }
    set_owner(op, owner);

}

/**
 * Sets to 0 vital variables in an object.
 *
 * @param op
 * object to reset.
 *
 * @note
 * this doesn't free associated memory for object.
 */
void reset_object(object *op) {

    op->name = NULL;
    op->name_pl = NULL;
    op->title = NULL;
    op->race = NULL;
    op->slaying = NULL;
    op->skill = NULL;
    op->msg = NULL;
    op->materialname = NULL;
    op->lore = NULL;
    clear_object(op);
}

/**
 * Zero the key_values on op, decrementing the shared-string
 * refcounts and freeing the links.
 *
 * @param op
 * object to clear.
 */
void free_key_values(object * op) {
    key_value * i;
    key_value * next = NULL;

    if (op->key_values == NULL) return;

    for (i = op->key_values; i != NULL; i = next) {
        /* Store next *first*. */
        next = i->next; 

        if (i->key) FREE_AND_CLEAR_STR(i->key);
        if (i->value) FREE_AND_CLEAR_STR(i->value);
        i->next = NULL;
        free(i);
    }

    op->key_values = NULL;
}


/**
 * Frees everything allocated by an object, and also
 * clears all variables and flags to default settings.
 *
 * @param op
 * object to clear
 */
void clear_object(object *op) {

    /*TODO this comment must be investigated*/
    /* redo this to be simpler/more efficient. Was also seeing
     * crashes in the old code.  Move this to the top - am
     * seeing periodic crashes in this code, and would like to have
     * as much info available as possible (eg, object name).
     */
    free_key_values(op);

    /* the memset will clear all these values for us, but we need
     * to reduce the refcount on them.
     */
    if (op->name!=NULL)	    FREE_AND_CLEAR_STR(op->name);
    if (op->name_pl!=NULL)  FREE_AND_CLEAR_STR(op->name_pl);
    if (op->title != NULL)  FREE_AND_CLEAR_STR(op->title);
    if (op->race!=NULL)	    FREE_AND_CLEAR_STR(op->race);
    if (op->slaying!=NULL)  FREE_AND_CLEAR_STR(op->slaying);
    if (op->skill!=NULL)    FREE_AND_CLEAR_STR(op->skill);
    if (op->msg!=NULL)	    FREE_AND_CLEAR_STR(op->msg);
    if (op->lore!=NULL)	    FREE_AND_CLEAR_STR(op->lore);
    if (op->materialname!= NULL) FREE_AND_CLEAR_STR(op->materialname);


    memset((void*)((char*)op + offsetof(object, name)),
		   0, sizeof(object)-offsetof(object, name));
    /* Below here, we clear things that are not done by the memset,
     * or set default values that are not zero.
     */
    /* This is more or less true */
    SET_FLAG(op, FLAG_REMOVED);


    op->contr = NULL;
    op->below=NULL;
    op->above=NULL;
    op->inv=NULL;
    op->container=NULL;
    op->env=NULL;
    op->more=NULL;
    op->head=NULL;
    op->map=NULL;
    op->active_next = NULL;
    op->active_prev = NULL;
    /* What is not cleared is next, prev, and count */

    op->expmul=1.0;
    op->face = blank_face;
    op->attacked_by_count= -1;
    if (settings.casting_time)
        op->casting_time = -1;

}

/**
 * Copy object first frees everything allocated by the second object,
 * and then copies the contents of the first object into the second
 * object, allocating what needs to be allocated.  Basically, any
 * data that is malloc'd needs to be re-malloc/copied.  Otherwise,
 * if the first object is freed, the pointers in the new object
 * will point at garbage.
 *
 * @param op2
 * object that we copy.from
 * @param op
 * object that we copy to.
 */

void copy_object(object *op2, object *op) {
    int is_freed=QUERY_FLAG(op,FLAG_FREED),is_removed=QUERY_FLAG(op,FLAG_REMOVED);

    /* Decrement the refcounts, but don't bother zeroing the fields;
    they'll be overwritten by memcpy. */
    if(op->name!=NULL)			free_string(op->name);
    if(op->name_pl!=NULL)		free_string(op->name_pl);
    if(op->title!=NULL)			free_string(op->title);
    if(op->race!=NULL)			free_string(op->race);
    if(op->slaying!=NULL)		free_string(op->slaying);
    if(op->skill!=NULL)			free_string(op->skill);
    if(op->msg!=NULL)			free_string(op->msg);
    if(op->lore!=NULL)			free_string(op->lore);
    if(op->materialname != NULL)	free_string(op->materialname);
    if(op->custom_name != NULL)		free_string(op->custom_name);
    if(op->discrete_damage != NULL)	FREE_AND_CLEAR(op->discrete_damage);
    if(op->spell_tags != NULL)		FREE_AND_CLEAR(op->spell_tags);

    /* Basically, same code as from clear_object() */

    free_key_values(op);

    /* op is the destination, op2 is the source. */
    (void) memcpy((void *)((char *) op +offsetof(object,name)),
                (void *)((char *) op2+offsetof(object,name)),
                sizeof(object)-offsetof(object, name));

    if(is_freed)	    SET_FLAG(op,FLAG_FREED);
    if(is_removed)	    SET_FLAG(op,FLAG_REMOVED);
    if(op->name!=NULL)	    add_refcount(op->name);
    if(op->name_pl!=NULL)   add_refcount(op->name_pl);
    if(op->title!=NULL)	    add_refcount(op->title);
    if(op->race!=NULL)	    add_refcount(op->race);
    if(op->slaying!=NULL)   add_refcount(op->slaying);
    if(op->skill!=NULL)	    add_refcount(op->skill);
    if(op->lore!=NULL)	    add_refcount(op->lore);
    if(op->msg!=NULL)	    add_refcount(op->msg);
    if(op->custom_name!=NULL)	    add_refcount(op->custom_name);
    if (op->materialname != NULL)   add_refcount(op->materialname);
    if (op->discrete_damage != NULL) {
        op->discrete_damage = malloc(sizeof(sint16) * NROFATTACKS);
        memcpy(op->discrete_damage, op2->discrete_damage, sizeof(sint16) * NROFATTACKS);
    }

    if (op->spell_tags != NULL) {
        op->spell_tags = malloc(sizeof(tag_t) * SPELL_TAG_SIZE);
        memcpy(op->spell_tags, op2->spell_tags, sizeof(tag_t) * SPELL_TAG_SIZE);
    }

    /* If archetype is a temporary one, we need to update reference count, because
     * that archetype will be freed by free_object when the last object is removed.
     */
    if (op->arch->reference_count > 0)
        op->arch->reference_count++;

    if (op2->speed<0)
        op->speed_left=op2->speed_left-RANDOM()%200/100.0;

    /* Copy over key_values, if any. */
    if (op2->key_values != NULL) {
        key_value * tail = NULL;
        key_value * i;

        op->key_values = NULL;

        for (i = op2->key_values; i != NULL; i = i->next) {
            key_value * new_link = malloc(sizeof(key_value));

            new_link->next = NULL;
            new_link->key = add_refcount(i->key);
            if (i->value) 
                new_link->value = add_refcount(i->value);
            else
                new_link->value = NULL;

            /* Try and be clever here, too. */
            if (op->key_values == NULL) {
                op->key_values = new_link;
                tail = new_link;
            } else {
                tail->next = new_link;
                tail = new_link;
            }
        }
    }

    update_ob_speed(op);
}

/**
 * copy an object with an inventory...  i.e., duplicate the inv too.
 * @param src_ob
 * object to copy.
 * @param dest_ob
 * where to copy.
 * @todo
 * replace with a function in common library (there is certainly one).
 */
void copy_object_with_inv(object *src_ob, object *dest_ob) {
    object *walk,*tmp;

    copy_object(src_ob,dest_ob);

    for(walk=src_ob->inv;walk!=NULL;walk=walk->below) {
        tmp=get_object();
        copy_object(walk,tmp);
        insert_ob_in_ob(tmp,dest_ob);
    }
}

/**
 * Allocates more objects for the list of unused objects.
 *
 * It is called from get_object() if the unused list is empty.
 *
 * If there is not enough memory, fatal() is called.
 */
static void expand_objects(void) {
    int i;
    object *new;
    new = (object *) CALLOC(OBJ_EXPAND,sizeof(object));

    if(new==NULL)
        fatal(OUT_OF_MEMORY);
    free_objects=new;
    new[0].prev=NULL;
    new[0].next= &new[1],
    SET_FLAG(&(new[0]), FLAG_REMOVED);
    SET_FLAG(&(new[0]), FLAG_FREED);

    for(i=1;i<OBJ_EXPAND-1;i++) {
        new[i].next= &new[i+1],
        new[i].prev= &new[i-1],
        SET_FLAG(&(new[i]), FLAG_REMOVED);
        SET_FLAG(&(new[i]), FLAG_FREED);
    }
    new[OBJ_EXPAND-1].prev= &new[OBJ_EXPAND-2],
    new[OBJ_EXPAND-1].next=NULL,
    SET_FLAG(&(new[OBJ_EXPAND-1]), FLAG_REMOVED);
    SET_FLAG(&(new[OBJ_EXPAND-1]), FLAG_FREED);

    nrofallocobjects += OBJ_EXPAND;
    nroffreeobjects += OBJ_EXPAND;
}

/**
 * Grabs an object from the list of unused objects, makes
 * sure it is initialised, and returns it.
 *
 * If there are no free objects, expand_objects() is called to get more.
 *
 * @return
 * cleared and ready to use object*.
 *
 * @note
 * will never fail, as expand_objects() will fatal() if memory allocation error.
 */
object *get_object(void) {
    object *op;

    if(free_objects==NULL) {
        expand_objects();
    }
    op=free_objects;
#ifdef MEMORY_DEBUG
    /* The idea is hopefully by doing a realloc, the memory
    * debugging program will now use the current stack trace to
    * report leaks.
    */
    op = realloc(op, sizeof(object));
    SET_FLAG(op, FLAG_REMOVED);
    SET_FLAG(op, FLAG_FREED);
#endif

    if(!QUERY_FLAG(op,FLAG_FREED)) {
        LOG(llevError,"Fatal: Getting busy object.\n");
    }
    free_objects=op->next;
    if(free_objects!=NULL)
        free_objects->prev=NULL;
    op->count= ++ob_count;
    op->name=NULL;
    op->name_pl=NULL;
    op->title=NULL;
    op->race=NULL;
    op->slaying=NULL;
    op->skill = NULL;
    op->lore=NULL;
    op->msg=NULL;
    op->materialname=NULL;
    op->next=objects;
    op->prev=NULL;
    op->active_next = NULL;
    op->active_prev = NULL;
    op->discrete_damage = NULL;
    op->spell_tags=NULL;
    if(objects!=NULL)
        objects->prev=op;
    objects=op;
    clear_object(op);
    SET_FLAG(op,FLAG_REMOVED);
    nroffreeobjects--;
    return op;
}

/**
 * If an object with the IS_TURNABLE() flag needs to be turned due
 * to the closest player being on the other side, this function can
 * be called to update the face variable, _and_ how it looks on the map.
 *
 * @param op
 * object to update.
 */
void update_turn_face(object *op) {
    if(!QUERY_FLAG(op,FLAG_IS_TURNABLE)||op->arch==NULL)
        return;
    SET_ANIMATION(op, op->direction);
    update_object(op,UP_OBJ_FACE);
}

/**
 * Updates the speed of an object.  If the speed changes from 0 to another
 * value, or vice versa, then add/remove the object from the active list.
 * This function needs to be called whenever the speed of an object changes.
 *
 * @param op
 * object to update. Must not be freed and still have a speed.
 *
 * @todo
 * check fixme & todo
 */
void update_ob_speed(object *op) {
    /* FIXME what the hell is this crappy hack?*/
    extern int arch_init;

    /* No reason putting the archetypes objects on the speed list,
     * since they never really need to be updated.
     */

    if (QUERY_FLAG(op, FLAG_FREED) && op->speed) {
        LOG(llevError,"Object %s is freed but has speed.\n", op->name);
#ifdef MANY_CORES
        abort();
#else
        op->speed = 0;
#endif
    }
    if (arch_init) {
        return;
    }
    if (FABS(op->speed)>MIN_ACTIVE_SPEED) {
        /* If already on active list, don't do anything */
        /* TODO this check can probably be simplified a lot */
        if (op->active_next || op->active_prev || op==active_objects)
            return;

        /* process_events() expects us to insert the object at the beginning
         * of the list. */
        op->active_next = active_objects;
        if (op->active_next!=NULL)
            op->active_next->active_prev = op;
        active_objects = op;
    } else {
        /* If not on the active list, nothing needs to be done */
        if (!op->active_next && !op->active_prev && op!=active_objects)
            return;

        if (op->active_prev==NULL) {
            active_objects = op->active_next;
            if (op->active_next!=NULL)
                op->active_next->active_prev = NULL;
        } else {
            op->active_prev->active_next = op->active_next;
            if (op->active_next)
                op->active_next->active_prev = op->active_prev;
        }
        op->active_next = NULL;
        op->active_prev = NULL;
    }
}

/**
 * This function removes object 'op' from the list of active
 * objects.
 * This should only be used for style maps or other such 
 * reference maps where you don't want an object that isn't
 * in play chewing up cpu time getting processed.
 * The reverse of this is to call update_ob_speed, which
 * will do the right thing based on the speed of the object.
 *
 * @param op
 * object to remove.
 */
void remove_from_active_list(object *op)
{
    /* If not on the active list, nothing needs to be done */
    if (!op->active_next && !op->active_prev && op!=active_objects)
        return;

    if (op->active_prev==NULL) {
        active_objects = op->active_next;
    if (op->active_next!=NULL)
        op->active_next->active_prev = NULL;
    } else {
        op->active_prev->active_next = op->active_next;
        if (op->active_next)
            op->active_next->active_prev = op->active_prev;
    }
    op->active_next = NULL;
    op->active_prev = NULL;
}

/**
 * update_object() updates the array which represents the map.
 *
 * It takes into account invisible objects (and represent squares covered
 * by invisible objects by whatever is below them (unless it's another
 * invisible object, etc...)
 *
 * If the object being updated is beneath a player, the look-window
 * of that player is updated (this might be a suboptimal way of
 * updating that window, though, since update_object() is called _often_)
 *
 * @param op
 * object to update
 * @param action
 * Hint of what the caller believes need to be done.
 * For example, if the only thing that has changed is the face (due to
 * an animation), we don't need to call update_position until that actually
 * comes into view of a player.  OTOH, many other things, like addition/removal
 * of walls or living creatures may need us to update the flags now.
 * current action are:
 * @li ::UP_OBJ_INSERT: op was inserted
 * @li ::UP_OBJ_REMOVE: op was removed
 * @li ::UP_OBJ_CHANGE: object has somehow changed.  In this case, we always update
 *  as that is easier than trying to look at what may have changed.
 * @li ::UP_OBJ_FACE: only the objects face has changed.
 *
 * @todo
 * this function should be renamed to something like update_object_map, update_object is a too general term
 * Also it might be worth moving it to map.c
 */
void update_object(object *op, int action) {
    int update_now=0, flags;
    MoveType move_on, move_off, move_block, move_slow;

    if (op == NULL) {
        /* this should never happen */
        LOG(llevDebug,"update_object() called for NULL object.\n");
        return;
    }

    if(op->env!=NULL) {
        /* Animation is currently handled by client, so nothing
        * to do in this case.
        */
        return;
    }

    /* If the map is saving, don't do anything as everything is
     * going to get freed anyways.
     */
    if (!op->map || op->map->in_memory == MAP_SAVING) return;

    /* make sure the object is within map boundaries */
    if (op->x < 0 || op->x >= MAP_WIDTH(op->map) ||
        op->y < 0 || op->y >= MAP_HEIGHT(op->map)) {
        LOG(llevError,"update_object() called for object out of map!\n");
#ifdef MANY_CORES
        abort();
#endif
        return;
    }

    flags = GET_MAP_FLAGS(op->map, op->x, op->y);
    SET_MAP_FLAGS(op->map, op->x, op->y, flags | P_NEED_UPDATE);
    move_slow = GET_MAP_MOVE_SLOW(op->map, op->x, op->y);
    move_on = GET_MAP_MOVE_ON(op->map, op->x, op->y);
    move_block = GET_MAP_MOVE_BLOCK(op->map, op->x, op->y);
    move_off = GET_MAP_MOVE_OFF(op->map, op->x, op->y);

    if (action == UP_OBJ_INSERT) {
        if (QUERY_FLAG(op, FLAG_BLOCKSVIEW) && !(flags & P_BLOCKSVIEW))
            update_now=1;

        if (QUERY_FLAG(op, FLAG_NO_MAGIC) && !(flags & P_NO_MAGIC))
            update_now=1;

        if (QUERY_FLAG(op, FLAG_DAMNED) && !(flags & P_NO_CLERIC))
            update_now=1;

        if (QUERY_FLAG(op, FLAG_ALIVE) && !(flags & P_IS_ALIVE))
            update_now=1;

        if ((move_on | op->move_on) != move_on) update_now=1;
        if ((move_off | op->move_off) != move_off) update_now=1;
        /* This isn't perfect, but I don't expect a lot of objects to
         * to have move_allow right now.    
         */
        if (((move_block | op->move_block) & ~op->move_allow) != move_block) 
            update_now=1;
        if ((move_slow | op->move_slow) != move_slow) update_now=1;
    }
    /* if the object is being removed, we can't make intelligent
     * decisions, because remove_ob can't really pass the object
     * that is being removed.
     */
    else if (action == UP_OBJ_CHANGE || action == UP_OBJ_REMOVE) {
        update_now=1;
    } else if (action == UP_OBJ_FACE) {
        /* Nothing to do for that case */
    }
    else {
        LOG(llevError,"update_object called with invalid action: %d\n", action);
    }

    if (update_now) {
        SET_MAP_FLAGS(op->map, op->x, op->y, flags | P_NO_ERROR | P_NEED_UPDATE);
        update_position(op->map, op->x, op->y);
    }

    if(op->more!=NULL)
        update_object(op->more, action);
}


/**
 * Frees everything allocated by an object, removes
 * it from the list of used objects, and puts it on the list of
 * free objects.  The IS_FREED() flag is set in the object.
 * The object must have been removed by remove_ob() first for
 * this function to succeed.
 *
 * Inventory will be dropped on the ground if in a map, else freed too.
 *
 * @param ob
 * object to free. Will become invalid when function returns.
 */
void free_object(object *ob) {
  free_object2(ob, 0);
}

/**
 * Frees everything allocated by an object, removes
 * it from the list of used objects, and puts it on the list of
 * free objects.  The IS_FREED() flag is set in the object.
 *
 * The object must have been removed by remove_ob() first for
 * this function to succeed.
 *
 * @param ob
 * object to free. Will become invalid when function returns.
 * @param free_inventory
 * if set, free inventory as well. Else drop items in inventory to the ground.
 *
 * @warning
 * the object's archetype should be a valid pointer, or NULL.
 */
static void free_object2(object *ob, int free_inventory) {
    object *tmp,*op;

    if (!QUERY_FLAG(ob,FLAG_REMOVED)) {
        char buf[HUGE_BUF];
        LOG(llevDebug,"Free object called with non removed object\n");
        dump_object(ob, buf, sizeof(buf));
        LOG(llevError, buf);
#ifdef MANY_CORES
        abort();
#endif
    }
    if(QUERY_FLAG(ob,FLAG_FRIENDLY)) {
        LOG(llevMonster,"Warning: tried to free friendly object.\n");
        remove_friendly_object(ob);
    }
    if(QUERY_FLAG(ob,FLAG_FREED)) {
        char buf[HUGE_BUF];
        dump_object(ob, buf, sizeof(buf));
        LOG(llevError,"Trying to free freed object.\n%s\n",buf);
        return;
    }

    /* Handle for plugin destroy event */
    execute_event(ob, EVENT_DESTROY,NULL,NULL,NULL,SCRIPT_FIX_NOTHING);

    if (ob->inv) {
        /* Only if the space blocks everything do we not process -
         * if some form of movemnt is allowed, let objects
         * drop on that space.
         */
        if (free_inventory || ob->map==NULL || ob->map->in_memory!=MAP_IN_MEMORY ||
            (GET_MAP_MOVE_BLOCK(ob->map, ob->x, ob->y) == MOVE_ALL)) {
            op=ob->inv;
            while(op!=NULL) {
                tmp=op->below;
                remove_ob(op);
                free_object2(op, free_inventory);
                op=tmp;
            }
        } else { /* Put objects in inventory onto this space */
            op=ob->inv;
            while(op!=NULL) {
                tmp=op->below;
                remove_ob(op);
                if(QUERY_FLAG(op,FLAG_STARTEQUIP)||QUERY_FLAG(op,FLAG_NO_DROP) ||
                   op->type==RUNE || op->type==TRAP || QUERY_FLAG(op,FLAG_IS_A_TEMPLATE))
                    free_object(op);
                else {
                    object *part;
                    /* If it's a multi-tile object, scatter dropped items randomly */
                    if (ob->more) {
                        int partcount = 0;
                        /* Get the number of non-head parts */
                        for(part = ob; part; part = part->more) {
                            partcount++;
                        }
                        /* Select a random part */
                        partcount = RANDOM()%partcount;
                        for(part = ob; partcount>0; partcount--) {
                            part = part->more;
                        }
                    } else {
                        part = ob;
                    }

                    if ( QUERY_FLAG(op,FLAG_ALIVE) ) {
                        int pos;

                        pos = find_free_spot(op,part->map,part->x, part->y, 0, SIZEOFFREE);
                        if ( pos == -1 )
                            free_object(op);
                        else {
                            op->x=part->x + freearr_x[ pos ];
                            op->y=part->y + freearr_y[ pos ];
                            insert_ob_in_map(op,part->map,NULL,0); /* Insert in same map as the envir */
                        }
                    }
                    else {
                        op->x=part->x;
                        op->y=part->y;
                        insert_ob_in_map(op,part->map,NULL,0); /* Insert in same map as the envir */
                    }
                }
                op=tmp;
            }
        }
    }

    if(ob->more!=NULL) {
        free_object2(ob->more, free_inventory);
        ob->more=NULL;
    }

    /* Remove object from the active list */
    ob->speed = 0;
    update_ob_speed(ob);

    SET_FLAG(ob, FLAG_FREED);
    ob->count = 0;

    /* Remove this object from the list of used objects */
    if(ob->prev==NULL) {
        objects=ob->next;
        if(objects!=NULL)
            objects->prev=NULL;
    } else {
        ob->prev->next=ob->next;
        if(ob->next!=NULL)
            ob->next->prev=ob->prev;
    }

    if(ob->name!=NULL)	    FREE_AND_CLEAR_STR(ob->name);
    if(ob->name_pl!=NULL)   FREE_AND_CLEAR_STR(ob->name_pl);
    if(ob->title!=NULL)	    FREE_AND_CLEAR_STR(ob->title);
    if(ob->race!=NULL)	    FREE_AND_CLEAR_STR(ob->race);
    if(ob->slaying!=NULL)   FREE_AND_CLEAR_STR(ob->slaying);
    if(ob->skill!=NULL)	    FREE_AND_CLEAR_STR(ob->skill);
    if(ob->lore!=NULL)	    FREE_AND_CLEAR_STR(ob->lore);
    if(ob->msg!=NULL)	    FREE_AND_CLEAR_STR(ob->msg);
    if(ob->materialname!=NULL) FREE_AND_CLEAR_STR(ob->materialname);
    if(ob->discrete_damage != NULL) FREE_AND_CLEAR(ob->discrete_damage);
    if (ob->spell_tags)	    FREE_AND_CLEAR(ob->spell_tags);

    /* Why aren't events freed? */
    free_key_values(ob);

    /* Test whether archetype is a temporary one, and if so look whether it should be trashed. */
    if (ob->arch && ob->arch->reference_count > 0) {
        if (--ob->arch->reference_count == 0) {
            free_arch(ob->arch);
        }
    }

    /* Now link it with the free_objects list: */
    ob->prev=NULL;
    ob->next=free_objects;
    if(free_objects!=NULL)
        free_objects->prev=ob;
    free_objects=ob;
    nroffreeobjects++;
}

/**
 * Objects statistics.
 *
 * @return
 * number of objects on the list of free objects.
 */
int count_free(void) {
    int i=0;
    object *tmp=free_objects;
    while(tmp!=NULL)
        tmp=tmp->next, i++;
    return i;
}

/**
 * Object statistics.
 *
 * @return
 * number of objects on the list of used objects.
 */
int count_used(void) {
    int i=0;
    object *tmp=objects;
    while(tmp!=NULL)
        tmp=tmp->next, i++;
    return i;
}

/**
 * Objects statistics.
 *
 * @return
 * number of objects on the list of active objects.
 */
int count_active(void) {
    int i=0;
    object *tmp=active_objects;
    while(tmp!=NULL)
        tmp=tmp->active_next, i++;
    return i;
}

/**
 * Recursively (outwards) subtracts a number from the
 * weight of an object (and what is carried by it's environment(s)).
 *
 * Takes into account the container's weight reduction.
 *
 * @param op
 * object to which weight is substracted.
 * @param weight
 * weight to remove.
 *
 * @todo
 * check if not mergeable with add_weight().
 */
void sub_weight (object *op, signed long weight) {
    while (op != NULL) {
        if (op->type == CONTAINER) {
            weight=(signed long)(weight*(100-op->stats.Str)/100);
        }
        op->carrying-=weight;
        op = op->env;
    }
}

/**
 * This function removes the object op from the linked list of objects
 * which it is currently tied to.  When this function is done, the
 * object will have no environment.  If the object previously had an
 * environment, the x and y coordinates will be updated to
 * the previous environment.
 *
 * @param op
 * object to remove. Must not be removed yet, else abort() is called.
 *
 * @todo
 * this function is a piece of overbloated crap or at lest
 * look like need cleanup it does to much different things.
 */
void remove_ob(object *op) {
    object *tmp,*last=NULL;
    object *otmp;
    tag_t tag;
    int check_walk_off;
    mapstruct *m;
    sint16 x,y;

    if(QUERY_FLAG(op,FLAG_REMOVED)) {
        char buf[HUGE_BUF];
        dump_object(op, buf, sizeof(buf));
        LOG(llevError,"Trying to remove removed object.\n%s\n",buf);
        abort();
    }
    if(op->more!=NULL)
        remove_ob(op->more);

    SET_FLAG(op, FLAG_REMOVED);

  /*
   * In this case, the object to be removed is in someones
   * inventory.
   */
  /* TODO try to call a generic inventory weight adjusting function like sub_weight */
    if(op->env!=NULL) {
        if(op->nrof)
        sub_weight(op->env, op->weight*op->nrof);
        else
        sub_weight(op->env, op->weight+op->carrying);

        /* NO_FIX_PLAYER is set when a great many changes are being
         * made to players inventory.  If set, avoiding the call
         * to save cpu time.
         */
        if ((otmp=get_player_container(op->env))!=NULL && otmp->contr && 
            !QUERY_FLAG(otmp,FLAG_NO_FIX_PLAYER))
            fix_object(otmp);

        if(op->above!=NULL)
            op->above->below=op->below;
        else
            op->env->inv=op->below;

        if(op->below!=NULL)
        op->below->above=op->above;

        /* we set up values so that it could be inserted into
         * the map, but we don't actually do that - it is up
         * to the caller to decide what we want to do.
         */
        op->x=op->env->x;
        op->y=op->env->y;
        op->ox=op->x;
        op->oy=op->y;
        op->map=op->env->map;
        op->above=NULL;
        op->below=NULL;
        op->env=NULL;
        return;
    }

    /* If we get here, we are removing it from a map */
    if (op->map == NULL) return;

    x = op->x;
    y = op->y;
    m = get_map_from_coord(op->map, &x, &y);

    if (!m) {
        LOG(llevError,"remove_ob called when object was on map but appears to not be within valid coordinates? %s (%d,%d)\n",
        op->map->path, op->x, op->y);
        abort();
  }
    if (op->map != m) {
        LOG(llevError,"remove_ob: Object not really on map it claimed to be on? %s != %s, %d,%d != %d,%d\n",
        op->map->path, m->path, op->x, op->y, x, y);
  }

  /* link the object above us */
    if (op->above)
        op->above->below=op->below;
    else
        SET_MAP_TOP(m,x,y,op->below); /* we were top, set new top */

    /* Relink the object below us, if there is one */
    if(op->below) {
        op->below->above=op->above;
    } else {
        /* Nothing below, which means we need to relink map object for this space 
         * use translated coordinates in case some oddness with map tiling is
         * evident
         */
        /*TODO is this check really needed?*/
        if(GET_MAP_OB(m,x,y)!=op) {
            char buf[HUGE_BUF];
            dump_object(op, buf, sizeof(buf));
            LOG(llevError,"remove_ob: GET_MAP_OB does not return object to be removed even though it appears to be on the bottom?\n%s\n", buf);
            dump_object(GET_MAP_OB(m,x,y), buf, sizeof(buf));
            LOG(llevError,"%s\n",buf);
        }
        SET_MAP_OB(m,x,y,op->above);  /* goes on above it. */
    }
    op->above=NULL;
    op->below=NULL;

    if (op->map->in_memory == MAP_SAVING)
        return;

    tag = op->count;
    check_walk_off = ! QUERY_FLAG (op, FLAG_NO_APPLY);
    for(tmp=GET_MAP_OB(m,x,y);tmp!=NULL;tmp=tmp->above) {
        /* No point updating the players look faces if he is the object
         * being removed.
         */

        if(tmp->type==PLAYER && tmp!=op) {
            /* If a container that the player is currently using somehow gets
             * removed (most likely destroyed), update the player view
             * appropriately.
            */
            if (tmp->container==op) {
                CLEAR_FLAG(op, FLAG_APPLIED);
                tmp->container=NULL;
            }
            tmp->contr->socket.update_look=1;
        }
        /* See if player moving off should effect something */
        if (check_walk_off && ((op->move_type & tmp->move_off) &&
            (op->move_type & ~tmp->move_off & ~tmp->move_block)==0)) {

            ob_move_on(tmp, op, NULL);
            if (was_destroyed (op, tag)) {
                LOG (llevError, "BUG: remove_ob(): name %s, archname %s destroyed "
                    "leaving object\n", tmp->name, tmp->arch->name);
            }
        }

        /* Eneq(@csd.uu.se): Fixed this to skip tmp->above=tmp */
        if(tmp->above == tmp)
            tmp->above = NULL;
        last=tmp;
    }
    /* last == NULL or there are no objects on this space */
    if (last==NULL) {
        /* set P_NEED_UPDATE, otherwise update_position will complain.  In theory,
        * we could preserve the flags (GET_MAP_FLAGS), but update_position figures
        * those out anyways, and if there are any flags set right now, they won't
        * be correct anyways.
        */
        SET_MAP_FLAGS(op->map, op->x, op->y,  P_NEED_UPDATE);
        update_position(op->map, op->x, op->y);
    }
    else
        update_object(last, UP_OBJ_REMOVE);

    if(QUERY_FLAG(op,FLAG_BLOCKSVIEW)|| (op->glow_radius != 0))
        update_all_los(op->map, op->x, op->y);
}

/**
 * This function goes through all objects below and including top, and
 * merges op to the first matching object.
 *
 * @param op
 * object to merge.
 * @param top
 * from which item to merge. If NULL, it is calculated.
 * @return
 * pointer to object if it succeded in the merge, otherwise NULL
 */
object *merge_ob(object *op, object *top) {
    if(!op->nrof)
        return 0;
    if(top==NULL)
        for(top=op;top!=NULL&&top->above!=NULL;top=top->above);
    for(;top!=NULL;top=top->below) {
        if(top==op)
            continue;
        if (can_merge(op,top)) {
            top->nrof+=op->nrof;
            op->weight = 0; /* Don't want any adjustements now */
            remove_ob(op);
            free_object(op);
            return top;
        }
    }
    return NULL;
}

/**
 * Same as insert_ob_in_map() except it handle separate coordinates and do a clean
 * job preparing multi-part monsters.
 *
 * @param op
 * object to insert.
 * @param m
 * map to insert into.
 * @param originator
 *
 * @param flag
 *
 * @param x
 * @param y
 * coordinates to insert at.
 */
object *insert_ob_in_map_at(object *op, mapstruct *m, object *originator, int flag, int x, int y){
    object* tmp;
    if (op->head)
        op=op->head;
    for (tmp=op;tmp;tmp=tmp->more) {
        tmp->x=x+tmp->arch->clone.x;
        tmp->y=y+tmp->arch->clone.y;
        if (op != tmp && !tmp->map)
            tmp->map = op->map ? op->map : m;
    }
    return insert_ob_in_map (op, m, originator, flag);
}


/**
 * This sees if there are any objects on the space that can
 * merge with op.  Note that op does not need to actually
 * be inserted on the map (when called from insert_ob_in_map,
 * it won't be), but op->map should be set correctly.
 *
 * Note that even if we find a match on the space, we keep progressing
 * looking for more.  This is because op->range is set to 0 in
 * explosion, so what may not have been mergable now is.
 *
 * @param op
 * object to try to merge into.
 *
 * @param x
 * @param y
 * coordinates to look at for merging.
 */
void merge_spell(object *op, sint16 x, sint16 y)
{
    object *tmp, *above;
    int i;

    /* We try to do some merging of spell objects - if something has same owner,
     * is same type of spell, and going in the same direction, it is somewhat
     * mergable.
     *
     * If the spell object has an other_arch, don't merge - when the spell
     * does something, like explodes, it will use this other_arch, and
     * if we merge, there is no easy way to make the correct values be
     * set on this new object (values should be doubled, tripled, etc.)
     *
     * We also care about speed - only process objects that will not be
     * active this tick.  Without this, the results are incorrect - think
     * of a case where tmp would normally get processed this tick, but
     * get merges with op, which does not get processed.
     */
    for (tmp=GET_MAP_OB(op->map,x,y);tmp!=NULL;tmp=above) {
	above = tmp->above;

	if (op->type == tmp->type && 
	    op->subtype == tmp->subtype &&
	    op->direction == tmp->direction && 
	    op->owner == tmp->owner && op->ownercount == tmp->ownercount &&
	    op->range == tmp->range &&
	    op->stats.wc == tmp->stats.wc && op->level == tmp->level && 
	    op->attacktype == tmp->attacktype && op->speed == tmp->speed &&
	    !tmp->other_arch && (tmp->speed_left +tmp->speed) < 0.0 &&
	    op!=tmp) {


	    /* Quick test - if one or the other objects already have hash tables
	     * set up, and that hash bucket contains a value that doesn't
	     * match what we want to set it up, we won't be able to merge.
	     * Note that these two if statements are the same, except
	     * for which object they are checking against.  They could
	     * be merged, but the line wrapping would be large enough
	     * that IMO it would become difficult to read the different clauses
	     * so its cleaner just to do 2 statements - MSW
	     */
	    if (op->spell_tags && !OB_SPELL_TAG_MATCH(op, tmp->stats.maxhp) && 
		OB_SPELL_TAG_HASH(op, tmp->stats.maxhp) != 0) continue;

	    if (tmp->spell_tags && !OB_SPELL_TAG_MATCH(tmp, op->stats.maxhp) && 
		OB_SPELL_TAG_HASH(tmp, op->stats.maxhp) != 0) continue;

	    /* If we merge, the data from tmp->spell_tags gets copied into op.
	     * so we need to make sure that slot isn't filled up.
	     */
	    if (tmp->spell_tags && !OB_SPELL_TAG_MATCH(tmp, tmp->stats.maxhp) &&
		OB_SPELL_TAG_HASH(tmp, tmp->stats.maxhp) != 0) continue;

	    /* If both objects have spell_tags, we need to see if there are conflicting
	     * values - if there are, we won't be able to merge then.
	     */
	    if (tmp->spell_tags && op->spell_tags) {
		int need_copy=0;

		for (i=0; i< SPELL_TAG_SIZE; i++) {
		    /* If the two tag values in the hash are set, but are
		     * not set to the same value, then these objects
		     * can not be merged.
		     */
		    if (op->spell_tags[i] && tmp->spell_tags[i] && 
			op->spell_tags[i] != tmp->spell_tags[i]) {
			    statistics.spell_hash_full++;
			    break;
			}
			/* If one tag is set and the other is not, that is
			 * fine, but we have to note that we need to copy
			 * the data in that case.
			 */
			if ((!op->spell_tags[i] && tmp->spell_tags[i]) ||
			    (op->spell_tags[i] && !tmp->spell_tags[i])) {
			    need_copy=1;
			}
		}
		/* If we did not get through entire array, it means
		 * we got a conflicting hash, and so we won't be
		 * able to merge these - just continue processing
		 * object on this space.
		 */
		if (i <= SPELL_TAG_SIZE) continue;

		/* Ok - everything checked out - we should be able to
		 * merge tmp in op.  So lets copy the tag data if
		 * needed.  Note that this is a selective copy, as
		 * we don't want to clear values that may be set in op.
		 */
		if (need_copy) {
		    for (i=0; i< SPELL_TAG_SIZE; i++)
			if (!op->spell_tags[i] && tmp->spell_tags[i] && tmp->spell_tags[i]!=op->stats.maxhp)
			    op->spell_tags[i] = tmp->spell_tags[i];
		}
		FREE_AND_CLEAR(tmp->spell_tags);
	    }

	    /* if tmp has a spell_tags table, copy it to op and free tmps */
	    if (tmp->spell_tags && !op->spell_tags) {
		op->spell_tags = tmp->spell_tags;
		tmp->spell_tags = NULL;

		/* We don't need to keep a copy of our maxhp value
		 * in the copied over value
		*/
		if (OB_SPELL_TAG_MATCH(op, op->stats.maxhp))
		    OB_SPELL_TAG_HASH(op, op->stats.maxhp) = 0;
	    }

	    /* For spells to work correctly, we need to record what spell
	     * tags we've merged in with this effect.  This is used
	     * in ok_to_put_more() to see if a spell effect is already on
	     * the space.
	     */
	    if (op->stats.maxhp != tmp->stats.maxhp) {
#ifdef OBJECT_DEBUG
		/* This if statement should never happen - the logic above should
		 * have prevented it.  It is a problem, because by now its possible
		 * we've destroyed the spell_tags in tmp, so we can't really
		 * just bail out.
		 */
		 
		if (op->spell_tags && OB_SPELL_TAG_HASH(op, tmp->stats.maxhp) != 0 &&
		    !OB_SPELL_TAG_MATCH(op, tmp->stats.maxhp)) {
		    LOG(llevError,"insert_ob_in_map: Got non matching spell tags: %d != %d\n",
			OB_SPELL_TAG_HASH(op, tmp->stats.maxhp),  tmp->stats.maxhp);
		}
#endif
		if (!op->spell_tags)
		    op->spell_tags = calloc(1, SPELL_TAG_SIZE * sizeof(tag_t));

		OB_SPELL_TAG_HASH(op, tmp->stats.maxhp) = tmp->stats.maxhp;
	    }

	    statistics.spell_merges++;
	    op->speed_left = MAX(op->speed_left, tmp->speed_left);

	    if (tmp->duration != op->duration) {
		/* We need to use tmp_dam here because otherwise the
		 * calculations can overflow the size of stats.dam.
		 */
		int tmp_dam = tmp->stats.dam * (tmp->duration + 1)+
			op->stats.dam * (op->duration + 1);

		op->duration = MAX(op->duration, tmp->duration);
		tmp_dam /= (op->duration + 1);
		op->stats.dam = tmp_dam + 1;
	    }
	    else {
		/* in this case, duration is the same, so simply adding
		 * up damage works.
		 */
		op->stats.dam += tmp->stats.dam;
	    }

	    remove_ob(tmp);
	    free_object(tmp);
	}
    }
}


/**
 * This function inserts the object in the two-way linked list
 * which represents what is on a map.
 *
 * @param op
 * object to insert. Must be removed. Its coordinates must be valid for the map.
 * @param m
 * map to insert into. Must not be NULL.
 * @param originator
 * player, monster or other object that caused 'op' to be inserted
 * into 'm'.  May be NULL.
 * @param flag
 * bitmask about special things to do (or not do) when this
 * function is called. See the object.h file for the INS_ values.
 * Passing 0 for flag gives proper default values, so flag really only needs
 * to be set if special handling is needed.
 * @return 
 * @li new object if 'op' was merged with other object.
 * @li NULL if 'op' was destroyed
 * @li just 'op' otherwise
 *
 * @todo
 * this function is a mess, and should be cleaned.
 */

object *insert_ob_in_map (object *op, mapstruct *m, object *originator, int flag)
{
    object *tmp, *top, *floor=NULL;
    sint16 x,y;

    if (QUERY_FLAG (op, FLAG_FREED)) {
        LOG (llevError, "Trying to insert freed object!\n");
        return NULL;
    }
    if(m==NULL) {
        char buf[HUGE_BUF];
        dump_object(op, buf, sizeof(buf));
        LOG(llevError,"Trying to insert in null-map!\n%s\n",buf);
        return op;
    }
    if(out_of_map(m,op->x,op->y)) {
        char buf[HUGE_BUF];
        dump_object(op, buf, sizeof(buf));
        LOG(llevError,"Trying to insert object outside the map.\n%s\n", buf);
#ifdef MANY_CORES
        /* Better to catch this here, as otherwise the next use of this object
         * is likely to cause a crash.  Better to find out where it is getting
         * improperly inserted.
         */
        abort();
#endif
        return op;
    }
    if(!QUERY_FLAG(op,FLAG_REMOVED)) {
        char buf[HUGE_BUF];
        dump_object(op, buf, sizeof(buf));
        LOG(llevError,"Trying to insert (map) inserted object.\n%s\n", buf);
        return op;
    }
    if(op->more!=NULL) {
        /* The part may be on a different map. */

        object *more = op->more;

        /* We really need the caller to normalize coordinates - if
         * we set the map, that doesn't work if the location is within
         * a map and this is straddling an edge.  So only if coordinate
         * is clear wrong do we normalize it.
         */
        if (OUT_OF_REAL_MAP(more->map, more->x, more->y)) {
            /* Debugging information so you can see the last coordinates this object had */
            more->ox = more->x;
            more->oy = more->y;
            more->map = get_map_from_coord(m, &more->x, &more->y);
        } else if (!more->map) {
            /* For backwards compatibility - when not dealing with tiled maps,
            * more->map should always point to the parent.
            */
            more->map = m;
        }

        if (insert_ob_in_map(more, more->map, originator, flag) == NULL) {
            if ( ! op->head)
            LOG (llevError, "BUG: insert_ob_in_map(): inserting op->more killed op\n");
            return NULL;
        }
    }
    CLEAR_FLAG(op,FLAG_REMOVED);

    /* Debugging information so you can see the last coordinates this object had */
    op->ox=op->x;
    op->oy=op->y;
    x = op->x;
    y = op->y;
    op->map=get_map_from_coord(m, &x, &y);

    /* this has to be done after we translate the coordinates. */
    if (op->nrof && !(flag & INS_NO_MERGE) && op->type != SPELL_EFFECT) {
        for(tmp=GET_MAP_OB(op->map,x,y);tmp!=NULL;tmp=tmp->above) {
            if (can_merge(op,tmp)) {
                op->nrof+=tmp->nrof;
                remove_ob(tmp);
                free_object(tmp);
	    }
	}
    }
    else if (op->type == SPELL_EFFECT && !op->range && !op->other_arch  && (op->speed_left + op->speed) < 0.0) {
	merge_spell(op, x, y);
    }

    /* Ideally, the caller figures this out.  However, it complicates a lot
     * of areas of callers (eg, anything that uses find_free_spot would now
     * need extra work
     */
    if (op->map != m) {
        /* coordinates should not change unless map also changes */
        op->x = x;
        op->y = y;
    }

    if (op->type != LAMP)
        CLEAR_FLAG(op,FLAG_APPLIED); /* hack for fixing F_APPLIED in items of dead people */
    CLEAR_FLAG(op, FLAG_INV_LOCKED);
    if (!QUERY_FLAG(op, FLAG_ALIVE))
        CLEAR_FLAG(op, FLAG_NO_STEAL);

    /* In many places, a player is passed as the originator, which
     * is fine.  However, if the player is on a transport, they are not
     * actually on the map, so we can't use them for the linked pointers,
     * nor should the walk on function below use them either.
     */
    if (originator && originator->contr && originator->contr->transport) 
        originator=originator->contr->transport;

    if (flag & INS_BELOW_ORIGINATOR) {
        if (originator->map != op->map || originator->x != op->x ||
            originator->y != op->y) {
            LOG(llevError,"insert_ob_in_map called with INS_BELOW_ORIGINATOR when originator not on same space!\n");
            abort();
        }
        op->above = originator;
        op->below = originator->below;
        if (op->below) op->below->above = op;
        else SET_MAP_OB(op->map, op->x, op->y, op);
        /* since *below* originator, no need to update top */
        originator->below = op;
    } else {
        /* If there are other objects, then */
        /* This test is incorrect i think, as ins_above_floor_only needs the floor variable
        if((! (flag & INS_MAP_LOAD)) && ((top=GET_MAP_OB(op->map,op->x,op->y))!=NULL)) {
        */
        if(((top=GET_MAP_OB(op->map,op->x,op->y))!=NULL)) {
            object *last=NULL;
            /*
             * If there are multiple objects on this space, we do some trickier handling.
             * We've already dealt with merging if appropriate.
             * Generally, we want to put the new object on top. But if
             * flag contains INS_ABOVE_FLOOR_ONLY, once we find the last
             * floor, we want to insert above that and no further.
             * Also, if there are spell objects on this space, we stop processing
             * once we get to them.  This reduces the need to traverse over all of 
             * them when adding another one - this saves quite a bit of cpu time
             * when lots of spells are cast in one area.  Currently, it is presumed
             * that flying non pickable objects are spell objects.
             */

            while (top != NULL) {
                if (QUERY_FLAG(top, FLAG_IS_FLOOR) ||
                    QUERY_FLAG(top, FLAG_OVERLAY_FLOOR)) floor = top;

                if (QUERY_FLAG(top, FLAG_NO_PICK) && 
                  (top->move_type & (MOVE_FLY_LOW |MOVE_FLY_HIGH)) &&
                  !QUERY_FLAG(top, FLAG_IS_FLOOR)) {
                    /* We insert above top, so we want this object below this */
                    top=top->below;
                    break;
                }
                last = top;
                top = top->above;
            }
            /* Don't want top to be NULL, so set it to the last valid object */
            top = last;

            /* We let update_position deal with figuring out what the space
             * looks like instead of lots of conditions here.
             * makes things faster, and effectively the same result.
             */

            /* Have object 'fall below' other objects that block view.
             * Unless those objects are exits, type 66
             * If INS_ON_TOP is used, don't do this processing
             * Need to find the object that in fact blocks view, otherwise
             * stacking is a bit odd.
             */
            if (!(flag & INS_ON_TOP) &&
              (get_map_flags(op->map, NULL, op->x, op->y, NULL, NULL) & P_BLOCKSVIEW) && 
              (op->face && !op->face->visibility)) {
                for (last=top; last != floor; last=last->below)
                    if (QUERY_FLAG(last, FLAG_BLOCKSVIEW)&&(last->type != EXIT)) break;
                /* Check to see i we found the object that blocks view,
                 * and make sure we have a below pointer for it so that
                 * we can get inserted below this one, which requires we
                 * set top to the object below us.
                 */
                if (last && last->below && last != floor) top=last->below;
            }
        } /* If objects on this space */
        if (flag & INS_MAP_LOAD)
            top = GET_MAP_TOP(op->map,op->x,op->y);
        if (flag & INS_ABOVE_FLOOR_ONLY) top = floor;

        /* Top is the object that our object (op) is going to get inserted above. */

        /* First object on this space */
        if (!top) {
            op->above = GET_MAP_OB(op->map, op->x, op->y);
            if (op->above) op->above->below = op;
            op->below = NULL;
            SET_MAP_OB(op->map, op->x, op->y, op);
        } else { /* get inserted into the stack above top */
            op->above = top->above;
            if (op->above) op->above->below = op;
            op->below = top;
            top->above = op;
        }
        if (op->above==NULL)
            SET_MAP_TOP(op->map,op->x, op->y, op);
    } /* else not INS_BELOW_ORIGINATOR */

    if(op->type==PLAYER)
        op->contr->do_los=1;

    /* If we have a floor, we know the player, if any, will be above
     * it, so save a few ticks and start from there.
     */
    if (!(flag & INS_MAP_LOAD))
        for(tmp=floor?floor:GET_MAP_OB(op->map,op->x,op->y);tmp!=NULL;tmp=tmp->above) {
            if (tmp->type == PLAYER)
                tmp->contr->socket.update_look=1;
        }

    /* If this object glows, it may affect lighting conditions that are
     * visible to others on this map.  But update_all_los is really
     * an inefficient way to do this, as it means los for all players
     * on the map will get recalculated.  The players could very well
     * be far away from this change and not affected in any way -
     * this should get redone to only look for players within range,
     * or just updating the P_NEED_UPDATE for spaces within this area
     * of effect may be sufficient.
     */
    if(MAP_DARKNESS(op->map) && (op->glow_radius != 0)) 
        update_all_los(op->map, op->x, op->y);


    /* updates flags (blocked, alive, no magic, etc) for this map space */
    update_object(op,UP_OBJ_INSERT);


    /* Don't know if moving this to the end will break anything.  However,
     * we want to have update_look set above before calling this.
     *
     * check_move_on() must be after this because code called from
     * check_move_on() depends on correct map flags (so functions like
     * blocked() and wall() work properly), and these flags are updated by
     * update_object().
     */

    /* if this is not the head or flag has been passed, don't check walk on status */

    if (!(flag & INS_NO_WALK_ON) && !op->head) {
        if (check_move_on(op, originator))
            return NULL;

        /* If we are a multi part object, lets work our way through the check
         * walk on's.
         */
        for (tmp=op->more; tmp!=NULL; tmp=tmp->more)
            if (check_move_on (tmp, originator))
                return NULL;
    }
    return op;
}

/**
 * This function inserts an object of a specified archetype in the map, but if it
 * finds an object of its own type, it'll remove that one first. 
 *
 * @param arch_string
 * object's archetype to insert
 *
 * @param op
 * object to insert it under:  supplies x and the map.
 *
 * @todo
 * the remove loop is suspicious - what happens when remove_ob() is called on tmp, which then
 * has its above/below cleared?
 */
void replace_insert_ob_in_map(const char *arch_string, object *op) {
    object *tmp;
    object *tmp1;

    /* first search for itself and remove any old instances */

    for(tmp=GET_MAP_OB(op->map,op->x,op->y); tmp!=NULL; tmp=tmp->above) {
        if(!strcmp(tmp->arch->name,arch_string)) /* same archetype */ {
            remove_ob(tmp);
            free_object(tmp);
        }
    }

    tmp1=arch_to_object(find_archetype(arch_string));

    tmp1->x = op->x; tmp1->y = op->y;
    insert_ob_in_map(tmp1,op->map,op,0);
}

/**
 * get_split_ob(ob,nr) splits up ob into two parts.  The part which
 * is returned contains nr objects, and the remaining parts contains
 * the rest (or is removed and freed if that number is 0).
 * On failure, NULL is returned, and the reason LOG()ed.
 *
 * @param orig_ob
 * object from which to split.
 * @param nr
 * number of elements to split.
 * @param err
 * buffer that will contain failure reason if NULL is returned. Can be NULL.
 * @param size
 * err's size
 * @return
 * split object, or NULL on failure.
 * @todo
 * handle case orig_ob->nrof == 0 (meaning 1).
 */
object *get_split_ob(object *orig_ob, uint32 nr, char* err, int size) {
    object *newob;
    int is_removed = (QUERY_FLAG (orig_ob, FLAG_REMOVED) != 0);

    if(orig_ob->nrof<nr) {
        /* If err is set, the caller knows that nr can be wrong (player trying to drop items), thus don't log that. */
        if (err)
            snprintf(err, size, "There are only %d %ss.", orig_ob->nrof?orig_ob->nrof:1, orig_ob->name);
        else
            LOG(llevDebug,"There are only %d %ss.\n", orig_ob->nrof?orig_ob->nrof:1, orig_ob->name);
        return NULL;
    }
    newob = object_create_clone(orig_ob);
    if((orig_ob->nrof-=nr)<1) {
        if (!is_removed)
            remove_ob(orig_ob);
        free_object2(orig_ob, 1);
    }
    else if (!is_removed) {
        if(orig_ob->env!=NULL)
            sub_weight (orig_ob->env,orig_ob->weight*nr);
        if (orig_ob->env == NULL && orig_ob->map->in_memory!=MAP_IN_MEMORY) {
            /* This is a failure, so always log it. */
            LOG(llevDebug,
                "Error, tried to split object whose map is not in memory.\n");
            if (err)
                snprintf(err, size, "Error, tried to split object whose map is not in memory.\n");
            return NULL;
        }
    }
    newob->nrof=nr;

    return newob;
}

/**
 * Decreases a specified number from
 * the amount of an object.  If the amount reaches 0, the object
 * is subsequently removed and freed.
 *
 * @param op
 * object to decrease.
 * @param i
 * number to remove.
 * @return
 * 'op' if something is left, NULL if the amount reached 0.
 */
object *decrease_ob_nr (object *op, uint32 i)
{
    object *tmp;
    player *pl;

    if (i == 0)   /* objects with op->nrof require this check */
        return op;

    if (i > op->nrof)
        i = op->nrof;

    if (QUERY_FLAG (op, FLAG_REMOVED))
    {
        op->nrof -= i;
    }
    else if (op->env != NULL)
    {
        /* is this object in the players inventory, or sub container
         * therein?
         */
        tmp = get_player_container (op->env);
        /* nope.  Is this a container the player has opened?
         * If so, set tmp to that player.
         * IMO, searching through all the players will mostly
         * likely be quicker than following op->env to the map,
         * and then searching the map for a player.
         */
        if (!tmp) {
            for (pl=first_player; pl; pl=pl->next)
            if (pl->ob->container == op->env) break;
            if (pl) tmp=pl->ob;
            else tmp=NULL;
        }

        if (i < op->nrof) {
            sub_weight (op->env, op->weight * i);
            op->nrof -= i;
            if (tmp) {
                esrv_send_item(tmp, op);
            }
        } else {
            remove_ob (op);
            op->nrof = 0;
            if (tmp) {
                esrv_del_item(tmp->contr, op->count);
            }
        }
    }
    else
    {
        object *above = op->above;

        if (i < op->nrof) {
            op->nrof -= i;
        } else {
            remove_ob (op);
            op->nrof = 0;
        }
        /* Since we just removed op, op->above is null */
        for (tmp = above; tmp != NULL; tmp = tmp->above)
            if (tmp->type == PLAYER) {
                if (op->nrof)
                    esrv_send_item(tmp, op);
                else
                    esrv_del_item(tmp->contr, op->count);
            }
    }

    if (op->nrof) {
        return op;
    } else {
        free_object (op);
        return NULL;
    }
}

/**
 * add_weight(object, weight) adds the specified weight to an object,
 * and also updates how much the environment(s) is/are carrying.
 *
 * Takes container weight reduction into account.
 *
 * @param op
 * object to which we add weight.
 * @param weight
 * weight to add.
 *
 * @todo
 * check if mergeable with sub_weight().
 */
void add_weight (object *op, signed long weight) {
    while (op!=NULL) {
        if (op->type == CONTAINER) {
        weight=(signed long)(weight*(100-op->stats.Str)/100);
        }
        op->carrying+=weight;
        op=op->env;
    }
}

/**
 * This function inserts the object op in the linked list
 * inside the object environment.
 *
 * @param op
 * object to insert. Must be removed and not NULL. Must not be multipart.
 * May become invalid after return, so use return value of the function.
 * @param where
 * object to insert into. Must not be NULL. Should be the head part.
 * @return
 * pointer to inserted item, which will be different than op if object was merged.
 */
object *insert_ob_in_ob(object *op,object *where) {
    object *tmp, *otmp;

    if(!QUERY_FLAG(op,FLAG_REMOVED)) {
        char buf[HUGE_BUF];
        dump_object(op, buf, sizeof(buf));
        LOG(llevError,"Trying to insert (ob) inserted object.\n%s\n", buf);
        return op;
    }
    if(where==NULL) {
        char buf[HUGE_BUF];
        dump_object(op, buf, sizeof(buf));
        LOG(llevError,"Trying to put object in NULL.\n%s\n", buf);
        return op;
    }
    if (where->head) {
        LOG(llevDebug, 
        "Warning: Tried to insert object wrong part of multipart object.\n");
        where = where->head;
    }
    if (op->more) {
        LOG(llevError, "Tried to insert multipart object %s (%d)\n",
            op->name, op->count);
        return op;
    }
    CLEAR_FLAG(op, FLAG_OBJ_ORIGINAL);
    CLEAR_FLAG(op, FLAG_REMOVED);
    if(op->nrof) {
        for(tmp=where->inv;tmp!=NULL;tmp=tmp->below)
            if ( can_merge(tmp,op) ) {
                /* return the original object and remove inserted object
                 * (client needs the original object) */
                tmp->nrof += op->nrof;
                /* Weight handling gets pretty funky.  Since we are adding to
                 * tmp->nrof, we need to increase the weight.
                 */
                add_weight (where, op->weight*op->nrof);
                SET_FLAG(op, FLAG_REMOVED);
                free_object(op); /* free the inserted object */
                return tmp;
            }

        /* the item couldn't merge. */
        add_weight (where, op->weight*op->nrof);
    } else
        add_weight (where, (op->weight+op->carrying));

    op->map=NULL;
    op->env=where;
    op->above=NULL;
    op->below=NULL;
    op->x=0,op->y=0;
    op->ox=0,op->oy=0;

    /* Client has no idea of ordering so lets not bother ordering it here.
     * It sure simplifies this function...
     */
    if (where->inv==NULL)
        where->inv=op;
    else {
        op->below = where->inv;
        op->below->above = op;
        where->inv = op;
    }

    otmp=get_player_container(where);
    if (otmp&&otmp->contr!=NULL) {
        if (!QUERY_FLAG(otmp,FLAG_NO_FIX_PLAYER) && (QUERY_FLAG(op, FLAG_APPLIED) || (op->type == SKILL) || (op->glow_radius != 0)))
            /* fix_object will only consider applied items, or skills, or items with a glow radius.
               thus no need to call it if our object hasn't that. */
            fix_object(otmp);
    }

    /* reset the light list and los of the players on the map */
    if((op->glow_radius!=0)&&where->map)
    {
#ifdef DEBUG_LIGHTS
        LOG(llevDebug, " insert_ob_in_ob(): got %s to insert in map/op\n",
        op->name);
#endif /* DEBUG_LIGHTS */ 
        if (MAP_DARKNESS(where->map)) {
            SET_MAP_FLAGS(where->map, where->x, where->y,  P_NEED_UPDATE);
            update_position(where->map, where->x, where->y);
            update_all_los(where->map, where->x, where->y);
        }
    }

    return op;
}

/**
 * Checks if any objects has a move_type that matches objects
 * that effect this object on this space.  Call apply() to process
 * these events.
 *
 * Any speed-modification due to SLOW_MOVE() of other present objects
 * will affect the speed_left of the object.
 * 4-21-95 added code to check if appropriate skill was readied - this will
 * permit faster movement by the player through this terrain. -b.t.
 *
 * MSW 2001-07-08: Check all objects on space, not just those below
 * object being inserted.  insert_ob_in_map may not put new objects
 * on top.
 *
 * @param op
 * object that may trigger something.
 * @param originator
 * player, monster or other object that caused 'op' to be inserted
 * into 'map'.  May be NULL.
 * @return
 * 1 if 'op' was destroyed, 0 otherwise.
 */
int check_move_on (object *op, object *originator)
{
    object *tmp;
    tag_t tag;
    mapstruct *m=op->map;
    int x=op->x, y=op->y;
    MoveType	move_on, move_slow, move_block;

    if(QUERY_FLAG(op,FLAG_NO_APPLY))
        return 0;

    tag = op->count;

    move_on = GET_MAP_MOVE_ON(op->map, op->x, op->y);
    move_slow = GET_MAP_MOVE_SLOW(op->map, op->x, op->y);
    move_block = GET_MAP_MOVE_BLOCK(op->map, op->x, op->y);

    /* if nothing on this space will slow op down or be applied,
     * no need to do checking below.    have to make sure move_type
     * is set, as lots of objects don't have it set - we treat that
     * as walking.
     */
    if (op->move_type && !(op->move_type & move_on) && !(op->move_type & move_slow))
        return 0;

    /* This is basically inverse logic of that below - basically,
     * if the object can avoid the move on or slow move, they do so,
     * but can't do it if the alternate movement they are using is
     * blocked.  Logic on this seems confusing, but does seem correct.
     */
    if ((op->move_type & ~move_on & ~move_block) != 0 &&
        (op->move_type & ~move_slow & ~move_block) != 0) return 0;

    /* The objects have to be checked from top to bottom.
     * Hence, we first go to the top: 
     */

    for (tmp=GET_MAP_OB(op->map, op->x, op->y); tmp!=NULL &&
        tmp->above!=NULL; tmp=tmp->above) {
        /* Trim the search when we find the first other spell effect 
         * this helps performance so that if a space has 50 spell objects,
         * we don't need to check all of them.
         */
        if ((tmp->move_type & MOVE_FLY_LOW) && QUERY_FLAG(tmp, FLAG_NO_PICK)) break;
    }
    for(;tmp!=NULL; tmp=tmp->below) {
        if (tmp == op) continue;    /* Can't apply yourself */

        /* Check to see if one of the movement types should be slowed down.
         * Second check makes sure that the movement types not being slowed
         * (~slow_move) is not blocked on this space - just because the
         * space doesn't slow down swimming (for example), if you can't actually
         * swim on that space, can't use it to avoid the penalty.
         */
        if (!QUERY_FLAG(op, FLAG_WIZPASS)) {
            if ((!op->move_type && tmp->move_slow & MOVE_WALK) ||
              ((op->move_type & tmp->move_slow) &&
              (op->move_type & ~tmp->move_slow & ~tmp->move_block) == 0)) {

                float diff;

                diff = tmp->move_slow_penalty*FABS(op->speed);
                if (op->type == PLAYER) {
                    if ((QUERY_FLAG(tmp, FLAG_IS_HILLY) && find_skill_by_number(op, SK_CLIMBING)) ||
                      (QUERY_FLAG(tmp, FLAG_IS_WOODED) && find_skill_by_number(op, SK_WOODSMAN))) {
                        diff /= 4.0;
                    }
                }
            op->speed_left -= diff;
            }
        }

        /* Basically same logic as above, except now for actual apply. */
        if ((!op->move_type && tmp->move_on & MOVE_WALK) ||
          ((op->move_type & tmp->move_on) &&
          (op->move_type & ~tmp->move_on & ~tmp->move_block)==0)) {

            ob_move_on(tmp, op, originator);
            if (was_destroyed (op, tag))
                return 1;

            /* what the person/creature stepped onto has moved the object
             * someplace new.  Don't process any further - if we did,
             * have a feeling strange problems would result.
             */
            if (op->map != m || op->x != x || op->y != y) return 0;
        }
    }
    return 0;
}

/**
 * Searches for any objects with a matching archetype at the given map and coordinates.
 *
 * @param at
 * archetype to search for.
 * @param m
 * @param x
 * @param y
 * where to search. Must be valid position.
 * @return
 * first matching object, or NULL if none matches.
 */

object *present_arch(const archetype *at, mapstruct *m, int x, int y) {
    object *tmp;
    if(m==NULL || out_of_map(m,x,y)) {
        LOG(llevError,"Present_arch called outside map.\n");
        return NULL;
    }
    for(tmp=GET_MAP_OB(m,x,y); tmp != NULL; tmp = tmp->above)
        if(tmp->arch == at)
        return tmp;
    return NULL;
}

/**
 * Searches for any objects with
 * a matching type variable at the given map and coordinates.
 *
 * @param type
 * type to get.
 * @param m
 * @param x
 * @param y
 * where to search. Must be valid position.
 * @return
 * first matching object, or NULL if none matches.
 *
 * @todo
 * type should be the same type as object::type.
 */
object *present(unsigned char type,mapstruct *m, int x,int y) {
    object *tmp;
    if(out_of_map(m,x,y)) {
        LOG(llevError,"Present called outside map.\n");
        return NULL;
    }
    for(tmp=GET_MAP_OB(m,x,y);tmp!=NULL;tmp=tmp->above)
        if(tmp->type==type)
        return tmp;
    return NULL;
}

/**
 * Searches for any objects with
 * a matching type variable in the inventory of the given object.
 * @param type
 * type to search for.
 * @param op
 * object to search into.
 * @return
 * first matching object, or NULL if none matches.
 */
object *present_in_ob(unsigned char type, const object *op) {
    object *tmp;
    for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
        if(tmp->type==type)
            return tmp;
    return NULL;
}

/**
 * Searches for any objects with
 * a matching type & name variable in the inventory of the given object.
 * This is mostly used by spell effect code, so that we only
 * have one spell effect at a time.
 * type can be used to narrow the search - if type is set,
 * the type must also match.  -1 can be passed for the type,
 * in which case the type does not need to pass.
 * str is the string to match against.  Note that we match against
 * the object name, not the archetype name.  this is so that the
 * spell code can use one object type (force), but change it's name
 * to be unique.
 *
 * @param type
 * object type to search for. -1 means to ignore it.
 * @param str
 * object name to search for.
 * @param op
 * where to search.
 * @return
 * first matching object, or NULL if none matches.
 *
 * @todo
 * use add_string() hack to avoid the strcmp?
 */

object *present_in_ob_by_name(int type, const char *str, const object *op) {
    object *tmp;

    for(tmp=op->inv; tmp!=NULL; tmp=tmp->below) {
        if ((type==-1 || tmp->type==type) && (!strcmp(str, tmp->name)))
            return tmp;
    }
    return NULL;
}

/**
 * Searches for any objects with a matching archetype in the inventory of the given object.
 *
 * @param at
 * archetype to search for.
 * @param op
 * where to search.
 * @return first matching object, or NULL if none matches.
 */
object *present_arch_in_ob(const archetype *at, const object *op)  {
    object *tmp;
    for(tmp=op->inv;tmp!=NULL;tmp=tmp->below)
        if( tmp->arch == at)
        return tmp;
    return NULL;
}

/**
 * Activate recursively a flag on an object's inventory
 *
 * @param op
 * object to recurse. Can have an empty inventory.
 * @param flag
 * flag to set.
 */
void flag_inv(object*op, int flag){
    object *tmp;
    if(op->inv)
        for(tmp = op->inv; tmp != NULL; tmp = tmp->below){
            SET_FLAG(tmp, flag);
        flag_inv(tmp,flag);
        }
}

/**
 * Desactivate recursively a flag on an object inventory
 *
 * @param op
 * object to recurse. Can have an empty inventory.
 * @param flag
 * flag to unset.
 */
void unflag_inv(object*op, int flag){
    object *tmp;
    if(op->inv)
        for(tmp = op->inv; tmp != NULL; tmp = tmp->below){
            CLEAR_FLAG(tmp, flag);
            unflag_inv(tmp,flag);
        }
}

/**
 * set_cheat(object) sets the cheat flag (WAS_WIZ) in the object and in
 * all it's inventory (recursively).
 * If checksums are used, a player will get set_cheat called for
 * him/her-self and all object carried by a call to this function.
 *
 * @param op
 * object for which to set the flag.
 */
void set_cheat(object *op) {
    SET_FLAG(op, FLAG_WAS_WIZ);
    flag_inv(op, FLAG_WAS_WIZ);
}

/**
 * Sets hx and hy to the coords to insert a possibly
 * multi-tile ob at, around gen.
 *
 * @param ob
 * object to insert. Must not be NULL.
 * @param gen
 * where to insert. Must not be NULL.
 * @param[out] hx
 * @param[out] hy
 * coordinates at which insertion is possible.
 * @return
 * 0 for success and -1 for failure.
 *
 * @note
 * This function assumes that multi-tile objects are rectangular.
 */
int find_multi_free_spot_around(object *ob, object *gen, int *hx, int *hy) {
    int genx, geny, genx2, geny2, sx, sy, sx2, sy2, ix, iy, nx, ny, i, flag;
    int freecount=0;

    if (ob->head)
        ob = ob->head;

    get_multi_size(ob, &sx, &sy, &sx2, &sy2);
    get_multi_size(gen, &genx, &geny, &genx2, &geny2);
    /*
     * sx and sy are now the coords of the bottom right corner of ob relative to the head.
     * genx and geny are now the coords of the bottom right corner of gen relative to the head.
     * sx2 and sy2 are now the coords of the head of ob relative to the top left corner.
     * genx2 and geny2 are now the coords of the head of gen relative to the top left corner.
     */

    sx++;
    sy++;
    genx++;
    geny++;
    /*
     * sx, sy, genx, and geny, are now the size of the object, excluding parts left and above
     * the head.
     */

    ix = gen->x - sx - genx2;
    iy = gen->y - sy - geny2;
    sx += genx + sx2;
    sy += geny + sy2;
    /*
     * ix and iy are the map coords of the top left square where the head of ob could possibly
     * be placed. sx and sy are now the size of the square to search for placement of the head
     * relative to ix and iy.
     */

    /*
     * Loop around the square of possible positions for the head of ob object:
     */
    for(i = 0; i < (sx+sx+sy+sy); i++) {
        if (i <= sx) {
            nx = i+ix;
            ny = iy;
        } else if (i <= (sx + sy)) {
            nx = ix + sx;
            ny = iy + i - sx;
        } else if (i <= (sx + sy + sx)) {
            nx = ix + sx - (i - (sx + sy));
            ny = iy + sy;
        } else {
            nx = ix;
            ny = iy + sy  - (i - (sx + sy + sx));
        }
        /* Check if the spot is free. */
        flag = ob_blocked(ob,gen->map,nx,ny);
        if (!flag) {
            freecount++;
        }
    }
    /* If no free spaces, return. */
    if (!freecount)
        return -1;

    /* Choose a random valid position */
    freecount = RANDOM()%freecount;
    for(i = 0; i < (sx+sx+sy+sy); i++) {
        if (i <= sx) {
            nx = i+ix;
            ny = iy;
        } else if (i <= (sx + sy)) {
            nx = ix + sx;
            ny = iy + i - sx;
        } else if (i <= (sx + sy + sx)) {
            nx = ix + sx - (i - (sx + sy));
            ny = iy + sy;
        } else {
            nx = ix;
            ny = iy + sy  - (i - (sx + sy + sx));
        }

        /* Make sure it's within map. */
        if (nx < 0 || nx >= MAP_WIDTH(gen->map) ||
            ny < 0 || ny >= MAP_HEIGHT(gen->map))
            continue;

        /* Check if the spot is free.*/
        flag = ob_blocked(ob,gen->map,nx,ny);
        if (!flag) {
            freecount--;
            if (freecount <= 0) {
                *hx = nx;
                *hy = ny;
                 return 0;
            }
        }
    }
    return -1;
}

/**
 * Sets hx and hy to the coords to insert a possibly
 * multi-tile ob at, within radius of generator, which
 * is stored in key_value "generator_radius".  Radius
 * defaults to 1.
 *
 * @param ob
 * object to insert. Must not be NULL.
 * @param gen
 * where to insert. Must not be NULL.
 * @param[out] hx
 * @param[out] hy
 * coordinates at which insertion is possible.
 * @return
 * 0 for success and -1 for failure.
 *
 * @note
 * This function assumes that multi-tile objects are rectangular.
 */
int find_multi_free_spot_within_radius(object *ob, object *gen, int *hx, int *hy) {
    int genx, geny, genx2, geny2, sx, sy, sx2, sy2, ix, iy, nx, ny, i, flag;
    sint8 x, y, radius;
    int freecount=0, freecountstop=0;
    const char *value;
    sint8* x_array;
    sint8* y_array;


        /* If radius is not set, default to 1 */
    value = get_ob_key_value(gen, "generator_radius");
    if( value ){
        radius = (sint8)strtol_local((char *)value, NULL, 10);
        if( radius < 1 ){
            radius = 1;
        }
    } else {
        radius = 1;
    }
    
    if (ob->head)
        ob = ob->head;

    get_multi_size(ob, &sx, &sy, &sx2, &sy2);
    get_multi_size(gen, &genx, &geny, &genx2, &geny2);
    /*
     * sx and sy are now the coords of the bottom right corner
     * of ob relative to the head.
     * genx and geny are now the coords of the bottom right corner
     * of gen relative to the head.
     * sx2 and sy2 are now the coords of the head of ob relative
     * to the top left corner.
     * genx2 and geny2 are now the coords of the head of gen relative
     * to the top left corner.
     */

    sx++;
    sy++;
    genx++;
    geny++;
    /*
     * sx, sy, genx, and geny, are now the size of the object,
     * excluding parts left and above the head.
     */

    ix = gen->x - sx - genx2 - radius + 1;
    iy = gen->y - sy - geny2 - radius + 1;
    sx += genx + sx2 + radius * 2 - 1;
    sy += geny + sy2 + radius * 2 - 1;
    
    /*
     * ix and iy are the map coords of the top left square where
     * the head of ob could possibly be placed. sx and sy are now
     * the size of the square to search for placement of the head
     * relative to ix and iy.
     */

    /* Create arrays large enough to hold free space coordinates */
    x_array = malloc(sx*sy * sizeof(sint8));
    y_array = malloc(sx*sy * sizeof(sint8));

    /*
     * Loop through the area of possible positions for the head of ob object:
     */
    for(x = 0; x < sx; x++){
        for(y = 0; y < sy; y++){
            nx = ix + x;
            ny = iy + y;


                /* Make sure it's within map. */
            if( get_map_flags(gen->map, NULL, nx, ny, NULL, NULL)
                & P_OUT_OF_MAP ){
                continue;
            }

                /* Check if the spot is free. */
            flag = ob_blocked(ob,gen->map,nx,ny);
            if (!flag) {
                x_array[freecount] = nx;
                y_array[freecount] = ny;
                freecount++;
            }
        }
    }
    /* If no free spaces, return. */
    if (!freecount){
        free(x_array);
        free(y_array);
        return -1;
    }
    
    /* Choose a random valid position */
    freecountstop = RANDOM()%freecount;
    for(i = 0; i < freecount; i++){
        nx = x_array[i];
        ny = y_array[i];

            /* Check if the spot is free.*/
        flag = ob_blocked(ob,gen->map,nx,ny);
        if (!flag) {
            freecountstop--;
            if (freecountstop <= 0) {
                *hx = nx;
                *hy = ny;
                free(x_array);
                free(y_array);
                return 0;
            }
        }
    }
    free(x_array);
    free(y_array);
    return -1;
}

/**
 * find_free_spot(object, map, x, y, start, stop) will search for
 * a spot at the given map and coordinates which will be able to contain
 * the given object.
 *
 * It returns a random choice among the alternatives found.
 *
 * @param ob
 * object to insert.
 * @param m
 * @param x
 * @param y
 * where to insert the object.
 * @param start
 * @param stop
 * first (inclusive) and last (exclusive) positions, in the freearr_ arrays, to search.
 * @return
 * index into ::freearr_x and ::freearr_y, -1 if no spot available (dir 0 = x,y)
 *
 * @note
 * this only checks to see if there is space for the head of the
 * object - if it is a multispace object, this should be called for all
 * pieces.
 * @note
 * This function does correctly handle tiled maps, but does not
 * inform the caller.  However, insert_ob_in_map will update as
 * necessary, so the caller shouldn't need to do any special work.
 * @note
 * Updated to take an object instead of archetype - this is necessary
 * because arch_blocked (now ob_blocked) needs to know the movement type
 * to know if the space in question will block the object.  We can't use
 * the archetype because that isn't correct if the monster has been 
 * customized, changed states, etc.
 */

int find_free_spot(const object *ob, mapstruct *m,int x,int y,int start,int stop) {
    int i,index=0, flag;
    static int altern[SIZEOFFREE];

    for(i=start;i<stop;i++) {
        flag = ob_blocked(ob,m,x+freearr_x[i],y+freearr_y[i]);
        if(!flag)
            altern[index++]=i;

        /* Basically, if we find a wall on a space, we cut down the search size.
         * In this way, we won't return spaces that are on another side of a wall.
         * This mostly work, but it cuts down the search size in all directions - 
         * if the space being examined only has a wall to the north and empty
         * spaces in all the other directions, this will reduce the search space
         * to only the spaces immediately surrounding the target area, and
         * won't look 2 spaces south of the target space.
         */
        else if ((flag & AB_NO_PASS) && maxfree[i]<stop)
            stop=maxfree[i];
    }
    if(!index) return -1;
    return altern[RANDOM()%index];
}

/**
 * find_first_free_spot(archetype, mapstruct, x, y) works like
 * find_free_spot(), but it will search max number of squares.
 * It will return the first available spot, not a random choice.
 * Changed 0.93.2: Have it return -1 if there is no free spot available.
 *
 * @param ob
 * object to insert.
 * @param m
 * @param x
 * @param y
 * where to insert the object.
 * @return
 * index into ::freearr_x and ::freearr_y, -1 if no spot available (dir 0 = x,y)
 */
int find_first_free_spot(const object *ob, mapstruct *m,int x,int y) {
    int i;
    for(i=0;i<SIZEOFFREE;i++) {
        if(!ob_blocked(ob,m,x+freearr_x[i],y+freearr_y[i]))
            return i;
    }
    return -1;
}

/**
 * Randomly permutes an array.
 *
 * @param arr
 * array to permute.
 * @param begin
 * @param end
 * first and last (exclusive) indexes to permute.
 */
static void permute(int *arr, int begin, int end)
{
    int i, j, tmp, len;

    len = end-begin;
    for(i = begin; i < end; i++)
    {
	j = begin+RANDOM()%len;

	tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
    }
}

/**
 * New function to make monster searching more efficient, and effective! 
 * This basically returns a randomized array (in the passed pointer) of
 * the spaces to find monsters.  In this way, it won't always look for
 * monsters to the north first.  However, the size of the array passed
 * covers all the spaces, so within that size, all the spaces within
 * the 3x3 area will be searched, just not in a predictable order.
 *
 * @param search_arr
 * array that will be initialized. Must contain at least SIZEOFFREE elements.
 */
void get_search_arr(int *search_arr)
{
    int i;

    for(i = 0; i < SIZEOFFREE; i++)
    {
        search_arr[i] = i;
    }

    permute(search_arr, 1, SIZEOFFREE1+1);
    permute(search_arr, SIZEOFFREE1+1, SIZEOFFREE2+1);
    permute(search_arr, SIZEOFFREE2+1, SIZEOFFREE);
}

/**
 * Search some close squares in the given map at the given coordinates for live objects.
 *
 * @param m
 * @param x
 * @param y
 * origin from which to search.
 * @param exclude
 * an object that will be ignored. Can be NULL.
 * @return
 * direction toward the first/closest live object if it finds any, otherwise 0.
 *
 * @note
 * Perhaps incorrectly, but I'm making the assumption that exclude
 * is actually want is going to try and move there.  We need this info
 * because we have to know what movement the thing looking to move
 * there is capable of.
 */
int find_dir(mapstruct *m, int x, int y, object *exclude) {
    int i,max=SIZEOFFREE, mflags;
    sint16 nx, ny;
    object *tmp;
    mapstruct *mp;
    MoveType	blocked, move_type;

    if (exclude && exclude->head) {
        exclude = exclude->head;
        move_type = exclude->move_type;
    } else {
        /* If we don't have anything, presume it can use all movement types. */
        move_type=MOVE_ALL;
    }

    for(i=1;i<max;i++) {
        mp = m;
        nx = x + freearr_x[i];
        ny = y + freearr_y[i];

        mflags = get_map_flags(m, &mp, nx, ny, &nx, &ny);
        if (mflags & P_OUT_OF_MAP) {
            max = maxfree[i];
        } else {
            blocked = GET_MAP_MOVE_BLOCK(mp, nx, ny);

            if ((move_type & blocked) == move_type) {
            max=maxfree[i];
            } else if (mflags & P_IS_ALIVE) {
                for (tmp=GET_MAP_OB(mp,nx,ny); tmp!= NULL; tmp=tmp->above) {
                    if ((QUERY_FLAG(tmp,FLAG_MONSTER) ||  tmp->type==PLAYER) &&
                    (tmp != exclude ||(tmp->head && tmp->head != exclude))) {
                        break;
                    }
                }
                if(tmp) {
                    return freedir[i];
                }
            }
        }
    }
    return 0;
}

/**
 * Return the square of the distance between the two given objects.
 *
 * @param ob1
 * @param ob2
 * objects we want to compute the distance of.
 */
int distance(const object *ob1, const object *ob2) {
    int i;
    i= (ob1->x - ob2->x)*(ob1->x - ob2->x)+
        (ob1->y - ob2->y)*(ob1->y - ob2->y);
    return i;
}

/**
 * Computes a direction which you should travel to move of x and y.
 * @param x
 * @param y
 * delta.
 * @return
 * direction
 */
int find_dir_2(int x, int y) {
    int q;
    if(!y)
        q= -300*x;
    else
        q=x*100/y;
    if(y>0) {
        if(q < -242)
            return 3 ;
        if (q < -41)
            return 2 ;
        if (q < 41)
            return 1 ;
        if (q < 242)
            return 8 ;
        return 7 ;
    }
    if (q < -242)
        return 7 ;
    if (q < -41)
        return 6 ;
    if (q < 41)
        return 5 ;
    if (q < 242)
        return 4 ;
    return 3 ;
}

/**
 * Computes an absolute direction.
 * @param d
 * direction to convert.
 * @return
 * number between 1 and 8, which represent the "absolute" direction of a number (it actually
 * takes care of "overflow" in previous calculations of a direction).
 */
int absdir(int d) {
    while(d<1) d+=8;
    while(d>8) d-=8;
    return d;
}

/**
 * Computes a direction difference.
 * @param dir1
 * @param dir2
 * directions to compare.
 * @return
 * how many 45-degrees differences there is between two directions
 * (which are expected to be absolute (see absdir())
 */
int dirdiff(int dir1, int dir2) {
    int d;
    d = abs(dir1 - dir2);
    if(d>4)
        d = 8 - d;
    return d;
}

/**
 * Basically, this is a table of directions, and what directions
 * one could go to go back to us.  Eg, entry 15 below is 4, 14, 16.
 * This basically means that if direction is 15, then it could either go
 * direction 4, 14, or 16 to get back to where we are.
 * Moved from spell_util.c to object.c with the other related direction
 * functions.
 *
 * peterm:
 * do LOS stuff for ball lightning.  Go after the closest VISIBLE monster.
 */
int reduction_dir[SIZEOFFREE][3] = {
    {0,0,0}, /* 0 */ 
    {0,0,0}, /* 1 */
    {0,0,0}, /* 2 */
    {0,0,0}, /* 3 */
    {0,0,0}, /* 4 */
    {0,0,0}, /* 5 */
    {0,0,0}, /* 6 */
    {0,0,0}, /* 7 */
    {0,0,0}, /* 8 */
    {8,1,2}, /* 9 */
    {1,2,-1}, /* 10 */
    {2,10,12}, /* 11 */
    {2,3,-1}, /* 12 */
    {2,3,4}, /* 13 */
    {3,4,-1}, /* 14 */
    {4,14,16}, /* 15 */
    {5,4,-1}, /* 16 */
    {4,5,6}, /* 17 */
    {6,5,-1}, /* 18 */
    {6,20,18}, /* 19 */
    {7,6,-1}, /* 20 */
    {6,7,8}, /* 21 */
    {7,8,-1}, /* 22 */
    {8,22,24}, /* 23 */
    {8,1,-1}, /* 24 */
    {24,9,10}, /* 25 */
    {9,10,-1}, /* 26 */
    {10,11,-1}, /* 27 */
    {27,11,29}, /* 28 */
    {11,12,-1}, /* 29 */
    {12,13,-1}, /* 30 */
    {12,13,14}, /* 31 */
    {13,14,-1}, /* 32 */
    {14,15,-1}, /* 33 */
    {33,15,35}, /* 34 */
    {16,15,-1}, /* 35 */
    {17,16,-1}, /* 36 */
    {18,17,16}, /* 37 */
    {18,17,-1}, /* 38 */
    {18,19,-1}, /* 39 */
    {41,19,39}, /* 40 */
    {19,20,-1}, /* 41 */
    {20,21,-1}, /* 42 */
    {20,21,22}, /* 43 */
    {21,22,-1}, /* 44 */
    {23,22,-1}, /* 45 */
    {45,47,23}, /* 46 */
    {23,24,-1}, /* 47 */
    {24,9,-1}}; /* 48 */

/**
 * Recursive routine to see if we can find a path to a certain point.
 *
 * Modified to be map tile aware -.MSW
 *
 * @param m
 * map we're on
 * @param x
 * @param y
 * origin coordinates
 * @param dir
 * direction we're going to. Must be less than SIZEOFFREE.
 * @return
 * 1 if we can see a direct way to get it
 *
 * @todo
 * better document, can't figure what it does :)
 */
int can_see_monsterP(mapstruct *m, int x, int y,int dir) {
    sint16 dx, dy;
    int mflags;

    if(dir<0) return 0;  /* exit condition:  invalid direction */

    dx = x + freearr_x[dir];
    dy = y + freearr_y[dir];

    mflags = get_map_flags(m, &m, dx, dy, &dx, &dy);

    /* This functional arguably was incorrect before - it was
     * checking for P_WALL - that was basically seeing if
     * we could move to the monster - this is being more
     * literal on if we can see it.  To know if we can actually
     * move to the monster, we'd need the monster passed in or
     * at least its move type.
     */
    if (mflags & (P_OUT_OF_MAP | P_BLOCKSVIEW)) return 0;

    /* yes, can see. */
    if(dir < 9) return 1;
    return can_see_monsterP(m, x, y, reduction_dir[dir][0]) |
        can_see_monsterP(m,x,y, reduction_dir[dir][1]) |
        can_see_monsterP(m,x,y, reduction_dir[dir][2]);
}
  
  
	
/**
 * Finds out if an object can be picked up.
 *
 * Add a check so we can't pick up invisible objects (0.93.8)
 *
 * @param who
 * who is trying to pick up. Can be a monster or a player.
 * @param item
 * item we're trying to pick up.
 * @return
 * 1 if it can be picked up, otherwise 0.
 *
 * @note
 * this introduces a weight limitation for monsters.
 */
int can_pick(const object *who, const object *item) {
  return /*QUERY_FLAG(who,FLAG_WIZ)||*/
    (item->weight>0&&!QUERY_FLAG(item,FLAG_NO_PICK)&&
    !QUERY_FLAG(item,FLAG_ALIVE)&&!item->invisible &&
    (who->type==PLAYER||item->weight<who->weight/3));
}


/**
 * Create clone from object to another.
 *
 * @param asrc
 * object to clone.
 * @return
 * clone of asrc, including inventory and 'more' body parts.
 *
 * @note
 * this function will return NULL only if asrc is NULL. If there is a memory allocation error, get_object() calls fatal().
 */
object *object_create_clone (object *asrc) {
    object *dst = NULL,*tmp,*src,*part,*prev, *item;

    if(!asrc) return NULL;
    src = asrc;
    if(src->head)
        src = src->head;

    prev = NULL;
    for(part = src; part; part = part->more) {
        tmp = get_object();
        copy_object(part,tmp);
        tmp->x -= src->x;
        tmp->y -= src->y;
        if(!part->head) {
            dst = tmp;
            tmp->head = NULL;
        } else {
            tmp->head = dst;
        }
        tmp->more = NULL;
        if(prev)
            prev->more = tmp;
        prev = tmp;
    }
    /*** copy inventory ***/
    for(item = src->inv; item; item = item->below) {
        (void) insert_ob_in_ob(object_create_clone(item),dst);
    }

    return dst;
}

/**
 * Find object in inventory.
 *
 * @param who
 * where to search.
 * @param type
 * @param subtype
 * what to search.
 * @return 
 * first object in who's inventory that has the same type and subtype match. NULL if no match.
 *
 * @note
 * will not search in inventory of items in inventory.
 */
object *find_obj_by_type_subtype(const object *who, int type, int subtype)
{
    object *tmp;

    for (tmp=who->inv; tmp; tmp=tmp->below)
        if (tmp->type == type && tmp->subtype == subtype) return tmp;

    return NULL;
}

/**
 * Search for a field by key.
 *
 * @param ob
 * object where search
 * @param key
 * key to search. Must be a passed in shared string - otherwise, this won't do the desired thing.
 * @return
 * the link from the list if ob has a field named key, otherwise NULL. 
 */
key_value * get_ob_key_link(const object * ob, const char * key) {
    key_value * link;

    for (link = ob->key_values; link != NULL; link = link->next) {
        if (link->key == key) {
            return link;
        }
    }

    return NULL;
}

/**
 * Get an extra value by key.
 *
 * @param op
 * object we're considering
 * @param key
 * key of which to retrieve the value. Doesn't need to be a shared string.
 * @return
 * the value of op has in an extra_field for key, or NULL if it doesn't have the key.
 *
 * @note
 * The returned string is shared.
 */
const char * get_ob_key_value(const object * op, const char * const key) {
    key_value * link;
    const char * canonical_key;

    canonical_key = find_string(key);

    if (canonical_key == NULL) {
        /* 1. There being a field named key on any object
         *    implies there'd be a shared string to find.
         * 2. Since there isn't, no object has this field.
         * 3. Therefore, *this* object doesn't have this field.
         */
        return NULL;
    }

    /* This is copied from get_ob_key_link() above -
     * only 4 lines, and saves the function call overhead.
     */
    for (link = op->key_values; link != NULL; link = link->next) {
        if (link->key == canonical_key) {
            return link->value;
        }
    }
    return NULL;
}


/**
 * Updates or sets a key value.
 *
 * @param op
 * object we're considering.
 * @param canonical_key
 * key to set or update. Must be a shared string.
 * @param value
 * value to set. Doesn't need to be a shared string.
 * @param add_key
 * if 0, will not add the key if it doesn't exist in op.
 * @return
 * TRUE if key was updated or added, FALSE else.
 */
static int set_ob_key_value_s(object * op, const char * canonical_key, const char * value, int add_key) {
    key_value * field = NULL, *last=NULL;

    LOG(llevDebug, "set_ob_value_s: '%s' '%s' %d\n", canonical_key, value, add_key);

    for (field=op->key_values; field != NULL; field=field->next) {
        if (field->key != canonical_key) {
            last = field;
            continue;
        }

        if (field->value) FREE_AND_CLEAR_STR(field->value);
        if (value)
            field->value = add_string(value);
        else {
            /* Basically, if the archetype has this key set,
             * we need to store the null value so when we save
             * it, we save the empty value so that when we load,
             * we get this value back again.
             */
            if (get_ob_key_link(&op->arch->clone, canonical_key))
                field->value = NULL;
            else {
                /* Delete this link */
                if (field->key) FREE_AND_CLEAR_STR(field->key);
                if (field->value) FREE_AND_CLEAR_STR(field->value);
                if (last) last->next = field->next;
                else op->key_values = field->next;
                free(field);
            }
        }
        return TRUE;
    }
    /* IF we get here, key doesn't exist */

    /* No field, we'll have to add it. */

    if (!add_key) {
        return FALSE;
    }
    /* There isn't any good reason to store a null
     * value in the key/value list.  If the archetype has
     * this key, then we should also have it, so shouldn't
     * be here.  If user wants to store empty strings,
     * should pass in ""
     */
    if (value == NULL) return TRUE;

    field = malloc(sizeof(key_value));

    field->key = add_refcount(canonical_key);
    field->value = add_string(value);
    /* Usual prepend-addition. */
    field->next = op->key_values;
    op->key_values = field;

    return TRUE;
}

/**
 * Updates the key in op to value.
 *
 * @param op
 * object we're considering.
 * @param key
 * key to set or update. Doesn't need to be a shared string.
 * @param value
 * value to set. Doesn't need to be a shared string.
 * @param add_key
 * if 0, will not add the key if it doesn't exist in op.
 * @return
 * TRUE if key was updated or added, FALSE else.
 *
 * @note
 * This function is merely a wrapper to set_ob_key_value_s() to ensure the key is a shared string.
 *
 * @note
 * In general, should be little reason FALSE is ever passed in for add_key
*/
int set_ob_key_value(object * op, const char * key, const char * value, int add_key) {
    const char * canonical_key = NULL;
    int floating_ref = FALSE;
    int ret;

    /* HACK This mess is to make sure set_ob_value() passes a shared string
     * to get_ob_key_link(), without leaving a leaked refcount. 
     */

    canonical_key = find_string(key);
    if (canonical_key == NULL) {
        canonical_key = add_string(key);
        floating_ref = TRUE;
    }

    ret = set_ob_key_value_s(op, canonical_key, value, add_key);

    if (floating_ref) {
        free_string(canonical_key);
    }

    return ret;
}

/** This is a subset of the parse_id command.  Basically, name can be
 * a string seperated lists of things to match, with certain keywords.
 * pl is the player (only needed to set count properly)
 * op is the item we are trying to match.  Calling function takes care
 * of what action might need to be done and if it is valid
 * (pickup, drop, etc.)  Return NONZERO if we have a match.  A higher
 * value means a better match.  0 means no match.
 *
 * Brief outline of the procedure:
 * We take apart the name variable into the individual components.
 * cases for 'all' and unpaid are pretty obvious.
 * Next, we check for a count (either specified in name, or in the
 * player object.)
 * If count is 1, make a quick check on the name.
 * IF count is >1, we need to make plural name.  Return if match.
 * Last, make a check on the full name.
 *
 * Details on values output (highest is output):
 * match type                 return value
 * ---------------------------------------
 * nothing                    0
 * 'all'                      1
 * 'unpaid'                   2
 * 'cursed'                   2
 * 'unlocked'                 2
 * partial custom name        3
 * op->name with count >1     4
 * op->name with count <2     6
 * op->name_pl with count >1  6
 * inside base name           12
 * inside short name          12
 * begin of base name         14
 * custom name                15
 * base name                  16
 * short name                 18
 * full name                  20
 * (note, count is extracted from begin of name parameter or 
 *  from pl->contr->count, name has priority)
 *
 * @param pl
 * object we're searching an item for. Must not be NULL.
 * @param op
 * object we're considering. Must not be NULL.
 * @param name
 * string we're searching.
 * @return
 * matching weight. The higher, the more the object matches.
 *
 * @todo
 * is the player->contr->count hack used?? Try to reduce buffers/calls to query_ functions.
 */
int item_matched_string(object *pl, object *op, const char *name)
{
    char *cp, local_name[MAX_BUF], name_op[MAX_BUF], name_short[HUGE_BUF], bname_s[MAX_BUF], bname_p[MAX_BUF];
    int count,retval=0;
    strcpy(local_name, name);   /* strtok is destructive to name */

    for (cp=strtok(local_name,","); cp; cp=strtok(NULL,",")) {
        while (cp[0]==' ') ++cp;    /* get rid of spaces */

        /*  LOG(llevDebug,"Trying to match %s\n", cp);*/
        /* All is a very generic match - low match value */
        if (!strcmp(cp,"all")) return 1;

        /* unpaid is a little more specific */
        if (!strcmp(cp,"unpaid") && QUERY_FLAG(op,FLAG_UNPAID)) return 2;
        if (!strcmp(cp,"cursed") && QUERY_FLAG(op,FLAG_KNOWN_CURSED) &&
            (QUERY_FLAG(op,FLAG_CURSED) ||QUERY_FLAG(op,FLAG_DAMNED)))
            return 2;

        if (!strcmp(cp,"unlocked") && !QUERY_FLAG(op, FLAG_INV_LOCKED))
            return 2;

        /* Allow for things like '100 arrows' */
        if ((count=atoi(cp))!=0) {
            cp=strchr(cp, ' ');
            while (cp && cp[0]==' ') ++cp;  /* get rid of spaces */
        }
        else {
            if (pl->type==PLAYER)
            count=pl->contr->count;
            else
            count = 0;
        }

        if (!cp || cp[0]=='\0' || count<0) return 0;


        /* The code here should go from highest retval to lowest.  That
         * is because of the 'else' handling - we don't want to match on
         * something and set a low retval, even though it may match a higher retcal
         * later.  So keep it in descending order here, so we try for the best
         * match first, and work downward.
         */
        query_name(op, name_op, MAX_BUF);
        query_short_name(op, name_short, HUGE_BUF);
        query_base_name(op,0, bname_s, MAX_BUF);
        query_base_name(op,1, bname_p, MAX_BUF);

        if (!strcasecmp(cp,name_op)) retval=20;
        else if (!strcasecmp(cp,name_short)) retval=18;
        else if (!strcasecmp(cp,bname_s)) retval=16;
        else if (!strcasecmp(cp,bname_p)) retval=16;
        else if (op->custom_name && !strcasecmp(cp,op->custom_name)) retval=15;
        else if (!strncasecmp(cp,bname_s,
                    strlen(cp))) retval=14;
        else if (!strncasecmp(cp,bname_p,
                    strlen(cp))) retval=14;

        /* Do substring checks, so things like 'Str+1' will match.
         * retval of these should perhaps be lower - they are lower
         * then the specific strcasecmp aboves, but still higher than
         * some other match criteria.
         */
        else if (strstr(bname_p, cp)) retval = 12;
        else if (strstr(bname_s, cp)) retval = 12;
        else if (strstr(name_short, cp)) retval = 12;

        /* Check against plural/non plural based on count. */
        else if (count>1 && !strcasecmp(cp,op->name_pl)) {
            retval=6;
        }
        else if (count==1 && !strcasecmp(op->name,cp)) {
            retval=6;
        }
        /* base name matched - not bad */
        else if (strcasecmp(cp,op->name)==0 && !count) retval=4;
        /* Check for partial custom name, but give a real low priority */
        else if (op->custom_name && strstr(op->custom_name, cp)) retval = 3;

        if (retval) {
            if (pl->type == PLAYER)
            pl->contr->count=count;
            return retval;
        }
    }
    return 0;
}

/**
 * Ensures specified object has its more parts correctly inserted in map.
 *
 * Extracted from common/map.c:link_multipart_objects
 *
 * @param tmp
 * object we want to fix. Must be on a map.
 **/
void fix_multipart_object(object* tmp)
{
    archetype	*at;
    object *op, *last;

    if (!tmp->map)
    {
        LOG(llevError, "fix_multipart_object: not on a map!\n");
        return;
    }

    /* already multipart - don't do anything more */
    if (tmp->head || tmp->more)
        return;

    /* If there is nothing more to this object, this for loop
     * won't do anything.
     */
    for (at = tmp->arch->more, last=tmp; at != NULL; at=at->more, last=op) {
        op = arch_to_object(at);

        /* update x,y coordinates */
        op->x += tmp->x;
        op->y += tmp->y;
        op->head = tmp;
        op->map = tmp->map;
        last->more = op;
        if (tmp->name != op->name) {
            if (op->name) free_string(op->name);
            op->name = add_string(tmp->name);
        }
        if (tmp->title != op->title) {
            if (op->title) free_string(op->title);
            op->title = add_string(tmp->title);
        }
        /* we could link all the parts onto tmp, and then just
         * call insert_ob_in_map once, but the effect is the same,
         * as insert_ob_in_map will call itself with each part, and
         * the coding is simpler to just to it here with each part.
         */
        insert_ob_in_map(op, op->map, tmp,INS_NO_MERGE|INS_ABOVE_FLOOR_ONLY|INS_NO_WALK_ON);
    } /* for at = tmp->arch->more */
}

/**
 * Computes the size of a multitile object.
 *
 * @param ob
 * object we compute the size of.
 * @param[out] sx
 * @param[out] sy
 * will contain the coords of the bottom right tail relative to the head. Must not be NULL.
 * @param[out] hx
 * @param[out] hy
 * will contain the coords of the head tile relative to the top left tile. Can be NULL.
 *
 * @todo
 * either check for sx/sy everywhere or remove the check :)
 */
static void get_multi_size(object *ob, int *sx, int *sy, int *hx, int *hy) {
    archetype *part;
    int maxx=0, maxy=0, minx=0, miny=0;
    if (ob->head)
        ob = ob->head;
    *sx = 1;
    *sy = 1;
    if (ob->arch->more){
        for(part = ob->arch; part; part = part->more) {
            if (part->clone.x > maxx)
                maxx = part->clone.x;
            if (part->clone.y > maxy)
                maxy = part->clone.y;
            if (part->clone.x < minx)
                minx = part->clone.x;
            if (part->clone.y < miny)
                miny = part->clone.y;
        }
    }
    if (sx) *sx = maxx;
    if (sy) *sy = maxy;
    if (hx) *hx = -minx;
    if (hy) *hy = -miny;
}

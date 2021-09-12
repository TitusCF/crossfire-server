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
 * @file anim.c
 * This file contains animation-related code.
 **/

#include "global.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assets.h"

/**
 * Updates the face-variable of an object.
 * If the object is the head of a multi-object, all objects are animated.
 * The object's state is not changed, but merely updated if needed (out of bounds of
 * new animation, reached end of animation, ...).
 * This function correctly handles FLAG_IS_TURNABLE too.
 *
 * @param op is the object to animate.
 * @param dir is the direction the object is facing.  This is generally same as
 *    op->direction, but in some cases, op->facing is used instead - the
 *    caller has a better idea which one it really wants to be using,
 *    so let it pass along the right one. This parameter is ignored if
 *    the object has a head or is turnable.
 */
void animate_object(object *op, int dir) {
    int max_state;  /* Max animation state object should be drawn in */
    int base_state; /* starting index # to draw from */
    uint16_t oldface = op->face->number;

    if (!op->animation || !NUM_ANIMATIONS(op)) {
        StringBuffer *sb;
        char *diff;

        LOG(llevError, "Object lacks animation.\n");
        sb = stringbuffer_new();
        object_dump(op, sb);
        diff = stringbuffer_finish(sb);
        LOG(llevError, "%s", diff);
        free(diff);
        return;
    }

    if (op->head) {
        dir = op->head->direction;

        if (NUM_ANIMATIONS(op) == NUM_ANIMATIONS(op->head))
            op->state = op->head->state;
    } else if (QUERY_FLAG(op, FLAG_IS_TURNABLE)) {
        dir = op->direction;
    }

    /* If object is turning, then max animation state is half through the
     * animations.  Otherwise, we can use all the animations.
     */
    max_state = NUM_ANIMATIONS(op)/NUM_FACINGS(op);
    base_state = 0;
    /* at least in the older aniamtions that used is_turning, the first half
     * of the animations were left facing, the second half right facing.
     * Note in old the is_turning, it was set so that the animation for a monster
     * was always towards the enemy - now it is whatever direction the monster
     * is facing.
     */
    if (NUM_FACINGS(op) == 2) {
        if (dir < 5)
            base_state = 0;
        else
            base_state = NUM_ANIMATIONS(op)/2;
    } else if (NUM_FACINGS(op) == 4) {
        if (dir == 0)
            base_state = 0;
        else
            base_state = ((dir-1)/2)*(NUM_ANIMATIONS(op)/4);
    } else if (NUM_FACINGS(op) == 8) {
        if (dir == 0)
            base_state = 0;
        else
            base_state = (dir-1)*(NUM_ANIMATIONS(op)/8);
    } else if (QUERY_FLAG(op, FLAG_IS_TURNABLE)) {
        base_state = (NUM_ANIMATIONS(op) / 9) * (dir);
        max_state = NUM_ANIMATIONS(op) / 9;
    }

    /* If beyond drawable states, reset */
    if (op->state >= max_state) {
        op->state = 0;
        if (op->temp_animation) {
            op->temp_animation = 0;
            animate_object(op, dir);
            return;
        }
    }
    SET_ANIMATION(op, op->state+base_state);

    if (op->face == blank_face)
        op->invisible = 1;

    /* This block covers monsters (eg, pixies) which are supposed to
     * cycle from visible to invisible and back to being visible.
     * as such, disable it for players, as then players would become
     * visible.
     */
    else if (op->type != PLAYER && QUERY_FLAG((&op->arch->clone), FLAG_ALIVE)) {
        if (op->face->number == 0) {
            op->invisible = 1;
            CLEAR_FLAG(op, FLAG_ALIVE);
        } else {
            op->invisible = 0;
            SET_FLAG(op, FLAG_ALIVE);
        }
    }

    if (op->more)
        animate_object(op->more, dir);

    /* object_update() will also recursively update all the pieces.
     * as such, we call it last, and only call it for the head
     * piece, and not for the other tail pieces.
     */
    if (!op->head && (oldface != op->face->number))
        object_update(op, UP_OBJ_FACE);
}

/**
 * Applies a compound animation to an object.
 *
 * @param who
 * object to apply the animation to. Must not be NULL.
 * @param suffix
 * animation suffix to apply. Must not be NULL.
 */
void apply_anim_suffix(object *who, const char *suffix) {
    const Animations *anim;
    object *head, *orig;
    char buf[MAX_BUF];

    assert(who);
    assert(suffix);

    if (who->temp_animation)
        /* don't overlap animation, let the current one finish. */
        return;

    head = HEAD(who);
    orig = head;
    snprintf(buf, MAX_BUF, "%s_%s", (head->animation ? head->animation->name : ""), suffix);
    anim = try_find_animation(buf);
    if (anim) {
        for (; head != NULL; head = head->more) {
            head->temp_animation = anim;
            head->temp_anim_speed = anim->num_animations / anim->facings;
            head->last_anim = 0;
            head->state = 0;
        }
        animate_object(orig, orig->facing);
    }
}

static void do_anim(const Animations *anim) {
    fprintf(stderr, "%5d %50s %5d\n", anim->num, anim->name, anim->num_animations);
}

/**
 * Dump all animations to stderr, for debugging purposes.
 */
void dump_animations(void) {
    fprintf(stderr, "id    name                                               faces\n");
    animations_for_each(do_anim);
}

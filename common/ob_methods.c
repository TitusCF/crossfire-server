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
 * @file
 * Those functions deal with the object/type system.
 */

#include <global.h>
#include <ob_methods.h>
#include <ob_types.h>
#include <sproto.h>

/*
 * The following functions are meant for calling methods. No actual behavoir
 * logic should be contained in this code. Code in the common/ directory should
 * be used for logic common to all types, and should always be called by
 * individual method code (i.e. all apply methods should call 'can_apply' from
 * common/). Defaults for all types should not be put here either, as that code
 * belongs in the common/ directory also, referenced to by base_type.
 */

/**
 * Apply an object by running an event hook or an object method. Consider
 * using apply_manual() instead of this function if the applier should check
 * for apply restrictions.
 * @param op The object to apply
 * @param applier The object that executes the apply action
 * @param aflags Special (always apply/unapply) flags
 * @retval 0 A player or monster can't apply objects of that type
 * @retval 1 has been applied, or there was an error applying the object
 * @retval 2 objects of that type can't be applied if not in inventory
 */
method_ret ob_apply(object *op, object *applier, int aflags) {
    method_ret ret;
    ob_methods *methods;

    if (events_execute_object_event(op, EVENT_APPLY, applier, NULL, NULL, SCRIPT_FIX_ALL) != 0)
        return METHOD_OK;

    for (methods = &type_methods[op->type]; methods; methods = methods->fallback) {
        if (methods->apply) {
            ret = methods->apply(op, applier, aflags);
            if (ret != METHOD_UNHANDLED)
                return ret;
        }
    }
    return METHOD_UNHANDLED;
}

/**
 * Processes an object, giving it the opportunity to move or react.
 * Note: The return value of ob_process doesn't seem to be used anymore.
 * @param op The object to process
 * @retval METHOD_UNHANDLED if the process method does not exist for that objec,
 */
method_ret ob_process(object *op) {
    method_ret ret;
    ob_methods *methods;

    for (methods = &type_methods[op->type]; methods; methods = methods->fallback) {
        if (methods->process) {
            ret = methods->process(op);
            if (ret != METHOD_UNHANDLED)
                return ret;
        }
    }
    return METHOD_UNHANDLED;
}

/**
 * Returns the description of an object, as seen by the given observer.
 * @param op The object to describe
 * @param observer The object to which the description is made
 * @param use_media_tags whether to add media tags or not on the description
 * @param buf Buffer that will contain the description
 * @param size Size of buf
 * @return
 * buf.
 */
char *ob_describe(const object *op, const object *observer, int use_media_tags, char *buf, size_t size) {
    ob_methods *methods;

    for (methods = &type_methods[op->type]; methods; methods = methods->fallback) {
        if (methods->describe) {
            methods->describe(op, observer, use_media_tags, buf, size);
            return buf;
        }
    }
    buf[0] = '\0';
    return buf;
}

/**
 * Makes an object move on top of another one.
 * @param op The object over which to move
 * @param victim The object moving over op
 * @param originator The object that is the cause of the move
 * @retval METHOD_UNHANDLED if the process method does not exist for that object
 */
method_ret ob_move_on(object *op, object *victim, object *originator) {
    method_ret ret;
    ob_methods *methods;

    for (methods = &type_methods[op->type]; methods; methods = methods->fallback) {
        if (methods->move_on) {
            ret = methods->move_on(op, victim, originator);
            if (ret != METHOD_UNHANDLED)
                return ret;
        }
    }
    return METHOD_UNHANDLED;
}

/**
 * An object is triggered by another one.
 * @param op The object being triggered
 * @param cause The object that is the cause of the trigger
 * @param state trigger state, 0 for released, other for pushed
 * @retval METHOD_UNHANDLED if the process method does not exist for that object
 * @todo check the exact state values/meaning
 */
method_ret ob_trigger(object *op, object *cause, int state) {
    method_ret ret;
    ob_methods *methods;

    for (methods = &type_methods[op->type]; methods; methods = methods->fallback) {
        if (methods->trigger) {
            ret = methods->trigger(op, cause, state);
            if (ret != METHOD_UNHANDLED)
                return ret;
        }
    }
    return METHOD_UNHANDLED;
}

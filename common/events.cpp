extern "C" {
#include <stdarg.h>
#include "global.h"
}
#include "events.h"
#include <map>

// If set then dump information about found event connectors
//#define EVENTS_DEBUG

static std::map<event_registration, f_plug_event> global_handlers[NR_EVENTS];
static event_registration next_event_registration = 1;
static std::map<std::string, f_plug_event> object_handlers;

event_registration events_register_global_handler(int eventcode, f_plug_event hook) {
    auto eg = next_event_registration;
    next_event_registration++;
    global_handlers[eventcode][eg] = hook;
    return eg;
}

void events_unregister_global_handler(int eventcode, event_registration id) {
    global_handlers[eventcode].erase(id);
}

void events_execute_global_event(int eventcode, ...) {
    va_list args;
    mapstruct *map;
    object *op;
    object *op2;
    player *pl;
    const char *buf;
    int i, rt;

    va_start(args, eventcode);

    switch (eventcode) {
    case EVENT_BORN:
        /*BORN: op*/
        op = va_arg(args, object *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op);
        }
        break;

    case EVENT_CLOCK:
        /*CLOCK: -*/
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode);
        }
        break;

    case EVENT_CRASH:
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode);
        }
        break;

    case EVENT_PLAYER_DEATH:
        /*PLAYER_DEATH: op*/
        op = va_arg(args, object *);
        op2 = va_arg(args, object *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, op2);
        }
        break;

    case EVENT_GKILL:
        /*GKILL: op, hitter*/
        op = va_arg(args, object *);
        op2 = va_arg(args, object *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, op2);
        }
        break;

    case EVENT_LOGIN:
        /*LOGIN: pl, pl->socket.host*/
        pl = va_arg(args, player *);
        buf = va_arg(args, char *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, pl, buf);
        }
        break;

    case EVENT_LOGOUT:
        /*LOGOUT: pl, pl->socket.host*/
        pl = va_arg(args, player *);
        buf = va_arg(args, char *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, pl, buf);
        }
        break;

    case EVENT_MAPENTER:
        /*MAPENTER: op, map*/
        op = va_arg(args, object *);
        map = va_arg(args, mapstruct *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, map);
        }
        break;

    case EVENT_MAPLEAVE:
        /*MAPLEAVE: op, map*/
        op = va_arg(args, object *);
        map = va_arg(args, mapstruct *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, map);
        }
        break;

    case EVENT_MAPRESET:
        /*MAPRESET: map*/
        map = va_arg(args, mapstruct *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, map);
        }
        break;

    case EVENT_REMOVE:
        /*REMOVE: op*/
        op = va_arg(args, object *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op);
        }
        break;

    case EVENT_SHOUT:
        /*SHOUT: op, parms, priority*/
        op = va_arg(args, object *);
        buf = va_arg(args, char *);
        i = va_arg(args, int);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, buf, i);
        }
        break;

    case EVENT_TELL:
        /* Tell: who, what, to who */
        op = va_arg(args, object *);
        buf = va_arg(args, const char *);
        op2 = va_arg(args, object *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, buf, op2);
        }
        break;

    case EVENT_MUZZLE:
        /*MUZZLE: op, parms*/
        op = va_arg(args, object *);
        buf = va_arg(args, char *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, buf);
        }
        break;

    case EVENT_KICK:
        /*KICK: op, parms*/
        op = va_arg(args, object *);
        buf = va_arg(args, char *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, op, buf);
        }
        break;

    case EVENT_MAPUNLOAD:
        /*MAPUNLOAD: map*/
        map = va_arg(args, mapstruct *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, map);
        }
        break;

    case EVENT_MAPLOAD:
        /*MAPLOAD: map*/
        map = va_arg(args, mapstruct *);
        for (auto gh = global_handlers[eventcode].begin(); gh != global_handlers[eventcode].end(); gh++) {
            (*(*gh).second)(&rt, eventcode, map);
        }
        break;
    }
    va_end(args);
}

static int do_execute_event(object *op, int eventcode, object *activator, object *third, const char *message, int fix, talk_info *talk) {
    int rv = 0;

    FOR_INV_PREPARE(op, tmp) {
        if (tmp->type == EVENT_CONNECTOR && tmp->subtype == eventcode) {
#ifdef EVENTS_DEBUG
            LOG(llevDebug, "********** EVENT HANDLER **********\n");
            LOG(llevDebug, " - Who am I      :%s\n", op->name);
            if (activator != NULL)
                LOG(llevDebug, " - Activator     :%s\n", activator->name);
            if (third != NULL)
                LOG(llevDebug, " - Other object  :%s\n", third->name);
            LOG(llevDebug, " - Event code    :%d\n", tmp->subtype);
            if (tmp->title != NULL)
                LOG(llevDebug, " - Event plugin  :%s\n", tmp->title);
            if (tmp->slaying != NULL)
                LOG(llevDebug, " - Event hook    :%s\n", tmp->slaying);
            if (tmp->name != NULL)
                LOG(llevDebug, " - Event options :%s\n", tmp->name);
#endif

            if (tmp->title == NULL) {
                object *env = object_get_env_recursive(tmp);
                LOG(llevError, "Event object without title at %d/%d in map %s\n", env->x, env->y, env->map->name);
                object_remove(tmp);
                object_free(tmp, FREE_OBJ_NO_DESTROY_CALLBACK);
            } else if (tmp->slaying == NULL) {
                object *env = object_get_env_recursive(tmp);
                LOG(llevError, "Event object without slaying at %d/%d in map %s\n", env->x, env->y, env->map->name);
                object_remove(tmp);
                object_free(tmp, FREE_OBJ_NO_DESTROY_CALLBACK);
            } else {
                auto handler = object_handlers.find(tmp->title);
                if (handler == object_handlers.end()) {
                    object *env = object_get_env_recursive(tmp);
                    LOG(llevError, "The requested handler doesn't exist: %s at %d/%d in map %s\n", tmp->title, env->x, env->y, env->map->name);
                    object_remove(tmp);
                    object_free(tmp, FREE_OBJ_NO_DESTROY_CALLBACK);
                } else {
                    int rvt = 0;
                    int rv;

                    tag_t oldtag = op->count;
                    rv = (*(*handler).second)(&rvt, op, activator, third, message, fix, tmp, talk);
                    if (object_was_destroyed(op, oldtag)) {
                        return rv;
                    }
                    if (QUERY_FLAG(tmp, FLAG_UNIQUE)) {
#ifdef EVENTS_DEBUG
                        LOG(llevDebug, "Removing unique event %s\n", tmp->slaying);
#endif
                        object_remove(tmp);
                        object_free(tmp, FREE_OBJ_NO_DESTROY_CALLBACK);
                    }
                    return rv;
                }
            }
        }
    } FOR_INV_FINISH();
    return rv;
}

void events_register_object_handler(const char *id, f_plug_event handler) {
    object_handlers[id] = handler;
    LOG(llevDebug, "events: registered object handler %s\n", id);
}

void events_unregister_object_handler(const char *id) {
    object_handlers.erase(id);
    LOG(llevDebug, "events: unregistered object handler %s\n", id);
}

int events_execute_object_event(object *op, int eventcode, object *activator, object *third, const char *message, int fix) {
    return do_execute_event(op, eventcode, activator, third, message, fix, NULL);
}

int events_execute_object_say(object *npc, talk_info *talk) {
    return do_execute_event(npc, EVENT_SAY, talk->who, NULL, talk->text, SCRIPT_FIX_ALL, talk);
}

int events_execute_object_user(object *op, object *activator, object *third, const char *message, int fix) {
    return events_execute_object_event(op, EVENT_USER, activator, third, message, fix);
}

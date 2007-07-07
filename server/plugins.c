/*
 * static char *rcsid_plugins_c =
 *   "$Id$";
 */

/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000-2006 Mark Wedel & Crossfire Development Team          */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */
/*  The authors can be reached via e-mail to crossfire-devel@real-time.com   */
/*****************************************************************************/
/* This is the server-side plugin management part.                           */
/*****************************************************************************/
/* Original code by Yann Chachkoff (yann.chachkoff@mailandnews.com).         */
/* Special thanks to:                                                        */
/* David Delbecq                   (david.delbecq@mailandnews.com);          */
/* Joris Bontje                    (jbontje@suespammers.org);                */
/* Philip Currlin                  (?);                                      */
/*****************************************************************************/

/**
 * @file
 * Plugin API.
 *
 * @todo
 * describe "wrappers" for functions, conventions used (parameters + return value).
 */

#if 0
/** If set, will log much info about plugin activity. */
#define PLUGIN_DEBUG
#endif

/*****************************************************************************/
/* First, the headers. We only include plugin.h, because all other includes  */
/* are done into it, and plugproto.h (which is used only by this file).      */
/*****************************************************************************/
#include <plugin.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#include <timers.h>
#endif

#define NR_OF_HOOKS 81

static const hook_entry plug_hooks[NR_OF_HOOKS] =
{
    {cfapi_system_add_string,       0, "cfapi_system_add_string"},
    {cfapi_system_register_global_event,   1, "cfapi_system_register_global_event"},
    {cfapi_system_remove_string,    2, "cfapi_system_remove_string"},
    {cfapi_system_unregister_global_event, 3, "cfapi_system_unregister_global_event"},
    {cfapi_system_check_path,       4, "cfapi_system_check_path"},
    {cfapi_system_re_cmp,           5, "cfapi_system_re_cmp"},
    {cfapi_system_strdup_local,     6, "cfapi_system_strdup_local"},
    {cfapi_system_directory,        7, "cfapi_system_directory"},
    {cfapi_system_find_animation,   8, "cfapi_system_find_animation"},
    {cfapi_object_clean_object,     9, "cfapi_object_clean_object"},
    {cfapi_object_on_same_map,      10, "cfapi_object_on_same_map"},
    {cfapi_object_get_key,          11, "cfapi_object_get_key"},
    {cfapi_object_set_key,          12, "cfapi_object_set_key"},
    {cfapi_object_get_property,     13, "cfapi_object_get_property"},
    {cfapi_object_set_property,     14,"cfapi_object_set_property"},
    {cfapi_object_apply,            15, "cfapi_object_apply"},
    {cfapi_object_identify,         16, "cfapi_object_identify"},
    {cfapi_object_describe,         17, "cfapi_object_describe"},
    {cfapi_object_drain,            18, "cfapi_object_drain"},
    {cfapi_object_fix,              19, "cfapi_object_fix"},
    {cfapi_object_give_skill,       20, "cfapi_object_give_skill"},
    {cfapi_object_transmute,        21, "cfapi_object_transmute"},
    {cfapi_object_remove,           22, "cfapi_object_remove"},
    {cfapi_object_delete,           23, "cfapi_object_delete"},
    {cfapi_object_clone,            24, "cfapi_object_clone"},
    {cfapi_object_find,             25, "cfapi_object_find"},
    {cfapi_object_create,           26, "cfapi_object_create"},
    {cfapi_object_insert,           27, "cfapi_object_insert"},
    {cfapi_object_split,            28, "cfapi_object_split"},
    {cfapi_object_merge,            29, "cfapi_object_merge"},
    {cfapi_object_distance,         30, "cfapi_object_distance"},
    {cfapi_object_update,           31, "cfapi_object_update"},
    {cfapi_object_clear,            32, "cfapi_object_clear"},
    {cfapi_object_reset,            33, "cfapi_object_reset"},
    {cfapi_object_check_inventory,  34, "cfapi_object_check_inventory"},
    {cfapi_object_spring_trap,      35, "cfapi_object_spring_trap"},
    {cfapi_object_check_trigger,    36, "cfapi_object_check_trigger"},
    {cfapi_object_query_cost,       37, "cfapi_object_query_cost"},
    {cfapi_object_query_money,      38, "cfapi_object_query_money"},
    {cfapi_object_cast,             39, "cfapi_object_cast"},
    {cfapi_object_learn_spell,      40, "cfapi_object_learn_spell"},
    {cfapi_object_forget_spell,     41, "cfapi_object_forget_spell"},
    {cfapi_object_check_spell,      42, "cfapi_object_check_spell"},
    {cfapi_object_pay_amount,       43, "cfapi_object_pay_amount"},
    {cfapi_object_pay_item,         44, "cfapi_object_pay_item"},
    {cfapi_object_transfer,         45, "cfapi_object_transfer"},
    {cfapi_object_drop,             46, "cfapi_object_drop"},
    {cfapi_object_change_abil,      47, "cfapi_object_change_abil"},
    {cfapi_object_find_archetype_inside, 48, "cfapi_object_find_archetype_inside"},
    {cfapi_object_say,              49, "cfapi_object_say"},
    {cfapi_map_get_map,             50, "cfapi_map_get_map"},
    {cfapi_map_has_been_loaded,     51, "cfapi_map_has_been_loaded"},
    {cfapi_map_create_path,         52, "cfapi_map_create_path"},
    {cfapi_map_get_map_property,    53, "cfapi_map_get_property"},
    {cfapi_map_set_map_property,    54, "cfapi_map_set_property"},
    {cfapi_map_out_of_map,          55, "cfapi_map_out_of_map"},
    {cfapi_map_update_position,     56, "cfapi_map_update_position"},
    {cfapi_map_delete_map,          57, "cfapi_map_delete_map"},
    {cfapi_map_message,             58, "cfapi_map_message"},
    {cfapi_map_get_object_at,       59, "cfapi_map_get_object_at"},
    {cfapi_map_change_light,        60, "cfapi_map_change_light"},
    {cfapi_map_present_arch_by_name,61, "cfapi_map_present_arch_by_name"},
    {cfapi_player_find,             62, "cfapi_player_find"},
    {cfapi_player_message,          63, "cfapi_player_message"},
    {cfapi_object_change_exp       ,64, "cfapi_object_change_exp"},
    {cfapi_object_teleport,         65, "cfapi_object_teleport"},
    {cfapi_object_pickup,           67, "cfapi_object_pickup"},
    {cfapi_object_move,             68, "cfapi_object_move"},
    {cfapi_object_apply_below,      69, "cfapi_object_apply_below"},
    {cfapi_archetype_get_property,  71, "cfapi_archetype_get_property"},
    {cfapi_party_get_property,      72, "cfapi_party_get_property"},
    {cfapi_region_get_property,     73, "cfapi_region_get_property"},
    {cfapi_player_can_pay,          74, "cfapi_player_can_pay"},
    {cfapi_log,                     75, "cfapi_log"},
    {cfapi_get_time,                76, "cfapi_system_get_time"},
    {cfapi_timer_create,            77, "cfapi_system_timer_create"},
    {cfapi_timer_destroy,           78, "cfapi_system_timer_destroy"},
    {cfapi_friendlylist_get_next,   79, "cfapi_friendlylist_get_next"},
    {cfapi_set_random_map_variable, 80, "cfapi_set_random_map_variable"},
    {cfapi_generate_random_map,     70, "cfapi_generate_random_map"},
};
int plugin_number = 0;
crossfire_plugin* plugins_list = NULL;

/*****************************************************************************/
/* NEW PLUGIN STUFF STARTS HERE                                              */
/*****************************************************************************/

#ifdef WIN32
static const char *plugins_dlerror(void)
{
    static char buf[256];
    DWORD err;
    char* p;
    err = GetLastError();
    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, buf, sizeof(buf), NULL) == 0)
        snprintf(buf, sizeof(buf), "error %lu", err);
    p = strchr(buf, '\0');
    while (p > buf && (p[-1] == '\r' || p[-1] == '\n'))
        p--;
    *p = '\0';
    return buf;
}
#endif /* WIN32 */

/**
 * Notify clients about a changed object.
 *
 * @param op the object that has changed
 */
static void send_changed_object(object *op)
{
    object* tmp;
    player *pl;

    if (op->env != NULL) {
        tmp = get_player_container(op->env);
        if (!tmp) {
            for (pl = first_player; pl; pl = pl->next)
                if (pl->ob->container == op->env)
                    break;
            if (pl)
                tmp = pl->ob;
            else
                tmp = NULL;
        }
        if (tmp)
            /* We don't know what changed, so we send everything. */
            esrv_update_item(UPD_ALL, tmp, op);
    } else {
        for (tmp = op->above; tmp != NULL; tmp = tmp->above)
            if (tmp->type == PLAYER)
                tmp->contr->socket.update_look = 1;
    }
}

/**
 * Notify clients about a removed object.
 *
 * @param op the object about to be removed from its environment; it must still
 * be present in its environment
 */
static void send_removed_object(object *op)
{
    object* tmp;
    player *pl;

    if (op->env == NULL) {
        /* no action necessary: remove_ob() notifies the client */
        return;
    }
    if (op->invisible)
        /* invisible items aren't sent to client anyway. */
        return;

    tmp = get_player_container(op->env);
    if (!tmp) {
        for (pl = first_player; pl; pl = pl->next)
            if (pl->ob->container == op->env)
                break;
        if (pl)
            tmp = pl->ob;
        else
            tmp = NULL;
    }
    if (tmp)
        esrv_del_item(tmp->contr, op->count);
}

int execute_event(object* op, int eventcode, object* activator, object* third, const char* message, int fix)
{
    object *tmp, *next;
    crossfire_plugin* plugin;
    int rv = 0;
    for (tmp = op->inv; tmp != NULL; tmp = next) {
        next = tmp->below;
        if (tmp->type == EVENT_CONNECTOR && tmp->subtype == eventcode) {
#ifdef PLUGIN_DEBUG
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
                send_removed_object(tmp);
                remove_ob(tmp);
                free_object(tmp);
            } else if (tmp->slaying == NULL) {
                object *env = object_get_env_recursive(tmp);
                LOG(llevError, "Event object without slaying at %d/%d in map %s\n", env->x, env->y, env->map->name);
                send_removed_object(tmp);
                remove_ob(tmp);
                free_object(tmp);
            } else {
                plugin = plugins_find_plugin(tmp->title);
                if (plugin == NULL) {
                    object *env = object_get_env_recursive(tmp);
                    LOG(llevError, "The requested plugin doesn't exit: %s at %d/%d in map %s\n", tmp->title, env->x, env->y, env->map->name);
                    send_removed_object(tmp);
                    remove_ob(tmp);
                    free_object(tmp);
                } else {
                    int rvt = 0;
                    int *rv;

                    rv = plugin->eventfunc(&rvt, op, /*eventcode, */activator, third, message, fix, /*tmp->slaying, tmp->name*/ tmp);
                    if (QUERY_FLAG(tmp, FLAG_UNIQUE))
                    {
#ifdef PLUGIN_DEBUG
                        LOG(llevDebug, "Removing unique event %s\n", tmp->slaying);
#endif
                        remove_ob(tmp);
                        free_object(tmp);
                    }
                    return *rv;
                }
            }
        }
    }
    return rv;
}

int execute_global_event(int eventcode, ...)
{
    va_list args;
    mapstruct* map;
    object* op;
    object* op2;
    player* pl;
    const char* buf;
    int i, rt;
    crossfire_plugin* cp;
    if (plugins_list == NULL)
        return -1;

    va_start(args, eventcode);

    switch (eventcode) {
    case EVENT_BORN:
        /*BORN: op*/
        op = va_arg(args, object*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op);
        }
        break;

    case EVENT_CLOCK:
        /*CLOCK: -*/
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode);
        }
        break;

    case EVENT_CRASH:
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode);
        }
        break;

    case EVENT_PLAYER_DEATH:
        /*PLAYER_DEATH: op*/
        op = va_arg(args, object*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op);
        }
        break;

    case EVENT_GKILL:
        /*GKILL: op, hitter*/
        op = va_arg(args, object*);
        op2 = va_arg(args, object*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, op2);
        }
        break;

    case EVENT_LOGIN:
        /*LOGIN: pl, pl->socket.host*/
        pl = va_arg(args, player*);
        buf = va_arg(args, char*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, pl, buf);
        }
        break;

    case EVENT_LOGOUT:
        /*LOGOUT: pl, pl->socket.host*/
        pl = va_arg(args, player*);
        buf = va_arg(args, char*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, pl, buf);
        }
        break;

    case EVENT_MAPENTER:
        /*MAPENTER: op, map*/
        op = va_arg(args, object*);
        map = va_arg(args, mapstruct*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, map);
        }
        break;

    case EVENT_MAPLEAVE:
        /*MAPLEAVE: op, map*/
        op = va_arg(args, object*);
        map = va_arg(args, mapstruct*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, map);
        }
        break;

    case EVENT_MAPRESET:
        /*MAPRESET: map*/
        map = va_arg(args, mapstruct*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, map);
        }
        break;

    case EVENT_REMOVE:
        /*REMOVE: op*/
        op = va_arg(args, object*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op);
        }
        break;

    case EVENT_SHOUT:
        /*SHOUT: op, parms, priority*/
        op = va_arg(args, object*);
        buf = va_arg(args, char*);
        i = va_arg(args, int);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, buf, i);
        }
        break;

    case EVENT_TELL:
        /* Tell: who, what, to who */
        op = va_arg(args, object*);
        buf = va_arg(args, const char*);
        op2 = va_arg(args, object*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, buf, op2);
        }
        break;

    case EVENT_MUZZLE:
        /*MUZZLE: op, parms*/
        op = va_arg(args, object*);
        buf = va_arg(args, char*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, buf);
        }
        break;

    case EVENT_KICK:
        /*KICK: op, parms*/
        op = va_arg(args, object*);
        buf = va_arg(args, char*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, op, buf);
        }
        break;
    case EVENT_MAPUNLOAD:
        /*MAPUNLOAD: map*/
        map = va_arg(args, mapstruct*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, map);
        }
        break;
    case EVENT_MAPLOAD:
        /*MAPLOAD: map*/
        map = va_arg(args, mapstruct*);
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode, map);
        }
        break;
    }
    va_end(args);
    return 0;
}

int plugins_init_plugin(const char* libfile)
{
    LIBPTRTYPE ptr;
    f_plug_init      initfunc;
    f_plug_api       propfunc;
    f_plug_api       eventfunc;
    f_plug_postinit  postfunc;
    f_plug_postinit  closefunc;
    int i;
    crossfire_plugin* cp;
    crossfire_plugin* ccp;

    /* Open the plugin lib and load the required functions */
    ptr = plugins_dlopen(libfile);
    if (ptr == NULL) {
        LOG(llevError, "Error trying to load %s: %s\n", libfile, plugins_dlerror());
        return -1;
    }
    initfunc = (f_plug_init)plugins_dlsym(ptr, "initPlugin");
    if (initfunc == NULL) {
        LOG(llevError, "Plugin error while requesting %s.initPlugin: %s\n",
            libfile, plugins_dlerror());
        plugins_dlclose(ptr);
        return -1;
    }
    propfunc = (f_plug_api)plugins_dlsym(ptr, "getPluginProperty");
    if (propfunc == NULL) {
        LOG(llevError, "Plugin error while requesting %s.getPluginProperty: %s\n",
            libfile, plugins_dlerror());
        plugins_dlclose(ptr);
        return -1;
    }
    eventfunc = (f_plug_api)plugins_dlsym(ptr, "eventListener");
    if (eventfunc == NULL) {
        LOG(llevError, "Plugin error while requesting %s.eventListener: %s\n",
            libfile, plugins_dlerror());
        plugins_dlclose(ptr);
        return -1;
    }
    postfunc = (f_plug_postinit)plugins_dlsym(ptr, "postInitPlugin");
    if (postfunc == NULL) {
        LOG(llevError, "Plugin error while requesting %s.postInitPlugin: %s\n",
            libfile, plugins_dlerror());
        plugins_dlclose(ptr);
        return -1;
    }
    closefunc = (f_plug_postinit)plugins_dlsym(ptr, "closePlugin");
    if (postfunc == NULL) {
        LOG(llevError, "Plugin error while requesting %s.closePlugin: %s\n",
            libfile, plugins_dlerror());
        plugins_dlclose(ptr);
        return -1;
    }
    if (postfunc == NULL) {
        LOG(llevError, "Plugin error while requesting %s.closePlugin: %s\n",
            libfile, plugins_dlerror());
        plugins_dlclose(ptr);
        return -1;
    }
    i = initfunc("2.0", cfapi_get_hooks);
    cp = malloc(sizeof(crossfire_plugin));
    for (i = 0; i < NR_EVENTS; i++)
        cp->gevent[i] = NULL;
    cp->eventfunc = eventfunc;
    cp->propfunc = propfunc;
    cp->closefunc = closefunc;
    cp->libptr = ptr;
    propfunc(&i, "Identification", cp->id, sizeof(cp->id));
    propfunc(&i, "FullName", cp->fullname, sizeof(cp->fullname));
    cp->next = NULL;
    cp->prev = NULL;
    if (plugins_list == NULL) {
        plugins_list = cp;
    } else {
        for (ccp = plugins_list; ccp->next != NULL; ccp = ccp->next)
            ;
        ccp->next = cp;
        cp->prev = ccp;
    }
    postfunc();
    plugin_number++;
    return 0;
}

void* cfapi_get_hooks(int* type, ...)
{
    va_list args;
    int request_type;
    char* buf;
    int fid;
    f_plug_api* rapi;
    int i;

    *type = CFAPI_NONE;

    va_start(args, type);
    request_type = va_arg(args, int);
    if (request_type == 0) { /* By nr */
        fid = va_arg(args, int);
        rapi = va_arg(args, f_plug_api*);
        if (fid < 0 || fid >= NR_OF_HOOKS) {
            *rapi = NULL;
            *type = CFAPI_NONE;
        } else {
            *rapi = plug_hooks[fid].func;
            *type = CFAPI_FUNC;
        }
    } else { /* by name */
        buf = va_arg(args, char*);
        rapi = va_arg(args, f_plug_api*);
        *rapi = NULL;
        *type = CFAPI_NONE;
        for (i = 0; i < NR_OF_HOOKS; i++) {
            if (!strcmp(buf, plug_hooks[i].fname)) {
                *rapi = plug_hooks[i].func;
                *type = CFAPI_FUNC;
                break;
            }
        }
    }
    va_end(args);
    return NULL;
}

int plugins_remove_plugin(const char* id)
{
    crossfire_plugin* cp;

    if (plugins_list == NULL)
        return -1;

    for (cp = plugins_list; cp != NULL; cp = cp->next) {
        if (!strcmp(id, cp->id)) {
            crossfire_plugin* n;
            crossfire_plugin* p;
            n = cp->next;
            p = cp->prev;
            if (cp->closefunc)
                cp->closefunc();
            plugins_dlclose(cp->libptr);
            if (n != NULL) {
                if (p != NULL) {
                    n->prev = p;
                    p->next = n;
                } else {
                    n->prev = NULL;
                    plugins_list = n;
                }
            } else {
                if (p != NULL)
                    p->next = NULL;
                else
                    plugins_list = NULL;
            }
            free(cp);
            plugin_number --;
            return 0;
        }
    }
    return -1;
}

crossfire_plugin* plugins_find_plugin(const char* id)
{
    crossfire_plugin* cp;

    if (plugins_list == NULL)
        return NULL;

    for (cp = plugins_list; cp != NULL; cp = cp->next) {
        if (!strcmp(id, cp->id)) {
            return cp;
        }
    }
    return NULL;
}

/*****************************************************************************/
/* Displays a list of loaded plugins (keystrings and description) in the     */
/* game log window.                                                          */
/*****************************************************************************/
void plugins_display_list(object *op)
{
    crossfire_plugin* cp;

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
		  "List of loaded plugins:\n-----------------------", NULL);

    if (plugins_list == NULL)
        return;

    for (cp = plugins_list; cp != NULL; cp = cp->next) {
        draw_ext_info_format(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_DEBUG,
			     "%s, %s",
			     "%s, %s",
			     cp->id, cp->fullname);
    }
}

/* SYSTEM-RELATED HOOKS */

/**
 * Wrapper for find_animation().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_system_find_animation(int *type, ...)
{
    va_list args;
    const char* anim;
    int* num;

    va_start(args, type);
    anim = va_arg(args, const char*);
    num = va_arg(args, int*);
    va_end(args);

    *num = find_animation(anim);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Wrapper for strdup_local().
 * @param type
 * will be CFAPI_STRING.
 * @return
 * NULL.
 */
void* cfapi_system_strdup_local(int *type, ...)
{
    va_list args;
    const char* txt;
    char** ret;

    va_start(args, type);
    txt = va_arg(args, const char*);
    ret = va_arg(args, char**);
    va_end(args);

    *ret = strdup_local(txt);
    *type = CFAPI_STRING;
    return NULL;
}

void* cfapi_system_register_global_event(int *type, ...)
{
    va_list args;
    int eventcode;
    char* pname;
    f_plug_api hook;
    crossfire_plugin* cp;

    va_start(args, type);
    eventcode = va_arg(args, int);
    pname     = va_arg(args, char*);
    hook      = va_arg(args, f_plug_api);
    va_end(args);

    *type = CFAPI_NONE;

    cp = plugins_find_plugin(pname);
    cp->gevent[eventcode] = hook;
    return NULL;
}

void* cfapi_system_unregister_global_event(int *type, ...)
{
    va_list args;
    int eventcode;
    char* pname;
    crossfire_plugin* cp;

    va_start(args, type);
    eventcode = va_arg(args, int);
    pname = va_arg(args, char*);
    va_end(args);

    *type = CFAPI_NONE;

    cp = plugins_find_plugin(pname);
    cp->gevent[eventcode] = NULL;

    return NULL;
}

/**
 * Wrapper for add_string().
 *
 * @param type
 * will be CFAPI_SSTRING.
 * @return
 * NULL.
 */
void* cfapi_system_add_string(int *type, ...)
{
    va_list args;
    const char* str;
    sstring* rv;

    va_start(args, type);
    str = va_arg(args, const char*);
    rv = va_arg(args, sstring*);
    va_end(args);

    *rv = add_string(str);
    *type = CFAPI_SSTRING;
    return NULL;
}

/**
 * Wrapper for free_string().
 *
 * @param type
 * will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_system_remove_string(int *type, ...)
{
    va_list args;
    sstring str;

    va_start(args, type);
    str = va_arg(args, sstring);
    va_end(args);

    free_string(str);
    *type = CFAPI_NONE;
    return NULL;
}
/**
 * Wrapper for check_path().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_system_check_path(int* type, ...)
{
    va_list args;
    const char* name;
    int prepend_dir;
    int* ret;

    va_start(args, type);

    name = va_arg(args, char*);
    prepend_dir = va_arg(args, int);
    ret = va_arg(args, int*);

    *ret = check_path(name, prepend_dir);

    va_end(args);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Wrapper for re_cmp().
 * @param type
 * will be CFAPI_STRING.
 * @return
 * NULL.
 */
void* cfapi_system_re_cmp(int* type, ...)
{
    va_list args;
    const char* str;
    const char* regexp;
    const char** rv;

    va_start(args, type);

    str = va_arg(args, char*);
    regexp = va_arg(args, char*);
    rv = va_arg(args, const char**);

    *rv = re_cmp(str, regexp);

    va_end(args);
    *type = CFAPI_STRING;
    return NULL;
}

void* cfapi_system_directory(int* type, ...)
{
    va_list args;
    int dirtype;
    const char** str;

    va_start(args, type);

    dirtype = va_arg(args, int);
    str = va_arg(args, const char**);
    va_end(args);

    *type = CFAPI_STRING;

    switch (dirtype)
    {
    case 0:
        *str = settings.mapdir;
        break;

    case 1:
        *str = settings.uniquedir;
        break;

    case 2:
        *str = settings.tmpdir;
        break;

    case 3:
        *str = settings.confdir;
        break;

    case 4:
        *str = settings.localdir;
        break;

    case 5:
        *str = settings.playerdir;
        break;

    case 6:
        *str = settings.datadir;
        break;

    default:
        *str = NULL;
    }

    return NULL;
}

/**
 * Wrapper for get_tod().
 *
 * @param type
 * will be CFAPI_NONE.
 * @return
 * NULL.
 */
void *cfapi_get_time(int *type, ...)
{
    va_list args;
    timeofday_t *tod;

    va_start(args,type);
    tod = va_arg(args, timeofday_t*);
    va_end(args);

    get_tod(tod);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for cfapi_timer_create().
 * @param type
 * unused
 * @return
 * always 0
 *
 * Additional parameters:
 * - ob : ::object* for which to create a timer
 * - delay : long, ticks or seconds
 * - mode : int, either ::TIMER_MODE_SECONDS or ::TIMER_MODE_CYCLES
 * - timer : int* that will contain timer's id
 */
void *cfapi_timer_create(int *type, ...)
{
    va_list args;
    int res;
    object* ob;
    long delay;
    int mode;
    int* timer;

    va_start(args, type);
    ob = va_arg(args, object*);
    delay = va_arg(args, long);
    mode = va_arg(args, int);
    timer = va_arg(args, int*);
    va_end(args);
    *type = CFAPI_INT;

    *timer = cftimer_find_free_id();
    if ( *timer != TIMER_ERR_ID )
    {
        res = cftimer_create(*timer, delay, ob, mode);
        if ( res != TIMER_ERR_NONE )
            *timer = res;
    }
    return 0;
}

/**
 * Wrapper for cftimer_destroy().
 * @param type
 * will be CFAPI_INT.
 * @return
 * always 0
 *
 * Additional parameters:
 * - timer: int that should be destroyed
 * - err: int* which will contain the return code of cftimer_destroy().
 */
void *cfapi_timer_destroy(int *type, ...)
{
    va_list args;
    int id;
    int* err;

    va_start(args, type);
    id = va_arg(args, int);
    err = va_arg(args, int*);
    va_end(args);
    *type = CFAPI_INT;

    *err = cftimer_destroy(id);

    return 0;
}

/**
 * Wrapper for LOG().
 * @param type
 * will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_log(int* type, ...)
{
    va_list args;
    LogLevel logLevel;
    const char* message;

    va_start(args, type);
    logLevel = va_arg(args, LogLevel);
    message = va_arg(args, const char*);
    LOG(logLevel, "%s", message);
    va_end(args);

    *type = CFAPI_NONE;

    return NULL;
}

/* MAP RELATED HOOKS */

/**
 * Gets map information.
 *
 * First parameter is a integer, which should be:
 * - 0 with 2 int and a mapstruct**: new map of specified size.
 * - 1 with char*, int, mapstruct**: call ready_map_name().
 * - 2 with mapstruct*, 2 int and mapstruct**: call to get_map_from_coord().
 */
void* cfapi_map_get_map(int* type, ...)
{
    va_list args;
    mapstruct** ret;
    int ctype;
    int x, y;
    sint16 nx, ny;
    const char* name;
    mapstruct* m;

    va_start(args, type);

    ctype = va_arg(args, int);

    switch (ctype)
    {
    case 0:
        x = va_arg(args, int);
        y = va_arg(args, int);
        ret = va_arg(args, mapstruct**);
        *ret = get_empty_map(x, y);
        break;

    case 1:
        name = va_arg(args, const char*);
        x = va_arg(args, int);
        ret = va_arg(args, mapstruct**);
        *ret = ready_map_name(name, x);
        break;

    case 2:
        m = va_arg(args, mapstruct*);
        nx = va_arg(args, int);
        ny = va_arg(args, int);
        ret = va_arg(args, mapstruct**);
        *ret = get_map_from_coord(m, &nx, &ny);
        break;

    default:
        *type = CFAPI_NONE;
        va_end(args);
        return NULL;
        break;
    }
    va_end(args);
    *type = CFAPI_PMAP;
    return NULL;
}

/**
 * Wrapper for has_been_loaded().
 * @param type
 * will be CFAPI_PMAP.
 * @return
 * NULL.
 */
void* cfapi_map_has_been_loaded(int* type, ...)
{
    va_list args;
    mapstruct** map;
    char* string;

    va_start(args, type);
    string = va_arg(args, char*);
    map = va_arg(args, mapstruct**);
    *map = has_been_loaded(string);
    va_end(args);
    *type = CFAPI_PMAP;
    return NULL;
}

/**
 * Wrapper for create_pathname() and create_overlay_pathname().
 * @param type
 * will be CFAPI_STRING.
 * @return
 * NULL.
 */
void* cfapi_map_create_path(int* type, ...)
{
    va_list args;
    int ctype, size;
    const char* str;
    char* name;

    va_start(args, type);

    ctype = va_arg(args, int);
    str = va_arg(args, const char*);
    name = va_arg(args, char*);
    size = va_arg(args, int);
    *type = CFAPI_STRING;

    switch (ctype)
    {
    case 0:
        create_pathname(str, name, size);
        break;

    case 1:
        create_overlay_pathname(str, name, MAX_BUF);
        break;

    /*case 2:
        rv = create_items_path(str);
        break;*/

    default:
        *type = CFAPI_NONE;
        break;
    }
    va_end(args);
    return NULL;
}


void* cfapi_map_get_map_property(int* type, ...)
{
    va_list args;
    mapstruct* map;
    int property;

    int* rint;
    mapstruct** rmap;
    sstring* rstr;
    region** rreg;
    sint16* nx, *ny;
    int x, y;

    va_start(args, type);

    map = va_arg(args, mapstruct*);
    property = va_arg(args, int);

    switch (property)
    {
    case CFAPI_MAP_PROP_FLAGS:
        rmap = va_arg(args, mapstruct**);
        x = va_arg(args, int);
        y = va_arg(args, int);
        nx = va_arg(args, sint16*);
        ny = va_arg(args, sint16*);
        rint = va_arg(args, int*);
        *rint = get_map_flags(map, rmap, x, y, nx, ny);
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_DIFFICULTY:
        rint = va_arg(args, int*);
        *rint = calculate_difficulty(map);
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_PATH:
        rstr = va_arg(args, sstring*);
        *rstr = map->path;
        *type = CFAPI_SSTRING;
        break;

    case CFAPI_MAP_PROP_TMPNAME:
        rstr = va_arg(args, sstring*);
        *rstr = map->tmpname;
        *type = CFAPI_SSTRING;
        break;

    case CFAPI_MAP_PROP_NAME:
        rstr = va_arg(args, sstring*);
        *rstr = map->name;
        *type = CFAPI_SSTRING;
        break;

    case CFAPI_MAP_PROP_RESET_TIME:
        rint = va_arg(args, int*);
        *rint = map->reset_time;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_RESET_TIMEOUT:
        rint = va_arg(args, int*);
        *rint = map->reset_timeout;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_PLAYERS:
        rint = va_arg(args, int*);
        *rint = map->players;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_DARKNESS:
        rint = va_arg(args, int*);
        *rint = map->darkness;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_WIDTH:
        rint = va_arg(args, int*);
        *rint = map->width;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_HEIGHT:
        rint = va_arg(args, int*);
        *rint = map->height;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_ENTER_X:
        rint = va_arg(args, int*);
        *rint = map->enter_x;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_ENTER_Y:
        rint = va_arg(args, int*);
        *rint = map->enter_y;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_TEMPERATURE:
        rint = va_arg(args, int*);
        *rint = map->temp;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_PRESSURE:
        rint = va_arg(args, int*);
        *rint = map->pressure;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_HUMIDITY:
        rint = va_arg(args, int*);
        *rint = map->humid;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_WINDSPEED:
        rint = va_arg(args, int*);
        *rint = map->windspeed;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_WINDDIR:
        rint = va_arg(args, int*);
        *rint = map->winddir;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_SKY:
        rint = va_arg(args, int*);
        *rint = map->sky;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_WPARTX:
        rint = va_arg(args, int*);
        *rint = map->wpartx;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_WPARTY:
        rint = va_arg(args, int*);
        *rint = map->wparty;
        *type = CFAPI_INT;
        break;

    case CFAPI_MAP_PROP_MESSAGE:
        rstr = va_arg(args, sstring*);
        *rstr = map->msg;
        *type = CFAPI_SSTRING;
        break;

    case CFAPI_MAP_PROP_NEXT:
        rmap = va_arg(args, mapstruct**);
        *rmap = map ? map->next : first_map;
        *type = CFAPI_PMAP;
        break;

    case CFAPI_MAP_PROP_REGION:
        rreg = va_arg(args, region**);
        *rreg = get_region_by_map(map);
        *type = CFAPI_PREGION;
        break;

    case CFAPI_MAP_PROP_UNIQUE:
        rint = va_arg(args, int*);
        *rint = map->unique;
        *type = CFAPI_INT;
        break;

    default:
        *type = CFAPI_NONE;
        break;
    }
    va_end(args);
    return NULL;
}

void* cfapi_map_set_map_property(int* type, ...)
{
    va_list args;
    mapstruct* map;
    int property;
    const char* buf;

    va_start(args, type);

    map = va_arg(args, mapstruct*);
    property = va_arg(args, int);

    switch (property)
    {
        case CFAPI_MAP_PROP_PATH:
            buf = va_arg(args, const char*);
            snprintf(map->path, sizeof(map->path), buf);
            *type = CFAPI_STRING;
            break;

        default:
            *type = CFAPI_NONE;
            break;
    }
    va_end(args);
    return NULL;
}

/**
 * Wrapper for out_of_map().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_map_out_of_map(int* type, ...)
{
    va_list args;
    mapstruct* map;
    int x, y;
    int* rint;

    va_start(args, type);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);
    rint = va_arg(args, int*);

    *rint = out_of_map(map, x, y);
    va_end(args);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Wrapper for update_position().
 * @param type
 * CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_map_update_position(int* type, ...)
{
    va_list args;
    mapstruct* map;
    int x, y;

    va_start(args, type);

    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);

    update_position(map, x, y);
    va_end(args);
    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_map_delete_map(int* type, ...)
{
    va_list args;
    mapstruct* map;
    va_start(args, type);

    map = va_arg(args, mapstruct*);

    delete_map(map);

    va_end(args);
    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_map_message(int* type, ...)
{
    va_list args;
    mapstruct* map;
    char* string;
    int color;

    va_start(args, type);
    map = va_arg(args, mapstruct*);
    string = va_arg(args, char*);
    color = va_arg(args, int);
    va_end(args);

    /* function should be extended to take message types probably */
    ext_info_map(color, map, MSG_TYPE_MISC, MSG_SUBTYPE_NONE, string, string);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for get_map_ob().
 * @param type
 * will be CFAPI_POBJECT.
 * @return
 * NULL.
 */
void* cfapi_map_get_object_at(int* type, ...)
{
    va_list args;
    mapstruct* map;
    int x, y;
    sint16 sx, sy;
    object** robj;

    va_start(args, type);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);
    robj = va_arg(args, object**);
    va_end(args);

    sx = x;
    sy = y;
    if (get_map_flags(map, &map, x, y, &sx, &sy) & P_OUT_OF_MAP)
        *robj = NULL;
    else
        *robj = get_map_ob(map, sx, sy);
    *type = CFAPI_POBJECT;
    return NULL;
}

/**
 * Kinda wrapper for present_arch() (but uses a string, not an archetype*).
 * @param type
 * will be CFAPI_POBJECT.
 * @return
 * NULL.
 * @todo fix archetype instead of string.
 */
void* cfapi_map_present_arch_by_name(int* type, ...)
{
    va_list args;
    int x, y;
    mapstruct* map;
    char* msg;
    object** robj;

    va_start(args, type);

    msg = va_arg(args, char*);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);
    robj = va_arg(args, object**);

    va_end(args);

    *robj = present_arch(find_archetype(msg), map, x, y);
    *type = CFAPI_POBJECT;
    return NULL;
}

/**
 * Wrapper for change_map_light().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_map_change_light(int* type, ...)
{
    va_list args;
    int change;
    mapstruct* map;
    int* rint;

    va_start(args, type);
    map = va_arg(args, mapstruct*);
    change = va_arg(args, int);
    rint = va_arg(args, int*);
    va_end(args);

    *type = CFAPI_INT;
    *rint = change_map_light(map, change);

    return NULL;
}

/* OBJECT-RELATED HOOKS */

/**
 * Moves an object.
 *
 * Syntax is:
 *  cfapi_object_move(&type, kind)
 * where kind is:
 * - 0: call move_ob()
 * - 1: call move_player()
 *
 * @param type
 * unused
 * @return
 * NULL.
 */
void* cfapi_object_move(int* type, ...)
{
    va_list args;
    int     kind;
    object* op;
    object* activator;
    player* pl;
    int     direction;
    int*    ret;

    va_start(args, type);
    kind = va_arg(args, int);
    switch (kind)
    {
    case 0:
        op = va_arg(args, object*);
        direction = va_arg(args, int);
        activator = va_arg(args, object*);
        ret = va_arg(args, int*);
        va_end(args);
        *ret = move_ob(op, direction, activator);
        break;

    case 1:
        pl = va_arg(args, player*);
        direction = va_arg(args, int);
        ret = va_arg(args, int*);
        va_end(args);
        *ret = move_player(pl->ob, direction);
        break;
    }
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Gets a key/value value for an object.
 *
 * @param type
 * will contain CFAPI_SSTRING.
 * @return
 * NULL.
 * @see get_ob_key_value().
 */
void* cfapi_object_get_key(int* type, ...)
{
    va_list args;
    const char* keyname;
    const char** value;
    object* op;

    va_start(args, type);
    op = va_arg(args, object*);
    keyname = va_arg(args, const char*);
    value = va_arg(args, const char**);
    va_end(args);

    *value = get_ob_key_value(op, keyname);
    *type = CFAPI_SSTRING;
    return NULL;
}

/**
 * Write a key/value for an object.
 * @param type
 * will contain CFAPI_SSTRING.
 * @return
 * NULL.
 * @see set_ob_key_value().
 */
void* cfapi_object_set_key(int* type, ...)
{
    va_list args;
    const char* keyname;
    const char* value;
    int* ret;
    object* op;
    int add_key;

    va_start(args, type);
    op = va_arg(args, object*);
    keyname = va_arg(args, char*);
    value = va_arg(args, char*);
    add_key = va_arg(args, int);
    ret = va_arg(args, int*);
    va_end(args);

    *ret = set_ob_key_value(op, keyname, value, add_key);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Main object property getter.
 */
void* cfapi_object_get_property(int* type, ...)
{
    va_list args;
    int property;
    object* op;
    int* rint;
    object** robject;
    mapstruct** rmap;
    float* rfloat;
    archetype** rarch;
    sstring* rsstring;
    char* rbuffer;
    int rbufsize;
    MoveType* rmove;
    sint64* rint64;
    partylist** rparty;
    double* rdouble;
    long* rlong;

    va_start(args, type);

    op = va_arg(args, object*);
    property = va_arg(args, int);
    switch (property)
        {
        case CFAPI_OBJECT_PROP_OB_ABOVE:
            robject = va_arg(args, object**);
            *robject = op->above;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_OB_BELOW:
            robject = va_arg(args, object**);
            *robject = op->below;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_NEXT_ACTIVE_OB:
            robject = va_arg(args, object**);
            *robject = op->active_next;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_PREV_ACTIVE_OB:
            robject = va_arg(args, object**);
            *robject = op->active_prev;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_INVENTORY:
            robject = va_arg(args, object**);
            *robject = op->inv;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_ENVIRONMENT:
            robject = va_arg(args, object**);
            *robject = op->env;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_HEAD:
            robject = va_arg(args, object**);
            *robject = op->head;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_CONTAINER:
            robject = va_arg(args, object**);
            *robject = op->container;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_MAP:
            rmap = va_arg(args, mapstruct**);
            *rmap = op->map;
            *type = CFAPI_PMAP;
            break;

        case CFAPI_OBJECT_PROP_COUNT:
            rint = va_arg(args, int*);
            *rint = op->count;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_NAME:
            rbuffer = va_arg(args, char*);
            rbufsize = va_arg(args, int);
            query_name(op, rbuffer, rbufsize);
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_NAME_PLURAL:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->name_pl;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_TITLE:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->title;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_RACE:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->race;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_SLAYING:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->slaying;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_SKILL:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->skill;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_MESSAGE:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->msg;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_LORE:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->lore;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_X:
            rint = va_arg(args, int*);
            *rint = op->x;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_Y:
            rint = va_arg(args, int*);
            *rint = op->y;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SPEED:
            rfloat = va_arg(args, float*);
            *rfloat = op->speed;
            *type = CFAPI_FLOAT;
            break;

        case CFAPI_OBJECT_PROP_SPEED_LEFT:
            rfloat = va_arg(args, float*);
            *rfloat = op->speed_left;
            *type = CFAPI_FLOAT;
            break;

        case CFAPI_OBJECT_PROP_NROF:
            rint = va_arg(args, int*);
            *rint = op->nrof;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_DIRECTION:
            rint = va_arg(args, int*);
            *rint = op->direction;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FACING:
            rint = va_arg(args, int*);
            *rint = op->facing;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_TYPE:
            rint = va_arg(args, int*);
            *rint = op->type;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SUBTYPE:
            rint = va_arg(args, int*);
            *rint = op->subtype;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CLIENT_TYPE:
            rint = va_arg(args, int*);
            *rint = op->client_type;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_RESIST:
            {
                int idx;
                sint16* resist;
                idx = va_arg(args, int);
                resist = va_arg(args, sint16*);
                *resist = op->resist[idx];
            }
            *type = CFAPI_INT16;
            break;

        case CFAPI_OBJECT_PROP_ATTACK_TYPE:
            rint = va_arg(args, int*);
            *rint = op->attacktype;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PATH_ATTUNED:
            rint = va_arg(args, int*);
            *rint = op->path_attuned;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PATH_REPELLED:
            rint = va_arg(args, int*);
            *rint = op->path_repelled;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PATH_DENIED:
            rint = va_arg(args, int*);
            *rint = op->path_denied;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL:
            rint = va_arg(args, int*);
            *rint = op->material;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL_NAME:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->materialname;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_MAGIC:
            rint = va_arg(args, int*);
            *rint = op->magic;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_VALUE:
            rlong = va_arg(args, long*);
            *rlong = op->value;
            *type = CFAPI_LONG;
            break;

        case CFAPI_OBJECT_PROP_LEVEL:
            rint = va_arg(args, int*);
            *rint = op->level;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_HEAL:
            rint = va_arg(args, int*);
            *rint = op->last_heal;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_SP:
            rint = va_arg(args, int*);
            *rint = op->last_sp;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_GRACE:
            rint = va_arg(args, int*);
            *rint = op->last_grace;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_EAT:
            rint = va_arg(args, int*);
            *rint = op->last_eat;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_INVISIBLE_TIME:
            rint = va_arg(args, int*);
            *rint = op->invisible;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PICK_UP:
            rint = va_arg(args, int*);
            *rint = op->pick_up;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_ITEM_POWER:
            rint = va_arg(args, int*);
            *rint = op->item_power;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GEN_SP_ARMOUR:
            rint = va_arg(args, int*);
            *rint = op->gen_sp_armour;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WEIGHT:
            rint = va_arg(args, int*);
            *rint = op->weight;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WEIGHT_LIMIT:
            rint = va_arg(args, int*);
            *rint = op->weight_limit;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CARRYING:
            rint = va_arg(args, int*);
            *rint = op->carrying;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GLOW_RADIUS:
            rint = va_arg(args, int*);
            *rint = op->glow_radius;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PERM_EXP:
            rint64 = va_arg(args, sint64*);
            *rint64 = op->perm_exp;
            *type = CFAPI_SINT64;
            break;

        case CFAPI_OBJECT_PROP_CURRENT_WEAPON:
            robject = va_arg(args, object**);
            *robject = op->current_weapon;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_ENEMY:
            robject = va_arg(args, object**);
            *robject = op->enemy;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_ATTACKED_BY:
            robject = va_arg(args, object**);
            *robject = op->attacked_by;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_RUN_AWAY:
            rint = va_arg(args, int*);
            *rint = op->run_away;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CHOSEN_SKILL:
            robject = va_arg(args, object**);
            *robject = op->chosen_skill;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_HIDDEN:
            rint = va_arg(args, int*);
            *rint = op->hide;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MOVE_STATUS:
            rint = va_arg(args, int*);
            *rint = op->move_status;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_ATTACK_MOVEMENT:
            rint = va_arg(args, int*);
            *rint = op->attack_movement;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SPELL_ITEM:
            robject = va_arg(args, object**);
            *robject = op->spellitem;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_EXP_MULTIPLIER:
            rdouble = va_arg(args, double*);
            *rdouble = op->expmul;
            *type = CFAPI_DOUBLE;
            break;

        case CFAPI_OBJECT_PROP_ARCHETYPE:
            rarch = va_arg(args, archetype**);
            *rarch = op->arch;
            *type = CFAPI_PARCH;
            break;

        case CFAPI_OBJECT_PROP_OTHER_ARCH:
            rarch = va_arg(args, archetype**);
            *rarch = op->other_arch;
            *type = CFAPI_PARCH;
            break;

        case CFAPI_OBJECT_PROP_CUSTOM_NAME:
            rsstring = va_arg(args, sstring*);
            *rsstring = (char*)op->custom_name;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_ANIM_SPEED:
            rint = va_arg(args, int*);
            *rint = op->anim_speed;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FRIENDLY:
            rint = va_arg(args, int*);
            *rint = is_friendly(op);
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SHORT_NAME:
            rbuffer = va_arg(args, char*);
            rbufsize = va_arg(args, int);
            query_short_name(op, rbuffer, rbufsize);
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_BASE_NAME:
            {
                int i;
                i = va_arg(args, int);
                rbuffer = va_arg(args, char*);
                rbufsize = va_arg(args, int);
                query_base_name(op, i, rbuffer, rbufsize);
                *type = CFAPI_STRING;
            }
            break;

        case CFAPI_OBJECT_PROP_MAGICAL:
            rint = va_arg(args, int*);
            *rint = is_magical(op);
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LUCK:
            rint = va_arg(args, int*);
            *rint = op->stats.luck;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_EXP:
            rint64 = va_arg(args, sint64*);
            *rint64 = op->stats.exp;
            *type = CFAPI_SINT64;
            break;

        case CFAPI_OBJECT_PROP_OWNER:
            robject = va_arg(args, object**);
            *robject = get_owner(op);
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_PRESENT:
            {
                int stype;
                stype = va_arg(args, int);
                switch (stype) {

                unsigned char ptype;
                char* buf;
                archetype* at;

                case 0: /* present_in_ob */
                    ptype = (unsigned char)(va_arg(args, int));
                    robject = va_arg(args, object**);
                    *robject = present_in_ob(ptype, op);
                    break;

                case 1: /* present_in_ob_by_name */
                    ptype = (unsigned char)(va_arg(args, int));
                    buf = va_arg(args, char*);
                    robject = va_arg(args, object**);
                    *robject = present_in_ob_by_name(ptype, buf, op);
                    break;

                case 2: /* present_arch_in_ob */
                    at = va_arg(args, archetype*);
                    robject = va_arg(args, object**);
                    *robject = present_arch_in_ob(at, op);
                    break;
                }
            }
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_CHEATER:
            rint = va_arg(args, int*);
            *rint = (QUERY_FLAG(op, FLAG_WAS_WIZ));
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MERGEABLE:
            {
                object* op2;
                op2 = va_arg(args, object*);
                rint = va_arg(args, int*);
                *rint = can_merge(op, op2);
            }
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PICKABLE:
            {
                object* op2;
                op2 = va_arg(args, object*);
                rint = va_arg(args, int*);
                *rint = can_pick(op2, op);
            }
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FLAGS:
            {
                int fl;
                fl = va_arg(args, int);
                rint = va_arg(args, int*);
                *rint = QUERY_FLAG(op, fl);
            }
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_STR:
            rint = va_arg(args, int*);
            *rint = op->stats.Str;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_DEX:
            rint = va_arg(args, int*);
            *rint = op->stats.Dex;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CON:
            rint = va_arg(args, int*);
            *rint = op->stats.Con;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WIS:
            rint = va_arg(args, int*);
            *rint = op->stats.Wis;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_INT:
            rint = va_arg(args, int*);
            *rint = op->stats.Int;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_POW:
            rint = va_arg(args, int*);
            *rint = op->stats.Pow;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CHA:
            rint = va_arg(args, int*);
            *rint = op->stats.Cha;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WC:
            rint = va_arg(args, int*);
            *rint = op->stats.wc;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_AC:
            rint = va_arg(args, int*);
            *rint = op->stats.ac;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_HP:
            rint = va_arg(args, int*);
            *rint = op->stats.hp;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SP:
            rint = va_arg(args, int*);
            *rint = op->stats.sp;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GP:
            rint = va_arg(args, int*);
            *rint = op->stats.grace;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FP:
            rint = va_arg(args, int*);
            *rint = op->stats.food;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MAXHP:
            rint = va_arg(args, int*);
            *rint = op->stats.maxhp;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MAXSP:
            rint = va_arg(args, int*);
            *rint = op->stats.maxsp;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MAXGP:
            rint = va_arg(args, int*);
            *rint = op->stats.maxgrace;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_DAM:
            rint = va_arg(args, int*);
            *rint = op->stats.dam;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GOD:
            rsstring = va_arg(args, sstring*);
            *rsstring = determine_god(op);
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_ARCH_NAME:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->arch->name;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_OBJECT_PROP_INVISIBLE:
            rint = va_arg(args, int*);
            *rint = op->invisible;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FACE:
            rint = va_arg(args, int*);
            *rint = op->animation_id;
            *type = CFAPI_INT;
            break;

        case CFAPI_PLAYER_PROP_IP:
            rsstring = va_arg(args, sstring*);
            *rsstring = op->contr->socket.host;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_PLAYER_PROP_MARKED_ITEM:
            robject = va_arg(args, object**);
            *robject = find_marked_object(op);
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_PLAYER_PROP_PARTY:
            rparty = va_arg(args, partylist**);
            *rparty = (op->contr ? op->contr->party : NULL);
            *type = CFAPI_PPARTY;
            break;
        case CFAPI_PLAYER_PROP_NEXT:
            robject = va_arg(args, object**);
            if (op)
                *robject = op->contr->next ? op->contr->next->ob : NULL;
            else
                /* This can be called when there is no player. */
                *robject = first_player ? first_player->ob : NULL;
            *type = CFAPI_POBJECT;
            break;
        case CFAPI_OBJECT_PROP_NO_SAVE:
            rint = va_arg(args, int*);
            *rint = op->no_save;
            *type = CFAPI_INT;
            break;
        case CFAPI_OBJECT_PROP_MOVE_TYPE:
            rmove = va_arg(args, MoveType*);
            *rmove = op->move_type;
            *type = CFAPI_MOVETYPE;
            break;
        case CFAPI_OBJECT_PROP_MOVE_BLOCK:
            rmove = va_arg(args, MoveType*);
            *rmove = op->move_block;
            *type = CFAPI_MOVETYPE;
            break;
        case CFAPI_OBJECT_PROP_MOVE_ALLOW:
            rmove = va_arg(args, MoveType*);
            *rmove = op->move_allow;
            *type = CFAPI_MOVETYPE;
            break;
        case CFAPI_OBJECT_PROP_MOVE_ON:
            rmove = va_arg(args, MoveType*);
            *rmove = op->move_on;
            *type = CFAPI_MOVETYPE;
            break;
        case CFAPI_OBJECT_PROP_MOVE_OFF:
            rmove = va_arg(args, MoveType*);
            *rmove = op->move_off;
            *type = CFAPI_MOVETYPE;
            break;
        case CFAPI_OBJECT_PROP_MOVE_SLOW:
            rmove = va_arg(args, MoveType*);
            *rmove = op->move_type;
            *type = CFAPI_MOVETYPE;
            break;
        case CFAPI_OBJECT_PROP_MOVE_SLOW_PENALTY:
            rfloat = va_arg(args, float*);
            *rfloat = op->move_slow_penalty;
            *type = CFAPI_FLOAT;
            break;
        case CFAPI_PLAYER_PROP_BED_MAP:
            rbuffer = va_arg(args, char*);
            rbufsize = va_arg(args, int);
            snprintf(rbuffer, rbufsize, op->contr->savebed_map);
            *type = CFAPI_STRING;
            break;
        case CFAPI_PLAYER_PROP_BED_X:
            rint = va_arg(args, int*);
            *rint = op->contr->bed_x;
            *type = CFAPI_INT;
            break;
        case CFAPI_PLAYER_PROP_BED_Y:
            rint = va_arg(args, int*);
            *rint = op->contr->bed_y;
            *type = CFAPI_INT;
            break;
        case CFAPI_OBJECT_PROP_DURATION:
            rint = va_arg(args, int*);
            *rint = op->duration;
            *type = CFAPI_INT;
            break;
        default:
            *type = CFAPI_NONE;
            break;
    }
    va_end(args);
    return NULL;
}

/**
 * Sets the property of an object.
 * Will send changes to client if required.
 * First argument should be an object*, second an integer..
 * Will not change an archetype's object.
 * @param type
 * will contain the type of the first argument used to change the property.
 * @return
 * NULL.
 */
void* cfapi_object_set_property(int* type, ...)
{
    va_list args;
    int iarg;
    long larg;
    char* sarg;
    double darg;
    object* oparg;
    object* op;
    int property;
    sint64 s64arg;
    partylist* partyarg;
    float farg;

    va_start(args, type);
    op = va_arg(args, object*);
    property = va_arg(args, int);
    *type = CFAPI_NONE;

    if (op != NULL && (!op->arch || (op != &op->arch->clone))) {
        switch (property)
        {
        case CFAPI_OBJECT_PROP_NAME:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->name, sarg);
            send_changed_object(op);
            break;

        case CFAPI_OBJECT_PROP_NAME_PLURAL:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->name_pl, sarg);
            send_changed_object(op);
            break;

        case CFAPI_OBJECT_PROP_TITLE:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->title, sarg);
            break;

        case CFAPI_OBJECT_PROP_RACE:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->race, sarg);
            break;

        case CFAPI_OBJECT_PROP_SLAYING:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->slaying, sarg);
            break;

        case CFAPI_OBJECT_PROP_SKILL:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->skill, sarg);
            break;

        case CFAPI_OBJECT_PROP_MESSAGE:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->msg, sarg);
            break;

        case CFAPI_OBJECT_PROP_LORE:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->lore, sarg);
            break;

        case CFAPI_OBJECT_PROP_SPEED:
            farg = va_arg(args, double);
            *type = CFAPI_FLOAT;
            if (farg != op->speed) {
                op->speed = farg;
                update_ob_speed(op);
            }
            break;

        case CFAPI_OBJECT_PROP_SPEED_LEFT:
            farg = va_arg(args, double);
            *type = CFAPI_FLOAT;
            op->speed_left = farg;
            break;

        case CFAPI_OBJECT_PROP_NROF:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            if (iarg < 0)
                iarg = 0;
            if (op->nrof > (uint32)iarg)
                decrease_ob_nr(op, op->nrof-iarg);
            else if (op->nrof < (uint32)iarg) {
                object* tmp;
                player *pl;
                op->nrof = iarg;
                if (op->env != NULL) {
                    tmp = get_player_container(op->env);
                    if (!tmp) {
                        for (pl = first_player; pl; pl = pl->next)
                            if (pl->ob->container == op->env)
                                break;
                        if (pl)
                            tmp = pl->ob;
                        else
                            tmp = NULL;
                    }
                    else {
                        sum_weight(tmp);
                        fix_object(tmp);
                    }
                    if (tmp)
                        esrv_update_item(UPD_NROF, tmp, op);
                }
                else
                {
                    object *above = op->above;

                    for (tmp = above; tmp != NULL; tmp = tmp->above)
                        if (tmp->type == PLAYER)
                            tmp->contr->socket.update_look=1;
                }
            }
            break;

        case CFAPI_OBJECT_PROP_DIRECTION:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->direction = iarg;
            break;

        case CFAPI_OBJECT_PROP_FACING:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->facing = iarg;
            break;

        case CFAPI_OBJECT_PROP_RESIST:
            {
                int iargbis = va_arg(args, int);
                *type = CFAPI_INT16;
                iarg = va_arg(args, int);
                op->resist[iargbis] = iarg;
            }
            break;

        case CFAPI_OBJECT_PROP_ATTACK_TYPE:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->attacktype = iarg;
            break;

        case CFAPI_OBJECT_PROP_PATH_ATTUNED:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->path_attuned = iarg;
            break;

        case CFAPI_OBJECT_PROP_PATH_REPELLED:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->path_repelled = iarg;
            break;

        case CFAPI_OBJECT_PROP_PATH_DENIED:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->path_denied = iarg;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->material = iarg;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL_NAME:
            break;

        case CFAPI_OBJECT_PROP_MAGIC:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->magic = iarg;
            break;

        case CFAPI_OBJECT_PROP_VALUE:
            larg = va_arg(args, long);
            *type = CFAPI_LONG;
            op->value = larg;
            break;

        case CFAPI_OBJECT_PROP_LEVEL:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->level = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_HEAL:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->last_heal = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_SP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->last_sp = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_GRACE:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->last_grace = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_EAT:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->last_eat = iarg;
            break;

        case CFAPI_OBJECT_PROP_INVISIBLE_TIME:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->invisible = iarg;
            break;

        case CFAPI_OBJECT_PROP_PICK_UP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->pick_up = iarg;
            break;

        case CFAPI_OBJECT_PROP_ITEM_POWER:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->item_power = iarg;
            break;

        case CFAPI_OBJECT_PROP_GEN_SP_ARMOUR:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->gen_sp_armour = iarg;
            break;

        case CFAPI_OBJECT_PROP_WEIGHT:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            if (op->weight != iarg) {
                object* tmp;
                player *pl;
                op->weight = iarg;
                if (op->env != NULL) {
                    tmp = get_player_container(op->env);
                    if (!tmp) {
                        for (pl = first_player; pl; pl = pl->next)
                            if (pl->ob->container == op->env)
                                break;
                        if (pl)
                            tmp = pl->ob;
                        else
                            tmp = NULL;
                    } else {
                        sum_weight(tmp);
                        fix_object(tmp);
                    }
                    if (tmp)
                        esrv_send_item(tmp, op);
                }
                else
                {
                    object *above = op->above;

                    for (tmp = above; tmp != NULL; tmp = tmp->above)
                        if (tmp->type == PLAYER)
                            esrv_send_item(tmp, op);
                }
            }
            break;

        case CFAPI_OBJECT_PROP_WEIGHT_LIMIT:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->weight_limit = iarg;
            break;

        case CFAPI_OBJECT_PROP_GLOW_RADIUS:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->glow_radius = iarg;
            break;

        case CFAPI_OBJECT_PROP_PERM_EXP:
            s64arg = va_arg(args, sint64);
            *type = CFAPI_SINT64;
            op->perm_exp = s64arg;
            break;

        case CFAPI_OBJECT_PROP_ENEMY:
            oparg = va_arg(args, object*);
            *type = CFAPI_POBJECT;
            op->enemy = oparg;
            break;

        case CFAPI_OBJECT_PROP_RUN_AWAY:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->run_away = iarg;
            break;

        case CFAPI_OBJECT_PROP_CHOSEN_SKILL:
            oparg = va_arg(args, object*);
            *type = CFAPI_POBJECT;
            op->chosen_skill = oparg;
            break;

        case CFAPI_OBJECT_PROP_HIDDEN:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->hide = iarg;
            break;

        case CFAPI_OBJECT_PROP_MOVE_STATUS:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->move_status = iarg;
            break;

        case CFAPI_OBJECT_PROP_ATTACK_MOVEMENT:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->attack_movement = iarg;
            break;

        case CFAPI_OBJECT_PROP_SPELL_ITEM:
            oparg = va_arg(args, object*);
            *type = CFAPI_POBJECT;
            op->spellitem = oparg;
            break;

        case CFAPI_OBJECT_PROP_EXP_MULTIPLIER:
            darg = va_arg(args, double);
            *type = CFAPI_DOUBLE;
            op->expmul = darg;
            break;

        case CFAPI_OBJECT_PROP_CUSTOM_NAME:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            FREE_AND_COPY(op->custom_name, sarg);
            send_changed_object(op);
            break;

        case CFAPI_OBJECT_PROP_ANIM_SPEED:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->anim_speed = iarg;
            break;

        case CFAPI_OBJECT_PROP_FRIENDLY:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            if (iarg == 1 && is_friendly(op) == 0)
                add_friendly_object(op);
            else if (iarg == 0 && is_friendly(op) == 1)
                remove_friendly_object(op);
            break;

        case CFAPI_OBJECT_PROP_LUCK:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.luck = iarg;
            break;

        case CFAPI_OBJECT_PROP_EXP:
            s64arg = va_arg(args, sint64);
            *type = CFAPI_SINT64;
            op->stats.exp = s64arg;
            break;

        case CFAPI_OBJECT_PROP_OWNER:
            oparg = va_arg(args, object*);
            *type = CFAPI_POBJECT;
            set_owner(op, oparg);
            break;

        case CFAPI_OBJECT_PROP_CHEATER:
            set_cheat(op);
            *type = CFAPI_NONE;
            break;

        case CFAPI_OBJECT_PROP_FLAGS:
            {
                int iargbis;
                iarg = va_arg(args, int);
                iargbis = va_arg(args, int);
                *type = CFAPI_INT;

                if (iargbis == 1)
                    SET_FLAG(op, iarg);
                else
                    CLEAR_FLAG(op, iarg);
            }
            break;

        case CFAPI_OBJECT_PROP_STR:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Str=iarg;
            break;

        case CFAPI_OBJECT_PROP_DEX:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Dex=iarg;
            break;

        case CFAPI_OBJECT_PROP_CON:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Con=iarg;
            break;

        case CFAPI_OBJECT_PROP_WIS:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Wis=iarg;
            break;

        case CFAPI_OBJECT_PROP_INT:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Int=iarg;
            break;

        case CFAPI_OBJECT_PROP_POW:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Pow=iarg;
            break;

        case CFAPI_OBJECT_PROP_CHA:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.Cha=iarg;
            break;

        case CFAPI_OBJECT_PROP_WC:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.wc=iarg;
            break;

        case CFAPI_OBJECT_PROP_AC:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.ac=iarg;
            break;

        case CFAPI_OBJECT_PROP_HP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.hp=iarg;
            break;

        case CFAPI_OBJECT_PROP_SP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.sp=iarg;
            break;

        case CFAPI_OBJECT_PROP_GP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.grace=iarg;
            break;

        case CFAPI_OBJECT_PROP_FP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.food=iarg;
            break;

        case CFAPI_OBJECT_PROP_MAXHP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.maxhp=iarg;
            break;

        case CFAPI_OBJECT_PROP_MAXSP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.maxsp=iarg;
            break;

        case CFAPI_OBJECT_PROP_MAXGP:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.maxgrace=iarg;
            break;

        case CFAPI_OBJECT_PROP_DAM:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->stats.dam=iarg;
            break;

        case CFAPI_OBJECT_PROP_FACE:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->animation_id = iarg;
            update_object(op, UP_OBJ_FACE);
            break;

        case CFAPI_OBJECT_ANIMATION:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            if (iarg != -1) {
                SET_ANIMATION(op, iarg);
            }
            update_object(op, UP_OBJ_FACE);
            break;
        case CFAPI_OBJECT_PROP_DURATION:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->duration = iarg;
            break;

        case CFAPI_PLAYER_PROP_MARKED_ITEM:
            if (op->contr) {
                oparg = va_arg(args, object*);
                *type = CFAPI_POBJECT;
                op->contr->mark = oparg;
                if (oparg)
                    op->contr->mark_count = oparg->count;
            }
            break;

        case CFAPI_PLAYER_PROP_PARTY:
            if (op->contr) {
                partyarg = va_arg(args, partylist*);
                *type = CFAPI_PPARTY;
                op->contr->party = partyarg;
            }
            break;
        case CFAPI_OBJECT_PROP_NO_SAVE:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->no_save = iarg;
            break;
        case CFAPI_PLAYER_PROP_BED_MAP:
            sarg = va_arg(args, char*);
            *type = CFAPI_STRING;
            strncpy(op->contr->savebed_map, sarg, MAX_BUF);
            break;
        case CFAPI_PLAYER_PROP_BED_X:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->contr->bed_x = iarg;
            break;
        case CFAPI_PLAYER_PROP_BED_Y:
            iarg = va_arg(args, int);
            *type = CFAPI_INT;
            op->contr->bed_y = iarg;
            break;
        default:
            break;
        }
    }
    va_end(args);

    return NULL;
}

/**
 * Applies an object below.
 *
 * @param type
 * will be CFAPI_NONE.
 * @return
 * always NULL.
 */
void* cfapi_object_apply_below(int* type, ...)
{
    va_list args;
    object* applier;

    va_start(args, type);

    applier = va_arg(args, object*);

    va_end(args);

    player_apply_below(applier);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Applies an object.
 *
 * @param type
 * will be CFAPI_INT.
 * @return
 * always NULL.
 */
void* cfapi_object_apply(int* type, ...)
{
    va_list args;
    object* applied;
    object* applier;
    int aflags;
    int* ret;

    va_start(args, type);

    applied = va_arg(args, object*);
    applier = va_arg(args, object*);
    aflags  = va_arg(args, int);
    ret = va_arg(args, int*);

    va_end(args);

    *type = CFAPI_INT;
    *ret = manual_apply(applier, applied, aflags);
    return NULL;
}

/**
 * Wrapper for identify().
 * @param type
 * will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_object_identify(int* type, ...)
{
    va_list args;
    object* op;

    va_start(args, type);

    op = va_arg(args, object*);

    va_end(args);

    identify(op);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for describe_item().
 * @param type
 * will be CFAPI_STRING.
 * @return
 * NULL.
 */
void* cfapi_object_describe(int* type, ...)
{
    va_list args;
    object* op;
    object* owner;
    char* desc;
    int size;

    va_start(args, type);

    op = va_arg(args, object*);
    owner = va_arg(args, object*);
    desc = va_arg(args, char*);
    size = va_arg(args, int);
    va_end(args);

    *type = CFAPI_STRING;
    describe_item(op, owner, desc, size);
    return NULL;
}
void* cfapi_object_drain(int* type, ...)
{
    va_list args;

    object* op;
    int ds;

    va_start(args, type);

    op = va_arg(args, object*);
    ds = va_arg(args, int);

    va_end(args);

    drain_specific_stat(op, ds);

    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_fix(int* type, ...)
{
    va_list args;
    object* op;

    va_start(args, type);

    op = va_arg(args, object*);

    va_end(args);

    fix_object(op);

    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_give_skill(int* type, ...)
{
    va_list args;

    object* op;
    char* skillname;

    va_start(args, type);

    op = va_arg(args, object*);
    skillname = va_arg(args, char*);

    va_end(args);

    *type = CFAPI_POBJECT;
    return give_skill_by_name(op, skillname);
}
void* cfapi_object_transmute(int* type, ...)
{
    va_list args;

    object* op;
    object* chg;

    va_start(args, type);

    op = va_arg(args, object*);
    chg = va_arg(args, object*);

    va_end(args);

    transmute_materialname(op, chg);
    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_remove(int* type, ...)
{
    va_list args;
    object* op;

    va_start(args, type);

    op = va_arg(args, object*);

    if (QUERY_FLAG(op, FLAG_REMOVED))
    {
        LOG(llevError, "Plugin trying to remove removed object %s\n", op->name);
        *type = CFAPI_NONE;
        return NULL;
    }

    va_end(args);

    send_removed_object(op);
    remove_ob(op);
    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_delete(int* type, ...)
{
    va_list args;
    object* op;

    va_start(args, type);

    op = va_arg(args, object*);

    if (QUERY_FLAG(op, FLAG_FREED) || !QUERY_FLAG(op, FLAG_REMOVED))
    {
        LOG(llevError, "Plugin trying to free freed/non removed object %s\n", op->name);
        *type = CFAPI_NONE;
        return NULL;
    }

    va_end(args);

    free_object(op);

    *type = CFAPI_NONE;
    return NULL;
}
/**
 * Clone an object, either through object_create_clone() or copy_object().
 * @param type
 * will be CFAPI_POBJECT.
 * @return
 * NULL.
 */
void* cfapi_object_clone(int* type, ...)
{
    va_list args;
    object* op;
    int kind;
    object** robj;

    va_start(args, type);

    op = va_arg(args, object*);
    kind = va_arg(args, int);
    robj = va_arg(args, object**);

    va_end(args);

    if (kind == 0) {
        *type = CFAPI_POBJECT;
        *robj = object_create_clone(op);
    } else {
        object* tmp;
        tmp = get_object();
        copy_object(op, tmp);
        *type = CFAPI_POBJECT;
        *robj = tmp;
    }
    return NULL;
}

void* cfapi_object_find(int* type, ...)
{
    va_list args;
    int ftype;
    void* rv;
    int ival;
    int ival2;
    char* sval;
    object* op;
    va_start(args, type);

    *type = CFAPI_POBJECT;
    ftype = va_arg(args, int);
    switch (ftype)
    {
    case 0:
        ival = va_arg(args, int);
        rv = find_object(ival);
        break;

    case 1:
        sval = va_arg(args, char*);
        rv = find_object_name(sval);
        break;

    case 2:
        op = va_arg(args, object*);
        ival = va_arg(args, int);
        ival2 = va_arg(args, int);
        rv = find_obj_by_type_subtype(op, ival, ival2);
        break;

    case 3:
        op = va_arg(args, object*);
        rv = get_player_container(op);
        break;

    default:
        rv = NULL;
        *type = CFAPI_NONE;
        break;
    }

    va_end(args);

    return rv;
}

/**
 * Wrapper for get_object(), create_archetype() and create_archetype_by_object_name().
 * @param type
 * will be CFAPI_POBJECT, or CFAPI_NONE if invalid value asked for.
 * @return
 * NULL.
 */
void* cfapi_object_create(int* type, ...)
{
    va_list args;
    int ival;
    object** robj;
    va_start(args, type);
    ival = va_arg(args, int);

    *type = CFAPI_POBJECT;
    switch (ival)
    {
    case 0:
        robj = va_arg(args, object**);
        *robj = get_object();
        break;

    case 1: /* Named object. Nearly the old plugin behavior, but we don't add artifact suffixes */
        {
            const char* sval;
            archetype* at;

            sval = va_arg(args, const char*);
            robj = va_arg(args, object**);
            va_end(args);

            at = find_archetype(sval);
            if (!at)
                at = find_archetype_by_object_name(sval);
            if (at) {
                *robj = object_create_arch(at);
            }
            else
                *robj = NULL;
        }
        break;

    default:
        *type = CFAPI_NONE;
        break;
    }
    va_end(args);
    return NULL;
}
void* cfapi_object_insert(int* type, ...)
{
    va_list args;
    object* op;
    object* orig;
    mapstruct* map;
    int flag, x, y;
    int itype;
    char* arch_string;
    object** robj;

    va_start(args, type);

    op = va_arg(args, object*);
    itype = va_arg(args, int);

    switch (itype) {
    case 0:
        map = va_arg(args, mapstruct*);
        orig = va_arg(args, object*);
        flag = va_arg(args, int);
        x = va_arg(args, int);
        y = va_arg(args, int);
        robj = va_arg(args, object**);
        *robj = insert_ob_in_map_at(op, map, orig, flag, x, y);
        *type = CFAPI_POBJECT;
        break;

    case 1:
        map = va_arg(args, mapstruct*);
        orig = va_arg(args, object*);
        flag = va_arg(args, int);
        robj = va_arg(args, object**);
        *robj = insert_ob_in_map(op, map, orig, flag);
        *type = CFAPI_POBJECT;
        break;

    case 2:
        arch_string = va_arg(args, char*);
        replace_insert_ob_in_map(arch_string, op);
        *type = CFAPI_NONE;
        break;

    case 3:
        orig = va_arg(args, object*);
        robj = va_arg(args, object**);
        *robj = insert_ob_in_ob(op, orig);
        *type = CFAPI_POBJECT;
        break;
    }

    va_end(args);

    return NULL;
}
/**
 * Wrapper for get_split_ob().
 * @param type
 * will be CFAPI_POBJECT.
 * @return
 * NULL.
 */
void* cfapi_object_split(int* type, ...)
{
    va_list args;

    int nr, size;
    object* op;
    char* buf;
    object** split;

    va_start(args, type);

    op = va_arg(args, object*);
    nr = va_arg(args, int);
    buf = va_arg(args, char*);
    size = va_arg(args, int);
    split = va_arg(args, object**);
    va_end(args);

    *type = CFAPI_POBJECT;
    *split = get_split_ob(op, nr, buf, size);
    return NULL;
}

/**
 * Wrapper for merge_ob().
 * @param type
 * Will be CFAPI_POBJECT.
 * @return
 * NULL.
 */
void* cfapi_object_merge(int* type, ...)
{
    va_list args;
    object* op;
    object* op2;
    object** merge;

    va_start(args, type);

    op = va_arg(args, object*);
    op2 = va_arg(args, object*);
    merge = va_arg(args, object**);

    va_end(args);


    *type = CFAPI_POBJECT;
    *merge = merge_ob(op, op2);
    return NULL;
}

/**
 * Wrapper for distance().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_distance(int* type, ...)
{
    va_list args;
    object* op;
    object* op2;
    int* rint;
    va_start(args, type);

    op = va_arg(args, object*);
    op2 = va_arg(args, object*);
    rint = va_arg(args, int*);

    va_end(args);

    *type = CFAPI_INT;
    *rint = distance(op, op2);
    return NULL;
}
/**
 * Wrapper for update_object().
 * @param type
 * Will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_object_update(int* type, ...)
{
    va_list args;
    int action;
    object* op;
    va_start(args, type);

    op = va_arg(args, object*);
    action = va_arg(args, int);

    va_end(args);

    update_object(op, action);
    *type = CFAPI_NONE;
    return NULL;
}
/**
 * Wrapper for clear_object().
 * @param type
 * Will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_object_clear(int* type, ...)
{
    va_list args;
    object* op;
    va_start(args, type);

    op = va_arg(args, object*);

    va_end(args);

    clear_object(op);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for clear_reset().
 * @param type
 * Will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_object_reset(int* type, ...)
{
    va_list args;
    object* op;

    va_start(args, type);

    op = va_arg(args, object*);

    va_end(args);

    reset_object(op);
    *type = CFAPI_NONE;
    return NULL;
}

void* cfapi_object_check_inventory(int* type, ...)
{
    va_list args;
    object* op;
    object* op2;
    int checktype;
    object* ret = NULL;

    va_start(args, type);

    op = va_arg(args, object*);
    op2 = va_arg(args, object*);
    checktype = va_arg(args, int);

    if (checktype == 0) {
        check_inv(op, op2);
        *type = CFAPI_NONE;
    } else {
        ret = check_inv_recursive(op, op2);
        *type = CFAPI_POBJECT;
    }

    va_end(args);

    return ret;
}

void* cfapi_object_clean_object(int* type, ...)
{
    va_list args;
    object* op;
    va_start(args, type);
    op = va_arg(args, object*);
    clean_object(op);
    va_end(args);
    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_on_same_map(int* type, ...)
{
    va_list args;
    object* op1;
    object* op2;
    int* rint;

    va_start(args, type);
    op1 = va_arg(args, object*);
    op2 = va_arg(args, object*);
    rint = va_arg(args, int*);
    va_end(args);

    *type = CFAPI_INT;
    *rint = on_same_map(op1, op2);

    return NULL;
}

void* cfapi_object_spring_trap(int* type, ...)
{
    object* trap;
    object* victim;
    va_list args;

    va_start(args, type);
    trap = va_arg(args, object*);
    victim = va_arg(args, object*);
    va_end(args);

    spring_trap(trap, victim);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for check_trigger().
 * @param type
 * Will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_check_trigger(int* type, ...)
{
    object* op;
    object* cause;
    va_list args;
    int* rint;

    va_start(args, type);
    op = va_arg(args, object*);
    cause = va_arg(args, object*);
    rint = va_arg(args, int*);
    va_end(args);

    *rint = check_trigger(op, cause);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Wrapper for query_cost().
 * @param type
 * Will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_query_cost(int* type, ...)
{
    object* op;
    object* who;
    int flags;
    va_list args;
    int* rint;

    va_start(args, type);
    op = va_arg(args, object*);
    who = va_arg(args, object*);
    flags = va_arg(args, int);
    rint = va_arg(args, int*);
    va_end(args);

    *rint = query_cost(op, who, flags);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Wrapper for query_money().
 * @param type
 * Will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_query_money(int* type, ...)
{
    object* op;
    va_list args;
    int* rint;

    va_start(args, type);
    op = va_arg(args, object*);
    rint = va_arg(args, int*);
    va_end(args);

    *rint = query_money(op);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Wrapper for query_money().
 * @param type
 * Will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_cast(int* type, ...)
{
    object* op;
    object* sp;
    int dir;
    char* str;
    object* caster;
    va_list args;
    int* rint;

    va_start(args, type);
    op = va_arg(args, object*);
    caster = va_arg(args, object*);
    dir = va_arg(args, int);
    sp = va_arg(args, object*);
    str = va_arg(args, char*);
    rint = va_arg(args, int*);
    va_end(args);

    *type = CFAPI_INT;

    if (!op->map) {
        *rint = -1;
        return NULL;
    }

    *rint = cast_spell(op, caster, dir, sp, str);
    return NULL;
}
void* cfapi_object_learn_spell(int* type, ...)
{
    object* op;
    object* sp;
    int prayer;
    va_list args;

    va_start(args, type);
    op = va_arg(args, object*);
    sp = va_arg(args, object*);
    prayer = va_arg(args, int);
    va_end(args);
    do_learn_spell(op, sp, prayer);
    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_forget_spell(int* type, ...)
{
    object* op;
    object* sp;
    va_list args;
    char name[MAX_BUF];

    va_start(args, type);
    op = va_arg(args, object*);
    sp = va_arg(args, object*);
    va_end(args);
    query_name(sp, name, MAX_BUF);
    do_forget_spell(op, name);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for check_spell_known().
 * @param type
 * Will be CFAPI_POBJECT.
 * @return
 * NULL.
 */
void* cfapi_object_check_spell(int* type, ...)
{
    object* op;
    char* spellname;
    va_list args;
    object** robj;

    va_start(args, type);
    op = va_arg(args, object*);
    spellname = va_arg(args, char*);
    robj = va_arg(args, object**);
    va_end(args);
    *robj = check_spell_known(op, spellname);
    *type = CFAPI_POBJECT;
    return NULL;
}
/**
 * Wrapper for pay_for_amount().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_pay_amount(int* type, ...)
{
    object* op;
    uint64 amount;
    va_list args;
    int* rint;

    va_start(args, type);
    op = va_arg(args, object*);
    amount = va_arg(args, uint64);
    rint = va_arg(args, int*);
    va_end(args);

    *rint = pay_for_amount(amount, op);
    *type = CFAPI_INT;
    return NULL;
}
/**
 * Wrapper for pay_for_item().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_pay_item(int* type, ...)
{
    object* op;
    object* tobuy;
    int* rint;

    va_list args;

    va_start(args, type);
    tobuy = va_arg(args, object*);
    op = va_arg(args, object*);
    rint = va_arg(args, int*);
    va_end(args);

    *rint = pay_for_item(tobuy, op);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Object transfer.
 * Parameters are object*, int meaning:
 * 0: call to transfer_ob()
 * 1: call to insert_ob_in_map_at()
 * @return
 * NULL.
 */
void* cfapi_object_transfer(int* type, ...)
{
    object* op;
    object* originator;
    int x, y, randompos, ttype, flag;
    va_list args;
    mapstruct* map;
    int* rint;
    object** robj;

    va_start(args, type);
    op = va_arg(args, object*);
    ttype = va_arg(args, int);
    switch (ttype)
    {
    case 0:
        x = va_arg(args, int);
        y = va_arg(args, int);
        randompos = va_arg(args, int);
        originator = va_arg(args, object*);
        rint = va_arg(args, int*);
        va_end(args);

        *rint = transfer_ob(op, x, y, randompos, originator);
        *type = CFAPI_INT;
        return NULL;
        break;

    case 1:
        map = va_arg(args, mapstruct*);
        originator = va_arg(args, object*);
        flag = va_arg(args, int);
        x = va_arg(args, int);
        y = va_arg(args, int);
        robj = va_arg(args, object**);
        va_end(args);
        if (x < 0 || y < 0) {
            x = map->enter_x;
            y = map->enter_y;
        }
        *robj = insert_ob_in_map_at(op, map, originator, flag, x, y);
        *type = CFAPI_POBJECT;
        return NULL;
        break;

    default:
        *type = CFAPI_NONE;
        return NULL;
        break;
    }
}

/**
 * Kinda wrapper for present_arch_in_ob().
 */
void* cfapi_object_find_archetype_inside(int* type, ...)
{
    object* op;
    int     critera;
    char*   str;
    va_list args;
    object** robj;

    *type = CFAPI_POBJECT;
    va_start(args, type);
    op = va_arg(args, object*);
    critera = va_arg(args, int);

    switch(critera)
    {
    case 0: /* By name, either exact or from query_name */
        str = va_arg(args, char*);
        robj = va_arg(args, object**);
        *robj = present_arch_in_ob(find_archetype(str), op);
        if (*robj == NULL) {
            object* tmp;
            char name[MAX_BUF];
            /* Search by query_name instead */
            for (tmp = op->inv; tmp; tmp = tmp->below) {
                query_name(tmp, name, MAX_BUF);
                if (!strncmp(name, str, strlen(str)))
                    *robj = tmp;
                if (!strncmp(tmp->name, str, strlen(str)))
                    *robj = tmp;
                if (*robj != NULL)
                    break;
            }
        }
        break;

    default:
        *robj = NULL;
        break;
    }
    va_end(args);

    return NULL;
}

/**
 * Wrapper for drop().
 * @param type
 * will be CFAPI_NONE.
 * @return
 * NULL.
 */
void* cfapi_object_drop(int* type, ...)
{
    object* op;
    object* author;
    va_list args;

    va_start(args, type);
    op = va_arg(args, object*);
    author = va_arg(args, object*);
    va_end(args);
    *type = CFAPI_NONE;

    if (QUERY_FLAG(op, FLAG_NO_DROP))
        return NULL;
    drop(author, op);

    if (author->type == PLAYER) {
        author->contr->count = 0;
        author->contr->socket.update_look = 1;
    }

    return NULL;
}

/**
 * Wrapper for change_abil().
 */
void* cfapi_object_change_abil(int* type, ...)
{
    object* op, *tmp;
    int* rint;
    va_list args;

    va_start(args, type);
    op = va_arg(args, object*);
    tmp = va_arg(args, object*);
    rint = va_arg(args, int*);
    va_end(args);

    *type = CFAPI_INT;
    *rint = change_abil(op, tmp);

    return NULL;
}

void* cfapi_object_say(int* type, ...)
{
    object* op;
    char* msg;
    va_list args;
    int* rint;

    va_start(args, type);
    op = va_arg(args, object*);
    msg = va_arg(args, char*);
    rint = va_arg(args, int*);
    va_end(args);

    if (op->type == PLAYER) {
        *rint = command_say(op, msg);
    } else {
        npc_say(op, msg);
        *rint = 0;
    }
    *type = CFAPI_INT;
    return NULL;
}

/* PLAYER SUBCLASS */

/**
 * Wrapper for find_player_partial_name().
 * @param type
 * will be CFAPI_PPLAYER.
 * @return
 * NULL.
 */
void* cfapi_player_find(int* type, ...)
{
    va_list args;
    char* sval;
    player** rpl;
    va_start(args, type);

    sval = va_arg(args, char*);
    rpl = va_arg(args, player**);
    va_end(args);

    *rpl = find_player_partial_name(sval);

    *type = CFAPI_PPLAYER;
    return NULL;
}
void* cfapi_player_message(int* type, ...)
{
    va_list args;
    int flags;
    int pri;
    object* pl;
    char* buf;

    va_start(args, type);

    flags = va_arg(args, int);
    pri   = va_arg(args, int);
    pl    = va_arg(args, object*);
    buf   = va_arg(args, char*);
    va_end(args);

    draw_ext_info(flags, pri, pl, MSG_TYPE_MISC, MSG_SUBTYPE_NONE,
		  buf, buf);
    *type = CFAPI_NONE;
    return NULL;
}

/**
 * Wrapper for change_exp().
 * @param type
 * will be CFAPI_NONE.
 * @return
 * NULL.
 */
void *cfapi_object_change_exp(int *type, ...)
{
    va_list(args);
    int flag;
    object* ob;
    const char* skill;
    sint64 exp;

    va_start(args, type);
    ob = va_arg(args, object*);
    exp = va_arg(args, sint64);
    skill = va_arg(args, const char*);
    flag = va_arg(args, int);
    va_end(args);

    *type = CFAPI_NONE;
    change_exp(ob, exp, skill, flag);
    return NULL;
}

/**
 * Wrapper for can_pay().
 * @param type
 * will be CFAPI_INT.
 * @return
 * NULL.
 */
void *cfapi_player_can_pay(int *type, ...)
{
    va_list args;
    object* pl;
    int* rint;

    va_start(args, type);
    pl = va_arg(args, object*);
    rint = va_arg(args, int*);
    va_end(args);

    *rint = can_pay(pl);
    *type = CFAPI_INT;
    return NULL;
}

/**
 * Teleports an object at a specified destination if possible.
 * @param type
 * Will be CFAPI_INT.
 * @return
 * NULL.
 */
void* cfapi_object_teleport(int *type, ...)
{
    mapstruct* map;
    int x, y;
    object* who;
    int* res;
    va_list args;

    va_start(args, type);
    who = va_arg(args, object*);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);
    res = va_arg(args, int*);
    *type = CFAPI_INT;

    if (!out_of_map(map, x, y)) {
        int k;
        k = find_first_free_spot(who, map, x, y);
        if (k == -1) {
            *res = 1;
            return NULL;
        }

        if (!QUERY_FLAG(who, FLAG_REMOVED)) {
            send_removed_object(who);
            remove_ob(who);
        }

        insert_ob_in_map_at(who, map, NULL, 0, x, y);
        if (who->type == PLAYER)
            map_newmap_cmd(who->contr);
        *res = 0;
    }

    return NULL;
}
void* cfapi_object_pickup(int *type, ...)
{
    object* who;
    object* what;
    va_list args;

    va_start(args, type);
    who = va_arg(args, object*);
    what = va_arg(args, object*);
    va_end(args);

    pick_up(who, what);
    *type = CFAPI_NONE;
    return NULL;
}

/* Archetype-related functions */
void* cfapi_archetype_get_property(int* type, ...)
{
    int prop;
    archetype* arch;
    va_list args;
    sstring* rsstring;
    archetype** rarch;
    object** robject;

    va_start(args, type);
    arch = va_arg(args, archetype*);
    prop = va_arg(args, int);
    switch (prop) {
    case CFAPI_ARCH_PROP_NAME:
        *type = CFAPI_SSTRING;
        rsstring = va_arg(args, sstring*);
        *rsstring = arch->name;
        break;

    case CFAPI_ARCH_PROP_NEXT:
        *type = CFAPI_PARCH;
        rarch = va_arg(args, archetype**);
        *rarch = arch ? arch->next : first_archetype;
        break;

    case CFAPI_ARCH_PROP_HEAD:
        *type = CFAPI_PARCH;
        rarch = va_arg(args, archetype**);
        *rarch = arch->head;
        break;

    case CFAPI_ARCH_PROP_MORE:
        *type = CFAPI_PARCH;
        rarch = va_arg(args, archetype**);
        *rarch = arch->more;
        break;

    case CFAPI_ARCH_PROP_CLONE:
        *type = CFAPI_POBJECT;
        robject = va_arg(args, object**);
        *robject = &arch->clone;
        break;

    default:
        *type = CFAPI_NONE;
        break;
    }
    va_end(args);
    return NULL;
}

/**
 * Party-related functions.
 *
 * @param type
 * data type returned.
 * @return
 * NULL.
 */
void* cfapi_party_get_property(int* type, ...)
{
    partylist* party;
    int prop;
    va_list args;
    object* obarg;
    sstring* rsstring;
    player** rplayer;
    partylist** rparty;

    va_start(args, type);
    party = va_arg(args, partylist*);
    prop = va_arg(args, int);
    switch (prop)
    {
        case CFAPI_PARTY_PROP_NAME:
            rsstring = va_arg(args, sstring*);
            *rsstring = party->partyname;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_PARTY_PROP_NEXT:
            rparty = va_arg(args, partylist**);
            *rparty = (party ? party->next : get_firstparty());
            *type = CFAPI_PPARTY;
            break;

        case CFAPI_PARTY_PROP_PASSWORD:
            rsstring = va_arg(args, sstring*);
            *rsstring = party->passwd;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_PARTY_PROP_PLAYER:
            *type = CFAPI_PPLAYER;
            obarg = va_arg(args, object*);
            rplayer = va_arg(args, player**);
            *rplayer = (obarg ? obarg->contr : first_player);
            for (; *rplayer != NULL; (*rplayer) = (*rplayer)->next)
                if ((*rplayer)->ob->contr->party == party) {
                    break;
                }
            break;

        default:
            *type = CFAPI_NONE;
            break;
    }
    va_end(args);
    return NULL;
}

/**
 * Regions-related functions.
 *
 * @param type
 * data type returned.
 * @return
 * NULL.
 */
void* cfapi_region_get_property(int* type, ...)
{
    region* reg;
    int prop;
    va_list args;
    /** Return values. */
    sstring* rsstring;
    region** rregion;

    va_start(args, type);
    reg = va_arg(args, region*);
    prop = va_arg(args, int);
    switch (prop) {
        case CFAPI_REGION_PROP_NAME:
            rsstring = va_arg(args, sstring*);
            *rsstring = reg->name;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_REGION_PROP_NEXT:
            rregion = va_arg(args, region**);
            *rregion = (reg?reg->next:first_region);
            *type = CFAPI_PREGION;
            break;

        case CFAPI_REGION_PROP_PARENT:
            rregion = va_arg(args, region**);
            *rregion = reg->parent;
            *type = CFAPI_PREGION;
            break;

        case CFAPI_REGION_PROP_LONGNAME:
            rsstring = va_arg(args, sstring*);
            *rsstring = reg->longname;
            *type = CFAPI_SSTRING;
            break;

        case CFAPI_REGION_PROP_MESSAGE:
            rsstring = va_arg(args, sstring*);
            *rsstring = reg->msg;
            *type = CFAPI_SSTRING;
            break;

        default:
            *type = CFAPI_NONE;
            break;
    }
    va_end(args);
    return NULL;
}

/**
 * Friend list access, to get objects on it.
 *
 * Expects one parameter, ob.
 *
 * @param type
 * unused
 * @return
 * - if ob is NULL, gets the first object on the friendlylist.
 * - if not NULL, get next object on the friendlylist after ob. NULL if none or ob not on list.
 */
void *cfapi_friendlylist_get_next(int *type, ...)
{
    object* ob;
    va_list args;
    objectlink* link;
    object** robject;

    va_start(args, type);
    ob = va_arg(args, object*);
    robject = va_arg(args, object**);
    va_end(args);

    *type = CFAPI_POBJECT;
    *robject = NULL;

    if (ob) {
        for (link = first_friendly_object; link; link = link->next) {
            if (ob == link->ob) {
                if (link->next) {
                    *robject = link->next->ob;
                    return NULL;
                }
                else {
                    return NULL;
                }
            }
        }
        return NULL;
    }

    if (first_friendly_object)
        *robject = first_friendly_object->ob;

    return NULL;

}

/*
 * Random-map related stuff.
 */

/**
 * Wrapper for set_random_map_variable().
 *
 * @param type
 * unused.
 * @return
 * NULL.
 */
void* cfapi_set_random_map_variable(int *type, ...) {

    va_list args;
    RMParms* rp;
    const char* buf;
    int* ret;

    va_start(args, type);
    rp = va_arg(args, RMParms*);
    buf = va_arg(args, const char*);
    ret = va_arg(args, int*);
    va_end(args);

    *ret = set_random_map_variable(rp, buf);
    *type = CFAPI_INT;

    return NULL;
}

/**
 * Wrapper for generate_random_map().
 *
 * @param type
 * unused.
 * @return
 * NULL.
 */
void* cfapi_generate_random_map(int *type, ...) {
    va_list args;
    const char* name;
    RMParms* rp;
    char** use_layout;
    mapstruct** ret;

    va_start(args, type);
    name = va_arg(args, const char*);
    rp = va_arg(args, RMParms*);
    use_layout = va_arg(args, char**);
    ret = va_arg(args, mapstruct**);
    va_end(args);

    *ret = generate_random_map(name, rp, use_layout);

    return NULL;
}


/*****************************************************************************/
/* NEW PLUGIN STUFF ENDS HERE                                                */
/*****************************************************************************/


/*****************************************************************************/
/* Tries to find if a given command is handled by a plugin.                  */
/* Note that find_plugin_command is called *before* the internal commands are*/
/* checked, meaning that you can "overwrite" them.                           */
/*****************************************************************************/
/**
 * @todo
 * remove static buffer.
 */
command_array_struct *find_plugin_command(char *cmd, object *op)
{
    int i;
    crossfire_plugin* cp;
    static command_array_struct rtn_cmd;

    if (plugins_list == NULL)
        return NULL;

    for (cp = plugins_list; cp != NULL; cp = cp->next) {
        if (cp->propfunc(&i, "command?", cmd, &rtn_cmd) != NULL)
            return &rtn_cmd;
    }
    return NULL;
}

/*****************************************************************************/
/* Plugins initialization. Browses the plugins directory and call            */
/* initOnePlugin for each file found.                                        */
/* Returns 0 if at least one plugin was successfully loaded, -1 if not       */
/*****************************************************************************/
int initPlugins(void)
{
    struct dirent *currentfile;
    DIR *plugdir;
    size_t l;
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    int result;

    LOG(llevInfo, "Initializing plugins\n");
    strcpy(buf, LIBDIR);
    strcat(buf, "/plugins/");
    LOG(llevInfo, "Plugins directory is %s\n", buf);

    plugdir = opendir(buf);
    if (plugdir == NULL)
        return -1;

    result = -1;
    while ((currentfile = readdir(plugdir)) != NULL) {
        l = strlen(currentfile->d_name);
        if (l > strlen(PLUGIN_SUFFIX)) {
            if (strcmp(currentfile->d_name+l-strlen(PLUGIN_SUFFIX), PLUGIN_SUFFIX) == 0) {
                strcpy(buf2, buf);
                strcat(buf2, currentfile->d_name);
                LOG(llevInfo, " -> Loading plugin : %s\n", currentfile->d_name);
                if (plugins_init_plugin(buf2) == 0)
                    result = 0;
            }
        }
    }

    closedir(plugdir);
    return result;
}

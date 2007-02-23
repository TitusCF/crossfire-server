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

/*****************************************************************************/
/* First, the headers. We only include plugin.h, because all other includes  */
/* are done into it, and plugproto.h (which is used only by this file).      */
/*****************************************************************************/
#include <plugin.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#include <timers.h>
#endif

#define NR_OF_HOOKS 79

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
    {cfapi_object_take,             47, "cfapi_object_take"},
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
    {cfapi_map_get_flags,           60, "cfapi_map_get_flags"},
    {cfapi_map_present_arch_by_name,61, "cfapi_map_present_arch_by_name"},
    {cfapi_player_find,             62, "cfapi_player_find"},
    {cfapi_player_message,          63, "cfapi_player_message"},
    {cfapi_player_send_inventory,   64, "cfapi_player_send_inventory"},
    {cfapi_object_teleport,         65, "cfapi_object_teleport"},
    {cfapi_object_speak,            66, "cfapi_object_speak"},
    {cfapi_object_pickup,           67, "cfapi_object_pickup"},
    {cfapi_object_move,             68, "cfapi_object_move"},
    {cfapi_object_apply_below,      69, "cfapi_object_apply_below"},
    {cfapi_archetype_get_first,     70, "cfapi_archetype_get_first"},
    {cfapi_archetype_get_property,  71, "cfapi_archetype_get_property"},
    {cfapi_party_get_property,      72, "cfapi_party_get_property"},
    {cfapi_region_get_property,     73, "cfapi_region_get_property"},
    {cfapi_player_can_pay,          74, "cfapi_player_can_pay"},
    {cfapi_log,                     75, "cfapi_log"},
    {cfapi_get_time,                76, "cfapi_system_get_time"},
    {cfapi_timer_create,            77, "cfapi_system_timer_create"},
    {cfapi_timer_destroy,           78, "cfapi_system_timer_destroy"},
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
            esrv_send_item(tmp, op);
    } else {
        for (tmp = op->above; tmp != NULL; tmp = tmp->above)
            if (tmp->type == PLAYER)
                esrv_send_item(tmp, op);
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

                    rv = plugin->eventfunc(&rvt, op, eventcode, activator, third, message, fix, tmp->slaying, tmp->name);
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
    char* buf;
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
        for (cp = plugins_list; cp != NULL; cp = cp->next) {
            if (cp->gevent[eventcode] != NULL)
                cp->gevent[eventcode](&rt, eventcode);
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
    strcpy(cp->id, propfunc(&i, "Identification"));
    strcpy(cp->fullname, propfunc(&i, "FullName"));
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
    f_plug_api rv;
    int i;

    va_start(args, type);
    request_type = va_arg(args, int);
    if (request_type == 0) { /* By nr */
        fid = va_arg(args, int);
        if (fid < 0 || fid >= NR_OF_HOOKS) {
            rv = NULL;
            *type = CFAPI_NONE;
        } else {
            rv = plug_hooks[fid].func;
            *type = CFAPI_FUNC;
        }
    } else { /* by name */
        buf = va_arg(args, char*);
        rv = NULL;
        for (i = 0; i < NR_OF_HOOKS; i++) {
            if (!strcmp(buf, plug_hooks[i].fname)) {
                rv = plug_hooks[i].func;
                *type = CFAPI_FUNC;
                break;
            }
        }
        if (rv == NULL) {
            *type = CFAPI_NONE;
        }
    }
    va_end(args);
    return rv;
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
 * Finds an animation.
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

void* cfapi_system_strdup_local(int *type, ...)
{
    va_list args;
    char* txt;
    va_start(args, type);
    txt = va_arg(args, char*);
    va_end(args);
    *type = CFAPI_STRING;
    return strdup_local(txt);
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

    cp = plugins_find_plugin(pname);
    cp->gevent[eventcode] = NULL;

    va_end(args);
    return NULL;
}

void* cfapi_system_add_string(int *type, ...)
{
    va_list args;
    const char* str;
    char* rv;

    va_start(args, type);
    str = va_arg(args, char*);
    va_end(args);

    rv = (char*)add_string(str);
    *type = CFAPI_STRING;
    return rv;
}

void* cfapi_system_remove_string(int *type, ...)
{
    va_list args;
    char* str;

    va_start(args, type);
    str = va_arg(args, char*);
    va_end(args);

    free_string(str);
    *type = CFAPI_NONE;
    return NULL;
}
/**
 * Checks if a file exists.
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

void* cfapi_system_re_cmp(int* type, ...)
{
    va_list args;
    char* rv;
    const char* str;
    const char* regexp;

    va_start(args, type);

    str = va_arg(args, char*);
    regexp = va_arg(args, char*);

    rv = (char*)re_cmp(str, regexp);

    va_end(args);
    *type = CFAPI_STRING;
    return rv;
}

void* cfapi_system_directory(int* type, ...)
{
    va_list args;
    int dirtype;

    va_start(args, type);

    dirtype = va_arg(args, int);
    va_end(args);

    *type = CFAPI_STRING;

    switch (dirtype)
    {
    case 0:
        return settings.mapdir;
        break;

    case 1:
        return settings.uniquedir;
        break;

    case 2:
        return settings.tmpdir;
        break;

    case 3:
        return settings.confdir;
        break;

    case 4:
        return settings.localdir;
        break;

    case 5:
        return settings.playerdir;
        break;

    case 6:
        return settings.datadir;
        break;
    }

    *type = CFAPI_NONE;
    return NULL;
}

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
 *
 * @see cftimer_create().
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
 * @param type
 * unused
 * @return
 * always 0
 *
 * Additional parameters:
 * - timer: int that should be destroyed
 * - err: int* which will contain the return code of cftimer_destroy().
 *
 * @see cftimer_destroy().
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

/* Logging hook */
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

    return NULL;
}

/* MAP RELATED HOOKS */

void* cfapi_map_get_map(int* type, ...)
{
    va_list args;
    mapstruct* rv;
    int ctype;
    int x, y;
    sint16 nx, ny;
    char* name;
    mapstruct* m;

    va_start(args, type);

    ctype = va_arg(args, int);

    switch (ctype)
    {
    case 0:
        x = va_arg(args, int);
        y = va_arg(args, int);
        rv = get_empty_map(x, y);
        break;

    case 1:
        name = va_arg(args, char*);
        x = va_arg(args, int);
        rv = ready_map_name(name, x);
        break;

    case 2:
        m = va_arg(args, mapstruct*);
        nx = va_arg(args, int);
        ny = va_arg(args, int);
        rv = get_map_from_coord(m, &nx, &ny);
        break;

    case 3:
        rv = first_map;
        break;

    default:
        *type = CFAPI_NONE;
        va_end(args);
        return NULL;
        break;
    }
    va_end(args);
    *type = CFAPI_PMAP;
    return rv;
}
void* cfapi_map_has_been_loaded(int* type, ...)
{
    va_list args;
    mapstruct* map;
    char* string;

    va_start(args, type);
    string = va_arg(args, char*);
    map = has_been_loaded(string);
    va_end(args);
    *type = CFAPI_PMAP;
    return map;
}
void* cfapi_map_create_path(int* type, ...)
{
    va_list args;
    int ctype;
    const char* str;
    char* rv;
    static char name[MAX_BUF];
    va_start(args, type);

    ctype = va_arg(args, int);
    str = va_arg(args, char*);
    *type = CFAPI_STRING;

    switch (ctype)
    {
    case 0:
        create_pathname(str, name, MAX_BUF);
        rv = name;
        break;

    case 1:
        create_overlay_pathname(str, name, MAX_BUF);
        rv = name;
        break;

    /*case 2:
        rv = create_items_path(str);
        break;*/

    default:
        rv = NULL;
        *type = CFAPI_NONE;
        break;
    }
    va_end(args);
    return rv;
}
void* cfapi_map_get_map_property(int* type, ...)
{
    va_list args;
    int x, y;
    sint16 nx, ny;
    mapstruct* map;
    mapstruct* newmap;
    static int rv;
    int property;
    char* buf;

    va_start(args, type);

    property = va_arg(args, int);
    switch (property)
    {
    case CFAPI_MAP_PROP_FLAGS:
        map = va_arg(args, mapstruct*);
        newmap = va_arg(args, mapstruct*);
        x = va_arg(args, int);
        y = va_arg(args, int);
        nx = va_arg(args, int);
        ny = va_arg(args, int);
        rv = get_map_flags(map, &newmap, x, y, &nx, &ny);
        va_end(args);
        *type = CFAPI_INT;
        return &rv;
        break;

    case CFAPI_MAP_PROP_DIFFICULTY:
        map = va_arg(args, mapstruct*);
        rv = calculate_difficulty(map);
        va_end(args);
        *type = CFAPI_INT;
        return &rv;
        break;

    case CFAPI_MAP_PROP_PATH:
        map = va_arg(args, mapstruct*);
        buf = map->path;
        *type = CFAPI_STRING;
        va_end(args);
        return buf;
        break;

    case CFAPI_MAP_PROP_TMPNAME:
        map = va_arg(args, mapstruct*);
        buf = map->tmpname;
        *type = CFAPI_STRING;
        va_end(args);
        return buf;
        break;

    case CFAPI_MAP_PROP_NAME:
        map = va_arg(args, mapstruct*);
        buf = map->name;
        *type = CFAPI_STRING;
        va_end(args);
        return buf;
        break;

    case CFAPI_MAP_PROP_RESET_TIME:
        map = va_arg(args, mapstruct*);
        rv = map->reset_time;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_RESET_TIMEOUT:
        map = va_arg(args, mapstruct*);
        rv = map->reset_timeout;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_PLAYERS:
        map = va_arg(args, mapstruct*);
        rv = map->players;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_DARKNESS:
        map = va_arg(args, mapstruct*);
        rv = map->darkness;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_WIDTH:
        map = va_arg(args, mapstruct*);
        rv = map->width;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_HEIGHT:
        map = va_arg(args, mapstruct*);
        rv = map->height;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_ENTER_X:
        map = va_arg(args, mapstruct*);
        rv = map->enter_x;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_ENTER_Y:
        map = va_arg(args, mapstruct*);
        rv = map->enter_y;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_TEMPERATURE:
        map = va_arg(args, mapstruct*);
        rv = map->temp;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_PRESSURE:
        map = va_arg(args, mapstruct*);
        rv = map->pressure;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_HUMIDITY:
        map = va_arg(args, mapstruct*);
        rv = map->humid;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_WINDSPEED:
        map = va_arg(args, mapstruct*);
        rv = map->windspeed;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_WINDDIR:
        map = va_arg(args, mapstruct*);
        rv = map->winddir;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_SKY:
        map = va_arg(args, mapstruct*);
        rv = map->sky;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_WPARTX:
        map = va_arg(args, mapstruct*);
        rv = map->wpartx;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_WPARTY:
        map = va_arg(args, mapstruct*);
        rv = map->wparty;
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_MESSAGE:
        map = va_arg(args, mapstruct*);
        buf = map->msg;
        *type = CFAPI_STRING;
        va_end(args);
        return buf;
        break;

    case CFAPI_MAP_PROP_NEXT:
        map = va_arg(args, mapstruct*);
        *type = CFAPI_PMAP;
        va_end(args);
        return map->next;
        break;

    case CFAPI_MAP_PROP_REGION:
        map = va_arg(args, mapstruct*);
        *type = CFAPI_PREGION;
        va_end(args);
        return get_region_by_map(map);
        break;

    default:
        *type = CFAPI_NONE;
        va_end(args);
        return NULL;
        break;
    }
}

void* cfapi_map_set_map_property(int* type, ...)
{
    va_list args;
    static int rv;
    mapstruct* map;
    int val;
    int property;

    va_start(args, type);

    property = va_arg(args, int);

    switch (property)
    {
    case CFAPI_MAP_PROP_LIGHT:
        map = va_arg(args, mapstruct*);
        val = va_arg(args, int);
        rv = change_map_light(map, val);
        *type = CFAPI_INT;
        va_end(args);
        return &rv;
        break;

    case CFAPI_MAP_PROP_RESET_TIME:
        map = va_arg(args, mapstruct*);
        *type = CFAPI_NONE;
        va_end(args);
        return NULL;
        break;

    default:
        *type = CFAPI_NONE;
        va_end(args);
        return NULL;
        break;
    }
}
void* cfapi_map_out_of_map(int* type, ...)
{
    va_list args;
    static int rv;
    mapstruct* map;
    int x, y;

    va_start(args, type);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);

    rv = out_of_map(map, x, y);
    va_end(args);
    *type = CFAPI_INT;
    return &rv;
}
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
void* cfapi_map_get_object_at(int* type, ...)
{
    va_list args;
    mapstruct* map;
    int x, y;
    object* rv;

    va_start(args, type);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);
    va_end(args);

    rv = get_map_ob(map, x, y);
    *type = CFAPI_POBJECT;
    return rv;
}
void* cfapi_map_get_flags(int* type, ...)
{
    va_list args;
    sint16 x, y;
    sint16 *nx, *ny;
    static mapstruct* map;
    mapstruct** newmap;
    static int rv;

    va_start(args, type);

    map = va_arg(args, mapstruct*);
    newmap = va_arg(args, mapstruct**);
    x = va_arg(args, int);
    y = va_arg(args, int);
    nx = va_arg(args, sint16*);
    ny = va_arg(args, sint16*);
    va_end(args);

    rv = get_map_flags(map, newmap, x, y, nx, ny);

    *type = CFAPI_INT;
    return &rv;
}
void* cfapi_map_present_arch_by_name(int* type, ...)
{
    va_list args;
    object* rv;
    int x, y;
    mapstruct* map;
    char* msg;

    va_start(args, type);

    msg = va_arg(args, char*);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);

    va_end(args);

    rv = present_arch(find_archetype(msg), map, x, y);
    *type = CFAPI_POBJECT;
    return rv;
}

/* OBJECT-RELATED HOOKS */

void* cfapi_object_move(int* type, ...)
{
    va_list args;
    int     kind;
    object* op;
    object* activator;
    player* pl;
    int     direction;
    static int rv=0;

    va_start(args, type);
    kind = va_arg(args, int);
    switch (kind)
    {
    case 0:
        op = va_arg(args, object*);
        direction = va_arg(args, int);
        activator = va_arg(args, object*);
        va_end(args);
        rv = move_ob(op, direction, activator);
        break;

    case 1:
        pl = va_arg(args, player*);
        direction = va_arg(args, int);
        va_end(args);
        rv = move_player(pl->ob, direction);
        break;
    }
    *type = CFAPI_INT;
    return &rv;
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
    *type = CFAPI_SSTRING;
    return NULL;
}
void* cfapi_object_get_property(int* type, ...)
{
    va_list args;
    int property;
    object* op;
    void* rv;
    static int ri;
    static float rf;
    static char name[MAX_BUF];

    va_start(args, type);

    op = va_arg(args, object*);
    property = va_arg(args, int);
    rv = NULL;
    if (op != NULL) {
        switch (property)
        {
        case CFAPI_OBJECT_PROP_OB_ABOVE:
            rv = op->above;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_OB_BELOW:
            rv = op->below;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_NEXT_ACTIVE_OB:
            rv = op->active_next;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_PREV_ACTIVE_OB:
            rv = op->active_prev;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_INVENTORY:
            rv = op->inv;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_ENVIRONMENT:
            rv = op->env;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_HEAD:
            rv = op->head;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_CONTAINER:
            rv = op->container;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_MAP:
            rv = op->map;
            *type = CFAPI_PMAP;
            break;

        case CFAPI_OBJECT_PROP_COUNT:
            rv = &op->count;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_NAME:
            query_name(op, name, MAX_BUF);
            rv = name;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_NAME_PLURAL:
            rv = (char*)op->name_pl;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_TITLE:
            rv = (char*)op->title;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_RACE:
            rv = (char*)op->race;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_SLAYING:
            rv = (char*)op->slaying;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_SKILL:
            rv = (char*)op->skill;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_MESSAGE:
            rv = (char*)op->msg;
            if (rv == NULL)
                rv = "";
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_LORE:
            rv = (char*)op->lore;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_X:
            ri = op->x;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_Y:
            ri = op->y;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SPEED:
            rv = &op->speed;
            *type = CFAPI_DOUBLE;
            break;

        case CFAPI_OBJECT_PROP_SPEED_LEFT:
            rv = &op->speed_left;
            *type = CFAPI_DOUBLE;
            break;

        case CFAPI_OBJECT_PROP_NROF:
            rv = &op->nrof;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_DIRECTION:
            rv = &op->direction;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FACING:
            rv = &op->facing;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_TYPE:
            ri = op->type;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SUBTYPE:
            ri = op->subtype;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CLIENT_TYPE:
            ri = op->client_type;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_RESIST:
            {
                int idx;
                idx = va_arg(args, int);
                rv = &op->resist[idx];
            }
            *type = CFAPI_INT16;
            break;

        case CFAPI_OBJECT_PROP_ATTACK_TYPE:
            rv = &op->attacktype;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PATH_ATTUNED:
            rv = &op->path_attuned;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PATH_REPELLED:
            rv = &op->path_repelled;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PATH_DENIED:
            rv = &op->path_denied;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL:
            rv = &op->material;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL_NAME:
            rv = (char*)op->materialname;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_MAGIC:
            rv = &op->magic;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_VALUE:
            rv = &op->value;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LEVEL:
            rv = &op->level;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_HEAL:
            ri = op->last_heal;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_SP:
            ri = op->last_sp;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_GRACE:
            ri = op->last_grace;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LAST_EAT:
            ri = op->last_eat;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_INVISIBLE_TIME:
            rv = &op->invisible;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PICK_UP:
            rv = &op->pick_up;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_ITEM_POWER:
            rv = &op->item_power;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GEN_SP_ARMOUR:
            rv = &op->gen_sp_armour;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WEIGHT:
            rv = &op->weight;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WEIGHT_LIMIT:
            rv = &op->weight_limit;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CARRYING:
            rv = &op->carrying;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GLOW_RADIUS:
            rv = &op->glow_radius;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PERM_EXP:
            rv = &op->perm_exp;
            *type = CFAPI_LONG;
            break;

        case CFAPI_OBJECT_PROP_CURRENT_WEAPON:
            rv = op->current_weapon;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_ENEMY:
            rv = op->enemy;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_ATTACKED_BY:
            rv = op->attacked_by;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_RUN_AWAY:
            rv = &op->run_away;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CHOSEN_SKILL:
            rv = op->chosen_skill;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_HIDDEN:
            rv = &op->hide;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MOVE_STATUS:
            rv = &op->move_status;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_ATTACK_MOVEMENT:
            rv = &op->attack_movement;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SPELL_ITEM:
            rv = op->spellitem;
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_EXP_MULTIPLIER:
            rv = &op->expmul;
            *type = CFAPI_DOUBLE;
            break;

        case CFAPI_OBJECT_PROP_ARCHETYPE:
            rv = op->arch;
            *type = CFAPI_PARCH;
            break;

        case CFAPI_OBJECT_PROP_OTHER_ARCH:
            rv = op->other_arch;
            *type = CFAPI_PARCH;
            break;

        case CFAPI_OBJECT_PROP_CUSTOM_NAME:
            rv = (char*)op->custom_name;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_ANIM_SPEED:
            rv = &op->anim_speed;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FRIENDLY:
            ri = is_friendly(op);
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SHORT_NAME:
            query_short_name(op, name, MAX_BUF);
            rv = name;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_BASE_NAME:
            {
                int i;
                i = va_arg(args, int);
                query_base_name(op, i, name, MAX_BUF);
                rv = name;
                *type = CFAPI_STRING;
            }
            break;

        case CFAPI_OBJECT_PROP_MAGICAL:
            ri = is_magical(op);
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_LUCK:
            rv = &op->stats.luck;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_EXP:
            rv = &op->stats.exp;
            *type = CFAPI_SINT64;
            break;

        case CFAPI_OBJECT_PROP_OWNER:
            rv = get_owner(op);
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_PRESENT:
            {
                int stype;
                rv = 0;
                stype = va_arg(args, int);
                switch (stype) {

                unsigned char ptype;
                char* buf;
                archetype* at;

                case 0: /* present_in_ob */
                    ptype = (unsigned char)(va_arg(args, int));
                    rv = present_in_ob(ptype, op);
                    break;

                case 1: /* present_in_ob_by_name */
                    ptype = (unsigned char)(va_arg(args, int));
                    buf = va_arg(args, char*);
                    rv = present_in_ob_by_name(ptype, buf, op);
                    break;

                case 2: /* present_arch_in_ob */
                    at = va_arg(args, archetype*);
                    rv = present_arch_in_ob(at, op);
                    break;
                }
            }
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_OBJECT_PROP_CHEATER:
            ri = (QUERY_FLAG(op, FLAG_WAS_WIZ));
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MERGEABLE:
            {
                object* op2;
                op2 = va_arg(args, object*);
                ri = can_merge(op, op2);
                rv = &ri;
            }
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_PICKABLE:
            {
                object* op2;
                rv = 0;
                op2 = va_arg(args, object*);
                ri = can_pick(op2, op);
                rv = &ri;
            }
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FLAGS:
            {
                int fl;
                ri = 0;
                fl = va_arg(args, int);
                ri = QUERY_FLAG(op, fl);
                rv = &ri;
            }
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_STR:
            ri = op->stats.Str;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_DEX:
            ri = op->stats.Dex;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CON:
            ri = op->stats.Con;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WIS:
            ri = op->stats.Wis;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_INT:
            ri = op->stats.Int;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_POW:
            ri = op->stats.Pow;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_CHA:
            ri = op->stats.Cha;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_WC:
            ri = op->stats.wc;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_AC:
            ri = op->stats.ac;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_HP:
            ri = op->stats.hp;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_SP:
            ri = op->stats.sp;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GP:
            ri = op->stats.grace;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FP:
            ri = op->stats.food;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MAXHP:
            ri = op->stats.maxhp;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MAXSP:
            ri = op->stats.maxsp;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_MAXGP:
            ri = op->stats.maxgrace;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_DAM:
            ri = op->stats.dam;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_GOD:
            rv = (char*)determine_god(op);
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_ARCH_NAME:
            rv = (char*)op->arch->name;
            *type = CFAPI_STRING;
            break;

        case CFAPI_OBJECT_PROP_INVISIBLE:
            ri = op->invisible;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_OBJECT_PROP_FACE:
            ri = op->animation_id;
            rv = &ri;
            *type = CFAPI_INT;
            break;

        case CFAPI_PLAYER_PROP_IP:
            rv = op->contr->socket.host;
            *type = CFAPI_STRING;
            break;

        case CFAPI_PLAYER_PROP_MARKED_ITEM:
            rv = find_marked_object(op);
            *type = CFAPI_POBJECT;
            break;

        case CFAPI_PLAYER_PROP_PARTY:
            rv = (op->contr ? op->contr->party : NULL);
            *type = CFAPI_PPARTY;
            break;
        case CFAPI_OBJECT_PROP_NO_SAVE:
            ri = op->no_save;
            rv = &ri;
            *type = CFAPI_INT;
            break;
        case CFAPI_OBJECT_PROP_MOVE_TYPE:
            ri = op->move_type;
            rv = &ri;
            *type = CFAPI_CHAR;
            break;
        case CFAPI_OBJECT_PROP_MOVE_BLOCK:
            ri = op->move_block;
            rv = &ri;
            *type = CFAPI_CHAR;
            break;
        case CFAPI_OBJECT_PROP_MOVE_ALLOW:
            ri = op->move_allow;
            rv = &ri;
            *type = CFAPI_CHAR;
            break;
        case CFAPI_OBJECT_PROP_MOVE_ON:
            ri = op->move_on;
            rv = &ri;
            *type = CFAPI_CHAR;
            break;
        case CFAPI_OBJECT_PROP_MOVE_OFF:
            ri = op->move_off;
            rv = &ri;
            *type = CFAPI_CHAR;
            break;
        case CFAPI_OBJECT_PROP_MOVE_SLOW:
            ri = op->move_type;
            rv = &ri;
            *type = CFAPI_CHAR;
            break;
        case CFAPI_OBJECT_PROP_MOVE_SLOW_PENALTY:
            rf = op->move_slow_penalty;
            rv = &rf;
            *type = CFAPI_FLOAT;
            break;
        case CFAPI_PLAYER_PROP_BED_MAP:
            ri = op->no_save;
            rv = op->contr->savebed_map;
            *type = CFAPI_STRING;
            break;
        case CFAPI_PLAYER_PROP_BED_X:
            ri = op->contr->bed_x;
            rv = &ri;
            *type = CFAPI_INT;
            break;
        case CFAPI_PLAYER_PROP_BED_Y:
            ri = op->contr->bed_y;
            rv = &ri;
            *type = CFAPI_INT;
            break;
        default:
            *type = CFAPI_NONE;
            break;
        }
    }
    va_end(args);
    return rv;
}

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
    void* rv;
    sint64 s64arg;
    partylist* partyarg;
    va_start(args, type);

    op = va_arg(args, object*);
    property = va_arg(args, int);
    rv = NULL;

    if (op != NULL && (!op->arch || (op != &op->arch->clone))) {
        switch (property)
        {
        case CFAPI_OBJECT_PROP_NAME:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->name, sarg);
            send_changed_object(op);
            break;

        case CFAPI_OBJECT_PROP_NAME_PLURAL:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->name_pl, sarg);
            send_changed_object(op);
            break;

        case CFAPI_OBJECT_PROP_TITLE:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->title, sarg);
            break;

        case CFAPI_OBJECT_PROP_RACE:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->race, sarg);
            break;

        case CFAPI_OBJECT_PROP_SLAYING:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->slaying, sarg);
            break;

        case CFAPI_OBJECT_PROP_SKILL:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->skill, sarg);
            break;

        case CFAPI_OBJECT_PROP_MESSAGE:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->msg, sarg);
            break;

        case CFAPI_OBJECT_PROP_LORE:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->lore, sarg);
            break;

        case CFAPI_OBJECT_PROP_SPEED:
            darg = va_arg(args, double);
            if (darg != op->speed) {
                op->speed = darg;
                update_ob_speed(op);
            }
            break;

        case CFAPI_OBJECT_PROP_SPEED_LEFT:
            darg = va_arg(args, double);
            op->speed_left = darg;
            break;

        case CFAPI_OBJECT_PROP_NROF:
            iarg = va_arg(args, int);
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

        case CFAPI_OBJECT_PROP_DIRECTION:
            iarg = va_arg(args, int);
            op->direction = iarg;
            break;

        case CFAPI_OBJECT_PROP_FACING:
            iarg = va_arg(args, int);
            op->facing = iarg;
            break;

        case CFAPI_OBJECT_PROP_RESIST:
            {
                int iargbis = va_arg(args, int);
                iarg = va_arg(args, int);
                op->resist[iargbis] = iarg;
            }
            break;

        case CFAPI_OBJECT_PROP_ATTACK_TYPE:
            iarg = va_arg(args, int);
            op->attacktype = iarg;
            break;

        case CFAPI_OBJECT_PROP_PATH_ATTUNED:
            iarg = va_arg(args, int);
            op->path_attuned = iarg;
            break;

        case CFAPI_OBJECT_PROP_PATH_REPELLED:
            iarg = va_arg(args, int);
            op->path_repelled = iarg;
            break;

        case CFAPI_OBJECT_PROP_PATH_DENIED:
            iarg = va_arg(args, int);
            op->path_denied = iarg;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL:
            iarg = va_arg(args, int);
            op->material = iarg;
            break;

        case CFAPI_OBJECT_PROP_MATERIAL_NAME:
            break;

        case CFAPI_OBJECT_PROP_MAGIC:
            iarg = va_arg(args, int);
            op->magic = iarg;
            break;

        case CFAPI_OBJECT_PROP_VALUE:
            iarg = va_arg(args, int);
            op->value = iarg;
            break;

        case CFAPI_OBJECT_PROP_LEVEL:
            iarg = va_arg(args, int);
            op->level = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_HEAL:
            iarg = va_arg(args, int);
            op->last_heal = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_SP:
            iarg = va_arg(args, int);
            op->last_sp = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_GRACE:
            iarg = va_arg(args, int);
            op->last_grace = iarg;
            break;

        case CFAPI_OBJECT_PROP_LAST_EAT:
            iarg = va_arg(args, int);
            op->last_eat = iarg;
            break;

        case CFAPI_OBJECT_PROP_INVISIBLE_TIME:
            iarg = va_arg(args, int);
            op->invisible = iarg;
            break;

        case CFAPI_OBJECT_PROP_PICK_UP:
            iarg = va_arg(args, int);
            op->pick_up = iarg;
            break;

        case CFAPI_OBJECT_PROP_ITEM_POWER:
            iarg = va_arg(args, int);
            op->item_power = iarg;
            break;

        case CFAPI_OBJECT_PROP_GEN_SP_ARMOUR:
            iarg = va_arg(args, int);
            op->gen_sp_armour = iarg;
            break;

        case CFAPI_OBJECT_PROP_WEIGHT:
            iarg = va_arg(args, int);
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
            op->weight_limit = iarg;
            break;

        case CFAPI_OBJECT_PROP_GLOW_RADIUS:
            iarg = va_arg(args, int);
            op->glow_radius = iarg;
            break;

        case CFAPI_OBJECT_PROP_PERM_EXP:
            larg = va_arg(args, long);
            op->perm_exp = larg;
            break;

        case CFAPI_OBJECT_PROP_ENEMY:
            oparg = va_arg(args, object*);
            op->enemy = oparg;
            break;

        case CFAPI_OBJECT_PROP_RUN_AWAY:
            iarg = va_arg(args, int);
            op->run_away = iarg;
            break;

        case CFAPI_OBJECT_PROP_CHOSEN_SKILL:
            oparg = va_arg(args, object*);
            op->chosen_skill = oparg;
            break;

        case CFAPI_OBJECT_PROP_HIDDEN:
            iarg = va_arg(args, int);
            op->hide = iarg;
            break;

        case CFAPI_OBJECT_PROP_MOVE_STATUS:
            iarg = va_arg(args, int);
            op->move_status = iarg;
            break;

        case CFAPI_OBJECT_PROP_ATTACK_MOVEMENT:
            iarg = va_arg(args, int);
            op->attack_movement = iarg;
            break;

        case CFAPI_OBJECT_PROP_SPELL_ITEM:
            oparg = va_arg(args, object*);
            op->spellitem = oparg;
            break;

        case CFAPI_OBJECT_PROP_EXP_MULTIPLIER:
            darg = va_arg(args, double);
            op->expmul = darg;
            break;

        case CFAPI_OBJECT_PROP_CUSTOM_NAME:
            sarg = va_arg(args, char*);
            FREE_AND_COPY(op->custom_name, sarg);
            send_changed_object(op);
            break;

        case CFAPI_OBJECT_PROP_ANIM_SPEED:
            iarg = va_arg(args, int);
            op->anim_speed = iarg;
            break;

        case CFAPI_OBJECT_PROP_FRIENDLY:
            iarg = va_arg(args, int);
            if (iarg == 1 && is_friendly(op) == 0)
                add_friendly_object(op);
            else if (iarg == 0 && is_friendly(op) == 1)
                remove_friendly_object(op);
            break;

        case CFAPI_OBJECT_PROP_LUCK:
            iarg = va_arg(args, int);
            op->stats.luck = iarg;
            break;

        case CFAPI_OBJECT_PROP_EXP:
            {
                char* skillname;

                s64arg = va_arg(args, sint64);
                skillname = va_arg(args, char*);
                iarg = va_arg(args, int);
                change_exp(op, s64arg, skillname, iarg);
            }
            break;

        case CFAPI_OBJECT_PROP_OWNER:
            oparg = va_arg(args, object*);
            set_owner(op, oparg);
            break;

        case CFAPI_OBJECT_PROP_CHEATER:
            set_cheat(op);
            break;

        case CFAPI_OBJECT_PROP_FLAGS:
            {
                int iargbis;
                iarg = va_arg(args, int);
                iargbis = va_arg(args, int);
                if (iargbis == 1)
                    SET_FLAG(op, iarg);
                else
                    CLEAR_FLAG(op, iarg);
            }
            break;

        case CFAPI_OBJECT_PROP_STR:
            iarg = va_arg(args, int);
            op->stats.Str=iarg;
            break;

        case CFAPI_OBJECT_PROP_DEX:
            iarg = va_arg(args, int);
            op->stats.Dex=iarg;
            break;

        case CFAPI_OBJECT_PROP_CON:
            iarg = va_arg(args, int);
            op->stats.Con=iarg;
            break;

        case CFAPI_OBJECT_PROP_WIS:
            iarg = va_arg(args, int);
            op->stats.Wis=iarg;
            break;

        case CFAPI_OBJECT_PROP_INT:
            iarg = va_arg(args, int);
            op->stats.Int=iarg;
            break;

        case CFAPI_OBJECT_PROP_POW:
            iarg = va_arg(args, int);
            op->stats.Pow=iarg;
            break;

        case CFAPI_OBJECT_PROP_CHA:
            iarg = va_arg(args, int);
            op->stats.Cha=iarg;
            break;

        case CFAPI_OBJECT_PROP_WC:
            iarg = va_arg(args, int);
            op->stats.wc=iarg;
            break;

        case CFAPI_OBJECT_PROP_AC:
            iarg = va_arg(args, int);
            op->stats.ac=iarg;
            break;

        case CFAPI_OBJECT_PROP_HP:
            iarg = va_arg(args, int);
            op->stats.hp=iarg;
            break;

        case CFAPI_OBJECT_PROP_SP:
            iarg = va_arg(args, int);
            op->stats.sp=iarg;
            break;

        case CFAPI_OBJECT_PROP_GP:
            iarg = va_arg(args, int);
            op->stats.grace=iarg;
            break;

        case CFAPI_OBJECT_PROP_FP:
            iarg = va_arg(args, int);
            op->stats.food=iarg;
            break;

        case CFAPI_OBJECT_PROP_MAXHP:
            iarg = va_arg(args, int);
            op->stats.maxhp=iarg;
            break;

        case CFAPI_OBJECT_PROP_MAXSP:
            iarg = va_arg(args, int);
            op->stats.maxsp=iarg;
            break;

        case CFAPI_OBJECT_PROP_MAXGP:
            iarg = va_arg(args, int);
            op->stats.maxgrace=iarg;
            break;

        case CFAPI_OBJECT_PROP_DAM:
            iarg = va_arg(args, int);
            op->stats.dam=iarg;
            break;

        case CFAPI_OBJECT_PROP_FACE:
            iarg = va_arg(args, int);
            op->animation_id = iarg;
            update_object(op, UP_OBJ_FACE);
            break;

        case CFAPI_OBJECT_ANIMATION:
            iarg = va_arg(args, int);
            if (iarg != -1) {
                SET_ANIMATION(op, iarg);
            }
            update_object(op, UP_OBJ_FACE);
            break;

        case CFAPI_PLAYER_PROP_MARKED_ITEM:
            if (op->contr) {
                oparg = va_arg(args, object*);
                op->contr->mark = oparg;
                if (oparg)
                    op->contr->mark_count = oparg->count;
            }
            break;

        case CFAPI_PLAYER_PROP_PARTY:
            if (op->contr) {
                partyarg = va_arg(args, partylist*);
                op->contr->party = partyarg;
            }
            break;
        case CFAPI_OBJECT_PROP_NO_SAVE:
            iarg = va_arg(args, int);
            op->no_save = iarg;
            break;
        case CFAPI_PLAYER_PROP_BED_MAP:
            sarg = va_arg(args, char*);
            strncpy(op->contr->savebed_map, sarg, MAX_BUF);
            break;
        case CFAPI_PLAYER_PROP_BED_X:
            iarg = va_arg(args, int);
            op->contr->bed_x = iarg;
            break;
        case CFAPI_PLAYER_PROP_BED_Y:
            iarg = va_arg(args, int);
            op->contr->bed_y = iarg;
            break;
        default:
            *type = CFAPI_NONE;
            break;
        }
    }
    va_end(args);

    *type = CFAPI_NONE;
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
void* cfapi_object_describe(int* type, ...)
{
    va_list args;
    object* op;
    object* owner;
    static char desc[VERY_BIG_BUF];

    va_start(args, type);

    op = va_arg(args, object*);
    owner = va_arg(args, object*);
    va_end(args);

    *type = CFAPI_STRING;
    describe_item(op, owner, desc, VERY_BIG_BUF);
    return desc;
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

    va_end(args);

    free_object(op);

    *type = CFAPI_NONE;
    return NULL;
}
void* cfapi_object_clone(int* type, ...)
{
    va_list args;
    object* op;
    int kind;

    va_start(args, type);

    op = va_arg(args, object*);
    kind = va_arg(args, int);

    va_end(args);

    if (kind == 0) {
        *type = CFAPI_POBJECT;
        return object_create_clone(op);
    } else {
        object* tmp;
        tmp = get_object();
        copy_object(op, tmp);
        *type = CFAPI_POBJECT;
        return tmp;
    }
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
void* cfapi_object_create(int* type, ...)
{
    va_list args;
    int ival;
    va_start(args, type);
    ival = va_arg(args, int);

    *type = CFAPI_POBJECT;
    switch (ival)
    {
    case 0:
        va_end(args);
        return get_object();
        break;

    case 1: /* Named object. Nearly the old plugin behavior, but we don't add artifact suffixes */
        {
            char* sval;
            object* op;
            char name[MAX_BUF];

            sval = va_arg(args, char*);

            op = create_archetype_by_object_name(sval);

            query_name(op, name, MAX_BUF);
            if (strncmp(name, ARCH_SINGULARITY, ARCH_SINGULARITY_LEN) == 0) {
                free_object(op);
                /* Try with archetype names... */
                op = create_archetype(sval);
                query_name(op, name, MAX_BUF);
                if (strncmp(name, ARCH_SINGULARITY, ARCH_SINGULARITY_LEN) == 0) {
                    free_object(op);
                    *type = CFAPI_NONE;
                    va_end(args);
                    return NULL;
                }
            }
            va_end(args);
            return op;
        }
        break;

    default:
        *type = CFAPI_NONE;
        va_end(args);
        return NULL;
        break;
    }
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
    void* rv = NULL;

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
        rv = insert_ob_in_map_at(op, map, orig, flag, x, y);
        *type = CFAPI_POBJECT;
        break;

    case 1:
        map = va_arg(args, mapstruct*);
        orig = va_arg(args, object*);
        flag = va_arg(args, int);
        rv = insert_ob_in_map(op, map, orig, flag);
        *type = CFAPI_POBJECT;
        break;

    case 2:
        arch_string = va_arg(args, char*);
        replace_insert_ob_in_map(arch_string, op);
        *type = CFAPI_NONE;
        break;

    case 3:
        orig = va_arg(args, object*);
        rv = insert_ob_in_ob(op, orig);
        if (orig->type == PLAYER) {
            esrv_send_item(orig, op);
        }
        *type = CFAPI_POBJECT;
        break;
    }

    va_end(args);

    return rv;
}
void* cfapi_object_split(int* type, ...)
{
    va_list args;

    int nr;
    object* op;
    va_start(args, type);

    op = va_arg(args, object*);
    nr = va_arg(args, int);

    va_end(args);
    *type = CFAPI_POBJECT;
    return get_split_ob(op, nr, NULL, 0);
}
void* cfapi_object_merge(int* type, ...)
{
    va_list args;
    object* op;
    object* op2;

    va_start(args, type);

    op = va_arg(args, object*);
    op2 = va_arg(args, object*);

    va_end(args);


    *type = CFAPI_POBJECT;
    return merge_ob(op, op2);
}
void* cfapi_object_distance(int* type, ...)
{
    va_list args;
    static int rv;
    object* op;
    object* op2;
    va_start(args, type);

    op = va_arg(args, object*);
    op2 = va_arg(args, object*);

    va_end(args);

    *type = CFAPI_INT;
    rv = distance(op, op2);
    return &rv;
}
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
    static int rv;
    object* op1;
    object* op2;

    va_start(args, type);
    op1 = va_arg(args, object*);
    op2 = va_arg(args, object*);

    rv = on_same_map(op1, op2);
    va_end(args);

    *type = CFAPI_INT;
    return &rv;
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

void* cfapi_object_check_trigger(int* type, ...)
{
    object* op;
    object* cause;
    va_list args;
    static int rv;

    va_start(args, type);
    op = va_arg(args, object*);
    cause = va_arg(args, object*);
    va_end(args);

    rv = check_trigger(op, cause);
    *type = CFAPI_INT;
    return &rv;
}

void* cfapi_object_query_cost(int* type, ...)
{
    object* op;
    object* who;
    int flags;
    va_list args;
    static int rv;

    va_start(args, type);
    op = va_arg(args, object*);
    who = va_arg(args, object*);
    flags = va_arg(args, int);
    va_end(args);

    rv = query_cost(op, who, flags);
    *type = CFAPI_INT;
    return &rv;
}

void* cfapi_object_query_money(int* type, ...)
{
    object* op;
    va_list args;
    static int rv;

    va_start(args, type);
    op = va_arg(args, object*);
    va_end(args);

    rv = query_money(op);
    *type = CFAPI_INT;
    return &rv;
}
void* cfapi_object_cast(int* type, ...)
{
    object* op;
    object* sp;
    int dir;
    char* str;
    object* caster;
    va_list args;
    static int rv;
    va_start(args, type);
    op = va_arg(args, object*);
    caster = va_arg(args, object*);
    dir = va_arg(args, int);
    sp = va_arg(args, object*);
    str = va_arg(args, char*);
    va_end(args);
    rv = cast_spell(op, caster, dir, sp, str);
    *type = CFAPI_INT;
    return &rv;
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
void* cfapi_object_check_spell(int* type, ...)
{
    object* op;
    char* spellname;
    va_list args;
    object* rv;

    va_start(args, type);
    op = va_arg(args, object*);
    spellname = va_arg(args, char*);
    va_end(args);
    rv = check_spell_known(op, spellname);
    *type = CFAPI_POBJECT;
    return rv;
}
void* cfapi_object_pay_amount(int* type, ...)
{
    object* op;
    uint64 amount;
    va_list args;
    static int rv;

    va_start(args, type);
    op = va_arg(args, object*);
    amount = va_arg(args, uint64);
    va_end(args);

    rv = pay_for_amount(amount, op);
    *type = CFAPI_INT;
    return &rv;
}
void* cfapi_object_pay_item(int* type, ...)
{
    object* op;
    object* tobuy;

    va_list args;
    static int rv;

    va_start(args, type);
    tobuy = va_arg(args, object*);
    op = va_arg(args, object*);
    va_end(args);

    rv = pay_for_item(tobuy, op);
    *type = CFAPI_INT;
    return &rv;
}
void* cfapi_object_transfer(int* type, ...)
{
    object* op;
    object* originator;
    int x, y, randompos, ttype;
    va_list args;
    static int rv=0;
    mapstruct* map;

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
        va_end(args);

        rv = transfer_ob(op, x, y, randompos, originator);
        *type = CFAPI_INT;
        return &rv;
        break;

    case 1:
        x = va_arg(args, int);
        y = va_arg(args, int);
        map = va_arg(args, mapstruct*);
        va_end(args);
        if (x < 0 || y < 0) {
            x = map->enter_x;
            y = map->enter_y;
        }
/*
        originator = get_object();
        EXIT_PATH(originator) = add_string(map->path);
        EXIT_X(originator) = x;
        EXIT_Y(originator) = y;
        printf("B Transfer: X=%d, Y=%d, OP=%s\n", x, y, op->name);*/
        /*enter_exit(op, originator);*/
        insert_ob_in_map_at(op, map, NULL, 0, x, y);
        /*printf("A Transfer: X=%d, Y=%d, MAP=%s\n", x, y, op->map->name);
        free_object(originator);
*/
        *type = CFAPI_INT;
        return &rv;
        break;

    default:
        *type = CFAPI_NONE;
        return NULL;
        break;
    }
}

void* cfapi_object_find_archetype_inside(int* type, ...)
{
    object* op;
    int     critera;
    char*   str;
    va_list args;
    object* rv;

    *type = CFAPI_POBJECT;
    va_start(args, type);
    op = va_arg(args, object*);
    critera = va_arg(args, int);

    switch(critera)
    {
    case 0: /* By name, either exact or from query_name */
        str = va_arg(args, char*);
        rv = present_arch_in_ob(find_archetype(str), op);
        if (rv == NULL) {
            object* tmp;
            char name[MAX_BUF];
            /* Search by query_name instead */
            for (tmp = op->inv; tmp; tmp = tmp->below) {
                query_name(tmp, name, MAX_BUF);
                if (!strncmp(name, str, strlen(str)))
                    rv = tmp;
                if (!strncmp(tmp->name, str, strlen(str)))
                    rv = tmp;
                if (rv != NULL)
                    break;
            }
        }
        break;

    default:
        rv = NULL;
        break;
    }
    va_end(args);

    if (rv == NULL) {
        *type = CFAPI_NONE;
    }
    return rv;
}

void* cfapi_object_drop(int* type, ...)
{
    object* op;
    object* author;
    va_list args;

    va_start(args, type);
    op = va_arg(args, object*);
    author = va_arg(args, object*);
    va_end(args);

    if (QUERY_FLAG(op, FLAG_NO_DROP))
        return NULL;
    drop(author, op);

    if (author->type == PLAYER) {
        author->contr->count = 0;
        author->contr->socket.update_look = 1;
    }

    *type = CFAPI_NONE;
    return NULL;
}

void* cfapi_object_take(int* type, ...)
{
    object *op;
    object *author;
    va_list args;

    va_start(args, type);
    op = va_arg(args, object*);
    author = va_arg(args, object*);
    va_end(args);
    pick_up(author, op);

    *type = CFAPI_NONE;
    return NULL;
}

void* cfapi_object_say(int* type, ...)
{
    static int rv;
    object* op;
    char* msg;
    va_list args;

    va_start(args, type);
    op = va_arg(args, object*);
    msg = va_arg(args, char*);
    va_end(args);
    
    if (op->type == PLAYER) {
        rv = command_say(op, msg);
    } else {
        npc_say(op, msg);
        rv = 0;
    }
    *type = CFAPI_INT;
    return &rv;
}
void* cfapi_object_speak(int* type, ...)
{
    object* op;
    char* msg;
    va_list args;
    static char buf[MAX_BUF];

    va_start(args, type);
    op = va_arg(args, object*);
    msg = va_arg(args, char*);
    va_end(args);

    if (!op || !msg)
        return NULL;
    sprintf(buf, "%s says: ", op->name);
    strncat(buf, msg, MAX_BUF-strlen(buf)-1);
    buf[MAX_BUF-1]=0;

    /* Maybe no always NPC? */
    ext_info_map(NDI_WHITE, op->map, MSG_TYPE_DIALOG, MSG_TYPE_DIALOG_NPC, buf, buf);

    communicate(op, msg);
    *type = CFAPI_NONE;
    return NULL;
}
/* PLAYER SUBCLASS */
void* cfapi_player_find(int* type, ...)
{
    va_list args;
    void* rv;
    char* sval;
    va_start(args, type);

    sval = va_arg(args, char*);
    va_end(args);

    rv = find_player(sval);

    *type = CFAPI_PPLAYER;
    return rv;
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
void *cfapi_player_send_inventory(int *type, ...)
{
    /* Currently a stub. Do we really need this anymore ? */
    *type = CFAPI_NONE;
    return NULL;
}

void *cfapi_player_can_pay(int *type, ...)
{
    va_list args;
    static int rv;
    object* pl;
    
    va_start(args, type);
    pl = va_arg(args, object*);
    rv = can_pay(pl);
    *type = CFAPI_INT;
    return &rv;
}

void* cfapi_object_teleport(int *type, ...)
{
    mapstruct* map;
    int x, y;
    object* who;
    static int result;
    va_list args;

    va_start(args, type);
    who = va_arg(args, object*);
    map = va_arg(args, mapstruct*);
    x = va_arg(args, int);
    y = va_arg(args, int);

    if (!out_of_map(map, x, y)) {
        int k;
        object *tmp;
        k = find_first_free_spot(who, map, x, y);
        if (k == -1) {
            result = 1;
            return &result;
        }

        send_removed_object(who);
        remove_ob(who);

        for (tmp = who; tmp != NULL; tmp = tmp->more)
            tmp->x = x+freearr_x[k]+(tmp->arch == NULL ? 0 : tmp->arch->clone.x),
            tmp->y = y+freearr_y[k]+(tmp->arch == NULL ? 0 : tmp->arch->clone.y);

        insert_ob_in_map(who, map, NULL, 0);
        if (who->type == PLAYER)
            map_newmap_cmd(who->contr);
        result = 0;
    }

    return &result;
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
void* cfapi_archetype_get_first(int* type, ...)
{
    va_list args;
    va_start(args, type);
    va_end(args);
    *type = CFAPI_PARCH;
    return first_archetype;
}

void* cfapi_archetype_get_property(int* type, ...)
{
    archetype* arch;
    int prop;
    va_list args;
    void* rv;

    va_start(args, type);
    arch = va_arg(args, archetype*);
    prop = va_arg(args, int);
    switch (prop) {
    case CFAPI_ARCH_PROP_NAME:
        *type = CFAPI_STRING;
        rv = (void*)arch->name;
        break;

    case CFAPI_ARCH_PROP_NEXT:
        *type = CFAPI_PARCH;
        rv = arch->next;
        break;

    case CFAPI_ARCH_PROP_HEAD:
        *type = CFAPI_PARCH;
        rv = arch->head;
        break;

    case CFAPI_ARCH_PROP_MORE:
        *type = CFAPI_PARCH;
        rv = arch->more;
        break;

    case CFAPI_ARCH_PROP_CLONE:
        *type = CFAPI_POBJECT;
        rv = &arch->clone;
        break;

    default:
        *type = CFAPI_NONE;
        rv = NULL;
        break;
    }
    va_end(args);
    return rv;
}

/* Party-related functions */
void* cfapi_party_get_property(int* type, ...)
{
    partylist* party;
    int prop;
    va_list args;
    void* rv;
    object* obarg;
    player* pl;

    va_start(args, type);
    party = va_arg(args, partylist*);
    prop = va_arg(args, int);
    switch (prop)
    {
    case CFAPI_PARTY_PROP_NAME:
        *type = CFAPI_STRING;
        rv = (void*)party->partyname;
        break;

    case CFAPI_PARTY_PROP_NEXT:
        *type = CFAPI_PPARTY;
        rv = (party ? party->next : get_firstparty());
        break;

    case CFAPI_PARTY_PROP_PASSWORD:
        *type = CFAPI_STRING;
        rv = (void*)party->passwd;
        break;

    case CFAPI_PARTY_PROP_PLAYER:
        *type = CFAPI_PPLAYER;
        obarg = va_arg(args, object*);
        pl = (obarg ? obarg->contr : first_player);
        rv = NULL;
        for (; pl != NULL; pl = pl->next)
            if (pl->ob->contr->party == party) {
                rv = (void*)pl;
                break;
            }
        break;

    default:
        *type = CFAPI_NONE;
        rv = NULL;
        break;
    }
    va_end(args);
    return rv;
}

/* Regions-related functions */
void* cfapi_region_get_property(int* type, ...)
{
    region* reg;
    int prop;
    va_list args;
    void* rv;

    va_start(args, type);
    reg = va_arg(args, region*);
    prop = va_arg(args, int);
    switch (prop) {
    case CFAPI_REGION_PROP_NAME:
        *type = CFAPI_STRING;
        rv = (void*)reg->name;
        break;

    case CFAPI_REGION_PROP_NEXT:
        *type = CFAPI_PREGION;
        rv = (reg?reg->next:first_region);
        break;

    case CFAPI_REGION_PROP_PARENT:
        *type = CFAPI_PREGION;
        rv = (void*)reg->parent;
        break;

    case CFAPI_REGION_PROP_LONGNAME:
        *type = CFAPI_STRING;
        rv = (void*)reg->longname;
        break;

    case CFAPI_REGION_PROP_MESSAGE:
        *type = CFAPI_STRING;
        rv = (void*)reg->msg;
        break;

    default:
        *type = CFAPI_NONE;
        rv = NULL;
        break;
    }
    va_end(args);
    return rv;
}

/*****************************************************************************/
/* NEW PLUGIN STUFF ENDS HERE                                                */
/*****************************************************************************/


/*****************************************************************************/
/* Tries to find if a given command is handled by a plugin.                  */
/* Note that find_plugin_command is called *before* the internal commands are*/
/* checked, meaning that you can "overwrite" them.                           */
/*****************************************************************************/
command_array_struct *find_plugin_command(char *cmd, object *op)
{
    int i;
    crossfire_plugin* cp;
    command_array_struct* rtn_cmd;

    if (plugins_list == NULL)
        return NULL;

    for (cp = plugins_list; cp != NULL; cp = cp->next) {
        rtn_cmd = cp->propfunc(&i, "command?", cmd);
        if (rtn_cmd)
            return rtn_cmd;
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

/*****************************************************************************/
/* Template for version 2.0 plugins.                                         */
/* Contact: yann.chachkoff@myrealbox.com                                     */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001-2005 by Chachkoff Yann (Feel free to deliver your complaints)     */
/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2008 the Crossfire development team                        */
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
/*****************************************************************************/

/**
 * @defgroup plugin_citybell
 * Plugin to control the temple bells in the main cities.
 * Ported out of python in an attempt to make it more efficient.
 *
 * @{
 */

/**
 * @file
 * This file is part of the @ref plugin_citybell "city bell plugin".
 * See this page for more information.
 */

#include <stdarg.h>
#include <string.h>

#include <cfcitybell.h>
#include <cfcitybell_proto.h>
#include <svnversion.h>

CF_PLUGIN char SvnRevPlugin[] = SVN_REV;
static int last_hr;

// Static functions

/**
 * Ring the temple bells for a player in Scorn.
 *
 * @param pl
 * The object of the player for whom the bells toll.
 */
static void ring_scorn(object *pl)
{
    const char *god_name = cf_object_get_sstring_property(pl, CFAPI_OBJECT_PROP_GOD);
    char buf[MAX_BUF];
    // TODO: We could be really clever and use the return value of strcmp to reduce comparisons.
    if (!strcmp(god_name, "Devourers") ||
        !strcmp(god_name, "Sorig") ||
        !strcmp(god_name, "Ruggilli") ||
        !strcmp(god_name, "Gaea") ||
        !strcmp(god_name, "Mostrai") ||
        !strcmp(god_name, "Lythander"))
    {
            snprintf(buf, MAX_BUF, "You hear the bell of the glorious temple of %s.", god_name);
    }
    else if (!strcmp(god_name, "Valriel") ||
        !strcmp(god_name, "Gorokh"))
    {
            snprintf(buf, MAX_BUF, "You hear the bell of the glorious church of %s.", god_name);
    }
    else
        strcpy(buf, "You hear the bells of the various temples of Scorn.");
    
    cf_player_message(pl, buf, NDI_UNIQUE|NDI_ORANGE);
}

/**
 * Ring the bell in Darcap
 *
 * @param pl
 * The object of the player hearing the bell.
 */
static void ring_darcap(object *pl)
{
    const char *god_name = cf_object_get_sstring_property(pl, CFAPI_OBJECT_PROP_GOD);
    
    if (!strcmp(god_name, "Devourers"))
    {
        cf_player_message(pl, "You hear the glorious bell of St. Andreas.", NDI_UNIQUE|NDI_ORANGE);
    }
    else
    {
        cf_player_message(pl, "You hear the bell of St. Andreas.", NDI_UNIQUE|NDI_ORANGE);
    }
}

/**
 * Ring the bells in Navar
 *
 * @param pl
 * Player object to hear the bells
 */
static void ring_navar(object *pl)
{
    const char *god_name = cf_object_get_sstring_property(pl, CFAPI_OBJECT_PROP_GOD);
    
    if (!strcmp(god_name, "Gorokh") ||
        !strcmp(god_name, "Ruggilli") ||
        !strcmp(god_name, "Sorig") ||
        !strcmp(god_name, "Valriel"))
    {
            char buf[MAX_BUF];
            snprintf(buf, MAX_BUF, "You hear the bell of the glorious temple of %s.", god_name);
            cf_player_message(pl, buf, NDI_UNIQUE|NDI_ORANGE);
    }
    else if (!strcmp(god_name, "Mostrai"))
    {
            cf_player_message(pl, "You hear the bell of Mostrai's glorious cathedral.", NDI_UNIQUE|NDI_ORANGE);
    }
    else if (!strcmp(god_name, "Gaea"))
    {
            cf_player_message(pl, "You hear the bell of Gaea's glorious shrine.", NDI_UNIQUE|NDI_ORANGE);
    }
    else
    {
            cf_player_message(pl, "You hear the bells of the temples of Navar.", NDI_UNIQUE|NDI_ORANGE);
    }
}

/**
 * Ring the city bells for each player.
 */
static void ring_bell(void)
{
    object *pl;
    region *reg;
    const char *reg_name;
    // Looks like we just build this as we go from the python plugin.
    // So we can do each one as we encounter it instead of building an array.
    pl = cf_object_get_object_property(NULL, CFAPI_PLAYER_PROP_NEXT);
    while (pl)
    {
        // If the player is on a map, then try to ring the bell
        if (pl->map)
        {
            reg = cf_map_get_region_property(pl->map, CFAPI_MAP_PROP_REGION);
            if (reg)
            {
                reg_name = cf_region_get_name(reg);
                if (!strcmp(reg_name, "scorn"))
                {
                    ring_scorn(pl);
                }
                else if (!strcmp(reg_name, "darcap"))
                {
                    ring_darcap(pl);
                }
                else if (!strcmp(reg_name, "navar"))
                {
                    ring_navar(pl);
                }
            }
        }
        // Get the next player
        pl = cf_object_get_object_property(pl, CFAPI_PLAYER_PROP_NEXT);
    }
}

/**
 * Global server event handling. Only uses EVENT_CLOCK.
 * @param type
 * The event type.
 * @return
 * 0.
 */
CF_PLUGIN int cfcitybell_globalEventListener(int *type, ...) {
    va_list args;
    int code;
    timeofday_t tod;
    
    va_start(args, type);
    code = va_arg(args, int);
    
    switch (code)
    {
        case EVENT_CLOCK:
            cf_get_time(&tod);
            if (tod.hour != last_hr)
            {
                last_hr = tod.hour;
                ring_bell();
            }
            break;
    }
    
    va_end(args);
    
    return 0;
}

/**
 * Unused.
 * @param type
 * unused.
 * @return
 * 0.
 */
CF_PLUGIN int eventListener(int *type, ...) {
    return 0;
}

/**
 * Plugin initialization.
 * @param iversion
 * server version.
 * @param gethooksptr
 * function to get hooks.
 * @return
 * 0.
 */
CF_PLUGIN int initPlugin(const char *iversion, f_plug_api gethooksptr) {
    cf_init_plugin(gethooksptr);

    cf_log(llevDebug, PLUGIN_VERSION " init\n");

    return 0;
}

/**
 * Get the plugin identification or full name.
 * @param type
 * unused.
 * @return
 * NULL.
 */
CF_PLUGIN void *getPluginProperty(int *type, ...) {
    va_list args;
    const char *propname;
    int size;
    char *buf;

    va_start(args, type);
    propname = va_arg(args, const char *);

    if (!strcmp(propname, "Identification")) {
        buf = va_arg(args, char *);
        size = va_arg(args, int);
        va_end(args);
        snprintf(buf, size, PLUGIN_NAME);
        return NULL;
    } else if (!strcmp(propname, "FullName")) {
        buf = va_arg(args, char *);
        size = va_arg(args, int);
        va_end(args);
        snprintf(buf, size, PLUGIN_VERSION);
        return NULL;
    }
    va_end(args);
    return NULL;
}

/**
 * Unused.
 * @param op
 * unused.
 * @param params
 * unused.
 * @return
 * -1.
 */
CF_PLUGIN int cfcitybell_runPluginCommand(object *op, char *params) {
    return -1;
}


/**
 * Plugin initialisation.
 * Also sets the initial last_hr
 * @return
 * 0.
 */
CF_PLUGIN int postInitPlugin(void) {
    timeofday_t tod;
    
    // Initialize last_hr
    cf_get_time(&tod);
    last_hr = tod.hour;
    
    cf_log(llevDebug, PLUGIN_VERSION " post init\n");

    cf_system_register_global_event(EVENT_CLOCK, PLUGIN_NAME, cfcitybell_globalEventListener);
    
    
    
    return 0;
}

/**
 * Unloading of plugin.
 * @return
 * 0.
 */
CF_PLUGIN int closePlugin(void) {
    cf_log(llevDebug, PLUGIN_VERSION " closing\n");
    return 0;
}

/*@}*/

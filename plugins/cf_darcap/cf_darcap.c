/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000-2009 Crossfire Development Team                       */
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
/*****************************************************************************/

/* First let's include the header file needed                                */

#include "cf_darcap.h"
#include <stdarg.h>
#include <plugin_common.h>

/**
@todo
- use tables instead of chairs, so player can come to counter
- more drinks
- keep how many times player bought something, to have a 'barman rating'
- link to other characters based on barman rating
*/

#define BS_IDLE     1
#define BS_NEW_PLAYER   2
#define BS_GOING_PLAYER 3
#define BS_WAITING_ORDER    4
#define BS_GETTING_ORDER    5
#define BS_BRINGING_ORDER   6
static int barmanState;

static mapstruct *tavernMap;
static object *barmanObject;
#define MAX_PLAYERS 5
static object *checkPlayers[MAX_PLAYERS];
static int playerState[MAX_PLAYERS];
static object *barmanTarget;
static int barmanTargetX, barmanTargetY, wantedDrink;

static const char* const drinkNames[] = {
    "wine",
    NULL };

static const char* const drinkArch[] = {
    "wine",
    NULL };

static const int const drinkPrices[] = {
    18,
    0 };

static int checkPlayersTable() {
    int i, x, y;
    object *test;

    if (barmanObject == NULL)
        return 0;

    for (i = 0; i < MAX_PLAYERS; i++) {
        if (checkPlayers[i] == NULL)
            continue;

        if (tavernMap != cf_object_get_map_property(checkPlayers[i], CFAPI_OBJECT_PROP_MAP)) {
            checkPlayers[i] = NULL;
            continue;
        }
        /* already served? */
        if (playerState[i] == 1) {
            continue;
        }

        /* cf_log(llevDebug, "checking player %s\n", checkPlayers[i]->name); */

        x = cf_object_get_int_property(checkPlayers[i], CFAPI_OBJECT_PROP_X);
        y = cf_object_get_int_property(checkPlayers[i], CFAPI_OBJECT_PROP_Y);

        /* find a chair */
        test = cf_map_get_object_at(tavernMap, x, y);
        while (test && strcmp(test->name, "chair")) {
            test = cf_object_get_object_property(test, CFAPI_OBJECT_PROP_OB_ABOVE);
        }

        if (test == NULL)
            continue;

        /* found a player, let's get to her */
        barmanTargetX = x;
        barmanTargetY = y;
        barmanTarget = checkPlayers[i];
        barmanState = BS_GOING_PLAYER;
        cf_log(llevDebug, "barman going to %d, %d (%s)\n", x, y, checkPlayers[i]->name);
        cf_object_say(barmanObject, "Coming to take your order.");
        return 1;
    }

    /* idle movement */
    return 0;
}

static int playerMoved() {
    int x, y;

    if (barmanTarget == NULL)
        return 0;

    if (tavernMap != cf_object_get_map_property(barmanTarget, CFAPI_OBJECT_PROP_MAP)) {
        return 1;
    }

    x = cf_object_get_int_property(barmanTarget, CFAPI_OBJECT_PROP_X);
    y = cf_object_get_int_property(barmanTarget, CFAPI_OBJECT_PROP_Y);

    if (x != barmanTargetX || y != barmanTargetY)
        return 1;

    return 0;
}

static int moveToPlayer() {
    int x, y, p;

    if (barmanObject == NULL)
        return 0;

    x = cf_object_get_int_property(barmanObject, CFAPI_OBJECT_PROP_X);
    y = cf_object_get_int_property(barmanObject, CFAPI_OBJECT_PROP_Y);

    if ((abs(x - barmanTargetX) <= 1) && (abs(y - barmanTargetY) <= 1)) {
        if (barmanState == BS_BRINGING_ORDER) {
            int p;
            object *drink;

            for (p = 0; p < MAX_PLAYERS; p++) {
                if (checkPlayers[p] == barmanTarget) {
                    playerState[p] = 1;
                }
            }

            drink = cf_create_object_by_name(drinkArch[wantedDrink]);
            if (!drink) {
                cf_log(llevError, "can't find archetype %s for drink %s\n", drinkNames[wantedDrink], drinkArch[wantedDrink]);
                return;
            }
            /** @todo insert on table instead of chair */
            cf_map_insert_object(tavernMap, drink, barmanTargetX, barmanTargetY);
            if (playerMoved()) {
                cf_object_say(barmanObject, "That's probably a waste, but well...");
            } else
                cf_object_say(barmanObject, "Here you go.");
            barmanState = BS_IDLE;
        } else if (barmanState == BS_GOING_PLAYER) {
            int drink = 0;
            char buf[MAX_BUF], price[MAX_BUF];

            cf_object_say(barmanObject, "What may I serve you?");
            cf_object_say(barmanObject, "I can offer you:");

            while (drinkNames[drink]) {
                cf_cost_string_from_value(drinkPrices[drink], price, MAX_BUF);
                snprintf(buf, MAX_BUF, "%s for %s", drinkNames[drink], price);
                cf_object_say(barmanObject, buf);
                drink++;
            }

            barmanState = BS_WAITING_ORDER;
        }
        return 1;
    }

    /* check if player is still at the same place */
    if (barmanState == BS_GOING_PLAYER && playerMoved()) {
        cf_object_say(barmanObject, "Humf, well, if you want something...");
        barmanState = BS_IDLE;
        return 0;
    }

    /** @todo check return to cancel move/state, though that shouldn't happen */
    cf_object_move_to(barmanObject, barmanTargetX, barmanTargetY);

    return 1;
}

static int moveToBar() {
    int move;

    if (barmanObject == NULL)
        return 0;

    move = cf_object_move_to(barmanObject, 5, 9);

    if (move == 0)
        barmanState = BS_BRINGING_ORDER;

    return 1;
}

static int checkOrder(const char* message) {
    int drink = 0;

    if (barmanObject == NULL)
        return 0;

    while (drinkNames[drink]) {
        if (strcmp(drinkNames[drink], message) == 0) {
            if (!cf_object_pay_amount(barmanTarget, drinkPrices[drink])) {
                cf_object_say(barmanObject, "Sorry, you don't have enough money.");
                return 1;
            }

            cf_object_say(barmanObject, "Ok, please wait a minute.");
            barmanState = BS_GETTING_ORDER;
            wantedDrink = drink;
            return 1;
        }
        drink++;
    }

    cf_object_say(barmanObject, "Sorry, I don't have that in stock.");
    return 1;
}

static int handleBarman(object *activator, char *message, int event_code) {
    if (event_code == EVENT_DEATH) {
        barmanObject = NULL;
        return 0;
    }

    switch (barmanState) {
        case BS_IDLE:
            if (event_code == EVENT_TIME)
                return checkPlayersTable();

        case BS_GOING_PLAYER:
            if (event_code == EVENT_TIME)
                return moveToPlayer();

        case BS_WAITING_ORDER:
            if (event_code == EVENT_SAY) {
                return checkOrder(message);
            }

            if (playerMoved()) {
                barmanState = BS_IDLE;
            }
            /** @todo timeout */
            return 1;

        case BS_GETTING_ORDER:
            return moveToBar();

        case BS_BRINGING_ORDER:
            return moveToPlayer();
    }

    return 0;
}

static void addBarmanHook(const char* archName) {
    object *hook = cf_create_object_by_name(archName);

    if (hook == NULL) {
        cf_log(llevDebug, "can't find %s arch!", archName);
        return;
    }

    hook->title = cf_add_string(PLUGIN_NAME);
    hook->slaying = cf_add_string("dummy");
    cf_object_insert_object(hook, barmanObject);
    cf_log(llevDebug, "hooked (%s) barman\n", archName);
}

static void processTavern(mapstruct *map) {
    object *test = cf_map_get_object_at(map, 5, 10);

    tavernMap = map;
    while (test && strcmp(test->name, "Cameron")) {
        test = cf_object_get_object_property(test, CFAPI_OBJECT_PROP_OB_ABOVE);
    }

    if (test == NULL) {
        cf_log(llevDebug, "can't find barman on map!");
        tavernMap = NULL;
        return;
    }

    cf_log(llevDebug, "found barman\n");

    barmanObject = test;
    barmanState = BS_IDLE;
    addBarmanHook("event_say");
    addBarmanHook("event_time");
    addBarmanHook("event_death");
}

static void barmanGreet(object *who) {
    int i;

    if (barmanObject == NULL)
        return;

    for (i = 0; i < MAX_PLAYERS; i++) {
        if (checkPlayers[i] == NULL) {
            cf_object_say(barmanObject, "Hello there. Please take a seat and I'll come to take your order.");
            checkPlayers[i] = who;
            playerState[i] = 0;
            break;
        }
    }
    if (i == MAX_PLAYERS) {
        cf_object_say(barmanObject, "Hands full!");
    }
}

static void playerLeaves(object *who) {
    int i;

    if (barmanObject == NULL)
        return;

    for (i = 0; i < MAX_PLAYERS; i++) {
        if (checkPlayers[i] == who) {
            checkPlayers[i] == NULL;
            playerState[i] = 0;
            if (who == barmanTarget) {
                barmanTarget = NULL;
                if (barmanState == BS_WAITING_ORDER || barmanState == BS_GETTING_ORDER || barmanState == BS_BRINGING_ORDER)
                    /** @todo say something */
                    barmanState = BS_IDLE;
            }
            break;
        }
    }
}

static void handleDeath(object *victim, object *killer) {
    mapstruct *map = cf_object_get_map_property(victim, CFAPI_OBJECT_PROP_MAP);
    sstring mappath;
    int h, w, i, j;
    object *npc, *say;

    mappath = cf_map_get_sstring_property(map, CFAPI_MAP_PROP_PATH);

    if (strcmp(mappath, "/darcap/darcap/church"))
        return;

    /* make all monsters aggressive */
    h = cf_map_get_height(map);
    w = cf_map_get_width(map);
    say = NULL;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            npc = cf_map_get_object_at(map, j, i);
            while (npc) {
                if (cf_object_get_flag(npc, FLAG_MONSTER) && cf_object_get_flag(npc, FLAG_UNAGGRESSIVE)) {
                    say = npc;
                    cf_object_set_flag(npc, FLAG_UNAGGRESSIVE, 0);
                }

                npc = cf_object_get_object_property(npc, CFAPI_OBJECT_PROP_OB_ABOVE);
            }
        }
    }

    /* And warn player of bad action */
    if (say != NULL) {
        cf_object_say(say, "You are going to pay!");
    }
}

CF_PLUGIN int initPlugin(const char *iversion, f_plug_api gethooksptr) {
    int i;

    cf_init_plugin(gethooksptr);
    printf("darcap");

    cf_log(llevDebug, PLUGIN_VERSION " init\n");

    tavernMap = NULL;
    barmanObject = NULL;
    barmanState = BS_IDLE;

    for (i = 0; i < MAX_PLAYERS; i++) {
        checkPlayers[i] = NULL;
        playerState[i] = 0;
    }

    return 0;
}

CF_PLUGIN void *getPluginProperty(int *type, ...) {
    va_list args;
    const char *propname;
    int i, size;
    command_array_struct *rtn_cmd;
    char *buf;

    va_start(args, type);
    propname = va_arg(args, const char *);

    if (!strcmp(propname, "command?")) {
        const char *cmdname;
        cmdname = va_arg(args, const char *);
        rtn_cmd = va_arg(args, command_array_struct *);
        va_end(args);

        /** Check if plugin handles custom command */
        return NULL;
    } else if (!strcmp(propname, "Identification")) {
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

CF_PLUGIN int runPluginCommand(object *op, char *params) {
    return -1;
}

CF_PLUGIN void *globalEventListener(int *type, ...) {
    va_list args;
    static int rv = 0;
    char *buf, message[MAX_BUF];
    player *pl;
    object *op, *who, *activator, *third, *event;
    int event_code, return_value;

    va_start(args, type);
    event_code = va_arg(args, int);

    message[0] = 0;

    who = NULL;
    activator = NULL;
    third = NULL;
    event = NULL;
    rv = return_value = 0;
    switch (event_code) {
        case EVENT_MAPLOAD:
        {
            mapstruct *map = va_arg(args, mapstruct*);
            if (strcmp(map->path, "/darcap/darcap/tavern") == 0) {
                processTavern(map);
                break;
            }
            cf_log(llevDebug, map->path);
        }

        case EVENT_MAPUNLOAD:
        {
            mapstruct *map = va_arg(args, mapstruct*);
            if (map == tavernMap) {
                barmanObject = NULL;
                tavernMap = NULL;
            }
        }

        case EVENT_MAPENTER:
        {
            object* who = va_arg(args, object *);
            mapstruct* map = va_arg(args, mapstruct *);
            if (map == tavernMap)
                barmanGreet(who);
            break;
        }

        case EVENT_MAPLEAVE:
        {
            object *who = va_arg(args, object *);
            mapstruct *map = va_arg(args, mapstruct *);
            if (map == tavernMap)
                playerLeaves(who);
        }

        case EVENT_GKILL:
        {
            object *victim = va_arg(args, object *);
            object *killer = va_arg(args, object *);
            handleDeath(victim, killer);
        }

/*
    case EVENT_CRASH:
        printf("Unimplemented for now\n");
        break;

    case EVENT_BORN:
        context->activator = va_arg(args, object *);
        break;

    case EVENT_PLAYER_DEATH:
        context->who = va_arg(args, object *);
        break;

    case EVENT_GKILL:
        context->who = va_arg(args, object *);
        context->activator = va_arg(args, object *);
        break;

    case EVENT_LOGIN:
        pl = va_arg(args, player *);
        context->activator = pl->ob;
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        break;

    case EVENT_LOGOUT:
        pl = va_arg(args, player *);
        context->activator = pl->ob;
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        break;

    case EVENT_REMOVE:
        context->activator = va_arg(args, object *);
        break;

    case EVENT_SHOUT:
        context->activator = va_arg(args, object *);
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        break;

    case EVENT_MUZZLE:
        context->activator = va_arg(args, object *);
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        break;

    case EVENT_KICK:
        context->activator = va_arg(args, object *);
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        break;

    case EVENT_MAPENTER:
        context->activator = va_arg(args, object *);
        break;

    case EVENT_MAPLEAVE:
        context->activator = va_arg(args, object *);
        break;

    case EVENT_CLOCK:
        break;

    case EVENT_MAPRESET:
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        break;

    case EVENT_TELL:
        context->activator = va_arg(args, object *);
        buf = va_arg(args, char *);
        if (buf != 0)
            strcpy(context->message, buf);
        context->third = va_arg(args, object *);
        break;*/
    }
    va_end(args);
    return_value = 0;

    return &rv;
}

CF_PLUGIN void *eventListener(int *type, ...) {
    static int rv = 0;
    va_list args;
    char *buf, message[MAX_BUF];
    object *who, *activator, *third, *event;
    int fix, event_code;

    message[0] = 0;

    va_start(args, type);

    who = va_arg(args, object *);
    activator = va_arg(args, object *);
    third = va_arg(args, object *);
    buf = va_arg(args, char *);
    if (buf != 0)
        strcpy(message, buf);
    fix = va_arg(args, int);
    event = va_arg(args, object *);
    event_code = event->subtype;
    va_end(args);

    if (who == barmanObject)
        rv = handleBarman(activator, message, event_code);

    return &rv;
}

CF_PLUGIN int postInitPlugin(void) {
    cf_log(llevDebug, PLUGIN_VERSION" post init\n");

    cf_system_register_global_event(EVENT_MAPLOAD, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_MAPUNLOAD, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_MAPENTER, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_GKILL, PLUGIN_NAME, globalEventListener);

    /* Pick the global events you want to monitor from this plugin */
/*
    cf_system_register_global_event(EVENT_BORN, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_CLOCK, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_CRASH, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_PLAYER_DEATH, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_LOGIN, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_LOGOUT, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_MAPLEAVE, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_REMOVE, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_SHOUT, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_TELL, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_MUZZLE, PLUGIN_NAME, globalEventListener);
    cf_system_register_global_event(EVENT_KICK, PLUGIN_NAME, globalEventListener);
*/
    return 0;
}

CF_PLUGIN int closePlugin(void) {
    cf_log(llevDebug, PLUGIN_VERSION " closing\n");
    return 0;
}

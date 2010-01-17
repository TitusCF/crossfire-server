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

static int handleSelling(object *what, object *bywho, object *event) {
    mapstruct *map = cf_object_get_map_property(bywho, CFAPI_OBJECT_PROP_MAP);
    sstring mappath = cf_map_get_sstring_property(map, CFAPI_MAP_PROP_PATH);
    sstring slaying = cf_object_get_sstring_property(event, CFAPI_OBJECT_PROP_SLAYING);
    object *guard, *obj;
    int count, x, y, i, j, h, w = 0;

    if (strncmp(mappath, "/darcap/darcap", strlen("/darcap/darcap")))
        return 0;
    if (strcmp(slaying, "darcap_church_chalice"))
        return 0;

    /*
     now for some punishment to the player who is selling those holy relics! :)
     put some guards around the shop mats
     */
    h = cf_map_get_height(map);
    w = cf_map_get_width(map);
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            obj = cf_map_get_object_at(map, j, i);
            while (obj) {
                if (cf_object_get_int_property(obj, CFAPI_OBJECT_PROP_TYPE) == SHOP_MAT) {
                    count = random() % 10 + 2;
                    while (count >= 0) {
                        guard = cf_create_object_by_name("guard");
                        if (!guard) {
                            return;
                        }
                        cf_object_set_flag(guard, FLAG_UNAGGRESSIVE, 0);
                        cf_object_set_flag(guard, FLAG_STAND_STILL, 0);
                        guard = cf_map_insert_object_around(map, guard, j, i);
                        count--;
                    }
                    /* no more for this spot obviously */
                    break;
                }
                obj = cf_object_get_object_property(obj, CFAPI_OBJECT_PROP_OB_ABOVE);
            }
        }
    }

    if (guard != NULL)
        cf_object_say(guard, "You thief!");

    return 0;
}

typedef enum {
    ds_get_potion = 1,
    ds_get_roots = 2,
    ds_bring_roots = 3,
    ds_bring_potion = 4
} darcapSpikeState;
static sstring darcapSpike = NULL;
static sstring darcapSpikeTitle = NULL;
static sstring darcapSpikeDescription = NULL;
static sstring darcapSpikeGetPotion = NULL;
static sstring darcapSpikeGetRoots = NULL;
static sstring darcapSpikeBringRoots = NULL;
static sstring darcapSpikeBringPotion = NULL;

static void fixMessageFromInventory(object *npc, const char *itemname) {
    object *inv = cf_object_get_object_property(npc, CFAPI_OBJECT_PROP_INVENTORY);
    sstring name;

    while (inv) {
        name = cf_object_get_sstring_property(inv, CFAPI_OBJECT_PROP_RAW_NAME);
        if (strcmp(name, itemname) == 0) {
            cf_object_set_string_property(npc, CFAPI_OBJECT_PROP_MESSAGE, inv->msg);
            return;
        }

        inv = cf_object_get_object_property(inv, CFAPI_OBJECT_PROP_OB_BELOW);
    }

    cf_log(llevDebug, "fixMessageFromInventory: couldn't find %s for %s\n", itemname, npc);
}

static int handleSpike(object *npc, object *bywho, object *event, const char *message) {
    int state = cf_quest_get_player_state(bywho, darcapSpike);

    if (state == -1) {
        /* ended */
        fixMessageFromInventory(npc, "dlg_ended");
        return 0;
    }

    if (state == 0) {
        fixMessageFromInventory(npc, "dlg_start");

        if (strcmp(message, "quest_accept") == 0) {
            cf_quest_start(bywho, darcapSpike, darcapSpikeTitle, darcapSpikeDescription, ds_get_potion, darcapSpikeGetPotion);
        }
        return 0;
    }
    if (state == ds_get_potion) {
        fixMessageFromInventory(npc, "dlg_progress");

        if (strcmp(message, "yes")) {
            /* check, give reward, or blame */
        }
        return 0;
    }

    return 0;
}

static object *giveItem(object *to, const char *archname, const char *name, const char *namepl, int face) {
    object *item = cf_create_object_by_name(archname), *drop;
    if (!item) {
        cf_log(llevError, "cf_darcap/giveItem: can't create %s!\n", archname);
        /* will crash later on anyway... */
        return NULL;
    }

    drop = cf_create_object_by_name("event_drop");
    cf_object_set_string_property(drop, CFAPI_OBJECT_PROP_TITLE, "cf_darcap");
    cf_object_set_string_property(drop, CFAPI_OBJECT_PROP_SLAYING, "darcap/Spike");
    cf_object_insert_object(drop, item);

    cf_object_set_flag(item, FLAG_STARTEQUIP, 1);
    cf_object_set_string_property(item, CFAPI_OBJECT_PROP_NAME, name);
    cf_object_set_string_property(item, CFAPI_OBJECT_PROP_NAME_PLURAL, namepl);
    if (face != 0)
        cf_object_set_int_property(item, CFAPI_OBJECT_PROP_FACE, face);
    return cf_object_insert_object(item, to);
}

static int handleMolthir(object *npc, object *bywho, object *event, const char *message) {
    int state = cf_quest_get_player_state(bywho, darcapSpike);

    if (state == ds_get_potion) {
        fixMessageFromInventory(npc, "dlg_first");

        if (strcmp(message, "sigh_ok") == 0) {
            cf_quest_set_player_state(bywho, darcapSpike, ds_get_roots, darcapSpikeGetRoots);
        }
        return 0;
    }

    if (state == ds_get_roots) {
        fixMessageFromInventory(npc, "dlg_wait_roots");
        return 0;
    }

    fixMessageFromInventory(npc, "dlg_standard");

    return 0;
}

static int handleBob(object *npc, object *bywho, object *event, const char *message) {
    int state = cf_quest_get_player_state(bywho, darcapSpike);

    if (state == ds_get_roots) {
        /** @todo handle lost case? */
        fixMessageFromInventory(npc, "dlg_roots");

        if (strcmp(message, "roots") == 0) {
            sstring playername = cf_object_get_sstring_property(bywho, CFAPI_OBJECT_PROP_RAW_NAME);
            object *roots = giveItem(bywho, "blackroot", "smaprh root", "smaprh roots", 0);
            /* no cheating */
            cf_object_set_key(roots, darcapSpike, playername, 1);
            cf_quest_set_player_state(bywho, darcapSpike, ds_bring_roots, darcapSpikeBringRoots);
        }
        return 0;
    }

    fixMessageFromInventory(npc, "dlg_standard");
    return 0;
}

static int handleSay(object *npc, object *bywho, object *event, const char *message) {
    sstring slaying = cf_object_get_sstring_property(event, CFAPI_OBJECT_PROP_SLAYING);
    if (strcmp(slaying, "darcap/Spike") == 0) {
        return handleSpike(npc, bywho, event, message);
    }
    if (strcmp(slaying, "darcap/Molthir") == 0) {
        return handleMolthir(npc, bywho, event, message);
    }
    if (strcmp(slaying, "darcap/Bob") == 0) {
        return handleBob(npc, bywho, event, message);
    }
    return 0;
}

static int handleDrop(object *who, object *activator, object *event) {
    sstring slaying = cf_object_get_sstring_property(event, CFAPI_OBJECT_PROP_SLAYING);
    if (strcmp(slaying, darcapSpike) == 0) {
        /* item shouldn't be dropped, but check state to enable to clean inventory in case of corruption */
        sstring name = cf_object_get_sstring_property(who, CFAPI_OBJECT_PROP_RAW_NAME);
        int state = cf_quest_get_player_state(activator, darcapSpike);

        if ((state == ds_bring_roots && strcmp(name, "smaprh root") == 0) || (state == ds_bring_potion && strcmp(name, "potion for the back") == 0)) {
            cf_player_message(activator, "Better not lose that, it's important!", 0);
            return 1;
        }
    }
    return 0;
}

static int handleGiveMolthir(object *to, object *by, object *item) {
    int state = cf_quest_get_player_state(by, darcapSpike);
    const char *key;
    sstring playername, name;
    int face;

    if (state != ds_bring_roots) {
        cf_object_say(to, "And what am I supposed to do with that?");
        return 0;
    }

    /* check the roots are the real ones */
    name = cf_object_get_sstring_property(item, CFAPI_OBJECT_PROP_RAW_NAME);
    key = cf_object_get_key(item, darcapSpike);
    if (!key || strcmp(name, "smaprh root")) {
        cf_object_say(to, "Sorry, those are not the roots I'm looking for...");
        return 0;
    }
    playername = cf_object_get_sstring_property(by, CFAPI_OBJECT_PROP_RAW_NAME);
    if (strcmp(key, playername) != 0) {
        cf_object_say(to, "Sorry, but those roots look weird, I'd better not use them.");
        return 0;
    }

    /* all good, remove roots, give potion, set state */
    cf_object_say(to, "Ha yes, those are the roots I need! Here, this is the potion Spike will need.");
    cf_object_remove(item);
    cf_quest_set_player_state(by, darcapSpike, ds_bring_potion, darcapSpikeBringPotion);
    /* create lead because it's inert, can't be applied, and such */
    /* and change face to that of a potion for consistency */
    face = cf_find_face("potioncha.111", 0);
    item = giveItem(by, "lead", "Molthir's potion for the back", "Molthir's potions for the back", face);
    /* no cheating either */
    cf_object_set_key(item, darcapSpike, playername, 1);

    return 0;
}

static int handleGiveSpike(object *to, object *by, object *item) {
    int state = cf_quest_get_player_state(by, darcapSpike);
    const char *key;
    sstring playername, name;

    if (state == 0) {
        cf_object_say(to, "And what am I supposed to do with that?");
        return 0;
    }

    name = cf_object_get_sstring_property(item, CFAPI_OBJECT_PROP_RAW_NAME);
    key = cf_object_get_key(item, darcapSpike);
    if (!key || strcmp(name, "Molthir's potion for the back")) {
        cf_object_say(to, "Ha, this is not Molthir's potion...");
        return 0;
    }
    playername = cf_object_get_sstring_property(by, CFAPI_OBJECT_PROP_RAW_NAME);
    if (strcmp(key, playername) != 0) {
        cf_object_say(to, "Sorry, but this potion has a weird color, I'd rather not use it.");
        return 0;
    }

    /* ok, this is the real one. */
    cf_object_say(to, "Ha yes, this is Molthir's potion, many thanks!");
    cf_object_remove(item);

    /** @todo timer to reward? */
    item = cf_create_object_by_name("platinacoin");
    cf_object_set_int_property(item, CFAPI_OBJECT_PROP_NROF, 5);
    cf_object_say(to, "Here is some reward for your good deeds.");
    cf_object_insert_object(item, by);

    cf_quest_end(by, darcapSpike);

    return 0;
}

static int handleGive(object *to, object *by, object *item, object *event) {
    sstring slaying = cf_object_get_sstring_property(event, CFAPI_OBJECT_PROP_SLAYING);

    if (strcmp(slaying, "darcap/Spike") == 0) {
        return handleGiveSpike(to, by, item);
    }

    if (strcmp(slaying, "darcap/Molthir") == 0) {
        return handleGiveMolthir(to, by, item);
    }
}

CF_PLUGIN int initPlugin(const char *iversion, f_plug_api gethooksptr) {
    int i;

    cf_init_plugin(gethooksptr);

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
    if (event->subtype == EVENT_SELLING)
        rv = handleSelling(who, activator, event);
    if (event->subtype == EVENT_SAY)
        rv = handleSay(who, activator, event, buf);
    if (event->subtype == EVENT_DROP)
        rv = handleDrop(who, activator, event);
    if (event->subtype == EVENT_USER && strcmp(message, "give") == 0)
        rv = handleGive(who, activator, third, event);

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

    darcapSpike = cf_add_string("darcap/Spike");
    darcapSpikeTitle = cf_add_string("Spike's aching back");
    darcapSpikeDescription = cf_add_string("Spike, the weapon shop owner, has backaches, and needs a potion to endure his pains.");
    darcapSpikeGetPotion = cf_add_string("You need to get a potion from the potion shop.");
    darcapSpikeGetRoots = cf_add_string("You need to get some smaprh roots from Bob's shop in the south of Darcap.");
    darcapSpikeBringRoots = cf_add_string("Bring back the smaprh roots to Molthir.");
    darcapSpikeBringPotion = cf_add_string("Bring back the potion to Spike.");

    return 0;
}

CF_PLUGIN int closePlugin(void) {
    cf_log(llevDebug, PLUGIN_VERSION " closing\n");
    return 0;
}
